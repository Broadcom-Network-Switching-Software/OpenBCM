/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        switch.c
 * Purpose:
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <bcm/switch.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/flex_ctr.h>
#if defined(BCM_INSTRUMENTATION_SUPPORT)
#include <bcm_int/esw/instrumentation.h>
#endif
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/tomahawk3_dispatch.h>
#endif /* INCLUDE_L3 */

#define AGM_LOCK(unit)      soc_mem_lock(unit, AGM_MONITOR_TABLEm)
#define AGM_UNLOCK(unit)    soc_mem_unlock(unit, AGM_MONITOR_TABLEm)

/* Helper routines for argument translation */
static int
_th3_bool_invert(int unit, int arg, int set)
{
    /* Same for both set/get */
    return !arg;
}

bcm_switch_binding_t tomahawk3_bindings[] =
{
    { bcmSwitchUnknownVlanToCpu, 0,
        CPU_CONTROL_0r, UVLAN_TOCPUf,
        NULL, 0 },
    { bcmSwitchArpReplyToCpu, 0,
        PROTOCOL_PKT_CONTROLr, ARP_REPLY_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpReplyDrop, 0,
        PROTOCOL_PKT_CONTROLr, ARP_REPLY_DROPf,
        NULL, 0 },
    { bcmSwitchArpRequestToCpu, 0,
        PROTOCOL_PKT_CONTROLr, ARP_REQUEST_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpRequestDrop, 0,
        PROTOCOL_PKT_CONTROLr, ARP_REQUEST_DROPf,
        NULL, 0 },
    { bcmSwitchNdPktToCpu, 0,
        PROTOCOL_PKT_CONTROLr, ND_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchNdPktDrop, 0,
        PROTOCOL_PKT_CONTROLr, ND_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDhcpPktToCpu, 0,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchDhcpPktDrop, 0,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchL2StaticMoveToCpu, 0,
        CPU_CONTROL_1r, STATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2NonStaticMoveToCpu, 0,
        CPU_CONTROL_1r, NONSTATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchSharedVlanEnable, 0,
        ING_CONFIG_64r, SVL_ENABLEf,
        NULL, 0},
    { bcmSwitchSharedVlanMismatchToCpu, 0,
        CPU_CONTROL_0r, PVLAN_VID_MISMATCH_TOCPUf ,
        NULL, 0},
    { bcmSwitchSharedVlanL2McastEnable, 0,
        ING_CONFIG_64r, LOOKUP_L2MC_WITH_FID_IDf,
        NULL, 0},
    { bcmSwitchBpduInvalidVlanDrop, 0,
        EGR_CONFIG_1r, BPDU_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMirrorInvalidVlanDrop, 0,
        EGR_CONFIG_1r, MIRROR_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMcastFloodBlocking, 0,
        IGMP_MLD_PKT_CONTROLr, PFM_RULE_APPLYf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldUcastEnable,  0,
        ING_CONFIG_64r, MLD_PKTS_UNICAST_IGNOREf,
        _th3_bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchMldDirectAttachedOnly,   0,
        ING_CONFIG_64r, MLD_CHECKS_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpReservedMcastEnable,    0,
        ING_CONFIG_64r, IPV4_RESERVED_MC_ADDR_IGMP_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldReservedMcastEnable,     0,
        ING_CONFIG_64r, IPV6_RESERVED_MC_ADDR_MLD_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpUcastEnable,            0,
        ING_CONFIG_64r, IGMP_PKTS_UNICAST_IGNOREf,
        _th3_bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchPortEgressBlockL2,          0,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L2f,
        NULL, 0},
    { bcmSwitchIp4McastL2DestCheck,        0,
        ING_CONFIG_64r, IPV4_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchIp6McastL2DestCheck,        0,
        ING_CONFIG_64r, IPV6_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchUnknownIpmcAsMcast, 0,
        ING_MISC_CONFIG2r, IPMC_MISS_AS_L2MCf,
        NULL, 0 },
    { bcmSwitchIpmcSameVlanL3Route, 0,
        EGR_CONFIG_1r, IPMC_ROUTE_SAME_VLANf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelMplsEcmp, 0,
        RTAG7_HASH_SELr, USE_FLOW_SEL_MPLS_ECMPf,
        NULL, 0},
    { bcmSwitchHashMPLSUseLabelStack, 0,
       RTAG7_HASH_CONTROL_2r, USE_MPLS_STACK_FOR_HASHINGf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange1, 0,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, 0,
        GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, 0,
       GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
       NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, 0,
        GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchEcmpMacroFlowHashEnable, 0,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
    { bcmSwitchDosAttackV4FirstFrag, 0,
        DOS_CONTROLr, IPV4_FIRST_FRAG_CHECK_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackSipEqualDip, 0,
        DOS_CONTROLr, DROP_IF_SIP_EQUALS_DIPf,
        NULL, 0 },
    { bcmSwitchDosAttackMACSAEqualMACDA, 0,
        DOS_CONTROLr, MACSA_EQUALS_MACDA_DROPf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackFlagZeroSeqZero, 0,
        DOS_CONTROLr, TCP_FLAGS_CTRL0_SEQ0_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackSynFrag, 0,
        DOS_CONTROLr, TCP_FLAGS_SYN_FRAG_ENABLEf,
        NULL, 0},
    { bcmSwitchDosAttackIcmpFragments, 0,
        DOS_CONTROL_2r, ICMP_FRAG_PKTS_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackIcmpV4, 0,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6, 0,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackTcpOffset, 0,
        DOS_CONTROL_2r, TCP_HDR_OFFSET_EQ1_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackUdpPortsEqual, 0,
        DOS_CONTROL_2r, UDP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpPortsEqual, 0,
        DOS_CONTROL_2r, TCP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsSF, 0,
        DOS_CONTROL_2r, TCP_FLAGS_SYN_FIN_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsFUP, 0,
        DOS_CONTROL_2r, TCP_FLAGS_FIN_URG_PSH_SEQ0_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpHdrPartial, 0,
        DOS_CONTROL_2r, TCP_HDR_PARTIAL_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackV6MinFragEnable, 0,
        DOS_CONTROL_2r, IPV6_MIN_FRAG_SIZE_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackIcmpPktOversize, 0,
        DOS_CONTROL_3r, BIG_ICMP_PKT_SIZEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6PingSize, 0,
        DOS_CONTROL_3r, BIG_ICMPV6_PKT_SIZEf,
        NULL, soc_feature_big_icmpv6_ping_check },
    { bcmSwitchDosAttackMinTcpHdrSize, 0,
        DOS_CONTROL_3r, MIN_TCPHDR_SIZEf,
        NULL, 0 },
    { bcmSwitchDosAttackToCpu, 0,
        CPU_CONTROL_0r, DOSATTACK_TOCPUf,
        NULL, 0 },
    { bcmSwitchTunnelIp4IdShared, 0,
        EGR_TUNNEL_ID_MASKr, SHARED_FRAG_ID_ENABLEf,
        NULL, 0},
    { bcmSwitchPortEgressBlockL3,          0,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L3f,
        NULL, 0},
    { bcmSwitchParityErrorToCpu, 0,
        CPU_CONTROL_0r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchSourceMacZeroDrop, 0,
        CPU_CONTROL_0r, MACSA_ALL_ZERO_DROPf,
        NULL, 0},
    { bcmSwitchMplsSequenceErrToCpu, 0,
        ING_MISC_CONFIGr, MPLS_SEQ_NUM_FAIL_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsLabelMissToCpu, 0,
        CPU_CONTROL_Mr, MPLS_LABEL_MISSf,
        NULL, 0},
    { bcmSwitchMplsTtlErrToCpu, 0,
        CPU_CONTROL_Mr, MPLS_TTL_CHECK_FAILf,
        NULL, 0},
    { bcmSwitchMplsInvalidL3PayloadToCpu, 0,
        CPU_CONTROL_Mr, MPLS_INVALID_PAYLOADf,
        NULL, 0},
    { bcmSwitchMplsInvalidActionToCpu, 0,
        CPU_CONTROL_Mr, MPLS_INVALID_ACTIONf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange1, 0,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, 0,
        GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, 0,
       GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
       NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, 0,
        GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPWControlWordToCpu, 0,
       ING_MISC_CONFIGr, PWACH_TOCPUf,
       NULL, soc_feature_mpls},
    { bcmSwitchCpuToCpuEnable, 0,
        ING_MISC_CONFIGr, DO_NOT_COPY_FROM_CPU_TO_CPUf,
        _th3_bool_invert, 0 },
    { bcmSwitchIpmcTtl1ToCpu, 0,
        CPU_CONTROL_1r, IPMC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcastTtl1ToCpu, 0,
        CPU_CONTROL_1r, L3UC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchDhcpLearn, 0,
        ING_MISC_CONFIG2r, DO_NOT_LEARN_DHCPf,
        NULL, 0 },
    { bcmSwitchTunnelUnknownIpmcDrop, 0,
        ING_MISC_CONFIG2r, UNKNOWN_TUNNEL_IPMC_DROPf,
        NULL, 0 },
    { bcmSwitchMcastUnknownErrToCpu, 0,
        CPU_CONTROL_1r, MC_INDEX_ERROR_TOCPUf,
        NULL,0},
    { bcmSwitchDosAttackV6MinFragSize, 0,
        IPV6_MIN_FRAG_SIZEr, PKT_LENGTHf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchEgressBlockUcast, 0,
        ING_MISC_CONFIGr, APPLY_EGR_MASK_ON_UC_ONLYf,
        NULL, soc_feature_egress_blk_ucast_override },
    { bcmSwitchHgHdrMcastFloodOverride, 0,
        EGR_CONFIG_1r, FORCE_STATIC_MH_PFMf,
        NULL, soc_feature_static_pfm },
    { bcmSwitchShaperAdjust, 0,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchUnknownMcastToCpu, 0,
        CPU_CONTROL_1r, UMC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownUcastToCpu, 0,
        CPU_CONTROL_1r, UUCAST_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3HeaderErrToCpu, 0,
        CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3DestToCpu, 0,
        CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcPortMissToCpu, 0,
        CPU_CONTROL_1r, IPMCPORTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcErrorToCpu, 0,
        CPU_CONTROL_1r, IPMCERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3ErrToCpu, 0,
        CPU_CONTROL_1r, V6L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3DstMissToCpu, 0,
        CPU_CONTROL_1r, V6L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3ErrToCpu, 0,
        CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3DstMissToCpu, 0,
        CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchTunnelErrToCpu, 0,
        CPU_CONTROL_1r, TUNNEL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchMartianAddrToCpu, 0,
        CPU_CONTROL_1r, MARTIAN_ADDR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcTtlErrToCpu, 0,
        CPU_CONTROL_1r, L3UC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3SlowpathToCpu, 0,
        CPU_CONTROL_1r, L3_SLOWPATH_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcTtlErrToCpu, 0,
        CPU_CONTROL_1r, IPMC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchSampleIngressRandomSeed, 0,
        SFLOW_ING_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchSampleEgressRandomSeed, 0,
        SFLOW_EGR_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchNonIpL3ErrToCpu, 0,
        CPU_CONTROL_1r, NIP_L3ERR_TOCPUf,
        NULL, soc_feature_nip_l3_err_tocpu },
    { bcmSwitchSourceRouteToCpu, 0,
        CPU_CONTROL_1r, SRCROUTE_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3MtuFailToCpu, 0,
        CPU_CONTROL_1r, L3_MTU_FAIL_TOCPUf,
        NULL, soc_feature_l3mtu_fail_tocpu },
    { bcmSwitchV6L3LocalLinkDrop,0,
        ING_MISC_CONFIG2r, IPV6_SIP_LINK_LOCAL_DROPf,
        NULL, 0},
    { bcmSwitchMplsGalAlertLabelToCpu, 0,
        CPU_CONTROL_Mr, MPLS_GAL_EXPOSED_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsRalAlertLabelToCpu, 0,
        CPU_CONTROL_Mr, MPLS_RAL_EXPOSED_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsIllegalReservedLabelToCpu, 0,
        CPU_CONTROL_Mr, MPLS_ILLEGAL_RSVD_LABEL_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsUnknownAchTypeToCpu, 0,
        CPU_CONTROL_Mr, MPLS_UNKNOWN_ACH_TYPE_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsUnknownAchVersionToCpu, 0,
        CPU_CONTROL_Mr, MPLS_UNKNOWN_ACH_VERSION_TOCPUf,
        NULL, 0},
    { bcmSwitchTimesyncUnknownVersionToCpu, 0,
        CPU_CONTROL_Mr, UNKNOWN_1588_VERSION_TO_CPUf,
        NULL, 0},
    { bcmSwitchTimesyncIngressVersion, 0,
        ING_1588_PARSING_CONTROLr, VERSION_CONTROLf,
        NULL, 0},
    { bcmSwitchTimesyncEgressVersion, 0,
        EGR_1588_PARSING_CONTROLr, VERSION_CONTROLf,
        NULL, 0},
    { bcmSwitchL2GreTunnelMissToCpu, 0,
        CPU_CONTROL_Mr, L2GRE_SIP_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchVxlanTunnelMissToCpu, 0,
        CPU_CONTROL_Mr, VXLAN_SIP_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchHashL2GreKeyMask0, 0,
        RTAG7_HASH_CONTROL_L2GRE_MASK_Ar, L2GRE_TUNNEL_GRE_KEY_MASK_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreKeyMask1, 0,
        RTAG7_HASH_CONTROL_L2GRE_MASK_Br, L2GRE_TUNNEL_GRE_KEY_MASK_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashSctpL4Port, 0,
        ING_SCTP_CONTROLr, PARSE_SCTPf,
        NULL, 0},
    { bcmSwitchHashUseL2GreTunnelGreKey0, 0,
        RTAG7_HASH_CONTROL_2r, L2GRE_TUNNEL_USE_GRE_KEY_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashUseL2GreTunnelGreKey1, 0,
        RTAG7_HASH_CONTROL_2r, L2GRE_TUNNEL_USE_GRE_KEY_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashField0Ip6FlowLabel,0,
       RTAG7_HASH_CONTROL_2r, ENABLE_FLOW_LABEL_IPV6_Af,
        NULL, 0},
    { bcmSwitchHashField1Ip6FlowLabel,0,
       RTAG7_HASH_CONTROL_2r, ENABLE_FLOW_LABEL_IPV6_Bf,
        NULL, 0},
    { bcmSwitchMacroFlowHashUseMSB, 0,
       RTAG7_HASH_CONTROL_2r, MACRO_FLOW_HASH_BYTE_SELf,
        NULL, 0},
    { bcmSwitchHashField0OverlayCntagRpidEnable, 0,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Af,
        NULL, 0},
    { bcmSwitchHashField1OverlayCntagRpidEnable, 0,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Bf,
        NULL, 0},
    { bcmSwitchHashMPLSUseLabelStack, 0,
       RTAG7_HASH_CONTROL_2r, USE_MPLS_STACK_FOR_HASHINGf,
        NULL, 0},
    { bcmSwitchHashMPLSExcludeReservedLabel, 0,
       RTAG7_HASH_CONTROL_2r, EXCLUDE_ALL_RESERVED_LABELS_FROM_MPLS_HASHf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkUc , 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_UCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelEcmp, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
    { bcmSwitchV6L3SrcDstLocalLinkDropCancel, 0,
        ING_MISC_CONFIG2r, IPV6_SIP_AND_DIP_LINK_LOCAL_DO_NOT_DROPf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkNonUnicast, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_NONUCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelEntropy, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_ENTROPY_LABELf,
        NULL, 0},
    { bcmSwitchFieldMultipathHashSeed, 0,
        FP_ECMP_HASH_CONTROLr, ECMP_HASH_SALTf,
        NULL, 0},
    { bcmSwitchRateLimitLinear,            0,
        MMU_MTRO_CONFIGr, ITU_MODE_SELf,
        _th3_bool_invert, 0}
};


int
_bcm_th3_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found)
{
    uint64 oval64, val64;
    int i;
    bcm_switch_binding_t *b;
    uint32 max;
    int fval, fwidth, prt, idx;

    *found = 0;
    /*
     * Tomahawk3_bindings can only handle switch type that requires
     * configuration in one field within one/multiple register. If the switch
     * type need to configure multiple fields, it need to be processed with
     * switch case method:
     */
    switch (type) {
        case bcmSwitchDosAttackIcmp:
            if (arg >= 0) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, DOS_CONTROL_2r, port, 0, &val64));
                oval64 = val64;
                soc_reg64_field32_set(unit,
                    DOS_CONTROL_2r, &val64, ICMP_V4_PING_SIZE_ENABLEf, !!arg);
                soc_reg64_field32_set(unit,
                    DOS_CONTROL_2r, &val64, ICMP_V6_PING_SIZE_ENABLEf, !!arg);
                if (COMPILER_64_NE(val64, oval64)) {
                    SOC_IF_ERROR_RETURN(
                        soc_reg_set(unit, DOS_CONTROL_2r, port, 0, val64));
                }
                *found = 1;
                return BCM_E_NONE;
            } else {
                return BCM_E_PARAM;
            }

        case bcmSwitchMeterAdjust:
            if (soc_feature(unit, soc_feature_meter_adjust)) {
                if (arg < 0) {
                    return BCM_E_PARAM;
                }

                /* EGR_COUNTER_CONTROLr */
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, EGR_COUNTER_CONTROLr, port, 0, &val64));
                oval64 = val64;
                fwidth = soc_reg_field_length(
                                unit, EGR_COUNTER_CONTROLr, PACKET_IFG_BYTESf);
                max = (fwidth < 32) ? ((1 << fwidth)) - 1 : -1;
                soc_reg64_field32_set(unit, EGR_COUNTER_CONTROLr, &val64,
                    PACKET_IFG_BYTESf, ((uint32)arg > max) ? max : (uint32)arg);
                if (COMPILER_64_NE(val64, oval64)) {
                    SOC_IF_ERROR_RETURN(
                       soc_reg_set(unit, EGR_COUNTER_CONTROLr, port, 0, val64));
                }

                /* EGR_SHAPING_CONTROLr */
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, EGR_SHAPING_CONTROLr, port, 0, &val64));
                oval64 = val64;
                fwidth = soc_reg_field_length(
                                unit, EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf);
                max = (fwidth < 32) ? ((1 << fwidth)) - 1 : -1;
                soc_reg64_field32_set(unit, EGR_SHAPING_CONTROLr, &val64,
                    PACKET_IFG_BYTESf, ((uint32)arg > max) ? max : (uint32)arg);
                if (COMPILER_64_NE(val64, oval64)) {
                    SOC_IF_ERROR_RETURN(
                       soc_reg_set(unit, EGR_SHAPING_CONTROLr, port, 0, val64));
                }

                /* IFP_METER_CONTROLr */
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, IFP_METER_CONTROLr, port, 0, &val64));
                oval64 = val64;
                fwidth = soc_reg_field_length(
                                unit, IFP_METER_CONTROLr, PACKET_IFG_BYTESf);
                max = (fwidth < 32) ? ((1 << fwidth)) - 1 : -1;
                soc_reg64_field32_set(unit, IFP_METER_CONTROLr, &val64,
                    PACKET_IFG_BYTESf, ((uint32)arg > max) ? max : (uint32)arg);
                if (COMPILER_64_NE(val64, oval64)) {
                    SOC_IF_ERROR_RETURN(
                        soc_reg_set(unit, IFP_METER_CONTROLr, port, 0, val64));
                }
                *found = 1;
            }
            return BCM_E_NONE;

        case bcmSwitchRemoteLearnTrust:
            arg = _th3_bool_invert(unit, arg, 1);
            if (arg < 0) {
                return BCM_E_PARAM;
            }

            /* ING_CONFIG_64r */
            SOC_IF_ERROR_RETURN(
                soc_reg_get(unit, ING_CONFIG_64r, port, 0, &val64));
            oval64 = val64;
            soc_reg64_field32_set(
                unit, ING_CONFIG_64r, &val64, IGNORE_HG_HDR_DONOT_LEARNf, arg);
            if (COMPILER_64_NE(val64, oval64)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_set(unit, ING_CONFIG_64r, port, 0, val64));
            }

            /* EGR_CONFIG_1r */
            SOC_IF_ERROR_RETURN(
                soc_reg_get(unit, EGR_CONFIG_1r, port, 0, &val64));
            oval64 = val64;
            soc_reg64_field32_set(
                unit, EGR_CONFIG_1r, &val64, IGNORE_HG_HDR_DONOT_LEARNf, arg);
            if (COMPILER_64_NE(val64, oval64)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_set(unit, EGR_CONFIG_1r, port, 0, val64));
            }
            *found = 1;
            return BCM_E_NONE;

        default:
            break;
    }

    for (i = 0; i < COUNTOF(tomahawk3_bindings); i++) {
        b = &tomahawk3_bindings[i];
        if (b->type == type) {
            /* We don't check the b->chip since the table is specific for TH3 */
            if (b->feature && !soc_feature(unit, b->feature)) {
                continue;
            }
            if (!SOC_REG_FIELD_VALID(unit, b->reg, b->field)) {
                continue;
            }
            if (b->xlate_arg) {
                if ((fval = (b->xlate_arg)(unit, arg, 1)) < 0) {
                    return fval;
                }
            } else {
                fval = arg;
            }
            /* Negative values should be treated as errors */
            if (fval < 0) {
                return BCM_E_PARAM;
            }
            if ((b->reg == PROTOCOL_PKT_CONTROLr) ||
                (b->reg == IGMP_MLD_PKT_CONTROLr)) {
                int index;

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_get(unit, port,
                    PROTOCOL_PKT_INDEXf, &index));
                prt = port;
                idx = index;
            } else {
                prt = port;
                idx = 0;
            }

            fwidth = soc_reg_field_length(unit, b->reg, b->field);
            if (fwidth < 32) {
                max = (1 << fwidth) - 1;
            } else {
                max = -1;
            }
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
            oval64 = val64;
            soc_reg64_field32_set(unit, b->reg, &val64, b->field,
                                ((uint32)fval > max) ? max : (uint32)fval);
            if (COMPILER_64_NE(val64, oval64)) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, b->reg, prt, idx, val64));
            }

            *found = 1;
            break;
        }
    }
    return BCM_E_NONE;
}


