

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_HEADERS_H_

#define _DNX_DATA_HEADERS_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_headers.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_headers_init(
    int unit);







typedef enum
{

    
    _dnx_data_headers_ftmh_feature_nof
} dnx_data_headers_ftmh_feature_e;



typedef int(
    *dnx_data_headers_ftmh_feature_get_f) (
    int unit,
    dnx_data_headers_ftmh_feature_e feature);


typedef uint32(
    *dnx_data_headers_ftmh_packet_size_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_traffic_class_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_src_sys_port_aggregate_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_pp_dsp_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_drop_precedence_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_action_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_action_is_mc_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_outlif_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_cni_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_ecn_enable_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_profile_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_visibility_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_internal_header_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tsh_en_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_internal_header_en_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_dest_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_ase_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_id_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_bier_bfr_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_stack_route_history_bmp_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_dst_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_observation_info_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_destination_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_src_sysport_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_trajectory_trace_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_port_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_direction_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_session_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_truncated_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_en_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_cos_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_vlan_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_erspan_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_profile_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_int_profile_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_int_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_packet_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_traffic_class_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_src_sys_port_aggregate_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_pp_dsp_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_drop_precedence_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_action_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_action_is_mc_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_outlif_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_cni_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_ecn_enable_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_profile_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_visibility_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tsh_en_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_internal_header_en_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_dest_ext_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_ase_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_id_ext_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_bier_bfr_ext_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_dst_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_observation_info_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_destination_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_src_sysport_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_trajectory_trace_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_port_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_direction_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_session_id_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_truncated_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_en_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_cos_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_vlan_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_erspan_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_id_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_profile_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_base_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_ase_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_tm_dst_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_flow_id_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_int_profile_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_int_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_add_dsp_ext_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_ftmh_feature_get_f feature_get;
    
    dnx_data_headers_ftmh_packet_size_offset_get_f packet_size_offset_get;
    
    dnx_data_headers_ftmh_traffic_class_offset_get_f traffic_class_offset_get;
    
    dnx_data_headers_ftmh_src_sys_port_aggregate_offset_get_f src_sys_port_aggregate_offset_get;
    
    dnx_data_headers_ftmh_pp_dsp_offset_get_f pp_dsp_offset_get;
    
    dnx_data_headers_ftmh_drop_precedence_offset_get_f drop_precedence_offset_get;
    
    dnx_data_headers_ftmh_tm_action_type_offset_get_f tm_action_type_offset_get;
    
    dnx_data_headers_ftmh_tm_action_is_mc_offset_get_f tm_action_is_mc_offset_get;
    
    dnx_data_headers_ftmh_outlif_offset_get_f outlif_offset_get;
    
    dnx_data_headers_ftmh_cni_offset_get_f cni_offset_get;
    
    dnx_data_headers_ftmh_ecn_enable_offset_get_f ecn_enable_offset_get;
    
    dnx_data_headers_ftmh_tm_profile_offset_get_f tm_profile_offset_get;
    
    dnx_data_headers_ftmh_visibility_offset_get_f visibility_offset_get;
    
    dnx_data_headers_ftmh_internal_header_type_offset_get_f internal_header_type_offset_get;
    
    dnx_data_headers_ftmh_tsh_en_offset_get_f tsh_en_offset_get;
    
    dnx_data_headers_ftmh_internal_header_en_offset_get_f internal_header_en_offset_get;
    
    dnx_data_headers_ftmh_tm_dest_ext_present_offset_get_f tm_dest_ext_present_offset_get;
    
    dnx_data_headers_ftmh_ase_present_offset_get_f ase_present_offset_get;
    
    dnx_data_headers_ftmh_flow_id_ext_present_offset_get_f flow_id_ext_present_offset_get;
    
    dnx_data_headers_ftmh_bier_bfr_ext_present_offset_get_f bier_bfr_ext_present_offset_get;
    
    dnx_data_headers_ftmh_stack_route_history_bmp_offset_get_f stack_route_history_bmp_offset_get;
    
    dnx_data_headers_ftmh_tm_dst_offset_get_f tm_dst_offset_get;
    
    dnx_data_headers_ftmh_observation_info_offset_get_f observation_info_offset_get;
    
    dnx_data_headers_ftmh_destination_offset_get_f destination_offset_get;
    
    dnx_data_headers_ftmh_src_sysport_offset_get_f src_sysport_offset_get;
    
    dnx_data_headers_ftmh_trajectory_trace_type_offset_get_f trajectory_trace_type_offset_get;
    
    dnx_data_headers_ftmh_port_offset_get_f port_offset_get;
    
    dnx_data_headers_ftmh_direction_offset_get_f direction_offset_get;
    
    dnx_data_headers_ftmh_session_id_offset_get_f session_id_offset_get;
    
    dnx_data_headers_ftmh_truncated_offset_get_f truncated_offset_get;
    
    dnx_data_headers_ftmh_en_offset_get_f en_offset_get;
    
    dnx_data_headers_ftmh_cos_offset_get_f cos_offset_get;
    
    dnx_data_headers_ftmh_vlan_offset_get_f vlan_offset_get;
    
    dnx_data_headers_ftmh_erspan_type_offset_get_f erspan_type_offset_get;
    
    dnx_data_headers_ftmh_flow_id_offset_get_f flow_id_offset_get;
    
    dnx_data_headers_ftmh_flow_profile_offset_get_f flow_profile_offset_get;
    
    dnx_data_headers_ftmh_int_profile_offset_get_f int_profile_offset_get;
    
    dnx_data_headers_ftmh_int_type_offset_get_f int_type_offset_get;
    
    dnx_data_headers_ftmh_packet_size_bits_get_f packet_size_bits_get;
    
    dnx_data_headers_ftmh_traffic_class_bits_get_f traffic_class_bits_get;
    
    dnx_data_headers_ftmh_src_sys_port_aggregate_bits_get_f src_sys_port_aggregate_bits_get;
    
    dnx_data_headers_ftmh_pp_dsp_bits_get_f pp_dsp_bits_get;
    
    dnx_data_headers_ftmh_drop_precedence_bits_get_f drop_precedence_bits_get;
    
    dnx_data_headers_ftmh_tm_action_type_bits_get_f tm_action_type_bits_get;
    
    dnx_data_headers_ftmh_tm_action_is_mc_bits_get_f tm_action_is_mc_bits_get;
    
    dnx_data_headers_ftmh_outlif_bits_get_f outlif_bits_get;
    
    dnx_data_headers_ftmh_cni_bits_get_f cni_bits_get;
    
    dnx_data_headers_ftmh_ecn_enable_bits_get_f ecn_enable_bits_get;
    
    dnx_data_headers_ftmh_tm_profile_bits_get_f tm_profile_bits_get;
    
    dnx_data_headers_ftmh_visibility_bits_get_f visibility_bits_get;
    
    dnx_data_headers_ftmh_tsh_en_bits_get_f tsh_en_bits_get;
    
    dnx_data_headers_ftmh_internal_header_en_bits_get_f internal_header_en_bits_get;
    
    dnx_data_headers_ftmh_tm_dest_ext_present_bits_get_f tm_dest_ext_present_bits_get;
    
    dnx_data_headers_ftmh_ase_present_bits_get_f ase_present_bits_get;
    
    dnx_data_headers_ftmh_flow_id_ext_present_bits_get_f flow_id_ext_present_bits_get;
    
    dnx_data_headers_ftmh_bier_bfr_ext_present_bits_get_f bier_bfr_ext_present_bits_get;
    
    dnx_data_headers_ftmh_tm_dst_bits_get_f tm_dst_bits_get;
    
    dnx_data_headers_ftmh_observation_info_bits_get_f observation_info_bits_get;
    
    dnx_data_headers_ftmh_destination_bits_get_f destination_bits_get;
    
    dnx_data_headers_ftmh_src_sysport_bits_get_f src_sysport_bits_get;
    
    dnx_data_headers_ftmh_trajectory_trace_type_bits_get_f trajectory_trace_type_bits_get;
    
    dnx_data_headers_ftmh_port_bits_get_f port_bits_get;
    
    dnx_data_headers_ftmh_direction_bits_get_f direction_bits_get;
    
    dnx_data_headers_ftmh_session_id_bits_get_f session_id_bits_get;
    
    dnx_data_headers_ftmh_truncated_bits_get_f truncated_bits_get;
    
    dnx_data_headers_ftmh_en_bits_get_f en_bits_get;
    
    dnx_data_headers_ftmh_cos_bits_get_f cos_bits_get;
    
    dnx_data_headers_ftmh_vlan_bits_get_f vlan_bits_get;
    
    dnx_data_headers_ftmh_erspan_type_bits_get_f erspan_type_bits_get;
    
    dnx_data_headers_ftmh_flow_id_bits_get_f flow_id_bits_get;
    
    dnx_data_headers_ftmh_flow_profile_bits_get_f flow_profile_bits_get;
    
    dnx_data_headers_ftmh_base_header_size_get_f base_header_size_get;
    
    dnx_data_headers_ftmh_ase_header_size_get_f ase_header_size_get;
    
    dnx_data_headers_ftmh_tm_dst_size_get_f tm_dst_size_get;
    
    dnx_data_headers_ftmh_flow_id_header_size_get_f flow_id_header_size_get;
    
    dnx_data_headers_ftmh_int_profile_bits_get_f int_profile_bits_get;
    
    dnx_data_headers_ftmh_int_type_bits_get_f int_type_bits_get;
    
    dnx_data_headers_ftmh_add_dsp_ext_get_f add_dsp_ext_get;
} dnx_data_if_headers_ftmh_t;







