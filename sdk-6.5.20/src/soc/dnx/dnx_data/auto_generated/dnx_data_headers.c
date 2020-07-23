

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BIERDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_headers.h>



extern shr_error_e jr2_a0_data_headers_attach(
    int unit);
extern shr_error_e jr2_b0_data_headers_attach(
    int unit);
extern shr_error_e j2c_a0_data_headers_attach(
    int unit);
extern shr_error_e q2a_a0_data_headers_attach(
    int unit);
extern shr_error_e j2p_a0_data_headers_attach(
    int unit);



static shr_error_e
dnx_data_headers_ftmh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ftmh";
    submodule_data->doc = "ftmh header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_ftmh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers ftmh features");

    
    submodule_data->nof_defines = _dnx_data_headers_ftmh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers ftmh defines");

    submodule_data->defines[dnx_data_headers_ftmh_define_packet_size_offset].name = "packet_size_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_packet_size_offset].doc = "packet size offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_packet_size_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_traffic_class_offset].name = "traffic_class_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_traffic_class_offset].doc = "traffic class offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_traffic_class_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset].name = "src_sys_port_aggregate_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset].doc = "source system port aggregate offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_pp_dsp_offset].name = "pp_dsp_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_pp_dsp_offset].doc = "PP DSP offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_pp_dsp_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_drop_precedence_offset].name = "drop_precedence_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_drop_precedence_offset].doc = "Drop Precedence offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_drop_precedence_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_type_offset].name = "tm_action_type_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_type_offset].doc = "tm action type offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_is_mc_offset].name = "tm_action_is_mc_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_is_mc_offset].doc = "tm action is multicast offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_is_mc_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_outlif_offset].name = "outlif_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_outlif_offset].doc = "multicast_id or mc_rep_idx or outlif offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_outlif_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_cni_offset].name = "cni_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_cni_offset].doc = "CNI offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_cni_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_ecn_enable_offset].name = "ecn_enable_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_ecn_enable_offset].doc = "ECN enable offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_ecn_enable_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_profile_offset].name = "tm_profile_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_profile_offset].doc = "tm profile offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_profile_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_visibility_offset].name = "visibility_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_visibility_offset].doc = "visibility offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_visibility_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_type_offset].name = "internal_header_type_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_type_offset].doc = "internal header type offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tsh_en_offset].name = "tsh_en_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_tsh_en_offset].doc = "TSH enable offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tsh_en_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_en_offset].name = "internal_header_en_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_en_offset].doc = "internal header enable offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_en_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dest_ext_present_offset].name = "tm_dest_ext_present_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dest_ext_present_offset].doc = "tm destination extension present offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dest_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_ase_present_offset].name = "ase_present_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_ase_present_offset].doc = "application specific extension present offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_ase_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_ext_present_offset].name = "flow_id_ext_present_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_ext_present_offset].doc = "flow iD extension present offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset].name = "bier_bfr_ext_present_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset].doc = "bier bfr extension present offset";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_stack_route_history_bmp_offset].name = "stack_route_history_bmp_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_stack_route_history_bmp_offset].doc = "stacking route history bitmap offset in stacking header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_stack_route_history_bmp_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_offset].name = "tm_dst_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_offset].doc = "TM destination offset in TM destination extension header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_observation_info_offset].name = "observation_info_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_observation_info_offset].doc = "observation information offset in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_observation_info_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_destination_offset].name = "destination_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_destination_offset].doc = "destination offset in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_destination_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_src_sysport_offset].name = "src_sysport_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sysport_offset].doc = "source system port offset in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sysport_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_trajectory_trace_type_offset].name = "trajectory_trace_type_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_trajectory_trace_type_offset].doc = "trajectory trace type offset in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_trajectory_trace_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_port_offset].name = "port_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_port_offset].doc = "Port field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_port_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_direction_offset].name = "direction_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_direction_offset].doc = "Direction field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_direction_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_session_id_offset].name = "session_id_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_session_id_offset].doc = "Session_ID field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_session_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_truncated_offset].name = "truncated_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_truncated_offset].doc = "Truncated flag field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_truncated_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_en_offset].name = "en_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_en_offset].doc = "The trunk encapsulation type field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_en_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_cos_offset].name = "cos_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_cos_offset].doc = "Class of service field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_cos_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_vlan_offset].name = "vlan_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_vlan_offset].doc = "Vlan field offset in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_vlan_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_erspan_type_offset].name = "erspan_type_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_erspan_type_offset].doc = "ERSPAN type offset in ERSPAN/sFLow header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_erspan_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_offset].name = "flow_id_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_offset].doc = "Flow id offset in Flow-ID header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_profile_offset].name = "flow_profile_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_profile_offset].doc = "Flow profile offset in Flow-ID header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_profile_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_int_profile_offset].name = "int_profile_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_int_profile_offset].doc = "int profile offset in Inband Network Telemetry header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_int_profile_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_int_type_offset].name = "int_type_offset";
    submodule_data->defines[dnx_data_headers_ftmh_define_int_type_offset].doc = "INT type offset in Inband Network Telemetry header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_int_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_packet_size_bits].name = "packet_size_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_packet_size_bits].doc = "packet size bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_packet_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_traffic_class_bits].name = "traffic_class_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_traffic_class_bits].doc = "traffic class bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_traffic_class_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits].name = "src_sys_port_aggregate_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits].doc = "source system port aggregate bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_pp_dsp_bits].name = "pp_dsp_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_pp_dsp_bits].doc = "PP DSP bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_pp_dsp_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_drop_precedence_bits].name = "drop_precedence_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_drop_precedence_bits].doc = "Drop Precedence bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_drop_precedence_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_type_bits].name = "tm_action_type_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_type_bits].doc = "tm action type bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_is_mc_bits].name = "tm_action_is_mc_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_is_mc_bits].doc = "tm action is multicast bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_action_is_mc_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_outlif_bits].name = "outlif_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_outlif_bits].doc = "multicast_id or mc_rep_idx or outlif bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_outlif_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_cni_bits].name = "cni_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_cni_bits].doc = "CNI bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_cni_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_ecn_enable_bits].name = "ecn_enable_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_ecn_enable_bits].doc = "ECN enable bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_ecn_enable_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_profile_bits].name = "tm_profile_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_profile_bits].doc = "tm profile bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_profile_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_visibility_bits].name = "visibility_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_visibility_bits].doc = "visibility bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_visibility_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tsh_en_bits].name = "tsh_en_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_tsh_en_bits].doc = "TSH enable bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tsh_en_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_en_bits].name = "internal_header_en_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_en_bits].doc = "internal header enable bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_internal_header_en_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dest_ext_present_bits].name = "tm_dest_ext_present_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dest_ext_present_bits].doc = "tm destination extension present bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dest_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_ase_present_bits].name = "ase_present_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_ase_present_bits].doc = "application specific extension present bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_ase_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_ext_present_bits].name = "flow_id_ext_present_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_ext_present_bits].doc = "flow iD extension present bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits].name = "bier_bfr_ext_present_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits].doc = "bier bfr extension present bits in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_bits].name = "tm_dst_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_bits].doc = "TM destination bits in TM destination extension header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_observation_info_bits].name = "observation_info_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_observation_info_bits].doc = "observation information bits in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_observation_info_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_destination_bits].name = "destination_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_destination_bits].doc = "destination bits in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_destination_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_src_sysport_bits].name = "src_sysport_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sysport_bits].doc = "source system port bits in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_src_sysport_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_trajectory_trace_type_bits].name = "trajectory_trace_type_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_trajectory_trace_type_bits].doc = "trajectory trace type bits in ase header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_trajectory_trace_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_port_bits].name = "port_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_port_bits].doc = "Port field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_port_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_direction_bits].name = "direction_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_direction_bits].doc = "Direction field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_direction_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_session_id_bits].name = "session_id_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_session_id_bits].doc = "Session_ID field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_session_id_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_truncated_bits].name = "truncated_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_truncated_bits].doc = "Truncated flag field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_truncated_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_en_bits].name = "en_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_en_bits].doc = "The trunk encapsulation type field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_en_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_cos_bits].name = "cos_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_cos_bits].doc = "Class of service field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_cos_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_vlan_bits].name = "vlan_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_vlan_bits].doc = "Vlan field bits in ERSPAN header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_vlan_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_erspan_type_bits].name = "erspan_type_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_erspan_type_bits].doc = "ERSPAN type bits in ERSPAN/sFlow header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_erspan_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_bits].name = "flow_id_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_bits].doc = "Flow id bits in Flow-ID header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_profile_bits].name = "flow_profile_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_profile_bits].doc = "Flow profile bits in Flow-ID header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_profile_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_base_header_size].name = "base_header_size";
    submodule_data->defines[dnx_data_headers_ftmh_define_base_header_size].doc = "FTMH BASE size in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_base_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_ase_header_size].name = "ase_header_size";
    submodule_data->defines[dnx_data_headers_ftmh_define_ase_header_size].doc = "FTMH Ase header extension size in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_ase_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_size].name = "tm_dst_size";
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_size].doc = "FTMH TM destination extension header size in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_tm_dst_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_header_size].name = "flow_id_header_size";
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_header_size].doc = "FTMH Flow-ID extension header size in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_flow_id_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_int_profile_bits].name = "int_profile_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_int_profile_bits].doc = "int profile bits in Inband Network Telemetry header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_int_profile_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_int_type_bits].name = "int_type_bits";
    submodule_data->defines[dnx_data_headers_ftmh_define_int_type_bits].doc = "INT type bits in Inband Network Telemetry header";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_int_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_define_add_dsp_ext].name = "add_dsp_ext";
    submodule_data->defines[dnx_data_headers_ftmh_define_add_dsp_ext].doc = "adding dsp extension for the ftmh";
    
    submodule_data->defines[dnx_data_headers_ftmh_define_add_dsp_ext].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_headers_ftmh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers ftmh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_ftmh_feature_get(
    int unit,
    dnx_data_headers_ftmh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, feature);
}


