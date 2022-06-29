
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _INIT_PP_COMMON_INCLUDED__

#define _INIT_PP_COMMON_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX families only!"
#endif

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
    int is_bier_ti;
} per_layer_protocol_t;

typedef struct ecol_ffc_s
{
    uint8 opcode;
    uint8 index;
    uint16 field_offset;
    uint8 field_size;
    uint8 key_offset;
} ecol_ffc_t;

#define BUILD_ECOL_FFC_INSTRUCTION_32(ecol_ffc) ((ecol_ffc.opcode << 24) | (ecol_ffc.index << 21) \
                                  | (ecol_ffc.field_offset << 10) | (ecol_ffc.field_size << 5) \
                                  | (ecol_ffc.key_offset << 0))

#define BUILD_ECOL_FFC_INSTRUCTION_16(ecol_ffc) ((ecol_ffc.opcode << 22) | (ecol_ffc.index << 19) \
                                  | (ecol_ffc.field_offset << 8) | (ecol_ffc.field_size << 4) \
                                  | (ecol_ffc.key_offset << 0))

shr_error_e egress_eedb_type_buster_enable_init(
    int unit);

shr_error_e egress_eedb_forwarding_domain_vsd_enable_init(
    int unit);

shr_error_e egress_eedb_svtag_indication_enable_init(
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

shr_error_e dnx_parser_geneve_init(
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

shr_error_e bare_metal_configuration_init(
    int unit);

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
    int is_bier_mpls,
    int is_bier_ti);

shr_error_e dnx_pph_base_size_init(
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

shr_error_e egress_exclude_source_filter_configure_init(
    int unit);

shr_error_e ingress_is_ipv6_header_configuration_init(
    int unit);

#endif