typedef enum
{

    
    _dnx_data_headers_tsh_feature_nof
} dnx_data_headers_tsh_feature_e;



typedef int(
    *dnx_data_headers_tsh_feature_get_f) (
    int unit,
    dnx_data_headers_tsh_feature_e feature);


typedef uint32(
    *dnx_data_headers_tsh_base_header_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_tsh_feature_get_f feature_get;
    
    dnx_data_headers_tsh_base_header_size_get_f base_header_size_get;
} dnx_data_if_headers_tsh_t;







typedef enum
{

    
    _dnx_data_headers_otsh_feature_nof
} dnx_data_headers_otsh_feature_e;



typedef int(
    *dnx_data_headers_otsh_feature_get_f) (
    int unit,
    dnx_data_headers_otsh_feature_e feature);


typedef uint32(
    *dnx_data_headers_otsh_base_header_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_otsh_feature_get_f feature_get;
    
    dnx_data_headers_otsh_base_header_size_get_f base_header_size_get;
} dnx_data_if_headers_otsh_t;







typedef enum
{
    
    dnx_data_headers_optimized_ftmh_ftmh_opt_parse,

    
    _dnx_data_headers_optimized_ftmh_feature_nof
} dnx_data_headers_optimized_ftmh_feature_e;



typedef int(
    *dnx_data_headers_optimized_ftmh_feature_get_f) (
    int unit,
    dnx_data_headers_optimized_ftmh_feature_e feature);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_base_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_packet_size_8_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_user_defined_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_fap_id_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_optimized_ftmh_feature_get_f feature_get;
    
    dnx_data_headers_optimized_ftmh_base_header_size_get_f base_header_size_get;
    
    dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_get_f packet_size_7_0_offset_get;
    
    dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_get_f tm_action_is_mc_offset_get;
    
    dnx_data_headers_optimized_ftmh_packet_size_8_offset_get_f packet_size_8_offset_get;
    
    dnx_data_headers_optimized_ftmh_user_defined_offset_get_f user_defined_offset_get;
    
    dnx_data_headers_optimized_ftmh_fap_id_offset_get_f fap_id_offset_get;
    
    dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_get_f pp_dsp_or_mc_id_offset_get;
} dnx_data_if_headers_optimized_ftmh_t;