uint32
dnx_data_headers_ftmh_packet_size_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_packet_size_offset);
}

uint32
dnx_data_headers_ftmh_traffic_class_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_traffic_class_offset);
}

uint32
dnx_data_headers_ftmh_src_sys_port_aggregate_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset);
}

uint32
dnx_data_headers_ftmh_pp_dsp_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_pp_dsp_offset);
}

uint32
dnx_data_headers_ftmh_drop_precedence_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_drop_precedence_offset);
}

uint32
dnx_data_headers_ftmh_tm_action_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_action_type_offset);
}

uint32
dnx_data_headers_ftmh_tm_action_is_mc_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_action_is_mc_offset);
}

uint32
dnx_data_headers_ftmh_outlif_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_outlif_offset);
}

uint32
dnx_data_headers_ftmh_cni_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_cni_offset);
}

uint32
dnx_data_headers_ftmh_ecn_enable_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_ecn_enable_offset);
}

uint32
dnx_data_headers_ftmh_tm_profile_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_profile_offset);
}

uint32
dnx_data_headers_ftmh_visibility_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_visibility_offset);
}

uint32
dnx_data_headers_ftmh_internal_header_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_internal_header_type_offset);
}

uint32
dnx_data_headers_ftmh_tsh_en_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tsh_en_offset);
}

uint32
dnx_data_headers_ftmh_internal_header_en_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_internal_header_en_offset);
}

uint32
dnx_data_headers_ftmh_tm_dest_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_dest_ext_present_offset);
}

uint32
dnx_data_headers_ftmh_ase_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_ase_present_offset);
}

uint32
dnx_data_headers_ftmh_flow_id_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_id_ext_present_offset);
}

uint32
dnx_data_headers_ftmh_bier_bfr_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset);
}

uint32
dnx_data_headers_ftmh_stack_route_history_bmp_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_stack_route_history_bmp_offset);
}

uint32
dnx_data_headers_ftmh_tm_dst_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_dst_offset);
}

uint32
dnx_data_headers_ftmh_observation_info_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_observation_info_offset);
}

uint32
dnx_data_headers_ftmh_destination_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_destination_offset);
}

uint32
dnx_data_headers_ftmh_src_sysport_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_src_sysport_offset);
}

uint32
dnx_data_headers_ftmh_trajectory_trace_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_trajectory_trace_type_offset);
}

uint32
dnx_data_headers_ftmh_port_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_port_offset);
}

uint32
dnx_data_headers_ftmh_direction_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_direction_offset);
}

uint32
dnx_data_headers_ftmh_session_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_session_id_offset);
}

uint32
dnx_data_headers_ftmh_truncated_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_truncated_offset);
}

uint32
dnx_data_headers_ftmh_en_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_en_offset);
}

uint32
dnx_data_headers_ftmh_cos_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_cos_offset);
}

uint32
dnx_data_headers_ftmh_vlan_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_vlan_offset);
}

uint32
dnx_data_headers_ftmh_erspan_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_erspan_type_offset);
}

uint32
dnx_data_headers_ftmh_flow_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_id_offset);
}

uint32
dnx_data_headers_ftmh_flow_profile_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_profile_offset);
}

uint32
dnx_data_headers_ftmh_int_profile_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_int_profile_offset);
}

uint32
dnx_data_headers_ftmh_int_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_int_type_offset);
}

uint32
dnx_data_headers_ftmh_packet_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_packet_size_bits);
}

uint32
dnx_data_headers_ftmh_traffic_class_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_traffic_class_bits);
}

uint32
dnx_data_headers_ftmh_src_sys_port_aggregate_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits);
}

uint32
dnx_data_headers_ftmh_pp_dsp_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_pp_dsp_bits);
}

uint32
dnx_data_headers_ftmh_drop_precedence_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_drop_precedence_bits);
}

uint32
dnx_data_headers_ftmh_tm_action_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_action_type_bits);
}

uint32
dnx_data_headers_ftmh_tm_action_is_mc_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_action_is_mc_bits);
}

uint32
dnx_data_headers_ftmh_outlif_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_outlif_bits);
}

uint32
dnx_data_headers_ftmh_cni_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_cni_bits);
}

uint32
dnx_data_headers_ftmh_ecn_enable_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_ecn_enable_bits);
}

uint32
dnx_data_headers_ftmh_tm_profile_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_profile_bits);
}

uint32
dnx_data_headers_ftmh_visibility_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_visibility_bits);
}

uint32
dnx_data_headers_ftmh_tsh_en_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tsh_en_bits);
}

uint32
dnx_data_headers_ftmh_internal_header_en_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_internal_header_en_bits);
}

uint32
dnx_data_headers_ftmh_tm_dest_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_dest_ext_present_bits);
}

uint32
dnx_data_headers_ftmh_ase_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_ase_present_bits);
}

