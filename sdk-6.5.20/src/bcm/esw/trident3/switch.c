/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <bcm/switch.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/trident3.h>

#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif

#define LARGE_SCALE_NAT_UC_ARG2REG(arg) \
    ((arg)==BCM_L3UC_NAT_NONE ? 0 : \
     (arg)==BCM_L3UC_NAT_UDP  ? 1 : \
     (arg)==BCM_L3UC_NAT_TCP  ? 2 : \
     (arg)==BCM_L3UC_NAT ? 3 : -1)

#define LARGE_SCALE_NAT_UC_REG2ARG(val) \
    ((val)==1 ? BCM_L3UC_NAT_UDP : \
     (val)==2 ? BCM_L3UC_NAT_TCP : \
     (val)==3 ? BCM_L3UC_NAT : 0)

#define LARGE_SCALE_NAT_MC_ARG2REG(arg) \
    ((arg)==BCM_L3MC_NAT_NONE ? 0 : \
     (arg)==BCM_L3MC_DNAT ? 1 : \
     (arg)==BCM_L3MC_DNAT_WITH_MACDA_UPDATE  ? 1 : \
     (arg)==BCM_L3MC_SNAT ? 2 : \
     (arg)==BCM_L3MC_DNAT_SNAT ? 3 : \
     (arg)==BCM_L3MC_DNAT_SNAT_WITH_MACDA_UPDATE ? 3 : \
     (arg)==BCM_L3MC_DNAPT ? 4 : \
     (arg)==BCM_L3MC_DNAPT_WITH_MACDA_UPDATE  ? 4 : \
     (arg)==BCM_L3MC_SNAPT ? 5 : \
     (arg)==BCM_L3MC_DNAPT_SNAPT ? 6 : \
     (arg)==BCM_L3MC_DNAPT_SNAPT_WITH_MACDA_UPDATE ? 6 : -1)

#define LARGE_SCALE_NAT_MC_REG2ARG(val, da) \
    ((val)==1 && !(da) ? BCM_L3MC_DNAT : \
     (val)==1 &&  (da) ? BCM_L3MC_DNAT_WITH_MACDA_UPDATE : \
     (val)==2          ? BCM_L3MC_SNAT : \
     (val)==3 && !(da) ? BCM_L3MC_DNAT_SNAT : \
     (val)==3 &&  (da) ? BCM_L3MC_DNAT_SNAT_WITH_MACDA_UPDATE : \
     (val)==4 && !(da) ? BCM_L3MC_DNAPT: \
     (val)==4 &&  (da) ? BCM_L3MC_DNAPT_WITH_MACDA_UPDATE: \
     (val)==5          ? BCM_L3MC_SNAPT: \
     (val)==6 && !(da) ? BCM_L3MC_DNAPT_SNAPT : \
     (val)==6 &&  (da) ? BCM_L3MC_DNAPT_SNAPT_WITH_MACDA_UPDATE : 0)

#define LARGE_SCALE_NAT_DA_UPDATE(arg) \
    ((arg)==BCM_L3MC_DNAT_WITH_MACDA_UPDATE || \
     (arg)==BCM_L3MC_DNAT_SNAT_WITH_MACDA_UPDATE || \
     (arg)==BCM_L3MC_DNAPT_WITH_MACDA_UPDATE || \
     (arg)==BCM_L3MC_DNAPT_SNAPT_WITH_MACDA_UPDATE)

#define LARGE_SCALE_NAT_UC_FIELD_ID(type) \
    ((type)==bcmSwitchL3UcLargeDNAT ?     L3UC_DNATf : \
     (type)==bcmSwitchL3UcLargeSNAT ?     L3UC_SNATf : \
     (type)==bcmSwitchL3UcLargeDNATSNAT ? L3UC_DNAT_SNATf : \
     (type)==bcmSwitchL3UcLargeDNAPT ?    L3UC_DNAPTf : \
     (type)==bcmSwitchL3UcLargeSNAPT ?    L3UC_SNAPTf : \
                                          L3UC_DNAPT_SNAPTf) 

#define LARGE_SCALE_NAT_MC_FIELD_ID(type) \
    ((type)==bcmSwitchL3McLargeNATRewriteType0 ? L3MC_RW_TYPE_0_SELf : \
     (type)==bcmSwitchL3McLargeNATRewriteType1 ? L3MC_RW_TYPE_1_SELf : \
                                                 L3MC_RW_TYPE_2_SELf)

#define LARGE_SCALE_NAT_MAC_UPDATE_FIELD_ID(type) \
    ((type)==bcmSwitchL3McLargeNATRewriteType0 ? L3MC_RW_TYPE_0_MAC_DA_UPDATEf : \
     (type)==bcmSwitchL3McLargeNATRewriteType1 ? L3MC_RW_TYPE_1_MAC_DA_UPDATEf : \
                                                 L3MC_RW_TYPE_2_MAC_DA_UPDATEf)

/* Helper routines for argument translation */

static int
_bool_invert(int unit, int arg, int set)
{
    /* Same for both set/get */
    return !arg;
}

#if defined(BCM_TRIDENT3_SUPPORT)
STATIC int _bcm_td3_switch_ifa_metadata_tbl_select (int unit, int arg, int set)
{
    int ret_value;

    if (set) {   /* set action */
        if (arg == BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE) {
            ret_value = 1;
        } else if (arg == BCM_SWITCH_IFA_METADATA_ADAPT1_DOUBLE) {
            ret_value = 0;
        } else if (arg == BCM_SWITCH_IFA_METADATA_ADAPT2_SINGLE) {
            ret_value = 3;
        } else if (arg == BCM_SWITCH_IFA_METADATA_ADAPT2_DOUBLE) {
            ret_value = 2;
        } else {
            ret_value = 0;
        }
    } else {
        if (arg == 1) {
            ret_value = BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE;
        } else if (arg == 0) {
            ret_value = BCM_SWITCH_IFA_METADATA_ADAPT1_DOUBLE;
        } else if (arg == 3) {
            ret_value = BCM_SWITCH_IFA_METADATA_ADAPT2_SINGLE;
        } else if (arg == 2) {
            ret_value = BCM_SWITCH_IFA_METADATA_ADAPT2_DOUBLE;
        } else {
            ret_value = BCM_SWITCH_IFA_METADATA_ADAPT1_DOUBLE;
        }
    }
    return ret_value;
}
#endif /* #if defined(BCM_TRIDENT3_SUPPORT) */

/*
 * Chip 0 means no TH or TD2P
 */