typedef enum
{
    
    dnx_data_headers_internal_header_visibilty_mode_global,

    
    _dnx_data_headers_internal_feature_nof
} dnx_data_headers_internal_feature_e;



typedef int(
    *dnx_data_headers_internal_feature_get_f) (
    int unit,
    dnx_data_headers_internal_feature_e feature);


typedef uint32(
    *dnx_data_headers_internal_parsing_start_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_parsing_start_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_lif_ext_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_lif_ext_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fai_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fai_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_tail_edit_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_tail_edit_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_forward_domain_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_forward_domain_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_inlif_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_inlif_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fhei_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fhei_size_sz0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fhei_size_sz1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fhei_size_sz2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fhei_size_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_fhei_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_learning_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_learning_ext_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_learning_ext_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_ttl_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_ttl_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_eth_rm_fli_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_eth_rm_fli_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_eth_rm_pso_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_eth_rm_pso_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_cfg_fai_unknown_address_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_internal_feature_get_f feature_get;
    
    dnx_data_headers_internal_parsing_start_type_offset_get_f parsing_start_type_offset_get;
    
    dnx_data_headers_internal_parsing_start_type_bits_get_f parsing_start_type_bits_get;
    
    dnx_data_headers_internal_lif_ext_type_offset_get_f lif_ext_type_offset_get;
    
    dnx_data_headers_internal_lif_ext_type_bits_get_f lif_ext_type_bits_get;
    
    dnx_data_headers_internal_fai_offset_get_f fai_offset_get;
    
    dnx_data_headers_internal_fai_bits_get_f fai_bits_get;
    
    dnx_data_headers_internal_tail_edit_offset_get_f tail_edit_offset_get;
    
    dnx_data_headers_internal_tail_edit_bits_get_f tail_edit_bits_get;
    
    dnx_data_headers_internal_forward_domain_offset_get_f forward_domain_offset_get;
    
    dnx_data_headers_internal_forward_domain_bits_get_f forward_domain_bits_get;
    
    dnx_data_headers_internal_inlif_offset_get_f inlif_offset_get;
    
    dnx_data_headers_internal_inlif_bits_get_f inlif_bits_get;
    
    dnx_data_headers_internal_fhei_size_get_f fhei_size_get;
    
    dnx_data_headers_internal_fhei_size_sz0_get_f fhei_size_sz0_get;
    
    dnx_data_headers_internal_fhei_size_sz1_get_f fhei_size_sz1_get;
    
    dnx_data_headers_internal_fhei_size_sz2_get_f fhei_size_sz2_get;
    
    dnx_data_headers_internal_fhei_size_offset_get_f fhei_size_offset_get;
    
    dnx_data_headers_internal_fhei_size_bits_get_f fhei_size_bits_get;
    
    dnx_data_headers_internal_learning_ext_present_offset_get_f learning_ext_present_offset_get;
    
    dnx_data_headers_internal_learning_ext_present_bits_get_f learning_ext_present_bits_get;
    
    dnx_data_headers_internal_learning_ext_size_get_f learning_ext_size_get;
    
    dnx_data_headers_internal_ttl_offset_get_f ttl_offset_get;
    
    dnx_data_headers_internal_ttl_bits_get_f ttl_bits_get;
    
    dnx_data_headers_internal_eth_rm_fli_offset_get_f eth_rm_fli_offset_get;
    
    dnx_data_headers_internal_eth_rm_fli_bits_get_f eth_rm_fli_bits_get;
    
    dnx_data_headers_internal_eth_rm_pso_offset_get_f eth_rm_pso_offset_get;
    
    dnx_data_headers_internal_eth_rm_pso_bits_get_f eth_rm_pso_bits_get;
    
    dnx_data_headers_internal_cfg_fai_unknown_address_enable_get_f cfg_fai_unknown_address_enable_get;
} dnx_data_if_headers_internal_t;