uint32
dnx_data_headers_ftmh_flow_id_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_id_ext_present_bits);
}

uint32
dnx_data_headers_ftmh_bier_bfr_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits);
}

uint32
dnx_data_headers_ftmh_tm_dst_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_dst_bits);
}

uint32
dnx_data_headers_ftmh_observation_info_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_observation_info_bits);
}

uint32
dnx_data_headers_ftmh_destination_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_destination_bits);
}

uint32
dnx_data_headers_ftmh_src_sysport_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_src_sysport_bits);
}

uint32
dnx_data_headers_ftmh_trajectory_trace_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_trajectory_trace_type_bits);
}

uint32
dnx_data_headers_ftmh_port_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_port_bits);
}

uint32
dnx_data_headers_ftmh_direction_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_direction_bits);
}

uint32
dnx_data_headers_ftmh_session_id_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_session_id_bits);
}

uint32
dnx_data_headers_ftmh_truncated_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_truncated_bits);
}

uint32
dnx_data_headers_ftmh_en_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_en_bits);
}

uint32
dnx_data_headers_ftmh_cos_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_cos_bits);
}

uint32
dnx_data_headers_ftmh_vlan_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_vlan_bits);
}

uint32
dnx_data_headers_ftmh_erspan_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_erspan_type_bits);
}

uint32
dnx_data_headers_ftmh_flow_id_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_id_bits);
}

uint32
dnx_data_headers_ftmh_flow_profile_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_profile_bits);
}

uint32
dnx_data_headers_ftmh_base_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_base_header_size);
}

uint32
dnx_data_headers_ftmh_ase_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_ase_header_size);
}

uint32
dnx_data_headers_ftmh_tm_dst_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_tm_dst_size);
}

uint32
dnx_data_headers_ftmh_flow_id_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_flow_id_header_size);
}

uint32
dnx_data_headers_ftmh_int_profile_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_int_profile_bits);
}

uint32
dnx_data_headers_ftmh_int_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_int_type_bits);
}

uint32
dnx_data_headers_ftmh_add_dsp_ext_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh, dnx_data_headers_ftmh_define_add_dsp_ext);
}










static shr_error_e
dnx_data_headers_tsh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tsh";
    submodule_data->doc = "tsh header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_tsh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers tsh features");

    
    submodule_data->nof_defines = _dnx_data_headers_tsh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers tsh defines");

    submodule_data->defines[dnx_data_headers_tsh_define_base_header_size].name = "base_header_size";
    submodule_data->defines[dnx_data_headers_tsh_define_base_header_size].doc = "TSH header size";
    
    submodule_data->defines[dnx_data_headers_tsh_define_base_header_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_tsh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers tsh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_tsh_feature_get(
    int unit,
    dnx_data_headers_tsh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_tsh, feature);
}


uint32
dnx_data_headers_tsh_base_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_tsh, dnx_data_headers_tsh_define_base_header_size);
}










static shr_error_e
dnx_data_headers_otsh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "otsh";
    submodule_data->doc = "tsh header parameters in legacy device";
    
    submodule_data->nof_features = _dnx_data_headers_otsh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers otsh features");

    
    submodule_data->nof_defines = _dnx_data_headers_otsh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers otsh defines");

    submodule_data->defines[dnx_data_headers_otsh_define_base_header_size].name = "base_header_size";
    submodule_data->defines[dnx_data_headers_otsh_define_base_header_size].doc = "OTSH header size";
    
    submodule_data->defines[dnx_data_headers_otsh_define_base_header_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_otsh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers otsh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_otsh_feature_get(
    int unit,
    dnx_data_headers_otsh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh, feature);
}


uint32
dnx_data_headers_otsh_base_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh, dnx_data_headers_otsh_define_base_header_size);
}










static shr_error_e
dnx_data_headers_optimized_ftmh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "optimized_ftmh";
    submodule_data->doc = "optimized ftmh header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_optimized_ftmh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers optimized_ftmh features");

    submodule_data->features[dnx_data_headers_optimized_ftmh_ftmh_opt_parse].name = "ftmh_opt_parse";
    submodule_data->features[dnx_data_headers_optimized_ftmh_ftmh_opt_parse].doc = "Indicate if FTMH_OPT parsing is supported";
    submodule_data->features[dnx_data_headers_optimized_ftmh_ftmh_opt_parse].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_headers_optimized_ftmh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers optimized_ftmh defines");

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_base_header_size].name = "base_header_size";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_base_header_size].doc = "Optimized FTMH header size";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_base_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset].name = "packet_size_7_0_offset";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset].doc = "packet size offset";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset].name = "tm_action_is_mc_offset";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset].doc = "tm action is multicast offset";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_packet_size_8_offset].name = "packet_size_8_offset";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_packet_size_8_offset].doc = "packet size offset";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_packet_size_8_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_user_defined_offset].name = "user_defined_offset";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_user_defined_offset].doc = "packet size offset";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_user_defined_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_fap_id_offset].name = "fap_id_offset";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_fap_id_offset].doc = "fap id of multicast_id[18:8] offset";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_fap_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset].name = "pp_dsp_or_mc_id_offset";
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset].doc = "pp_dsp or multicast_id[7:0] offset";
    
    submodule_data->defines[dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_optimized_ftmh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers optimized_ftmh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_optimized_ftmh_feature_get(
    int unit,
    dnx_data_headers_optimized_ftmh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, feature);
}


uint32
dnx_data_headers_optimized_ftmh_base_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_base_header_size);
}

uint32
dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset);
}

uint32
dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset);
}

uint32
dnx_data_headers_optimized_ftmh_packet_size_8_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_packet_size_8_offset);
}

uint32
dnx_data_headers_optimized_ftmh_user_defined_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_user_defined_offset);
}

uint32
dnx_data_headers_optimized_ftmh_fap_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_fap_id_offset);
}

uint32
dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_optimized_ftmh, dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset);
}










