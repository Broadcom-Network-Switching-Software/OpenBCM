
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _INIT_PP_COMMON_INCLUDED__

#define _INIT_PP_COMMON_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX families only!"
#endif

shr_error_e egress_eedb_type_buster_enable_init(
    int unit);

shr_error_e egress_eedb_forwarding_domain_vsd_enable_init(
    int unit);

shr_error_e egress_eedb_svtag_indication_enable_init(
    int unit);

shr_error_e ingress_prt_profile_configuration_init(
    int unit);

shr_error_e ingress_lbp_vlan_editing_configuration_init(
    int unit);

shr_error_e ingress_ippe_parser_context_mapping(
    int unit);

shr_error_e ingress_ippb_l4_protocols_config(
    int unit);

shr_error_e ingress_port_general_configuration_init(
    int unit);

shr_error_e ingress_da_type_map_init(
    int unit);

shr_error_e ingress_stat_fec_ecmp_mapping_init(
    int unit);

shr_error_e ire_packet_generator_init(
    int unit);

shr_error_e ingress_egress_per_layer_protocol_configuration_init(
    int unit);

shr_error_e ingress_udp_next_protocol_enable_init(
    int unit);

shr_error_e dnx_pph_base_size_init(
    int unit);

shr_error_e ingress_ecologic_srv6_segment_endpoint_init(
    int unit);

shr_error_e ingress_ecologic_srv6_egress_tunnel_1_pass_init(
    int unit);

shr_error_e ingress_ecologic_srv6_egress_tunnel_2_pass_init(
    int unit);

shr_error_e ingress_ecologic_init(
    int unit);

shr_error_e ingress_itpp_general_network_header_termination_init(
    int unit);

shr_error_e egress_cfg_in_lif_null_value_init(
    int unit);

shr_error_e egress_ingress_trapped_by_fhei_configure_init(
    int unit);

shr_error_e egress_fallback_to_bridge_configuration(
    int unit,
    uint32 fwd_additional_info,
    uint8 ipvx_fallback_to_bridge_enable);

shr_error_e ingress_forwarding_domain_assignment_mode_init(
    int unit);

shr_error_e egress_mapping_forward_context_configure_init(
    int unit);

shr_error_e egress_port_outlif_profile_configuration_init(
    int unit);

shr_error_e ingress_prt_recycle_command_configuration_init(
    int unit);

#endif