typedef enum
{

    
    _dnx_data_headers_internal_legacy_feature_nof
} dnx_data_headers_internal_legacy_feature_e;



typedef int(
    *dnx_data_headers_internal_legacy_feature_get_f) (
    int unit,
    dnx_data_headers_internal_legacy_feature_e feature);


typedef uint32(
    *dnx_data_headers_internal_legacy_fhei_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_fhei_size_sz0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_fhei_size_sz1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_fhei_size_sz2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_inlif_inrif_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_inlif_inrif_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_unknown_address_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_unknown_address_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_forwarding_header_offset_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_forwarding_header_offset_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_forward_code_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_forward_code_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_fhei_size_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_fhei_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_learning_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_learning_ext_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_learning_ext_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_eei_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_internal_legacy_eei_ext_present_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_internal_legacy_feature_get_f feature_get;
    
    dnx_data_headers_internal_legacy_fhei_size_get_f fhei_size_get;
    
    dnx_data_headers_internal_legacy_fhei_size_sz0_get_f fhei_size_sz0_get;
    
    dnx_data_headers_internal_legacy_fhei_size_sz1_get_f fhei_size_sz1_get;
    
    dnx_data_headers_internal_legacy_fhei_size_sz2_get_f fhei_size_sz2_get;
    
    dnx_data_headers_internal_legacy_inlif_inrif_offset_get_f inlif_inrif_offset_get;
    
    dnx_data_headers_internal_legacy_inlif_inrif_bits_get_f inlif_inrif_bits_get;
    
    dnx_data_headers_internal_legacy_unknown_address_offset_get_f unknown_address_offset_get;
    
    dnx_data_headers_internal_legacy_unknown_address_bits_get_f unknown_address_bits_get;
    
    dnx_data_headers_internal_legacy_forwarding_header_offset_offset_get_f forwarding_header_offset_offset_get;
    
    dnx_data_headers_internal_legacy_forwarding_header_offset_bits_get_f forwarding_header_offset_bits_get;
    
    dnx_data_headers_internal_legacy_forward_code_offset_get_f forward_code_offset_get;
    
    dnx_data_headers_internal_legacy_forward_code_bits_get_f forward_code_bits_get;
    
    dnx_data_headers_internal_legacy_fhei_size_offset_get_f fhei_size_offset_get;
    
    dnx_data_headers_internal_legacy_fhei_size_bits_get_f fhei_size_bits_get;
    
    dnx_data_headers_internal_legacy_learning_ext_present_offset_get_f learning_ext_present_offset_get;
    
    dnx_data_headers_internal_legacy_learning_ext_present_bits_get_f learning_ext_present_bits_get;
    
    dnx_data_headers_internal_legacy_learning_ext_size_get_f learning_ext_size_get;
    
    dnx_data_headers_internal_legacy_eei_ext_present_offset_get_f eei_ext_present_offset_get;
    
    dnx_data_headers_internal_legacy_eei_ext_present_bits_get_f eei_ext_present_bits_get;
} dnx_data_if_headers_internal_legacy_t;