int
_bcm_th3_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found)
{
    uint64 val64;
    int i;
    bcm_switch_binding_t *b;
    int prt, idx;

    *found = 0;

    /*
     * Tomahawk3_bindings can only handle switch type that requires
     * configuration in one field within one/multiple register. If the switch
     * type need to configure multiple fields, it need to be processed with
     * switch case method:
     */
    switch (type) {
        case bcmSwitchDosAttackIcmp:
            SOC_IF_ERROR_RETURN(
                soc_reg_get(unit, DOS_CONTROL_2r, port, 0, &val64));
            *arg = soc_reg64_field32_get(
                        unit, DOS_CONTROL_2r, val64, ICMP_V4_PING_SIZE_ENABLEf);
            *found = 1;
            return BCM_E_NONE;

        case bcmSwitchMeterAdjust:
            if (soc_feature(unit, soc_feature_meter_adjust)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, EGR_COUNTER_CONTROLr, port, 0, &val64));
                *arg = soc_reg64_field32_get(
                        unit, EGR_COUNTER_CONTROLr, val64, PACKET_IFG_BYTESf);
                *found = 1;
            }
            return BCM_E_NONE;

        case bcmSwitchRemoteLearnTrust:
            SOC_IF_ERROR_RETURN(
                soc_reg_get(unit, ING_CONFIG_64r, port, 0, &val64));
            *arg = soc_reg64_field32_get(
                    unit, ING_CONFIG_64r, val64, IGNORE_HG_HDR_DONOT_LEARNf);
            *arg = _th3_bool_invert(unit, *arg, 0);
            *found = 1;
            return BCM_E_NONE;

        default:
            break;
    }

    for (i = 0; i < COUNTOF(tomahawk3_bindings); i++) {
        b = &tomahawk3_bindings[i];
        if (b->type == type) {
            if (b->feature && !soc_feature(unit, b->feature)) {
                continue;
            }

            if (!SOC_REG_FIELD_VALID(unit, b->reg, b->field)) {
                continue;
            }
            if ((b->reg == PROTOCOL_PKT_CONTROLr) ||
                (b->reg == IGMP_MLD_PKT_CONTROLr)) {
                int index;

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_get(unit, port,
                    PROTOCOL_PKT_INDEXf, &index));
                prt = port;
                idx = index;
            } else {
                prt = port;
                idx = 0;
            }
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
            *arg = soc_reg64_field32_get(unit, b->reg, val64, b->field);

            if (b->xlate_arg) {
                *arg = (b->xlate_arg)(unit, *arg, 0);
            }
            *found = 1;
            break;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_switch_pkt_trace_info_clear
 *
 * Purpose:
 *      This API is used to clear the existing SDK states for the visibility
 *      capture on the given port. It also clears the HW capture buffers for
 *      the pipe on which the given port resides.
 *      If the port parameter is passed in as -1 then SDK will reset the SW
 *      and HW capture states respective to all the pipes.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical port number
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 */
int bcm_tomahawk3_switch_pkt_trace_info_clear(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_INSTRUMENTATION_SUPPORT
    int pipe = -1;

    /* Extract the pipe information from the given valid port */
    if (-1 != port) {
        if (!SOC_PORT_VALID(unit, port) ||
            SOC_FAILURE(soc_port_pipe_get(unit, port, &pipe))) {
            return BCM_E_PORT;
        }
    }

    /* There are no SW states so just reset the HW states */
    rv = _bcm_th3_pkt_trace_hw_reset(unit, pipe);
#endif
    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      bcmi_th3_switch_agm_l3_ecmp_get_entries_per_period
 *
 * Purpose:
 *      This helper function determines the number of counter entries needed
 *      or already allocated for an ECMP group. For ECMP groups which are
 *      configured with dynamic mode RESILIENT HASH, dynamic size of the grp
 *      is the number of counters. For all other ECMP groups it is the number
 *      of actual members in the group.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_id - (IN) AGM identifier
 *      ecmp_id - (IN) ECMP Group identifier
 *      num_entries - (OUT) Number of entries to be returned to caller
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This helper routine does not take any locks. It is assumed the caller
 *      would lock approriate resources before calling it.
 */
static int
bcmi_th3_switch_agm_l3_ecmp_get_entries_per_period(int unit,
   bcm_switch_agm_id_t agm_id, bcm_if_t ecmp_id, uint32 *num_entries) {

    int rv = BCM_E_NONE;
    int mbr_cnt;
    agm_monitor_t   agm_mnt;
    bcm_l3_egress_ecmp_t ecmp_info;

    if (!num_entries) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_th_switch_agm_info(unit, agm_id, &agm_mnt));
    *num_entries = agm_mnt.attr.num_members;
    bcm_l3_egress_ecmp_t_init(&ecmp_info);

    ecmp_info.ecmp_intf = ecmp_id;
    rv = bcm_esw_l3_ecmp_get(unit, &ecmp_info, 0, NULL, &mbr_cnt);
    if (BCM_SUCCESS(rv)) {
        if (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT == ecmp_info.dynamic_mode) {
            if (ecmp_info.dynamic_size > *num_entries) {
                *num_entries = ecmp_info.dynamic_size;
            }
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "L3 ECMP ID %d info get failed (%d)\n"), ecmp_id, rv));
    }
    return rv;
}

