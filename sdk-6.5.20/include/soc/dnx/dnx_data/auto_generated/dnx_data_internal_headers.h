

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_HEADERS_H_

#define _DNX_DATA_INTERNAL_HEADERS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_headers_submodule_ftmh,
    dnx_data_headers_submodule_tsh,
    dnx_data_headers_submodule_otsh,
    dnx_data_headers_submodule_optimized_ftmh,
    dnx_data_headers_submodule_internal,
    dnx_data_headers_submodule_internal_legacy,
    dnx_data_headers_submodule_fhei_sz1_trap_sniff,
    dnx_data_headers_submodule_fhei_trap_sniff_legacy,
    dnx_data_headers_submodule_udh,
    dnx_data_headers_submodule_ftmh_legacy,
    dnx_data_headers_submodule_otsh_legacy,
    dnx_data_headers_submodule_system_headers,
    dnx_data_headers_submodule_otmh,
    dnx_data_headers_submodule_pph,
    dnx_data_headers_submodule_general,
    dnx_data_headers_submodule_feature,

    
    _dnx_data_headers_submodule_nof
} dnx_data_headers_submodule_e;








int dnx_data_headers_ftmh_feature_get(
    int unit,
    dnx_data_headers_ftmh_feature_e feature);



typedef enum
{
    dnx_data_headers_ftmh_define_packet_size_offset,
    dnx_data_headers_ftmh_define_traffic_class_offset,
    dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset,
    dnx_data_headers_ftmh_define_pp_dsp_offset,
    dnx_data_headers_ftmh_define_drop_precedence_offset,
    dnx_data_headers_ftmh_define_tm_action_type_offset,
    dnx_data_headers_ftmh_define_tm_action_is_mc_offset,
    dnx_data_headers_ftmh_define_outlif_offset,
    dnx_data_headers_ftmh_define_cni_offset,
    dnx_data_headers_ftmh_define_ecn_enable_offset,
    dnx_data_headers_ftmh_define_tm_profile_offset,
    dnx_data_headers_ftmh_define_visibility_offset,
    dnx_data_headers_ftmh_define_internal_header_type_offset,
    dnx_data_headers_ftmh_define_tsh_en_offset,
    dnx_data_headers_ftmh_define_internal_header_en_offset,
    dnx_data_headers_ftmh_define_tm_dest_ext_present_offset,
    dnx_data_headers_ftmh_define_ase_present_offset,
    dnx_data_headers_ftmh_define_flow_id_ext_present_offset,
    dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset,
    dnx_data_headers_ftmh_define_stack_route_history_bmp_offset,
    dnx_data_headers_ftmh_define_tm_dst_offset,
    dnx_data_headers_ftmh_define_observation_info_offset,
    dnx_data_headers_ftmh_define_destination_offset,
    dnx_data_headers_ftmh_define_src_sysport_offset,
    dnx_data_headers_ftmh_define_trajectory_trace_type_offset,
    dnx_data_headers_ftmh_define_port_offset,
    dnx_data_headers_ftmh_define_direction_offset,
    dnx_data_headers_ftmh_define_session_id_offset,
    dnx_data_headers_ftmh_define_truncated_offset,
    dnx_data_headers_ftmh_define_en_offset,
    dnx_data_headers_ftmh_define_cos_offset,
    dnx_data_headers_ftmh_define_vlan_offset,
    dnx_data_headers_ftmh_define_erspan_type_offset,
    dnx_data_headers_ftmh_define_flow_id_offset,
    dnx_data_headers_ftmh_define_flow_profile_offset,
    dnx_data_headers_ftmh_define_int_profile_offset,
    dnx_data_headers_ftmh_define_int_type_offset,
    dnx_data_headers_ftmh_define_packet_size_bits,
    dnx_data_headers_ftmh_define_traffic_class_bits,
    dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits,
    dnx_data_headers_ftmh_define_pp_dsp_bits,
    dnx_data_headers_ftmh_define_drop_precedence_bits,
    dnx_data_headers_ftmh_define_tm_action_type_bits,
    dnx_data_headers_ftmh_define_tm_action_is_mc_bits,
    dnx_data_headers_ftmh_define_outlif_bits,
    dnx_data_headers_ftmh_define_cni_bits,
    dnx_data_headers_ftmh_define_ecn_enable_bits,
    dnx_data_headers_ftmh_define_tm_profile_bits,
    dnx_data_headers_ftmh_define_visibility_bits,
    dnx_data_headers_ftmh_define_tsh_en_bits,
    dnx_data_headers_ftmh_define_internal_header_en_bits,
    dnx_data_headers_ftmh_define_tm_dest_ext_present_bits,
    dnx_data_headers_ftmh_define_ase_present_bits,
    dnx_data_headers_ftmh_define_flow_id_ext_present_bits,
    dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits,
    dnx_data_headers_ftmh_define_tm_dst_bits,
    dnx_data_headers_ftmh_define_observation_info_bits,
    dnx_data_headers_ftmh_define_destination_bits,
    dnx_data_headers_ftmh_define_src_sysport_bits,
    dnx_data_headers_ftmh_define_trajectory_trace_type_bits,
    dnx_data_headers_ftmh_define_port_bits,
    dnx_data_headers_ftmh_define_direction_bits,
    dnx_data_headers_ftmh_define_session_id_bits,
    dnx_data_headers_ftmh_define_truncated_bits,
    dnx_data_headers_ftmh_define_en_bits,
    dnx_data_headers_ftmh_define_cos_bits,
    dnx_data_headers_ftmh_define_vlan_bits,
    dnx_data_headers_ftmh_define_erspan_type_bits,
    dnx_data_headers_ftmh_define_flow_id_bits,
    dnx_data_headers_ftmh_define_flow_profile_bits,
    dnx_data_headers_ftmh_define_base_header_size,
    dnx_data_headers_ftmh_define_ase_header_size,
    dnx_data_headers_ftmh_define_tm_dst_size,
    dnx_data_headers_ftmh_define_flow_id_header_size,
    dnx_data_headers_ftmh_define_int_profile_bits,
    dnx_data_headers_ftmh_define_int_type_bits,
    dnx_data_headers_ftmh_define_add_dsp_ext,

    
    _dnx_data_headers_ftmh_define_nof
} dnx_data_headers_ftmh_define_e;