bcm_switch_binding_t trident3_bindings[] =
{

#if defined(BCM_TRIUMPH3_SUPPORT)
    {bcmSwitchWlanClientAuthorizeAll, 0,
        AXP_WRX_MASTER_CTRLr, WCD_DISABLEf,
        NULL, 0},
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    { bcmSwitchWlanClientUnauthToCpu, 0,
        CPU_CONTROL_Mr, WLAN_DOT1X_DROPf,
        NULL, 0},
    { bcmSwitchWlanClientRoamedOutErrorToCpu, 0,
        CPU_CONTROL_Mr, WLAN_ROAM_ERRORf,
        NULL, 0},
    { bcmSwitchWlanClientSrcMacMissToCpu, 0,
        CPU_CONTROL_Mr, WCD_SA_MISSf,
        NULL, 0},
    { bcmSwitchWlanClientDstMacMissToCpu, 0,
        CPU_CONTROL_Mr, WCD_DA_MISSf,
        NULL, 0},
    { bcmSwitchOamHeaderErrorToCpu, 1,
        CPU_CONTROL_Mr, OAM_HEADER_ERROR_TOCPUf,
        NULL, 0},
    { bcmSwitchOamUnknownVersionToCpu, 1,
        CPU_CONTROL_Mr, OAM_UNKNOWN_OPCODE_VERSION_TOCPUf,
        NULL, 0},
    { bcmSwitchL3SrcBindFailToCpu, 1,
        CPU_CONTROL_Mr, MAC_BIND_FAILf,
        NULL, 0},
    { bcmSwitchTunnelIp4IdShared, 1,
        EGR_TUNNEL_ID_MASKr, SHARED_FRAG_ID_ENABLEf,
        NULL, 0},
    { bcmSwitchIpfixRateViolationDataInsert, 0,
        ING_IPFIX_FLOW_RATE_CONTROLr, SUSPECT_FLOW_INSERT_DISABLEf,
        _bool_invert, soc_feature_ipfix_rate },
    { bcmSwitchIpfixRateViolationPersistent, 0,
        ING_IPFIX_FLOW_RATE_CONTROLr, SUSPECT_FLOW_CONVERT_DISABLEf,
        NULL, 0},
    { bcmSwitchCustomerQueuing, 0,
        ING_MISC_CONFIGr, PHB2_COS_MODEf,
        NULL, soc_feature_vlan_queue_map},
    { bcmSwitchOamUnknownVersionDrop,  1,
        OAM_DROP_CONTROLr, OAM_UNKNOWN_OPCODE_VERSION_DROPf,
        NULL, 0},
    { bcmSwitchDirectedMirroring, 0,
        EGR_PORT_64r, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, 0,
        IEGR_PORT_64r, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
#endif

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    { bcmSwitchOamUnexpectedPktToCpu, 1,
        CPU_CONTROL_Mr, OAM_UNEXPECTED_PKT_TOCPUf,
        NULL, 0},
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchOamCcmToCpu, 1,
        CCM_COPYTO_CPU_CONTROLr, ERROR_CCM_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchOamXconCcmToCpu, 1,
        CCM_COPYTO_CPU_CONTROLr, XCON_CCM_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchOamXconOtherToCpu, 1,
        CCM_COPYTO_CPU_CONTROLr, XCON_OTHER_COPY_TOCPUf,
        NULL, 0},
#endif /* !BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
    { bcmSwitchL3InterfaceUrpfEnable, 1,
       ING_CONFIG_64r, L3IIF_URPF_SELECTf,
        NULL, soc_feature_urpf },
#endif

#if defined(BCM_TRX_SUPPORT)
    { bcmSwitchClassBasedMoveFailPktToCpu, 1,
        CPU_CONTROL_1r, CLASS_BASED_SM_PREVENTED_TOCPUf,
        NULL, soc_feature_class_based_learning },
    { bcmSwitchL3UrpfFailToCpu,            1,
        CPU_CONTROL_1r, URPF_MISS_TOCPUf,
        NULL, soc_feature_urpf },
    { bcmSwitchClassBasedMoveFailPktDrop,  1,
        ING_MISC_CONFIG2r, CLASS_BASED_SM_PREVENTED_DROPf,
        NULL, soc_feature_class_based_learning },
    { bcmSwitchSTPBlockedFieldBypass,      1,
        ING_MISC_CONFIG2r, BLOCKED_PORTS_FP_DISABLEf,
        NULL, 0},
    { bcmSwitchRateLimitLinear,            1,
        MISCCONFIGr, ITU_MODE_SELf,
        _bool_invert, 0},
    { bcmSwitchRemoteLearnTrust,           1,
        ING_CONFIG_64r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchMldDirectAttachedOnly,      1,
        ING_CONFIG_64r, MLD_CHECKS_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanL2McastEnable,    1,
        ING_CONFIG_64r, LOOKUP_L2MC_WITH_FID_IDf,
        NULL, 0},
    { bcmSwitchMldUcastEnable,             1,
        ING_CONFIG_64r, MLD_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanEnable, 1,
        ING_CONFIG_64r, SVL_ENABLEf,
        NULL, 0},
    { bcmSwitchIgmpReservedMcastEnable,    1,
        ING_CONFIG_64r, IPV4_RESERVED_MC_ADDR_IGMP_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldReservedMcastEnable,     1,
        ING_CONFIG_64r, IPV6_RESERVED_MC_ADDR_MLD_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpUcastEnable,            1,
        ING_CONFIG_64r, IGMP_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchPortEgressBlockL3,          1,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L3f,
        NULL, 0},
    { bcmSwitchPortEgressBlockL2,          1,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L2f,
        NULL, 0},
    { bcmSwitchHgHdrExtLengthEnable,       1,
        ING_CONFIG_64r, IGNORE_HG_HDR_HDR_EXT_LENf,
        _bool_invert, 0 },
    { bcmSwitchIp4McastL2DestCheck,        1,
        ING_CONFIG_64r, IPV4_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchIp6McastL2DestCheck,        1,
        ING_CONFIG_64r, IPV6_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchL3TunnelUrpfMode,           1,
        ING_CONFIG_64r, TUNNEL_URPF_MODEf,
        NULL, soc_feature_urpf },
    { bcmSwitchL3TunnelUrpfDefaultRoute,   1,
        ING_CONFIG_64r, TUNNEL_URPF_DEFAULTROUTECHECKf,
        _bool_invert, soc_feature_urpf },
    { bcmSwitchMirrorStackMode, 1,
        ING_CONFIG_64r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, 1,
        ING_CONFIG_64r, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchMirrorSrcModCheck, 1,
        ING_CONFIG_64r, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchProxySrcKnockout, 1,
        IHG_LOOKUPr, REMOVE_MH_SRC_PORTf,
        NULL, soc_feature_proxy_port_property },
#endif /* BCM_1_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    { bcmSwitchDosAttackToCpu, 1,
        CPU_CONTROL_0r, DOSATTACK_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, 1,
        CPU_CONTROL_0r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchUnknownVlanToCpu, 1,
        CPU_CONTROL_0r, UVLAN_TOCPUf,
        NULL, 0 },
    { bcmSwitchSourceMacZeroDrop, 1,
        CPU_CONTROL_0r, MACSA_ALL_ZERO_DROPf,
        NULL, 0},
    { bcmSwitchMplsSequenceErrToCpu, 1,
        ING_MISC_CONFIGr, MPLS_SEQ_NUM_FAIL_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsLabelMissToCpu, 0,
        CPU_CONTROL_Mr, MPLS_LABEL_MISSf,
        NULL, 0},
    { bcmSwitchMplsTtlErrToCpu, 1,
        CPU_CONTROL_Mr, MPLS_TTL_CHECK_FAILf,
        NULL, 0},
    { bcmSwitchMplsInvalidL3PayloadToCpu, 0,
        CPU_CONTROL_Mr, MPLS_INVALID_PAYLOADf,
        NULL, 0},
    { bcmSwitchMplsInvalidActionToCpu, 0,
        CPU_CONTROL_Mr, MPLS_INVALID_ACTIONf,
        NULL, 0},
    { bcmSwitchSharedVlanMismatchToCpu, 0,
        CPU_CONTROL_0r, PVLAN_VID_MISMATCH_TOCPUf ,
        NULL, 0},
    { bcmSwitchForceForwardFabricTrunk, 0,
        ING_MISC_CONFIGr, LOCAL_SW_DISABLE_HGTRUNK_RES_ENf,
        NULL, 0},
    { bcmSwitchFieldMultipathHashSeed, 0,
        FP_ECMP_HASH_CONTROLr, ECMP_HASH_SALTf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange1, 0,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, 1,
        GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, 1,
       GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
       NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, 1,
        GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchL3UrpfRouteEnableExternal, 1,
       L3_DEFIP_RPF_CONTROLr, DEFIP_RPF_ENABLEf,
       NULL, soc_feature_urpf},
    { bcmSwitchMplsPWControlWordToCpu, 0,
       ING_MISC_CONFIGr, PWACH_TOCPUf,
       NULL, soc_feature_mpls},
    { bcmSwitchMplsPWControlTypeToCpu, 0,
       ING_MISC_CONFIGr, OTHER_CW_TYPE_TOCPUf,
       NULL, soc_feature_mpls},
    { bcmSwitchMplsPWCountPktsAll, 0,
       ING_MISC_CONFIGr, PW_COUNT_ALLf,
       NULL, soc_feature_mpls},
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchCpuToCpuEnable, 1,
        ING_MISC_CONFIGr, DO_NOT_COPY_FROM_CPU_TO_CPUf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
    { bcmSwitchCpuProtocolPrio, 0,
        CPU_CONTROL_2r, CPU_PROTOCOL_PRIORITYf,
        NULL, 0 },
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT)
    { bcmSwitchAlternateStoreForward, 0,
        ASFCONFIGr, ASF_ENf,
        NULL, soc_feature_asf },
   { bcmSwitchRemoteLearnTrust, 0,
        ING_CONFIGr, DO_NOT_LEARN_ENABLEf,
        NULL, soc_feature_remote_learn_trust },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchL3UrpfRouteEnable, 1,
        L3_DEFIP_RPF_CONTROLr, DEFIP_RPF_ENABLEf,
        NULL, soc_feature_urpf},
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchStationMoveOverLearnLimitToCpu, 1,
        CPU_CONTROL_1r, MACLMT_STNMV_TOCPUf,
        NULL, soc_feature_mac_learn_limit},
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchControlOpcodePriority, 0,
        CPU_CONTROL_2r, CPU_MH_CONTROL_PRIORITYf,
        NULL, 0 },
#endif /* BCM_BRADLEY_SUPPORT || BCM_RAPTOR_SUPPORT */


#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchBpduInvalidVlanDrop, 1,
        EGR_CONFIG_1_64r, BPDU_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMirrorInvalidVlanDrop, 1,
        EGR_CONFIG_1_64r, MIRROR_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMcastFloodBlocking, 1,
        IGMP_MLD_PKT_CONTROLr, PFM_RULE_APPLYf,
        NULL, soc_feature_igmp_mld_support },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchIpmcTtl1ToCpu, 1,
        CPU_CONTROL_1r, IPMC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcastTtl1ToCpu, 1,
        CPU_CONTROL_1r, L3UC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchDhcpLearn, 1,
        ING_MISC_CONFIG2r, DO_NOT_LEARN_DHCPf,
        NULL, 0 },
    { bcmSwitchUnknownIpmcAsMcast, 1,
        ING_MISC_CONFIG2r, IPMC_MISS_AS_L2MCf,
        NULL, 0 },
    { bcmSwitchTunnelUnknownIpmcDrop, 1,
        ING_MISC_CONFIG2r, UNKNOWN_TUNNEL_IPMC_DROPf,
        NULL, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchMcastUnknownErrToCpu, 1,
        CPU_CONTROL_1r, MC_INDEX_ERROR_TOCPUf,
        NULL,0},
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) ||\
    defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchDosAttackMACSAEqualMACDA, 1,
        DOS_CONTROLr, MACSA_EQUALS_MACDA_DROPf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackFlagZeroSeqZero, 1,
        DOS_CONTROLr, TCP_FLAGS_CTRL0_SEQ0_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackSynFrag, 1,
        DOS_CONTROLr, TCP_FLAGS_SYN_FRAG_ENABLEf,
        NULL, 0},
    { bcmSwitchDosAttackIcmp, 1,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmp, 1,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV4, 1,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6, 1,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpFragments, 1,
        DOS_CONTROL_2r, ICMP_FRAG_PKTS_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpOffset, 1,
        DOS_CONTROL_2r, TCP_HDR_OFFSET_EQ1_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackUdpPortsEqual, 1,
        DOS_CONTROL_2r, UDP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpPortsEqual, 1,
        DOS_CONTROL_2r, TCP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsSF, 1,
        DOS_CONTROL_2r, TCP_FLAGS_SYN_FIN_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsFUP, 1,
        DOS_CONTROL_2r, TCP_FLAGS_FIN_URG_PSH_SEQ0_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpHdrPartial, 1,
        DOS_CONTROL_2r, TCP_HDR_PARTIAL_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackV6MinFragEnable, 1,
        DOS_CONTROL_2r, IPV6_MIN_FRAG_SIZE_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined (BCM_TRIDENT_SUPPORT) || defined (BCM_KATANA_SUPPORT)
    { bcmSwitchHashNivSrcIfEtagSvidSelect0, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_0_SELECT_Af,
        NULL, 0 },
    { bcmSwitchHashNivSrcIfEtagSvidSelect1, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_0_SELECT_Bf,
        NULL, 0 },
    { bcmSwitchHashNivDstIfEtagVidSelect0, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_1_SELECT_Af,
        NULL, 0 },
    { bcmSwitchHashNivDstIfEtagVidSelect1, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_1_SELECT_Bf,
        NULL, 0 },
#endif /* BCM_TRIDENT_SUPPORT */


#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) ||\
    defined(BCM_BRADLEY_SUPPORT)
    { bcmSwitchDirectedMirroring, 0,
        ING_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDirectedMirroring, 1,
        EGR_PORTr, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, 1,
        IEGR_PORTr, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, 1,
        ING_CONFIGr, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, 0,
        MISCCONFIGr, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorSrcModCheck, 0,
        ING_CONFIGr, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorCopy, 0,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_COPYf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorOnly, 0,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_ONLY_PKTf,
        NULL, soc_feature_src_modid_blk },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) ||\
    defined(BCM_RAVEN_SUPPORT)
    { bcmSwitchDestPortHGTrunk, 0,
        ING_MISC_CONFIGr, USE_DEST_PORTf,
        NULL, soc_feature_port_trunk_index },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_HELIX_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchDirectedMirroring, 1,
        MISCCONFIGr, DRACO_1_5_MIRRORING_MODE_ENf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchMirrorUnmarked, 1,
        EGR_CONFIG_1_64r, RING_MODEf,
        _bool_invert, 0 },
    { bcmSwitchMirrorStackMode, 1,
        EGR_CONFIG_1_64r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMeterAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        FP_METER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        EFP_METER_CONTROL_2r, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        EGR_COUNTER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        EGR_COUNTER_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        MTRI_IFGr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchEgressBlockUcast, 1,
        ING_MISC_CONFIGr, APPLY_EGR_MASK_ON_UC_ONLYf,
        NULL, soc_feature_egress_blk_ucast_override },
    { bcmSwitchSourceModBlockUcast, 1,
        ING_MISC_CONFIGr, APPLY_SRCMOD_BLOCK_ON_UC_ONLYf,
        NULL, soc_feature_src_modid_blk_ucast_override },
    { bcmSwitchHgHdrMcastFloodOverride, 1,
        EGR_CONFIG_1_64r, FORCE_STATIC_MH_PFMf,
        NULL, soc_feature_static_pfm },
    { bcmSwitchShaperAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchShaperAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchShaperAdjust, 0,
        MTRI_IFGr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_HELIX_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchHgHdrErrToCpu, 1,
        CPU_CONTROL_1r, HG_HDR_ERROR_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchClassTagPacketsToCpu, 1,
        CPU_CONTROL_1r, HG_HDR_TYPE1_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchRemoteLearnTrust, 1,
        EGR_CONFIG_1_64r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchSourceModBlockControlOpcode, 1,
        ING_MISC_CONFIGr, DO_NOT_APPLY_SRCMOD_BLOCK_ON_SCf,
        _bool_invert, soc_feature_src_modid_blk_opcode_override },
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT)
    { bcmSwitchCpuProtoBpduPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_BPDU_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoArpPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_ARP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIgmpPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_IGMP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoDhcpPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_DHCP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIpmcReservedPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_IPMC_RESERVED_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIpOptionsPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_IP_OPTIONS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoExceptionsPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_EXCEPTIONS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchUnknownMcastToCpu, 1,
    	CPU_CONTROL_1r, UMC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownUcastToCpu, 1,
    	CPU_CONTROL_1r, UUCAST_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2StaticMoveToCpu, 1,
    	CPU_CONTROL_1r, STATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3HeaderErrToCpu, 1, /* compat */
    	CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3SrcToCpu, 1,
	    CPU_CONTROL_1r, UNRESOLVEDL3SRC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3DestToCpu, 1, /* compat */
    	CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcPortMissToCpu, 1,
    	CPU_CONTROL_1r, IPMCPORTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcErrorToCpu, 1,
    	CPU_CONTROL_1r, IPMCERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2NonStaticMoveToCpu, 1, 
	    CPU_CONTROL_1r, NONSTATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3ErrToCpu, 1,
     	CPU_CONTROL_1r, V6L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3DstMissToCpu, 1,
	    CPU_CONTROL_1r, V6L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3ErrToCpu, 1,
    	CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3DstMissToCpu, 1,
    	CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchTunnelErrToCpu, 1,
    	CPU_CONTROL_1r, TUNNEL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchMartianAddrToCpu, 1,
     	CPU_CONTROL_1r, MARTIAN_ADDR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcTtlErrToCpu, 1,
    	CPU_CONTROL_1r, L3UC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3SlowpathToCpu, 1,
    	CPU_CONTROL_1r, L3_SLOWPATH_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcTtlErrToCpu, 1,
    	CPU_CONTROL_1r, IPMC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchSampleIngressRandomSeed, 1,
	    SFLOW_ING_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchSampleEgressRandomSeed, 1,
    	SFLOW_EGR_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchArpReplyToCpu, 1,
	    PROTOCOL_PKT_CONTROLr, ARP_REPLY_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpReplyDrop, 1,
    	PROTOCOL_PKT_CONTROLr, ARP_REPLY_DROPf,
        NULL, 0 },
    { bcmSwitchArpRequestToCpu, 1,
    	PROTOCOL_PKT_CONTROLr, ARP_REQUEST_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpRequestDrop, 1,
    	PROTOCOL_PKT_CONTROLr, ARP_REQUEST_DROPf,
        NULL, 0 },
    { bcmSwitchNdPktToCpu, 1,
    	PROTOCOL_PKT_CONTROLr, ND_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchNdPktDrop, 1,
    	PROTOCOL_PKT_CONTROLr, ND_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDhcpPktToCpu, 1,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchDhcpPktDrop, 1,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDosAttackSipEqualDip, 1,
        DOS_CONTROLr, DROP_IF_SIP_EQUALS_DIPf,
        NULL, 0 },
    { bcmSwitchDosAttackV4FirstFrag, 1,
        DOS_CONTROLr, IPV4_FIRST_FRAG_CHECK_ENABLEf,
        NULL, 0 },
    { bcmSwitchNonIpL3ErrToCpu, 1, 
        CPU_CONTROL_1r, NIP_L3ERR_TOCPUf,
        NULL, soc_feature_nip_l3_err_tocpu },
    { bcmSwitchSourceRouteToCpu, 1,
	    CPU_CONTROL_1r, SRCROUTE_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, 0,
        CPU_CONTROL_1r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchDirectedMirroring, 1,
        EGR_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror},