/*
 * Function:
 *      bcm_th3_switch_agm_l3_ecmp_stat_destroy
 *
 * Purpose:
 *      This function  detaches and de-allocates the flex stat counter
 *      associated with the AGM with the given AGM ID. Once flex stat APIs
 *      successfully complete the counter ID in the AGM is reset to 0.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_id - (IN) AGM identifier
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This function takes AGM_LOCK, so the caller does not need to lock
 */
int bcm_th3_switch_agm_l3_ecmp_stat_destroy(int unit,
                                            bcm_switch_agm_id_t agm_id) {

    int             rv = BCM_E_NONE;
    agm_monitor_t   agm_mnt;

    AGM_LOCK(unit);
    rv = _bcm_th_switch_agm_info(unit, agm_id, &agm_mnt);

    if (BCM_SUCCESS(rv)) {
        /* Detach the flex stat counter */
        rv = bcm_th_agm_stat_detach(unit, agm_mnt.attr.agm_id,
                    agm_mnt.agm_pool_id, (uint32)agm_mnt.counter_id);

        if (BCM_SUCCESS(rv)) {
            /* Destroy flex counter */
            rv = bcm_th_agm_stat_id_clear(unit, agm_mnt.counter_id);

            if (BCM_SUCCESS(rv)) {
                /* Update software bookkeeping information */
                _bcm_th_switch_agm_update_counter_id(unit, agm_id, 0);
            } else {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                  "AGM %d destroy counter id failed, rv = %d.\n"), agm_id, rv));
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "AGM %d detach failed, rv = %d.\n"), agm_id, rv));
        }
    }
    AGM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_th3_switch_agm_l3_ecmp_stat_create
 *
 * Purpose:
 *      This function allocates and attaches a flex stat counter associated
 *      with an AGM with the given ID for given ECMP group ID. The number of
 *      counter indexes to be allocated is determined based on the ECMP group
 *      type. If the ECMP group was created with dynamic mode RESILIENT, the
 *      number of counter entries would be (dynamic_size * agm_periods), else
 *      it would be (ecmp_grp_member_count * agm_periods). Once flex stat APIs
 *      successfully complete, the counter ID in the AGM is set to Flex stat
 *      given counter ID.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_id - (IN) AGM identifier
 *      l3_ecmp_id - (IN) ECMP group identifier
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This function takes AGM_LOCK, so the caller does not need to lock
 */