uint32 dnx_data_headers_ftmh_packet_size_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_traffic_class_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_src_sys_port_aggregate_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_pp_dsp_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_drop_precedence_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_action_type_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_action_is_mc_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_outlif_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_cni_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_ecn_enable_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_profile_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_visibility_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_internal_header_type_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_tsh_en_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_internal_header_en_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_dest_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_ase_present_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_id_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_bier_bfr_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_stack_route_history_bmp_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_dst_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_observation_info_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_destination_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_src_sysport_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_trajectory_trace_type_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_port_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_direction_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_session_id_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_truncated_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_en_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_cos_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_vlan_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_erspan_type_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_id_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_profile_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_int_profile_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_int_type_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_packet_size_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_traffic_class_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_src_sys_port_aggregate_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_pp_dsp_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_drop_precedence_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_action_type_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_action_is_mc_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_outlif_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_cni_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_ecn_enable_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_profile_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_visibility_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_tsh_en_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_internal_header_en_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_dest_ext_present_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_ase_present_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_id_ext_present_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_bier_bfr_ext_present_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_dst_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_observation_info_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_destination_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_src_sysport_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_trajectory_trace_type_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_port_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_direction_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_session_id_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_truncated_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_en_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_cos_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_vlan_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_erspan_type_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_id_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_profile_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_base_header_size_get(
    int unit);


uint32 dnx_data_headers_ftmh_ase_header_size_get(
    int unit);


uint32 dnx_data_headers_ftmh_tm_dst_size_get(
    int unit);


uint32 dnx_data_headers_ftmh_flow_id_header_size_get(
    int unit);


