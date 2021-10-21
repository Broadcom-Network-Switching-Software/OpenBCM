
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

#define ADDITIONAL_HEADERS_NONE                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE

#define ADDITIONAL_HEADERS_CW_ESI_EVPN          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI_CW

#define ADDITIONAL_HEADERS_ESI_EVPN             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI

#define ADDITIONAL_HEADERS_CW_FL_ESI_EVPN       DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFLOESI

#define ADDITIONAL_HEADERS_FL_ESI_EVPN          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FLOESI

#define ADDITIONAL_HEADERS_CW_EL_ELI_PWE        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOELOELI

#define ADDITIONAL_HEADERS_CW_FL_PWE            DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFL

#define ADDITIONAL_HEADERS_EL_ELI               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ELOELI

#define ADDITIONAL_HEADERS_CW_PWE               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CW

#define ADDITIONAL_HEADERS_FL_PWE               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FL

#define ADDITIONAL_HEADERS_VXLAN_GPE_UDP        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE

#define ADDITIONAL_HEADERS_GENEVE_UDP           DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GENEVE

#define ADDITIONAL_HEADERS_VXLAN_UDP            DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN

#define ADDITIONAL_HEADERS_GRE4                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE4

#define ADDITIONAL_HEADERS_WO_TNI_GRE8          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_KEY

#define ADDITIONAL_HEADERS_TNI_GRE8             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V4

#define ADDITIONAL_HEADERS_UDP                  DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP

#define ADDITIONAL_HEADERS_GRE12                DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE12

#define ADDITIONAL_HEADERS_IPV6_TNI_GRE8        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V6

#define ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN    DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_SN

#define ADDITIONAL_HEADERS_VXLAN_DOUBLE_UDP_ESP DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_DOUBLE_UDP_ESP

typedef struct
{

    int layer_type_enum_val;

    int is_ethernet;
    int is_arp;
    int is_ipv4;
    int is_ipv6;
    int is_mpls_term;
    int is_mpls_fwd;
    int is_oam;
    int is_udp;
    int is_bfd_single_hop;
    int is_bfd_multi_hop;
    int is_tcp;
    int is_icmp;
    int is_icmpv6;
    int is_8021_x_2;
    int is_igmp;
    int is_bier_mpls;
} per_layer_protocol_t;

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

shr_error_e ingress_udp_next_protocol_enable_init(
    int unit);

shr_error_e egress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ipv4,
    int is_ipv6,
    int is_mpls,
    int is_oam,
    int is_tcp,
    int is_udp);

shr_error_e ingress_layer_protocol_allow_speculative_learning(
    int unit,
    int layer_entry,
    int enable);

shr_error_e ingress_per_layer_protocol_configuration(
    int unit,
    int layer_type_enum_val,
    int is_ethernet,
    int is_arp,
    int is_ipv4,
    int is_ipv6,
    int is_mpls_term,
    int is_mpls_fwd,
    int is_udp,
    int is_bfd_single_hop,
    int is_bfd_multi_hop,
    int is_icmpv6,
    int is_igmp,
    int is_8021_x_2,
    int is_icmp,
    int is_bier_mpls);

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

shr_error_e ingress_egress_parser_configuration_init(
    int unit);

shr_error_e ingress_gre_next_protocol_eth_init(
    int unit);

shr_error_e egress_additional_headers_map_tables_configuration_init(
    int unit);

shr_error_e egress_exclude_source_filter_configure_init(
    int unit);
#endif