static shr_error_e
dnx_data_headers_internal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "internal";
    submodule_data->doc = "internal header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_internal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers internal features");

    submodule_data->features[dnx_data_headers_internal_header_visibilty_mode_global].name = "header_visibilty_mode_global";
    submodule_data->features[dnx_data_headers_internal_header_visibilty_mode_global].doc = "Indicate if header signal visibility is global or regular visibility";
    submodule_data->features[dnx_data_headers_internal_header_visibilty_mode_global].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_headers_internal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers internal defines");

    submodule_data->defines[dnx_data_headers_internal_define_parsing_start_type_offset].name = "parsing_start_type_offset";
    submodule_data->defines[dnx_data_headers_internal_define_parsing_start_type_offset].doc = "Parsing start type offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_parsing_start_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_parsing_start_type_bits].name = "parsing_start_type_bits";
    submodule_data->defines[dnx_data_headers_internal_define_parsing_start_type_bits].doc = "Parsing start type bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_parsing_start_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_lif_ext_type_offset].name = "lif_ext_type_offset";
    submodule_data->defines[dnx_data_headers_internal_define_lif_ext_type_offset].doc = "Lif Extenson type offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_lif_ext_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_lif_ext_type_bits].name = "lif_ext_type_bits";
    submodule_data->defines[dnx_data_headers_internal_define_lif_ext_type_bits].doc = "Lif Extenson type bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_lif_ext_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fai_offset].name = "fai_offset";
    submodule_data->defines[dnx_data_headers_internal_define_fai_offset].doc = "FAI offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_fai_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fai_bits].name = "fai_bits";
    submodule_data->defines[dnx_data_headers_internal_define_fai_bits].doc = "FAI bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_fai_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_tail_edit_offset].name = "tail_edit_offset";
    submodule_data->defines[dnx_data_headers_internal_define_tail_edit_offset].doc = "tail edit profile offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_tail_edit_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_tail_edit_bits].name = "tail_edit_bits";
    submodule_data->defines[dnx_data_headers_internal_define_tail_edit_bits].doc = "tail edit profile bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_tail_edit_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_forward_domain_offset].name = "forward_domain_offset";
    submodule_data->defines[dnx_data_headers_internal_define_forward_domain_offset].doc = "Forward domain in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_forward_domain_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_forward_domain_bits].name = "forward_domain_bits";
    submodule_data->defines[dnx_data_headers_internal_define_forward_domain_bits].doc = "Forward domain bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_forward_domain_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_inlif_offset].name = "inlif_offset";
    submodule_data->defines[dnx_data_headers_internal_define_inlif_offset].doc = "In-LIF offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_inlif_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_inlif_bits].name = "inlif_bits";
    submodule_data->defines[dnx_data_headers_internal_define_inlif_bits].doc = "In-LIF bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_inlif_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fhei_size].name = "fhei_size";
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size].doc = "FHEI header size for internal header stamped";
    
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz0].name = "fhei_size_sz0";
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz0].doc = "FHEI header size of type SZ0";
    
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz0].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz1].name = "fhei_size_sz1";
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz1].doc = "FHEI header size of type SZ1";
    
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz2].name = "fhei_size_sz2";
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz2].doc = "FHEI header size of type SZ2";
    
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_sz2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_offset].name = "fhei_size_offset";
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_offset].doc = "FHEI size offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_bits].name = "fhei_size_bits";
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_bits].doc = "FHEI size bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_fhei_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_present_offset].name = "learning_ext_present_offset";
    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_present_offset].doc = "Learning Extension present offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_present_bits].name = "learning_ext_present_bits";
    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_present_bits].doc = "Learning Extension present bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_size].name = "learning_ext_size";
    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_size].doc = "Learning Extension header size";
    
    submodule_data->defines[dnx_data_headers_internal_define_learning_ext_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_ttl_offset].name = "ttl_offset";
    submodule_data->defines[dnx_data_headers_internal_define_ttl_offset].doc = "TTL offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_ttl_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_ttl_bits].name = "ttl_bits";
    submodule_data->defines[dnx_data_headers_internal_define_ttl_bits].doc = "TTL bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_ttl_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_fli_offset].name = "eth_rm_fli_offset";
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_fli_offset].doc = "fwd layer index offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_fli_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_fli_bits].name = "eth_rm_fli_bits";
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_fli_bits].doc = "fwd layer index bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_fli_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_pso_offset].name = "eth_rm_pso_offset";
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_pso_offset].doc = "parsing start offset offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_pso_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_pso_bits].name = "eth_rm_pso_bits";
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_pso_bits].doc = "parsing start offset bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_define_eth_rm_pso_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_define_cfg_fai_unknown_address_enable].name = "cfg_fai_unknown_address_enable";
    submodule_data->defines[dnx_data_headers_internal_define_cfg_fai_unknown_address_enable].doc = "If set, require to enable setting of unknown address to FAI";
    
    submodule_data->defines[dnx_data_headers_internal_define_cfg_fai_unknown_address_enable].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_internal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers internal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_internal_feature_get(
    int unit,
    dnx_data_headers_internal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, feature);
}


uint32
dnx_data_headers_internal_parsing_start_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_parsing_start_type_offset);
}

uint32
dnx_data_headers_internal_parsing_start_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_parsing_start_type_bits);
}

uint32
dnx_data_headers_internal_lif_ext_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_lif_ext_type_offset);
}

uint32
dnx_data_headers_internal_lif_ext_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_lif_ext_type_bits);
}

uint32
dnx_data_headers_internal_fai_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fai_offset);
}

uint32
dnx_data_headers_internal_fai_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fai_bits);
}

uint32
dnx_data_headers_internal_tail_edit_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_tail_edit_offset);
}

uint32
dnx_data_headers_internal_tail_edit_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_tail_edit_bits);
}

uint32
dnx_data_headers_internal_forward_domain_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_forward_domain_offset);
}

uint32
dnx_data_headers_internal_forward_domain_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_forward_domain_bits);
}

uint32
dnx_data_headers_internal_inlif_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_inlif_offset);
}

uint32
dnx_data_headers_internal_inlif_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_inlif_bits);
}

uint32
dnx_data_headers_internal_fhei_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fhei_size);
}

uint32
dnx_data_headers_internal_fhei_size_sz0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fhei_size_sz0);
}

uint32
dnx_data_headers_internal_fhei_size_sz1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fhei_size_sz1);
}

uint32
dnx_data_headers_internal_fhei_size_sz2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fhei_size_sz2);
}

uint32
dnx_data_headers_internal_fhei_size_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fhei_size_offset);
}

uint32
dnx_data_headers_internal_fhei_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_fhei_size_bits);
}

uint32
dnx_data_headers_internal_learning_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_learning_ext_present_offset);
}

uint32
dnx_data_headers_internal_learning_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_learning_ext_present_bits);
}

uint32
dnx_data_headers_internal_learning_ext_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_learning_ext_size);
}

uint32
dnx_data_headers_internal_ttl_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_ttl_offset);
}

uint32
dnx_data_headers_internal_ttl_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_ttl_bits);
}

uint32
dnx_data_headers_internal_eth_rm_fli_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_eth_rm_fli_offset);
}

uint32
dnx_data_headers_internal_eth_rm_fli_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_eth_rm_fli_bits);
}

uint32
dnx_data_headers_internal_eth_rm_pso_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_eth_rm_pso_offset);
}

uint32
dnx_data_headers_internal_eth_rm_pso_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_eth_rm_pso_bits);
}

uint32
dnx_data_headers_internal_cfg_fai_unknown_address_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal, dnx_data_headers_internal_define_cfg_fai_unknown_address_enable);
}