#endif /* BCM_HELIX_SUPPORT || BCM_FIREBOLT_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAVEN_SUPPORT) ||  \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchL3MtuFailToCpu, 1,
        CPU_CONTROL_1r, L3_MTU_FAIL_TOCPUf,
        NULL, soc_feature_l3mtu_fail_tocpu },
    { bcmSwitchIpmcSameVlanL3Route, 1,
        EGR_CONFIG_1_64r, IPMC_ROUTE_SAME_VLANf,
        NULL, soc_feature_l3},
    { bcmSwitchIpmcSameVlanPruning, 1,
        MMU_SCFG_TOQ_MC_CFG0r, IPMC_IND_MODEf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    { bcmSwitchPrioDropToCpu, 1,
        CPU_CONTROL_0r, DOT1P_ADMITTANCE_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivPrioDropToCpu, 1,
        CPU_CONTROL_0r, DOT1P_ADMITTANCE_DROP_TOCPUf,
        NULL, soc_feature_niv},
#if 0 /* TD3TBD NIV_FORWARDING_DROP_TOCPUf is removed */
    { bcmSwitchNivInterfaceMissToCpu, 1,
        CPU_CONTROL_0r, NIV_FORWARDING_DROP_TOCPUf,
        NULL, soc_feature_niv},
#endif
    { bcmSwitchNivRpfFailToCpu, 1,
        CPU_CONTROL_0r, RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivTagInvalidToCpu, 1,
        CPU_CONTROL_0r, VNTAG_FORMAT_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivTagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_PRESENT_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivUntagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_NOT_PRESENT_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchTrillTtlErrToCpu, 1,
        CPU_CONTROL_0r, TRILL_HOPCOUNT_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillHeaderErrToCpu, 1,
        CPU_CONTROL_0r, TRILL_ERROR_FRAMES_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillMismatchToCpu, 1,
        CPU_CONTROL_0r, TRILL_UNEXPECTED_FRAMES_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillNameMissToCpu, 1,
        CPU_CONTROL_0r, TRILL_RBRIDGE_LOOKUP_MISS_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillRpfFailToCpu, 1,
        CPU_CONTROL_0r, TRILL_RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillOptionsToCpu, 1,
        CPU_CONTROL_0r, TRILL_OPTIONS_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillNameErrDrop, 1,
        TRILL_DROP_CONTROLr, INGRESS_RBRIDGE_EQ_EGRESS_RBRIDGE_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillHeaderVersionErrDrop, 1,
        TRILL_DROP_CONTROLr, TRILL_HDR_VERSION_NON_ZERO_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillNameMissDrop, 1,
        TRILL_DROP_CONTROLr, UNKNOWN_INGRESS_RBRIDGE_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillAdjacencyFailDrop, 1,
        TRILL_DROP_CONTROLr, TRILL_ADJACENCY_FAIL_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillHeaderErrDrop, 1,
        TRILL_DROP_CONTROLr, UC_TRILL_HDR_MC_MACDA_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillRpfFailDrop, 1,
        TRILL_DROP_CONTROLr, RPF_CHECK_FAIL_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchIngParseL2TunnelTermDipSipSelect, 1, 
        ING_L2_TUNNEL_PARSE_CONTROLr, IFP_L2_TUNNEL_PAYLOAD_FIELD_SELf ,
        NULL, 0},
    { bcmSwitchIngParseL3L4IPv4, 1,
        ING_L2_TUNNEL_PARSE_CONTROLr, IGMP_ENABLEf ,
        NULL, 0},
    { bcmSwitchIngParseL3L4IPv6, 1,
        ING_L2_TUNNEL_PARSE_CONTROLr, MLD_ENABLEf ,
        NULL, 0},
    /* This switch control is valid for TD+ only.
     * 'soc_feature_l3_ecmp_1k_groups' is used to
     * distinguish between TD and TD+ */
    { bcmSwitchEcmpMacroFlowHashEnable, 0,
        ING_CONFIG_2r, ECMP_HASH_16BITSf,
        NULL, soc_feature_l3_ecmp_1k_groups },
#endif /* BCM_TRIDENT_SUPPORT || BCM_KATANA_SUPPORT || BCM_KATANA2_SUPPORT*/

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
#if 0 /* TD3TBD NIV_FORWARDING_DROP_TOCPUf is removed */
    { bcmSwitchExtenderInterfaceMissToCpu, 1,
        CPU_CONTROL_0r, NIV_FORWARDING_DROP_TOCPUf,
        NULL, soc_feature_port_extension},
#endif
    { bcmSwitchExtenderRpfFailToCpu, 1,
        CPU_CONTROL_0r, RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchEtagInvalidToCpu, 1,
        CPU_CONTROL_0r, VNTAG_FORMAT_DROP_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchEtagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_PRESENT_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchNonEtagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_NOT_PRESENT_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchMplsGalAlertLabelToCpu, 1,
        CPU_CONTROL_Mr, MPLS_GAL_EXPOSED_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsRalAlertLabelToCpu, 1,
        CPU_CONTROL_Mr, MPLS_RAL_EXPOSED_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsIllegalReservedLabelToCpu, 1,
        CPU_CONTROL_Mr, MPLS_ILLEGAL_RSVD_LABEL_TO_CPUf,
        NULL, 0},
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchMplsUnknownAchTypeToCpu, 1,
        CPU_CONTROL_Mr, MPLS_UNKNOWN_ACH_TYPE_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsUnknownAchVersionToCpu, 1,
        CPU_CONTROL_Mr, MPLS_UNKNOWN_ACH_VERSION_TOCPUf,
        NULL, 0},
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchFailoverDropToCpu, 1,
        CPU_CONTROL_1r, PROTECTION_DATA_DROP_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsReservedEntropyLabelToCpu, 0,
        CPU_CONTROL_Mr, ENTROPY_LABEL_IN_0_15_RANGE_TO_CPUf,
        NULL, 0},
    { bcmSwitchL3SrcBindMissToCpu, 1,
        CPU_CONTROL_Mr, MAC_IP_BIND_LOOKUP_MISS_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsLookupsExceededToCpu, 0,
        CPU_CONTROL_Mr, MPLS_OUT_OF_LOOKUPS_TOCPUf,
        NULL, 0},
    { bcmSwitchTimesyncUnknownVersionToCpu, 1,
        CPU_CONTROL_Mr, UNKNOWN_1588_VERSION_TO_CPUf,
        NULL, 0},
    { bcmSwitchTimesyncIngressVersion, 1,
        ING_1588_PARSING_CONTROLr, VERSION_CONTROLf,
        NULL, 0},
    { bcmSwitchTimesyncEgressVersion, 1,
        EGR_1588_PARSING_CONTROLr, VERSION_CONTROLf,
        NULL, 0},
    { bcmSwitchCongestionCnmToCpu, 1,
        IE2E_CONTROLr, ICNM_TO_CPUf,
        NULL, 0},
    { bcmSwtichCongestionCnmProxyErrorToCpu, 1,
        ING_MISC_CONFIGr, QCN_CNM_PRP_DLF_TO_CPUf,
        NULL, 0},
    { bcmSwtichCongestionCnmProxyToCpu, 1,
        ING_MISC_CONFIGr, QCN_CNM_PRP_TO_CPUf,
        NULL, 0},
    { bcmSwitchL2GreTunnelMissToCpu, 1,
        CPU_CONTROL_Mr, L2GRE_SIP_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL2GreVpnIdMissToCpu, 1,
        CPU_CONTROL_Mr, L2GRE_VPNID_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchWlanTunnelErrorDrop, 0,
        ING_CONFIG_64r, WLAN_DECRYPT_OFFLOAD_ENABLEf,
        _bool_invert, 0},
    { bcmSwitchMplsReservedEntropyLabelDrop, 0,
        DROP_CONTROL_0r, ENTROPY_LABEL_IN_0_15_RANGE_DO_NOT_DROPf,
        _bool_invert, 0},
    { bcmSwitchRemoteProtectionTrust, 1,
        ING_CONFIG_64r, USE_PROT_STATUSf,
        NULL, 0},
    { bcmSwitchVxlanTunnelMissToCpu, 1,
        CPU_CONTROL_Mr, VXLAN_SIP_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchVxlanVnIdMissToCpu, 1,
        CPU_CONTROL_Mr, VXLAN_VN_ID_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchHashL2GreKeyMask0, 1,
        RTAG7_HASH_CONTROL_L2GRE_MASK_Ar, L2GRE_TUNNEL_GRE_KEY_MASK_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreKeyMask1, 1,
        RTAG7_HASH_CONTROL_L2GRE_MASK_Br, L2GRE_TUNNEL_GRE_KEY_MASK_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchRemoteProtectionEnable, 1 ,
        EGR_HG_HDR_PROT_STATUS_TX_CONTROLr, SET_PROT_STATUSf,
        NULL, 0},

