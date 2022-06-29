/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */
#ifndef STANDARD_1_DIAG_DNX2_IKLEAP_LAYERS_INFO_INTERNAL_H_INCLUDED
#define STANDARD_1_DIAG_DNX2_IKLEAP_LAYERS_INFO_INTERNAL_H_INCLUDED

#ifdef BCM_DNX_SUPPORT

#include <src/appl/diag/dnx/pp/kleap/diag_dnx_ikleap_layers_translation_to_pparse.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/* *INDENT-OFF* */

    kleap_layer_to_pparse_t *standard_1_dummy_headers_layers[] = {
    &General_header,
    &FCoE_header,
    &PPPoE_header,
    &L2TP_header,
    &GTP_header,
    &BIER_NON_MPLS_header,
    &ETHERNET_header,
    &RCH_header,
    &SRV6_RCH_USP_PSP_AND_PSP_EXT_header,
    &IPv4_header,
    &IPv6_header,
    &MPLS_header,
    &INITIALIZATION_header,
    &UDP_header,
    &General_layer,
    &ETHERNET_layer,
    &RCH_layer,
    &SRV6_RCH_USP_PSP_AND_PSP_EXT_layer,
    &PPPoE_layer,
    &L2TP_layer,
    &GTP_layer,
    &FCoE_layer,
    &MPLS_layer,
    &BIER_NON_MPLS_layer,
    &IPv4_layer,
    &IPv6_layer,
    &INITIALIZATION_layer,
    &UDP_layer
};