typedef enum
{

    
    _dnx_data_headers_fhei_sz1_trap_sniff_feature_nof
} dnx_data_headers_fhei_sz1_trap_sniff_feature_e;



typedef int(
    *dnx_data_headers_fhei_sz1_trap_sniff_feature_get_f) (
    int unit,
    dnx_data_headers_fhei_sz1_trap_sniff_feature_e feature);


typedef uint32(
    *dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_sz1_trap_sniff_code_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_sz1_trap_sniff_code_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_fhei_sz1_trap_sniff_feature_get_f feature_get;
    
    dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_get_f fhei_type_offset_get;
    
    dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_get_f fhei_type_bits_get;
    
    dnx_data_headers_fhei_sz1_trap_sniff_code_offset_get_f code_offset_get;
    
    dnx_data_headers_fhei_sz1_trap_sniff_code_bits_get_f code_bits_get;
    
    dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_get_f qualifier_offset_get;
    
    dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_get_f qualifier_bits_get;
} dnx_data_if_headers_fhei_sz1_trap_sniff_t;







typedef enum
{

    
    _dnx_data_headers_fhei_trap_sniff_legacy_feature_nof
} dnx_data_headers_fhei_trap_sniff_legacy_feature_e;



typedef int(
    *dnx_data_headers_fhei_trap_sniff_legacy_feature_get_f) (
    int unit,
    dnx_data_headers_fhei_trap_sniff_legacy_feature_e feature);


typedef uint32(
    *dnx_data_headers_fhei_trap_sniff_legacy_code_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_trap_sniff_legacy_code_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_fhei_trap_sniff_legacy_feature_get_f feature_get;
    
    dnx_data_headers_fhei_trap_sniff_legacy_code_offset_get_f code_offset_get;
    
    dnx_data_headers_fhei_trap_sniff_legacy_code_bits_get_f code_bits_get;
    
    dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_get_f qualifier_offset_get;
    
    dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_get_f qualifier_bits_get;
} dnx_data_if_headers_fhei_trap_sniff_legacy_t;







typedef enum
{

    
    _dnx_data_headers_udh_feature_nof
} dnx_data_headers_udh_feature_e;



typedef int(
    *dnx_data_headers_udh_feature_get_f) (
    int unit,
    dnx_data_headers_udh_feature_e feature);