static shr_error_e
dnx_data_headers_internal_legacy_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "internal_legacy";
    submodule_data->doc = "internal header parameters  in legacy device";
    
    submodule_data->nof_features = _dnx_data_headers_internal_legacy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers internal_legacy features");

    
    submodule_data->nof_defines = _dnx_data_headers_internal_legacy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers internal_legacy defines");

    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size].name = "fhei_size";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size].doc = "FHEI header size for internal header stamped";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz0].name = "fhei_size_sz0";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz0].doc = "FHEI header size of type SZ0";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz0].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz1].name = "fhei_size_sz1";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz1].doc = "FHEI header size of type SZ1";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz2].name = "fhei_size_sz2";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz2].doc = "FHEI header size of type SZ2";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_sz2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_inlif_inrif_offset].name = "inlif_inrif_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_inlif_inrif_offset].doc = "In-LIF offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_inlif_inrif_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_inlif_inrif_bits].name = "inlif_inrif_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_inlif_inrif_bits].doc = "In-LIF bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_inlif_inrif_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_unknown_address_offset].name = "unknown_address_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_unknown_address_offset].doc = "Unknown address offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_unknown_address_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_unknown_address_bits].name = "unknown_address_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_unknown_address_bits].doc = "Unknown address bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_unknown_address_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset].name = "forwarding_header_offset_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset].doc = "Forwarding header offset in internal header, valid in jericho mode only";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits].name = "forwarding_header_offset_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits].doc = "Forwarding header offset in internal header, valid in jericho mode only";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_forward_code_offset].name = "forward_code_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forward_code_offset].doc = "Forward code offset in internal header, valid in jericho mode only";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forward_code_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_forward_code_bits].name = "forward_code_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forward_code_bits].doc = "Forward code present offset in internal header, valid in jericho mode only";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_forward_code_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_offset].name = "fhei_size_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_offset].doc = "FHEI size offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_bits].name = "fhei_size_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_bits].doc = "FHEI size bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_fhei_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_present_offset].name = "learning_ext_present_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_present_offset].doc = "Learning Extension present offset in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_present_bits].name = "learning_ext_present_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_present_bits].doc = "Learning Extension present bits in internal header";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_size].name = "learning_ext_size";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_size].doc = "Learning Extension header size";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_learning_ext_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_eei_ext_present_offset].name = "eei_ext_present_offset";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_eei_ext_present_offset].doc = "EEI Extension present offset in internal header, valid in jericho mode only";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_eei_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_internal_legacy_define_eei_ext_present_bits].name = "eei_ext_present_bits";
    submodule_data->defines[dnx_data_headers_internal_legacy_define_eei_ext_present_bits].doc = "EEI Extension present bits in internal header, valid in jericho mode only";
    
    submodule_data->defines[dnx_data_headers_internal_legacy_define_eei_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_internal_legacy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers internal_legacy tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_internal_legacy_feature_get(
    int unit,
    dnx_data_headers_internal_legacy_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, feature);
}


uint32
dnx_data_headers_internal_legacy_fhei_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_fhei_size);
}

uint32
dnx_data_headers_internal_legacy_fhei_size_sz0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_fhei_size_sz0);
}

uint32
dnx_data_headers_internal_legacy_fhei_size_sz1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_fhei_size_sz1);
}

uint32
dnx_data_headers_internal_legacy_fhei_size_sz2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_fhei_size_sz2);
}

uint32
dnx_data_headers_internal_legacy_inlif_inrif_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_inlif_inrif_offset);
}

uint32
dnx_data_headers_internal_legacy_inlif_inrif_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_inlif_inrif_bits);
}

uint32
dnx_data_headers_internal_legacy_unknown_address_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_unknown_address_offset);
}

uint32
dnx_data_headers_internal_legacy_unknown_address_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_unknown_address_bits);
}

uint32
dnx_data_headers_internal_legacy_forwarding_header_offset_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset);
}

uint32
dnx_data_headers_internal_legacy_forwarding_header_offset_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits);
}

uint32
dnx_data_headers_internal_legacy_forward_code_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_forward_code_offset);
}

uint32
dnx_data_headers_internal_legacy_forward_code_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_forward_code_bits);
}

uint32
dnx_data_headers_internal_legacy_fhei_size_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_fhei_size_offset);
}

uint32
dnx_data_headers_internal_legacy_fhei_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_fhei_size_bits);
}

uint32
dnx_data_headers_internal_legacy_learning_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_learning_ext_present_offset);
}

uint32
dnx_data_headers_internal_legacy_learning_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_learning_ext_present_bits);
}

uint32
dnx_data_headers_internal_legacy_learning_ext_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_learning_ext_size);
}

uint32
dnx_data_headers_internal_legacy_eei_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_eei_ext_present_offset);
}

uint32
dnx_data_headers_internal_legacy_eei_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_internal_legacy, dnx_data_headers_internal_legacy_define_eei_ext_present_bits);
}










static shr_error_e
dnx_data_headers_fhei_sz1_trap_sniff_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fhei_sz1_trap_sniff";
    submodule_data->doc = "fhei header parameters of fhei_size sz1, fhei_type trap or sniff";
    
    submodule_data->nof_features = _dnx_data_headers_fhei_sz1_trap_sniff_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers fhei_sz1_trap_sniff features");

    
    submodule_data->nof_defines = _dnx_data_headers_fhei_sz1_trap_sniff_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers fhei_sz1_trap_sniff defines");

    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset].name = "fhei_type_offset";
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset].doc = "FHEI Extension type offset";
    
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits].name = "fhei_type_bits";
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits].doc = "FHEI Extension type bits";
    
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset].name = "code_offset";
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset].doc = "FHEI Extension code offset";
    
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits].name = "code_bits";
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits].doc = "FHEI Extension code bits";
    
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset].name = "qualifier_offset";
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset].doc = "FHEI Extension qualifier offset";
    
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits].name = "qualifier_bits";
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits].doc = "FHEI Extension qualifier bits";
    
    submodule_data->defines[dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_fhei_sz1_trap_sniff_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers fhei_sz1_trap_sniff tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_fhei_sz1_trap_sniff_feature_get(
    int unit,
    dnx_data_headers_fhei_sz1_trap_sniff_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, feature);
}


uint32
dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset);
}

uint32
dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits);
}

uint32
dnx_data_headers_fhei_sz1_trap_sniff_code_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset);
}

uint32
dnx_data_headers_fhei_sz1_trap_sniff_code_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits);
}

uint32
dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset);
}

uint32
dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_sz1_trap_sniff, dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits);
}










static shr_error_e
dnx_data_headers_fhei_trap_sniff_legacy_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fhei_trap_sniff_legacy";
    submodule_data->doc = "ffhei header parameters of type trap or sniff in legacy devices";
    
    submodule_data->nof_features = _dnx_data_headers_fhei_trap_sniff_legacy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers fhei_trap_sniff_legacy features");

    
    submodule_data->nof_defines = _dnx_data_headers_fhei_trap_sniff_legacy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers fhei_trap_sniff_legacy defines");

    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset].name = "code_offset";
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset].doc = "FHEI Extension code offset";
    
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits].name = "code_bits";
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits].doc = "FHEI Extension code bits";
    
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset].name = "qualifier_offset";
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset].doc = "FHEI Extension qualifier offset";
    
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits].name = "qualifier_bits";
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits].doc = "FHEI Extension qualifier bits";
    
    submodule_data->defines[dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_fhei_trap_sniff_legacy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers fhei_trap_sniff_legacy tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_fhei_trap_sniff_legacy_feature_get(
    int unit,
    dnx_data_headers_fhei_trap_sniff_legacy_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_trap_sniff_legacy, feature);
}


uint32
dnx_data_headers_fhei_trap_sniff_legacy_code_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_trap_sniff_legacy, dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset);
}

uint32
dnx_data_headers_fhei_trap_sniff_legacy_code_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_trap_sniff_legacy, dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits);
}

uint32
dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_trap_sniff_legacy, dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset);
}

uint32
dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_fhei_trap_sniff_legacy, dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits);
}










