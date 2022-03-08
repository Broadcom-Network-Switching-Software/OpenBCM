/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#if defined (BCM_DNX2_SUPPORT)

#include <shared/utilex/utilex_str.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <src/bcm/dnx/algo/lif_mngr/auto_generated/standard_1_dbx_pre_compiled_lif_mdb_xml_parser.h>

#define _INTERNAL_AUTO_GENERATED_ARRAYS_MARKER
#define _INTERNAL_AUTO_GENERATED_FILE_SEARCH

/* *INDENT-OFF* */
_INTERNAL_AUTO_GENERATED_ARRAYS_MARKER

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_0="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
\n\
<PhasesEedbCatalog>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_A\" NumOfMappings=\"46\">\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_BFD_IPV4_EXTRA_SIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_BASE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_NON_MPLS_INITIAL_4BYTES\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER_DATA_BFR_BITSTR\" ResultType=\"ETPS_DATA_BFR_BIT_STR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RIF_BASIC\" ResultType=\"ETPS_ETH_RIF\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_RIF_BASIC\" ResultType=\"ETPS_ETH_RIF_STAT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPFIX_PSAMP\" ResultType=\"ETPS_IPFIX_PSAMP\" LL_Allocation=\"Mandatory\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_SCALE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_1="\
    <Element AppDb=\"EEDB_PPPOE\" ResultType=\"ETPS_PPPOE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RSPAN\" ResultType=\"ETPS_RSPAN\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_EXT_FIRST\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_EXT_SECOND\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_HDR_DP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_HDR_SAMP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6\" ResultType=\"ETPS_SRV6\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6\" ResultType=\"ETPS_SRV6_NO_SIDS\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_B\" NumOfMappings=\"42\">\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC_1TAG_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_TUNNELING\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_BASE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_NON_MPLS_INITIAL_4BYTES\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER_DATA_BFR_BITSTR\" ResultType=\"ETPS_DATA_BFR_BIT_STR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_2="\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PPPOE\" ResultType=\"ETPS_PPPOE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_L2_MC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_TWAMP\" ResultType=\"ETPS_REFLECTOR_TWAMP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6\" ResultType=\"ETPS_SRV6\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6\" ResultType=\"ETPS_SRV6_NO_SIDS\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_C\" NumOfMappings=\"53\">\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_BFD_IPV4_EXTRA_SIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_BASE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_NON_MPLS_INITIAL_4BYTES\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER_DATA_BFR_BITSTR\" ResultType=\"ETPS_DATA_BFR_BIT_STR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ERSPAN\" ResultType=\"ETPS_ERSPANV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ERSPAN\" ResultType=\"ETPS_ERSPANV3\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GRE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_3="_IPV6_TUNNEL_VXLAN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PPPOE\" ResultType=\"ETPS_PPPOE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_L2_MC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_D\" NumOfMappings=\"46\">\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_BASE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER\" ResultType=\"ETPS_BIER_NON_MPLS_INITIAL_4BYTES\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_BIER_DATA_BFR_BITSTR\" ResultType=\"ETPS_DATA_BFR_BIT_STR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
   ";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_4=" <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GRE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_L2_MC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_E\" NumOfMappings=\"41\">\n\
    <Element AppDb=\"EEDB_BIER_DATA_BFR_BITSTR\" ResultType=\"ETPS_DATA_BFR_BIT_STR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_5="=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GRE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_F\" NumOfMappings=\"42\">\n\
    <Element AppDb=\"EEDB_BIER_DATA_BFR_BITSTR\" ResultType=\"ETPS_DATA_BFR_BIT_STR\" LL_All";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_6="ocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GRE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_VXLAN_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNN";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_7="EL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_TWAMP\" ResultType=\"ETPS_REFLECTOR_TWAMP\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_G\" NumOfMappings=\"42\">\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_CUSTOM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_TUNNELING\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_L2TP\" ResultType=\"ETPS_L2TPV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_IML\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_8="_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RCH\" ResultType=\"ETPS_RCHV2\" LL_Allocation=\"Never\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_H\" NumOfMappings=\"16\">\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_TRIPLE_TAG\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC_1TAG_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_CUSTOM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_TUNNELING\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DATA_IPFIX_CODE\" ResultType=\"ETPS_DATA_IPFIX_CODE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DATA_DIP\" ResultType=\"ETPS_DATA_IPV6_DIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_SFLOW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_DATA_SID\" ResultType=\"ETPS_DATA_SID\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PWE\" ResultType=\"ETPS_IOAM_FL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RCH\" ResultType=\"ETPS_RCHV2\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
</PhasesEedbCatalog>\n\
";
_INTERNAL_AUTO_GENERATED_ARRAYS_MARKER

shr_error_e
pre_compiled_lif_phases_standard_1_xml_to_buf(
    char *filePath,
    char **buf,
    long int * size_p)
{
    char *file_buf;
    long int _internal_size_p;
    long int offset;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    
    *size_p = 0;

    _INTERNAL_AUTO_GENERATED_FILE_SEARCH

    if(!sal_strncmp("jericho2_a0/mdb/auto_generated/standard_1/standard_1_etps_in_eedb_phases.xml", filePath, RHFILE_MAX_SIZE))
    {
        _internal_size_p = 0;
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_0, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_1, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_2, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_3, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_4, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_5, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_6, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_7, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        _internal_size_p += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_8, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);

        *size_p = _internal_size_p;
        offset = 0;
        file_buf = sal_alloc(*size_p, "buf");

        sal_strncpy(file_buf, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_0, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_0, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_0, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_1, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_1, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_1, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_2, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_2, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_2, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_3, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_3, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_3, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_4, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_4, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_4, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_5, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_5, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_5, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_6, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_6, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_6, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_7, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_7, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));

        offset += sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_7, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE);
        sal_strncpy(file_buf + offset, jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_8, sal_strnlen(jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_8, STANDARD_1_LIF_PHASES_PRE_COMPILED_STRING_MAX_SIZE));


        *buf = file_buf;
        SHR_EXIT();
    }
    _INTERNAL_AUTO_GENERATED_FILE_SEARCH
    
    SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#else 

typedef int make_iso_compilers_happy;

#endif