uint32 dnx_data_headers_ftmh_int_profile_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_int_type_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_add_dsp_ext_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_ftmh_table_nof
} dnx_data_headers_ftmh_table_e;









int dnx_data_headers_tsh_feature_get(
    int unit,
    dnx_data_headers_tsh_feature_e feature);



typedef enum
{
    dnx_data_headers_tsh_define_base_header_size,

    
    _dnx_data_headers_tsh_define_nof
} dnx_data_headers_tsh_define_e;



uint32 dnx_data_headers_tsh_base_header_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_tsh_table_nof
} dnx_data_headers_tsh_table_e;









int dnx_data_headers_otsh_feature_get(
    int unit,
    dnx_data_headers_otsh_feature_e feature);



typedef enum
{
    dnx_data_headers_otsh_define_base_header_size,

    
    _dnx_data_headers_otsh_define_nof
} dnx_data_headers_otsh_define_e;



uint32 dnx_data_headers_otsh_base_header_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_otsh_table_nof
} dnx_data_headers_otsh_table_e;









int dnx_data_headers_optimized_ftmh_feature_get(
    int unit,
    dnx_data_headers_optimized_ftmh_feature_e feature);



typedef enum
{
    dnx_data_headers_optimized_ftmh_define_base_header_size,
    dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset,
    dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset,
    dnx_data_headers_optimized_ftmh_define_packet_size_8_offset,
    dnx_data_headers_optimized_ftmh_define_user_defined_offset,
    dnx_data_headers_optimized_ftmh_define_fap_id_offset,
    dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset,

    
    _dnx_data_headers_optimized_ftmh_define_nof
} dnx_data_headers_optimized_ftmh_define_e;



uint32 dnx_data_headers_optimized_ftmh_base_header_size_get(
    int unit);


uint32 dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_get(
    int unit);


uint32 dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_get(
    int unit);


uint32 dnx_data_headers_optimized_ftmh_packet_size_8_offset_get(
    int unit);


uint32 dnx_data_headers_optimized_ftmh_user_defined_offset_get(
    int unit);


uint32 dnx_data_headers_optimized_ftmh_fap_id_offset_get(
    int unit);


uint32 dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_optimized_ftmh_table_nof
} dnx_data_headers_optimized_ftmh_table_e;









int dnx_data_headers_internal_feature_get(
    int unit,
    dnx_data_headers_internal_feature_e feature);



typedef enum
{
    dnx_data_headers_internal_define_parsing_start_type_offset,
    dnx_data_headers_internal_define_parsing_start_type_bits,
    dnx_data_headers_internal_define_lif_ext_type_offset,
    dnx_data_headers_internal_define_lif_ext_type_bits,
    dnx_data_headers_internal_define_fai_offset,
    dnx_data_headers_internal_define_fai_bits,
    dnx_data_headers_internal_define_tail_edit_offset,
    dnx_data_headers_internal_define_tail_edit_bits,
    dnx_data_headers_internal_define_forward_domain_offset,
    dnx_data_headers_internal_define_forward_domain_bits,
    dnx_data_headers_internal_define_inlif_offset,
    dnx_data_headers_internal_define_inlif_bits,
    dnx_data_headers_internal_define_fhei_size,
    dnx_data_headers_internal_define_fhei_size_sz0,
    dnx_data_headers_internal_define_fhei_size_sz1,
    dnx_data_headers_internal_define_fhei_size_sz2,
    dnx_data_headers_internal_define_fhei_size_offset,
    dnx_data_headers_internal_define_fhei_size_bits,
    dnx_data_headers_internal_define_learning_ext_present_offset,
    dnx_data_headers_internal_define_learning_ext_present_bits,
    dnx_data_headers_internal_define_learning_ext_size,
    dnx_data_headers_internal_define_ttl_offset,
    dnx_data_headers_internal_define_ttl_bits,
    dnx_data_headers_internal_define_eth_rm_fli_offset,
    dnx_data_headers_internal_define_eth_rm_fli_bits,
    dnx_data_headers_internal_define_eth_rm_pso_offset,
    dnx_data_headers_internal_define_eth_rm_pso_bits,
    dnx_data_headers_internal_define_cfg_fai_unknown_address_enable,

    
    _dnx_data_headers_internal_define_nof
} dnx_data_headers_internal_define_e;