#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchL3NATEnable, 1,
        ING_CONFIG_64r, NAT_ENABLEf,
        NULL, 0},
    { bcmSwitchL3DNATHairpinToCpu, 1,
        CPU_CONTROL_Mr, DNAT_HAIRPIN_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3DNATMissToCpu, 1,
        CPU_CONTROL_Mr, DNAT_MISS_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3SNATMissToCpu, 1,
        CPU_CONTROL_Mr, SNAT_MISS_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3NatOtherToCpu, 1,
        CPU_CONTROL_Mr, NAT_OTHER_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3NatRealmCrossingIcmpToCpu, 1,
        CPU_CONTROL_Mr, NAT_REALM_CROSSING_ICMP_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3NatFragmentsToCpu, 1,
        CPU_CONTROL_Mr, NAT_FRAGMENTS_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchHashUseL2GreTunnelGreKey0, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_TUNNEL_USE_GRE_KEY_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashUseL2GreTunnelGreKey1, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_TUNNEL_USE_GRE_KEY_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GrePayloadSelect0, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_PAYLOAD_HASH_SELECT_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GrePayloadSelect1, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_PAYLOAD_HASH_SELECT_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreNetworkPortPayloadDisable0, 1,
        RTAG7_HASH_CONTROL_2r, DISABLE_HASH_L2GRE_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreNetworkPortPayloadDisable1, 1,
        RTAG7_HASH_CONTROL_2r, DISABLE_HASH_L2GRE_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchEcmpMacroFlowHashEnable, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
    { bcmSwitchMcQueueSchedMode, 1,
        HSP_SCHED_PORT_CONFIGr, MC_GROUP_MODEf,
        NULL, 0},
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
    { bcmSwitchHashField0Ip6FlowLabel, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_FLOW_LABEL_IPV6_Af,
        NULL, 0},
    { bcmSwitchHashField1Ip6FlowLabel, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_FLOW_LABEL_IPV6_Bf,
        NULL, 0},
    { bcmSwitchMacroFlowHashUseMSB, 1,
       RTAG7_HASH_CONTROL_2r, MACRO_FLOW_HASH_BYTE_SELf,
        NULL, 0},
#endif
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    { bcmSwitchHashHg2PktFieldsEnable, 1,
       RTAG7_HASH_CONTROL_2r, TREAT_PPD2_AS_KNOWN_PPDf,
        NULL, 0},
    { bcmSwitchHashField0OverlayCntagRpidEnable, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Af,
        NULL, 0},
    { bcmSwitchMimPayloadTpidEnable, 1,
       RTAG7_HASH_CONTROL_2r, MIM_PAYLOAD_TPID_ENABLEf,
        NULL, soc_feature_mim},
    { bcmSwitchHashMimUseTunnelHeader0, 1,
       RTAG7_HASH_CONTROL_2r, DISABLE_HASH_MIM_INNER_L2_Af,
        NULL, soc_feature_mim},
    { bcmSwitchHashMimUseTunnelHeader1, 1,
       RTAG7_HASH_CONTROL_2r, DISABLE_HASH_MIM_INNER_L2_Bf,
        NULL, soc_feature_mim},
    { bcmSwitchHashField1OverlayCntagRpidEnable, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Bf,
        NULL, 0},
    { bcmSwitchHashMPLSUseLabelStack, 0,
       RTAG7_HASH_CONTROL_2r, USE_MPLS_STACK_FOR_HASHINGf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkUc , 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_UCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkNonUnicast , 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_NONUCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelEcmp, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    { bcmSwitchMplsPortIndependentLowerRange1, 1,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, 1,
        GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, 1,
        GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, 1,
        GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
        NULL, 0},
#endif
    { bcmSwitchHashUseFlowSelMplsEcmp, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_MPLS_ECMPf,
        NULL, 0},
    { bcmSwitchHashMPLSUseLabelStack, 1,
       RTAG7_HASH_CONTROLr, USE_MPLS_STACK_FOR_HASHINGf,
        NULL, 0},
#if defined(BCM_TRIDENT3_SUPPORT)
    { bcmSwitchMplsCtrlPktToCpu, 1,
        CPU_CONTROL_Mr, MPLS_CONTROL_PKT_TO_CPUf,
        NULL, 0},
    { bcmSwitchInvalidTpidToCpu, 1,
        CPU_CONTROL_1r, INVALID_TPID_TO_CPUf,
        NULL, 0},
    { bcmSwitchFlowSelectMissToCpu, 1,
        CPU_CONTROL_1r, PKT_FLOW_SELECT_MISS_TO_CPUf,
        NULL, 0},
    { bcmSwitchL3HdrMismatchToCpu, 1,
        CPU_CONTROL_1r, NON_L3_FLOW_TO_CPUf,
        NULL, 0},
    { bcmSwitchIngParseL2TunnelTermArp, 1, ING_L2_TUNNEL_PARSE_CONTROLr,
        PARSE_ARP_PAYLOADf, NULL, 0},
    { bcmSwitchIngParseL2TunnelTermRarp, 1, ING_L2_TUNNEL_PARSE_CONTROLr,
        PARSE_RARP_PAYLOADf, NULL, 0},
    { bcmSwitchGpeUdpDestPortSet, 1, GPE_CONTROLr,
        UDP_DEST_PORTf, NULL, soc_feature_flex_flow},
    { bcmSwitchGeneveUdpDestPortSet, 1, GENEVE_CONTROLr,
        UDP_DEST_PORTf, NULL, soc_feature_flex_flow},
    { bcmSwitchGpeEntropyEnable, 1, EGR_GPE_CONTROLr,
        UDP_SOURCE_PORT_SELf, NULL, soc_feature_flex_flow},
    { bcmSwitchGeneveEntropyEnable, 1, EGR_GENEVE_CONTROLr,
        UDP_SOURCE_PORT_SELf, NULL, soc_feature_flex_flow},
    { bcmSwitchL3UcLargeNATEnable, 1,
        LARGE_SCALE_NAT_CONTROLr, L3UC_NAT_ENABLEf,
        NULL, soc_feature_large_scale_nat},
    { bcmSwitchL3McLargeNATEnable, 1,
        LARGE_SCALE_NAT_CONTROLr, L3MC_NAT_ENABLEf,
        NULL, soc_feature_large_scale_nat},
    { bcmSwitchL2ChangeFieldsEnable, 1,
        EGR_FLEX_CONFIGr, ENABLE_CHANGE_L2_FIELDSf,
        NULL, soc_feature_vfi_switched_l2_change_fields},
    { bcmSwitchIFA2Enable, 1,
        ING_FLEX_CONFIGr, IFA_2_ENABLEf,
        NULL, soc_feature_enable_ifa_2},
    { bcmSwitchIFA2HeaderType, 1,
        IFA_CONTROLr, PROTOCOL_IDf,
        NULL, soc_feature_enable_ifa_2},
    { bcmSwitchIFA2MetaDataFirstLookupType, 1,
        EGR_FLEX_CONFIGr, METADATA_LOOKUP_1_TYPEf,
        NULL, soc_feature_enable_ifa_2},
    { bcmSwitchIFA2MetaDataSecondLookupType, 1,
        EGR_FLEX_CONFIGr, METADATA_LOOKUP_2_TYPEf,
        NULL, soc_feature_enable_ifa_2},
    { bcmSwitchIFAMetaDataAdaptTable, 1,
        EGR_FLEX_CONFIGr, IFA_METADATA_EGR_ADAPT_TABLE_SELECTf,
        _bcm_td3_switch_ifa_metadata_tbl_select,
        soc_feature_ifa_adapt_table_select},
    { bcmSwitchEgressVlanTranslate2Enable, 1,
        EGR_FLEX_CONFIGr, ENABLE_EVXLT2_FIXED_LOOKUPf,
        NULL, soc_feature_egr_vlan_xlate2_enable},
    { bcmSwitchVxlanIPv4TunnelVlanTranslateEnable, 1,
        EGR_FLEX_CONFIGr, VXLAN_IPV4_TUNNEL_VLAN_TRANS_ENABLEf,
        NULL, soc_feature_vxlan_tunnel_vlan_egress_translation},
    { bcmSwitchVxlanIPv6TunnelVlanTranslateEnable, 1,
        VXLAN_IPV6_CONTROLr, MSHG_ENABLEf,
        NULL, soc_feature_vxlan_tunnel_vlan_egress_translation},
#endif
#if defined(BCM_HELIX5_SUPPORT)
    { bcmSwitchDosAttackTcpFirstOption, 1,
        PARSER1_HVE_TCP_PARSE_CONTROLr, TCP_TLV_TYPEf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackInnerTcpFirstOption, 1,
        PARSER2_HVE_TCP_PARSE_CONTROLr, TCP_TLV_TYPEf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsMask, 1,
        PARSER1_HVE_TCP_PARSE_CONTROLr, TCP_FLAG_MASKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackInnerTcpFlagsMask, 1,
        PARSER2_HVE_TCP_PARSE_CONTROLr, TCP_FLAG_MASKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsSR, 1,
        DOS_CONTROL_3r, TCP_SYN_RST_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsZero, 1,
        DOS_CONTROL_3r, TCP_FLAG_ZERO_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsSynOption, 1,
        DOS_CONTROL_3r, TCP_SYN_WITH_DATA_PAYLOAD_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsSynFirstFrag, 1,
        DOS_CONTROL_3r, TCP_SYN_FIRST_FRAGMENT_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsFinNoAck, 1,
        DOS_CONTROL_3r, TCP_FIN_NO_ACK_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsRstNoAck, 1,
        DOS_CONTROL_3r, TCP_RST_NO_ACK_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsNoSFRA, 1,
        DOS_CONTROL_3r, TCP_NO_SYN_FIN_RST_ACK_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsUrgNoUrgPtr, 1,
        DOS_CONTROL_3r, TCP_URG_NO_URG_POINTER_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsNoUrgWithUrgPtr, 1,
        DOS_CONTROL_3r, TCP_NO_URG_URG_POINTER_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpDstPortZero, 1,
        DOS_CONTROL_3r, TCP_DPORT_ZERO_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpSrcPortZero, 1,
        DOS_CONTROL_3r, TCP_SPORT_ZERO_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsResvdNonZero, 1,
        DOS_CONTROL_3r, TCP_RESERVED_NON_ZERO_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpV4DstBcast, 1,
        DOS_CONTROL_3r, TCP_IP_BROADCAST_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchDosAttackTcpFlagsAck, 1,
        DOS_CONTROL_3r, TCP_ACK_ACKNUMBER_ZERO_CHECKf,
        NULL, soc_feature_broadscan_dos_ctrl},
    { bcmSwitchHashSctpL4Port, 1,
        ING_SCTP_CONTROLr, PARSE_SCTPf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelEcmpOverlay, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ECMP1f,
        NULL, 0},
    { bcmSwitchHashUseFlowSelEntropy, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ENTROPY_LABELf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelHigigTrunkUnicast, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_HG_TRUNK_UCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelHigigTrunkNonUnicast, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_HG_TRUNK_NONUCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelHigigTrunkFailover, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_HG_TRUNK_FAILOVERf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelLbidUnicast, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_LBID_UCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelLbidNonUnicast, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_LBID_NONUCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkResilientHash, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_RH_LAGf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelVpTrunk, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_VPLAGf,
        NULL, 0},
#endif
};

int
_bcm_td3_switch_control_port_binding_set(int unit,
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

    for (i = 0; i < COUNTOF(trident3_bindings); i++) {
        b = &trident3_bindings[i];
        /* b->chip == 1 check will be removed after we done bringup and final clean up */
        if ((b->type == type) && (b->chip == 1)) {
            /* We don't check the b->chip here since the table is specific for Greyhound*/
            if (b->feature && !soc_feature(unit, b->feature)) {
                continue;
            }
            if (!soc_reg_field_valid(unit, b->reg, b->field)) {
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

            if (type == bcmSwitchSharedVlanEnable) {
                BCM_IF_ERROR_RETURN(_bcm_td3_vlan_shared_vlan_enable_set(unit, arg));
            }

            *found = 1;
            break;
        }
    }
    return BCM_E_NONE;
}

int
_bcm_td3_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found)
{
    uint64 val64;
    int i;
    bcm_switch_binding_t *b;
    int prt, idx;

    *found = 0;

    for (i = 0; i < COUNTOF(trident3_bindings); i++) {
        b = &trident3_bindings[i];
        /* b->chip == 1 check will be removed after we done bringup and final clean up */
        if ((b->type == type) && (b->chip == 1)) {
            if (b->feature && !soc_feature(unit, b->feature)) {
                continue;
            }
            if (!SOC_REG_IS_VALID(unit, b->reg)) {
                continue;
            }

            if (!soc_reg_field_valid(unit, b->reg, b->field)) {
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

            if (type == bcmSwitchSharedVlanEnable) {
                BCM_IF_ERROR_RETURN(_bcm_td3_vlan_shared_vlan_enable_get(unit, arg));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
                *arg = soc_reg64_field32_get(unit, b->reg, val64, b->field);
            }

            if (b->xlate_arg) {
                *arg = (b->xlate_arg)(unit, *arg, 0);
            }
            *found = 1;
            break;
        }
    }
    return BCM_E_NONE;
}

/* Indexing of the memories EGR_SHAPING_CONTROL & EGR_COUNTER_CONTROL:
 * 3-bit EGR_LPORT_PROFILE.IFG_PROFILE_INDEX,5-bit IFG byte_cnt/l2 header offset
 * from Editor
 */
STATIC int
_bcm_td3_meter_index_get(int unit, bcm_port_t port, int *index)
{
    egr_lport_profile_entry_t egr_lport_profile_entry;
    uint32 ifg_profile_index;
    egr_port_entry_t egr_port_entry;
    int port_type, hdr_offset;

    SOC_IF_ERROR_RETURN(
        READ_EGR_LPORT_PROFILEm(unit, MEM_BLOCK_ANY, port,
                                &egr_lport_profile_entry));
    ifg_profile_index = soc_EGR_LPORT_PROFILEm_field32_get(unit,
                            &egr_lport_profile_entry, IFG_PROFILE_INDEXf);
    SOC_IF_ERROR_RETURN(
        READ_EGR_PORTm(unit, MEM_BLOCK_ANY, port, &egr_port_entry));
    port_type = soc_mem_field32_get(unit, EGR_PORTm, &egr_port_entry,
                                    PORT_TYPEf);

    if (port_type == 0) {
        /* Ethernet */
        *index = ifg_profile_index << 5;
    } else if (port_type == 1) {
        /* Higig */
        hdr_offset = 8;
        if ((soc_EGR_LPORT_PROFILEm_field32_get(unit,
                    &egr_lport_profile_entry, EH_EXT_HDR_ENABLEf)) == 1) {
            /* HG + EXT */
            hdr_offset = 10;
        }
        *index = (ifg_profile_index << 5) | hdr_offset;
    }

    return BCM_E_NONE;
}

/*
 * Update following registers/memories, field
 * EGR_COUNTER_CONTROLm, PACKET_IFG_BYTESf
 * EGR_SHAPING_CONTROLm, PACKET_IFG_BYTESf
 * IFP_METER_CONTROLr, PACKET_IFG_BYTESf
 */
int
_bcm_td3_meter_adjust_set(int unit, bcm_port_t port, int arg)
{
    int len, max_val;
    int i, index = 0;
    soc_mem_t mem[2] = { EGR_COUNTER_CONTROLm,
                         EGR_SHAPING_CONTROLm };

    if (!soc_feature(unit, soc_feature_meter_adjust)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            soc_field_t fields[2];
            uint32 values[2];
            uint32 two_compl=0;

            len = soc_mem_field_length(unit, mem[0], PACKET_IFG_BYTESf);
            max_val = (1 << len) - 1;
            if ((arg < ((-1) * max_val)) || (arg > max_val)) {
                return BCM_E_PARAM;
            }

            if (arg < 0) {
                values[1] = 1;
            }
            /* PACKET_IFG_BYTES is only 6 bytes in length. */
            /* 2's complement is to inverse the bits and then add 1. */
            two_compl = (~(arg & max_val)) + 1;
            /* We may get bigger value from 2's complement. scale it. */
            two_compl = (two_compl & max_val);

            fields[0] = PACKET_IFG_BYTESf;
            fields[1] = PACKET_IFG_SIGN_BITf;
            values[0] = two_compl;

            return bcm_esw_port_egr_lport_fields_set(
                       unit, port, EGR_COUNTER_CONTROLm, 2,
                       fields, values);
    }
#endif /* defined(BCM_HGPROXY_COE_SUPPORT) */


    BCM_IF_ERROR_RETURN(_bcm_td3_meter_index_get(unit, port, &index));

    for (i = 0; i < 2 ; ++i ) {
        len = soc_mem_field_length(unit, mem[i], PACKET_IFG_BYTESf);
        max_val = (1 << len) - 1;
        if (arg < 0 || arg > max_val) {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(
            soc_mem_field32_modify(unit, mem[i], index, PACKET_IFG_BYTESf,
                                   arg));
    }

    if (soc_feature(unit, soc_feature_ifp_meter_control_is_mem)) {
        len = soc_mem_field_length(unit, IFP_METER_CONTROLm, PACKET_IFG_BYTESf);
        max_val = (1 << len) - 1;
        if (arg < 0 || arg > max_val) {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(
            soc_mem_field32_modify(unit, IFP_METER_CONTROLm, port,
                PACKET_IFG_BYTESf, arg));
    } else {

        len = soc_reg_field_length(unit, IFP_METER_CONTROLr, PACKET_IFG_BYTESf);
        max_val = (1 << len) - 1;
        if (arg < 0 || arg > max_val) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, IFP_METER_CONTROLr, port,
                PACKET_IFG_BYTESf, arg));
    }

    return BCM_E_NONE;
}

int
_bcm_td3_meter_adjust_get(int unit, bcm_port_t port, int *arg)
{
    uint32 val;
    int index = 0;

    BCM_IF_ERROR_RETURN(_bcm_td3_meter_index_get(unit, port, &index));

    SOC_IF_ERROR_RETURN(
        READ_EGR_COUNTER_CONTROLm(unit, MEM_BLOCK_ANY, index, &val));
    *arg = soc_mem_field32_get(unit, EGR_COUNTER_CONTROLm, &val,
                               PACKET_IFG_BYTESf);

    return BCM_E_NONE;
}

int
_bcm_td3_shaper_adjust_set(int unit, bcm_port_t port, int arg)
{
    int index = 0;
    int len, max_val;
    soc_mem_t mem = EGR_SHAPING_CONTROLm;

    BCM_IF_ERROR_RETURN(_bcm_td3_meter_index_get(unit, port, &index));

    len = soc_mem_field_length(unit, mem, PACKET_IFG_BYTESf);
    max_val = (1 << len) - 1;
    if (arg < 0 || arg > max_val) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_field32_modify(unit, mem, index, PACKET_IFG_BYTESf, arg));

    return BCM_E_NONE;
}

int
_bcm_td3_shaper_adjust_get(int unit, bcm_port_t port, int *arg)
{
    uint32 val;
    int index = 0;

    BCM_IF_ERROR_RETURN(_bcm_td3_meter_index_get(unit, port, &index));

    SOC_IF_ERROR_RETURN(
        READ_EGR_SHAPING_CONTROLm(unit, MEM_BLOCK_ANY, index, &val));
    *arg = soc_mem_field32_get(unit, EGR_SHAPING_CONTROLm, &val,
                               PACKET_IFG_BYTESf);

    return BCM_E_NONE;
}

int
_bcm_td3_dosattack_pkt_size_set(int unit, bcm_switch_control_t type, int arg)
{
    int i = 0;
    int num_entries = 0;
    uint64 val64;
    uint32 val32;
    soc_reg_t reg[2];
    soc_field_t field[2];

    if (arg <= 0) {
        return BCM_E_PARAM;
    }
    switch(type) {
        case bcmSwitchDosAttackIcmpPktOversize:
            reg[0] = PARSER1_DOS_CONTROL_3r;
            field[0] = BIG_ICMP_PKT_SIZEf;
            reg[1] = PARSER2_DOS_CONTROL_3r;
            field[1] = BIG_ICMP_PKT_SIZEf;
            num_entries = 2;
            break;
        case bcmSwitchDosAttackMinTcpHdrSize:
            reg[0] = PARSER1_DOS_CONTROL_3r;
            field[0] = MIN_TCPHDR_SIZEf;
            reg[1] = PARSER2_DOS_CONTROL_3r;
            field[1] = MIN_TCPHDR_SIZEf;
            num_entries = 2;
            break;
        case bcmSwitchDosAttackIcmpV6PingSize:
            reg[0] = PARSER1_DOS_CONTROL_3r;
            field[0] = BIG_ICMPV6_PKT_SIZEf;
            reg[1] = PARSER2_DOS_CONTROL_3r;
            field[1] = BIG_ICMPV6_PKT_SIZEf;
            num_entries = 2;
            break;
        case bcmSwitchDosAttackV6MinFragSize:
            reg[0] = PARSER1_IPV6_MIN_FRAG_SIZEr;
            field[0] = PKT_LENGTHf;
            reg[1] = PARSER2_IPV6_MIN_FRAG_SIZEr;
            field[1] = PKT_LENGTHf;
            num_entries = 2;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    for (i = 0; i < num_entries; i++) {
        if(SOC_REG_IS_VALID(unit, reg[i])) {
            COMPILER_64_ZERO(val64);
            SOC_IF_ERROR_RETURN(soc_reg_get(unit,
                        reg[i], REG_PORT_ANY, 0, &val64));
            val32 = arg;
            soc_reg64_field32_set(unit, reg[i], &val64,
                    field[i], val32);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit,
                        reg[i], REG_PORT_ANY, 0, val64));
        }
    }

    return BCM_E_NONE;
}