typedef uint32(
    *dnx_data_headers_udh_data_type0_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type1_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type2_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type3_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type0_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type1_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type2_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_type3_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_data_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_udh_base_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_udh_feature_get_f feature_get;
    
    dnx_data_headers_udh_data_type0_offset_get_f data_type0_offset_get;
    
    dnx_data_headers_udh_data_type1_offset_get_f data_type1_offset_get;
    
    dnx_data_headers_udh_data_type2_offset_get_f data_type2_offset_get;
    
    dnx_data_headers_udh_data_type3_offset_get_f data_type3_offset_get;
    
    dnx_data_headers_udh_data_offset_get_f data_offset_get;
    
    dnx_data_headers_udh_data_type0_bits_get_f data_type0_bits_get;
    
    dnx_data_headers_udh_data_type1_bits_get_f data_type1_bits_get;
    
    dnx_data_headers_udh_data_type2_bits_get_f data_type2_bits_get;
    
    dnx_data_headers_udh_data_type3_bits_get_f data_type3_bits_get;
    
    dnx_data_headers_udh_data_bits_get_f data_bits_get;
    
    dnx_data_headers_udh_base_size_get_f base_size_get;
} dnx_data_if_headers_udh_t;







typedef enum
{

    
    _dnx_data_headers_ftmh_legacy_feature_nof
} dnx_data_headers_ftmh_legacy_feature_e;



typedef int(
    *dnx_data_headers_ftmh_legacy_feature_get_f) (
    int unit,
    dnx_data_headers_ftmh_legacy_feature_e feature);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_packet_size_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_traffic_class_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_pp_dsp_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_drop_precedence_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_tm_action_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_otsh_en_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_internal_header_en_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_outlif_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_cni_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_ecn_enable_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_exclude_source_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_packet_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_traffic_class_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_pp_dsp_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_drop_precedence_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_tm_action_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_otsh_en_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_internal_header_en_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_outlif_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_cni_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_ecn_enable_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_exclude_source_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_base_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_ftmh_legacy_dsp_ext_header_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_ftmh_legacy_feature_get_f feature_get;
    
    dnx_data_headers_ftmh_legacy_packet_size_offset_get_f packet_size_offset_get;
    
    dnx_data_headers_ftmh_legacy_traffic_class_offset_get_f traffic_class_offset_get;
    
    dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_get_f src_sys_port_aggregate_offset_get;
    
    dnx_data_headers_ftmh_legacy_pp_dsp_offset_get_f pp_dsp_offset_get;
    
    dnx_data_headers_ftmh_legacy_drop_precedence_offset_get_f drop_precedence_offset_get;
    
    dnx_data_headers_ftmh_legacy_tm_action_type_offset_get_f tm_action_type_offset_get;
    
    dnx_data_headers_ftmh_legacy_otsh_en_offset_get_f otsh_en_offset_get;
    
    dnx_data_headers_ftmh_legacy_internal_header_en_offset_get_f internal_header_en_offset_get;
    
    dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_get_f outbound_mirr_disable_offset_get;
    
    dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_get_f tm_action_is_mc_offset_get;
    
    dnx_data_headers_ftmh_legacy_outlif_offset_get_f outlif_offset_get;
    
    dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_get_f dsp_ext_present_offset_get;
    
    dnx_data_headers_ftmh_legacy_cni_offset_get_f cni_offset_get;
    
    dnx_data_headers_ftmh_legacy_ecn_enable_offset_get_f ecn_enable_offset_get;
    
    dnx_data_headers_ftmh_legacy_exclude_source_offset_get_f exclude_source_offset_get;
    
    dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_get_f stack_route_history_bmp_offset_get;
    
    dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_get_f dsp_ext_header_offset_get;
    
    dnx_data_headers_ftmh_legacy_packet_size_bits_get_f packet_size_bits_get;
    
    dnx_data_headers_ftmh_legacy_traffic_class_bits_get_f traffic_class_bits_get;
    
    dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_get_f src_sys_port_aggregate_bits_get;
    
    dnx_data_headers_ftmh_legacy_pp_dsp_bits_get_f pp_dsp_bits_get;
    
    dnx_data_headers_ftmh_legacy_drop_precedence_bits_get_f drop_precedence_bits_get;
    
    dnx_data_headers_ftmh_legacy_tm_action_type_bits_get_f tm_action_type_bits_get;
    
    dnx_data_headers_ftmh_legacy_otsh_en_bits_get_f otsh_en_bits_get;
    
    dnx_data_headers_ftmh_legacy_internal_header_en_bits_get_f internal_header_en_bits_get;
    
    dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_get_f outbound_mirr_disable_bits_get;
    
    dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_get_f tm_action_is_mc_bits_get;
    
    dnx_data_headers_ftmh_legacy_outlif_bits_get_f outlif_bits_get;
    
    dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_get_f dsp_ext_present_bits_get;
    
    dnx_data_headers_ftmh_legacy_cni_bits_get_f cni_bits_get;
    
    dnx_data_headers_ftmh_legacy_ecn_enable_bits_get_f ecn_enable_bits_get;
    
    dnx_data_headers_ftmh_legacy_exclude_source_bits_get_f exclude_source_bits_get;
    
    dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_get_f dsp_ext_header_bits_get;
    
    dnx_data_headers_ftmh_legacy_base_header_size_get_f base_header_size_get;
    
    dnx_data_headers_ftmh_legacy_dsp_ext_header_size_get_f dsp_ext_header_size_get;
} dnx_data_if_headers_ftmh_legacy_t;







