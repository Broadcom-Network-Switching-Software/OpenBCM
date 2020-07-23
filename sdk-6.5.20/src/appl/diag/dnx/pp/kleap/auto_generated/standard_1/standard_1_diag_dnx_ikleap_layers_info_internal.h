/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */
#ifndef STANDARD_1_DIAG_DNX_IKLEAP_LAYERS_INFO_INTERNAL_H_INCLUDED
#define STANDARD_1_DIAG_DNX_IKLEAP_LAYERS_INFO_INTERNAL_H_INCLUDED
#include <src/appl/diag/dnx/pp/kleap/diag_dnx_ikleap_layers_translation_to_pparse.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/* *INDENT-OFF* */

static kleap_layer_to_pparse_t *standard_1_vt1_header[DBAL_NOF_ENUM_VT1_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___RCH_TERM_BFIR*/
    &BIER_MPLS_header, &BIER_TI_header, &General_header, &General_header,
    /**BRIDGE___UNTAGGED*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_C_TAGS*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___C_C_TAGS*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_S_TAGS*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_S_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_S_C_TAGS*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_PRIORITY_C_TAGS*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___UNTAGGED_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___C_TAG_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_TAG_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_C_TAGS_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_TAG_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_C_TAG_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_S_TAG_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___E_S_C_TAGS_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___S_PRIORITY_C_TAGS_FRR*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_STC_C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_STC_S_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_STC_UNTAGGED*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_SP_C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_S_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_UNTAGGED*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_DROP_AND_CONT*/
    &RCH_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_JR_MODE*/
    &RCH_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_REFLECTOR*/
    &RCH_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_XTERMINATION*/
    &RCH_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &RCH_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**MPLS___RAW_MPLS*/
    &INITIALIZATION_header, &General_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_vt2_header[DBAL_NOF_ENUM_VT2_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___TT_P2P_MPLS*/
    &BIER_MPLS_header, &General_header, &General_header, &General_header,
    /**BIER___TT_P2P_TI*/
    &BIER_TI_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_VRF_by_VSI*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_VRF_by_VFT*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_VRF_by_Port*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION*/
    &General_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_P2P_Tunnel*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___2nd_Pass_TT_P2P_Tunnel*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_IPSEC*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_IPSEC_UDP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_SIP_MSBS*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_IPSEC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_IPSEC_UDP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**MAC_IN_MAC___Learn*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**MPLS___TT*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___2ND_PASS_TT*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_vt3_header[DBAL_NOF_ENUM_VT3_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___TT_MP_MPLS*/
    &BIER_MPLS_header, &General_header, &General_header, &General_header,
    /**BIER___TT_MP_TI*/
    &BIER_TI_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**GRE_With_Key___TNI2VRF_VTT3*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**GRE_With_Key___TNI2VSI_VTT3*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**IPV4___TT_MP_AND_TCAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___2nd_Pass_TT_MP_AND_TCAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_P2P_AND_TCAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_DIP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___2nd_Pass_TT_DIP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**MPLS___TT*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_FL_AFTER_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_FL_AND_CW_AFTER_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___SPECULATIVE_PARSING_FIX*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___P2P_Only*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**PPPoE___SA_COMPRESSION*/
    &PPPoE_header, &General_header, &General_header, &General_header,
    /**VxLAN_GPE___VNI2VSI_VTT3*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**VxLAN_GPE___VNI2VRF_VTT3*/
    &IPv4_header, &IPv6_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_vt4_header[DBAL_NOF_ENUM_VT4_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___2nd_Parsing*/
    &General_header, &General_header, &General_header, &General_header,
    /**GRE_With_Key___TNI2VRF_VTT4*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**GRE_With_Key___TNI2VSI_VTT4*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**IPV4___TT_P2P_Tunnel*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_MP_Tunnel*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_SIP_IDX_DIP_OR_DIP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_SIP_IDX_DIP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**L2TPv2___TT_WO_LENGTH*/
    &L2TP_header, &General_header, &General_header, &General_header,
    /**L2TPv2___TT_W_LENGTH*/
    &L2TP_header, &General_header, &General_header, &General_header,
    /**MAC_IN_MAC___ISID_WITHOUT_DOMAIN*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**MAC_IN_MAC___ISID_WITH_DOMAIN*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**MPLS___TT*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_FL_AFTER_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_FL_AND_CW_AFTER_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___P2P_Only*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**PPPoE___TT_CMPRS_SA*/
    &PPPoE_header, &General_header, &General_header, &General_header,
    /**PPPoE___TT_FULL_SA*/
    &PPPoE_header, &General_header, &General_header, &General_header,
    /**VxLAN_GPE___VNI2VSI_VTT4*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**VxLAN_GPE___VNI2VRF_VTT4*/
    &IPv4_header, &IPv6_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_vt5_header[DBAL_NOF_ENUM_VT5_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped_Learn_Native_AC*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped_Learn_Native_AC*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped_Learn_Native_AC*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped_Learn_Native_AC*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped_Learn_Native_AC*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped_Learn_Native_AC*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_OTHER*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_C_C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_S_S_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___PON_DTC_S_C_TAG*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**GRE_With_Key___TNI2VRF_VTT5*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**GRE_With_Key___TNI2VSI_VTT5*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**GTP___TT_NWK_Scoped_TEID*/
    &GTP_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_P2P_Tunnel*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_MP_Tunnel*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_IPSEC*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___TT_IPSEC_UDP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_IPSEC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___TT_IPSEC_UDP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**MPLS___TT*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___EVPN_IML*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___P2P_Only*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**PPPoE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK*/
    &PPPoE_header, &General_header, &General_header, &General_header,
    /**SRv6___Extended_Tunnel_terminate*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___One_Pass_Tunnel_terminate*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___One_Pass_Tunnel_terminate_ETH*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___uSID_EndPoint*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___Unified_end_point_SID_extract_0*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___Unified_end_point_SID_extract_1*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___Unified_end_point_SID_extract_2*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**VxLAN_GPE___VNI2VSI_VTT5*/
    &IPv4_header, &IPv6_header, &General_header, &General_header,
    /**VxLAN_GPE___VNI2VRF_VTT5*/
    &IPv4_header, &IPv6_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_fwd1_header[DBAL_NOF_ENUM_FWD1_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___TI_FORWARDING*/
    &BIER_TI_header, &General_header, &General_header, &General_header,
    /**BIER___MPLS_FORWARDING*/
    &BIER_MPLS_header, &General_header, &General_header, &General_header,
    /**BRIDGE___SVL*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___IPv4MC_SVL*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___IPv6MC_SVL*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_NO_VFT*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_NPV_NO_VFT*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_VFT*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_NPV_VFT*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___TRANSIT*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___TRANSIT_FIP*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX_MACT_SA_LKP*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private_Public*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private_Public_MACT_SA_LKP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private_MACT_SA_LKP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_Public_Optimized_UC*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_UC*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_Public_Optimized_UC_w_Options*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_UC_w_Options*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_Public_Optimized_UC_MACT_SA_LKP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_UC_MACT_SA_LKP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_Public_Optimized_UC_w_Options_MACT_SA_LKP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_UC_w_Options_MACT_SA_LKP*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___NAT_UPSTREAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___NAT_DOWNSTREAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private_Public*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private_Public_MACT_SA_LKP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private_MACT_SA_LKP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_Public_Optimized_UC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_UC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_Public_Optimized_UC_MACT_SA_LKP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_UC_MACT_SA_LKP*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**MPLS___FWD*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___FWD_MACT_SA_LKP*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___FWD_PER_INTERFACE*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___OAM_ONLY*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**SLLB___IPV4_ROUTE*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**SLLB___IPV6_ROUTE*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___Segment_end_point_forwarding*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___uSID_end_point_forwarding*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___uSID_NOP_CTX*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___Unified_end_point_forwarding*/
    &IPv6_header, &General_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_fwd2_header[DBAL_NOF_ENUM_FWD2_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BRIDGE___IPv4MC_SVL*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___IPv6MC_SVL*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**BRIDGE___SVL*/
    &ETHERNET_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**FCoE___FCF_FIP*/
    &FCoE_header, &General_header, &General_header, &General_header,
    /**GENERAL___NOP_CTX*/
    &General_header, &General_header, &General_header, &General_header,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private_W_BF_W_F2B*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private_WO_BF_W_F2B*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___MC_Private_WO_BF_WO_F2B*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_UC*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___Private_UC_LPM_Only*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___NAT_UPSTREAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV4___NAT_DOWNSTREAM*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private_W_BF_W_F2B*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private_WO_BF_W_F2B*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___MC_Private_WO_BF_WO_F2B*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_UC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_UC_LPM_Only*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**IPV6___Private_UC_BFD*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**MPLS___OAM_ONLY*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**MPLS___FWD*/
    &MPLS_header, &General_header, &General_header, &General_header,
    /**SLLB___VIRTUAL_IPV4_ROUTE*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**SLLB___VIRTUAL_IPV6_ROUTE*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SLLB___SIMPLE_IPV4_ROUTE*/
    &IPv4_header, &General_header, &General_header, &General_header,
    /**SLLB___SIMPLE_IPV6_ROUTE*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___Private_UC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___uSID_Private_UC*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**SRv6___uSID_NOP_CTX*/
    &IPv6_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_32*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_33*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_34*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_35*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_36*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_37*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_38*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_39*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_40*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_41*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_42*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_43*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_44*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_45*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_46*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_47*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_48*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_49*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_50*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_51*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_52*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_53*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_54*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_55*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_56*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_57*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_58*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_59*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_60*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_61*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_62*/
    &General_header, &General_header, &General_header, &General_header,
    /**ACL_CONTEXT_63*/
    &General_header, &General_header, &General_header, &General_header
};

static kleap_layer_to_pparse_t *standard_1_vt1_qualifier[DBAL_NOF_ENUM_VT1_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___RCH_TERM_BFIR*/
    &BIER_MPLS_layer, &BIER_TI_layer, &General_layer, &General_layer,
    /**BRIDGE___UNTAGGED*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_C_TAGS*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___C_C_TAGS*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_S_TAGS*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_S_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_S_C_TAGS*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_PRIORITY_C_TAGS*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___UNTAGGED_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___C_TAG_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_TAG_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_C_TAGS_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_TAG_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_C_TAG_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_S_TAG_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___E_S_C_TAGS_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___S_PRIORITY_C_TAGS_FRR*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_STC_C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_STC_S_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_STC_UNTAGGED*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_SP_C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_S_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_UNTAGGED*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_DROP_AND_CONT*/
    &RCH_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_JR_MODE*/
    &RCH_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_REFLECTOR*/
    &RCH_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_XTERMINATION*/
    &RCH_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &RCH_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___RAW_MPLS*/
    &INITIALIZATION_layer, &General_layer, &General_layer, &General_layer
};

static kleap_layer_to_pparse_t *standard_1_vt2_qualifier[DBAL_NOF_ENUM_VT2_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___TT_P2P_MPLS*/
    &BIER_MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**BIER___TT_P2P_TI*/
    &BIER_TI_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_VRF_by_VSI*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_VRF_by_VFT*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_VRF_by_Port*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_P2P_Tunnel*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___2nd_Pass_TT_P2P_Tunnel*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_IPSEC*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_IPSEC_UDP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_SIP_MSBS*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_IPSEC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_IPSEC_UDP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**MAC_IN_MAC___Learn*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___2ND_PASS_TT*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};

static kleap_layer_to_pparse_t *standard_1_vt3_qualifier[DBAL_NOF_ENUM_VT3_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___TT_MP_MPLS*/
    &BIER_MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**BIER___TT_MP_TI*/
    &BIER_TI_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GRE_With_Key___TNI2VRF_VTT3*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**GRE_With_Key___TNI2VSI_VTT3*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**IPV4___TT_MP_AND_TCAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___2nd_Pass_TT_MP_AND_TCAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_P2P_AND_TCAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_DIP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___2nd_Pass_TT_DIP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_FL_AFTER_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_FL_AND_CW_AFTER_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___SPECULATIVE_PARSING_FIX*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___P2P_Only*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**PPPoE___SA_COMPRESSION*/
    &PPPoE_layer, &General_layer, &General_layer, &General_layer,
    /**VxLAN_GPE___VNI2VSI_VTT3*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**VxLAN_GPE___VNI2VRF_VTT3*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer
};

static kleap_layer_to_pparse_t *standard_1_vt4_qualifier[DBAL_NOF_ENUM_VT4_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___2nd_Parsing*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GRE_With_Key___TNI2VRF_VTT4*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**GRE_With_Key___TNI2VSI_VTT4*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**IPV4___TT_P2P_Tunnel*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_MP_Tunnel*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_SIP_IDX_DIP_OR_DIP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_SIP_IDX_DIP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**L2TPv2___TT_WO_LENGTH*/
    &L2TP_layer, &General_layer, &General_layer, &General_layer,
    /**L2TPv2___TT_W_LENGTH*/
    &L2TP_layer, &General_layer, &General_layer, &General_layer,
    /**MAC_IN_MAC___ISID_WITHOUT_DOMAIN*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**MAC_IN_MAC___ISID_WITH_DOMAIN*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_FL_AFTER_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_FL_AND_CW_AFTER_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___P2P_Only*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**PPPoE___TT_CMPRS_SA*/
    &PPPoE_layer, &General_layer, &General_layer, &General_layer,
    /**PPPoE___TT_FULL_SA*/
    &PPPoE_layer, &General_layer, &General_layer, &General_layer,
    /**VxLAN_GPE___VNI2VSI_VTT4*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**VxLAN_GPE___VNI2VRF_VTT4*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer
};

static kleap_layer_to_pparse_t *standard_1_vt5_qualifier[DBAL_NOF_ENUM_VT5_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped_Learn_Native_AC*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped_Learn_Native_AC*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped_Learn_Native_AC*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped_Learn_Native_AC*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped_Learn_Native_AC*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped_Learn_Native_AC*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_OTHER*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_C_C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_S_S_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___PON_DTC_S_C_TAG*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GRE_With_Key___TNI2VRF_VTT5*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**GRE_With_Key___TNI2VSI_VTT5*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**GTP___TT_NWK_Scoped_TEID*/
    &GTP_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_P2P_Tunnel*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_MP_Tunnel*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_IPSEC*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___TT_IPSEC_UDP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_IPSEC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___TT_IPSEC_UDP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___TT_PER_INTERFACE_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___EVPN_IML*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___P2P_Only*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___SPECIAL_LABEL*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**PPPoE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK*/
    &PPPoE_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Extended_Tunnel_terminate*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___One_Pass_Tunnel_terminate*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___One_Pass_Tunnel_terminate_ETH*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___uSID_EndPoint*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Unified_end_point_SID_extract_0*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Unified_end_point_SID_extract_1*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Unified_end_point_SID_extract_2*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**VxLAN_GPE___VNI2VSI_VTT5*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer,
    /**VxLAN_GPE___VNI2VRF_VTT5*/
    &IPv4_layer, &IPv6_layer, &General_layer, &General_layer
};

static kleap_layer_to_pparse_t *standard_1_fwd1_qualifier[DBAL_NOF_ENUM_FWD1_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BIER___TI_FORWARDING*/
    &BIER_TI_layer, &General_layer, &General_layer, &General_layer,
    /**BIER___MPLS_FORWARDING*/
    &BIER_MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___SVL*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___IPv4MC_SVL*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___IPv6MC_SVL*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_NO_VFT*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_NPV_NO_VFT*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_VFT*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_NPV_VFT*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___TRANSIT*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___TRANSIT_FIP*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX_MACT_SA_LKP*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private_Public*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private_Public_MACT_SA_LKP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private_MACT_SA_LKP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_Public_Optimized_UC*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_UC*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_Public_Optimized_UC_w_Options*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_UC_w_Options*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_Public_Optimized_UC_MACT_SA_LKP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_UC_MACT_SA_LKP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_Public_Optimized_UC_w_Options_MACT_SA_LKP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_UC_w_Options_MACT_SA_LKP*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___NAT_UPSTREAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___NAT_DOWNSTREAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private_Public*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private_Public_MACT_SA_LKP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private_MACT_SA_LKP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_Public_Optimized_UC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_UC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_Public_Optimized_UC_MACT_SA_LKP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_UC_MACT_SA_LKP*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___FWD*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___FWD_MACT_SA_LKP*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___FWD_PER_INTERFACE*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___OAM_ONLY*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**SLLB___IPV4_ROUTE*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**SLLB___IPV6_ROUTE*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Segment_end_point_forwarding*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___uSID_end_point_forwarding*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___uSID_NOP_CTX*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Unified_end_point_forwarding*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};

static kleap_layer_to_pparse_t *standard_1_fwd2_qualifier[DBAL_NOF_ENUM_FWD2_CONTEXT_ID_VALUES * KLEAP_NOF_RELATIVE_LAYERS] = {
    /**BRIDGE___IPv4MC_SVL*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___IPv6MC_SVL*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**BRIDGE___SVL*/
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**FCoE___FCF_FIP*/
    &FCoE_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___NOP_CTX*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**GENERAL___RCH_TERM_EXT_ENCAP*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private_W_BF_W_F2B*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private_WO_BF_W_F2B*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___MC_Private_WO_BF_WO_F2B*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_UC*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___Private_UC_LPM_Only*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___NAT_UPSTREAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV4___NAT_DOWNSTREAM*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private_W_BF_W_F2B*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private_WO_BF_W_F2B*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___MC_Private_WO_BF_WO_F2B*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_UC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_UC_LPM_Only*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**IPV6___Private_UC_BFD*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___OAM_ONLY*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**MPLS___FWD*/
    &MPLS_layer, &General_layer, &General_layer, &General_layer,
    /**SLLB___VIRTUAL_IPV4_ROUTE*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**SLLB___VIRTUAL_IPV6_ROUTE*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SLLB___SIMPLE_IPV4_ROUTE*/
    &IPv4_layer, &General_layer, &General_layer, &General_layer,
    /**SLLB___SIMPLE_IPV6_ROUTE*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___Private_UC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___uSID_Private_UC*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**SRv6___uSID_NOP_CTX*/
    &IPv6_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_32*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_33*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_34*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_35*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_36*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_37*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_38*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_39*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_40*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_41*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_42*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_43*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_44*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_45*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_46*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_47*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_48*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_49*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_50*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_51*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_52*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_53*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_54*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_55*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_56*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_57*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_58*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_59*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_60*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_61*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_62*/
    &General_layer, &General_layer, &General_layer, &General_layer,
    /**ACL_CONTEXT_63*/
    &General_layer, &General_layer, &General_layer, &General_layer
};

#endif
/* *INDENT-ON* */
