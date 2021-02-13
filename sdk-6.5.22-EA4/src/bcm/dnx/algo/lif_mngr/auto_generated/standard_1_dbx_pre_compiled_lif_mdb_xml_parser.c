/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#if defined (BCM_DNX2_SUPPORT)

#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <src/bcm/dnx/algo/lif_mngr/auto_generated/standard_1_dbx_pre_compiled_lif_mdb_xml_parser.h>

/* *INDENT-OFF* */
/** Auto-generated arrays.*/

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_0="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
\n\
<PhasesEedbCatalog>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_A\" NumOfMappings=\"42\">\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_BFD_IPV4_EXTRA_SIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RIF_BASIC\" ResultType=\"ETPS_ETH_RIF\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_RIF_BASIC\" ResultType=\"ETPS_ETH_RIF_STAT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPFIX_PSAMP\" ResultType=\"ETPS_IPFIX_PSAMP\" LL_Allocation=\"Mandatory\"/>\n\
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
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PPPOE\" ResultType=\"ETPS_PPPOE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RSPAN\" ResultType=\"ETPS_RSPAN\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_EXT_FIRST\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SF";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_1="LOW\" ResultType=\"ETPS_SFLOW_EXT_SECOND\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_HDR_DP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW\" ResultType=\"ETPS_SFLOW_HDR_SAMP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6\" ResultType=\"ETPS_SRV6\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_B\" NumOfMappings=\"39\">\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC_1TAG_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_TUNNELING\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PPPOE\" ResultType=\"ETPS_PPPOE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REF";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_2="LECTOR_L2_MC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_TWAMP\" ResultType=\"ETPS_REFLECTOR_TWAMP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6\" ResultType=\"ETPS_SRV6\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_C\" NumOfMappings=\"51\">\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_BFD_IPV4_EXTRA_SIP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ERSPAN\" ResultType=\"ETPS_ERSPANV2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ERSPAN\" ResultType=\"ETPS_ERSPANV3\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_MPLS";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_3="_TUNNEL\" ResultType=\"ETPS_MPLS_1_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_1_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_2_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_3_AH\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_2_TANDEM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_PPPOE\" ResultType=\"ETPS_PPPOE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_L2_MC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_D\" NumOfMappings=\"44\">\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_EVPN\" ResultType=\"ETPS_MPLS_1\" LL_A";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_4="llocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_L2_MC\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_E\" NumOfMappings=\"41\">\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GRE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_RAW\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IP";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_5="V6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_F\" NumOfMappings=\"42\">\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_COPY_INNER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_GRE_GENEVE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_UDP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV4_TUNNEL\" ResultType=\"ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_6="\"EEDB_IPV6_TUNNEL\" ResultType=\"ETPS_IPV6_TUNNEL_GENEVE\" LL_Allocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_TWAMP\" ResultType=\"ETPS_REFLECTOR_TWAMP\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_G\" NumOfMappings=\"40\">\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_CUSTOM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_TUNNELING\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_LAWFUL_INTERCEPT\" ResultType=\"ETPS_DATA_VAR_SIZE_DATA\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_GTP\" ResultType=\"ETPS_GTPV1U\" ";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_7="LL_Allocation=\"Optional\"/>\n\
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
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_FHEI_PLUS\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_IFIT\" ResultType=\"ETPS_MPLS_IFIT\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_DUMMY_TABLE_NOT_TO_USE\" ResultType=\"ETPS_MPLS_PHP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_MPLS_TUNNEL\" ResultType=\"ETPS_MPLS_PHP_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RCH\" ResultType=\"ETPS_RCHV2\" LL_Allocation=\"Never\"/>\n\
</EedbPhase>\n\
\n\
\n\
<EedbPhase Name=\"EEDB_H\" NumOfMappings=\"15\">\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_QOS_OAM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_OUT_AC\" ResultType=\"ETPS_AC_TRIPLE_TAG\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC\" LL_Allocation=\"Optional\"/>\n\
 ";

const char *jericho2_a0_mdb_auto_generated_standard_1_standard_1_etps_in_eedb_phases_jr2_a0_8="   <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_AC_1TAG_STAT\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_CUSTOM\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_ARP\" ResultType=\"ETPS_ARP_SA_TUNNELING\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SRV6_SID\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_IPV6_DIP\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_SFLOW_DATA\" ResultType=\"ETPS_DATA_128B\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_NULL\" ResultType=\"ETPS_NULL\" LL_Allocation=\"Optional\"/>\n\
    <Element AppDb=\"EEDB_RCH\" ResultType=\"ETPS_RCHV2\" LL_Allocation=\"Never\"/>\n\
    <Element AppDb=\"EEDB_OAM_REFLECTOR\" ResultType=\"ETPS_REFLECTOR_OAM\" LL_Allocation=\"Optional\"/>\n\
</EedbPhase>\n\
\n\
</PhasesEedbCatalog>\n\
";
/** Auto-generated arrays.*/

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

    /** Auto-generated file search.*/

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
    /** Auto-generated file search.*/
    /*
     * failure to find existing entry is error
     */
    SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/* *INDENT-ON* */
#else /* (BCM_DNX2_SUPPORT) */

typedef int make_iso_compilers_happy;

#endif /* (BCM_DNX2_SUPPORT) */