int
_bcm_td3_dosattack_pkt_size_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32 val32;
    uint64 val64;
    soc_reg_t reg;
    soc_field_t field;

    switch(type) {
        case bcmSwitchDosAttackIcmpPktOversize:
            reg = PARSER1_DOS_CONTROL_3r;
            field = BIG_ICMP_PKT_SIZEf;
            break;
        case bcmSwitchDosAttackMinTcpHdrSize:
            reg = PARSER1_DOS_CONTROL_3r;
            field = MIN_TCPHDR_SIZEf;
            break;
        case bcmSwitchDosAttackIcmpV6PingSize:
            reg = PARSER1_DOS_CONTROL_3r;
            field = BIG_ICMPV6_PKT_SIZEf;
            break;
        case bcmSwitchDosAttackV6MinFragSize:
            reg = PARSER1_IPV6_MIN_FRAG_SIZEr;
            field = PKT_LENGTHf;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }
    if(SOC_REG_IS_VALID(unit, reg)) {
        COMPILER_64_ZERO(val64);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit,
                    reg, REG_PORT_ANY, 0, &val64));
        val32 = soc_reg64_field32_get(unit,
                reg, val64, field);
        *arg = val32;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_gbp_ethertype_set
 * Description:
 *      Set the ethertype for GBP
 * Parameters:
 *      unit        -  unit number
 *      arg         - argument to set as Ethertype
 * Returns:
 *      BCM_E_xxxx
 */