typedef enum
{

    
    _dnx_data_headers_otsh_legacy_feature_nof
} dnx_data_headers_otsh_legacy_feature_e;



typedef int(
    *dnx_data_headers_otsh_legacy_feature_get_f) (
    int unit,
    dnx_data_headers_otsh_legacy_feature_e feature);


typedef uint32(
    *dnx_data_headers_otsh_legacy_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_oam_sub_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_oam_sub_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_mep_type_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_mep_type_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_tp_command_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_tp_command_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_ts_encapsulation_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_ts_encapsulation_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_offset_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_otsh_legacy_offset_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_otsh_legacy_feature_get_f feature_get;
    
    dnx_data_headers_otsh_legacy_type_offset_get_f type_offset_get;
    
    dnx_data_headers_otsh_legacy_type_bits_get_f type_bits_get;
    
    dnx_data_headers_otsh_legacy_oam_sub_type_offset_get_f oam_sub_type_offset_get;
    
    dnx_data_headers_otsh_legacy_oam_sub_type_bits_get_f oam_sub_type_bits_get;
    
    dnx_data_headers_otsh_legacy_mep_type_offset_get_f mep_type_offset_get;
    
    dnx_data_headers_otsh_legacy_mep_type_bits_get_f mep_type_bits_get;
    
    dnx_data_headers_otsh_legacy_tp_command_offset_get_f tp_command_offset_get;
    
    dnx_data_headers_otsh_legacy_tp_command_bits_get_f tp_command_bits_get;
    
    dnx_data_headers_otsh_legacy_ts_encapsulation_offset_get_f ts_encapsulation_offset_get;
    
    dnx_data_headers_otsh_legacy_ts_encapsulation_bits_get_f ts_encapsulation_bits_get;
    
    dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_get_f oam_ts_data_0_offset_get;
    
    dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_get_f oam_ts_data_0_bits_get;
    
    dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_get_f oam_ts_data_1_offset_get;
    
    dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_get_f oam_ts_data_1_bits_get;
    
    dnx_data_headers_otsh_legacy_offset_offset_get_f offset_offset_get;
    
    dnx_data_headers_otsh_legacy_offset_bits_get_f offset_bits_get;
} dnx_data_if_headers_otsh_legacy_t;







typedef enum
{

    
    _dnx_data_headers_system_headers_feature_nof
} dnx_data_headers_system_headers_feature_e;



typedef int(
    *dnx_data_headers_system_headers_feature_get_f) (
    int unit,
    dnx_data_headers_system_headers_feature_e feature);