int bcm_th3_switch_agm_l3_ecmp_stat_create(int unit,
        bcm_switch_agm_id_t agm_id, bcm_if_t l3_ecmp_id) {

    int             rv = BCM_E_NONE;
    uint32          counter_id = 0;
    uint32          stat_cnt_per_period = 0;
    agm_monitor_t   agm_mnt;

    AGM_LOCK(unit);
    rv = _bcm_th_switch_agm_info(unit, agm_id, &agm_mnt);

    if (BCM_SUCCESS(rv)) {
        /* Get number of counter entries to be created for one AGM period */
        rv = bcmi_th3_switch_agm_l3_ecmp_get_entries_per_period(
                        unit, agm_id, l3_ecmp_id, &stat_cnt_per_period);

        if (BCM_SUCCESS(rv)) {
            /* Allocate a flex stat counter ID */
            rv = bcm_th_agm_stat_id_get(unit,
                             agm_id,
                             agm_mnt.agm_pool_id,
                             stat_cnt_per_period*(agm_mnt.attr.period_num + 1),
                             &counter_id);

            if (BCM_SUCCESS(rv)) {
                LOG_VERBOSE(BSL_LS_BCM_SWITCH,
                    (BSL_META_U(unit, "Allocated counter id %d for AGM %d.\n"),
                    counter_id, agm_id));

                /* Attach the counter */
                rv = bcm_th_agm_stat_attach(
                        unit, agm_id, agm_mnt.agm_pool_id, counter_id);
                if (BCM_FAILURE(rv)) {
                    AGM_UNLOCK(unit);
                    return rv;
                }

                if (BCM_SUCCESS(rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_SWITCH,
                        (BSL_META_U(unit, "Attached counter id %d to AGM %d\n"),
                        counter_id, agm_id));

                    /* Update software bookkeeping information */
                    _bcm_th_switch_agm_update_counter_id(
                                                    unit, agm_id, counter_id);
                } else {
                    LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                        "Attach failed of counter %d for AGM %d failed\n"),
                        counter_id, agm_id));
                    bcm_th_agm_stat_id_clear(unit, counter_id);
                }
            } else {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "Allocation of counter with count %d for AGM %d failed\n"),
                    stat_cnt_per_period*(agm_mnt.attr.period_num + 1), agm_id));
            }
        }
    }

    AGM_UNLOCK(unit);
    return rv;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcmi_th3_agm_stat_retrieve
 *
 * Purpose:
 *      This helper function retrieves the packet stat count and byte stat count
 *      for the given flex stat counter index. At first pkt count is retrieved.
 *      Only if it is non zero byte count is retrieved else byte count is
 *      assumed to be zero.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_id - (IN) AGM identifier
 *      ctr_idx - (IN) Flex stat counter index
 *      ctr_val - (OUT) counter value to be sent back to caller
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This helper routine does not take any locks. It is assumed the caller
 *      would lock approriate resources before calling it.
 */