static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___UNTAGGED_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_C_TAGS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___C_C_TAGS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_S_TAGS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_C_TAGS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_PRIORITY_C_TAGS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___UNTAGGED_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___C_TAG_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_TAG_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_C_TAGS_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_TAG_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_C_TAG_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_TAG_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_C_TAGS_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_PRIORITY_C_TAGS_FRR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_STC_C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_STC_S_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_STC_UNTAGGED_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_SP_C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_S_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_UNTAGGED_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_DROP_AND_CONT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &RCH_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_JR_MODE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &RCH_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_REFLECTOR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &RCH_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_XTERMINATION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &RCH_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_EXT_ENCAP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &RCH_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____MPLS___RAW_MPLS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &INITIALIZATION_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____SRv6___RCH_SRv6_USP_PSP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &SRV6_RCH_USP_PSP_AND_PSP_EXT_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___FTMH_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____vt2_____BIER___BFER_2nd_PASS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____FCoE___FCF_VRF_by_VSI_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &FCoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____FCoE___FCF_VRF_by_VFT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &FCoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____FCoE___FCF_VRF_by_Port_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &FCoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___RCH_TERM_EXT_ENCAP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___MAC_SA_COMPRESSION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___TT_P2P_Tunnel_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___2nd_Pass_TT_P2P_Tunnel_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV6___TT_SIP_MSBS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MAC_IN_MAC___Learn_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___TT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___TT_PER_INTERFACE_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___2ND_PASS_TT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___EVPN_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___SPECIAL_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____BRIDGE___NOP_IVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &UDP_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV6___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV6___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &UDP_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____vt3_____BNG___PPPoE_SA_COMPRESSION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &PPPoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___RCH_TERM_EXT_ENCAP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___IPSEC_PARSING_CONTEXT_FIX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___IPV4_MAC_SA_COMPRESSION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENEVE___VNI2VSI_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENEVE___VNI2VRF_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GRE_With_Key___TNI2VRF_VTT3_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GRE_With_Key___TNI2VSI_VTT3_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_MP_AND_TCAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___2nd_Pass_TT_MP_AND_TCAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_P2P_AND_TCAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___VRF_RESELECT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_DIP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___2nd_Pass_TT_DIP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_DIP_W_IPv6_EH_PARSING_CONTEXT_FIX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___2nd_Pass_TT_DIP_W_IPv6_EH_PARSING_CONTEXT_FIX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___IPv6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___TT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___TT_PER_INTERFACE_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___EVPN_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___EVPN_FL_AFTER_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___EVPN_FL_AND_CW_AFTER_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___SPECULATIVE_PARSING_FIX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___P2P_Only_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___SPECIAL_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____VxLAN_GPE___VNI2VSI_VTT3_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____VxLAN_GPE___VNI2VRF_VTT3_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____BRIDGE___NOP_IVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___IFIT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &UDP_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_IPSEC_Double_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &UDP_header, &UDP_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &UDP_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___PPPoE_TT_CMPRS_SA_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &PPPoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___PPPoE_TT_FULL_SA_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &PPPoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___L2TP_TT_WO_LENGTH_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &L2TP_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___L2TP_TT_W_LENGTH_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &L2TP_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENERAL___2nd_Parsing_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENEVE___VNI2VSI_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENEVE___VNI2VRF_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GRE_With_Key___TNI2VRF_VTT4_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GRE_With_Key___TNI2VSI_VTT4_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_P2P_Tunnel_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_MP_Tunnel_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_SIP_IDX_DIP_OR_DIP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_SIP_IDX_DIP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_32_USID_16_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_48_USID_16_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_48_GSID_16_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_64_GSID_16_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_48_USID_16_NO_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITHOUT_DOMAIN_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITH_DOMAIN_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___TT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___TT_PER_INTERFACE_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___EVPN_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___EVPN_FL_AFTER_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___EVPN_FL_AND_CW_AFTER_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___P2P_Only_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___SPECIAL_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____VxLAN_GPE___VNI2VSI_VTT4_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____VxLAN_GPE___VNI2VRF_VTT4_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___IFIT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &UDP_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &UDP_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____vt5_____BNG___PPPoE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &PPPoE_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped_Learn_Native_AC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped_Learn_Native_AC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped_Learn_Native_AC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped_Learn_Native_AC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped_Learn_Native_AC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped_Learn_Native_AC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_OTHER_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_C_C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_S_S_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_S_C_TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_Specific_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENERAL___IPSEC_LAYER_FIX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VSI_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VRF_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VSIoIPv6_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VRFoIPv6_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GRE_With_Key___TNI2VRF_VTT5_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GRE_With_Key___TNI2VSI_VTT5_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_P2P_Tunnel_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_MP_Tunnel_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___TT_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___TT_PER_INTERFACE_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___EVPN_IML_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___P2P_Only_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___SPECIAL_LABEL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___GAL_WITH_FLOW_LABEL_FIX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &MPLS_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___Extended_Tunnel_terminate_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___Classic_EndPoint_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___uSID_EndPoint_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_0_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_1_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_2_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_3_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____UPF___TT_NWK_Scoped_TEID_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &GTP_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____VxLAN_GPE___VNI2VSI_VTT5_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____VxLAN_GPE___VNI2VRF_VTT5_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &UDP_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV6___TT_IPSEC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV6___TT_IPSEC_UDP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &UDP_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____fwd1_____BIER___NON_MPLS_BFR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &BIER_NON_MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BIER___NON_MPLS_BFR_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &BIER_NON_MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPv4_SIP_ADDR_CK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPv6_SIP_ADDR_CK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_MACT_SA_LKP__W_IPv4_SIP_ADDR_CK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_MACT_SA_LKP__W_IPv6_SIP_ADDR_CK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv4MC_FORWARD_LKP__W_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv4_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPv4_SIP_ADDR_CK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv4_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv6MC_FORWARD_LKP__W_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv6_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPv6_SIP_ADDR_CK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv6_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___EXTENDED_P2P_0TAG_LP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___EXTENDED_P2P_1TAG_LP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____GENERAL___NOP_CTX_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____GENERAL___RCH_TERM_EXT_ENCAP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_Public_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_Public_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_w_Options_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_w_Options_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_w_Options_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_w_Options_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___NOP_CTX_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_Public_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_Public_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_Public_Optimized_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_Public_Optimized_UC_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_UC_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___NOP_CTX_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___FWD_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___FWD_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___FWD_PER_INTERFACE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___OAM_ONLY_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____OAM___BRIDGE_TRUNK_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SLLB___IPV4_ROUTE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SLLB___IPV6_ROUTE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___Segment_end_point_forwarding_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___Classic_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_end_point_forwarding_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_NO_DEFAULT_end_point_forwarding_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_NO_DEFAULT_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_48b_Prefix_end_point_forwarding_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_48b_Prefix_NO_DEFAULT_end_point_forwarding_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_0_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_1_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_2_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_3_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___IVL__W_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___IVL__W_IPv4MC_FORWARD_LKP__W_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv4_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___IVL__W_IPv6MC_FORWARD_LKP__W_MACT_SA_LKP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv6_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___NAT_UPSTREAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___NAT_DOWNSTREAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____fwd2_____BIER___NON_MPLS_BFR_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &BIER_NON_MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv4MC_SVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv4_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv6MC_SVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv6_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___SVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_0TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_1TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_2TAG_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____GENERAL___NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____GENERAL___RCH_TERM_EXT_ENCAP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &UNKNOWN_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___MC_Private_W_BF_W_F2B_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___MC_Private_WO_BF_W_F2B_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___MC_Private_WO_BF_WO_F2B_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___Private_UC_LPM_Only_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___WO_FIB_LOOKUP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___MC_Private_W_BF_W_F2B_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___MC_Private_WO_BF_W_F2B_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___MC_Private_WO_BF_WO_F2B_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___Private_UC_LPM_Only_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___Private_UC_BFD_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___WO_FIB_LOOKUP_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____MPLS___OAM_ONLY_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____MPLS___FWD_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &MPLS_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___VIRTUAL_IPV4_ROUTE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___VIRTUAL_IPV6_ROUTE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___SIMPLE_IPV4_ROUTE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___SIMPLE_IPV6_ROUTE_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___Classic_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_NO_DEFAULT_Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_NO_DEFAULT_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___GSID_NOP_CTX_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___GSID_Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___GSID_64b_Prefix_Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_48b_Prefix_Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_48b_Prefix_NO_DEFAULT_Private_UC_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv6_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_48_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_49_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_50_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_51_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_52_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_53_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_54_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_55_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_56_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_57_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_58_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_59_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_60_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_61_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_62_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_63_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &General_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv4MC_IVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv4_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv6MC_IVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &IPv6_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IVL_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &ETHERNET_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___NAT_UPSTREAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___NAT_DOWNSTREAM_header[KLEAP_NOF_RELATIVE_LAYERS] = 
{
    &IPv4_header, &General_header, &General_header, &General_header
};