static shr_error_e
dnx_data_headers_udh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "udh";
    submodule_data->doc = "udh header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_udh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers udh features");

    
    submodule_data->nof_defines = _dnx_data_headers_udh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers udh defines");

    submodule_data->defines[dnx_data_headers_udh_define_data_type0_offset].name = "data_type0_offset";
    submodule_data->defines[dnx_data_headers_udh_define_data_type0_offset].doc = "UDH data type0 offset in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type0_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type1_offset].name = "data_type1_offset";
    submodule_data->defines[dnx_data_headers_udh_define_data_type1_offset].doc = "UDH data type1 offset in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type1_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type2_offset].name = "data_type2_offset";
    submodule_data->defines[dnx_data_headers_udh_define_data_type2_offset].doc = "UDH data type2 offset in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type2_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type3_offset].name = "data_type3_offset";
    submodule_data->defines[dnx_data_headers_udh_define_data_type3_offset].doc = "UDH data type3 offset in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type3_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_offset].name = "data_offset";
    submodule_data->defines[dnx_data_headers_udh_define_data_offset].doc = "UDH data offset in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type0_bits].name = "data_type0_bits";
    submodule_data->defines[dnx_data_headers_udh_define_data_type0_bits].doc = "UDH data type0 bits in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type0_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type1_bits].name = "data_type1_bits";
    submodule_data->defines[dnx_data_headers_udh_define_data_type1_bits].doc = "UDH data type1 bits in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type1_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type2_bits].name = "data_type2_bits";
    submodule_data->defines[dnx_data_headers_udh_define_data_type2_bits].doc = "UDH data type2 bits in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type2_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_type3_bits].name = "data_type3_bits";
    submodule_data->defines[dnx_data_headers_udh_define_data_type3_bits].doc = "UDH data type3 bits in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_type3_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_data_bits].name = "data_bits";
    submodule_data->defines[dnx_data_headers_udh_define_data_bits].doc = "UDH data bits in UDH header";
    
    submodule_data->defines[dnx_data_headers_udh_define_data_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_udh_define_base_size].name = "base_size";
    submodule_data->defines[dnx_data_headers_udh_define_base_size].doc = "UDH base size";
    
    submodule_data->defines[dnx_data_headers_udh_define_base_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_udh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers udh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_udh_feature_get(
    int unit,
    dnx_data_headers_udh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, feature);
}


uint32
dnx_data_headers_udh_data_type0_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type0_offset);
}

uint32
dnx_data_headers_udh_data_type1_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type1_offset);
}

uint32
dnx_data_headers_udh_data_type2_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type2_offset);
}

uint32
dnx_data_headers_udh_data_type3_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type3_offset);
}

uint32
dnx_data_headers_udh_data_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_offset);
}

uint32
dnx_data_headers_udh_data_type0_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type0_bits);
}

uint32
dnx_data_headers_udh_data_type1_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type1_bits);
}

uint32
dnx_data_headers_udh_data_type2_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type2_bits);
}

uint32
dnx_data_headers_udh_data_type3_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_type3_bits);
}

uint32
dnx_data_headers_udh_data_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_data_bits);
}

uint32
dnx_data_headers_udh_base_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_udh, dnx_data_headers_udh_define_base_size);
}










static shr_error_e
dnx_data_headers_ftmh_legacy_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ftmh_legacy";
    submodule_data->doc = "ftmh header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_ftmh_legacy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers ftmh_legacy features");

    
    submodule_data->nof_defines = _dnx_data_headers_ftmh_legacy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers ftmh_legacy defines");

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_packet_size_offset].name = "packet_size_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_packet_size_offset].doc = "packet size offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_packet_size_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_traffic_class_offset].name = "traffic_class_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_traffic_class_offset].doc = "traffic class offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_traffic_class_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset].name = "src_sys_port_aggregate_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset].doc = "source system port aggregate offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_pp_dsp_offset].name = "pp_dsp_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_pp_dsp_offset].doc = "PP DSP offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_pp_dsp_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_drop_precedence_offset].name = "drop_precedence_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_drop_precedence_offset].doc = "Drop Precedence offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_drop_precedence_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_type_offset].name = "tm_action_type_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_type_offset].doc = "tm action type offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_otsh_en_offset].name = "otsh_en_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_otsh_en_offset].doc = "OTSH enable offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_otsh_en_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_internal_header_en_offset].name = "internal_header_en_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_internal_header_en_offset].doc = "internal header enable offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_internal_header_en_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset].name = "outbound_mirr_disable_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset].doc = "outbound-mirr-disable parameter offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset].name = "tm_action_is_mc_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset].doc = "tm action is multicast offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outlif_offset].name = "outlif_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outlif_offset].doc = "multicast_id or outlif offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outlif_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset].name = "dsp_ext_present_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset].doc = "tm destination extension present offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_cni_offset].name = "cni_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_cni_offset].doc = "CNI offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_cni_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_ecn_enable_offset].name = "ecn_enable_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_ecn_enable_offset].doc = "ECN enable offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_ecn_enable_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_exclude_source_offset].name = "exclude_source_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_exclude_source_offset].doc = "exclude soutrce offset in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_exclude_source_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset].name = "stack_route_history_bmp_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset].doc = "stacking route history bitmap offset for stacking header";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset].name = "dsp_ext_header_offset";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset].doc = "DSP offset for TM destination extension header";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_packet_size_bits].name = "packet_size_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_packet_size_bits].doc = "packet size bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_packet_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_traffic_class_bits].name = "traffic_class_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_traffic_class_bits].doc = "traffic class bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_traffic_class_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits].name = "src_sys_port_aggregate_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits].doc = "source system port aggregate bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_pp_dsp_bits].name = "pp_dsp_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_pp_dsp_bits].doc = "PP DSP bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_pp_dsp_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_drop_precedence_bits].name = "drop_precedence_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_drop_precedence_bits].doc = "Drop Precedence bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_drop_precedence_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_type_bits].name = "tm_action_type_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_type_bits].doc = "tm action type bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_otsh_en_bits].name = "otsh_en_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_otsh_en_bits].doc = "OTSH enable bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_otsh_en_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_internal_header_en_bits].name = "internal_header_en_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_internal_header_en_bits].doc = "internal header enable bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_internal_header_en_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits].name = "outbound_mirr_disable_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits].doc = "outbound-mirr-disable parameter bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits].name = "tm_action_is_mc_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits].doc = "tm action is multicast bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outlif_bits].name = "outlif_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outlif_bits].doc = "multicast_id or outlif bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_outlif_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits].name = "dsp_ext_present_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits].doc = "tm destination extension present bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_cni_bits].name = "cni_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_cni_bits].doc = "CNI bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_cni_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_ecn_enable_bits].name = "ecn_enable_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_ecn_enable_bits].doc = "ECN enable bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_ecn_enable_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_exclude_source_bits].name = "exclude_source_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_exclude_source_bits].doc = "exclude source bits in legacy FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_exclude_source_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits].name = "dsp_ext_header_bits";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits].doc = "DSP bits for TM destination extension header";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_base_header_size].name = "base_header_size";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_base_header_size].doc = "Legacy FTMH BASE size";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_base_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size].name = "dsp_ext_header_size";
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size].doc = "FTMH DSP extension header size in FTMH";
    
    submodule_data->defines[dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_ftmh_legacy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers ftmh_legacy tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_ftmh_legacy_feature_get(
    int unit,
    dnx_data_headers_ftmh_legacy_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, feature);
}


uint32
dnx_data_headers_ftmh_legacy_packet_size_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_packet_size_offset);
}

uint32
dnx_data_headers_ftmh_legacy_traffic_class_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_traffic_class_offset);
}

uint32
dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset);
}

uint32
dnx_data_headers_ftmh_legacy_pp_dsp_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_pp_dsp_offset);
}

uint32
dnx_data_headers_ftmh_legacy_drop_precedence_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_drop_precedence_offset);
}

uint32
dnx_data_headers_ftmh_legacy_tm_action_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_tm_action_type_offset);
}

uint32
dnx_data_headers_ftmh_legacy_otsh_en_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_otsh_en_offset);
}