typedef uint32(
    *dnx_data_headers_system_headers_system_headers_mode_jericho_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_system_headers_mode_jericho2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_system_headers_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_udh_base_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_system_headers_feature_get_f feature_get;
    
    dnx_data_headers_system_headers_system_headers_mode_jericho_get_f system_headers_mode_jericho_get;
    
    dnx_data_headers_system_headers_system_headers_mode_jericho2_get_f system_headers_mode_jericho2_get;
    
    dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_get_f crc_size_counted_in_pkt_len_get;
    
    dnx_data_headers_system_headers_system_headers_mode_get_f system_headers_mode_get;
    
    dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_get_f jr_mode_ftmh_lb_key_ext_mode_get;
    
    dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_get_f jr_mode_ftmh_stacking_ext_mode_get;
    
    dnx_data_headers_system_headers_udh_base_size_get_f udh_base_size_get;
    
    dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_get_f jr_mode_ftmh_oam_ts_right_shift_get;
} dnx_data_if_headers_system_headers_t;







typedef enum
{
    
    dnx_data_headers_otmh_otmh_enable,

    
    _dnx_data_headers_otmh_feature_nof
} dnx_data_headers_otmh_feature_e;



typedef int(
    *dnx_data_headers_otmh_feature_get_f) (
    int unit,
    dnx_data_headers_otmh_feature_e feature);



typedef struct
{
    
    dnx_data_headers_otmh_feature_get_f feature_get;
} dnx_data_if_headers_otmh_t;







typedef enum
{

    
    _dnx_data_headers_pph_feature_nof
} dnx_data_headers_pph_feature_e;



typedef int(
    *dnx_data_headers_pph_feature_get_f) (
    int unit,
    dnx_data_headers_pph_feature_e feature);


typedef uint32(
    *dnx_data_headers_pph_pph_vsi_selection_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_pph_feature_get_f feature_get;
    
    dnx_data_headers_pph_pph_vsi_selection_size_get_f pph_vsi_selection_size_get;
} dnx_data_if_headers_pph_t;







typedef enum
{
    
    dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx,
    
    dnx_data_headers_general_jericho_mode_sys_header_is_supported,

    
    _dnx_data_headers_general_feature_nof
} dnx_data_headers_general_feature_e;



typedef int(
    *dnx_data_headers_general_feature_get_f) (
    int unit,
    dnx_data_headers_general_feature_e feature);



typedef struct
{
    
    dnx_data_headers_general_feature_get_f feature_get;
} dnx_data_if_headers_general_t;







typedef enum
{
    
    dnx_data_headers_feature_fhei_mpls_cmd_from_eei,
    
    dnx_data_headers_feature_unknown_address,
    
    dnx_data_headers_feature_tsh_ext_with_flow_id,

    
    _dnx_data_headers_feature_feature_nof
} dnx_data_headers_feature_feature_e;



typedef int(
    *dnx_data_headers_feature_feature_get_f) (
    int unit,
    dnx_data_headers_feature_feature_e feature);


typedef uint32(
    *dnx_data_headers_feature_fhei_mpls_swap_remark_profile_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_headers_feature_feature_get_f feature_get;
    
    dnx_data_headers_feature_fhei_mpls_swap_remark_profile_size_get_f fhei_mpls_swap_remark_profile_size_get;
} dnx_data_if_headers_feature_t;





typedef struct
{
    
    dnx_data_if_headers_ftmh_t ftmh;
    
    dnx_data_if_headers_tsh_t tsh;
    
    dnx_data_if_headers_otsh_t otsh;
    
    dnx_data_if_headers_optimized_ftmh_t optimized_ftmh;
    
    dnx_data_if_headers_internal_t internal;
    
    dnx_data_if_headers_internal_legacy_t internal_legacy;
    
    dnx_data_if_headers_fhei_sz1_trap_sniff_t fhei_sz1_trap_sniff;
    
    dnx_data_if_headers_fhei_trap_sniff_legacy_t fhei_trap_sniff_legacy;
    
    dnx_data_if_headers_udh_t udh;
    
    dnx_data_if_headers_ftmh_legacy_t ftmh_legacy;
    
    dnx_data_if_headers_otsh_legacy_t otsh_legacy;
    
    dnx_data_if_headers_system_headers_t system_headers;
    
    dnx_data_if_headers_otmh_t otmh;
    
    dnx_data_if_headers_pph_t pph;
    
    dnx_data_if_headers_general_t general;
    
    dnx_data_if_headers_feature_t feature;
} dnx_data_if_headers_t;




extern dnx_data_if_headers_t dnx_data_headers;


#endif 