static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___UNTAGGED_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_C_TAGS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___C_C_TAGS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_S_TAGS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_C_TAGS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_PRIORITY_C_TAGS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___UNTAGGED_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___C_TAG_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_TAG_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_C_TAGS_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_TAG_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_C_TAG_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_TAG_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___E_S_C_TAGS_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___S_PRIORITY_C_TAGS_FRR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_STC_C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_STC_S_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_STC_UNTAGGED_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_SP_C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_S_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PON_DTC_UNTAGGED_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_DROP_AND_CONT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &RCH_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_JR_MODE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &RCH_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_REFLECTOR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &RCH_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_XTERMINATION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &RCH_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___RCH_TERM_EXT_ENCAP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &RCH_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____MPLS___RAW_MPLS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &INITIALIZATION_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____SRv6___RCH_SRv6_USP_PSP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &SRV6_RCH_USP_PSP_AND_PSP_EXT_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt1_____GENERAL___FTMH_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};


static kleap_layer_to_pparse_t *standard_1_____vt2_____BIER___BFER_2nd_PASS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____FCoE___FCF_VRF_by_VSI_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &FCoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____FCoE___FCF_VRF_by_VFT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &FCoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____FCoE___FCF_VRF_by_Port_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &FCoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___RCH_TERM_EXT_ENCAP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____GENERAL___MAC_SA_COMPRESSION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___TT_P2P_Tunnel_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___2nd_Pass_TT_P2P_Tunnel_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV6___TT_SIP_MSBS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MAC_IN_MAC___Learn_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___TT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___TT_PER_INTERFACE_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___2ND_PASS_TT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___EVPN_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____MPLS___SPECIAL_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____BRIDGE___NOP_IVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV4___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &UDP_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV6___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt2_____IPV6___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &UDP_layer, &General_layer, &General_layer
};