uint32
dnx_data_headers_ftmh_legacy_internal_header_en_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_internal_header_en_offset);
}

uint32
dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset);
}

uint32
dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset);
}

uint32
dnx_data_headers_ftmh_legacy_outlif_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_outlif_offset);
}

uint32
dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset);
}

uint32
dnx_data_headers_ftmh_legacy_cni_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_cni_offset);
}

uint32
dnx_data_headers_ftmh_legacy_ecn_enable_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_ecn_enable_offset);
}

uint32
dnx_data_headers_ftmh_legacy_exclude_source_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_exclude_source_offset);
}

uint32
dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset);
}

uint32
dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset);
}

uint32
dnx_data_headers_ftmh_legacy_packet_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_packet_size_bits);
}

uint32
dnx_data_headers_ftmh_legacy_traffic_class_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_traffic_class_bits);
}

uint32
dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits);
}

uint32
dnx_data_headers_ftmh_legacy_pp_dsp_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_pp_dsp_bits);
}

uint32
dnx_data_headers_ftmh_legacy_drop_precedence_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_drop_precedence_bits);
}

uint32
dnx_data_headers_ftmh_legacy_tm_action_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_tm_action_type_bits);
}

uint32
dnx_data_headers_ftmh_legacy_otsh_en_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_otsh_en_bits);
}

uint32
dnx_data_headers_ftmh_legacy_internal_header_en_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_internal_header_en_bits);
}

uint32
dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits);
}

uint32
dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits);
}

uint32
dnx_data_headers_ftmh_legacy_outlif_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_outlif_bits);
}

uint32
dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits);
}

uint32
dnx_data_headers_ftmh_legacy_cni_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_cni_bits);
}

uint32
dnx_data_headers_ftmh_legacy_ecn_enable_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_ecn_enable_bits);
}

uint32
dnx_data_headers_ftmh_legacy_exclude_source_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_exclude_source_bits);
}

uint32
dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits);
}

uint32
dnx_data_headers_ftmh_legacy_base_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_base_header_size);
}

uint32
dnx_data_headers_ftmh_legacy_dsp_ext_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_ftmh_legacy, dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size);
}










static shr_error_e
dnx_data_headers_otsh_legacy_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "otsh_legacy";
    submodule_data->doc = "otsh header parameters";
    
    submodule_data->nof_features = _dnx_data_headers_otsh_legacy_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers otsh_legacy features");

    
    submodule_data->nof_defines = _dnx_data_headers_otsh_legacy_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers otsh_legacy defines");

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_type_offset].name = "type_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_type_offset].doc = "type offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_type_bits].name = "type_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_type_bits].doc = "type bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_sub_type_offset].name = "oam_sub_type_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_sub_type_offset].doc = "oam-sub-type offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_sub_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_sub_type_bits].name = "oam_sub_type_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_sub_type_bits].doc = "oam-sub-type bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_sub_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_mep_type_offset].name = "mep_type_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_mep_type_offset].doc = "mep-type offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_mep_type_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_mep_type_bits].name = "mep_type_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_mep_type_bits].doc = "mep-type bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_mep_type_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_tp_command_offset].name = "tp_command_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_tp_command_offset].doc = "tp-command offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_tp_command_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_tp_command_bits].name = "tp_command_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_tp_command_bits].doc = "tp-command  bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_tp_command_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset].name = "ts_encapsulation_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset].doc = "ts-encapsulation offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits].name = "ts_encapsulation_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits].doc = "ts-encapsulation bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset].name = "oam_ts_data_0_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset].doc = "oam-ts-data higher 2 bits offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits].name = "oam_ts_data_0_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits].doc = "oam-ts-data higher 2 bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset].name = "oam_ts_data_1_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset].doc = "oam-ts-data lower 32 bits offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits].name = "oam_ts_data_1_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits].doc = "oam-ts-data lower 32 bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_offset_offset].name = "offset_offset";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_offset_offset].doc = "offset offset in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_offset_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_otsh_legacy_define_offset_bits].name = "offset_bits";
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_offset_bits].doc = "offset bits in legacy OTSH";
    
    submodule_data->defines[dnx_data_headers_otsh_legacy_define_offset_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_otsh_legacy_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers otsh_legacy tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_otsh_legacy_feature_get(
    int unit,
    dnx_data_headers_otsh_legacy_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, feature);
}


uint32
dnx_data_headers_otsh_legacy_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_type_offset);
}

uint32
dnx_data_headers_otsh_legacy_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_type_bits);
}

uint32
dnx_data_headers_otsh_legacy_oam_sub_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_oam_sub_type_offset);
}

uint32
dnx_data_headers_otsh_legacy_oam_sub_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_oam_sub_type_bits);
}

uint32
dnx_data_headers_otsh_legacy_mep_type_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_mep_type_offset);
}

uint32
dnx_data_headers_otsh_legacy_mep_type_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_mep_type_bits);
}

uint32
dnx_data_headers_otsh_legacy_tp_command_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_tp_command_offset);
}

uint32
dnx_data_headers_otsh_legacy_tp_command_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_tp_command_bits);
}

uint32
dnx_data_headers_otsh_legacy_ts_encapsulation_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset);
}

uint32
dnx_data_headers_otsh_legacy_ts_encapsulation_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits);
}

uint32
dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset);
}

uint32
dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits);
}

uint32
dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset);
}

uint32
dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits);
}

uint32
dnx_data_headers_otsh_legacy_offset_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_offset_offset);
}

uint32
dnx_data_headers_otsh_legacy_offset_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otsh_legacy, dnx_data_headers_otsh_legacy_define_offset_bits);
}










static shr_error_e
dnx_data_headers_system_headers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "system_headers";
    submodule_data->doc = "system headers";
    
    submodule_data->nof_features = _dnx_data_headers_system_headers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers system_headers features");

    
    submodule_data->nof_defines = _dnx_data_headers_system_headers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers system_headers defines");

    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode_jericho].name = "system_headers_mode_jericho";
    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode_jericho].doc = "System headers mode Jericho";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode_jericho].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode_jericho2].name = "system_headers_mode_jericho2";
    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode_jericho2].doc = "Sysstem headers mode Jericho2";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode_jericho2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len].name = "crc_size_counted_in_pkt_len";
    submodule_data->defines[dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len].doc = "CRC size counted packet length";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode].name = "system_headers_mode";
    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode].doc = "System headers mode the device supports.";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_system_headers_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode].name = "jr_mode_ftmh_lb_key_ext_mode";
    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode].doc = "global FTMH load balance key extention mode";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode].name = "jr_mode_ftmh_stacking_ext_mode";
    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode].doc = "global FTMH Stacking extention mode";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_headers_system_headers_define_udh_base_size].name = "udh_base_size";
    submodule_data->defines[dnx_data_headers_system_headers_define_udh_base_size].doc = "udh base header size";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_udh_base_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift].name = "jr_mode_ftmh_oam_ts_right_shift";
    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift].doc = "Right shift for timestamp in OAM TS sytem header";
    
    submodule_data->defines[dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_headers_system_headers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers system_headers tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_system_headers_feature_get(
    int unit,
    dnx_data_headers_system_headers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, feature);
}


uint32
dnx_data_headers_system_headers_system_headers_mode_jericho_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_system_headers_mode_jericho);
}

uint32
dnx_data_headers_system_headers_system_headers_mode_jericho2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_system_headers_mode_jericho2);
}