uint32 dnx_data_headers_internal_parsing_start_type_offset_get(
    int unit);


uint32 dnx_data_headers_internal_parsing_start_type_bits_get(
    int unit);


uint32 dnx_data_headers_internal_lif_ext_type_offset_get(
    int unit);


uint32 dnx_data_headers_internal_lif_ext_type_bits_get(
    int unit);


uint32 dnx_data_headers_internal_fai_offset_get(
    int unit);


uint32 dnx_data_headers_internal_fai_bits_get(
    int unit);


uint32 dnx_data_headers_internal_tail_edit_offset_get(
    int unit);


uint32 dnx_data_headers_internal_tail_edit_bits_get(
    int unit);


uint32 dnx_data_headers_internal_forward_domain_offset_get(
    int unit);


uint32 dnx_data_headers_internal_forward_domain_bits_get(
    int unit);


uint32 dnx_data_headers_internal_inlif_offset_get(
    int unit);


uint32 dnx_data_headers_internal_inlif_bits_get(
    int unit);


uint32 dnx_data_headers_internal_fhei_size_get(
    int unit);


uint32 dnx_data_headers_internal_fhei_size_sz0_get(
    int unit);


uint32 dnx_data_headers_internal_fhei_size_sz1_get(
    int unit);


uint32 dnx_data_headers_internal_fhei_size_sz2_get(
    int unit);


uint32 dnx_data_headers_internal_fhei_size_offset_get(
    int unit);


uint32 dnx_data_headers_internal_fhei_size_bits_get(
    int unit);


uint32 dnx_data_headers_internal_learning_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_internal_learning_ext_present_bits_get(
    int unit);


uint32 dnx_data_headers_internal_learning_ext_size_get(
    int unit);


uint32 dnx_data_headers_internal_ttl_offset_get(
    int unit);


uint32 dnx_data_headers_internal_ttl_bits_get(
    int unit);


uint32 dnx_data_headers_internal_eth_rm_fli_offset_get(
    int unit);


uint32 dnx_data_headers_internal_eth_rm_fli_bits_get(
    int unit);


uint32 dnx_data_headers_internal_eth_rm_pso_offset_get(
    int unit);


uint32 dnx_data_headers_internal_eth_rm_pso_bits_get(
    int unit);


uint32 dnx_data_headers_internal_cfg_fai_unknown_address_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_internal_table_nof
} dnx_data_headers_internal_table_e;









int dnx_data_headers_internal_legacy_feature_get(
    int unit,
    dnx_data_headers_internal_legacy_feature_e feature);



typedef enum
{
    dnx_data_headers_internal_legacy_define_fhei_size,
    dnx_data_headers_internal_legacy_define_fhei_size_sz0,
    dnx_data_headers_internal_legacy_define_fhei_size_sz1,
    dnx_data_headers_internal_legacy_define_fhei_size_sz2,
    dnx_data_headers_internal_legacy_define_inlif_inrif_offset,
    dnx_data_headers_internal_legacy_define_inlif_inrif_bits,
    dnx_data_headers_internal_legacy_define_unknown_address_offset,
    dnx_data_headers_internal_legacy_define_unknown_address_bits,
    dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset,
    dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits,
    dnx_data_headers_internal_legacy_define_forward_code_offset,
    dnx_data_headers_internal_legacy_define_forward_code_bits,
    dnx_data_headers_internal_legacy_define_fhei_size_offset,
    dnx_data_headers_internal_legacy_define_fhei_size_bits,
    dnx_data_headers_internal_legacy_define_learning_ext_present_offset,
    dnx_data_headers_internal_legacy_define_learning_ext_present_bits,
    dnx_data_headers_internal_legacy_define_learning_ext_size,
    dnx_data_headers_internal_legacy_define_eei_ext_present_offset,
    dnx_data_headers_internal_legacy_define_eei_ext_present_bits,

    
    _dnx_data_headers_internal_legacy_define_nof
} dnx_data_headers_internal_legacy_define_e;



