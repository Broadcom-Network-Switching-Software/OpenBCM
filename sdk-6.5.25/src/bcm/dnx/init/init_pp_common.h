
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

#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET (0)
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET (1)
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET (2)

#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_SET(qualifier_p, segment_left_is_zero) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ZERO_OFFSET, segment_left_is_zero))
#define LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_SET(qualifier_p, segment_left_is_one) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SEGMENT_LEFT_IS_ONE_OFFSET, segment_left_is_one))
#define LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_SET(qualifier_p, single_pass_termination) (utilex_bitstream_set(qualifier_p, LAYER_QUALIFIER_SRV6_SINGLE_PASS_TERMINATION_OFFSET, single_pass_termination))

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_OFFSET (1)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SIZE (1)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_OFFSET (2)
#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SIZE (5)
#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_OFFSET (7)
#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SIZE (1)

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SET(qualifier_p, _first_add_header_exists_) \
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_OFFSET, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_EXISTS_SIZE, \
                                _first_add_header_exists_))

#define LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SET(qualifier_p, _first_add_header_) \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_OFFSET, \
                                LAYER_QUALIFIER_IPV6_1ST_ADD_HEADER_SIZE, \
                                _first_add_header_))

#define LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SET(qualifier_p, _srv6_sl0_single_pass_) \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(qualifier_p, \
                                LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_OFFSET, \
                                LAYER_QUALIFIER_IPV6_2ND_ADD_HEADER_SRV6_SINGLE_PASS_TERM_SIZE, \
                                _srv6_sl0_single_pass_))

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