uint32
dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len);
}

uint32
dnx_data_headers_system_headers_system_headers_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_system_headers_mode);
}

uint32
dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode);
}

uint32
dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode);
}

uint32
dnx_data_headers_system_headers_udh_base_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_udh_base_size);
}

uint32
dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_system_headers, dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift);
}










static shr_error_e
dnx_data_headers_otmh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "otmh";
    submodule_data->doc = "Outgoing TM header";
    
    submodule_data->nof_features = _dnx_data_headers_otmh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers otmh features");

    submodule_data->features[dnx_data_headers_otmh_otmh_enable].name = "otmh_enable";
    submodule_data->features[dnx_data_headers_otmh_otmh_enable].doc = "Enable/Disable OTMH. '1' means that otmh is supported, '0' means no support.";
    submodule_data->features[dnx_data_headers_otmh_otmh_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_headers_otmh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers otmh defines");

    
    submodule_data->nof_tables = _dnx_data_headers_otmh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers otmh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_otmh_feature_get(
    int unit,
    dnx_data_headers_otmh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_otmh, feature);
}











static shr_error_e
dnx_data_headers_pph_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pph";
    submodule_data->doc = "Packet proccessing header values";
    
    submodule_data->nof_features = _dnx_data_headers_pph_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers pph features");

    
    submodule_data->nof_defines = _dnx_data_headers_pph_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers pph defines");

    submodule_data->defines[dnx_data_headers_pph_define_pph_vsi_selection_size].name = "pph_vsi_selection_size";
    submodule_data->defines[dnx_data_headers_pph_define_pph_vsi_selection_size].doc = "Size of VSI selection field";
    
    submodule_data->defines[dnx_data_headers_pph_define_pph_vsi_selection_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_pph_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers pph tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_pph_feature_get(
    int unit,
    dnx_data_headers_pph_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_pph, feature);
}


uint32
dnx_data_headers_pph_pph_vsi_selection_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_pph, dnx_data_headers_pph_define_pph_vsi_selection_size);
}










static shr_error_e
dnx_data_headers_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General attributes";
    
    submodule_data->nof_features = _dnx_data_headers_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers general features");

    submodule_data->features[dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx].name = "fwd_header_offset_calc_with_egress_pars_idx";
    submodule_data->features[dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx].doc = "PPH Forwarding Header Offset calculation. '1' means calculation uses egress parsing index, '0' means not use egress parsing index.";
    submodule_data->features[dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_headers_general_jericho_mode_sys_header_is_supported].name = "jericho_mode_sys_header_is_supported";
    submodule_data->features[dnx_data_headers_general_jericho_mode_sys_header_is_supported].doc = "Jericho system header support. '1' means support, '0' means not support.";
    submodule_data->features[dnx_data_headers_general_jericho_mode_sys_header_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_headers_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers general defines");

    
    submodule_data->nof_tables = _dnx_data_headers_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_general_feature_get(
    int unit,
    dnx_data_headers_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_general, feature);
}











static shr_error_e
dnx_data_headers_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_headers_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data headers feature features");

    submodule_data->features[dnx_data_headers_feature_fhei_mpls_cmd_from_eei].name = "fhei_mpls_cmd_from_eei";
    submodule_data->features[dnx_data_headers_feature_fhei_mpls_cmd_from_eei].doc = "If set, means MPLS command in FHEI is from EEI[23:20], else configured globally";
    submodule_data->features[dnx_data_headers_feature_fhei_mpls_cmd_from_eei].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_headers_feature_unknown_address].name = "unknown_address";
    submodule_data->features[dnx_data_headers_feature_unknown_address].doc = "If set, FAI[0] indicates forwarding lookup result, else FAI[0] is not for unknown address";
    submodule_data->features[dnx_data_headers_feature_unknown_address].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_headers_feature_tsh_ext_with_flow_id].name = "tsh_ext_with_flow_id";
    submodule_data->features[dnx_data_headers_feature_tsh_ext_with_flow_id].doc = "If set, TSH_Ext is constructed by [flow-id-ext, tsh-ext] values";
    submodule_data->features[dnx_data_headers_feature_tsh_ext_with_flow_id].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_headers_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data headers feature defines");

    submodule_data->defines[dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size].name = "fhei_mpls_swap_remark_profile_size";
    submodule_data->defines[dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size].doc = "Size of remark_profile mapped from mpls swap command";
    
    submodule_data->defines[dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_headers_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data headers feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_headers_feature_feature_get(
    int unit,
    dnx_data_headers_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_feature, feature);
}


uint32
dnx_data_headers_feature_fhei_mpls_swap_remark_profile_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_headers, dnx_data_headers_submodule_feature, dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size);
}







shr_error_e
dnx_data_headers_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "headers";
    module_data->nof_submodules = _dnx_data_headers_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data headers submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_headers_ftmh_init(unit, &module_data->submodules[dnx_data_headers_submodule_ftmh]));
    SHR_IF_ERR_EXIT(dnx_data_headers_tsh_init(unit, &module_data->submodules[dnx_data_headers_submodule_tsh]));
    SHR_IF_ERR_EXIT(dnx_data_headers_otsh_init(unit, &module_data->submodules[dnx_data_headers_submodule_otsh]));
    SHR_IF_ERR_EXIT(dnx_data_headers_optimized_ftmh_init(unit, &module_data->submodules[dnx_data_headers_submodule_optimized_ftmh]));
    SHR_IF_ERR_EXIT(dnx_data_headers_internal_init(unit, &module_data->submodules[dnx_data_headers_submodule_internal]));
    SHR_IF_ERR_EXIT(dnx_data_headers_internal_legacy_init(unit, &module_data->submodules[dnx_data_headers_submodule_internal_legacy]));
    SHR_IF_ERR_EXIT(dnx_data_headers_fhei_sz1_trap_sniff_init(unit, &module_data->submodules[dnx_data_headers_submodule_fhei_sz1_trap_sniff]));
    SHR_IF_ERR_EXIT(dnx_data_headers_fhei_trap_sniff_legacy_init(unit, &module_data->submodules[dnx_data_headers_submodule_fhei_trap_sniff_legacy]));
    SHR_IF_ERR_EXIT(dnx_data_headers_udh_init(unit, &module_data->submodules[dnx_data_headers_submodule_udh]));
    SHR_IF_ERR_EXIT(dnx_data_headers_ftmh_legacy_init(unit, &module_data->submodules[dnx_data_headers_submodule_ftmh_legacy]));
    SHR_IF_ERR_EXIT(dnx_data_headers_otsh_legacy_init(unit, &module_data->submodules[dnx_data_headers_submodule_otsh_legacy]));
    SHR_IF_ERR_EXIT(dnx_data_headers_system_headers_init(unit, &module_data->submodules[dnx_data_headers_submodule_system_headers]));
    SHR_IF_ERR_EXIT(dnx_data_headers_otmh_init(unit, &module_data->submodules[dnx_data_headers_submodule_otmh]));
    SHR_IF_ERR_EXIT(dnx_data_headers_pph_init(unit, &module_data->submodules[dnx_data_headers_submodule_pph]));
    SHR_IF_ERR_EXIT(dnx_data_headers_general_init(unit, &module_data->submodules[dnx_data_headers_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_headers_feature_init(unit, &module_data->submodules[dnx_data_headers_submodule_feature]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_headers_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_headers_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_headers_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