static kleap_layer_to_pparse_t *standard_1_____vt3_____BNG___PPPoE_SA_COMPRESSION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &PPPoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___RCH_TERM_EXT_ENCAP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___IPSEC_PARSING_CONTEXT_FIX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENERAL___IPV4_MAC_SA_COMPRESSION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENEVE___VNI2VSI_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GENEVE___VNI2VRF_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GRE_With_Key___TNI2VRF_VTT3_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____GRE_With_Key___TNI2VSI_VTT3_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_MP_AND_TCAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___2nd_Pass_TT_MP_AND_TCAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_P2P_AND_TCAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___VRF_RESELECT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_DIP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___2nd_Pass_TT_DIP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_DIP_W_IPv6_EH_PARSING_CONTEXT_FIX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___2nd_Pass_TT_DIP_W_IPv6_EH_PARSING_CONTEXT_FIX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___IPv6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___TT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___TT_PER_INTERFACE_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___EVPN_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___EVPN_FL_AFTER_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___EVPN_FL_AND_CW_AFTER_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___SPECULATIVE_PARSING_FIX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___P2P_Only_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___SPECIAL_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____VxLAN_GPE___VNI2VSI_VTT3_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____VxLAN_GPE___VNI2VRF_VTT3_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____BRIDGE___NOP_IVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____MPLS___IFIT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &UDP_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV4___TT_IPSEC_Double_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &UDP_layer, &UDP_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt3_____IPV6___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &UDP_layer, &General_layer, &General_layer
};


static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___PPPoE_TT_CMPRS_SA_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &PPPoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___PPPoE_TT_FULL_SA_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &PPPoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___L2TP_TT_WO_LENGTH_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &L2TP_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BNG___L2TP_TT_W_LENGTH_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &L2TP_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENERAL___2nd_Parsing_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENEVE___VNI2VSI_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GENEVE___VNI2VRF_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GRE_With_Key___TNI2VRF_VTT4_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____GRE_With_Key___TNI2VSI_VTT4_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_P2P_Tunnel_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_MP_Tunnel_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_SIP_IDX_DIP_OR_DIP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_SIP_IDX_DIP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_32_USID_16_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_48_USID_16_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_48_GSID_16_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_64_GSID_16_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_USID_PREFIX_48_USID_16_NO_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITHOUT_DOMAIN_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITH_DOMAIN_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___TT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___TT_PER_INTERFACE_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___EVPN_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___EVPN_FL_AFTER_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___EVPN_FL_AND_CW_AFTER_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___P2P_Only_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___SPECIAL_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____VxLAN_GPE___VNI2VSI_VTT4_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____VxLAN_GPE___VNI2VRF_VTT4_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____MPLS___IFIT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV4___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &UDP_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt4_____IPV6___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &UDP_layer, &General_layer, &General_layer
};


static kleap_layer_to_pparse_t *standard_1_____vt5_____BNG___PPPoE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &PPPoE_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_NWK_Scoped_Learn_Native_AC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_NWK_Scoped_Learn_Native_AC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_NWK_Scoped_Learn_Native_AC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_0_VTAG_LIF_Scoped_Learn_Native_AC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_1_VTAG_LIF_Scoped_Learn_Native_AC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___Inner_Ethernet_2_VTAG_LIF_Scoped_Learn_Native_AC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_OTHER_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_C_C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_S_S_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PON_DTC_S_C_TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_Specific_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENERAL___IPSEC_LAYER_FIX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VSI_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VRF_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VSIoIPv6_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GENEVE___VNI2VRFoIPv6_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GRE_With_Key___TNI2VRF_VTT5_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____GRE_With_Key___TNI2VSI_VTT5_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_P2P_Tunnel_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_MP_Tunnel_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___TT_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___TT_PER_INTERFACE_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___EVPN_IML_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___P2P_Only_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___SPECIAL_LABEL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____MPLS___GAL_WITH_FLOW_LABEL_FIX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &MPLS_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___Extended_Tunnel_terminate_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___Classic_EndPoint_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___uSID_EndPoint_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_0_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_1_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_2_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____SRv6___GSID_endpoint_SID_extract_3_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____UPF___TT_NWK_Scoped_TEID_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &GTP_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____VxLAN_GPE___VNI2VSI_VTT5_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____VxLAN_GPE___VNI2VRF_VTT5_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV4___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &UDP_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV6___TT_IPSEC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____vt5_____IPV6___TT_IPSEC_UDP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &UDP_layer, &General_layer, &General_layer
};