int
_bcm_td3_gbp_ethertype_set(int unit, int arg)
{
    uint32   rval;

    if (arg < 1 || arg > 0xffff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_ING_GBP_ETHERTYPEr(unit, &rval));
    /* Enable and Set ING GBP ethertype for parsing */
    soc_reg_field_set(unit, ING_GBP_ETHERTYPEr, &rval, ENABLEf, 1);
    soc_reg_field_set(unit, ING_GBP_ETHERTYPEr, &rval, ETHERTYPEf, arg);
    /* Set EGR GBP ethertype for editing */
    soc_reg_field_set(unit, EGR_GBP_ETHERTYPEr, &rval, ETHERTYPEf, arg);

    SOC_IF_ERROR_RETURN(WRITE_ING_GBP_ETHERTYPEr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_GBP_ETHERTYPEr(unit, rval));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ing_gbp_ethertype_get
 * Description:
 *      Get the ethertype field for GBP
 * Parameters:
 *      unit        - unit number
 *      arg         - Ethrtype to get
 * Returns:
 *      BCM_E_xxxx
 */

int
_bcm_td3_ing_gbp_ethertype_get(int unit, int *arg)
{
    uint32   rval;
    int      enable;
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_ING_GBP_ETHERTYPEr(unit, &rval));
    /* If ethertype enabled, return ethertype */
    enable = soc_reg_field_get(unit, ING_GBP_ETHERTYPEr, rval, ENABLEf);
    if (enable) {
        *arg = soc_reg_field_get(unit, ING_GBP_ETHERTYPEr, rval, ETHERTYPEf);
    } else {
        *arg = 0;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_custom_fields_set
 * Description:
 *      Set fields to parse custom packets
 * Parameters:
 *      unit        -  unit number
 *      arg         -  Value of fields set
 * Returns:
 *      BCM_E_xxxx
 */
int
_bcm_td3_custom_fields_set(int unit, bcm_switch_control_t type, int arg)
{
    uint32 val32;
    uint64 val64;
    int max_val, len;
    soc_reg_t reg;
    soc_field_t field;

    switch(type) {
        case bcmSwitchCustomOuterEthertype1:
            reg = PARSER1_USER_DEFINED_1_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomOuterEthertype2:
            reg = PARSER1_USER_DEFINED_2_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomInnerEthertype1:
            reg = PARSER2_USER_DEFINED_1_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomInnerEthertype2:
            reg = PARSER2_USER_DEFINED_2_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomOuterProtocolId1:
            reg = PARSER1_USER_DEFINED_1_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomOuterProtocolId2:
            reg = PARSER1_USER_DEFINED_2_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomInnerProtocolId1:
            reg = PARSER2_USER_DEFINED_1_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomInnerProtocolId2:
            reg = PARSER2_USER_DEFINED_2_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomOuterUdpDstPort1:
            reg = PARSER1_USER_DEFINED_1_UDP_DEST_PORTr;
            field = PORTf;
            break;
            case bcmSwitchCustomOuterUdpDstPort2:
                reg = PARSER1_USER_DEFINED_2_UDP_DEST_PORTr;
            field = PORTf;
            break;
        case bcmSwitchCustomInnerUdpDstPort1:
            reg = PARSER2_USER_DEFINED_1_UDP_DEST_PORTr;
            field = PORTf;
            break;
        case bcmSwitchCustomInnerUdpDstPort2:
            reg = PARSER2_USER_DEFINED_2_UDP_DEST_PORTr;
            field = PORTf;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    len = soc_reg_field_length(unit, reg, field);
    max_val = (1 << len) - 1;
    if (arg < 1 || arg > max_val) {
        return BCM_E_PARAM;
    }

    if(SOC_REG_IS_VALID(unit, reg)) {
        COMPILER_64_ZERO(val64);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit,
                    reg, REG_PORT_ANY, 0, &val64));
        val32 = arg;
    /* Enable and Set custom field value for parsing */
        soc_reg64_field32_set(unit, reg, &val64, ENABLEf, 1);
        soc_reg64_field32_set(unit, reg, &val64, field, val32);

        SOC_IF_ERROR_RETURN(soc_reg_set(unit,
                    reg, REG_PORT_ANY, 0, val64));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_custom_fields_get
 * Description:
 *      Get values of custom fields
 * Parameters:
 *      unit        - unit number
 *      arg         - Custom field value to get
 * Returns:
 *      BCM_E_xxxx
 */

int
_bcm_td3_custom_fields_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32 val32, enable;
    uint64 val64;
    soc_reg_t reg;
    soc_field_t field;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    switch(type) {
        case bcmSwitchCustomOuterEthertype1:
            reg = PARSER1_USER_DEFINED_1_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomOuterEthertype2:
            reg = PARSER1_USER_DEFINED_2_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomInnerEthertype1:
            reg = PARSER2_USER_DEFINED_1_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomInnerEthertype2:
            reg = PARSER2_USER_DEFINED_2_ETHERTYPEr;
            field = ETHERTYPEf;
            break;
        case bcmSwitchCustomOuterProtocolId1:
            reg = PARSER1_USER_DEFINED_1_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomOuterProtocolId2:
            reg = PARSER1_USER_DEFINED_2_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomInnerProtocolId1:
            reg = PARSER2_USER_DEFINED_1_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomInnerProtocolId2:
            reg = PARSER2_USER_DEFINED_2_IP_PROTOCOLr;
            field = PROTOCOL_IDf;
            break;
        case bcmSwitchCustomOuterUdpDstPort1:
            reg = PARSER1_USER_DEFINED_1_UDP_DEST_PORTr;
            field = PORTf;
            break;
            case bcmSwitchCustomOuterUdpDstPort2:
                reg = PARSER1_USER_DEFINED_2_UDP_DEST_PORTr;
            field = PORTf;
            break;
        case bcmSwitchCustomInnerUdpDstPort1:
            reg = PARSER2_USER_DEFINED_1_UDP_DEST_PORTr;
            field = PORTf;
            break;
        case bcmSwitchCustomInnerUdpDstPort2:
            reg = PARSER2_USER_DEFINED_2_UDP_DEST_PORTr;
            field = PORTf;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    if(SOC_REG_IS_VALID(unit, reg)) {
        COMPILER_64_ZERO(val64);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit,
                    reg, REG_PORT_ANY, 0, &val64));
        /* If enabled, return custom field value */
        enable = soc_reg64_field32_get(unit, reg, val64, ENABLEf);
        if (enable) {
            val32 = soc_reg64_field32_get(unit, reg, val64, field);
        } else {
            val32 = 0;
        }
        *arg = val32;
    }
    return BCM_E_NONE;
}

#if defined(BCM_HELIX5_SUPPORT)
/*
 * Function:
 *      _bcm_mcastgbp_default_classid_set
 * Description:
 *      Set the default class id for Mcast GBP SID_DID lookup miss
 * Parameters:
 *      unit        -  unit number
 *      arg         - argument to set as Default Classid
 * Returns:
 *      BCM_E_xxxx
 */
int
_bcm_mcastgbp_default_classid_set(int unit, int arg)
{
    int len, max_val;
    uint32   rval;

    BCM_IF_ERROR_RETURN(READ_EVXLT2_GBP_SID_DID_LOOKUP_MISS_POLICYr(unit, &rval));

    len = soc_reg_field_length(unit, EVXLT2_GBP_SID_DID_LOOKUP_MISS_POLICYr, CLASS_IDf);
    max_val = (1 << len) - 1;
    if (arg < 1 || arg > max_val) {
        return BCM_E_PARAM;
    }

    /* Program default class id when there is miss in
     * GBP SID_DID lookup for MC flows*/
    soc_reg_field_set(unit, EVXLT2_GBP_SID_DID_LOOKUP_MISS_POLICYr, &rval, CLASS_IDf, arg);
    SOC_IF_ERROR_RETURN(WRITE_EVXLT2_GBP_SID_DID_LOOKUP_MISS_POLICYr(unit, rval));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mcastgbp_default_classid_get
 * Description:
 *      Get the default class id for Mcast GBP SID_DID lookup miss
 * Parameters:
 *      unit        - unit number
 *      arg         - Default Classid to get
 * Returns:
 *      BCM_E_xxxx
 */

int
_bcm_mcastgbp_default_classid_get(int unit, int *arg)
{
    uint32   rval;
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_EVXLT2_GBP_SID_DID_LOOKUP_MISS_POLICYr(unit, &rval));
    /* Return the value of default Class Id */
    *arg = soc_reg_field_get(unit, EVXLT2_GBP_SID_DID_LOOKUP_MISS_POLICYr, rval, CLASS_IDf);
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_td3_vxlan_udp_source_port_sel_set
 * Purpose:
 *      Select UDP Source port for VXLAN
 * Parameters:
 *      unit - SOC unit number.
 *      mode - Selection mode of UDP SourcePort.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_vxlan_udp_source_port_sel_set(int unit, int mode)
{
    if ((mode < bcmSwitchTunnelUdpSrcPortStatic) ||
        (mode >= bcmSwitchTunnelUdpSrcPortCount) ) {
         return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, EGR_VXLAN_CONTROLr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_VXLAN_CONTROLr, USE_SOURCE_PORT_SELf)){
         SOC_IF_ERROR_RETURN(
             soc_reg_field32_modify(unit, EGR_VXLAN_CONTROLr,
                                    REG_PORT_ANY, USE_SOURCE_PORT_SELf, mode));
    }

    if (SOC_REG_IS_VALID(unit,FLEX_EDITOR_VXLAN_CONTROL_64r)) {
        SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, FLEX_EDITOR_VXLAN_CONTROL_64r,
                                   REG_PORT_ANY, VXLAN_FLAGSf, 0x8));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_vxlan_udp_source_port_sel_get
 * Purpose:
 *      Get UDP Source port selection mode for VXLAN
 * Parameters:
 *      unit - SOC unit number.
 *      mode - Selection mode of UDP SourcePort.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_vxlan_udp_source_port_sel_get(int unit, int *mode)
{
    int rv = BCM_E_NONE;
    uint64 reg64;

    COMPILER_64_ZERO(reg64);

    SOC_IF_ERROR_RETURN(READ_EGR_VXLAN_CONTROLr(unit, &reg64));
    *mode = soc_reg64_field32_get(unit, EGR_VXLAN_CONTROLr,
                                  reg64, USE_SOURCE_PORT_SELf);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_gpe_udp_source_port_sel_set
 * Purpose:
 *      Select UDP Source port for GPE
 * Parameters:
 *      unit - SOC unit number.
 *      mode - Selection mode of UDP SourcePort.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_gpe_udp_source_port_sel_set(int unit, int mode)
{
    if ((mode < bcmSwitchTunnelUdpSrcPortStatic) ||
        (mode >= bcmSwitchTunnelUdpSrcPortCount) ) {
         return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, EGR_GPE_CONTROLr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROLr, UDP_SOURCE_PORT_SELf)){
         SOC_IF_ERROR_RETURN(
             soc_reg_field32_modify(unit, EGR_GPE_CONTROLr,
                                    REG_PORT_ANY, UDP_SOURCE_PORT_SELf, mode));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_gpe_udp_source_port_sel_get
 * Purpose:
 *      Get UDP Source port selection mode for GPE
 * Parameters:
 *      unit - SOC unit number.
 *      mode - Selection mode of UDP SourcePort.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_gpe_udp_source_port_sel_get(int unit, int *mode)
{
    uint32 reg_val = 0;

    SOC_IF_ERROR_RETURN(READ_EGR_GPE_CONTROLr(unit, &reg_val));
    *mode = soc_reg_field_get(unit, EGR_GPE_CONTROLr,
                              reg_val, UDP_SOURCE_PORT_SELf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_geneve_udp_source_port_sel_set
 * Purpose:
 *      Select UDP Source port for GENEVE
 * Parameters:
 *      unit - SOC unit number.
 *      mode - Selection mode of UDP SourcePort.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_geneve_udp_source_port_sel_set(int unit, int mode)
{
    if ((mode < bcmSwitchTunnelUdpSrcPortStatic) ||
        (mode >= bcmSwitchTunnelUdpSrcPortCount) ) {
         return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, EGR_GENEVE_CONTROLr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_GENEVE_CONTROLr, UDP_SOURCE_PORT_SELf)){
         SOC_IF_ERROR_RETURN(
             soc_reg_field32_modify(unit, EGR_GENEVE_CONTROLr,
                                    REG_PORT_ANY, UDP_SOURCE_PORT_SELf, mode));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_geneve_udp_source_port_sel_get
 * Purpose:
 *      Get UDP Source port selection mode for GENEVE
 * Parameters:
 *      unit - SOC unit number.
 *      mode - Selection mode of UDP SourcePort.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_geneve_udp_source_port_sel_get(int unit, int *mode)
{
    uint32 reg_val = 0;

    SOC_IF_ERROR_RETURN(READ_EGR_GENEVE_CONTROLr(unit, &reg_val));
    *mode = soc_reg_field_get(unit, EGR_GENEVE_CONTROLr,
                              reg_val, UDP_SOURCE_PORT_SELf);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td3_ipv6_reserved_multicast_addr_multi_set
 * Description:
 *     Set multiple ipv6 reserved multicast address and mask.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      inner            - (IN) inner or outer ipv6 header
 *      index            - (IN) table entry
 *      ip6_addr         - (IN) ipv6 address
 *      ip6_mask         - (IN) ipv6 addres mask
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_td3_ipv6_reserved_multicast_addr_multi_set(int unit,
                                          int inner,
                                          int index,
                                          bcm_ip6_t ip6_addr,
                                          bcm_ip6_t ip6_mask)
{
    soc_mem_t mem;
    uint32    ip6_fields[4];
    uint32    entry_buf[SOC_MAX_MEM_FIELD_WORDS];

    if (inner) {
        mem = PARSER2_ING_IPV6_MC_RESERVED_ADDRESSm;
    }
    else {
        mem = PARSER1_ING_IPV6_MC_RESERVED_ADDRESSm;
    }
    /* clear entry memory */
    sal_memset(entry_buf, 0, sizeof(entry_buf));
    /* encode v6 address into ip6_fields */
    SAL_IP6_ADDR_TO_UINT32(ip6_addr, ip6_fields);
    /* set address in entry */
    soc_mem_field_set(unit, mem, entry_buf, ADDRESSf, &(ip6_fields[0]));
    /* encode v6 mask into ip6_fields */
    SAL_IP6_ADDR_TO_UINT32(ip6_mask, ip6_fields);
    /* set the mask in entry */
    soc_mem_field_set(unit, mem, entry_buf, MASKf, &(ip6_fields[0]));
    /* write entry to hardware */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                                      entry_buf));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td3_ipv6_reserved_multicast_addr_multi_get
 * Description:
 *     get multiple ipv6 reserved multicast address and mask
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      inner            - (IN) inner or outer ipv6 header
 *      index            - (IN) table entry
 *      ip6_addr         - (OUT) ipv6 address
 *      ip6_mask         - (OUT) ipv6 addres mask
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_td3_ipv6_reserved_multicast_addr_multi_get(int unit,
                                                int inner,
                                                int index,
                                                bcm_ip6_t *ip6_addr,
                                                bcm_ip6_t *ip6_mask)
{
    soc_mem_t mem;
    uint32    ip6_fields[4];
    uint32    entry_buf[SOC_MAX_MEM_FIELD_WORDS];

    if (inner) {
        mem = PARSER2_ING_IPV6_MC_RESERVED_ADDRESSm;
    }
    else {
        mem = PARSER1_ING_IPV6_MC_RESERVED_ADDRESSm;
    }
    /* clear entry memory */
    sal_memset(entry_buf, 0, sizeof(entry_buf));
    /* read entry from hardware */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry_buf));
    /* deduce the address from entry */
    soc_mem_field_get(unit, mem, entry_buf, ADDRESSf, &(ip6_fields[0]));
    /* set the address in user buf */
    SAL_IP6_ADDR_FROM_UINT32(*ip6_addr, ip6_fields);

    /* deduce the mask from entry */
    soc_mem_field_get(unit, mem, entry_buf, MASKf, &(ip6_fields[0]));
    /* set the mask in user buf */
    SAL_IP6_ADDR_FROM_UINT32(*ip6_mask, ip6_fields);

    return BCM_E_NONE;
}

#if defined(BCM_HELIX5_SUPPORT)
/*
 * Function:
 *     bcmi_hx5_switch_chip_info_get
 * Description:
 *     Get IC Id for the device.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      type             - (IN) Chip Info switch Enum
 *      max_size         - (IN) Max size of data_buf
 *      data_buf         - (OUT) data buffer to be populated
 *      actual_size      - (OUT) Actual size data in data buffer
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_hx5_switch_chip_info_get(int unit,
                              bcm_switch_chip_info_t info_type,
                              int max_size,
                              void *data_buf,
                              int *actual_size)
{
    int rv = BCM_E_UNAVAIL;

    switch(info_type) {
        case bcmSwitchChipInfoIcid:
            rv = soc_helix5_icid_get(unit,
                    max_size, (uint8 *) data_buf, actual_size);
            break;
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }

    return rv;
}
#endif

int _bcm_td3_large_scale_nat_uc_set(int unit,
                                    bcm_switch_control_t type,
                                    int arg)
{
    int val;
    soc_field_t fid;

    fid = LARGE_SCALE_NAT_UC_FIELD_ID(type);
    val = LARGE_SCALE_NAT_UC_ARG2REG(arg);

    if (val < 0) return BCM_E_PARAM;

    BCM_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, LARGE_SCALE_NAT_CONTROLr, REG_PORT_ANY, fid, val));

    return BCM_E_NONE;
}

int _bcm_td3_large_scale_nat_uc_get(int unit,
                                    bcm_switch_control_t type,
                                    int *arg)
{
    uint32 reg_val=0;
    uint32 field_val=0;
    soc_field_t fid;

    BCM_IF_ERROR_RETURN(READ_LARGE_SCALE_NAT_CONTROLr(unit, &reg_val));

    fid = LARGE_SCALE_NAT_UC_FIELD_ID(type);

    field_val = soc_reg_field_get(unit, LARGE_SCALE_NAT_CONTROLr, reg_val, fid);

    *arg = LARGE_SCALE_NAT_UC_REG2ARG(field_val);
    return BCM_E_NONE;
}

int _bcm_td3_large_scale_nat_mc_set(int unit,
                                    bcm_switch_control_t type,
                                    int arg)
{
    soc_field_t fid1 = LARGE_SCALE_NAT_MC_FIELD_ID(type);
    soc_field_t fid2 = LARGE_SCALE_NAT_MAC_UPDATE_FIELD_ID(type);
    uint32 da_update = LARGE_SCALE_NAT_DA_UPDATE(arg);
    int val;

    val = LARGE_SCALE_NAT_MC_ARG2REG(arg);

    if (val < 0) return BCM_E_PARAM;

    BCM_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, LARGE_SCALE_NAT_CONTROLr, REG_PORT_ANY, fid1, val));

    BCM_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, LARGE_SCALE_NAT_CONTROLr, REG_PORT_ANY, fid2, da_update));

    return BCM_E_NONE;
}

int _bcm_td3_large_scale_nat_mc_get(int unit,
                                    bcm_switch_control_t type,
                                    int *arg)
{
    uint32 reg_val=0;
    uint32 field_val=0;
    uint32 da_update=0;
    soc_field_t fid1 = LARGE_SCALE_NAT_MC_FIELD_ID(type);
    soc_field_t fid2 = LARGE_SCALE_NAT_MAC_UPDATE_FIELD_ID(type);

    BCM_IF_ERROR_RETURN(READ_LARGE_SCALE_NAT_CONTROLr(unit, &reg_val));

    field_val = soc_reg_field_get(unit, LARGE_SCALE_NAT_CONTROLr, reg_val, fid1);
    da_update = soc_reg_field_get(unit, LARGE_SCALE_NAT_CONTROLr, reg_val, fid2);

    *arg = LARGE_SCALE_NAT_MC_REG2ARG(field_val, da_update);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ioam_max_payload_length_set
 * Purpose:
 *      Set max length of IOAM payload
 * Parameters:
 *      unit - SOC unit number.
 *      payload_length - max length of IOAM payload.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_max_payload_length_set(int unit, int payload_length)
{
    if ((payload_length < 0) ||
        (payload_length > 0xFFFF)) {
        return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, IOAM_CONTROLr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, IOAM_CONTROLr, MAX_PAYLOAD_LENGTHf)){
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, IOAM_CONTROLr,
                    REG_PORT_ANY, MAX_PAYLOAD_LENGTHf, payload_length));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ioam_max_payload_length_get
 * Purpose:
 *      Get  max length of IOAM payload
 * Parameters:
 *      unit - SOC unit number.
 *      payload_length - max length of IOAM payload.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_max_payload_length_get(int unit, int *payload_length)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if (!SOC_REG_FIELD_VALID(unit, IOAM_CONTROLr, MAX_PAYLOAD_LENGTHf)) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_IOAM_CONTROLr(unit, &reg32));
    *payload_length = soc_reg_field_get(unit, IOAM_CONTROLr,
            reg32, MAX_PAYLOAD_LENGTHf);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_ioam_namespace_id_set
 * Purpose:
 *      Set namespace field in IOAM payload to match.
 * Parameters:
 *      unit - SOC unit number.
 *      namespace_id - Name spaceid in IOAM payload to match.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_namespaceid_set(int unit, int namespace_id)
{
    if ((namespace_id < 0) ||
        (namespace_id > 0xFFFF)) {
        return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, IOAM_CONTROLr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, IOAM_CONTROLr, NAMESPACE_IDf)){
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, IOAM_CONTROLr,
                    REG_PORT_ANY, NAMESPACE_IDf, namespace_id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ioam_namespace_id_get
 * Purpose:
 *      Get namespace field in IOAM payload to match.
 * Parameters:
 *      unit - SOC unit number.
 *      namespace_id - Name spaceid in IOAM payload to match.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_namespaceid_get(int unit, int *namespace_id)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if (!SOC_REG_FIELD_VALID(unit, IOAM_CONTROLr, NAMESPACE_IDf)) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_IOAM_CONTROLr(unit, &reg32));
    *namespace_id = soc_reg_field_get(unit, IOAM_CONTROLr,
            reg32, NAMESPACE_IDf);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_ioam_protocol_set
 * Purpose:
 *      Set ioam protocol value to identify IOAM header
 * Parameters:
 *      unit - SOC unit number.
 *      ioam_proto - Protocol value to identify IOAM header.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_protocol_set(int unit, int ioam_proto)
{
    if ((ioam_proto < 0) ||
        (ioam_proto > 0xFF)) {
        return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, GPE_NEXT_PROTOCOLr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, GPE_NEXT_PROTOCOLr, IOAMf)){
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, GPE_NEXT_PROTOCOLr,
                    REG_PORT_ANY, IOAMf, ioam_proto));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ioam_protocol_get
 * Purpose:
 *      Get ioam protocol value to identify IOAM header
 * Parameters:
 *      unit - SOC unit number.
 *      ioam_proto - Protocol value to identify IOAM header.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_protocol_get(int unit, int *ioam_proto)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if (!SOC_REG_FIELD_VALID(unit, GPE_NEXT_PROTOCOLr, IOAMf)) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_GPE_NEXT_PROTOCOLr(unit, &reg32));
    *ioam_proto = soc_reg_field_get(unit, GPE_NEXT_PROTOCOLr,
            reg32, IOAMf);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_v6linklocal_disposition_set
 * Purpose:
 *      Set disposition controls for handling V6 packets with
 *      Link local address.
 * Parameters:
 *      unit - SOC unit number.
 *      drop_control- Control chosen for drop behavior.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_v6linklocal_disposition_set(int unit, int drop_control)
{
    if ((drop_control < bcmSwitchV6L3LocalLinkSrcDrop) ||
        (drop_control >= bcmSwitchV6L3LocalLinkSelectCount)) {
        return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, IPV6_SIP_DIP_LINK_LOCAL_DROP_CONFIGr)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, IPV6_SIP_DIP_LINK_LOCAL_DROP_CONFIGr, DROPf)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, IPV6_SIP_DIP_LINK_LOCAL_DROP_CONFIGr,
                    REG_PORT_ANY, DROPf, drop_control));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_v6linklocal_disposition_get
 * Purpose:
 *      Get disposition controls for handling V6 packets with
 *      Link local address.
 * Parameters:
 *      unit - SOC unit number.
 *      drop_control- Control chosen for drop behavior
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_v6linklocal_disposition_get(int unit, int *drop_control)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if (!SOC_REG_FIELD_VALID(unit, IPV6_SIP_DIP_LINK_LOCAL_DROP_CONFIGr, DROPf)) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_IPV6_SIP_DIP_LINK_LOCAL_DROP_CONFIGr(unit, &reg32));
    *drop_control = soc_reg_field_get(unit, IPV6_SIP_DIP_LINK_LOCAL_DROP_CONFIGr,
            reg32, DROPf);
    return rv;
}
#if defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
/*
 * Function:
 *      _bcm_td3_ioam_e2e_protocol_set
 * Purpose:
 *      Set ioam protocol value to identify IOAM E2E header
 * Parameters:
 *      unit - SOC unit number.
 *      ioam_proto - Protocol value to identify IOAM E2E header.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_e2e_protocol_set(int unit, int ioam_proto)
{
    if ((ioam_proto < 0) ||
        (ioam_proto > 0xFF)) {
        return BCM_E_PARAM;
    }

    if ((!SOC_REG_IS_VALID(unit, GPE_NEXT_PROTOCOLr)) ||
        (!SOC_REG_IS_VALID(unit, EGR_GPE_CONTROL_1r))) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, GPE_NEXT_PROTOCOLr, IOAM_E2Ef)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, GPE_NEXT_PROTOCOLr,
                    REG_PORT_ANY, IOAM_E2Ef, ioam_proto));
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROL_1r,
             NEXT_PROTOCOL_IOAM_E2Ef)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EGR_GPE_CONTROL_1r,
                    REG_PORT_ANY, NEXT_PROTOCOL_IOAM_E2Ef, ioam_proto));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ioam_e2e_protocol_get
 * Purpose:
 *      Get ioam protocol value to identify IOAM E2E header
 * Parameters:
 *      unit - SOC unit number.
 *      ioam_proto - Protocol value to identify IOAM E2E header.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_e2e_protocol_get(int unit, int *ioam_proto)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if ((!SOC_REG_FIELD_VALID(unit, GPE_NEXT_PROTOCOLr, IOAM_E2Ef)) ||
        (!SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROL_1r, NEXT_PROTOCOL_IOAM_E2Ef))) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_GPE_NEXT_PROTOCOLr(unit, &reg32));
    *ioam_proto = soc_reg_field_get(unit, GPE_NEXT_PROTOCOLr,
            reg32, IOAM_E2Ef);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_gbp_type_set
 * Purpose:
 *      Set GBP protocol value.
 * Parameters:
 *      unit - SOC unit number.
 *      gbp_proto - GBP protocol value
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_gbp_type_set(int unit, int gbp_proto)
{
    if ((gbp_proto < 0) ||
        (gbp_proto > 0xFF)) {
        return BCM_E_PARAM;
    }

    if ((!SOC_REG_IS_VALID(unit, GPE_NEXT_PROTOCOLr)) ||
        (!SOC_REG_IS_VALID(unit, EGR_GPE_CONTROL_1r))) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, GPE_NEXT_PROTOCOLr, GBPf)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, GPE_NEXT_PROTOCOLr,
                    REG_PORT_ANY, GBPf, gbp_proto));
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROL_1r,
             NEXT_PROTOCOL_GBPf)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EGR_GPE_CONTROL_1r,
                    REG_PORT_ANY, NEXT_PROTOCOL_GBPf, gbp_proto));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gbp_type_get
 * Purpose:
 *      Get GBP protocol value from GPE header.
 * Parameters:
 *      unit - SOC unit number.
 *      gbp_proto - Get GBP protocol value
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_gbp_type_get(int unit, int *gbp_proto)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if ((!SOC_REG_FIELD_VALID(unit, GPE_NEXT_PROTOCOLr, GBPf)) ||
        (!SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROL_1r, NEXT_PROTOCOL_GBPf))) {
        return BCM_E_UNAVAIL;
    }
    SOC_IF_ERROR_RETURN(READ_GPE_NEXT_PROTOCOLr(unit, &reg32));
    *gbp_proto = soc_reg_field_get(unit, GPE_NEXT_PROTOCOLr,
            reg32, GBPf);
    return rv;
}

/*
 * Function:
 *      _bcm_td3_ioam_e2e_gbp_type_set
 * Purpose:
 *      Set GBP protocol value in IOAM_E2E header.
 * Parameters:
 *      unit - SOC unit number.
 *      gbp_proto - GBP protocol value
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_e2e_gbp_type_set(int unit, int gbp_proto)
{
    if ((gbp_proto < 0) ||
        (gbp_proto > 0xFF)) {
        return BCM_E_PARAM;
    }

    if ((!SOC_REG_IS_VALID(unit, GPE_IOAM_E2E_NEXT_PROTOCOLr)) ||
        (!SOC_REG_IS_VALID(unit, EGR_GPE_CONTROL_1r))) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, GPE_IOAM_E2E_NEXT_PROTOCOLr, GBPf)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, GPE_IOAM_E2E_NEXT_PROTOCOLr,
                    REG_PORT_ANY, GBPf, gbp_proto));
    }

    if (SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROL_1r,
             IOAM_E2E_NEXT_PROTOCOL_GBPf)) {
        SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EGR_GPE_CONTROL_1r,
                    REG_PORT_ANY, IOAM_E2E_NEXT_PROTOCOL_GBPf, gbp_proto));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ioam_e2e_gbp_type_get
 * Purpose:
 *      Get GBP protocol value from IOAM_E2E header.
 * Parameters:
 *      unit - SOC unit number.
 *      gbp_proto - Get GBP protocol value
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td3_ioam_e2e_gbp_type_get(int unit, int *gbp_proto)
{
    int rv = BCM_E_NONE;
    uint32 reg32 = 0;

    if ((!SOC_REG_FIELD_VALID(unit, GPE_IOAM_E2E_NEXT_PROTOCOLr, GBPf)) ||
        (!SOC_REG_FIELD_VALID(unit, EGR_GPE_CONTROL_1r, IOAM_E2E_NEXT_PROTOCOL_GBPf))) {
        return BCM_E_UNAVAIL;
    }
    SOC_IF_ERROR_RETURN(READ_GPE_IOAM_E2E_NEXT_PROTOCOLr(unit, &reg32));
    *gbp_proto = soc_reg_field_get(unit, GPE_IOAM_E2E_NEXT_PROTOCOLr,
            reg32, GBPf);
    return rv;
}

#endif /* BCM_HURRICANE4_SUPPORT & BCM_FIREBOLT6_SUPPORT */