static int
_bcmi_th3_agm_stat_retrieve(int unit,
                           bcm_switch_agm_id_t agm_id,
                           uint32 ctr_idx,
                           bcm_stat_value_t *ctr_val) {

    int                         rv = BCM_E_NONE;
    uint32                      num_table;
    bcm_stat_flex_table_info_t  tbl_info;

    if (!ctr_val) {
        return BCM_E_PARAM;
    }

    /* Initialize to 0 */
    sal_memset(ctr_val, 0, sizeof(bcm_stat_value_t));

    BCM_IF_ERROR_RETURN(
        bcm_th_agm_stat_get_table_info(unit, agm_id, &num_table, &tbl_info));
    if (bcmStatFlexDirectionIngress != tbl_info.direction) {
        return BCM_E_NONE;
    }

    /* Get Pkt count */
    rv = _bcm_esw_stat_counter_get(unit, TRUE,
                tbl_info.index, tbl_info.table, 0,
                0 /* packet flag*/, ctr_idx, ctr_val);

    if (BCM_FAILURE(rv)) {
        sal_memset(ctr_val, 0, sizeof(bcm_stat_value_t));
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d collected pkt counter %d failed, rv = %d.\n"),
            agm_id, ctr_idx, rv));
    } else if (!COMPILER_64_IS_ZERO(ctr_val->packets64)) {

        /* Get byte count */
        rv = _bcm_esw_stat_counter_get(unit, TRUE,
                    tbl_info.index, tbl_info.table, 0,
                    1 /* byte flag */, ctr_idx, ctr_val);

        if (BCM_FAILURE(rv)) {
            sal_memset(ctr_val, 0, sizeof(bcm_stat_value_t));
            LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "AGM %d collected byte counter %d failed, rv = %d.\n"),
                agm_id, ctr_idx, rv));
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcmi_th3_switch_agm_trunk_stat_get
 *
 * Purpose:
 *      This internal function gets the AGM stats for Trunk.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_mnt - (IN) AGM monitor
 *      member_arr - (IN) array to hold Trunk member gport IDs
 *      nstat - (IN) size of the stat array passed in
 *      stat_arr - (OUT) retrieved flex stat values
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This function does not check validity of given parameters, as it is
 *      meant to be called after doing such validations. Hence it also does
 *      not takes appr. locks,  Caller need to take locks before calling it
 */