static kleap_layer_to_pparse_t *standard_1_____fwd1_____BIER___NON_MPLS_BFR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &BIER_NON_MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BIER___NON_MPLS_BFR_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &BIER_NON_MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPv4_SIP_ADDR_CK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPv6_SIP_ADDR_CK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_MACT_SA_LKP__W_IPv4_SIP_ADDR_CK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_MACT_SA_LKP__W_IPv6_SIP_ADDR_CK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv4MC_FORWARD_LKP__W_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv4_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPv4_SIP_ADDR_CK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv4_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv6MC_FORWARD_LKP__W_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv6_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___SVL__W_IPv6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPv6_SIP_ADDR_CK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv6_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___EXTENDED_P2P_0TAG_LP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___EXTENDED_P2P_1TAG_LP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____GENERAL___NOP_CTX_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____GENERAL___RCH_TERM_EXT_ENCAP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_Public_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_Public_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___MC_Private_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_w_Options_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_w_Options_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_Public_Optimized_UC_w_Options_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___Private_UC_w_Options_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___NOP_CTX_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_Public_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_Public_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___MC_Private_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_Public_Optimized_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_Public_Optimized_UC_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___Private_UC_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV6___NOP_CTX_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___FWD_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___FWD_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___FWD_PER_INTERFACE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____MPLS___OAM_ONLY_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____OAM___BRIDGE_TRUNK_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SLLB___IPV4_ROUTE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SLLB___IPV6_ROUTE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___Segment_end_point_forwarding_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___Classic_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_end_point_forwarding_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_NO_DEFAULT_end_point_forwarding_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_NO_DEFAULT_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_48b_Prefix_end_point_forwarding_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___uSID_48b_Prefix_NO_DEFAULT_end_point_forwarding_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_0_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_1_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_2_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____SRv6___GSID_endpoint_forwarding_SID_extract_3_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___IVL__W_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___IVL__W_IPv4MC_FORWARD_LKP__W_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv4_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____BRIDGE___IVL__W_IPv6MC_FORWARD_LKP__W_MACT_SA_LKP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv6_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___NAT_UPSTREAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd1_____IPV4___NAT_DOWNSTREAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};


static kleap_layer_to_pparse_t *standard_1_____fwd2_____BIER___NON_MPLS_BFR_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &BIER_NON_MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv4MC_SVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv4_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv6MC_SVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv6_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___SVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_0TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_1TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_2TAG_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___EXTENDED_P2P_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____GENERAL___NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____GENERAL___RCH_TERM_EXT_ENCAP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &UNKNOWN_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___MC_Private_W_BF_W_F2B_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___MC_Private_WO_BF_W_F2B_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___MC_Private_WO_BF_WO_F2B_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___Private_UC_LPM_Only_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___WO_FIB_LOOKUP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___MC_Private_W_BF_W_F2B_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___MC_Private_WO_BF_W_F2B_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___MC_Private_WO_BF_WO_F2B_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___Private_UC_LPM_Only_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___Private_UC_BFD_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV6___WO_FIB_LOOKUP_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____MPLS___OAM_ONLY_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____MPLS___FWD_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &MPLS_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___VIRTUAL_IPV4_ROUTE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___VIRTUAL_IPV6_ROUTE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___SIMPLE_IPV4_ROUTE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SLLB___SIMPLE_IPV6_ROUTE_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___Classic_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_NO_DEFAULT_Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_NO_DEFAULT_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___GSID_NOP_CTX_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___GSID_Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___GSID_64b_Prefix_Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_48b_Prefix_Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____SRv6___uSID_48b_Prefix_NO_DEFAULT_Private_UC_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv6_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_48_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_49_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_50_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_51_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_52_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_53_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_54_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_55_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_56_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_57_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_58_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_59_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_60_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_61_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_62_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____ACL_CONTEXT_63_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &General_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv4MC_IVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv4_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IPv6MC_IVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &IPv6_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____BRIDGE___IVL_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &ETHERNET_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___NAT_UPSTREAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};
static kleap_layer_to_pparse_t *standard_1_____fwd2_____IPV4___NAT_DOWNSTREAM_qualifier[KLEAP_NOF_RELATIVE_LAYERS] = {
    &IPv4_layer, &General_layer, &General_layer, &General_layer
};



#endif /* BCM_DNX_SUPPORT */
#endif
/* *INDENT-ON* */