uint32 dnx_data_headers_internal_legacy_fhei_size_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_fhei_size_sz0_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_fhei_size_sz1_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_fhei_size_sz2_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_inlif_inrif_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_inlif_inrif_bits_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_unknown_address_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_unknown_address_bits_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_forwarding_header_offset_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_forwarding_header_offset_bits_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_forward_code_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_forward_code_bits_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_fhei_size_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_fhei_size_bits_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_learning_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_learning_ext_present_bits_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_learning_ext_size_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_eei_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_internal_legacy_eei_ext_present_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_internal_legacy_table_nof
} dnx_data_headers_internal_legacy_table_e;









int dnx_data_headers_fhei_sz1_trap_sniff_feature_get(
    int unit,
    dnx_data_headers_fhei_sz1_trap_sniff_feature_e feature);



typedef enum
{
    dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset,
    dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits,
    dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset,
    dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits,
    dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset,
    dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits,

    
    _dnx_data_headers_fhei_sz1_trap_sniff_define_nof
} dnx_data_headers_fhei_sz1_trap_sniff_define_e;



uint32 dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_get(
    int unit);


uint32 dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_get(
    int unit);


uint32 dnx_data_headers_fhei_sz1_trap_sniff_code_offset_get(
    int unit);


uint32 dnx_data_headers_fhei_sz1_trap_sniff_code_bits_get(
    int unit);


uint32 dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_get(
    int unit);


uint32 dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_fhei_sz1_trap_sniff_table_nof
} dnx_data_headers_fhei_sz1_trap_sniff_table_e;









int dnx_data_headers_fhei_trap_sniff_legacy_feature_get(
    int unit,
    dnx_data_headers_fhei_trap_sniff_legacy_feature_e feature);



typedef enum
{
    dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset,
    dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits,
    dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset,
    dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits,

    
    _dnx_data_headers_fhei_trap_sniff_legacy_define_nof
} dnx_data_headers_fhei_trap_sniff_legacy_define_e;



uint32 dnx_data_headers_fhei_trap_sniff_legacy_code_offset_get(
    int unit);


uint32 dnx_data_headers_fhei_trap_sniff_legacy_code_bits_get(
    int unit);


uint32 dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_get(
    int unit);


uint32 dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_fhei_trap_sniff_legacy_table_nof
} dnx_data_headers_fhei_trap_sniff_legacy_table_e;









int dnx_data_headers_udh_feature_get(
    int unit,
    dnx_data_headers_udh_feature_e feature);



typedef enum
{
    dnx_data_headers_udh_define_data_type0_offset,
    dnx_data_headers_udh_define_data_type1_offset,
    dnx_data_headers_udh_define_data_type2_offset,
    dnx_data_headers_udh_define_data_type3_offset,
    dnx_data_headers_udh_define_data_offset,
    dnx_data_headers_udh_define_data_type0_bits,
    dnx_data_headers_udh_define_data_type1_bits,
    dnx_data_headers_udh_define_data_type2_bits,
    dnx_data_headers_udh_define_data_type3_bits,
    dnx_data_headers_udh_define_data_bits,
    dnx_data_headers_udh_define_base_size,

    
    _dnx_data_headers_udh_define_nof
} dnx_data_headers_udh_define_e;



uint32 dnx_data_headers_udh_data_type0_offset_get(
    int unit);


uint32 dnx_data_headers_udh_data_type1_offset_get(
    int unit);


uint32 dnx_data_headers_udh_data_type2_offset_get(
    int unit);


uint32 dnx_data_headers_udh_data_type3_offset_get(
    int unit);


uint32 dnx_data_headers_udh_data_offset_get(
    int unit);


uint32 dnx_data_headers_udh_data_type0_bits_get(
    int unit);


uint32 dnx_data_headers_udh_data_type1_bits_get(
    int unit);


uint32 dnx_data_headers_udh_data_type2_bits_get(
    int unit);


uint32 dnx_data_headers_udh_data_type3_bits_get(
    int unit);


uint32 dnx_data_headers_udh_data_bits_get(
    int unit);


uint32 dnx_data_headers_udh_base_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_udh_table_nof
} dnx_data_headers_udh_table_e;









int dnx_data_headers_ftmh_legacy_feature_get(
    int unit,
    dnx_data_headers_ftmh_legacy_feature_e feature);



typedef enum
{
    dnx_data_headers_ftmh_legacy_define_packet_size_offset,
    dnx_data_headers_ftmh_legacy_define_traffic_class_offset,
    dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset,
    dnx_data_headers_ftmh_legacy_define_pp_dsp_offset,
    dnx_data_headers_ftmh_legacy_define_drop_precedence_offset,
    dnx_data_headers_ftmh_legacy_define_tm_action_type_offset,
    dnx_data_headers_ftmh_legacy_define_otsh_en_offset,
    dnx_data_headers_ftmh_legacy_define_internal_header_en_offset,
    dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset,
    dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset,
    dnx_data_headers_ftmh_legacy_define_outlif_offset,
    dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset,
    dnx_data_headers_ftmh_legacy_define_cni_offset,
    dnx_data_headers_ftmh_legacy_define_ecn_enable_offset,
    dnx_data_headers_ftmh_legacy_define_exclude_source_offset,
    dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset,
    dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset,
    dnx_data_headers_ftmh_legacy_define_packet_size_bits,
    dnx_data_headers_ftmh_legacy_define_traffic_class_bits,
    dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits,
    dnx_data_headers_ftmh_legacy_define_pp_dsp_bits,
    dnx_data_headers_ftmh_legacy_define_drop_precedence_bits,
    dnx_data_headers_ftmh_legacy_define_tm_action_type_bits,
    dnx_data_headers_ftmh_legacy_define_otsh_en_bits,
    dnx_data_headers_ftmh_legacy_define_internal_header_en_bits,
    dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits,
    dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits,
    dnx_data_headers_ftmh_legacy_define_outlif_bits,
    dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits,
    dnx_data_headers_ftmh_legacy_define_cni_bits,
    dnx_data_headers_ftmh_legacy_define_ecn_enable_bits,
    dnx_data_headers_ftmh_legacy_define_exclude_source_bits,
    dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits,
    dnx_data_headers_ftmh_legacy_define_base_header_size,
    dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size,

    
    _dnx_data_headers_ftmh_legacy_define_nof
} dnx_data_headers_ftmh_legacy_define_e;



uint32 dnx_data_headers_ftmh_legacy_packet_size_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_traffic_class_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_pp_dsp_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_drop_precedence_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_tm_action_type_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_otsh_en_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_internal_header_en_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_outlif_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_cni_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_ecn_enable_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_exclude_source_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_packet_size_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_traffic_class_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_pp_dsp_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_drop_precedence_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_tm_action_type_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_otsh_en_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_internal_header_en_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_outlif_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_cni_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_ecn_enable_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_exclude_source_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_base_header_size_get(
    int unit);


uint32 dnx_data_headers_ftmh_legacy_dsp_ext_header_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_ftmh_legacy_table_nof
} dnx_data_headers_ftmh_legacy_table_e;









int dnx_data_headers_otsh_legacy_feature_get(
    int unit,
    dnx_data_headers_otsh_legacy_feature_e feature);



typedef enum
{
    dnx_data_headers_otsh_legacy_define_type_offset,
    dnx_data_headers_otsh_legacy_define_type_bits,
    dnx_data_headers_otsh_legacy_define_oam_sub_type_offset,
    dnx_data_headers_otsh_legacy_define_oam_sub_type_bits,
    dnx_data_headers_otsh_legacy_define_mep_type_offset,
    dnx_data_headers_otsh_legacy_define_mep_type_bits,
    dnx_data_headers_otsh_legacy_define_tp_command_offset,
    dnx_data_headers_otsh_legacy_define_tp_command_bits,
    dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset,
    dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits,
    dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset,
    dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits,
    dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset,
    dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits,
    dnx_data_headers_otsh_legacy_define_offset_offset,
    dnx_data_headers_otsh_legacy_define_offset_bits,

    
    _dnx_data_headers_otsh_legacy_define_nof
} dnx_data_headers_otsh_legacy_define_e;



uint32 dnx_data_headers_otsh_legacy_type_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_type_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_oam_sub_type_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_oam_sub_type_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_mep_type_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_mep_type_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_tp_command_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_tp_command_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_ts_encapsulation_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_ts_encapsulation_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_offset_offset_get(
    int unit);


uint32 dnx_data_headers_otsh_legacy_offset_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_otsh_legacy_table_nof
} dnx_data_headers_otsh_legacy_table_e;









int dnx_data_headers_system_headers_feature_get(
    int unit,
    dnx_data_headers_system_headers_feature_e feature);



typedef enum
{
    dnx_data_headers_system_headers_define_system_headers_mode_jericho,
    dnx_data_headers_system_headers_define_system_headers_mode_jericho2,
    dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len,
    dnx_data_headers_system_headers_define_system_headers_mode,
    dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode,
    dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode,
    dnx_data_headers_system_headers_define_udh_base_size,
    dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift,

    
    _dnx_data_headers_system_headers_define_nof
} dnx_data_headers_system_headers_define_e;



uint32 dnx_data_headers_system_headers_system_headers_mode_jericho_get(
    int unit);


uint32 dnx_data_headers_system_headers_system_headers_mode_jericho2_get(
    int unit);


uint32 dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_get(
    int unit);


uint32 dnx_data_headers_system_headers_system_headers_mode_get(
    int unit);


uint32 dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_get(
    int unit);


uint32 dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_get(
    int unit);


uint32 dnx_data_headers_system_headers_udh_base_size_get(
    int unit);


uint32 dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_system_headers_table_nof
} dnx_data_headers_system_headers_table_e;









int dnx_data_headers_otmh_feature_get(
    int unit,
    dnx_data_headers_otmh_feature_e feature);



typedef enum
{

    
    _dnx_data_headers_otmh_define_nof
} dnx_data_headers_otmh_define_e;




typedef enum
{

    
    _dnx_data_headers_otmh_table_nof
} dnx_data_headers_otmh_table_e;









int dnx_data_headers_pph_feature_get(
    int unit,
    dnx_data_headers_pph_feature_e feature);



typedef enum
{
    dnx_data_headers_pph_define_pph_vsi_selection_size,

    
    _dnx_data_headers_pph_define_nof
} dnx_data_headers_pph_define_e;



uint32 dnx_data_headers_pph_pph_vsi_selection_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_pph_table_nof
} dnx_data_headers_pph_table_e;









int dnx_data_headers_general_feature_get(
    int unit,
    dnx_data_headers_general_feature_e feature);



typedef enum
{

    
    _dnx_data_headers_general_define_nof
} dnx_data_headers_general_define_e;




typedef enum
{

    
    _dnx_data_headers_general_table_nof
} dnx_data_headers_general_table_e;









int dnx_data_headers_feature_feature_get(
    int unit,
    dnx_data_headers_feature_feature_e feature);



typedef enum
{
    dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size,

    
    _dnx_data_headers_feature_define_nof
} dnx_data_headers_feature_define_e;



uint32 dnx_data_headers_feature_fhei_mpls_swap_remark_profile_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_headers_feature_table_nof
} dnx_data_headers_feature_table_e;






shr_error_e dnx_data_headers_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