static int
_bcmi_th3_switch_agm_trunk_stat_get(int unit,
                            agm_monitor_t *agm_mnt,
                            void *member_arr,
                            int nstat,
                            bcm_switch_agm_stat_t *stat_arr) {
    int     i;
    int     mbr_cnt;
    bcm_trunk_member_t *mbr_arr = (bcm_trunk_member_t *)member_arr;
    bcm_trunk_info_t tinfo;
    bcm_stat_value_t ctr_val;

    LOG_VERBOSE(BSL_LS_BCM_SWITCH, (
        BSL_META_U(unit, "AGM %d(Trunk) stat get: mbr cnt %d, period %d.\n"),
        agm_mnt->attr.agm_id, agm_mnt->attr.num_members,
        (nstat / agm_mnt->attr.num_members) + 1));


    BCM_IF_ERROR_RETURN(
        bcm_esw_trunk_get(unit, (bcm_trunk_t)agm_mnt->group_id,
                &tinfo, agm_mnt->attr.num_members, mbr_arr, &mbr_cnt));

    if (0 == mbr_cnt) {
        return BCM_E_FAIL;
    }

    /* Fill in the nstat array with available data */
    for (i = 0; i < nstat ; i++) {
        stat_arr[i].period = i / agm_mnt->attr.num_members;
        stat_arr[i].intf_id = BCM_IF_INVALID;
        stat_arr[i].gport_id = mbr_arr[i % mbr_cnt].gport;

        BCM_IF_ERROR_RETURN(
          _bcmi_th3_agm_stat_retrieve(unit, agm_mnt->attr.agm_id, i, &ctr_val));

        stat_arr[i].packets = ctr_val.packets64;
        stat_arr[i].bytes = ctr_val.bytes;
    }

    return BCM_E_NONE;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcmi_th3_switch_agm_l3_ecmp_stat_get
 *
 * Purpose:
 *      This internal function gets the AGM stats for ECMP group.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_mnt - (IN) AGM monitor
 *      member_arr - (IN) array to hold ECMP Egr obj intf IDs
 *      nstat - (IN) size of the stat array passed in
 *      stat_arr - (OUT) retrieved flex stat values
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This function does not check validity of given parameters, as it is
 *      meant to be called after doing such validations. Hence it also does
 *      not takes appr. locks,  Caller need to take locks before calling it
 */
static int
_bcmi_th3_switch_agm_l3_ecmp_stat_get(int unit,
                            agm_monitor_t *agm_mnt,
                            void *member_arr,
                            int nstat,
                            bcm_switch_agm_stat_t *stat_arr) {

    int     i, j;
    int     stat_arr_idx;
    int     num_periods = 0;
    int     mbr, mbr_cnt = 0;
    int     nhop_to_intf_offset = 0;
    uint32  nhop_idx;
    uint32  counter_idx;
    uint32  ecmp_base_ptr = 0;
    uint32  entries_per_period = agm_mnt->attr.num_members;
    bcm_if_t    egr_obj_intf;
    bcm_if_t    *mbr_arr = (bcm_if_t *)member_arr;
    bcm_if_t    ecmp_intf = (bcm_if_t)agm_mnt->group_id;
    soc_mem_t   ecmp_mem = INVALIDm;
    ecmp_entry_t         ecmp_entry;
    bcm_stat_value_t     counter_val;
    ecmp_count_entry_t   ecmp_grp_entry;
    bcm_l3_egress_ecmp_t ecmp_info;


    /* Calculate number of periods based on given nstat parameter */
    num_periods =
        (nstat + agm_mnt->attr.num_members - 1) / agm_mnt->attr.num_members;

    LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
        "AGM %d(ECMP) stat get: group member count %d, period %d.\n"),
        agm_mnt->attr.agm_id, agm_mnt->attr.num_members, num_periods));

    /* Retrieve and validate the members of the ECMP group */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_intf = ecmp_intf;
    BCM_IF_ERROR_RETURN(
        bcm_esw_l3_egress_ecmp_get(unit, &ecmp_info, 0, NULL, &mbr_cnt));
    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_multipath_get(
        unit, ecmp_intf, agm_mnt->attr.num_members, mbr_arr, &mbr_cnt));
    if (0 == mbr_cnt) {
        return BCM_E_FAIL;
    }

    /* For RH ECMPs find out the base address for L3_ECMP memory and offset
     * which will be used to covert nhop to corresponding egr obj interface ID.
     * Also adjust the counter entries per AGM period values based on dyn size.
     */
    if (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT == ecmp_info.dynamic_mode) {
        if (ecmp_info.dynamic_size > agm_mnt->attr.num_members) {
            entries_per_period = ecmp_info.dynamic_size;
        }

        if (bcmSwitchAgmTypeL3EcmpOverlay == agm_mnt->attr.agm_type) {
            ecmp_mem = ECMP_GROUP_HIERARCHICALm;
            nhop_to_intf_offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        } else {
            ecmp_mem = L3_ECMP_COUNTm;
            nhop_to_intf_offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, ecmp_mem,
            MEM_BLOCK_ANY, ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit),
            &ecmp_grp_entry));
        ecmp_base_ptr =
            soc_mem_field32_get(unit, ecmp_mem, &ecmp_grp_entry, BASE_PTRf);
    }

    /* Initialize the stat arr with known rese values */
    for (i = 0; i < nstat ; i++) {
        stat_arr[i].period = i / agm_mnt->attr.num_members;
        COMPILER_64_ZERO(stat_arr[i].packets);
        COMPILER_64_ZERO(stat_arr[i].bytes);
        stat_arr[i].gport_id = BCM_GPORT_INVALID;
        stat_arr[i].intf_id = mbr_arr[i % mbr_cnt];
    }

    for (i = 0; i < num_periods; i++) {
        for (j = 0; j < entries_per_period; j++) {
            counter_idx = (i * entries_per_period) + j;

            /* Retrieve the stat counts */
            BCM_IF_ERROR_RETURN(
                _bcmi_th3_agm_stat_retrieve(unit,
                    agm_mnt->attr.agm_id, counter_idx, &counter_val));

            /* If there are no packets on this counter then skip to next ctr */
            if (COMPILER_64_IS_ZERO(counter_val.packets64)) {
                continue;
            }

            /* Calculate the index into arr where the stats are to be updated */
            if (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT == ecmp_info.dynamic_mode) {

                /* For RH ECMPs, index into stat arr is derived indirectly.
                 * First next hop index is retrieved from L3 ECMP memory which
                 * is then used to deduce the egr object interface ID. This egr
                 * intf is further searched into the member array. The offset of
                 * member array where the intf is found is used directly to
                 * derive the index into USer given stat array.
                 */
                SOC_IF_ERROR_RETURN(READ_L3_ECMPm(
                    unit, MEM_BLOCK_ANY, j + ecmp_base_ptr, &ecmp_entry));

                nhop_idx =
                    soc_L3_ECMPm_field32_get(
                        unit, &ecmp_entry, NEXT_HOP_INDEXf);

                egr_obj_intf = nhop_to_intf_offset + (bcm_if_t)nhop_idx;

                for (mbr=0 ; mbr<mbr_cnt ; mbr++) {
                    if (mbr_arr[mbr] == egr_obj_intf) {
                        stat_arr_idx = mbr + (i * agm_mnt->attr.num_members);
                        break;
                    }
                }

                if (mbr >= mbr_cnt) {
                    LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                        "AGM %d ECMP egr obj %d not found.\n"),
                        agm_mnt->attr.agm_id, egr_obj_intf));
                    return BCM_E_FAIL;
                }
            } else {
                /* For non RH ECMPs, derive the index into stat arr directly */
                stat_arr_idx = (i * agm_mnt->attr.num_members) + j;
            }

            /* Update the stat arr if the index is within the max arr size */
            if (stat_arr_idx < nstat) {
                COMPILER_64_ADD_64( stat_arr[stat_arr_idx].packets,
                                    counter_val.packets64 );
                COMPILER_64_ADD_64( stat_arr[stat_arr_idx].bytes,
                                    counter_val.bytes );
            } else {
                /* If the calculated index is out of range then assert, however
                 * for last AGM period there could be possibility that User did
                 * not provide enough space to accomodate data for all members,
                 * hence in this case only assert if calculated index is out of
                 * range provided if the given array was of sufficient size.
                 */
                if (i != (num_periods-1)) {
                    assert(stat_arr_idx < nstat);
                } else if (stat_arr_idx >= ((i+1)*agm_mnt->attr.num_members)) {
                    assert(stat_arr_idx < ((i+1)*agm_mnt->attr.num_members));
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      bcm_th3_switch_agm_stat_get
 *
 * Purpose:
 *      This function initializes the given stat array with reset values. It
 *      allocates an array to hold ECMP or Trunk member information based on
 *      the AGM type. Then it calls appr. function based on AGM type to
 *      retrieve the flex stats.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_id - (IN) AGM identifier
 *      nstat - (IN) size of the stat array passed in
 *      stat_arr - (OUT) retrieved flex stat values
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This function checks validity of all the given parameters, as it is
 *      meant to be called from API dispatch layer. It also takes appr. locks
 *      so caller need not take locks.
 */
int bcm_th3_switch_agm_stat_get(int unit,
                           bcm_switch_agm_id_t agm_id,
                           int nstat,
                           bcm_switch_agm_stat_t *stat_arr) {

    int             elm_size = 0;
    int             rv = BCM_E_UNAVAIL;
    void            *mbr_arr = NULL;
    agm_monitor_t   agm_mnt;
    int (*func_p)(int, agm_monitor_t *, void *, int, bcm_switch_agm_stat_t *);

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (NULL == stat_arr || nstat <= 0) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcm_th_switch_agm_id_validate(unit, agm_id));

    func_p = NULL;
    AGM_LOCK(unit);

    /* Get hold of AGM monitor info */
    rv = _bcm_th_switch_agm_info(unit, agm_id, &agm_mnt);

    if (BCM_SUCCESS(rv)) {
        /* Validate the nstat supported by the AGM. If the user has
         * provided larger than needed buffer, return BCM_E_PARAM
         */
        if (nstat > ((agm_mnt.attr.period_num+1) * agm_mnt.attr.num_members)) {
            AGM_UNLOCK(unit);
            return BCM_E_PARAM;
        }

        if ((bcmSwitchAgmTypeL3Ecmp == agm_mnt.attr.agm_type) ||
            (bcmSwitchAgmTypeL3EcmpOverlay == agm_mnt.attr.agm_type)) {
#if defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_l3)) {
                elm_size = sizeof(bcm_if_t);
                func_p = _bcmi_th3_switch_agm_l3_ecmp_stat_get;
            } else
#endif
            {
                AGM_UNLOCK(unit);
                return BCM_E_UNAVAIL;
            }
        } else if (bcmSwitchAgmTypeTrunk == agm_mnt.attr.agm_type) {
            elm_size = sizeof(bcm_trunk_member_t);
            func_p = _bcmi_th3_switch_agm_trunk_stat_get;
        } else {
            AGM_UNLOCK(unit);
            return BCM_E_UNAVAIL;
        }

        /* Allocate member array */
        mbr_arr = sal_alloc(elm_size * agm_mnt.attr.num_members, "mbr_arr");
        if (mbr_arr) {
            /* Call AGM type specific function to get the stats */
            rv = func_p(unit, &agm_mnt, mbr_arr, nstat, stat_arr);

            /* Free the member array */
            sal_free(mbr_arr);
        } else {
            rv = BCM_E_MEMORY;
        }
    }

    AGM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_th3_switch_agm_stat_clear
 *
 * Purpose:
 *      This function calls appr. flex stat APIs to clear the counter value for
 *      counter associated with the given AGM ID.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      agm_id - (IN) AGM identifier
 *
 * Returns:
 *      BCM_E_xxx
 *
 * Notes:
 *      This function checks validity of all the given parameters, as it is
 *      meant to be called from API dispatch layer. It also takes appr. locks
 *      so caller need not take locks.
 */
int bcm_th3_switch_agm_stat_clear(int unit,
                           bcm_switch_agm_id_t agm_id) {

    int     i, j;
    int     num_periods;
    int     rv = BCM_E_NONE;
    uint32  num_table;
    uint32  entries_per_period;
    agm_monitor_t agm_mnt;
    bcm_stat_value_t counter_val;
    bcm_stat_flex_table_info_t tbl_info;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    BCM_IF_ERROR_RETURN(bcm_th_switch_agm_id_validate(unit, agm_id));

    AGM_LOCK(unit);
    rv = bcm_th_agm_stat_get_table_info(unit, agm_id, &num_table, &tbl_info);
    if (BCM_FAILURE(rv) || bcmStatFlexDirectionIngress != tbl_info.direction) {
        goto _exit_stat_cl;
    }

    rv = _bcm_th_switch_agm_info(unit, agm_id, &agm_mnt);
    if (BCM_FAILURE(rv)) {
        goto _exit_stat_cl;
    }

    num_periods = agm_mnt.attr.period_num + 1;
    entries_per_period = agm_mnt.attr.num_members;

    if ((bcmSwitchAgmTypeL3Ecmp == agm_mnt.attr.agm_type) ||
        (bcmSwitchAgmTypeL3EcmpOverlay == agm_mnt.attr.agm_type)) {
#if defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_l3)) {
            rv = bcmi_th3_switch_agm_l3_ecmp_get_entries_per_period(unit,
                    agm_id, (bcm_if_t)agm_mnt.group_id, &entries_per_period);
        } else
#endif
        {
            rv = BCM_E_UNAVAIL;
        }
        if (BCM_FAILURE(rv)) {
            goto _exit_stat_cl;
        }
    }

    sal_memset(&counter_val, 0, sizeof(bcm_stat_value_t));

    for (i = 0; i < num_periods; i++) {
        for (j = 0; j < entries_per_period; j++) {

            /* Clear packet and byte count, both at once */
            rv = _bcm_esw_stat_counter_set(
                    unit, tbl_info.index, tbl_info.table,
                        0, -1, ((i * entries_per_period) + j), &counter_val);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "AGM %d clear packet/byte counter %d failed, rv = %d.\n"),
                    agm_id, ((i * entries_per_period) + j), rv));
                goto _exit_stat_cl;
            }
        }
    }

_exit_stat_cl:
    AGM_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */
