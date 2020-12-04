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
#include <bcm_int/esw/greyhound.h>
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw/hurricane3.h>
#include <soc/greyhound2.h>

#if defined(BCM_XGS3_SWITCH_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/link.h>
#include <bcm_int/esw/timesync.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw_dispatch.h>
#include <soc/higig.h>
#include <soc/scache.h>
#include <shared/shr_bprof.h>
#include <soc/pbsmh.h>
#include <bcm_int/esw/ipmc.h>
#include <soc/bradley.h>
#include <soc/hurricane3.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/qos.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/tsn.h>
#endif
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/rx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/common/time.h>
#include <bcm_int/esw/niv.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/common/xflow_macsec_cmn.h>

/* Valid RTAG7 hash field selection disable controls */
#define GH2_RTAG7_HASH_CONTROL_IS_VALID(flags)        \
    ((flags & BCM_HASH_FIELD0_DISABLE_IP4) ||         \
     (flags & BCM_HASH_FIELD1_DISABLE_IP4) ||         \
     (flags & BCM_HASH_FIELD0_DISABLE_IP6) ||         \
     (flags & BCM_HASH_FIELD1_DISABLE_IP6) ||         \
     (flags & BCM_HASH_FIELD0_DISABLE_VXLAN) ||       \
     (flags & BCM_HASH_FIELD1_DISABLE_VXLAN))

#define GH2_ICID_LENGTH_BYTES   32 /* ICID length in bytes */

/* Helper routines for argument translation */

static int
_bool_invert(int unit, int arg, int set)
{
    /* Same for both set/get */
    return !arg;
}

bcm_switch_binding_t greyhound2_bindings[] =
{
    { bcmSwitchOamHeaderErrorToCpu, 0,
        CPU_CONTROL_Mr, OAM_HEADER_ERROR_TOCPUf,
        NULL, 0 },
    { bcmSwitchOamUnknownVersionToCpu, 0,
        CPU_CONTROL_Mr, OAM_UNKNOWN_OPCODE_VERSION_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3SrcBindFailToCpu, 0,
        CPU_CONTROL_Mr, MAC_BIND_FAILf,
        NULL, 0},
    { bcmSwitchOamUnknownVersionDrop, 0,
        OAM_DROP_CONTROLr, OAM_UNKNOWN_OPCODE_VERSION_DROPf,
        NULL, 0},
    { bcmSwitchOamUnexpectedPktToCpu, 0,
        CPU_CONTROL_Mr, OAM_UNEXPECTED_PKT_TOCPUf,
        NULL, 0},
    { bcmSwitchOamCcmToCpu, 0,
        CCM_COPYTO_CPU_CONTROLr, ERROR_CCM_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchOamXconCcmToCpu, 0,
        CCM_COPYTO_CPU_CONTROLr, XCON_CCM_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchOamXconOtherToCpu, 0,
        CCM_COPYTO_CPU_CONTROLr, XCON_OTHER_COPY_TOCPUf,
        NULL, 0},

    { bcmSwitchSTPBlockedFieldBypass, 0,
        ING_MISC_CONFIG2r, BLOCKED_PORTS_FP_DISABLEf,
        NULL, 0 },
    { bcmSwitchRemoteLearnTrust, 0,
        ING_CONFIG_64r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchRemoteLearnTrust, 0,
        EGR_CONFIG_1r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchMldDirectAttachedOnly, 0,
        ING_CONFIG_64r, MLD_CHECKS_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanL2McastEnable, 0,
        ING_CONFIG_64r, LOOKUP_L2MC_WITH_FID_IDf,
        NULL, 0 },
    { bcmSwitchMldUcastEnable, 0,
        ING_CONFIG_64r, MLD_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanEnable, 0,
        ING_CONFIG_64r, SVL_ENABLEf,
        NULL, 0 },
    { bcmSwitchIgmpReservedMcastEnable, 0,
        ING_CONFIG_64r, IPV4_RESERVED_MC_ADDR_IGMP_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldReservedMcastEnable, 0,
        ING_CONFIG_64r, IPV6_RESERVED_MC_ADDR_MLD_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpUcastEnable, 0,
        ING_CONFIG_64r, IGMP_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchPortEgressBlockL3, 0,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L3f,
        NULL, 0 },
    { bcmSwitchPortEgressBlockL2, 0,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L2f,
        NULL, 0 },
    { bcmSwitchHgHdrExtLengthEnable, 0,
        ING_CONFIG_64r, IGNORE_HG_HDR_HDR_EXT_LENf,
        _bool_invert, 0 },
    { bcmSwitchIp4McastL2DestCheck, 0,
        ING_CONFIG_64r, IPV4_MC_MACDA_CHECK_ENABLEf,
        NULL, 0 },
    { bcmSwitchIp6McastL2DestCheck, 0,
        ING_CONFIG_64r, IPV6_MC_MACDA_CHECK_ENABLEf,
        NULL, 0 },
    { bcmSwitchMirrorStackMode, 0,
        ING_CONFIG_64r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, 0,
        EGR_CONFIG_1r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorSrcModCheck, 0,
        ING_CONFIG_64r, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },

    { bcmSwitchDosAttackToCpu, 0,
        CPU_CONTROL_0r, DOSATTACK_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, 0,
        CPU_CONTROL_0r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchUnknownVlanToCpu, 0,
        CPU_CONTROL_0r, UVLAN_TOCPUf,
        NULL, 0 },
    { bcmSwitchSourceMacZeroDrop, 0,
        CPU_CONTROL_0r, MACSA_ALL_ZERO_DROPf,
        NULL, 0},
    { bcmSwitchSharedVlanMismatchToCpu, 0,
        CPU_CONTROL_0r, PVLAN_VID_MISMATCH_TOCPUf,
        NULL, 0 },
    { bcmSwitchForceForwardFabricTrunk, 0,
        ING_MISC_CONFIGr, LOCAL_SW_DISABLE_HGTRUNK_RES_ENf,
        NULL, 0 },
    { bcmSwitchFieldMultipathHashSeed, 0,
        FP_ECMP_HASH_CONTROLr, ECMP_HASH_SALTf,
        NULL, 0 },
    { bcmSwitchCpuToCpuEnable, 0,
        ING_MISC_CONFIGr, DO_NOT_COPY_FROM_CPU_TO_CPUf,
        _bool_invert, 0 },

    { bcmSwitchStationMoveOverLearnLimitToCpu, 0,
        CPU_CONTROL_1r, MACLMT_STNMV_TOCPUf,
        NULL, soc_feature_mac_learn_limit },

    { bcmSwitchBpduInvalidVlanDrop, 0,
        EGR_CONFIG_1r, BPDU_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMirrorInvalidVlanDrop, 0,
        EGR_CONFIG_1r, MIRROR_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMcastFloodBlocking, 0,
        IGMP_MLD_PKT_CONTROLr, PFM_RULE_APPLYf,
        NULL, soc_feature_igmp_mld_support },

    { bcmSwitchIpmcTtl1ToCpu, 0,
        CPU_CONTROL_1r, IPMC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcastTtl1ToCpu, 0,
        CPU_CONTROL_1r, L3UC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchDhcpLearn, 0,
        ING_MISC_CONFIG2r, DO_NOT_LEARN_DHCPf,
        NULL, 0 },
    { bcmSwitchUnknownIpmcAsMcast, 0,
        ING_MISC_CONFIG2r, IPMC_MISS_AS_L2MCf,
        NULL, 0 },

    { bcmSwitchMcastUnknownErrToCpu, 0,
        CPU_CONTROL_1r, MC_INDEX_ERROR_TOCPUf,
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
    { bcmSwitchDosAttackIcmp, 0,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmp, 0,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV4, 0,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6, 0,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpFragments, 0,
        DOS_CONTROL_2r, ICMP_FRAG_PKTS_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
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
    { bcmSwitchDosAttackIcmpPktOversize, 0,
        DOS_CONTROLr, BIG_ICMP_PKT_SIZEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6PingSize, 0,
        DOS_CONTROL_2r, BIG_ICMPV6_PKT_SIZEf,
        NULL, soc_feature_big_icmpv6_ping_check },
    { bcmSwitchDosAttackMinTcpHdrSize, 0,
        DOS_CONTROLr, MIN_TCPHDR_SIZEf,
        NULL, 0 },

    { bcmSwitchSrcModBlockMirrorCopy, 0,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_COPYf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorOnly, 0,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_ONLY_PKTf,
        NULL, soc_feature_src_modid_blk },

    { bcmSwitchDestPortHGTrunk, 0,
        ING_MISC_CONFIGr, USE_DEST_PORTf,
        NULL, soc_feature_port_trunk_index },

    { bcmSwitchMirrorUnmarked, 0,
        EGR_CONFIG_1r, RING_MODEf,
        _bool_invert, 0 },
    { bcmSwitchMeterAdjust, 0,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        FP_METER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        EFP_METER_CONTROL_2r, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        EGR_COUNTER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        EGR_COUNTER_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchEgressBlockUcast, 0,
        ING_MISC_CONFIGr, APPLY_EGR_MASK_ON_UC_ONLYf,
        NULL, soc_feature_egress_blk_ucast_override },
    { bcmSwitchSourceModBlockUcast, 0,
        ING_MISC_CONFIGr, APPLY_SRCMOD_BLOCK_ON_UC_ONLYf,
        NULL, soc_feature_src_modid_blk_ucast_override },
    { bcmSwitchHgHdrMcastFloodOverride, 0,
        EGR_CONFIG_1r, FORCE_STATIC_MH_PFMf,
        NULL, soc_feature_static_pfm },
    { bcmSwitchShaperAdjust, 0,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },

    { bcmSwitchHgHdrErrToCpu, 0,
        CPU_CONTROL_1r, HG_HDR_ERROR_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchClassTagPacketsToCpu, 0,
        CPU_CONTROL_1r, HG_HDR_TYPE1_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchSourceModBlockControlOpcode, 0,
        ING_MISC_CONFIGr, DO_NOT_APPLY_SRCMOD_BLOCK_ON_SCf,
        _bool_invert, soc_feature_src_modid_blk_opcode_override },
    { bcmSwitchUnknownMcastToCpu, 0,
        CPU_CONTROL_1r, UMC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownUcastToCpu, 0,
        CPU_CONTROL_1r, UUCAST_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2StaticMoveToCpu, 0,
        CPU_CONTROL_1r, STATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3HeaderErrToCpu, 0, /* compat */
        CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3SrcToCpu, 0,
        CPU_CONTROL_1r, UNRESOLVEDL3SRC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3DestToCpu, 0, /* compat */
        CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcPortMissToCpu, 0,
        CPU_CONTROL_1r, IPMCPORTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcErrorToCpu, 0,
        CPU_CONTROL_1r, IPMCERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2NonStaticMoveToCpu, 0,
        CPU_CONTROL_1r, NONSTATICMOVE_TOCPUf,
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
    { bcmSwitchDosAttackSipEqualDip, 0,
        DOS_CONTROLr, DROP_IF_SIP_EQUALS_DIPf,
        NULL, 0 },
    { bcmSwitchDosAttackV4FirstFrag, 0,
        DOS_CONTROLr, IPV4_FIRST_FRAG_CHECK_ENABLEf,
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
    { bcmSwitchIpmcSameVlanL3Route, 0,
        EGR_CONFIG_1r, IPMC_ROUTE_SAME_VLANf,
        NULL, soc_feature_l3},
    { bcmSwitchIpmcSameVlanPruning, 0,
        MISCCONFIGr, IPMC_IND_MODEf,
        _bool_invert, 0 },

    { bcmSwitchPrioDropToCpu, 0,
        CPU_CONTROL_0r, DOT1P_ADMITTANCE_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivPrioDropToCpu, 0,
        CPU_CONTROL_0r, DOT1P_ADMITTANCE_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivInterfaceMissToCpu, 0,
        CPU_CONTROL_0r, NIV_FORWARDING_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivRpfFailToCpu, 0,
        CPU_CONTROL_0r, NIV_RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivTagInvalidToCpu, 0,
        CPU_CONTROL_0r, VNTAG_FORMAT_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivTagDropToCpu, 0,
        CPU_CONTROL_0r, DISCARD_VNTAG_PRESENT_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivUntagDropToCpu, 0,
        CPU_CONTROL_0r, DISCARD_VNTAG_NOT_PRESENT_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchV6L3LocalLinkDrop, 0,
        ING_MISC_CONFIG2r, IPV6_SIP_LINK_LOCAL_DROPf,
        NULL, 0},

    { bcmSwitchExtenderInterfaceMissToCpu, 0,
        CPU_CONTROL_0r, NIV_FORWARDING_DROP_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchExtenderRpfFailToCpu, 0,
        CPU_CONTROL_0r, NIV_RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchEtagDropToCpu, 0,
        CPU_CONTROL_0r, DISCARD_VNTAG_PRESENT_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchNonEtagDropToCpu, 0,
        CPU_CONTROL_0r, DISCARD_VNTAG_NOT_PRESENT_TOCPUf,
        NULL, soc_feature_port_extension},

    { bcmSwitchL3SrcBindMissToCpu, 0,
        CPU_CONTROL_Mr, MAC_IP_BIND_LOOKUP_MISS_TOCPUf,
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
    { bcmSwitchProxySrcKnockout, 0,
        IHG_LOOKUPr, REMOVE_MH_SRC_PORTf,
        NULL, soc_feature_proxy_port_property},
    { bcmSwitchDosAttackV6MinFragEnable, 0,
        DOS_CONTROL_2r, IPV6_MIN_FRAG_SIZE_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl},
    { bcmSwitchDosAttackV6MinFragSize, 0,
        IPV6_MIN_FRAG_SIZEr, PKT_LENGTHf,
        NULL, soc_feature_enhanced_dos_ctrl},

    { bcmSwitchL3InterfaceUrpfEnable, 0,
        ING_CONFIG_64r, L3IIF_URPF_SELECTf,
        NULL, soc_feature_urpf},
    { bcmSwitchL3UrpfFailToCpu, 0,
        CPU_CONTROL_1r, URPF_MISS_TOCPUf,
        NULL, soc_feature_urpf},
    { bcmSwitchL3UrpfRouteEnable, 0,
        L3_DEFIP_RPF_CONTROLr, DEFIP_RPF_ENABLEf,
        NULL, soc_feature_urpf},

    { bcmSwitchMimlEncapInnerEthertype, 0,
        EGR_MIML_ETHERTYPEr, ETHERTYPEf,
        NULL, soc_feature_miml},
    { bcmSwitchSTPDisabledFieldBypass, 0,
        ING_MISC_CONFIG2r, DISABLED_PORTS_FP_DISABLEf,
        NULL, 0 },

    /* RTAG7 related control setting */
    { bcmSwitchHashField0Ip6FlowLabel, 0,
       RTAG7_HASH_CONTROLr, ENABLE_FLOW_LABEL_IPV6_Af,
        NULL, 0},
    { bcmSwitchHashField1Ip6FlowLabel, 0,
       RTAG7_HASH_CONTROLr, ENABLE_FLOW_LABEL_IPV6_Bf,
        NULL, 0},
    { bcmSwitchHashNivSrcIfEtagSvidSelect0, 0,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_0_SELECT_Af,
        NULL, 0 },
    { bcmSwitchHashNivSrcIfEtagSvidSelect1, 0,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_0_SELECT_Bf,
        NULL, 0 },
    { bcmSwitchHashNivDstIfEtagVidSelect0, 0,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_1_SELECT_Af,
        NULL, 0 },
    { bcmSwitchHashNivDstIfEtagVidSelect1, 0,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_1_SELECT_Bf,
        NULL, 0 },
    { bcmSwitchHashField0OverlayCntagRpidEnable, 0,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Af,
        NULL, 0},
    { bcmSwitchHashField1OverlayCntagRpidEnable, 0,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Bf,
        NULL, 0},
    { bcmSwitchMacroFlowHashUseMSB, 0,
       RTAG7_HASH_CONTROL_2r, MACRO_FLOW_HASH_BYTE_SELf,
        NULL, 0},
    { bcmSwitchHashSctpL4Port, 0,
        ING_SCTP_CONTROLr, PARSE_SCTPf,
        NULL, 0},
    { bcmSwitchEcmpMacroFlowHashEnable, 0,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
    /*Doing the same config as bcmSwitchEcmpMacroFlowHashEnable*/
    { bcmSwitchHashUseFlowSelEcmp, 0,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkUc , 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_UCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelLbidNonUnicast, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_LBID_NONUCf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelLbidUnicast, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_LBID_UCf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelHigigTrunkNonUnicast, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_HG_TRUNK_NONUCf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelHigigTrunkUnicast, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_HG_TRUNK_UCf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelHigigTrunkFailover, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_HG_TRUNK_FAILOVERf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelTrunkNonUnicast, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_NONUCf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelEntropy, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_ENTROPYf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelVxlanEcmp, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_VXLAN_ECMPf,
        NULL, soc_feature_gh2_rtag7},
    { bcmSwitchHashUseFlowSelRoce2Ecmp, 0,
       RTAG7_HASH_SELr, USE_FLOW_SEL_ROCEV2_ECMPf,
        NULL, soc_feature_rroce},
    { bcmSwitchHashField0Roce1FlowLabel, 0,
       RTAG7_HASH_CONTROLr, ENABLE_ROCEV1_GRH_FLOW_LABEL_Af,
        NULL, soc_feature_rroce},
    { bcmSwitchHashField1Roce1FlowLabel, 0,
       RTAG7_HASH_CONTROLr, ENABLE_ROCEV1_GRH_FLOW_LABEL_Bf,
        NULL, soc_feature_rroce},
    { bcmSwitchHashField0Roce2FlowLabel, 0,
       RTAG7_HASH_CONTROLr, ENABLE_ROCEV2_FLOW_LABEL_Af,
        NULL, soc_feature_rroce},
    { bcmSwitchHashField1Roce2FlowLabel, 0,
       RTAG7_HASH_CONTROLr, ENABLE_ROCEV2_FLOW_LABEL_Bf,
        NULL, soc_feature_rroce},
    { bcmSwitchTunnelIp4IdShared, 0,
        EGR_TUNNEL_ID_MASKr, SHARED_FRAG_ID_ENABLEf,
        NULL, 0},
};


int
bcm_gh2_switch_control_port_binding_set(int unit,
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
    for (i = 0; i < COUNTOF(greyhound2_bindings); i++) {
        b = &greyhound2_bindings[i];
        if (b->type == type) {
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
                prt = index;
                idx = 0;
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
            BCM_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
            oval64 = val64;
            soc_reg64_field32_set(unit, b->reg, &val64, b->field,
                                ((uint32)fval > max) ? max : (uint32)fval);
            if (COMPILER_64_NE(val64, oval64)) {
                BCM_IF_ERROR_RETURN(soc_reg_set(unit, b->reg, prt, idx, val64));
            }

            *found = 1;
        }
    }
    return BCM_E_NONE;
}

int
bcm_gh2_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found)
{
    uint64 val64;
    int i;
    bcm_switch_binding_t *b;
    int prt, idx;

    *found = 0;

    for (i = 0; i < COUNTOF(greyhound2_bindings); i++) {
        b = &greyhound2_bindings[i];
        if (b->type == type) {
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
                prt = index;
                idx = 0;
            } else {
                prt = port;
                idx = 0;
            }
            BCM_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
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
 *      bcm_gh2_switch_rtag7_selectcontrol_set
 * Description:
 *      Set the enhanced (aka rtag 7) hash select control.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_selectcontrol_set(int unit, int arg)
{
    uint64  hash_control;
    uint32  val;

    /*
     * GH2 only supports IPv4, IPv6 and VXLAN hash field selection disable
     * control. IP over IP, GRE, MPLS and MIM are not supported.
     */
    if (arg) {
        if (!GH2_RTAG7_HASH_CONTROL_IS_VALID(arg)) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));

    /* IPv4 for hash block A/B */
    val = (arg & BCM_HASH_FIELD0_DISABLE_IP4 ) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_IPV4_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_IP4) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_IPV4_Bf, val);

    /* IPv6 for hash block A/B */
    val = (arg & BCM_HASH_FIELD0_DISABLE_IP6) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_IPV6_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_IP6) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_IPV6_Bf, val);

    /* VXLAN for hash block A/B */
    val = (arg & BCM_HASH_FIELD0_DISABLE_VXLAN ) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_VXLAN_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_VXLAN) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_VXLAN_Bf, val);

    BCM_IF_ERROR_RETURN(WRITE_RTAG7_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_selectcontrol_get
 * Description:
 *      Get the current enhanced (aka rtag 7) hash control settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_selectcontrol_get(int unit, int *arg)
{
    uint64  hash_control;
    uint32  val;

    *arg = 0;
    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));

    /* IPv4 for hash block A/B */
    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_IPV4_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_IP4;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_IPV4_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_IP4;

    /* IPv6 for hash block A/B */
    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_IPV6_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_IP6;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_IPV6_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_IP6;

    /* VXLAN for hash block A/B */
    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_VXLAN_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_VXLAN;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_VXLAN_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_VXLAN;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_switch_rtag7_extended_roce_selectcontrol_set
 * Description:
 *      Set the enhanced (aka rtag 7) hash select control.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */
int
bcmi_gh2_switch_rtag7_extended_roce_selectcontrol_set(int unit, int arg)
{
    uint64  hash_control;
    uint32  val;

    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));

    val = (arg & BCM_HASH_FIELD0_DISABLE_ROCE1) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_ROCEV1_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_ROCE1) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_ROCEV1_Bf, val);

    val = (arg & BCM_HASH_FIELD0_DISABLE_ROCE2_IP4) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_ROCEV2_IPV4_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_ROCE2_IP4) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_ROCEV2_IPV4_Bf, val);

    val = (arg & BCM_HASH_FIELD0_DISABLE_ROCE2_IP6) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_ROCEV2_IPV6_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_ROCE2_IP6) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_ROCEV2_IPV6_Bf, val);


    BCM_IF_ERROR_RETURN(WRITE_RTAG7_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_gh2_switch_rtag7_extended_roce_selectcontrol_get
 * Description:
 *      Get the current enhanced (aka rtag 7) hash control settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcmi_gh2_switch_rtag7_extended_roce_selectcontrol_get(int unit, int *arg)
{
    uint64  hash_control;
    uint32  val;

    *arg = 0;
    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_ROCEV1_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_ROCE1;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_ROCEV1_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_ROCE1;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_ROCEV2_IPV4_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_ROCE2_IP4;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_ROCEV2_IPV4_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_ROCE2_IP4;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_ROCEV2_IPV6_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_ROCE2_IP6;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_ROCEV2_IPV6_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_ROCE2_IP6;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_gh2_switch_rtag7_fieldconfig_set
 * Description:
 *      Set the enhanced (aka rtag 7) hash mode (A0/A1/B0/B1) config settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_fieldconfig_set(int unit,
                          bcm_switch_control_t type, int arg)
{
    soc_reg_t  reg;
    soc_field_t  field;
    uint64  hash_control;
    uint32  val;

    reg = RTAG7_HASH_CONTROL_3r;
    switch (type) {
        case bcmSwitchHashField0Config:
            field = HASH_A0_FUNCTION_SELECTf;
            break;
        case bcmSwitchHashField0Config1:
            field = HASH_A1_FUNCTION_SELECTf;
            break;
        case bcmSwitchHashField1Config:
            field = HASH_B0_FUNCTION_SELECTf;
            break;
        case bcmSwitchHashField1Config1:
            field = HASH_B1_FUNCTION_SELECTf;
            break;
        case bcmSwitchMacroFlowHashFieldConfig:
            reg = RTAG7_HASH_CONTROL_2r;
            field = MACRO_FLOW_HASH_FUNC_SELf;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    switch (arg) {
        case BCM_HASH_FIELD_CONFIG_CRC16XOR8:
            val = ENHANCE_XGS3_HASH_CRC16XOR8;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16XOR4:
            val = ENHANCE_XGS3_HASH_CRC16XOR4;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16XOR2:
            val = ENHANCE_XGS3_HASH_CRC16XOR2;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16XOR1:
            val = ENHANCE_XGS3_HASH_CRC16XOR1;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16:
            val = ENHANCE_XGS3_HASH_CRC16;
            break;
        case BCM_HASH_FIELD_CONFIG_XOR16:
            val = ENHANCE_XGS3_HASH_XOR16;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16CCITT:
            val = ENHANCE_XGS3_HASH_CRC16CCITT;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC32LO:
            val = ENHANCE_XGS3_HASH_CRC32LO;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC32HI:
            val = ENHANCE_XGS3_HASH_CRC32HI;
            break;
        default:
            val = 0;
            break;
    }

    if (!SOC_REG_FIELD_VALID(unit, reg, field)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &hash_control));
    soc_reg64_field32_set(unit, reg, &hash_control, field, val);
    BCM_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_fieldconfig_get
 * Description:
 *      Get the current enhanced (aka rtag 7) hash mode (A0/A1/B0/B1) config settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_fieldconfig_get(int unit,
                          bcm_switch_control_t type, int *arg)
{
    soc_reg_t  reg;
    soc_field_t  field;
    uint64  hash_control;
    uint32  val;

    reg = RTAG7_HASH_CONTROL_3r;
    switch (type) {
        case bcmSwitchHashField0Config:
            field = HASH_A0_FUNCTION_SELECTf;
            break;
        case bcmSwitchHashField0Config1:
            field = HASH_A1_FUNCTION_SELECTf;
            break;
        case bcmSwitchHashField1Config:
            field = HASH_B0_FUNCTION_SELECTf;
            break;
        case bcmSwitchHashField1Config1:
            field = HASH_B1_FUNCTION_SELECTf;
            break;
        case bcmSwitchMacroFlowHashFieldConfig:
            reg = RTAG7_HASH_CONTROL_2r;
            field = MACRO_FLOW_HASH_FUNC_SELf;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    if (!SOC_REG_FIELD_VALID(unit, reg, field)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &hash_control));
    val = soc_reg64_field32_get(unit, reg, hash_control, field);

    switch (val) {
        case ENHANCE_XGS3_HASH_CRC16XOR8:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR8;
            break;
        case ENHANCE_XGS3_HASH_CRC16XOR4:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR4;
            break;
        case ENHANCE_XGS3_HASH_CRC16XOR2:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR2;
            break;
        case ENHANCE_XGS3_HASH_CRC16XOR1:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR1;
            break;
        case ENHANCE_XGS3_HASH_CRC16:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16;
            break;
        case ENHANCE_XGS3_HASH_XOR16:
            *arg = BCM_HASH_FIELD_CONFIG_XOR16;
            break;
        case ENHANCE_XGS3_HASH_CRC16CCITT:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16CCITT;
            break;
        case ENHANCE_XGS3_HASH_CRC32LO:
            *arg = BCM_HASH_FIELD_CONFIG_CRC32LO;
            break;
        case ENHANCE_XGS3_HASH_CRC32HI:
            *arg = BCM_HASH_FIELD_CONFIG_CRC32HI;
            break;
        default:
            *arg = 0;
            break;
    }

    return BCM_E_NONE;
}

_hash_bmap_t hash_select_gh2_common[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 }
};

_hash_bmap_t hash_select_ip4_gh2[] =
{
    { BCM_HASH_FIELD_PROTOCOL,  0x0010 },
    { BCM_HASH_FIELD_DSTL4,     0x0020 },
    { BCM_HASH_FIELD_SRCL4,     0x0040 },
    { BCM_HASH_FIELD_VLAN,      0x0080 },
    { BCM_HASH_FIELD_IP4DST_LO, 0x0100 },
    { BCM_HASH_FIELD_IP4DST_HI, 0x0200 },
    { BCM_HASH_FIELD_IP4SRC_LO, 0x0400 },
    { BCM_HASH_FIELD_IP4SRC_HI, 0x0800 }
};

_hash_bmap_t hash_select_ip6_gh2[] =
{
    { BCM_HASH_FIELD_NXT_HDR,   0x0010 },
    { BCM_HASH_FIELD_DSTL4,     0x0020 },
    { BCM_HASH_FIELD_SRCL4,     0x0040 },
    { BCM_HASH_FIELD_VLAN,      0x0080 },
    { BCM_HASH_FIELD_IP6DST_LO, 0x0100 },
    { BCM_HASH_FIELD_IP6DST_HI, 0x0200 },
    { BCM_HASH_FIELD_IP6SRC_LO, 0x0400 },
    { BCM_HASH_FIELD_IP6SRC_HI, 0x0800 }
};

_hash_bmap_t hash_select_l2_gh2[] =
{
    { BCM_HASH_FIELD_VLAN,      0x0010 },
    { BCM_HASH_FIELD_ETHER_TYPE,0x0020 },
    { BCM_HASH_FIELD_MACDA_LO,  0x0040 },
    { BCM_HASH_FIELD_MACDA_MI,  0x0080 },
    { BCM_HASH_FIELD_MACDA_HI,  0x0100 },
    { BCM_HASH_FIELD_MACSA_LO,  0x0200 },
    { BCM_HASH_FIELD_MACSA_MI,  0x0400 },
    { BCM_HASH_FIELD_MACSA_HI,  0x0800 }
};

_hash_bmap_t hash_select_unknown_gh2[] =
{
    { BCM_HASH_FIELD_LOAD_BALANCE,0x0010 }
};

_hash_bmap_t hash_field_payload_control_gh2[] =
{
    { BCM_HASH_SELECT_INNER_L2, 0x0000 },
    { BCM_HASH_SELECT_INNER_L3, 0x0001 }
};

_hash_bmap_t hash_field_ipv6_collapse_control_gh2[] =
{
    { BCM_HASH_IP6_COLLAPSE_XOR, 0x0000},
    { BCM_HASH_IP6_COLLAPSE_LSB, 0x0001}
};

_hash_bmap_t hash_select_roce1_gh2[] =
{
    { BCM_HASH_ROCE_FIELD_BTH_DSTQP_COLLAPSED,   0x0010 },
    { BCM_HASH_ROCE_FIELD_BTH_PARTITION_KEY,     0x0020 },
    { BCM_HASH_ROCE_FIELD_VLAN,     0x0040 },
    { BCM_HASH_ROCE_FIELD_MACDA_LO, 0x0080 },
    { BCM_HASH_ROCE_FIELD_MACDA_MI, 0x0100 },
    { BCM_HASH_ROCE_FIELD_MACDA_HI, 0x0200 },
    { BCM_HASH_ROCE_FIELD_MACSA_LO, 0x0400 },
    { BCM_HASH_ROCE_FIELD_MACSA_MI, 0x0800 },
    { BCM_HASH_ROCE_FIELD_MACSA_HI, 0x1000 }
};

_hash_bmap_t hash_select_roce2_ip4_gh2[] =
{
    { BCM_HASH_ROCE_FIELD_BTH_DSTQP_COLLAPSED,   0x0010 },
    { BCM_HASH_ROCE_FIELD_BTH_PARTITION_KEY,     0x0020 },
    { BCM_HASH_ROCE_FIELD_DSTL4,     0x0040 },
    { BCM_HASH_ROCE_FIELD_SRCL4,     0x0080 },
    { BCM_HASH_ROCE_FIELD_VLAN,      0x0100 },
    { BCM_HASH_ROCE_FIELD_IP4DST_LO, 0x0200 },
    { BCM_HASH_ROCE_FIELD_IP4DST_HI, 0x0400 },
    { BCM_HASH_ROCE_FIELD_IP4SRC_LO, 0x0800 },
    { BCM_HASH_ROCE_FIELD_IP4SRC_HI, 0x1000 }
};

_hash_bmap_t hash_select_roce2_ip6_gh2[] =
{
    { BCM_HASH_ROCE_FIELD_BTH_DSTQP_COLLAPSED,   0x0010 },
    { BCM_HASH_ROCE_FIELD_BTH_PARTITION_KEY,     0x0020 },
    { BCM_HASH_ROCE_FIELD_DSTL4,     0x0040 },
    { BCM_HASH_ROCE_FIELD_SRCL4,     0x0080 },
    { BCM_HASH_ROCE_FIELD_VLAN,      0x0100 },
    { BCM_HASH_ROCE_FIELD_IP6DST_LO, 0x0200 },
    { BCM_HASH_ROCE_FIELD_IP6DST_HI, 0x0400 },
    { BCM_HASH_ROCE_FIELD_IP6SRC_LO, 0x0800 },
    { BCM_HASH_ROCE_FIELD_IP6SRC_HI, 0x1000 }
};



_hash_fieldselect_t hash_select_info_gh2[] =
{
    { bcmSwitchHashIP4Field0,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_HASH_FIELD_BMAP_1r, IPV4_FIELD_BITMAP_Af},
    { bcmSwitchHashIP4Field1,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_HASH_FIELD_BMAP_1r, IPV4_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP4TcpUdpField0,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2r, IPV4_TCP_UDP_FIELD_BITMAP_Af},
    { bcmSwitchHashIP4TcpUdpField1,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2r, IPV4_TCP_UDP_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP4TcpUdpPortsEqualField0,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1r,
        IPV4_TCP_UDP_SRC_EQ_DST_FIELD_BITMAP_Af},
    { bcmSwitchHashIP4TcpUdpPortsEqualField1,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1r,
        IPV4_TCP_UDP_SRC_EQ_DST_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP6Field0,
        hash_select_ip6_gh2, COUNTOF(hash_select_ip6_gh2),
        RTAG7_HASH_FIELD_BMAP_2r, IPV6_FIELD_BITMAP_Af},
    { bcmSwitchHashIP6Field1,
        hash_select_ip6_gh2, COUNTOF(hash_select_ip6_gh2),
        RTAG7_HASH_FIELD_BMAP_2r, IPV6_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP6TcpUdpField0,
        hash_select_ip6_gh2, COUNTOF(hash_select_ip6_gh2),
        RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2r, IPV6_TCP_UDP_SCTP_FIELD_BITMAP_Af},
    { bcmSwitchHashIP6TcpUdpField1,
        hash_select_ip6_gh2, COUNTOF(hash_select_ip6_gh2),
        RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2r, IPV6_TCP_UDP_SCTP_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP6TcpUdpPortsEqualField0,
        hash_select_ip6_gh2, COUNTOF(hash_select_ip6_gh2),
        RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1r,
        IPV6_TCP_UDP_SCTP_SRC_EQ_DST_FIELD_BITMAP_Af},
    { bcmSwitchHashIP6TcpUdpPortsEqualField1,
        hash_select_ip6_gh2, COUNTOF(hash_select_ip6_gh2),
        RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1r,
        IPV6_TCP_UDP_SCTP_SRC_EQ_DST_FIELD_BITMAP_Bf},
    { bcmSwitchHashL2Field0,
        hash_select_l2_gh2, COUNTOF(hash_select_l2_gh2),
        RTAG7_HASH_FIELD_BMAP_3r, L2_FIELD_BITMAP_Af},
    { bcmSwitchHashL2Field1,
        hash_select_l2_gh2, COUNTOF(hash_select_l2_gh2),
        RTAG7_HASH_FIELD_BMAP_3r, L2_FIELD_BITMAP_Bf},
    { bcmSwitchHashHG2UnknownField0,
        hash_select_unknown_gh2, COUNTOF(hash_select_unknown_gh2),
        RTAG7_HASH_FIELD_BMAP_5r, UNKNOWN_PPD_FIELD_BITMAP_Af},
    { bcmSwitchHashHG2UnknownField1,
        hash_select_unknown_gh2, COUNTOF(hash_select_unknown_gh2),
        RTAG7_HASH_FIELD_BMAP_5r, UNKNOWN_PPD_FIELD_BITMAP_Bf},
    { bcmSwitchHashL2VxlanField0,
        hash_select_l2_gh2, COUNTOF(hash_select_l2_gh2),
        RTAG7_VXLAN_PAYLOAD_L2_HASH_FIELD_BMAPr, VXLAN_PAYLOAD_L2_BITMAP_Af},
    { bcmSwitchHashL2VxlanField1,
        hash_select_l2_gh2, COUNTOF(hash_select_l2_gh2),
        RTAG7_VXLAN_PAYLOAD_L2_HASH_FIELD_BMAPr, VXLAN_PAYLOAD_L2_BITMAP_Bf},
    { bcmSwitchHashL3VxlanField0,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_VXLAN_PAYLOAD_L3_HASH_FIELD_BMAPr, VXLAN_PAYLOAD_L3_BITMAP_Af},
    { bcmSwitchHashL3VxlanField1,
        hash_select_ip4_gh2, COUNTOF(hash_select_ip4_gh2),
        RTAG7_VXLAN_PAYLOAD_L3_HASH_FIELD_BMAPr, VXLAN_PAYLOAD_L3_BITMAP_Bf},
    { bcmSwitchHashRoce1Field0,
        hash_select_roce1_gh2, COUNTOF(hash_select_roce1_gh2),
        RTAG7_ROCEV1_HASH_FIELD_BMAPr, ROCEV1_FIELD_BITMAP_Af},
    { bcmSwitchHashRoce1Field1,
        hash_select_roce1_gh2, COUNTOF(hash_select_roce1_gh2),
        RTAG7_ROCEV1_HASH_FIELD_BMAPr, ROCEV1_FIELD_BITMAP_Bf},
    { bcmSwitchHashRoce2IP4Field0,
        hash_select_roce2_ip4_gh2, COUNTOF(hash_select_roce2_ip4_gh2),
        RTAG7_ROCEV2_HASH_FIELD_BMAPr, ROCEV2_IPV4_FIELD_BITMAP_Af},
    { bcmSwitchHashRoce2IP4Field1,
        hash_select_roce2_ip4_gh2, COUNTOF(hash_select_roce2_ip4_gh2),
        RTAG7_ROCEV2_HASH_FIELD_BMAPr, ROCEV2_IPV4_FIELD_BITMAP_Bf},
    { bcmSwitchHashRoce2IP6Field0,
        hash_select_roce2_ip6_gh2, COUNTOF(hash_select_roce2_ip6_gh2),
        RTAG7_ROCEV2_HASH_FIELD_BMAPr, ROCEV2_IPV6_FIELD_BITMAP_Af},
    { bcmSwitchHashRoce2IP6Field1,
        hash_select_roce2_ip6_gh2, COUNTOF(hash_select_roce2_ip6_gh2),
        RTAG7_ROCEV2_HASH_FIELD_BMAPr, ROCEV2_IPV6_FIELD_BITMAP_Bf},
    { 0 }
};

_hash_fieldselect_t hash_select_control_gh2[] =
{
    { bcmSwitchHashVxlanPayloadSelect0,
       hash_field_payload_control_gh2, COUNTOF(hash_field_payload_control_gh2),
       RTAG7_HASH_CONTROLr, VXLAN_PAYLOAD_HASH_SELECT_Af},
    { bcmSwitchHashVxlanPayloadSelect1,
       hash_field_payload_control_gh2, COUNTOF(hash_field_payload_control_gh2),
       RTAG7_HASH_CONTROLr, VXLAN_PAYLOAD_HASH_SELECT_Bf},
    { bcmSwitchHashIP6AddrCollapseSelect0,
       hash_field_ipv6_collapse_control_gh2, COUNTOF(hash_field_ipv6_collapse_control_gh2),
       RTAG7_HASH_CONTROL_2r, IPV6_COLLAPSED_ADDR_SELECT_Af},
    { bcmSwitchHashIP6AddrCollapseSelect1,
       hash_field_ipv6_collapse_control_gh2, COUNTOF(hash_field_ipv6_collapse_control_gh2),
       RTAG7_HASH_CONTROL_2r, IPV6_COLLAPSED_ADDR_SELECT_Bf},
    { 0 }
};

/*
 * Function:
 *      bcm_gh2_switch_rtag7_fieldselect_set
 * Description:
 *      Set the enhanced (aka rtag 7) field select settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_fieldselect_set(int unit,
                          bcm_switch_control_t type, int arg)
{
    uint32 hash_control;
    uint64 hash_control_64;
    int  i, j, count, common_count;
    uint32 flags = 0;
    _hash_fieldselect_t *info, *base_info;
    _hash_bmap_t *common_bmap;

    common_bmap = hash_select_gh2_common;
    common_count = COUNTOF(hash_select_gh2_common);
    base_info = hash_select_info_gh2;
    count = COUNTOF(hash_select_info_gh2);

    for (i = 0; i < count; i++) {
        info = base_info + i;
        if (info->type != type) {
            continue;
        }

        for (j = 0; j < common_count; j++) {
            if (common_bmap[j].flag & arg) {
                flags |= common_bmap[j].hw_map;
            }
        }

        for (j = 0; j < info->size; j++) {
            if (info->map[j].flag & arg) {
                flags |= info->map[j].hw_map;
            }
        }
        if (SOC_REG_IS_ABOVE_32(unit, info->reg)) {
            COMPILER_64_ZERO(hash_control_64);
            BCM_IF_ERROR_RETURN(soc_reg64_get(unit, info->reg,
                                              REG_PORT_ANY, 0,
                                              &hash_control_64));
            soc_reg64_field32_set(unit, info->reg,
                                  &hash_control_64, info->field, flags);
            BCM_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->reg, REG_PORT_ANY,
                               0, hash_control_64));
        } else {
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, info->reg,
                               REG_PORT_ANY, 0, &hash_control));
            soc_reg_field_set(unit, info->reg,
                              &hash_control, info->field, flags);
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, info->reg, REG_PORT_ANY,
                               0, hash_control));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_fieldselect_get
 * Description:
 *      Get the current enhanced (aka rtag 7) field select settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_fieldselect_get(int unit,
                          bcm_switch_control_t type, int *arg)
{
    uint32 hash_control;
    uint64 hash_control_64;
    int  i, j, count, common_count;
    uint32 flags;
    _hash_fieldselect_t *info, *base_info;
    _hash_bmap_t *common_bmap;

    common_bmap = hash_select_gh2_common;
    common_count = COUNTOF(hash_select_gh2_common);
    base_info = hash_select_info_gh2;
    count = COUNTOF(hash_select_info_gh2);

    for (i = 0; i < count; i++) {
        info = base_info + i;
        if (info->type != type) {
            continue;
        }

        if (SOC_REG_IS_ABOVE_32(unit, info->reg)) {
            COMPILER_64_ZERO(hash_control_64);
            BCM_IF_ERROR_RETURN
                (soc_reg64_get(unit, info->reg,
                               REG_PORT_ANY, 0, &hash_control_64));
            flags = soc_reg64_field32_get(unit, info->reg,
                                          hash_control_64, info->field);
        } else {
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, info->reg,
                               REG_PORT_ANY, 0, &hash_control));
            flags = soc_reg_field_get(unit, info->reg,
                                      hash_control, info->field);
        }

        *arg = 0;
        for (j = 0; j < common_count; j++) {
            if (common_bmap[j].hw_map & flags) {
                *arg |= common_bmap[j].flag;
            }
        }

        for (j = 0; j < info->size; j++) {
            if (info->map[j].hw_map & flags) {
                *arg |= info->map[j].flag;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_field_control_set
 * Description:
 *      Set RTAG7 field controls for hash blocks.
 * Parameters:
 *      unit - unit no.
 *      type - switch control enum type.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_field_control_set(int unit,
                            bcm_switch_control_t type, int arg)
{
    uint64  hash_control;
    int  cnt, i;
    uint32  count = 0;
    _hash_fieldselect_t *base_info = NULL, *info;

    base_info = hash_select_control_gh2;
    count = COUNTOF(hash_select_control_gh2) - 1;

    if (0 == count) {
        return BCM_E_UNAVAIL;
    }

    for (i = 0; i < count; i++) {
        info = base_info + i;
        if (info->type != type) {
            continue;
        }

        for (cnt = 0; cnt < info->size; cnt++) {
            if (info->map[cnt].flag == arg) {
                break;
            }
        }
        if (info->size == cnt) {
            return BCM_E_PARAM;
        }

         BCM_IF_ERROR_RETURN(
             soc_reg_get(unit, info->reg, REG_PORT_ANY, 0, &hash_control));
         soc_reg64_field32_set(unit, info->reg,
             &hash_control, info->field, info->map[cnt].hw_map);
         BCM_IF_ERROR_RETURN(
             soc_reg_set(unit, info->reg, REG_PORT_ANY, 0, hash_control));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_field_control_get
 * Description:
 *      Get RTAG7 field controls for hash blocks.
 * Parameters:
 *      unit - unit no.
 *      type - switch control enum type.
 *      arg  - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_field_control_get(int unit,
                            bcm_switch_control_t type, int *arg)
{
    uint64  hash_control;
    uint32  val;
    int  cnt, i;
    uint32  count = 0;
    _hash_fieldselect_t *base_info = NULL, *info;

    base_info = hash_select_control_gh2;
    count = COUNTOF(hash_select_control_gh2) - 1;

    if (0 == count) {
        return BCM_E_UNAVAIL;
    }

    for (i = 0; i < count; i++) {
        info = base_info + i;
        if (info->type != type) {
            continue;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg_get(unit, info->reg, REG_PORT_ANY, 0, &hash_control));

        val = soc_reg64_field32_get(unit, info->reg, hash_control, info->field);

        for (cnt = 0; cnt < info->size; cnt++) {
            if (info->map[cnt].hw_map == val) {
                break;
            }
        }
        if (info->size == cnt) {
            return BCM_E_INTERNAL;
        }

        *arg = info->map[cnt].flag;

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_symmetric_hash_control_set
 * Description:
 *      Set the current symmetric (aka rtag 7) hash control settings for Ip4, Ip6 packet.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Symmetric hash control setting values (BCM_SYMMETRIC_HASH_XXX).
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_symmetric_hash_control_set(int unit, int arg)
{
    uint64  hash_control;
    uint32  valid_bit_mask, val;

    valid_bit_mask = (BCM_SYMMETRIC_HASH_0_IP4_ENABLE |
                      BCM_SYMMETRIC_HASH_1_IP4_ENABLE |
                      BCM_SYMMETRIC_HASH_0_IP6_ENABLE |
                      BCM_SYMMETRIC_HASH_1_IP6_ENABLE |
                      BCM_SYMMETRIC_HASH_0_SUPPRESS_UNIDIR_FIELD_ENABLE |
                      BCM_SYMMETRIC_HASH_1_SUPPRESS_UNIDIR_FIELD_ENABLE);

    if (arg & (~valid_bit_mask)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg_get(unit, RTAG7_HASH_CONTROLr, REG_PORT_ANY, 0, &hash_control));

    val = (arg & BCM_SYMMETRIC_HASH_0_IP4_ENABLE ) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          SYMMETRIC_HASH_IPV4_Af, val);

    val = (arg & BCM_SYMMETRIC_HASH_1_IP4_ENABLE) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          SYMMETRIC_HASH_IPV4_Bf, val);

    val = (arg & BCM_SYMMETRIC_HASH_0_IP6_ENABLE) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          SYMMETRIC_HASH_IPV6_Af, val);

    val = (arg & BCM_SYMMETRIC_HASH_1_IP6_ENABLE) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          SYMMETRIC_HASH_IPV6_Bf, val);

    val = (arg & BCM_SYMMETRIC_HASH_0_SUPPRESS_UNIDIR_FIELD_ENABLE) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          SYMMETRIC_HASH_SUPPRESS_UNIDIR_FIELDS_Af, val);

    val = (arg & BCM_SYMMETRIC_HASH_1_SUPPRESS_UNIDIR_FIELD_ENABLE) ? 1 : 0;
    soc_reg64_field32_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          SYMMETRIC_HASH_SUPPRESS_UNIDIR_FIELDS_Bf, val);

    BCM_IF_ERROR_RETURN(
        soc_reg_set(unit, RTAG7_HASH_CONTROLr, REG_PORT_ANY, 0, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_symmetric_hash_control_get
 * Description:
 *      Get the current symmetric (aka rtag 7) hash control settings for Ip4, Ip6 packet.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to the symmetric hash control setting values (BCM_SYMMETRIC_HASH_XXX).
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_symmetric_hash_control_get(int unit, int *arg)
{
    uint64  hash_control;
    uint32  val;

    *arg = 0;
    BCM_IF_ERROR_RETURN(
        soc_reg_get(unit, RTAG7_HASH_CONTROLr, REG_PORT_ANY, 0, &hash_control));

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                          SYMMETRIC_HASH_IPV4_Af);
    if (val) *arg |= BCM_SYMMETRIC_HASH_0_IP4_ENABLE;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                          SYMMETRIC_HASH_IPV4_Bf);
    if (val) *arg |= BCM_SYMMETRIC_HASH_1_IP4_ENABLE;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                          SYMMETRIC_HASH_IPV6_Af);
    if (val) *arg |= BCM_SYMMETRIC_HASH_0_IP6_ENABLE;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                          SYMMETRIC_HASH_IPV6_Bf);
    if (val) *arg |= BCM_SYMMETRIC_HASH_1_IP6_ENABLE;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                          SYMMETRIC_HASH_SUPPRESS_UNIDIR_FIELDS_Af);
    if (val) *arg |= BCM_SYMMETRIC_HASH_0_SUPPRESS_UNIDIR_FIELD_ENABLE;

    val = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                          SYMMETRIC_HASH_SUPPRESS_UNIDIR_FIELDS_Bf);
    if (val) *arg |= BCM_SYMMETRIC_HASH_1_SUPPRESS_UNIDIR_FIELD_ENABLE;

    return BCM_E_NONE;
}

/*
 * Enhanced hash bit selection/offset helper function
 */
STATIC int
_bcm_gh2_hash_offset(int unit, bcm_port_t port, bcm_switch_control_t type,
                     hash_offset_info_t *info)
{
     int index = -1;

    /*
     * For gh2, the index is got from PORT_TAB.RTAG7_PORT_PROFILE_INDEX or from
     * LPORT_PROFILE_TABLE.RTAG7_PORT_PROFILE_INDEX
     *
     * The index field will be ignored for LPORTs.
     */
    if (BCM_GPORT_IS_SET(port)) {
        info->idx = -1;
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_tab_get(unit, port, RTAG7_PORT_PROFILE_INDEXf, &index));
        info->idx = index;
    }

    switch (type) {
        case bcmSwitchTrunkHashSet0UnicastOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_TRUNK_UCf;
            info->offset_f = OFFSET_TRUNK_UCf;
            info->concat_f = CONCATENATE_HASH_FIELDS_TRUNK_UCf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchTrunkHashSet0NonUnicastOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_TRUNK_NONUCf;
            info->offset_f = OFFSET_TRUNK_NONUCf;
            info->concat_f = CONCATENATE_HASH_FIELDS_TRUNK_NONUCf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchFabricTrunkHashSet0UnicastOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_HG_TRUNK_UCf;
            info->offset_f = OFFSET_HG_TRUNK_UCf;
            info->concat_f = CONCATENATE_HASH_FIELDS_HG_TRUNK_UCf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_HG_TRUNK_NONUCf;
            info->offset_f = OFFSET_HG_TRUNK_NONUCf;
            info->concat_f = CONCATENATE_HASH_FIELDS_HG_TRUNK_NONUCf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchFabricTrunkFailoverHashOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_HG_TRUNK_FAILOVERf;
            info->offset_f = OFFSET_HG_TRUNK_FAILOVERf;
            info->concat_f = CONCATENATE_HASH_FIELDS_HG_TRUNK_FAILOVERf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchECMPVxlanHashOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_VXLAN_ECMPf;
            info->offset_f = OFFSET_VXLAN_ECMPf;
            info->concat_f = CONCATENATE_HASH_FIELDS_VXLAN_ECMPf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchLoadBalanceHashSet0UnicastOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_LBID_UCf;
            info->offset_f = OFFSET_LBID_UCf;
            info->concat_f = CONCATENATE_HASH_FIELDS_LBID_UCf;
            info->regmem =  RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_LBID_NONUCf;
            info->offset_f = OFFSET_LBID_NONUCf;
            info->concat_f = CONCATENATE_HASH_FIELDS_LBID_NONUCf;
            info->regmem =  RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchECMPHashSet0Offset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_ECMPf;
            info->offset_f = OFFSET_ECMPf;
            info->concat_f = CONCATENATE_HASH_FIELDS_ECMPf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchEntropyHashSet0Offset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_ENTROPYf;
            info->offset_f = OFFSET_ENTROPYf;
            info->concat_f = CONCATENATE_HASH_FIELDS_ENTROPYf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchEcmpRoce2HashOffset:
            info->hash_concat = 0;
            info->sub_f = SUB_SEL_ROCEV2_ECMPf;
            info->offset_f = OFFSET_ROCEV2_ECMPf;
            info->concat_f = CONCATENATE_HASH_FIELDS_ROCEV2_ECMPf;
            info->regmem = RTAG7_PORT_BASED_HASHm;
            break;
        case bcmSwitchTrunkFailoverHashOffset:
        case bcmSwitchFabricTrunkDynamicHashOffset:
        case bcmSwitchTrunkDynamicHashOffset:
        case bcmSwitchEcmpDynamicHashOffset:
        case bcmSwitchFabricTrunkResilientHashOffset:
        case bcmSwitchTrunkResilientHashOffset:
        case bcmSwitchEcmpResilientHashOffset:
        case bcmSwitchECMPUnderlayVxlanHashOffset:
        case bcmSwitchECMPL2GreHashOffset:
        case bcmSwitchECMPUnderlayL2GreHashOffset:
        case bcmSwitchECMPTrillHashOffset:
        case bcmSwitchECMPUnderlayTrillHashOffset:
        case bcmSwitchECMPMplsHashOffset:
        case bcmSwitchVirtualPortDynamicHashOffset:
        case bcmSwitchVirtualPortUnderlayDynamicHashOffset:
        case bcmSwitchECMPOverlayHashOffset:
        case bcmSwitchECMPUnderlayHashSet0Offset:
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_fieldoffset_set
 * Description:
 *      Set the enhanced (aka rtag 7) bits selection offset.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_fieldoffset_set(int unit, bcm_port_t port,
                          bcm_switch_control_t type, int arg)
{
    int  sub_field_width[8];
    int  total_width, offset = -1, i;
    hash_offset_info_t  info;
    uint32  hash_control_entry[SOC_MAX_MEM_WORDS];
    int field_count = 0;
    soc_field_t fields[3];
    uint32 values[3];

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_gh2_hash_offset(unit, port, type, &info));

    /* Hash Bits Sub-field Selection - not concatenated */
    sub_field_width[0] = 16;  /* HASH_A0 (16bit) */
    sub_field_width[1] = 16;  /* HASH_B0 (16bit) */
    sub_field_width[2] = 4;   /* LBN (4bit) */
    sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit) */
    sub_field_width[4] = 8;   /* MH.LBID / IRSEL local LBID (8 bit) */
    sub_field_width[5] = 8;   /* SW1 LBID ( 8 bit) */
    sub_field_width[6] = 16;  /* HASH_A1 (16 bit) */
    sub_field_width[7] = 16;  /* HASH_B1 (16 bit) */

    /* Get hash bits width */
    total_width = 0;
    for (i = 0; i < 8; i++) {
        total_width += sub_field_width[i];
    }
    /* Concatenate if offset value exceeds total hash width */
    if (arg >= total_width) {
        info.hash_concat = 1;
    }

    /* Concatenate if enforced or if offset value exceeds total hash width */
    if ((info.hash_concat == 1)  && (info.concat_f != -1)) {
        /* Concatenation hash computation order is referenced from
         *  fb_Irsel1_arch::compute_rtag7_hash
         */

        /* Hash Bits Sub-field Selection - concatenated */
        sub_field_width[0] = 64;  /* Concat B1,B0,A1,A0(64bit) */
        sub_field_width[1] = 0;   /* 0 (Reserved) */
        sub_field_width[2] = 4;   /* LBN (4bit) */
        sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit) */
        sub_field_width[4] = 8;   /* MH.LBID / IRSEL local LBID (8 bit) */
        sub_field_width[5] = 8;   /* SW1 LBID (8 bit) */
        sub_field_width[6] = 0;   /* 0 (Reserved) */
        sub_field_width[7] = 0;   /* 0 (Reserved) */

        /* Get concatenated hash bits width */
        total_width = 0;
        for (i = 0; i < 8; i++) {
            total_width += sub_field_width[i];
        }
    }

    /* Select hash sub select and hash bit offset */
    offset = arg % total_width;
    for (i = 0; i < 8; i++) {
         offset -= sub_field_width[i];
         if (offset < 0) {
             offset += sub_field_width[i];
             break;
         }
    }

    if (SOC_MEM_IS_VALID(unit, info.regmem)) {
        /* LPORT Profile */
        if (BCM_GPORT_IS_PROXY(port)) {
            fields[0] = info.sub_f;
            values[0] = i;
            field_count++;
            fields[1] =  info.offset_f;
            values[1] = offset;
            field_count++;
            if (info.concat_f != -1) {
                fields[2] = info.concat_f;
                values[2] = info.hash_concat;
                field_count++;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_lport_fields_set(unit, port,
                                               LPORT_PROFILE_RTAG7_TAB,
                                               field_count, fields, values));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, info.regmem,
                              MEM_BLOCK_ANY, info.idx, hash_control_entry));
            soc_mem_field32_set(unit,
                                info.regmem, hash_control_entry, info.sub_f, i);
            soc_mem_field32_set(unit,
                                info.regmem, hash_control_entry,
                                info.offset_f, offset);
            if (info.concat_f != -1) {
                soc_mem_field32_set(unit,
                                    info.regmem, hash_control_entry,
                                    info.concat_f, info.hash_concat);
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit,
                               info.regmem, MEM_BLOCK_ALL, info.idx,
                               hash_control_entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_fieldoffset_get
 * Description:
 *      Get the current enhanced (aka rtag 7) bit selection offset.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_gh2_switch_rtag7_fieldoffset_get(int unit, bcm_port_t port,
                          bcm_switch_control_t type, int *arg)
{
    int  sub_field_width[8];
    int  idx = 0, i;
    hash_offset_info_t  info;
    int  total_width = 0;
    int  concat = 0;
    uint32  hash_control_entry[SOC_MAX_MEM_WORDS];
    int field_count = 0;
    soc_field_t fields[3];
    uint32 values[3];

    BCM_IF_ERROR_RETURN(_bcm_gh2_hash_offset(unit, port, type, &info));

    if (SOC_MEM_IS_VALID(unit, info.regmem)) {
        /* LPORT Profile */
        if (BCM_GPORT_IS_PROXY(port)) {
            fields[0] = info.sub_f;
            field_count++;
            fields[1] = info.offset_f;
            field_count++;
            if (info.concat_f != -1) {
                fields[2] = info.concat_f;
                field_count++;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_lport_fields_get(unit, port,
                                               LPORT_PROFILE_RTAG7_TAB,
                                               field_count, fields, values));
            idx = values[0];
            *arg = values[1];
            if (info.concat_f != -1) {
                concat = values[2];
            }
        } else {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, info.regmem,
                              MEM_BLOCK_ANY, info.idx, hash_control_entry));
            idx = soc_mem_field32_get(unit, info.regmem,
                                      hash_control_entry, info.sub_f);
            *arg  = soc_mem_field32_get(unit, info.regmem,
                                        hash_control_entry, info.offset_f);
            if (info.concat_f != -1) {
                concat = soc_mem_field32_get(unit, info.regmem,
                                             hash_control_entry, info.concat_f);
            }
        }
    } else {
        return BCM_E_PARAM;
    }

    sub_field_width[0] = 16;  /* HASH_A0 (16bit) */
    sub_field_width[1] = 16;  /* HASH_B0 (16bit) */
    sub_field_width[2] = 4;   /* LBN (4bit) */
    sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit) */
    sub_field_width[4] = 8;   /* MH.LBID / IRSEL local LBID (8 bit) */
    sub_field_width[5] = 8;   /* SW1 LBID ( 8 bit)*/
    sub_field_width[6] = 16;  /* HASH_A1 (16 bit) */
    sub_field_width[7] = 16;  /* HASH_B1 (16 bit) */

    if (concat) {
        /* Get hash bits width */
        for (i = 0; i < 8; i++) {
            total_width += sub_field_width[i];
        }

        /* Concatenation hash computation order is referenced from
         * fb_Irsel1_arch::compute_rtag7_hash
         */
        sub_field_width[0] = 64;  /* Concat B1,B0,A1,A0(64bit) */
        sub_field_width[1] = 0;   /* 0 (Reserved) */
        sub_field_width[2] = 4;   /* LBN (4bit) */
        sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit) */
        sub_field_width[4] = 8;   /* MH.LBID / IRSEL local LBID (8 bit) */
        sub_field_width[5] = 8;   /* SW1 LBID (8 bit) */
        sub_field_width[6] = 0;   /* 0 (Reserved) */
        sub_field_width[7] = 0;   /* 0 (Reserved) */

        *arg += total_width;
    }

    for (i = 0; i < idx; i++) {
         *arg += sub_field_width[i];
    }

    return BCM_E_NONE;
}

STATIC int
bcm_gh2_switch_rtag7_macroflow_hash_info_get(
    int unit,
    bcm_switch_control_t type,
    hash_offset_info_t *hash_info)
{
    /* Input parameter check */
    if (NULL == hash_info) {
        return BCM_E_PARAM;
    }

    hash_info->regmem = RTAG7_FLOW_BASED_HASHm;

    switch (type) {
        case bcmSwitchMacroFlowEcmpHashConcatEnable:
            hash_info->sub_f = SUB_SEL_ECMPf;
            hash_info->offset_f = OFFSET_ECMPf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_ECMPf;
            hash_info->hash_concat = 1;
            break;
        case bcmSwitchMacroFlowHashMinOffset:
        case bcmSwitchMacroFlowHashMaxOffset:
        case bcmSwitchMacroFlowHashStrideOffset:
            hash_info->sub_f = SUB_SEL_ECMPf;
            hash_info->offset_f = OFFSET_ECMPf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_ECMPf;
            hash_info->hash_concat = 0;
            break;
        case bcmSwitchMacroFlowLoadBalanceHashConcatEnable:
            hash_info->sub_f = SUB_SEL_LBIDf;
            hash_info->offset_f = OFFSET_LBIDf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_LBIDf;
            hash_info->hash_concat = 1;
            break;
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset:
            hash_info->sub_f = SUB_SEL_LBIDf;
            hash_info->offset_f = OFFSET_LBIDf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_LBIDf;
            hash_info->hash_concat = 0;
            break;
        case bcmSwitchMacroFlowTrunkHashConcatEnable:
            hash_info->sub_f = SUB_SEL_TRUNKf;
            hash_info->offset_f = OFFSET_TRUNKf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_TRUNKf;
            hash_info->hash_concat = 1;
            break;
        case bcmSwitchMacroFlowTrunkHashMinOffset:
        case bcmSwitchMacroFlowTrunkHashMaxOffset:
        case bcmSwitchMacroFlowTrunkHashStrideOffset:
            hash_info->sub_f = SUB_SEL_TRUNKf;
            hash_info->offset_f = OFFSET_TRUNKf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_TRUNKf;
            hash_info->hash_concat = 0;
            break;
        case bcmSwitchMacroFlowHigigTrunkHashConcatEnable:
            hash_info->sub_f = SUB_SEL_HG_TRUNKf;
            hash_info->offset_f = OFFSET_HG_TRUNKf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_HG_TRUNKf;
            hash_info->hash_concat = 1;
            break;
        case bcmSwitchMacroFlowHigigTrunkHashMinOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMaxOffset:
        case bcmSwitchMacroFlowHigigTrunkHashStrideOffset:
            hash_info->sub_f = SUB_SEL_HG_TRUNKf;
            hash_info->offset_f = OFFSET_HG_TRUNKf;
            hash_info->concat_f = CONCATENATE_HASH_FIELDS_HG_TRUNKf;
            hash_info->hash_concat = 0;
            break;
        case bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable:
        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

STATIC int
bcm_gh2_switch_rtag7_macroflow_param_get(
    int unit,
    bcm_switch_control_t type,
    int **sub_field_base,
    int *min_offset,
    int *max_offset,
    int *stride_offset,
    int *concat_enable)
{
    /* sub field width: 16, 16, 4, 16, 8, 8, 16, 16 */
    static int base[] = {0, 16, 32, 36, 52, 60, 68, 84, 100};
    rtag7_flow_based_hash_entry_t entry;
    int min, max, stride;
    int index, offset;
    uint32 fval, sub_sel;
    hash_offset_info_t hash_info;
    int concat_value = 0;

    sal_memset(&hash_info, 0, sizeof(hash_offset_info_t));
    BCM_IF_ERROR_RETURN(
        bcm_gh2_switch_rtag7_macroflow_hash_info_get(unit, type, &hash_info));

    if (!SOC_MEM_IS_VALID(unit, hash_info.regmem)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, hash_info.regmem, MEM_BLOCK_ANY, 0, &entry));
    sub_sel = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                                  hash_info.sub_f);
    fval = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                               hash_info.offset_f);
    if (hash_info.concat_f != -1) {
        concat_value = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                                           hash_info.concat_f);
        if (hash_info.hash_concat || concat_value) {
            /* sub field width: 64, 0, 4, 64, 8, 8, 0, 0 */
            base[0] = 0;
            base[1] = 64;  /* base[1] = base[0] + width of base[0] */
            base[2] = 64;  /* base[2] = base[1] + width of base[1] */
            base[3] = 68;  /* base[3] = base[2] + width of base[2] */
            base[4] = 132; /* base[4] = base[3] + width of base[3] */
            base[5] = 140; /* base[5] = base[4] + width of base[4] */
            base[6] = 148; /* base[6] = base[5] + width of base[5] */
            base[7] = 148; /* base[7] = base[6] + width of base[6] */
            base[8] = 148; /* base[8] = base[7] + width of base[7] */
        } else {
            /* sub field width: 16, 16, 4, 16, 8, 8, 16, 16 */
            base[0] = 0;
            base[1] = 16;  /* base[1] = base[0] + width of base[0] */
            base[2] = 32;  /* base[2] = base[1] + width of base[1] */
            base[3] = 36;  /* base[3] = base[2] + width of base[2] */
            base[4] = 52;  /* base[4] = base[3] + width of base[3] */
            base[5] = 60;  /* base[5] = base[4] + width of base[4] */
            base[6] = 68;  /* base[6] = base[5] + width of base[5] */
            base[7] = 84;  /* base[7] = base[6] + width of base[6] */
            base[8] = 100; /* base[8] = base[7] + width of base[7] */
        }
    }
    min = base[sub_sel] + fval;

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, hash_info.regmem, MEM_BLOCK_ANY, 1, &entry));
    sub_sel = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                                  hash_info.sub_f);
    fval = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                               hash_info.offset_f);
    max = base[sub_sel] + fval;
    stride = max - min;

    if (stride != 0) {
        for (index = 2;
             index <= soc_mem_index_max(unit, hash_info.regmem);
             index++) {
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, hash_info.regmem, MEM_BLOCK_ANY,
                             index, &entry));
            sub_sel = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                                          hash_info.sub_f);
            fval = soc_mem_field32_get(unit, hash_info.regmem, &entry,
                                       hash_info.offset_f);
            offset = base[sub_sel] + fval;
            if (offset != max + stride) {
                break;
            }
            max = offset;
        }
    }

    if (sub_field_base != NULL) {
        *sub_field_base = base;
    }
    if (min_offset != NULL) {
        *min_offset = min;
    }
    if (max_offset != NULL) {
        *max_offset = max;
    }
    if (stride_offset != NULL) {
        *stride_offset = stride;
    }
    if (concat_enable != NULL) {
        *concat_enable = concat_value;
    }

    return BCM_E_NONE;
}

STATIC int
bcm_gh2_switch_rtag7_macroflow_offset_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    int *sub_field_base;
    rtag7_flow_based_hash_entry_t entry;
    int min_offset, max_offset, stride_offset, concat_enable;
    int index, offset;
    uint32 sub_sel;
    hash_offset_info_t hash_info;

    sal_memset(&hash_info, 0, sizeof(hash_offset_info_t));
    /*
     * Hash info such as sub_sel field, offset field and concatenate field
     * are retrieved using bcm_gh2_switch_rtag7_macroflow_hash_info_get
     * based on given type.
     */
    BCM_IF_ERROR_RETURN(
        bcm_gh2_switch_rtag7_macroflow_hash_info_get(unit, type, &hash_info));

    /*
     * bcm_gh2_switch_rtag7_macroflow_param_get fetches sub field base,
     * min offset, max offset, stride offset and concatenation values currently
     * configured in the hardware for the given type.
     */
    BCM_IF_ERROR_RETURN(
        bcm_gh2_switch_rtag7_macroflow_param_get(unit, type, &sub_field_base,
                                                 &min_offset, &max_offset, &stride_offset,
                                                 &concat_enable));

    if (arg < -1 || arg >= sub_field_base[8]) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmSwitchMacroFlowHashMinOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMinOffset:
        case bcmSwitchMacroFlowTrunkHashMinOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset:
            if (arg == min_offset) {
                return BCM_E_NONE;
            } else if ((max_offset > 0) && (arg > max_offset)) {
                return BCM_E_PARAM;
            }
            min_offset = arg == -1 ? 0 : arg;
            break;
        case bcmSwitchMacroFlowHashMaxOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMaxOffset:
        case bcmSwitchMacroFlowTrunkHashMaxOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset:
            if (arg == max_offset) {
                return BCM_E_NONE;
            } else if ((arg < min_offset) && (arg != -1)) {
                return BCM_E_PARAM;
            }
            max_offset = arg == -1 ? sub_field_base[8] - 1 : arg;
            break;
        case bcmSwitchMacroFlowHashStrideOffset:
        case bcmSwitchMacroFlowHigigTrunkHashStrideOffset:
        case bcmSwitchMacroFlowTrunkHashStrideOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset:
            if (arg == stride_offset) {
                return BCM_E_NONE;
            } else if (arg < 0) {
                return BCM_E_PARAM;
            }
            stride_offset = arg;
            break;
        case bcmSwitchMacroFlowEcmpHashConcatEnable:
        case bcmSwitchMacroFlowLoadBalanceHashConcatEnable:
        case bcmSwitchMacroFlowTrunkHashConcatEnable:
        case bcmSwitchMacroFlowHigigTrunkHashConcatEnable:
            if (arg == concat_enable) {
                return BCM_E_NONE;
            } else if (!((arg == 0) || (arg == 1))) {
                return BCM_E_PARAM;
            }
            concat_enable = arg;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    if (stride_offset == 0 && min_offset != max_offset) {
        stride_offset = 1;
    }

    if (((type == bcmSwitchMacroFlowHashStrideOffset) ||
        (type == bcmSwitchMacroFlowHigigTrunkHashStrideOffset) ||
        (type == bcmSwitchMacroFlowTrunkHashStrideOffset) ||
        (type == bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset)) &&
        (stride_offset == 1) && (min_offset == max_offset)) {
        max_offset += stride_offset;
    }

    offset = min_offset;
    for (index = 0; index <= soc_mem_index_max(unit, hash_info.regmem);
         index++) {
        for (sub_sel = 0; sub_sel < 7; sub_sel++) {
            if (offset < sub_field_base[sub_sel + 1]) {
                break;
            }
        }

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, hash_info.regmem, MEM_BLOCK_ANY,
                          index, &entry));

        soc_mem_field32_set(unit, hash_info.regmem, &entry,
                            hash_info.sub_f, sub_sel);
        soc_mem_field32_set(unit, hash_info.regmem, &entry,
                            hash_info.offset_f,
                            (offset - sub_field_base[sub_sel]));
        if (hash_info.concat_f != -1) {
            soc_mem_field32_set(unit, hash_info.regmem, &entry,
                                hash_info.concat_f, concat_enable);
        }

        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, hash_info.regmem, MEM_BLOCK_ANY,
                          index, &entry));
        offset += stride_offset;
        if (offset > max_offset) {
            offset = min_offset;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_random_hash_seed_get(int unit, hash_offset_info_t *hash_info,
                              int seed, int *offset, int *subsel)
{
    int concat, random;
    uint32 hash_entry[SOC_MAX_MEM_WORDS];

    /* Input parameter check */
    if ((NULL == hash_info) || (NULL == offset) || (NULL == subsel)) {
        return BCM_E_PARAM;
    }

    /* Check for hash Concatenation */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, hash_info->regmem, MEM_BLOCK_ANY, 0, &hash_entry));
    concat =  soc_mem_field32_get(unit, hash_info->regmem, &hash_entry,
                                  hash_info->concat_f);

    /* Generate random number for the seed as described in C standards */
    random = seed * 1103515245 + 12345;
    /* random_seed / 65536) % 32768 */
    random = (random >> 16) & 0x7FFF;

    if (concat) {
        *offset = random & 0x3f;
        /* Use HASH A0_A1_B0_B1 */
        *subsel = 0;
    } else {
        *offset = random & 0xf;
        /* User any of hash A0, A1, B0, B1 */
        *subsel = (random & 0x30) >> 4;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh2_switch_rtag7_symmetric_hash_control_set
 * Description:
 *      Set the RTAG7 Macro Flow Hash Seed/Concatenation control settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - Enable/Disable values
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
bcm_gh2_switch_rtag7_macroflow_hash_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    soc_mem_t hash_sel_regmem;
    uint32 hash_entry[SOC_MAX_MEM_WORDS];
    hash_offset_info_t hash_info;
    soc_field_t hash_flow_select[5];
    uint32 rval;
    int index = 0, min, max;
    int offset = 0, subsel = 0;

    sal_memset(&hash_info, 0, sizeof(hash_offset_info_t));
    sal_memset(&hash_flow_select, 0, sizeof(hash_flow_select));

    hash_sel_regmem = RTAG7_HASH_SELr;
    hash_info.regmem = RTAG7_FLOW_BASED_HASHm;

    switch (type) {
        /* Set RTAG7 Macro Flow Hash Seed for ECMP */
        case bcmSwitchMacroFlowECMPHashSeed:
            hash_info.sub_f = SUB_SEL_ECMPf;
            hash_info.offset_f = OFFSET_ECMPf;
            hash_info.concat_f = CONCATENATE_HASH_FIELDS_ECMPf;
            hash_flow_select[index++] = USE_FLOW_SEL_VXLAN_ECMPf;
            hash_flow_select[index++] = USE_FLOW_SEL_ROCEV2_ECMPf;
            break;
        /* Set RTAG7 Macro Flow Hash Seed for LBID/ENTROPY */
        case bcmSwitchMacroFlowLoadBalanceHashSeed:
            hash_info.sub_f = SUB_SEL_LBIDf;
            hash_info.offset_f = OFFSET_LBIDf;
            hash_info.concat_f = CONCATENATE_HASH_FIELDS_LBIDf;
            hash_flow_select[index++] = USE_FLOW_SEL_LBID_UCf;
            hash_flow_select[index++] = USE_FLOW_SEL_LBID_NONUCf;
            hash_flow_select[index++] = USE_FLOW_SEL_ENTROPYf;
            break;
        /* Set RTAG7 Macro Flow Hash Seed for TRUNK */
        case bcmSwitchMacroFlowTrunkHashSeed:
            hash_info.sub_f = SUB_SEL_TRUNKf;
            hash_info.offset_f = OFFSET_TRUNKf;
            hash_info.concat_f = CONCATENATE_HASH_FIELDS_TRUNKf;
            hash_flow_select[index++] = USE_FLOW_SEL_TRUNK_UCf;
            hash_flow_select[index++] = USE_FLOW_SEL_TRUNK_NONUCf;
            break;
        /* Set RTAG7 Macro Flow Hash Seed for HG TRUNK */
        case bcmSwitchMacroFlowHigigTrunkHashSeed:
            hash_info.sub_f = SUB_SEL_HG_TRUNKf;
            hash_info.offset_f = OFFSET_HG_TRUNKf;
            hash_info.concat_f = CONCATENATE_HASH_FIELDS_HG_TRUNKf;
            hash_flow_select[index++] = USE_FLOW_SEL_HG_TRUNK_UCf;
            hash_flow_select[index++] = USE_FLOW_SEL_HG_TRUNK_NONUCf;
            hash_flow_select[index++] = USE_FLOW_SEL_HG_TRUNK_FAILOVERf;
            break;
        case bcmSwitchMacroFlowECMPUnderlayHashSeed:
            return BCM_E_UNAVAIL;
        default:
            return BCM_E_PARAM;
    }

    if (!soc_mem_is_valid(unit, hash_info.regmem)) {
        return BCM_E_UNAVAIL;
    }
    min = soc_mem_index_min(unit, hash_info.regmem);
    max = soc_mem_index_max(unit, hash_info.regmem);

    /* Set random generated hash offset and sub select */
    BCM_IF_ERROR_RETURN(
        _bcm_gh2_random_hash_seed_get(unit, &hash_info, arg,
                                      &offset, &subsel));

    for (index = min; index <= max; index++) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, hash_info.regmem,
                         MEM_BLOCK_ANY, index, &hash_entry));
        soc_mem_field32_set(unit, hash_info.regmem, &hash_entry,
                            hash_info.sub_f, subsel);
        soc_mem_field32_set(unit, hash_info.regmem, &hash_entry,
                            hash_info.offset_f, offset);
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, hash_info.regmem,
                          MEM_BLOCK_ALL, index, &hash_entry));
    }

    /* Enable Macro flow bits */
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, hash_sel_regmem, REG_PORT_ANY, 0, &rval));

    for (index = 0;
         index < (sizeof(hash_flow_select) / sizeof(soc_field_t));
         index++) {
        if (soc_reg_field_valid(unit, hash_sel_regmem,
                                hash_flow_select[index])) {
            soc_reg_field_set(unit, hash_sel_regmem, &rval,
                              hash_flow_select[index], 1);
        }
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, hash_sel_regmem, REG_PORT_ANY, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *          _bcm_esw_switch_control_gport_resolve
 * Description:
 *          Decodes local physical port from a gport
 * Parameters:
 *          unit - StrataSwitch PCI device unit number (driver internal).
 *      gport - a gport to decode
 *      port - (Out) Local physical port encoded in gport
 * Returns:
 *      BCM_E_xxxx
 * Note
 *      In case of gport contains other then local port error will be returned.
 */


STATIC int
_bcm_gh2_switch_control_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *port)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    bcm_port_t      local_port;
    int             id, isMymodid;

    if (NULL == port) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport, &modid, &local_port, &tgid, &id));

    if ((BCM_TRUNK_INVALID != tgid) || (-1 != id)) {
        return BCM_E_PARAM;
    }
    /* Check if modid is local */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, modid, &isMymodid));

    if (isMymodid != TRUE) {
        return BCM_E_PORT;
    }
    *port = local_port;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_gh2_switch_control_port_proxy_validate
 * Description:
 *      Checks that switch control is valid for a PROXY GPORT.
 *      Resolves port to expected port value for the given port control type.
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      type      - (IN) Switch control type
 *      port_out  - (OUT) Returns port (gport or BCM format) that
 *                  is expected for corresponding switch control type.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_gh2_switch_control_port_proxy_validate(int unit, bcm_port_t port,
                                           bcm_switch_control_t type,
                                           bcm_port_t *port_out)
{
    int rv = BCM_E_PORT;
    int support = FALSE;

    if (!soc_feature(unit, soc_feature_proxy_port_property)) {
        return BCM_E_PORT;
    }

    switch(type) {
    case bcmSwitchModuleLoopback:
    case bcmSwitchTrunkHashSet0UnicastOffset:
    case bcmSwitchTrunkHashSet0NonUnicastOffset:
    case bcmSwitchTrunkFailoverHashOffset:
    case bcmSwitchFabricTrunkHashSet0UnicastOffset:
    case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
    case bcmSwitchFabricTrunkFailoverHashOffset:
    case bcmSwitchFabricTrunkDynamicHashOffset:
    case bcmSwitchTrunkDynamicHashOffset:
    case bcmSwitchEcmpDynamicHashOffset:
    case bcmSwitchFabricTrunkResilientHashOffset:
    case bcmSwitchTrunkResilientHashOffset:
    case bcmSwitchEcmpResilientHashOffset:
    case bcmSwitchECMPVxlanHashOffset:
    case bcmSwitchECMPUnderlayVxlanHashOffset:
    case bcmSwitchECMPL2GreHashOffset:
    case bcmSwitchECMPUnderlayL2GreHashOffset:
    case bcmSwitchECMPTrillHashOffset:
    case bcmSwitchECMPUnderlayTrillHashOffset:
    case bcmSwitchECMPMplsHashOffset:
    case bcmSwitchVirtualPortDynamicHashOffset:
    case bcmSwitchVirtualPortUnderlayDynamicHashOffset:
    case bcmSwitchLoadBalanceHashSet0UnicastOffset:
    case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
    case bcmSwitchECMPHashSet0Offset:
    case bcmSwitchECMPOverlayHashOffset:
    case bcmSwitchECMPUnderlayHashSet0Offset:
    case bcmSwitchEntropyHashSet0Offset:
        support = TRUE;
        break;
    default:
        support = FALSE;
        break;
    }

    if (support) {
        *port_out = port;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcm_gh2_switch_control_port_validate
 * Description:
 *      Checks that port is valid and that control is supported
 *      for the given port type.  Resolves port to expected
 *      port value for the given switch control type.
 *      Current valid port types are:
 *          GPORT PROXY
 *          GPORT (various types) that resolves to local physical port
 *          BCM port (non-gport)
 * Parameters:
 *      unit      - (IN) Device number
 *      port      - (IN) Port to resolve
 *      type      - (IN) Switch control type
 *      port_out  - (OUT) Returns port (gport or BCM format) that
 *                  is expected for corresponding switch control type.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
bcm_gh2_switch_control_port_validate(int unit, bcm_port_t port,
                                     bcm_switch_control_t type,
                                     bcm_port_t *port_out)
{
    int rv = BCM_E_PORT;

    if (BCM_GPORT_IS_PROXY(port)) {
        rv = bcm_gh2_switch_control_port_proxy_validate(unit, port,
                                                        type, port_out);
    } else {
        /* All other types are supported for valid physical local ports */
        rv = _bcm_gh2_switch_control_gport_resolve(unit, port, port_out);
    }

    return rv;
}


STATIC int
_bcm_gh2_pkt_age_set(int unit, int arg)
{
    int aging_ticks = arg;
    uint32 pat_val = 0;

    if (!arg) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT_r(unit, 0, 0)); /* Aesthetic */
    } else if ((arg < 0) || (arg > 7162)) {
        return BCM_E_PARAM;
    } else {
        /* Use 125 us clock pulses */
        soc_reg_field_set(unit, PKTAGINGTIMERr, &pat_val, AGINGTICKSELf, 0);

        /* Convert ms request to 125 us pulses */
        aging_ticks *= 8;

        /* We want this many pulses to happen before aging,
         * but the HW setting is pulses per cycle.
         * We need to divide the by the number of cycles before aging.*/
        aging_ticks =
            (aging_ticks + NUM_MMU_AGE_CYCLES - 1) / NUM_MMU_AGE_CYCLES;
        soc_reg_field_set(unit, PKTAGINGTIMERr, &pat_val,
                          DURATIONSELECTf, aging_ticks);
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, pat_val));

        /* Set all COSQs to the same value
         * - They use the maximum of NUM_MMU_AGE_CYCLES to expire */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT_r(unit, 0, 0));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_gh2_pkt_age_get(int unit, int *arg)
{
    uint32 timerval, limitval, mask;
    int i, loopcount;

    /* Check that all COSQs have the same value */
    SOC_IF_ERROR_RETURN(READ_PKTAGINGLIMIT_r(unit, 0, &limitval));
    limitval &= 0xFFFFFF; /* AGINGLIMITCOSn fields only */
    loopcount = mask = soc_reg_field_get(unit, PKTAGINGLIMIT_r,
                                         limitval, AGINGLIMITCOS_X0f);
    /* Make mask of eight identical AGINGLIMITCOSn fields */
    for (i = 1; i < 8; i++) {
        mask = (mask << 3) | loopcount;
    }

    if (mask != limitval) {
        /* COSQs currently are not programmed identically */
        return BCM_E_CONFIG;
    }

    /* Return the (common) setting */
    loopcount = NUM_MMU_AGE_CYCLES - loopcount;
    SOC_IF_ERROR_RETURN(READ_PKTAGINGTIMERr(unit, &timerval));

    *arg =
      (loopcount *
       soc_reg_field_get(unit, PKTAGINGTIMERr, timerval, DURATIONSELECTf)) / 8;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_er_hashselect_set
 * Description:
 *      Set the Hash Select for L2, L3 or Multipath type.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_er_hashselect_set(int unit, bcm_switch_control_t type, int arg)
{
    uint32      hash_control, val=0;
    soc_field_t field;
    soc_reg_t   hash_reg = HASH_CONTROLr;
    int         dual = FALSE;

    switch (type) {
        case bcmSwitchHashL2:
            field = L2_AND_VLAN_MAC_HASH_SELECTf;
            if (!soc_reg_field_valid(unit, hash_reg, field)) {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashL3:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, L3_HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = L3_HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashMultipath:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_SELf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = ECMP_HASH_SELf;
                if ((arg == BCM_HASH_CRC16L) || (arg == BCM_HASH_CRC16U)) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashL2Dual:
            dual = TRUE;
            hash_reg = L2_AUX_HASH_CONTROLr;
            field = HASH_SELECTf;
            if (!soc_reg_field_valid(unit, hash_reg, field)) {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashL3Dual:
            if (soc_reg_field_valid(unit, L3_AUX_HASH_CONTROLr, HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                dual = TRUE;
                hash_reg = L3_AUX_HASH_CONTROLr;
                field = HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixIngress:
        case bcmSwitchHashIpfixEgress:
        case bcmSwitchHashIpfixIngressDual:
        case bcmSwitchHashIpfixEgressDual:
        case bcmSwitchHashMPLS:
        case bcmSwitchHashMPLSDual:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashVlanTranslate:
            if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Af;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashVlanTranslateDual:
            if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Bf)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashEgressVlanTranslate:
            if (soc_reg_field_valid(unit, EGR_VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = EGR_VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Af;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashEgressVlanTranslateDual:
            if (soc_reg_field_valid(unit, EGR_VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Bf)) {
                hash_reg = EGR_VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashWlanPort:
        case bcmSwitchHashWlanPortDual:
        case bcmSwitchHashWlanClient:
        case bcmSwitchHashWlanClientDual:
        case bcmSwitchHashRegexAction:
        case bcmSwitchHashRegexActionDual:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL3DNATPool:
        case bcmSwitchHashL3DNATPoolDual:
        case bcmSwitchHashVpVlanMemberIngress:
        case bcmSwitchHashVpVlanMemberIngressDual:
        case bcmSwitchHashVpVlanMemberEgress:
        case bcmSwitchHashVpVlanMemberEgressDual:
        case bcmSwitchHashL2Endpoint:
        case bcmSwitchHashL2EndpointDual:
        case bcmSwitchHashEndpointQueueMap:
        case bcmSwitchHashEndpointQueueMapDual:
        case bcmSwitchHashOamEgress:
        case bcmSwitchHashOamEgressDual:
        case bcmSwitchHashOam:
        case bcmSwitchHashOamDual:
            return BCM_E_UNAVAIL;
            break;
        default:
            return BCM_E_PARAM;
            break;
    }

    switch (arg) {
        case BCM_HASH_ZERO:
            val = FB_HASH_ZERO;
            break;
        case BCM_HASH_LSB:
            val = FB_HASH_LSB;
            break;
        case BCM_HASH_CRC16L:
            val = FB_HASH_CRC16_LOWER;
            break;
        case BCM_HASH_CRC16U:
            val = FB_HASH_CRC16_UPPER;
            break;
        case BCM_HASH_CRC32L:
            val = FB_HASH_CRC32_LOWER;
            break;
        case BCM_HASH_CRC32U:
            val = FB_HASH_CRC32_UPPER;
            break;
        default:
            return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_read_any_block(unit, hash_reg, &hash_control));
    soc_reg_field_set(unit, hash_reg, &hash_control, field, val);
    if (dual) {
        uint32 base_hash_control, base_hash;
        soc_field_t base_field;

        base_field = (type == bcmSwitchHashL2Dual) ?
            L2_AND_VLAN_MAC_HASH_SELECTf : L3_HASH_SELECTf;

        BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &base_hash_control));
        base_hash = soc_reg_field_get(unit, HASH_CONTROLr,
                                      base_hash_control, base_field);
        soc_reg_field_set(unit, hash_reg, &hash_control,
                          ENABLEf, (base_hash == val) ? 0 : 1);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_write_all_blocks(unit, hash_reg, hash_control));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_er_hashselect_get
 * Description:
 *      Get the current Hash Select settings. Value returned
 *      is of the form BCM_HASH_*.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_er_hashselect_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32      hash_control, val=0;
    soc_reg_t   hash_reg = HASH_CONTROLr;
    soc_field_t field;
    int         dual = FALSE;

    switch (type) {
        case bcmSwitchHashL2:
            field = L2_AND_VLAN_MAC_HASH_SELECTf;
            if (!soc_reg_field_valid(unit, hash_reg, field)) {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashL3:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, L3_HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = L3_HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashMultipath:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_SELf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = ECMP_HASH_SELf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashL2Dual:
            dual = TRUE;
            hash_reg = L2_AUX_HASH_CONTROLr;
            field = HASH_SELECTf;
            if (!soc_reg_field_valid(unit, hash_reg, field)) {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashL3Dual:
            if (soc_reg_field_valid(unit, L3_AUX_HASH_CONTROLr, HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                dual = TRUE;
                hash_reg = L3_AUX_HASH_CONTROLr;
                field = HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixIngress:
        case bcmSwitchHashIpfixEgress:
        case bcmSwitchHashIpfixIngressDual:
        case bcmSwitchHashIpfixEgressDual:
        case bcmSwitchHashMPLS:
        case bcmSwitchHashMPLSDual:
            return BCM_E_UNAVAIL;
        case bcmSwitchHashVlanTranslate:
            if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Af;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashVlanTranslateDual:
            if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Bf)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashEgressVlanTranslate:
            if (soc_reg_field_valid(unit, EGR_VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = EGR_VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Af;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashEgressVlanTranslateDual:
            if (soc_reg_field_valid(unit, EGR_VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Bf)) {
                hash_reg = EGR_VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashWlanPort:
        case bcmSwitchHashWlanPortDual:
        case bcmSwitchHashWlanClient:
        case bcmSwitchHashWlanClientDual:
        case bcmSwitchHashRegexAction:
        case bcmSwitchHashRegexActionDual:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL3DNATPool:
        case bcmSwitchHashL3DNATPoolDual:
        case bcmSwitchHashVpVlanMemberIngress:
        case bcmSwitchHashVpVlanMemberIngressDual:
        case bcmSwitchHashVpVlanMemberEgress:
        case bcmSwitchHashVpVlanMemberEgressDual:
        case bcmSwitchHashL2Endpoint:
        case bcmSwitchHashL2EndpointDual:
        case bcmSwitchHashEndpointQueueMap:
        case bcmSwitchHashEndpointQueueMapDual:
        case bcmSwitchHashOamEgress:
        case bcmSwitchHashOamEgressDual:
        case bcmSwitchHashOam:
        case bcmSwitchHashOamDual:
            return BCM_E_UNAVAIL;
            break;
        default:
            return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_read_any_block(unit, hash_reg, &hash_control));

    if (dual &&
        !soc_reg_field_get(unit, hash_reg, hash_control, ENABLEf)) {
        /* Dual hash not enabled, just return primary hash */
        field = (type == bcmSwitchHashL2Dual) ?
            L2_AND_VLAN_MAC_HASH_SELECTf : L3_HASH_SELECTf;
        hash_reg = HASH_CONTROLr;
        BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    }
    val = soc_reg_field_get(unit, hash_reg, hash_control, field);

    switch (val) {
        case FB_HASH_ZERO:
            *arg = BCM_HASH_ZERO;
            break;
        case FB_HASH_LSB:
            *arg = BCM_HASH_LSB;
            break;
        case FB_HASH_CRC16_LOWER:
            *arg = BCM_HASH_CRC16L;
            break;
        case FB_HASH_CRC16_UPPER:
            *arg = BCM_HASH_CRC16U;
            break;
        case FB_HASH_CRC32_LOWER:
            *arg = BCM_HASH_CRC32L;
            break;
        case FB_HASH_CRC32_UPPER:
            *arg = BCM_HASH_CRC32U;
            break;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_hashcontrol_set
 * Description:
 *      Set the Hash Control for L2, L3 or Multipath type.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_hashcontrol_set(int unit, int arg)
{
    uint32      hash_control, val=0;

    if (!soc_feature(unit, soc_feature_l3)) {
        if ((arg & BCM_HASH_CONTROL_MULTIPATH_L4PORTS) ||
            (arg & BCM_HASH_CONTROL_MULTIPATH_DIP)) {
            return BCM_E_UNAVAIL;
        }
    }

    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));

    val = (arg & BCM_HASH_CONTROL_MULTIPATH_L4PORTS) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          USE_TCP_UDP_PORTSf, val);

    if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_USE_DIPf)) {
        val = (arg & BCM_HASH_CONTROL_MULTIPATH_DIP) ? 1 : 0;
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_USE_DIPf, val);

        val = BCM_HASH_CONTROL_MULTIPATH_USERDEF_VAL(arg);
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_UDFf, val);
    }

    val = (arg & BCM_HASH_CONTROL_TRUNK_UC_XGS2) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      ENABLE_DRACO1_5_HASHf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_UC_SRCPORT) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      UC_TRUNK_HASH_USE_SRC_PORTf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_DST) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_DST_ENABLEf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_SRC) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_SRC_ENABLEf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_MODPORT) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_MOD_PORT_ENABLEf, val);

    val = (arg & BCM_HASH_CONTROL_ECMP_ENHANCE) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      ECMP_HASH_USE_RTAG7f, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_USE_RTAG7f, val);


    BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_hashcontrol_get
 * Description:
 *      Get the current Hash Control settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_hashcontrol_get(int unit, int *arg)
{
    uint32      hash_control, val=0;

    *arg = 0;
    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));


    if (soc_feature(unit, soc_feature_l3)) {
        if (soc_reg_field_valid(unit, HASH_CONTROLr, USE_TCP_UDP_PORTSf)) {
            val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    USE_TCP_UDP_PORTSf);
            if (val) *arg |= BCM_HASH_CONTROL_MULTIPATH_L4PORTS;
        }
        if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_USE_DIPf)) {
            val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    ECMP_HASH_USE_DIPf);
            if (val) *arg |= BCM_HASH_CONTROL_MULTIPATH_DIP;
        }

        if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_UDFf)) {
            val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    ECMP_HASH_UDFf);
            *arg |= BCM_HASH_CONTROL_MULTIPATH_USERDEF(val);
        }
    }

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            ENABLE_DRACO1_5_HASHf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_UC_XGS2;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            UC_TRUNK_HASH_USE_SRC_PORTf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_UC_SRCPORT;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_DST_ENABLEf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_DST;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_SRC_ENABLEf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_SRC;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_MOD_PORT_ENABLEf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_MODPORT;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            ECMP_HASH_USE_RTAG7f);
    if (val) *arg |= BCM_HASH_CONTROL_ECMP_ENHANCE;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_USE_RTAG7f);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;

    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_er_color_mode_set(int unit, bcm_port_t port, int arg)
{
        uint32 val, oval, cfi_as_cng;
        uint64 rval64;

        BCM_IF_ERROR_RETURN
            (READ_EGR_VLAN_CONTROL_1r(unit, port, &val));

        oval = val;

        switch (arg) {
        case BCM_COLOR_PRIORITY:
            cfi_as_cng = 0;
            break;
        case BCM_COLOR_OUTER_CFI:
            cfi_as_cng = 0xf;
            break;
        case BCM_COLOR_INNER_CFI:
            if (soc_feature(unit, soc_feature_color_inner_cfi)) {
                cfi_as_cng = 0x0;
                BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
                soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                                      CFI_AS_CNGf, 0);
                soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                                      CVLAN_CFI_AS_CNGf, 1);
                SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));
            } else {
                    return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
        }

        soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &val,
                          CFI_AS_CNGf, cfi_as_cng);
        if (oval != val) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_port_config_set(unit, port,
                                          _bcmPortCfiAsCng,
                                          cfi_as_cng));
            BCM_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_CONTROL_1r(unit, port, val));
        }

    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_er_color_mode_get(int unit, bcm_port_t port, int *arg)
{
    uint32    val, cfi_as_cng;
    uint64    rval64;
    int    cvlan_cfi_as_cng = 0;
    if (soc_feature(unit, soc_feature_color_inner_cfi)) {
        BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
        cvlan_cfi_as_cng = soc_reg64_field32_get(unit, ING_CONFIG_64r, rval64,
                                                 CVLAN_CFI_AS_CNGf);
    }

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_1r(unit, port, &val));

    cfi_as_cng = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r, val,
                                   CFI_AS_CNGf);
    if (cfi_as_cng == 0) {
        if (cvlan_cfi_as_cng && soc_feature(unit, soc_feature_color_inner_cfi)) {
            *arg = BCM_COLOR_INNER_CFI;
        } else {
            *arg = BCM_COLOR_PRIORITY;
        }
    } else {
        *arg = BCM_COLOR_OUTER_CFI;
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_gh2_mod_lb_set(int unit, bcm_port_t port, int arg)
{
    if (soc_feature(unit, soc_feature_module_loopback)) {
        return _bcm_esw_port_config_set(unit, port, _bcmPortModuleLoopback,
                                        (arg ? 1 : 0));
    }
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_gh2_mod_lb_get(int unit, bcm_port_t port, int *arg)
{
    if (soc_feature(unit, soc_feature_module_loopback)) {
        return _bcm_esw_port_config_get(unit, port, _bcmPortModuleLoopback,
                                        arg);
    }
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_gh2_ing_rate_limit_ifg_set(int unit, bcm_port_t port, int arg)
{
    if (arg > BCM_FB2_BMC_IFG_MAX) {
        arg = BCM_FB2_BMC_IFG_MAX;
    } else if (arg < 0) {
        return BCM_E_PARAM;
    }
    return soc_reg_field32_modify(unit, BKPMETERINGCONFIG_EXTr, port,
                                  IFG_ACCT_SELf, arg);
}


STATIC int
_bcm_gh2_ing_rate_limit_ifg_get(int unit, bcm_port_t port, int *arg)
{
    uint32 bmc_reg;
    BCM_IF_ERROR_RETURN
        (READ_BKPMETERINGCONFIG_EXTr(unit, port, &bmc_reg));
    *arg = soc_reg_field_get(unit, BKPMETERINGCONFIG_EXTr, bmc_reg,
                             IFG_ACCT_SELf);
    return BCM_E_NONE;
}


STATIC int
_bcm_gh2_urpf_route_enable(int unit,  int arg)
{
#ifdef INCLUDE_L3
    int orig_val, rv = BCM_E_NONE;
    uint32 reg_val;

    if (!soc_feature(unit, soc_feature_urpf)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(unit, &reg_val));

    /* Handle nothing changed case. */
    orig_val = soc_reg_field_get(unit, L3_DEFIP_RPF_CONTROLr,
                                 reg_val, DEFIP_RPF_ENABLEf);
    if (orig_val == (arg ? 1 : 0)) {
        return (BCM_E_NONE);
    }

    /* State changed -> Delete all the routes. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_defip_del_all(unit));

    /* Destroy Hash/Avl quick lookup structure. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_fbx_defip_deinit(unit));

    /* Set urpf route enable bit. */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L3_DEFIP_RPF_CONTROLr, REG_PORT_ANY,
                                DEFIP_RPF_ENABLEf, (arg ? 1 : 0)));


    /* Reinit Hash/Avl quick lookup structure. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_fbx_defip_init(unit));

    /* Lock the DEFIP tables so the SOC TCAM scanning logic doesn't
     * trip up on the URPF reconfiguration.
     */
    soc_mem_lock(unit, L3_DEFIPm);
    if (SOC_MEM_IS_ENABLED(unit, L3_DEFIP_PAIR_128m)) {
        soc_mem_lock(unit, L3_DEFIP_PAIR_128m);
    }

    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l3_defip_urpf = arg ? 1 : 0;
    SOC_CONTROL_UNLOCK(unit);

    /* Clear h/w memory before use */
    if (BCM_SUCCESS(rv)) {
        rv = soc_mem_clear(unit, L3_DEFIPm, MEM_BLOCK_ALL, 0);
    }
    if (SOC_MEM_IS_ENABLED(unit, L3_DEFIP_PAIR_128m)) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_clear(unit, L3_DEFIP_PAIR_128m,
                               MEM_BLOCK_ALL, 0);
        }
        soc_mem_unlock(unit, L3_DEFIP_PAIR_128m);
    }

    soc_mem_unlock(unit, L3_DEFIPm);
    /* Must release the locks before return. */

    return rv;
#endif /* INCLUDE_L3 */
    return (BCM_E_UNAVAIL);
}

STATIC int
_bcm_gh2_urpf_port_mode_set(int unit, bcm_port_t port, int arg)
{
    if (!soc_feature(unit, soc_feature_urpf)) {
        return (BCM_E_UNAVAIL);
    }

    switch (arg){
      case BCM_SWITCH_URPF_DISABLE:
      case BCM_SWITCH_URPF_LOOSE:
      case BCM_SWITCH_URPF_STRICT:
          return _bcm_esw_port_config_set(unit, port, _bcmPortL3UrpfMode, arg);
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_PARAM);
}

STATIC int
_bcm_gh2_urpf_def_gw_enable(int unit, bcm_port_t port, int arg)
{
    int enable = (arg) ? TRUE : FALSE;

    if (!soc_feature(unit, soc_feature_urpf)) {
        return (BCM_E_UNAVAIL);
    }

    return _bcm_esw_port_config_set(unit, port,
                                    _bcmPortL3UrpfDefaultRoute, enable);
}

STATIC int
_bcm_gh2_prot_pkt_profile_set(int unit, soc_reg_t reg, bcm_port_t port,
                              int count, soc_field_t *fields, uint32 *values)
{
    soc_mem_t mem;
    port_tab_entry_t entry;
    int port_index, i;
    uint32 prot_pkt_ctrl, igmp_mld_pkt_ctrl, *rval_ptr;
    uint32 old_index, index;

    if (reg == PROTOCOL_PKT_CONTROLr) {
        rval_ptr = &prot_pkt_ctrl;
    } else if (reg == IGMP_MLD_PKT_CONTROLr) {
        rval_ptr = &igmp_mld_pkt_ctrl;
    } else {
        return BCM_E_INTERNAL;
    }

    mem = PORT_TABm;
    port_index = port;
    if (IS_CPU_PORT(unit, port)) {
        if (SOC_MEM_IS_VALID(unit, IPORT_TABLEm)) {
            mem = IPORT_TABLEm;
        } else {
            port_index = SOC_INFO(unit).cpu_hg_index;
        }
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, port_index, &entry));
    old_index = soc_mem_field32_get(unit, mem, &entry, PROTOCOL_PKT_INDEXf);

    BCM_IF_ERROR_RETURN
        (_bcm_prot_pkt_ctrl_get(unit, old_index, &prot_pkt_ctrl,
                                &igmp_mld_pkt_ctrl));
    for (i = 0; i < count; i++) {
        soc_reg_field_set(unit, reg, rval_ptr, fields[i], values[i]);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_prot_pkt_ctrl_add(unit, prot_pkt_ctrl, igmp_mld_pkt_ctrl,
                                &index));
    BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_delete(unit, old_index));
    return soc_mem_field32_modify(unit, PORT_TABm, port_index,
                                  PROTOCOL_PKT_INDEXf, index);
}

STATIC int
_bcm_gh2_protocol_pkt_index_get(int unit, bcm_port_t port, int *index)
{
    port_tab_entry_t entry;
    soc_mem_t mem;
    int port_index;

    if (!soc_mem_field_valid(unit, PORT_TABm, PROTOCOL_PKT_INDEXf)) {
        return BCM_E_INTERNAL;
    }

    mem = PORT_TABm;
    port_index = port;
    if (IS_CPU_PORT(unit, port)) {
        if (SOC_MEM_IS_VALID(unit, IPORT_TABLEm)) {
            mem = IPORT_TABLEm;
        } else {
            port_index = SOC_INFO(unit).cpu_hg_index;
        }
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, port_index, &entry));
    *index = soc_mem_field32_get(unit, mem, &entry, PROTOCOL_PKT_INDEXf);
    return BCM_E_NONE;
}


STATIC int
_bcm_gh2_prot_pkt_action_set(int unit,
                             bcm_port_t port,
                             bcm_switch_control_t type,
                             int arg)
{
    soc_field_t field;
    uint32 value;

    value = arg ? 1 : 0;

    switch(type) {
    case bcmSwitchArpReplyToCpu:
        field = ARP_REPLY_TO_CPUf;
        break;
    case bcmSwitchArpReplyDrop:
        field = ARP_REPLY_DROPf;
        break;
    case bcmSwitchArpRequestToCpu:
        field = ARP_REQUEST_TO_CPUf;
        break;
    case bcmSwitchArpRequestDrop:
        field = ARP_REQUEST_DROPf;
        break;
    case bcmSwitchNdPktToCpu:
        field = ND_PKT_TO_CPUf;
        break;
    case bcmSwitchNdPktDrop:
        field = ND_PKT_DROPf;
        break;
    case bcmSwitchDhcpPktToCpu:
        field = DHCP_PKT_TO_CPUf;
        break;
    case bcmSwitchDhcpPktDrop:
        field = DHCP_PKT_DROPf;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    return _bcm_gh2_prot_pkt_profile_set(unit, PROTOCOL_PKT_CONTROLr, port, 1,
                                         &field, &value);
}



/*
 * Function:
 *      _bcm_gh2_igmp_action_set
 * Description:
 *      Set the IGMP/MLD registers
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - IGMP / MLD actions
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_igmp_action_set(int unit,
                         bcm_port_t port,
                         bcm_switch_control_t type,
                         int arg)
{
    uint32      values[3];
    soc_field_t fields[3];
    int         idx;
    soc_reg_t   reg = INVALIDr ;
    int         fcount = 1;
    int         value = (arg) ? 1 : 0;

    for (idx = 0; idx < COUNTOF(values); idx++) {
        values[idx] = value;
        fields[idx] = INVALIDf;
    }

    if (soc_feature(unit, soc_feature_igmp_mld_support)) {
        reg = IGMP_MLD_PKT_CONTROLr;
        /* Given control type select register field. */
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                fields[0] = IGMP_REP_LEAVE_TO_CPUf;
                fields[1] = IGMP_QUERY_TO_CPUf;
                fields[2] = IGMP_UNKNOWN_MSG_TO_CPUf;
                fcount = 3;
                break;
            case bcmSwitchIgmpPktDrop:
                fields[0] = IGMP_REP_LEAVE_FWD_ACTIONf;
                fields[1] = IGMP_QUERY_FWD_ACTIONf;
                fields[2] = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                fcount = 3;
                values[0] = values[1] = values[2] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldPktToCpu:
                fields[0] = MLD_REP_DONE_TO_CPUf;
                fields[1] = MLD_QUERY_TO_CPUf;
                fcount = 2;
                break;
            case bcmSwitchMldPktDrop:
                fields[0] = MLD_REP_DONE_FWD_ACTIONf;
                fields[1] = MLD_QUERY_FWD_ACTIONf;
                fcount = 2;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktToCpu:
                fields[0] = IPV4_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV4ResvdMcPktFlood:
                fields[0] = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                fields[0] = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                fields[0] = IPV6_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV6ResvdMcPktFlood:
                fields[0] = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                fields[0] = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveDrop:
                fields[0] = IGMP_REP_LEAVE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveFlood:
                fields[0] = IGMP_REP_LEAVE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveToCpu:
                fields[0] = IGMP_REP_LEAVE_TO_CPUf;
                break;
            case bcmSwitchIgmpQueryDrop:
                fields[0] = IGMP_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpQueryFlood:
                fields[0] = IGMP_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpQueryToCpu:
                fields[0] = IGMP_QUERY_TO_CPUf;
                break;
            case bcmSwitchIgmpUnknownDrop:
                fields[0] = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpUnknownFlood:
                fields[0] = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpUnknownToCpu:
                fields[0] = IGMP_UNKNOWN_MSG_TO_CPUf;
                break;
            case bcmSwitchMldReportDoneDrop:
                fields[0] = MLD_REP_DONE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldReportDoneFlood:
                fields[0] = MLD_REP_DONE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldReportDoneToCpu:
                fields[0] = MLD_REP_DONE_TO_CPUf;
                break;
            case bcmSwitchMldQueryDrop:
                fields[0] = MLD_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldQueryFlood:
                fields[0] = MLD_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldQueryToCpu:
                fields[0] = MLD_QUERY_TO_CPUf;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryDrop:
                fields[0] = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryFlood:
                fields[0] = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryToCpu:
                fields[0] = IPV4_MC_ROUTER_ADV_PKT_TO_CPUf;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryDrop:
                fields[0] = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryFlood:
                fields[0] = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryToCpu:
                fields[0] = IPV6_MC_ROUTER_ADV_PKT_TO_CPUf;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else if (soc_feature(unit, soc_feature_proto_pkt_ctrl)){
        reg = PROTOCOL_PKT_CONTROLr;
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                fields[0] = IGMP_PKT_TO_CPUf;
                break;
            case bcmSwitchIgmpPktDrop:
                fields[0] = IGMP_PKT_DROPf;
                break;
            case bcmSwitchV4ResvdMcPktToCpu:
                fields[0] = IPV4_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                fields[0] = IPV4_RESVD_MC_PKT_DROPf;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                fields[0] = IPV6_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                fields[0] = IPV6_RESVD_MC_PKT_DROPf;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else {
        return (BCM_E_UNAVAIL);
    }

    if (SOC_REG_INFO(unit, reg).regtype != soc_portreg) {
        return _bcm_gh2_prot_pkt_profile_set(unit, reg, port, fcount, fields,
                                             values);
    } else {
        return soc_reg_fields32_modify(unit, reg, port, fcount, fields,
                                       values);
    }
}


/*
 * Function:
 *      _bcm_gh2_igmp_action_get
 * Description:
 *      Get the IGMP/MLD registers
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      *arg  - IGMP / MLD actions
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_igmp_action_get(int unit,
                         bcm_port_t port,
                         bcm_switch_control_t type,
                         int *arg)
{
    uint32      igmp;
    soc_field_t field = INVALIDf;
    soc_reg_t   reg = INVALIDr;
    int         act_value = BCM_SWITCH_RESERVED_VALUE;
    int         hw_value;

    if (soc_feature(unit, soc_feature_igmp_mld_support)) {
        reg = IGMP_MLD_PKT_CONTROLr;
        /* Given control type select register field. */
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                field = IGMP_REP_LEAVE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpPktDrop:
                field = IGMP_REP_LEAVE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchMldPktToCpu:
                field = MLD_REP_DONE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchMldPktDrop:
                field = MLD_REP_DONE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktToCpu:
                field = IPV4_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV4ResvdMcPktFlood:
                field = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                field = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                field = IPV6_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV6ResvdMcPktFlood:
                field = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                field = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveDrop:
                field = IGMP_REP_LEAVE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveFlood:
                field = IGMP_REP_LEAVE_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveToCpu:
                field = IGMP_REP_LEAVE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpQueryDrop:
                field = IGMP_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpQueryFlood:
                field = IGMP_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIgmpQueryToCpu:
                field = IGMP_QUERY_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpUnknownDrop:
                field = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpUnknownFlood:
                field = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIgmpUnknownToCpu:
                field = IGMP_UNKNOWN_MSG_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchMldReportDoneDrop:
                field = MLD_REP_DONE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchMldReportDoneFlood:
                field = MLD_REP_DONE_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchMldReportDoneToCpu:
                field = MLD_REP_DONE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchMldQueryDrop:
                field = MLD_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchMldQueryFlood:
                field = MLD_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchMldQueryToCpu:
                field = MLD_QUERY_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryDrop:
                field = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryFlood:
                field = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryToCpu:
                field = IPV4_MC_ROUTER_ADV_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryDrop:
                field = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryFlood:
                field = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryToCpu:
                field = IPV6_MC_ROUTER_ADV_PKT_TO_CPUf;
                act_value = 1;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else if (soc_feature(unit, soc_feature_proto_pkt_ctrl)){
        reg = PROTOCOL_PKT_CONTROLr;
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                field = IGMP_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpPktDrop:
                field = IGMP_PKT_DROPf;
                act_value = 1;
                break;
            case bcmSwitchV4ResvdMcPktToCpu:
                field = IPV4_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                field = IPV4_RESVD_MC_PKT_DROPf;
                act_value = 1;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                field = IPV6_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                field = IPV6_RESVD_MC_PKT_DROPf;
                act_value = 1;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else {
        return (BCM_E_UNAVAIL);
    }

    if (soc_reg_field_valid(unit, reg, field)) {
        if (soc_mem_field_valid(unit, PORT_TABm, PROTOCOL_PKT_INDEXf)) {
            int index;

            BCM_IF_ERROR_RETURN
                (_bcm_gh2_protocol_pkt_index_get(unit, port, &index));
            if(SOC_REG_INFO(unit, reg).regtype == soc_portreg) {
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, (soc_port_t)index,
                                                  0, &igmp));
            } else {
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY,
                                                  index, &igmp));
            }
        } else {

            if (IGMP_MLD_PKT_CONTROLr == reg) {
                BCM_IF_ERROR_RETURN(READ_IGMP_MLD_PKT_CONTROLr(unit, port, &igmp));
            } else {
                BCM_IF_ERROR_RETURN(READ_PROTOCOL_PKT_CONTROLr(unit, port, &igmp));
            }
        }
        hw_value = soc_reg_field_get(unit, reg, igmp, field);
        *arg = (act_value == hw_value) ? 1 : 0;
        return (BCM_E_NONE);
    }

    return (BCM_E_UNAVAIL);
}

STATIC int
_bcm_gh2_layered_qos_resolution_set(int unit,
                                    bcm_port_t port,
                                    bcm_switch_control_t type,
                                    int arg)
{
    uint64 rval64;
    uint32 rval, value;

    value = arg ? 0 : 1; /* layered resolution  : serial resolution */
    BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          IGNORE_PPD0_PRESERVE_QOSf, value);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          IGNORE_PPD2_PRESERVE_QOSf, value);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));
    BCM_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval,
                      DISABLE_PPD0_PRESERVE_QOSf, value);
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval,
                      DISABLE_PPD2_PRESERVE_QOSf, value);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    return BCM_E_NONE;

}


STATIC int
_bcm_gh2_layered_qos_resolution_get(int unit,
                                    bcm_port_t port,
                                    bcm_switch_control_t type,
                                    int *arg)
{
    uint64 rval64;
    BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    *arg = soc_reg64_field32_get(unit, ING_CONFIG_64r, rval64,
                                 IGNORE_PPD0_PRESERVE_QOSf) ? 0 : 1;
    return BCM_E_NONE;

}

/*
 * Update following reg/field
 *     FP_METER_CONTROLr, PACKET_IFG_BYTESf
 *     EGR_COUNTER_CONTROLr, PACKET_IFG_BYTESf
 *     EGR_COUNTER_CONTROLr, PACKET_IFG_BYTES_2f
 *     EFP_METER_CONTROL_2r, PACKET_IFG_BYTESf
 *     EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf
 */
STATIC int
_bcm_gh2_meter_adjust_set(int unit, bcm_port_t port, int arg)
{
    int len, max_val;
    int i;
    soc_reg_t reg[4] = { FP_METER_CONTROLr,
                         EGR_COUNTER_CONTROLr,
                         EFP_METER_CONTROL_2r,
                         EGR_SHAPING_CONTROLr };

    for (i = 0; i < 4 ; ++i ) {
        if (SOC_REG_FIELD_VALID(unit, reg[i], PACKET_IFG_BYTESf)) {
            len = soc_reg_field_length(unit, reg[i], PACKET_IFG_BYTESf);
            max_val = (1 << len) - 1;
            if (arg < 0 || arg > max_val) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, reg[i], port,
                                       PACKET_IFG_BYTESf, arg));
        }

        if (SOC_REG_FIELD_VALID(unit, reg[i], PACKET_IFG_BYTES_2f)) {
            len = soc_reg_field_length(unit, reg[i], PACKET_IFG_BYTES_2f);
            max_val = (1 << len) - 1;
            if (arg < 0 || arg > max_val) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, reg[i], port,
                                       PACKET_IFG_BYTES_2f, arg));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_meter_adjust_get(int unit, bcm_port_t port, int *arg)
{
    uint32 val;

    BCM_IF_ERROR_RETURN(
        READ_EGR_COUNTER_CONTROLr(unit, port, &val));
    *arg = soc_reg_field_get(unit, EGR_COUNTER_CONTROLr, val,
                             PACKET_IFG_BYTESf);

    return BCM_E_NONE;
}

/*
 * Update following reg/field
 * EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf
 */
STATIC int
_bcm_gh2_shaper_adjust_set(int unit, bcm_port_t port, int arg)
{
    int len, max_val;
    soc_reg_t reg = EGR_SHAPING_CONTROLr;

    len = soc_reg_field_length(unit, reg, PACKET_IFG_BYTESf);
    max_val = (1 << len) - 1;
    if (arg < 0 || arg > max_val) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit, reg, port,
                               PACKET_IFG_BYTESf, arg));
    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_shaper_adjust_get(int unit, bcm_port_t port, int *arg)
{
    uint32 val;

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, EGR_SHAPING_CONTROLr, port, 0, &val));
    *arg = soc_reg_field_get(unit, EGR_SHAPING_CONTROLr, val,
                             PACKET_IFG_BYTESf);

    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_gh2_switch_encap_scache_sync
 * Purpose:
 *      Syncs switch encap warmboot state to scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_gh2_switch_encap_scache_sync(int unit)
{
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCACHE_PART_ENCAP);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_SWITCH_WB_ENCAP_DEFAULT_VERSION, NULL));
    if (soc_feature(unit, soc_feature_miml) ||
        soc_feature(unit, soc_feature_custom_header)) {

        BCM_IF_ERROR_RETURN(_bcm_hr3_switch_encap_sync(unit, &scache_ptr));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_switch_misc_scache_sync
 * Purpose:
 *      Syncs L2 part warmboot state to scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_gh2_switch_misc_scache_sync(int unit)
{
    uint8   *scache_ptr;
    int     size;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCACHE_PART_MISC);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_SWITCH_WB_MISC_DEFAULT_VERSION, NULL));

    /* bcmSwitchHashDualMoveDepth */
    sal_memcpy(scache_ptr, &SOC_DUAL_HASH_MOVE_MAX(unit),
               sizeof(SOC_DUAL_HASH_MOVE_MAX(unit)));
    scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX(unit));
    /* bcmSwitchHashDualMoveDepthL2 */
    sal_memcpy(scache_ptr, &SOC_DUAL_HASH_MOVE_MAX_L2X(unit),
               sizeof(SOC_DUAL_HASH_MOVE_MAX_L2X(unit)));
    scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_L2X(unit));
#if defined(INCLUDE_L3)
    /* bcmSwitchHashDualMoveDepthL3 */
    sal_memcpy(scache_ptr, &SOC_DUAL_HASH_MOVE_MAX_L3X(unit),
               sizeof(SOC_DUAL_HASH_MOVE_MAX_L3X(unit)));
    scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_L3X(unit));
#endif /* INCLUDE_L3 */
    /* bcmSwitchHashDualMoveDepthVlan */
    sal_memcpy(scache_ptr, &SOC_DUAL_HASH_MOVE_MAX_VLAN(unit),
               sizeof(SOC_DUAL_HASH_MOVE_MAX_VLAN(unit)));
    scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_VLAN(unit));
    /* bcmSwitchHashDualMoveDepthMpls */
    sal_memcpy(scache_ptr, &SOC_DUAL_HASH_MOVE_MAX_MPLS(unit),
               sizeof(SOC_DUAL_HASH_MOVE_MAX_MPLS(unit)));
    scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_MPLS(unit));
    /* bcmSwitchHashDualMoveDepthEgressVlan */
    sal_memcpy(scache_ptr, &SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit),
               sizeof(SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit)));
    scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit));
    /* bcmSwitchL3TunnelIpV4ModeOnly */
    soc_tunnel_term_block_size_get(unit, &size);
    sal_memcpy(scache_ptr, &size, sizeof(size));
    scache_ptr += sizeof(size);
    /* bcmSwitchL2OverflowEvent */
    sal_memcpy(scache_ptr, &SOC_CONTROL(unit)->l2_overflow_enable,
               sizeof(SOC_CONTROL(unit)->l2_overflow_enable));
    scache_ptr += sizeof(SOC_CONTROL(unit)->l2_overflow_enable);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_switch_scache_sync
 * Purpose:
 *      Syncs switch warmboot state to scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_gh2_switch_scache_sync(int unit)
{
    if (soc_feature(unit, soc_feature_dual_hash)) {
        BCM_IF_ERROR_RETURN(_bcm_gh2_switch_misc_scache_sync(unit));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_switch_control_sync(int unit, int arg)
{
    int rv = BCM_E_NONE;
    int backup_size;
    uint8 *scache_module_ptr = NULL;

    BCM_IF_ERROR_RETURN(soc_scache_module_max_alloc(unit,
                            &scache_module_ptr, &backup_size));
    {
        /* Call each module to write Level 2 Warm Boot info */
        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_VLAN, (&scache_module_ptr),
                                0, 0, backup_size, soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_VLAN,
                         _bcm_esw_vlan_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_VLAN, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_STG, (&scache_module_ptr),
                                0, 0, backup_size, soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_STG,
                         _bcm_esw_stg_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_STG, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_COSQ, (&scache_module_ptr),
                                0, 0, backup_size, soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_COSQ,
                         _bcm_esw_cosq_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_COSQ, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_MIRROR, (&scache_module_ptr),
                                0, 0, backup_size, soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_MIRROR,
                         _bcm_esw_mirror_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_MIRROR, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

        if (soc_feature(unit, soc_feature_virtual_switching) ||
            soc_feature(unit, soc_feature_gport_service_counters) ||
            soc_feature(unit, soc_feature_fast_init) ||
            soc_feature(unit, soc_feature_int_common_init)){
            _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_COMMON, (&scache_module_ptr),
                                    0, 0, backup_size, soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_COMMON,
                             _bcm_common_scache_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_COMMON, (&scache_module_ptr),
                                   0, 0, soc_scache_module_dirty_set, rv);
        }

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_UDF, (&scache_module_ptr),
                                0, 0, backup_size, soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_UDF,
                         _bcm_esw_udf_scache_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_UDF, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_SWITCH, (&scache_module_ptr),
                                1, 9, backup_size, soc_scache_module_data_backup, rv);

        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_SWITCH,
                         bcmi_gh2_switch_scache_sync, (&scache_module_ptr), rv);

        if (soc_feature(unit, soc_feature_miml) ||
            soc_feature(unit, soc_feature_custom_header)) {
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_SWITCH,
            _bcm_gh2_switch_encap_scache_sync, (&scache_module_ptr), rv);
        }

        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_SWITCH, (&scache_module_ptr),
                               1, 9, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_FIELD, (&scache_module_ptr),
                                0, 1, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_FIELD,
                         _bcm_esw_field_scache_sync, (&scache_module_ptr), rv);
        if (rv == BCM_E_UNAVAIL) {
             rv = BCM_E_NONE;
        }
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_FIELD, (&scache_module_ptr),
                               0, 1, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_RX, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_RX,
                         _bcm_esw_rx_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_RX, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

#if defined(INCLUDE_XFLOW_MACSEC)
        if (soc_feature(unit, soc_feature_xflow_macsec)) {
            _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_XFLOW_MACSEC, (&scache_module_ptr),
                                    0, 0, backup_size, soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_XFLOW_MACSEC,
                            _bcm_common_xflow_macsec_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_XFLOW_MACSEC, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);
        }
#endif
#ifdef INCLUDE_L3
        if (soc_feature(unit, soc_feature_virtual_switching)) {
            _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_VIRTUAL, (&scache_module_ptr),
                                    0, 0, backup_size,soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_COMMON,
                             _bcm_esw_virtual_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_VIRTUAL, (&scache_module_ptr),
                                   0, 0, soc_scache_module_dirty_set, rv);
        }
         _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_L3, (&scache_module_ptr),
                                 0, 0, backup_size,soc_scache_module_data_backup, rv);

         _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_L3,
                          _bcm_esw_l3_sync, (&scache_module_ptr), rv);
         _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_L3, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);

         _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_IPMC, (&scache_module_ptr),
                                 0, 0, backup_size,soc_scache_module_data_backup, rv);
         _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_IPMC,
                          _bcm_esw_ipmc_sync, (&scache_module_ptr), rv);
         _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_IPMC, (&scache_module_ptr),
                                0, 0, soc_scache_module_dirty_set, rv);

         if (soc_feature(unit, soc_feature_mpls)) {
             _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_MPLS, (&scache_module_ptr),
                                     0, 0, backup_size,soc_scache_module_data_backup, rv);
             _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_MPLS,
                              _bcm_esw_mpls_sync, (&scache_module_ptr), rv);
             _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_MPLS, (&scache_module_ptr),
                                    0, 0, soc_scache_module_dirty_set, rv);

         }

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_MULTICAST, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_MULTICAST,
                         _bcm_esw_multicast_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_MULTICAST, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);


#endif /* INCLUDE_L3 */
        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_TRUNK, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_TRUNK,
                         _bcm_esw_trunk_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_TRUNK, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_LINKSCAN, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_LINKSCAN,
                         _bcm_esw_link_sync,(&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_LINKSCAN, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_PORT, (&scache_module_ptr),
                                BCM_PORT_WB_SCACHE_PART_MIN,
                                BCM_PORT_WB_SCACHE_PART_MAX,
                                backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_PORT,
                         _bcm_esw_port_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_PORT, (&scache_module_ptr),
                               BCM_PORT_WB_SCACHE_PART_MIN,
                               BCM_PORT_WB_SCACHE_PART_MAX,
                               soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_OAM, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_OAM,
                         _bcm_esw_oam_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_OAM, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);


        if (soc_feature(unit, soc_feature_time_support)) {
            _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_TIME, (&scache_module_ptr),
                                    0, 0, backup_size,soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_TIME,
                     _bcm_time_scache_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_TIME, (&scache_module_ptr),
                                   0, 0, soc_scache_module_dirty_set, rv);
        }

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_QOS, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_QOS,
                         _bcm_esw_qos_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_QOS, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_STACK, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_STK,
                         _bcm_esw_stk_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_STACK, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, SOC_SCACHE_SWITCH_CONTROL, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        rv = _bcm_switch_control_scache_sync(unit);
        if (!_BCM_SYNC_SUCCESS(rv)) {
            LOG_CLI((BSL_META_U(unit, "Switch_control_sync failed\n")));
            sal_free(scache_module_ptr);
            return rv;
        }
        _BCM_SYNC_MODULE_DIRTY(unit, SOC_SCACHE_SWITCH_CONTROL, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

#ifdef INCLUDE_L3
        if (soc_feature(unit, soc_feature_vxlan_lite)) {
            _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_VXLAN,
                                    (&scache_module_ptr), 0, 0, backup_size,
                                    soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_VXLAN,
                             _bcm_esw_vxlan_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_VXLAN,
                                   (&scache_module_ptr), 0, 0,
                                   soc_scache_module_dirty_set, rv);
        }

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_NIV, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_NIV,
                         _bcm_esw_niv_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_NIV, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_SUBPORT, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_SUBPORT,
                         _bcm_esw_subport_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_SUBPORT, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);
#endif /* INCLUDE_L3 */

#if defined(INCLUDE_REGEX)
    /* No WARMBOOT support for REGEX */
#endif /* INCLUDE_REGEX */

        _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_STAT, (&scache_module_ptr),
                                0, 1, backup_size,soc_scache_module_data_backup, rv);
        _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_STAT,
                         _bcm_esw_stat_sync, (&scache_module_ptr), rv);
        _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_STAT, (&scache_module_ptr),
                               0, 1, soc_scache_module_dirty_set, rv);

#if defined(BCM_TSN_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn)) {
            _BCM_SYNC_MODULE_BACKUP(
                unit, BCM_MODULE_TSN, (&scache_module_ptr),
                BCM_TSN_WB_SCACHE_PART_MIN,
                BCM_TSN_WB_SCACHE_PART_MAX,
                backup_size, soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(
                unit, SHR_BPROF_BCM_TSN,
                bcm_esw_tsn_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(
                unit, BCM_MODULE_TSN, (&scache_module_ptr),
                BCM_TSN_WB_SCACHE_PART_MIN,
                BCM_TSN_WB_SCACHE_PART_MAX,
                soc_scache_module_dirty_set, rv);
        }
#endif /* BCM_TSN_SUPPORT */

#if defined(BCM_GLOBAL_METER_SUPPORT)
        if (soc_feature(unit, soc_feature_global_meter)) {
            _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_POLICER, (&scache_module_ptr),
                                    0, 0, backup_size,soc_scache_module_data_backup, rv);
            _BCM_SYNC_MODULE(unit, SHR_BPROF_BCM_GLB_METER,
                             _bcm_esw_global_meter_policer_sync, (&scache_module_ptr), rv);
            _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_POLICER, (&scache_module_ptr),
                                   0, 0, soc_scache_module_dirty_set, rv);
        }
#endif /* BCM_GLOBAL_METER_SUPPORT */
    }

    if (_BCM_SYNC_SUCCESS(rv)) {
        _BCM_SYNC_MODULE_BACKUP(unit, SOC_SCACHE_MEMCACHE_HANDLE, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
        rv = _bcm_mem_scache_sync(unit);
        if (rv == BCM_E_UNAVAIL) {
            rv = BCM_E_NONE;
        }
        _BCM_SYNC_MODULE_DIRTY(unit, SOC_SCACHE_MEMCACHE_HANDLE, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);
    }
    if (_BCM_SYNC_SUCCESS(rv)) {
        _BCM_SYNC_MODULE_BACKUP(unit, SOC_SCACHE_FLEXIO_HANDLE, (&scache_module_ptr),
                                0, 0, backup_size,soc_scache_module_data_backup, rv);
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            rv = soc_fl_port_init_speed_scache_sync(unit);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            rv = soc_gh2_port_init_speed_scache_sync(unit);
        }
        if (!_BCM_SYNC_SUCCESS(rv)) {
            LOG_CLI((BSL_META_U(unit, "soc port init speed sync failed\n")));
            sal_free(scache_module_ptr);
            return rv;
        }
        _BCM_SYNC_MODULE_DIRTY(unit, SOC_SCACHE_FLEXIO_HANDLE, (&scache_module_ptr),
                               0, 0, soc_scache_module_dirty_set, rv);
    }
    _BCM_SYNC_MODULE_BACKUP(unit, BCM_MODULE_L2, (&scache_module_ptr),
                            0, 0, backup_size, soc_scache_module_data_backup, rv);
    rv = _bcm_esw_l2_wb_sync(unit);
    _BCM_SYNC_MODULE_DIRTY(unit, BCM_MODULE_L2, (&scache_module_ptr),
                           0, 0, soc_scache_module_dirty_set, rv);

    sal_free(scache_module_ptr);
    /* Now send all data to the persistent storage */
    if (_BCM_SYNC_SUCCESS(rv)) {
        rv = soc_scache_commit(unit);
    }

    /* Mark scache as clean */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 0;
    SOC_CONTROL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_gh2_switch_encap_wb_scache_size_get
 * Purpose:
 *      Returns required scache size for switch encap module
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      req_scache_size      - (OUT) Request scache size
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_gh2_switch_encap_wb_scache_size_get(int unit,
    int *req_scache_size)
{
    if (soc_feature(unit, soc_feature_miml) ||
        soc_feature(unit, soc_feature_custom_header)) {
#if defined(BCM_HURRICANE3_SUPPORT)
        BCM_IF_ERROR_RETURN(
            _bcm_hr3_switch_encap_wb_scache_size_get(unit, req_scache_size));
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_switch_misc_wb_scache_size_get
 * Purpose:
 *      Returns required scache size for L2 part
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      req_scache_size      - (OUT) Request scache size
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_gh2_switch_misc_wb_scache_size_get(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    if (req_scache_size == NULL) {
        return BCM_E_PARAM;
    }

    /* bcmSwitchHashDualMoveDepth */
    alloc_size += sizeof(SOC_DUAL_HASH_MOVE_MAX(unit));

    /* bcmSwitchHashDualMoveDepthL2 */
    alloc_size += sizeof(SOC_DUAL_HASH_MOVE_MAX_L2X(unit));

#if defined(INCLUDE_L3)
    /* bcmSwitchHashDualMoveDepthL3 */
    alloc_size += sizeof(SOC_DUAL_HASH_MOVE_MAX_L3X(unit));
#endif /* INCLUDE_L3 */
    /* bcmSwitchHashDualMoveDepthVlan */
    alloc_size += sizeof(SOC_DUAL_HASH_MOVE_MAX_VLAN(unit));

    /* bcmSwitchHashDualMoveDepthMpls */
    alloc_size += sizeof(SOC_DUAL_HASH_MOVE_MAX_MPLS(unit));

    /* bcmSwitchHashDualMoveDepthEgressVlan */
    alloc_size += sizeof(SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit));

    /* bcmSwitchL3TunnelIpV4ModeOnly */
    alloc_size += sizeof(int);

    /* bcmSwitchL2OverflowEvent */
    alloc_size += sizeof(SOC_CONTROL(unit)->l2_overflow_enable);

    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_switch_encap_wb_alloc
 * Purpose:
 *      Allocates required scache size for the switch encap module recovery
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_gh2_switch_encap_wb_alloc(int unit)
{
    int rv;
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    int req_scache_size = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCACHE_PART_ENCAP);

    rv = _bcm_gh2_switch_encap_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
            req_scache_size, &scache_ptr, BCM_SWITCH_WB_ENCAP_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_switch_misc_wb_alloc
 * Purpose:
 *      Allocates required scache size for the L2 part recovery
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_gh2_switch_misc_wb_alloc(int unit)
{
    int     rv;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;
    int     req_scache_size = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCACHE_PART_MISC);

    rv = _bcm_gh2_switch_misc_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 req_scache_size, &scache_ptr,
                                 BCM_SWITCH_WB_MISC_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_gh2_switch_encap_reinit
 * Purpose:
 *      Recovers switch encap warmboot state from scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_gh2_switch_encap_reinit(int unit)
{
    int rv = BCM_E_INTERNAL;
    uint8 *scache_ptr;
    uint16 recovered_ver = 0;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCACHE_PART_ENCAP);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_SWITCH_WB_ENCAP_DEFAULT_VERSION, &recovered_ver);

    if (rv == BCM_E_NOT_FOUND) {
        return _bcm_gh2_switch_encap_wb_alloc(unit);
    } else if (rv == BCM_E_NONE) {
        if (recovered_ver >= BCM_SWITCH_WB_ENCAP_VERSION_1_0) {
#if defined(BCM_HURRICANE3_SUPPORT)
            BCM_IF_ERROR_RETURN(_bcm_hr3_switch_encap_reinit(unit, &scache_ptr));
#endif /* BCM_HURRICANE3_SUPPORT */
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_gh2_switch_misc_reinit
 * Purpose:
 *      Recovers L2 part warmboot state from scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_gh2_switch_misc_reinit(int unit)
{
    int     rv = BCM_E_INTERNAL, size;
    uint8   *scache_ptr;
    uint16  recovered_ver = 0;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCACHE_PART_MISC);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_SWITCH_WB_MISC_DEFAULT_VERSION,
                                 &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        BCM_IF_ERROR_RETURN(_bcm_gh2_switch_misc_wb_alloc(unit));
        return BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {
        if (recovered_ver >= BCM_SWITCH_WB_MISC_VERSION_1_0) {
            /* bcmSwitchHashDualMoveDepth */
            sal_memcpy(&SOC_DUAL_HASH_MOVE_MAX(unit), scache_ptr,
                       sizeof(SOC_DUAL_HASH_MOVE_MAX(unit)));
            scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX(unit));
            /* bcmSwitchHashDualMoveDepthL2 */
            sal_memcpy(&SOC_DUAL_HASH_MOVE_MAX_L2X(unit), scache_ptr,
                       sizeof(SOC_DUAL_HASH_MOVE_MAX_L2X(unit)));
            scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_L2X(unit));
#if defined(INCLUDE_L3)
            /* bcmSwitchHashDualMoveDepthL3 */
            sal_memcpy(&SOC_DUAL_HASH_MOVE_MAX_L3X(unit), scache_ptr,
                       sizeof(SOC_DUAL_HASH_MOVE_MAX_L3X(unit)));
            scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_L3X(unit));
#endif /* INCLUDE_L3 */
            /* bcmSwitchHashDualMoveDepthVlan */
            sal_memcpy(&SOC_DUAL_HASH_MOVE_MAX_VLAN(unit), scache_ptr,
                       sizeof(SOC_DUAL_HASH_MOVE_MAX_VLAN(unit)));
            scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_VLAN(unit));
            /* bcmSwitchHashDualMoveDepthMpls */
            sal_memcpy(&SOC_DUAL_HASH_MOVE_MAX_MPLS(unit), scache_ptr,
                       sizeof(SOC_DUAL_HASH_MOVE_MAX_MPLS(unit)));
            scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_MPLS(unit));
            /* bcmSwitchHashDualMoveDepthEgressVlan */
            sal_memcpy(&SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit), scache_ptr,
                       sizeof(SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit)));
            scache_ptr += sizeof(SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit));
            /* bcmSwitchL3TunnelIpV4ModeOnly */
            sal_memcpy(&size, scache_ptr, sizeof(size));
            soc_tunnel_term_block_size_set(unit, size);
            scache_ptr += sizeof(size);
            /* bcmSwitchL2OverflowEvent */
            sal_memcpy(&SOC_CONTROL(unit)->l2_overflow_enable, scache_ptr,
                       sizeof(SOC_CONTROL(unit)->l2_overflow_enable));
            scache_ptr += sizeof(SOC_CONTROL(unit)->l2_overflow_enable);
        }

        if (recovered_ver < BCM_SWITCH_WB_MISC_DEFAULT_VERSION) {
            BCM_IF_ERROR_RETURN(_bcm_gh2_switch_misc_wb_alloc(unit));
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmi_gh2_switch_vxlan_tpid_reinit
 * Description:
 *      Recovers VXLAN tpid information
 * Parameters:
 *      unit - (IN) Device Number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_switch_vxlan_tpid_reinit(int unit)
{
    uint64 reg_val64;
    int tpid_enable, tpid_index;

    /* Recover TPID reference count from ING_VXLAN_CONTROLr */
    COMPILER_64_ZERO(reg_val64);
    BCM_IF_ERROR_RETURN(READ_ING_VXLAN_CONTROLr(unit, &reg_val64));

    tpid_enable = soc_reg64_field32_get(unit, ING_VXLAN_CONTROLr,
                                        reg_val64,
                                        PAYLOAD_OUTER_TPID_ENABLEf);

    tpid_index = 0;
    while (tpid_enable) {
        if (tpid_enable & 0x1) {
            BCM_IF_ERROR_RETURN(
                _bcm_fb2_outer_tpid_tab_ref_count_add(unit, tpid_index, 1));
        }
        tpid_enable >>= 1;
        tpid_index++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_switch_wb_alloc
 * Purpose:
 *      Allocates required scache size for the switch module recovery
 * Parameters:
 *      unit - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_gh2_switch_wb_alloc(int unit)
{
    if (soc_feature(unit, soc_feature_miml) ||
        soc_feature(unit, soc_feature_custom_header)) {
        BCM_IF_ERROR_RETURN(_bcm_gh2_switch_encap_wb_alloc(unit));
    }

    if (soc_feature(unit, soc_feature_dual_hash)) {
        BCM_IF_ERROR_RETURN(_bcm_gh2_switch_misc_wb_alloc(unit));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_switch_reinit
 * Purpose:
 *      Recovers switch warmboot state from scache
 * Parameters:
 *      unit - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_gh2_switch_reinit(int unit)
{
    if (soc_feature(unit, soc_feature_miml) ||
        soc_feature(unit, soc_feature_custom_header)) {
        BCM_IF_ERROR_RETURN(_bcm_gh2_switch_encap_reinit(unit));
    }

    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        BCM_IF_ERROR_RETURN(bcmi_gh2_switch_vxlan_tpid_reinit(unit));
    }

    if (soc_feature(unit, soc_feature_dual_hash)) {
        BCM_IF_ERROR_RETURN(_bcm_gh2_switch_misc_reinit(unit));
    }

    return BCM_E_NONE;
}

#endif /* WARM_BOOT_SUPPORT */

static uint8 *_mod_type_table[BCM_LOCAL_UNITS_MAX];
STATIC int
_bcm_gh2_switch_module_type_set(int unit, bcm_module_t mod, uint32 mod_type)
{
    int modid_cnt;

    if ((mod > SOC_MODID_MAX(unit)) || (mod_type != BCM_SWITCH_MODULE_XGS3)) {
        return BCM_E_PARAM;
    } else {
        if (NULL == _mod_type_table[unit]) {
            if (SOC_MODID_MAX(unit)) {
                modid_cnt = SOC_MODID_MAX(unit);
            } else {
                modid_cnt = 1;
            }
            _mod_type_table[unit] = (uint8 *)sal_alloc(
                (sizeof(uint8) * modid_cnt), "MOD TYPE");
            if (NULL == _mod_type_table[unit]) {
                return BCM_E_MEMORY;
            }
            sal_memset(_mod_type_table[unit], BCM_SWITCH_MODULE_UNKNOWN,
                       (sizeof(uint8) * modid_cnt));
        }
        _mod_type_table[unit][mod] = mod_type;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_switch_module_type_get(int unit, bcm_module_t mod, uint32 *mod_type)
{
    *mod_type = BCM_SWITCH_MODULE_UNKNOWN;
    if (mod > SOC_MODID_MAX(unit)) {
        return BCM_E_PARAM;
    } else {
        if (NULL != _mod_type_table[unit]) {
            *mod_type = _mod_type_table[unit][mod];
        }
    }
    return BCM_E_NONE;
}

/*
 * Bulk clear of various hit bit tables
 */
STATIC int
_bcm_gh2_switch_hit_clear_set(int unit, bcm_switch_control_t type, int arg)
{
    if (arg != 1) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmSwitchL2HitClear:
            CLEARMEM(unit, L2_HITSA_ONLYm);
            CLEARMEM(unit, L2_HITDA_ONLYm);
            return BCM_E_NONE;

            break;
        case bcmSwitchL2SrcHitClear:
            CLEARMEM(unit, L2_HITSA_ONLYm);
            return BCM_E_NONE;
            break;
        case bcmSwitchL2DstHitClear:
            CLEARMEM(unit, L2_HITDA_ONLYm);
            return BCM_E_NONE;
            break;
        case bcmSwitchL3HostHitClear:
            CLEARMEM(unit, L3_ENTRY_HIT_ONLYm);
            return BCM_E_NONE;
            break;
        case bcmSwitchL3RouteHitClear:
            CLEARMEM(unit, L3_DEFIP_HIT_ONLYm);
            CLEARMEM(unit, L3_DEFIP_128_HIT_ONLYm);
            return BCM_E_NONE;
            break;
        default:
            break;
    }

    return BCM_E_UNAVAIL;
}

#undef CLEARMEM

/*
 * Return an error if a hit clear is not supportable on this device
 */
STATIC int
_bcm_gh2_switch_hit_clear_get(int unit, bcm_switch_control_t type, int *arg)
{
    int         rv;

    rv = BCM_E_UNAVAIL;

    switch (type) {
        case bcmSwitchL2HitClear:
            rv = BCM_E_NONE;
            break;
        case bcmSwitchL2SrcHitClear:
            rv = BCM_E_NONE;
            break;
        case bcmSwitchL2DstHitClear:
            rv = BCM_E_NONE;
            break;
        case bcmSwitchL3HostHitClear:
            if (!soc_feature(unit, soc_feature_l3) ||
                soc_feature(unit, soc_feature_fp_based_routing)) {
                break;
            }
            rv = BCM_E_NONE;
            break;
        case bcmSwitchL3RouteHitClear:
            if (!soc_feature(unit, soc_feature_l3) ||
                soc_feature(unit, soc_feature_fp_based_routing)) {
                break;
            }
            rv = BCM_E_NONE;
            break;
        default:
            break;
    }
    if (rv == BCM_E_NONE && arg != NULL) {
        *arg = 0;
    }
    return rv;
}

#if (defined(BCM_RCPU_SUPPORT) || defined(BCM_OOB_RCPU_SUPPORT)) && \
     defined(BCM_XGS3_SWITCH_SUPPORT)
/*
 * Function:
 *      _bcm_gh2_rcpu_switch_regall_idx_set
 * Description:
 *      Helper function to set register to all ones if arg is non zero
 *  and to all zeros otherwise.
 * Parameters:
 *      unit - Device unit number
 *  reg - The desired register to program
 *  idx - The register index
 *  arg - The value with which decission is made
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_gh2_rcpu_switch_regall_idx_set(int unit,
                                soc_reg_t reg,
                                int idx,
                                soc_field_t field,
                                int arg)
{
    uint32  raddr, rval, value, width;

    rval = 0;
    if (arg) {
        width = soc_reg_field_length(unit, reg, field);
        if (32 == width) {
            value = 0xffffffff;
        } else {
            value = (1 << width) - 1;
        }
         soc_reg_field_set(unit, reg, &rval, field, value);
    }
    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, idx);
    return soc_pci_write(unit, raddr, rval);
}

/*
 * Function:
 *      _bcm_gh2_rcpu_switch_regall_idx_get
 * Description:
 *      Helper function to return arg = 1 if register is set to all ones
 *  and to zero otherwise.
 * Parameters:
 *      unit - Device unit number
 *  reg - The desired register to read
 *  arg - The value returnrd
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_gh2_rcpu_switch_regall_idx_get(int unit, soc_reg_t reg, int idx, int *arg)
{
    uint32  raddr, rval;

    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, idx);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(
        soc_pci_getreg(unit, raddr, &rval));

    *arg = (!rval) ? 0 : 1;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_rcpu_switch_regall_set
 * Description:
 *      Helper function to set register to all ones if arg is non zero
 *  and to all zeros otherwise.
 * Parameters:
 *      unit - Device unit number
 *  reg - The desired register to program
 *  arg - The value with which decission is made
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_gh2_rcpu_switch_regall_set(int unit, soc_reg_t reg, soc_field_t field, int arg)
{
    return _bcm_gh2_rcpu_switch_regall_idx_set(unit, reg, 0, field, arg);
}

STATIC int
_bcm_gh2_rcpu_switch_regall_get(int unit, soc_reg_t reg, int *arg)
{
    return _bcm_gh2_rcpu_switch_regall_idx_get(unit, reg, 0, arg);
}

/*
 * Function:
 *      _bcm_gh2_rcpu_switch_mac_lo_set
 * Description:
 *      Set the low 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set the mac for
 *      mac_lo      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_rcpu_switch_mac_lo_set(int unit,
                            bcm_switch_control_t type,
                            uint32 mac_lo)
{
    uint32      regval, fieldval;
    soc_reg_t   reg = INVALIDr ;
    soc_field_t f_lo = INVALIDf;


    /* Given control type select register. */
    switch (type) {
        case bcmSwitchRemoteCpuLocalMacNonOui:
            reg = CMIC_PKT_LMAC0_LOr;
            f_lo = MAC0_LOf;
            break;
        case bcmSwitchRemoteCpuDestMacNonOui:
            reg = CMIC_PKT_RMACr;
            f_lo = MAC_LOf;
            break;
        default:
            return BCM_E_PARAM;
    }
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(
        soc_pci_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                       &regval));

    fieldval = soc_reg_field_get(unit, reg, regval, f_lo);
    fieldval &= 0xff000000;
    fieldval |= ((mac_lo << 8) >> 8);
    soc_reg_field_set(unit, reg, &regval, f_lo, fieldval);

    return soc_pci_write(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                         regval);
}

/*
 * Function:
 *      _bcm_gh2_rcpu_switch_mac_lo_get
 * Description:
 *      Get the lower 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the lower MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_rcpu_switch_mac_lo_get(int unit,
                            bcm_switch_control_t type,
                            int *arg)
{
    soc_reg_t   reg = INVALIDr ;
    soc_field_t f_lo = INVALIDf;
    uint32      regval, mac;

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchRemoteCpuLocalMacNonOui:
            reg = CMIC_PKT_LMAC0_LOr;
            f_lo = MAC0_LOf;
            break;
        case bcmSwitchRemoteCpuDestMacNonOui:
            reg = CMIC_PKT_RMACr;
            f_lo = MAC_LOf;
            break;
        default:
            return BCM_E_PARAM;
    }
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(
        soc_pci_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                       &regval));
    mac = soc_reg_field_get(unit, reg, regval, f_lo);

    *arg = (mac << 8) >> 8;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_rcpu_switch_mac_hi_set
 * Description:
 *      Set the upper 24 bits of MAC address field for RCPU registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set upper MAC for
 *      mac_hi      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_rcpu_switch_mac_hi_set(int unit,
                            bcm_switch_control_t type,
                            uint32 mac_hi)
{
    soc_reg_t   reg1, reg2;
    soc_field_t f_lo, f_hi;
    uint32      fieldval, regval1, regval2, raddr1, raddr2;

    reg1 = reg2 = INVALIDr;

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchRemoteCpuLocalMacOui:
            reg1 = CMIC_PKT_LMAC0_LOr;
            reg2 = CMIC_PKT_LMAC0_HIr;
            f_lo = MAC0_LOf;
            f_hi = MAC0_HIf;
            break;
        case bcmSwitchRemoteCpuDestMacOui:
            reg1 = CMIC_PKT_RMACr;
            reg2 = CMIC_PKT_RMAC_HIr;
            f_lo = MAC_LOf;
            f_hi = MAC_HIf;
            break;
        default:
            return BCM_E_PARAM;
    }

    regval1 = regval2 = 0;

    raddr1 = soc_reg_addr(unit, reg1, REG_PORT_ANY, 0);
    raddr2 = soc_reg_addr(unit, reg2, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(
        soc_pci_getreg(unit, raddr1, &regval1));
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(
        soc_pci_getreg(unit, raddr2, &regval2));

    fieldval = (mac_hi << 24);
    soc_reg_field_set(unit, reg1, &regval1, f_lo, fieldval);
    fieldval = (mac_hi >> 8) & 0xffff;
    soc_reg_field_set(unit, reg2, &regval2, f_hi, fieldval);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr1, regval1));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr2, regval2));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_rcpu_switch_mac_hi_get
 * Description:
 *      Get the upper 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the upper MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_gh2_rcpu_switch_mac_hi_get(int unit,
                            bcm_switch_control_t type,
                         int *arg)
{
    soc_reg_t   reg1, reg2;
    soc_field_t f_lo, f_hi;
    uint32      mac, regval1, regval2;

    reg1 = reg2 = INVALIDr;

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchRemoteCpuLocalMacOui:
            reg1 = CMIC_PKT_LMAC0_LOr;
            reg2 = CMIC_PKT_LMAC0_HIr;
            f_lo = MAC0_LOf;
            f_hi = MAC0_HIf;
            break;
        case bcmSwitchRemoteCpuDestMacOui:
            reg1 = CMIC_PKT_RMACr;
            reg2 = CMIC_PKT_RMAC_HIr;
            f_lo = MAC_LOf;
            f_hi = MAC_HIf;
            break;
        default:
            return BCM_E_PARAM;
    }
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(
        soc_pci_getreg(unit, soc_reg_addr(unit, reg1, REG_PORT_ANY, 0),
                       &regval1));
    mac = (soc_reg_field_get(unit, reg1, regval1, f_lo) >> 24);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(
        soc_pci_getreg(unit, soc_reg_addr(unit, reg2, REG_PORT_ANY, 0),
                       &regval2));

    mac |= (soc_reg_field_get(unit, reg2, regval2, f_hi) << 8);

    *arg = (int)mac;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_rcpu_switch_vlan_tpid_sig_set
 * Description:
 *      Helper function to set VLAN, TPID ot Signature for RCPU
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to modify
 *  arg - The value to set
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_gh2_rcpu_switch_vlan_tpid_sig_set(int unit,
                                   bcm_switch_control_t type,
                                   int arg)
{
    soc_reg_t   reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32      value, raddr, rval;

    if (!soc_feature(unit, soc_feature_rcpu_1)) {
        return BCM_E_UNAVAIL;
    }

    switch (type) {
        case bcmSwitchRemoteCpuVlan:
            reg = CMIC_PKT_VLANr;
            field = VLAN_IDf;
            value = (arg & 0xffff);
            break;
        case bcmSwitchRemoteCpuTpid:
            reg = CMIC_PKT_VLANr;
            field = TPIDf;
            value = (arg & 0xffff);
            break;
        case bcmSwitchRemoteCpuSignature:
            reg = CMIC_PKT_ETHER_SIGr;
            field = SIGNATUREf;
            value = (arg & 0xffff);
            break;
        case bcmSwitchRemoteCpuEthertype:
            reg = CMIC_PKT_ETHER_SIGr;
            field = ETHERTYPEf;
            value = (arg & 0xffff);
            break;
        default:
            return BCM_E_PARAM;
    }

    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(
        soc_pci_getreg(unit, raddr, &rval));
    soc_reg_field_set(unit, reg, &rval, field, value);

    return soc_pci_write(unit, raddr, rval);
}

/*
 * Function:
 *      _bcm_gh2_rcpu_switch_vlan_tpid_sig_get
 * Description:
 *      Helper function to get VLAN, TPID ot Signature for RCPU
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to get
 *  arg - The value returned
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_gh2_rcpu_switch_vlan_tpid_sig_get(int unit,
                                   bcm_switch_control_t type,
                                   int *arg)
{
    soc_reg_t   reg;
    soc_field_t field;
    uint32  raddr, rval;


    if (!soc_feature(unit, soc_feature_rcpu_1)) {
        return BCM_E_UNAVAIL;
    }

    switch (type) {
        case bcmSwitchRemoteCpuVlan:
            reg = CMIC_PKT_VLANr;
            field = VLAN_IDf;
            break;
        case bcmSwitchRemoteCpuTpid:
            reg = CMIC_PKT_VLANr;
            field = TPIDf;
            break;
        case bcmSwitchRemoteCpuSignature:
            reg = CMIC_PKT_ETHER_SIGr;
            field = SIGNATUREf;
            break;
        case bcmSwitchRemoteCpuEthertype:
            reg = CMIC_PKT_ETHER_SIGr;
            field = ETHERTYPEf;
            break;
        default:
            return BCM_E_PARAM;
    }

    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(
        soc_pci_getreg(unit, raddr, &rval));

    *arg = soc_reg_field_get(unit, reg, rval, field);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_gh2_rcpu_pipe_bypass_header_set
 * Description:
 *      Prepare and program the SOBMH header for RCPU usage
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - port number to specify in the SOBMH
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_gh2_rcpu_pipe_bypass_header_set(int unit,
                            bcm_switch_control_t type,
                            int arg)
{
    uint32          pbh[3];     /* 3 words SOBMH header to program */
    soc_pbsmh_hdr_t *p_pbh = (soc_pbsmh_hdr_t *)pbh;
    bcm_port_t      port;
    bcm_module_t    modid;
    uint32          raddr;
    int qnum;

    if (BCM_GPORT_IS_SET(arg)) {
        bcm_trunk_t tgid;
        int         id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, arg, &modid, &port, &tgid, &id));
        if ((BCM_TRUNK_INVALID != tgid) || (-1 != id) ) {
            return BCM_E_PORT;
        }
    } else {
        port = arg;
        modid = SOC_DEFAULT_DMA_SRCMOD_GET(unit);
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    /* Setup SOBMH header according to supplied argument */
    if (SOC_DCB_TYPE(unit) == 23 || SOC_DCB_TYPE(unit) == 26 ||
        SOC_DCB_TYPE(unit) == 29 || SOC_DCB_TYPE(unit) == 31 ||
        SOC_DCB_TYPE(unit) == 30 || SOC_DCB_TYPE(unit) == 34 ||
        SOC_DCB_TYPE(unit) == 37) {
        {
            qnum = SOC_INFO(unit).port_uc_cosq_base[port];
        }
        PBS_MH_V7_W0_START_SET(pbh);
        PBS_MH_V7_W1_DPORT_QNUM_SET(pbh, port, qnum);
        PBS_MH_V7_W2_SMOD_COS_QNUM_SET(pbh, modid, 1, 0, qnum, 0);

    } else if (SOC_DCB_TYPE(unit) == 32) {
            PBS_MH_V9_W0_START_SET(pbh);
            PBS_MH_V9_W1_DPORT_SET(pbh, port);
            PBS_MH_V9_W2_SMOD_COS_SET(pbh, modid, 1, 0);
    } else if (SOC_DCB_TYPE(unit) == 33 || SOC_DCB_TYPE(unit) == 35) {
        qnum = SOC_INFO(unit).port_uc_cosq_base[port];
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_start, SOC_PBSMH_START_INTERNAL);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_header_type, SOC_SOBMH_FROM_CPU);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_dst_port, port);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_unicast, 1 );
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_dst_port, port);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_src_mod, modid);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_cos, 0);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_queue_num, qnum);
        soc_pbsmh_field_set(unit, p_pbh, PBSMH_pri, 0);
    } else {
        PBS_MH_W0_START_SET(pbh);
        PBS_MH_W1_RSVD_SET(pbh);
        if ((SOC_DCB_TYPE(unit) == 11) || (SOC_DCB_TYPE(unit) == 12) ||
            (SOC_DCB_TYPE(unit) == 15) || (SOC_DCB_TYPE(unit) == 17) ||
            (SOC_DCB_TYPE(unit) == 18)) {
            PBS_MH_V2_W2_SMOD_DPORT_COS_SET(pbh, modid, port, 0, 0, 0);
        } else if ((SOC_DCB_TYPE(unit) == 14) ||
                   (SOC_DCB_TYPE(unit) == 19) ||
                   (SOC_DCB_TYPE(unit) == 20)) {

            PBS_MH_V3_W2_SMOD_DPORT_COS_SET(pbh, modid, port,0, 0, 0);
        } else if (SOC_DCB_TYPE(unit) == 16) {
            PBS_MH_V4_W2_SMOD_DPORT_COS_SET(pbh, modid, port,0, 0, 0);
        } else if (SOC_DCB_TYPE(unit) == 21) {
            PBS_MH_V5_W1_SMOD_SET(pbh, modid, 1, 0, 0);
            PBS_MH_V5_W2_DPORT_COS_SET(pbh, port, 0, 0);
        } else if (SOC_DCB_TYPE(unit) == 24) {
            PBS_MH_V5_W1_SMOD_SET(pbh, modid, 1, 0, 0);
            PBS_MH_V6_W2_DPORT_COS_QNUM_SET(pbh, port, 0,
                             (SOC_INFO(unit).port_cosq_base[port]), 0);
        } else {
            PBS_MH_V1_W2_SMOD_DPORT_COS_SET(pbh, modid, port, 0, 0, 0);
        }
    }

    raddr = soc_reg_addr(unit, CMIC_PKT_RMH0r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, pbh[0]));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH1r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, pbh[1]));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH2r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, pbh[2]));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_rcpu_pipe_bypass_header_get
 * Description:
 *      Get the SOBMH header for RCPU usage and retrieve the port numebr
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - port number in the SOBMH header
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_gh2_rcpu_pipe_bypass_header_get(int unit,
                            bcm_switch_control_t type,
                            int *arg)
{
    soc_pbsmh_hdr_t *pbh;
    uint32          hw_buff[3];     /* 3 words SOBMH header */
    uint32          raddr;
    int             isGport;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    raddr = soc_reg_addr(unit, CMIC_PKT_RMH0r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hw_buff[0])));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH1r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hw_buff[1])));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH2r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hw_buff[2])));

    pbh = (soc_pbsmh_hdr_t *)hw_buff;

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

    if (isGport) {
        _bcm_gport_dest_t gport_st;

        gport_st.gport_type = BCM_GPORT_TYPE_MODPORT;
        gport_st.modid = soc_pbsmh_field_get(unit, pbh, PBSMH_src_mod);
        gport_st.port = soc_pbsmh_field_get(unit, pbh, PBSMH_dst_port);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_construct(unit, &gport_st, arg));
    } else {
        *arg = soc_pbsmh_field_get(unit, pbh, PBSMH_dst_port);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_gh2_rcpu_higig_header_set
 * Description:
 *      Prepare and program the HIGIG header for RCPU usage
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      arg         - port number to specify in the SOBMH
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_gh2_rcpu_higig_header_set(int unit, int arg)
{
#if defined(BCM_HIGIG2_SUPPORT)
    uint32           hghdr[4];     /* 4 words HIGIG2 header to program */
    soc_higig2_hdr_t *xgh = (soc_higig2_hdr_t *)hghdr;
    bcm_port_t      port;
    bcm_module_t    modid;
    uint32          raddr, rval, vlan_val;

    if (BCM_GPORT_IS_SET(arg)) {
        bcm_trunk_t tgid;
        int         id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, arg, &modid, &port, &tgid, &id));
        if ((BCM_TRUNK_INVALID != tgid) || (-1 != id) ) {
            return BCM_E_PORT;
        }
    } else {
        port = arg;
        modid = SOC_DEFAULT_DMA_SRCMOD_GET(unit);
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    sal_memset(hghdr, 0, sizeof(soc_higig2_hdr_t));
    soc_higig2_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
    soc_higig2_field_set(unit, xgh, HG_ppd_type, 0);
    soc_higig2_field_set(unit, xgh, HG_dst_mod, modid);
    soc_higig2_field_set(unit, xgh, HG_dst_port, port);

    raddr = soc_reg_addr(unit, CMIC_PKT_VLANr, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    BCM_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
    vlan_val = soc_reg_field_get(unit, CMIC_PKT_VLANr, rval, VLAN_IDf);
    if (0 == vlan_val) {
        vlan_val = DEFAULT_RCPU_VLAN;
    }
    soc_higig2_field_set(unit, xgh, HG_vlan_tag, vlan_val);
    soc_higig2_field_set(unit, xgh, HG_opcode, SOC_HIGIG_OP_CPU);

    raddr = soc_reg_addr(unit, CMIC_PKT_RMH0r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, hghdr[0]));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH1r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, hghdr[1]));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH2r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, hghdr[2]));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH3r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, hghdr[3]));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_HIGIG2_SUPPORT */
}

/*
 * Function:
 *      _bcm_gh2_rcpu_higig_header_get
 * Description:
 *      Get the SOBMH header for RCPU usage and retrieve the port numebr
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      type        - The required switch control type to get MAC for
 *      arg         - port number in the SOBMH header
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_gh2_rcpu_higig_header_get(int unit, int *arg)
{
#if defined(BCM_HIGIG2_SUPPORT)
    uint32          hghdr[4];     /* 4 words HIGIG2 header to program */
    soc_higig2_hdr_t *xgh = (soc_higig2_hdr_t *)hghdr;
    uint32          raddr;
    int             isGport;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    raddr = soc_reg_addr(unit, CMIC_PKT_RMH0r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hghdr[0])));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH1r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hghdr[1])));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH2r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hghdr[2])));
    raddr = soc_reg_addr(unit, CMIC_PKT_RMH3r, REG_PORT_ANY, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &(hghdr[3])));

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

    if (isGport) {
        _bcm_gport_dest_t gport_st;

        gport_st.gport_type = BCM_GPORT_TYPE_MODPORT;
        gport_st.modid = soc_higig2_field_get(unit, xgh, HG_dst_mod);
        gport_st.port = soc_higig2_field_get(unit, xgh, HG_dst_port);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_construct(unit, &gport_st, arg));
    } else {
        *arg = soc_higig2_field_get(unit, xgh, HG_dst_port);
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}
#endif


STATIC int
_bcm_gh2_switch_sync_port_select_set(int unit, uint32 val)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(
        READ_TOP_MISC_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                      L1_RCVD_PRI_CLK_PORT_SELf, val);
    BCM_IF_ERROR_RETURN(
        WRITE_TOP_MISC_CONTROL_2r(unit, rval));

    return BCM_E_NONE;
}


STATIC int
_bcm_gh2_switch_sync_port_select_get(int unit, uint8 type, uint32 *val)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(
        READ_TOP_MISC_CONTROL_2r(unit, &rval));
    if (type) {
        /* primary (1) */
        *val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                 L1_RCVD_PRI_CLK_PORT_SELf);
    } else {
        /* backup (0) */
        *val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                 L1_RCVD_BKUP_CLK_PORT_SELf);
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_gh2_switch_sync_port_backup_select_set(int unit, uint32 val)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(
        READ_TOP_MISC_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                      L1_RCVD_BKUP_CLK_PORT_SELf, val);
    BCM_IF_ERROR_RETURN(
        WRITE_TOP_MISC_CONTROL_2r(unit, rval));
    return BCM_E_NONE;
}

STATIC int
 _bcm_gh2_switch_div_ctrl_select_set(int unit, uint32 val)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(
        READ_TOP_L1_RCVD_CLK_CONTROLr(unit, &rval));
    if (val == 1) {
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CONTROLr, &rval,
                          L1_RCVD_FREQ_SELf, 1);
    } else {
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CONTROLr, &rval,
                          L1_RCVD_FREQ_SELf, 0);
    }
    BCM_IF_ERROR_RETURN(
        WRITE_TOP_L1_RCVD_CLK_CONTROLr(unit, rval));
    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_switch_div_ctrl_select_get(int unit, uint8 type, uint32 *val)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(
        READ_TOP_L1_RCVD_CLK_CONTROLr(unit, &rval));
    if (type) {
        /* primary (1) */
        *val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CONTROLr, rval,
                                 L1_RCVD_FREQ_SELf);
    } else {
        /* backup (0) */
        *val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CONTROLr, rval,
                                 L1_RCVD_BKUP_FREQ_SELf);
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_gh2_switch_div_ctrl_backup_select_set(int unit, uint32 val)
{

    uint32 rval;
    BCM_IF_ERROR_RETURN(
        READ_TOP_L1_RCVD_CLK_CONTROLr(unit, &rval));
    if (val == 1) {
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CONTROLr, &rval,
                          L1_RCVD_BKUP_FREQ_SELf, 1);
    } else {
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CONTROLr, &rval,
                          L1_RCVD_BKUP_FREQ_SELf, 0);
    }
    BCM_IF_ERROR_RETURN(
        WRITE_TOP_L1_RCVD_CLK_CONTROLr(unit, rval));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_gh2_switch_control_port_set
 * Description:
 *      Specify general switch behaviors on a per-port basis.
 * Parameters:
 *      unit - Device unit number
 *      port - Port to affect
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_gh2_switch_control_port_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg)
{
    if ((type == bcmSwitchFailoverStackTrunk) ||
        (type == bcmSwitchFailoverEtherTrunk)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (bcm_gh2_switch_control_port_validate(unit, port, type, &port));
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int found;

        switch(type) {
            case bcmSwitchPktAge:
                return _bcm_gh2_pkt_age_set(unit, arg);
                break;
            case bcmSwitchHashL2:
            case bcmSwitchHashL3:
            case bcmSwitchHashMultipath:
            case bcmSwitchHashIpfixIngress:
            case bcmSwitchHashIpfixEgress:
            case bcmSwitchHashIpfixIngressDual:
            case bcmSwitchHashIpfixEgressDual:
            case bcmSwitchHashVlanTranslate:
            case bcmSwitchHashMPLS:
            case bcmSwitchHashWlanPort:
            case bcmSwitchHashWlanPortDual:
            case bcmSwitchHashWlanClient:
            case bcmSwitchHashWlanClientDual:
            case bcmSwitchHashRegexAction:
            case bcmSwitchHashRegexActionDual:
            case bcmSwitchHashL3DNATPool:
            case bcmSwitchHashL3DNATPoolDual:
            case bcmSwitchHashVpVlanMemberIngress:
            case bcmSwitchHashVpVlanMemberIngressDual:
            case bcmSwitchHashVpVlanMemberEgress:
            case bcmSwitchHashVpVlanMemberEgressDual:
            case bcmSwitchHashL2Endpoint:
            case bcmSwitchHashL2EndpointDual:
            case bcmSwitchHashEndpointQueueMap:
            case bcmSwitchHashEndpointQueueMapDual:
                return _bcm_gh2_er_hashselect_set(unit, type, arg);
                break;
            case bcmSwitchHashControl:
                return _bcm_gh2_hashcontrol_set(unit, arg);
                break;
            case bcmSwitchFieldMultipathHashSelect:
                return _bcm_field_tr_multipath_hashcontrol_set(unit, arg);
                break;
            case bcmSwitchL3IngressMode:
#ifdef INCLUDE_L3
                if ((soc_feature(unit, soc_feature_l3)) && (soc_feature(unit,soc_feature_l3_ingress_interface))) {
                     return bcm_xgs3_l3_ingress_mode_set(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3IngressInterfaceMapSet:
#ifdef INCLUDE_L3
                if ((soc_feature(unit, soc_feature_l3)) && (soc_feature(unit,soc_feature_l3_ingress_interface))) {
                     return bcm_xgs3_l3_ingress_intf_map_set(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;

            case bcmSwitchColorSelect:
                return _bcm_gh2_er_color_mode_set(unit, port, arg);
                break;
            case bcmSwitchModuleLoopback:
                return _bcm_gh2_mod_lb_set(unit, port, arg);
                break;
            case bcmSwitchIngressRateLimitIncludeIFG:
                return _bcm_gh2_ing_rate_limit_ifg_set(unit, port, arg);
                break;
            case bcmSwitchVrfMax:
                if (soc_feature(unit, soc_feature_l3)) {
                    return soc_max_vrf_set(unit, arg);
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;

            case bcmSwitchL3UrpfRouteEnable:
                if (soc_feature(unit, soc_feature_urpf)) {
                    /*
                     * Saber - 56246 has only 2 L3_DEFIP tcams
                     * Both IPV6 and RPF cannot be supported
                     * on this device.
                     */
                    if (soc_feature(unit, soc_feature_l3_max_2_defip_tcams)) {
                        if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 0)) {
                            LOG_ERROR(BSL_LS_BCM_COMMON,
                                      (BSL_META_U(unit,
                                       "Cannot enable IPV6 and URPF "
                                       "only 2 L3_DEFIP tcams present\n")));
                            return BCM_E_CONFIG;
                        }
                    }
                    return _bcm_gh2_urpf_route_enable(unit, arg);
                }
                return BCM_E_UNAVAIL;
                break;

            case bcmSwitchL3UrpfMode:
                if (soc_feature(unit, soc_feature_urpf)) {
                    return _bcm_gh2_urpf_port_mode_set(unit, port, arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3UrpfDefaultRoute:
                if (soc_feature(unit, soc_feature_urpf)) {
                    return _bcm_gh2_urpf_def_gw_enable(unit, port, arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashL2Dual:
            case bcmSwitchHashL3Dual:
            case bcmSwitchHashVlanTranslateDual:
            case bcmSwitchHashEgressVlanTranslate:
            case bcmSwitchHashEgressVlanTranslateDual:
            case bcmSwitchHashMPLSDual:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    return _bcm_gh2_er_hashselect_set(unit, type, arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashSelectControl:
                return bcm_gh2_switch_rtag7_selectcontrol_set(unit, arg);
                break;
            case bcmSwitchHashIP4Field0:
            case bcmSwitchHashIP4Field1:
            case bcmSwitchHashIP6Field0:
            case bcmSwitchHashIP6Field1:
            case bcmSwitchHashL2Field0:
            case bcmSwitchHashL2Field1:
            case bcmSwitchHashHG2UnknownField0:
            case bcmSwitchHashHG2UnknownField1:
                return bcm_gh2_switch_rtag7_fieldselect_set(unit, type, arg);
                break;
            case bcmSwitchHashMPLSField0:
            case bcmSwitchHashMPLSField1:
            case bcmSwitchHashL2MPLSField0:
            case bcmSwitchHashL2MPLSField1:
            case bcmSwitchHashL3MPLSField0:
            case bcmSwitchHashL3MPLSField1:
            case bcmSwitchHashMPLSTunnelField0:
            case bcmSwitchHashMPLSTunnelField1:
            case bcmSwitchHashMIMTunnelField0:
            case bcmSwitchHashMIMTunnelField1:
            case bcmSwitchHashMIMField0:
            case bcmSwitchHashMIMField1:
            case bcmSwitchHashL3MiMField0:
            case bcmSwitchHashL3MiMField1:
            case bcmSwitchHashL3L2MPLSField0:
            case bcmSwitchHashL3L2MPLSField1:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashL2VxlanField0:
            case bcmSwitchHashL2VxlanField1:
            case bcmSwitchHashL3VxlanField0:
            case bcmSwitchHashL3VxlanField1:
                return bcm_gh2_switch_rtag7_fieldselect_set(unit, type, arg);
                break;
            case bcmSwitchHashL2L2GreField0:
            case bcmSwitchHashL2L2GreField1:
            case bcmSwitchHashL3L2GreField0:
            case bcmSwitchHashL3L2GreField1:
            case bcmSwitchHashFCOEField0:
            case bcmSwitchHashFCOEField1:
            case bcmSwitchHashL2TrillField0:
            case bcmSwitchHashL2TrillField1:
            case bcmSwitchHashL3TrillField0:
            case bcmSwitchHashL3TrillField1:
            case bcmSwitchHashTrillTunnelField0:
            case bcmSwitchHashTrillTunnelField1:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashIP4TcpUdpField0:
            case bcmSwitchHashIP4TcpUdpField1:
            case bcmSwitchHashIP4TcpUdpPortsEqualField0:
            case bcmSwitchHashIP4TcpUdpPortsEqualField1:
            case bcmSwitchHashIP6TcpUdpField0:
            case bcmSwitchHashIP6TcpUdpField1:
            case bcmSwitchHashIP6TcpUdpPortsEqualField0:
            case bcmSwitchHashIP6TcpUdpPortsEqualField1:
                return bcm_gh2_switch_rtag7_fieldselect_set(unit, type, arg);
            case bcmSwitchHashField0PreProcessEnable:
                return (soc_reg_field32_modify(unit, RTAG7_HASH_CONTROL_3r,
                            REG_PORT_ANY, HASH_PRE_PROCESSING_ENABLE_Af, arg));
            case bcmSwitchHashField1PreProcessEnable:
                return (soc_reg_field32_modify(unit, RTAG7_HASH_CONTROL_3r,
                            REG_PORT_ANY, HASH_PRE_PROCESSING_ENABLE_Bf, arg));
            case bcmSwitchHashField0Config:
            case bcmSwitchHashField0Config1:
            case bcmSwitchHashField1Config:
            case bcmSwitchHashField1Config1:
            case bcmSwitchMacroFlowHashFieldConfig:
                return bcm_gh2_switch_rtag7_fieldconfig_set(unit, type, arg);
                break;
            case bcmSwitchTrunkHashSet1UnicastOffset:
            case bcmSwitchTrunkHashSet1NonUnicastOffset:
            case bcmSwitchFabricTrunkHashSet1UnicastOffset:
            case bcmSwitchFabricTrunkHashSet1NonUnicastOffset:
            case bcmSwitchLoadBalanceHashSet1UnicastOffset:
            case bcmSwitchLoadBalanceHashSet1NonUnicastOffset:
            case bcmSwitchECMPHashSet1Offset:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchTrunkHashSet0UnicastOffset:
            case bcmSwitchTrunkHashSet0NonUnicastOffset:
            case bcmSwitchECMPHashSet0Offset:
            case bcmSwitchECMPVxlanHashOffset:
            case bcmSwitchEntropyHashSet0Offset:
            case bcmSwitchFabricTrunkHashSet0UnicastOffset:
            case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
            case bcmSwitchLoadBalanceHashSet0UnicastOffset:
            case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
            case bcmSwitchFabricTrunkFailoverHashOffset:
                return bcm_gh2_switch_rtag7_fieldoffset_set(unit, port, type,
                                                            arg);
                break;
            case bcmSwitchEcmpRoce2HashOffset:
                if (soc_feature(unit, soc_feature_rroce)) {
                    return bcm_gh2_switch_rtag7_fieldoffset_set(unit, port,
                                                                type, arg);
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmSwitchTrunkFailoverHashOffset:
            case bcmSwitchFabricTrunkDynamicHashOffset:
            case bcmSwitchTrunkDynamicHashOffset:
            case bcmSwitchEcmpDynamicHashOffset:
            case bcmSwitchFabricTrunkResilientHashOffset:
            case bcmSwitchTrunkResilientHashOffset:
            case bcmSwitchEcmpResilientHashOffset:
            case bcmSwitchECMPUnderlayHashSet0Offset:
            case bcmSwitchECMPUnderlayVxlanHashOffset:
            case bcmSwitchECMPL2GreHashOffset:
            case bcmSwitchECMPUnderlayL2GreHashOffset:
            case bcmSwitchECMPTrillHashOffset:
            case bcmSwitchECMPUnderlayTrillHashOffset:
            case bcmSwitchECMPMplsHashOffset:
            case bcmSwitchVirtualPortDynamicHashOffset:
            case bcmSwitchECMPOverlayHashOffset:
            case bcmSwitchVirtualPortUnderlayDynamicHashOffset:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchArpReplyToCpu:
            case bcmSwitchArpReplyDrop:
            case bcmSwitchArpRequestToCpu:
            case bcmSwitchArpRequestDrop:
            case bcmSwitchNdPktToCpu:
            case bcmSwitchNdPktDrop:
            case bcmSwitchDhcpPktToCpu:
            case bcmSwitchDhcpPktDrop:
                if (SOC_REG_INFO(unit, PROTOCOL_PKT_CONTROLr).regtype !=
                    soc_portreg) {
                    return _bcm_gh2_prot_pkt_action_set(unit, port, type, arg);
                }
                break;
            case bcmSwitchIgmpPktToCpu:
            case bcmSwitchIgmpPktDrop:
            case bcmSwitchMldPktToCpu:
            case bcmSwitchMldPktDrop:
            case bcmSwitchV4ResvdMcPktToCpu:
            case bcmSwitchV4ResvdMcPktFlood:
            case bcmSwitchV4ResvdMcPktDrop:
            case bcmSwitchV6ResvdMcPktToCpu:
            case bcmSwitchV6ResvdMcPktFlood:
            case bcmSwitchV6ResvdMcPktDrop:
            case bcmSwitchIgmpReportLeaveDrop:
            case bcmSwitchIgmpReportLeaveFlood:
            case bcmSwitchIgmpReportLeaveToCpu:
            case bcmSwitchIgmpQueryDrop:
            case bcmSwitchIgmpQueryFlood:
            case bcmSwitchIgmpQueryToCpu:
            case bcmSwitchIgmpUnknownDrop:
            case bcmSwitchIgmpUnknownFlood:
            case bcmSwitchIgmpUnknownToCpu:
            case bcmSwitchMldReportDoneDrop:
            case bcmSwitchMldReportDoneFlood:
            case bcmSwitchMldReportDoneToCpu:
            case bcmSwitchMldQueryDrop:
            case bcmSwitchMldQueryFlood:
            case bcmSwitchMldQueryToCpu:
            case bcmSwitchIpmcV4RouterDiscoveryDrop:
            case bcmSwitchIpmcV4RouterDiscoveryFlood:
            case bcmSwitchIpmcV4RouterDiscoveryToCpu:
            case bcmSwitchIpmcV6RouterDiscoveryDrop:
            case bcmSwitchIpmcV6RouterDiscoveryFlood:
            case bcmSwitchIpmcV6RouterDiscoveryToCpu:
                return _bcm_gh2_igmp_action_set(unit, port, type, arg);
                break;
            case bcmSwitchDirectedMirroring:
                if (!soc_feature(unit, soc_feature_xgs1_mirror)) {
                    /* Directed mirroring cannot be turned off */
                    return (arg == 0) ? BCM_E_PARAM : BCM_E_NONE;
                }
                break;
            case bcmSwitchFlexibleMirrorDestinations:
            case bcmSwitchMirrorExclusive:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3EgressMode:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_l3)) {
                    return bcm_xgs3_l3_egress_mode_set(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
             case bcmSwitchL3HostAsRouteReturnValue:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_l3)) {
                     return bcm_xgs3_l3_host_as_route_return_set(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3DefipMultipathCountUpdate:
#ifdef INCLUDE_L3
                if(soc_feature(unit, soc_feature_l3_defip_ecmp_count)) {
                    return _bcm_xgs3_l3_multipathCountUpdate(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchIpmcReplicationSharing:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                    SOC_IPMCREPLSHR_SET(unit, arg);
#ifdef BCM_WARM_BOOT_SUPPORT
                    {
                        int rv;
                        /* Record this value in HW for Warm Boot recovery. */
                        rv = _bcm_esw_ipmc_repl_wb_flags_set(
                                unit, (arg ? _BCM_IPMC_WB_REPL_LIST : 0),
                                _BCM_IPMC_WB_REPL_LIST);
                        if (BCM_FAILURE(rv) && (BCM_E_UNAVAIL != rv)) {
                            return rv;
                        }
                    }
#endif /* BCM_WARM_BOOT_SUPPORT */
                    return BCM_E_NONE;
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchIpmcReplicationAvailabilityThreshold:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                    return _bcm_esw_ipmc_repl_threshold_set(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepth:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    if (arg >= 0) {
                        SOC_DUAL_HASH_MOVE_MAX(unit) = arg;
                        return BCM_E_NONE;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthL2:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    if (arg >= 0) {
                        SOC_DUAL_HASH_MOVE_MAX_L2X(unit) = arg;
                        return BCM_E_NONE;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthMpls:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    if (arg >= 0) {
                        SOC_DUAL_HASH_MOVE_MAX_MPLS(unit) = arg;
                        return BCM_E_NONE;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthVlan:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    if (arg >= 0) {
                        SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) = arg;
                        return BCM_E_NONE;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthEgressVlan:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    if (arg >= 0) {
                        SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit) = arg;
                        return BCM_E_NONE;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                return BCM_E_UNAVAIL;
                break;
#if defined(INCLUDE_L3)
            case bcmSwitchHashDualMoveDepthL3:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    if (arg >= 0) {
                        SOC_DUAL_HASH_MOVE_MAX_L3X(unit) = arg;
                        return BCM_E_NONE;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                return BCM_E_UNAVAIL;
                break;
#endif
            case bcmSwitchHashDualMoveDepthWlanPort:
            case bcmSwitchHashDualMoveDepthWlanClient:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL2PortBlocking:
                if (soc_feature(unit, soc_feature_src_mac_group)) {
                    SOC_L2X_GROUP_ENABLE_SET(unit, _bool_invert(unit, arg, 0));
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchTimeSyncPktDrop:
            case bcmSwitchTimeSyncPktFlood:
            case bcmSwitchTimeSyncPktToCpu:
            case bcmSwitchMmrpPktDrop:
            case bcmSwitchMmrpPktFlood:
            case bcmSwitchMmrpPktToCpu:
            case bcmSwitchSrpPktDrop:
            case bcmSwitchSrpPktFlood:
            case bcmSwitchSrpPktToCpu:
            case bcmSwitchSRPEthertype:
            case bcmSwitchMMRPEthertype:
            case bcmSwitchTimeSyncEthertype:
            case bcmSwitchSRPDestMacOui:
            case bcmSwitchMMRPDestMacOui:
            case bcmSwitchTimeSyncDestMacOui:
            case bcmSwitchSRPDestMacNonOui:
            case bcmSwitchMMRPDestMacNonOui:
            case bcmSwitchTimeSyncDestMacNonOui:
            case bcmSwitchTimeSyncMessageTypeBitmap:
            case bcmSwitchTimeSyncClassAPktPrio:
            case bcmSwitchTimeSyncClassBPktPrio:
            case bcmSwitchTimeSyncClassAExeptionPktPrio:
            case bcmSwitchTimeSyncClassBExeptionPktPrio:
            case bcmSwitchStgInvalidToCpu:
            case bcmSwitchVlanTranslateEgressMissToCpu:
            case bcmSwitchL3PktErrToCpu:
            case bcmSwitchMtuFailureToCpu:
            case bcmSwitchSrcKnockoutToCpu:
            case bcmSwitchWlanTunnelMismatchToCpu:
            case bcmSwitchWlanTunnelMismatchDrop:
            case bcmSwitchWlanPortMissToCpu:
            case bcmSwitchUnknownVlanToCpuCosq:
            case bcmSwitchStgInvalidToCpuCosq:
            case bcmSwitchVlanTranslateEgressMissToCpuCosq:
            case bcmSwitchTunnelErrToCpuCosq:
            case bcmSwitchL3HeaderErrToCpuCosq:
            case bcmSwitchL3PktErrToCpuCosq:
            case bcmSwitchIpmcTtlErrToCpuCosq:
            case bcmSwitchMtuFailureToCpuCosq:
            case bcmSwitchHgHdrErrToCpuCosq:
            case bcmSwitchSrcKnockoutToCpuCosq:
            case bcmSwitchWlanTunnelMismatchToCpuCosq:
            case bcmSwitchWlanPortMissToCpuCosq:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchLayeredQoSResolution:
                return _bcm_gh2_layered_qos_resolution_set(unit, port, type, arg);
                break;
            case bcmSwitchEncapErrorToCpu:
            case bcmSwitchMirrorEgressTrueColorSelect:
            case bcmSwitchMirrorEgressTruePriority:
            case bcmSwitchL3UrpfRouteEnableExternal:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchLinkDownInfoSkip:
                return _bcm_esw_link_port_info_skip_set(unit, port, arg);
                break;
            case bcmSwitchDropSobmhOnLinkDown:
            case bcmSwitchCongestionNotificationIdLow:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchMeterAdjust:
                if (soc_feature(unit, soc_feature_meter_adjust)) {
                    return _bcm_gh2_meter_adjust_set(unit, port, arg);
                }
                break;
            case bcmSwitchShaperAdjust:
                if (soc_feature(unit, soc_feature_meter_adjust)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_gh2_shaper_adjust_set(unit, port, arg));
                }
                break;
            case bcmSwitchPFCClass0Queue:
            case bcmSwitchPFCClass1Queue:
            case bcmSwitchPFCClass2Queue:
            case bcmSwitchPFCClass3Queue:
            case bcmSwitchPFCClass4Queue:
            case bcmSwitchPFCClass5Queue:
            case bcmSwitchPFCClass6Queue:
            case bcmSwitchPFCClass7Queue:
                if (arg < 0) {
                    return bcmi_gh2_cosq_port_pfc_op(unit, port, type,
                                                     _BCM_COSQ_OP_CLEAR, arg);
                } else {
                    return bcmi_gh2_cosq_port_pfc_op(unit, port, type,
                                                     _BCM_COSQ_OP_ADD, arg);
                }
               break;
            case bcmSwitchPFCClass0McastQueue:
            case bcmSwitchPFCClass1McastQueue:
            case bcmSwitchPFCClass2McastQueue:
            case bcmSwitchPFCClass3McastQueue:
            case bcmSwitchPFCClass4McastQueue:
            case bcmSwitchPFCClass5McastQueue:
            case bcmSwitchPFCClass6McastQueue:
            case bcmSwitchPFCClass7McastQueue:
            case bcmSwitchPFCClass0DestmodQueue:
            case bcmSwitchPFCClass1DestmodQueue:
            case bcmSwitchPFCClass2DestmodQueue:
            case bcmSwitchPFCClass3DestmodQueue:
            case bcmSwitchPFCClass4DestmodQueue:
            case bcmSwitchPFCClass5DestmodQueue:
            case bcmSwitchPFCClass6DestmodQueue:
            case bcmSwitchPFCClass7DestmodQueue:
            case bcmSwitchEntropyHashSet1Offset:
            case bcmSwitchMiMDefaultSVP:
            case bcmSwitchAlternateStoreForward:
            case bcmSwitchMcQueueSchedMode:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchTimesyncEgressTimestampingMode:
                if (soc_feature(unit, soc_feature_timesync_support) &&
                    soc_feature(unit, soc_feature_timesync_timestampingmode)) {
                    return _bcm_esw_port_timesync_timestamping_mode_set(unit, port,
                                        (bcm_switch_timesync_timestamping_mode_t)arg);
                }
                break;
            case bcmSwitchHashRoce1Field0:
            case bcmSwitchHashRoce1Field1:
            case bcmSwitchHashRoce2IP4Field0:
            case bcmSwitchHashRoce2IP4Field1:
            case bcmSwitchHashRoce2IP6Field0:
            case bcmSwitchHashRoce2IP6Field1:
                if (soc_feature(unit, soc_feature_rroce)) {
                    return bcm_gh2_switch_rtag7_fieldselect_set(unit,
                                                                type, arg);
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;

            default:
                break;
        }

        found = 0;              /* True if one or more matches found */
        BCM_IF_ERROR_RETURN(
            bcm_gh2_switch_control_port_binding_set(unit, port,
            type, arg, &found));
        return (found ? BCM_E_NONE : BCM_E_UNAVAIL);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}



/*
 * Function:
 *      bcm_gh2_switch_control_port_get
 * Description:
 *      Retrieve general switch behaviors on a per-port basis
 * Parameters:
 *      unit - Device unit number
 *      port - Port to check
 *      type - The desired configuration parameter to retrieve
 *      arg - Pointer to where the retrieved value will be written
 * Returns:
 *      BCM_E_xxx
 */

int
bcm_gh2_switch_control_port_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg)
{

    if ((type == bcmSwitchFailoverStackTrunk) ||
        (type == bcmSwitchFailoverEtherTrunk)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (bcm_gh2_switch_control_port_validate(unit, port, type, &port));
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int found;
        switch(type) {
            case bcmSwitchPktAge:
                return _bcm_gh2_pkt_age_get(unit, arg);
                break;
            case bcmSwitchHashL2:
            case bcmSwitchHashL3:
            case bcmSwitchHashMultipath:
            case bcmSwitchHashIpfixIngress:
            case bcmSwitchHashIpfixEgress:
            case bcmSwitchHashIpfixIngressDual:
            case bcmSwitchHashIpfixEgressDual:
            case bcmSwitchHashVlanTranslate:
            case bcmSwitchHashMPLS:
            case bcmSwitchHashWlanPort:
            case bcmSwitchHashWlanPortDual:
            case bcmSwitchHashWlanClient:
            case bcmSwitchHashWlanClientDual:
            case bcmSwitchHashRegexAction:
            case bcmSwitchHashRegexActionDual:
            case bcmSwitchHashL3DNATPool:
            case bcmSwitchHashL3DNATPoolDual:
            case bcmSwitchHashVpVlanMemberIngress:
            case bcmSwitchHashVpVlanMemberIngressDual:
            case bcmSwitchHashVpVlanMemberEgress:
            case bcmSwitchHashVpVlanMemberEgressDual:
            case bcmSwitchHashL2Endpoint:
            case bcmSwitchHashL2EndpointDual:
            case bcmSwitchHashEndpointQueueMap:
            case bcmSwitchHashEndpointQueueMapDual:
                return _bcm_gh2_er_hashselect_get(unit, type, arg);
                break;
            case bcmSwitchHashControl:
                return _bcm_gh2_hashcontrol_get(unit, arg);
                break;
            case bcmSwitchFieldMultipathHashSelect:
                return _bcm_field_tr_multipath_hashcontrol_get(unit, arg);
                break;

            case bcmSwitchL3IngressMode:
#ifdef INCLUDE_L3
                if ((soc_feature(unit, soc_feature_l3)) &&
                    (soc_feature(unit,soc_feature_l3_ingress_interface))) {
                    return bcm_xgs3_l3_ingress_mode_get(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3IngressInterfaceMapSet:
#ifdef INCLUDE_L3
                if ((soc_feature(unit, soc_feature_l3)) &&
                    (soc_feature(unit,soc_feature_l3_ingress_interface))) {
                    return bcm_xgs3_l3_ingress_intf_map_get(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchColorSelect:
                return _bcm_gh2_er_color_mode_get(unit, port, arg);
                break;
            case bcmSwitchModuleLoopback:
                return _bcm_gh2_mod_lb_get(unit, port, arg);
                break;
            case bcmSwitchIngressRateLimitIncludeIFG:
                return _bcm_gh2_ing_rate_limit_ifg_get(unit, port, arg);
                break;
            case bcmSwitchVrfMax:
                if (soc_feature(unit, soc_feature_l3)) {
                    *arg = SOC_VRF_MAX(unit);
                    return (BCM_E_NONE);
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmSwitchL3UrpfMode:
                if (soc_feature(unit, soc_feature_urpf)) {
                    return _bcm_esw_port_config_get(unit, port,
                                                    _bcmPortL3UrpfMode, arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3UrpfDefaultRoute:
                if (soc_feature(unit, soc_feature_urpf)) {
                    return _bcm_esw_port_config_get(unit, port,
                                                    _bcmPortL3UrpfDefaultRoute, arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashL2Dual:
            case bcmSwitchHashL3Dual:
            case bcmSwitchHashVlanTranslateDual:
            case bcmSwitchHashEgressVlanTranslate:
            case bcmSwitchHashEgressVlanTranslateDual:
            case bcmSwitchHashMPLSDual:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    return _bcm_gh2_er_hashselect_get(unit, type, arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashSelectControl:
                return bcm_gh2_switch_rtag7_selectcontrol_get(unit, arg);
                break;
            case bcmSwitchHashIP4Field0:
            case bcmSwitchHashIP4Field1:
            case bcmSwitchHashIP6Field0:
            case bcmSwitchHashIP6Field1:
            case bcmSwitchHashL2Field0:
            case bcmSwitchHashL2Field1:
            case bcmSwitchHashHG2UnknownField0:
            case bcmSwitchHashHG2UnknownField1:
                return bcm_gh2_switch_rtag7_fieldselect_get(unit, type, arg);
                break;
            case bcmSwitchHashMPLSField0:
            case bcmSwitchHashMPLSField1:
            case bcmSwitchHashL2MPLSField0:
            case bcmSwitchHashL2MPLSField1:
            case bcmSwitchHashL3MPLSField0:
            case bcmSwitchHashL3MPLSField1:
            case bcmSwitchHashMPLSTunnelField0:
            case bcmSwitchHashMPLSTunnelField1:
            case bcmSwitchHashMIMTunnelField0:
            case bcmSwitchHashMIMTunnelField1:
            case bcmSwitchHashMIMField0:
            case bcmSwitchHashMIMField1:
            case bcmSwitchHashL3MiMField0:
            case bcmSwitchHashL3MiMField1:
            case bcmSwitchHashL3L2MPLSField0:
            case bcmSwitchHashL3L2MPLSField1:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashL2VxlanField0:
            case bcmSwitchHashL2VxlanField1:
            case bcmSwitchHashL3VxlanField0:
            case bcmSwitchHashL3VxlanField1:
                return bcm_gh2_switch_rtag7_fieldselect_get(unit, type, arg);
                break;
            case bcmSwitchHashL2L2GreField0:
            case bcmSwitchHashL2L2GreField1:
            case bcmSwitchHashL3L2GreField0:
            case bcmSwitchHashL3L2GreField1:
            case bcmSwitchHashFCOEField0:
            case bcmSwitchHashFCOEField1:
            case bcmSwitchHashL2TrillField0:
            case bcmSwitchHashL2TrillField1:
            case bcmSwitchHashL3TrillField0:
            case bcmSwitchHashL3TrillField1:
            case bcmSwitchHashTrillTunnelField0:
            case bcmSwitchHashTrillTunnelField1:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashIP4TcpUdpField0:
            case bcmSwitchHashIP4TcpUdpField1:
            case bcmSwitchHashIP4TcpUdpPortsEqualField0:
            case bcmSwitchHashIP4TcpUdpPortsEqualField1:
            case bcmSwitchHashIP6TcpUdpField0:
            case bcmSwitchHashIP6TcpUdpField1:
            case bcmSwitchHashIP6TcpUdpPortsEqualField0:
            case bcmSwitchHashIP6TcpUdpPortsEqualField1:
                return bcm_gh2_switch_rtag7_fieldselect_get(unit, type, arg);
                break;
            case bcmSwitchHashField0PreProcessEnable:
                {
                uint32 rtag7_hash_control_3;
                SOC_IF_ERROR_RETURN
                    (READ_RTAG7_HASH_CONTROL_3r(unit, &rtag7_hash_control_3));
                *arg = soc_reg_field_get(unit, RTAG7_HASH_CONTROL_3r,
                           rtag7_hash_control_3, HASH_PRE_PROCESSING_ENABLE_Af);
                }
                return BCM_E_NONE;
                break;
            case bcmSwitchHashField1PreProcessEnable:
                {
                uint32 rtag7_hash_control_3;
                SOC_IF_ERROR_RETURN
                    (READ_RTAG7_HASH_CONTROL_3r(unit, &rtag7_hash_control_3));
                *arg = soc_reg_field_get(unit, RTAG7_HASH_CONTROL_3r,
                            rtag7_hash_control_3, HASH_PRE_PROCESSING_ENABLE_Bf);
                }
                return BCM_E_NONE;
                break;
            case bcmSwitchHashField0Config:
            case bcmSwitchHashField0Config1:
            case bcmSwitchHashField1Config:
            case bcmSwitchHashField1Config1:
            case bcmSwitchMacroFlowHashFieldConfig:
                return bcm_gh2_switch_rtag7_fieldconfig_get(unit, type, arg);
                break;
            case bcmSwitchTrunkHashSet1UnicastOffset:
            case bcmSwitchTrunkHashSet1NonUnicastOffset:
            case bcmSwitchFabricTrunkHashSet1UnicastOffset:
            case bcmSwitchFabricTrunkHashSet1NonUnicastOffset:
            case bcmSwitchLoadBalanceHashSet1UnicastOffset:
            case bcmSwitchLoadBalanceHashSet1NonUnicastOffset:
                 return BCM_E_UNAVAIL;
                 break;
            case bcmSwitchTrunkHashSet0UnicastOffset:
            case bcmSwitchTrunkHashSet0NonUnicastOffset:
            case bcmSwitchECMPHashSet0Offset:
            case bcmSwitchECMPVxlanHashOffset:
            case bcmSwitchEntropyHashSet0Offset:
            case bcmSwitchFabricTrunkHashSet0UnicastOffset:
            case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
            case bcmSwitchLoadBalanceHashSet0UnicastOffset:
            case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
            case bcmSwitchFabricTrunkFailoverHashOffset:
                return bcm_gh2_switch_rtag7_fieldoffset_get(unit, port,
                                                            type, arg);
                break;
            case bcmSwitchEcmpRoce2HashOffset:
                if (soc_feature(unit, soc_feature_rroce)) {
                    return bcm_gh2_switch_rtag7_fieldoffset_get(unit, port,
                                                                type, arg);
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmSwitchFabricTrunkDynamicHashOffset:
            case bcmSwitchTrunkDynamicHashOffset:
            case bcmSwitchEcmpDynamicHashOffset:
            case bcmSwitchFabricTrunkResilientHashOffset:
            case bcmSwitchTrunkResilientHashOffset:
            case bcmSwitchEcmpResilientHashOffset:
            case bcmSwitchTrunkFailoverHashOffset:
            case bcmSwitchECMPUnderlayHashSet0Offset:
            case bcmSwitchECMPUnderlayVxlanHashOffset:
            case bcmSwitchECMPL2GreHashOffset:
            case bcmSwitchECMPUnderlayL2GreHashOffset:
            case bcmSwitchECMPTrillHashOffset:
            case bcmSwitchECMPUnderlayTrillHashOffset:
            case bcmSwitchECMPMplsHashOffset:
            case bcmSwitchVirtualPortDynamicHashOffset:
            case bcmSwitchECMPOverlayHashOffset:
            case bcmSwitchVirtualPortUnderlayDynamicHashOffset:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchIgmpPktToCpu:
            case bcmSwitchIgmpPktDrop:
            case bcmSwitchMldPktToCpu:
            case bcmSwitchMldPktDrop:
            case bcmSwitchV4ResvdMcPktToCpu:
            case bcmSwitchV4ResvdMcPktFlood:
            case bcmSwitchV4ResvdMcPktDrop:
            case bcmSwitchV6ResvdMcPktToCpu:
            case bcmSwitchV6ResvdMcPktFlood:
            case bcmSwitchV6ResvdMcPktDrop:
            case bcmSwitchIgmpReportLeaveDrop:
            case bcmSwitchIgmpReportLeaveFlood:
            case bcmSwitchIgmpReportLeaveToCpu:
            case bcmSwitchIgmpQueryDrop:
            case bcmSwitchIgmpQueryFlood:
            case bcmSwitchIgmpQueryToCpu:
            case bcmSwitchIgmpUnknownDrop:
            case bcmSwitchIgmpUnknownFlood:
            case bcmSwitchIgmpUnknownToCpu:
            case bcmSwitchMldReportDoneDrop:
            case bcmSwitchMldReportDoneFlood:
            case bcmSwitchMldReportDoneToCpu:
            case bcmSwitchMldQueryDrop:
            case bcmSwitchMldQueryFlood:
            case bcmSwitchMldQueryToCpu:
            case bcmSwitchIpmcV4RouterDiscoveryDrop:
            case bcmSwitchIpmcV4RouterDiscoveryFlood:
            case bcmSwitchIpmcV4RouterDiscoveryToCpu:
            case bcmSwitchIpmcV6RouterDiscoveryDrop:
            case bcmSwitchIpmcV6RouterDiscoveryFlood:
            case bcmSwitchIpmcV6RouterDiscoveryToCpu:
                return _bcm_gh2_igmp_action_get(unit, port, type, arg);
                break;
            case bcmSwitchDirectedMirroring:
                *arg = 1;
                return BCM_E_NONE;
                break;
            case bcmSwitchFlexibleMirrorDestinations:
            case bcmSwitchMirrorExclusive:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL3EgressMode:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_l3)) {
                    return bcm_xgs3_l3_egress_mode_get(unit, arg);
                }
#endif /* INCLUDE_L3 */
                break;
            case bcmSwitchL3HostAsRouteReturnValue:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_l3)) {
                      return bcm_xgs3_l3_host_as_route_return_get(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;

            case bcmSwitchL3DefipMultipathCountUpdate:
#ifdef INCLUDE_L3
                if(soc_feature(unit, soc_feature_l3_defip_ecmp_count)) {
                  *arg = 1;
                  return BCM_E_NONE;
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchIpmcReplicationSharing:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                    *arg = SOC_IPMCREPLSHR_GET(unit);
                    return BCM_E_NONE;
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchIpmcReplicationAvailabilityThreshold:
#ifdef INCLUDE_L3
                if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                    return _bcm_esw_ipmc_repl_threshold_get(unit, arg);
                }
#endif /* INCLUDE_L3 */
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepth:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    *arg = SOC_DUAL_HASH_MOVE_MAX(unit);
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthL2:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    *arg = SOC_DUAL_HASH_MOVE_MAX_L2X(unit);
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthMpls:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    *arg = SOC_DUAL_HASH_MOVE_MAX_MPLS(unit);
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthVlan:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    *arg = SOC_DUAL_HASH_MOVE_MAX_VLAN(unit);
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashDualMoveDepthEgressVlan:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    *arg = SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit);
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
#if defined(INCLUDE_L3)
            case bcmSwitchHashDualMoveDepthL3:
                if (soc_feature(unit, soc_feature_dual_hash)) {
                    *arg = SOC_DUAL_HASH_MOVE_MAX_L3X(unit);
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;
#endif /* INCLUDE_L3 */
            case bcmSwitchHashDualMoveDepthWlanPort:
            case bcmSwitchHashDualMoveDepthWlanClient:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchL2PortBlocking:
                if (soc_feature(unit, soc_feature_src_mac_group)) {
                    *arg = (!SOC_L2X_GROUP_ENABLE_GET(unit));
                    return BCM_E_NONE;
                }
                return BCM_E_UNAVAIL;
                break;

            case bcmSwitchTimeSyncPktDrop:
            case bcmSwitchTimeSyncPktFlood:
            case bcmSwitchTimeSyncPktToCpu:
            case bcmSwitchMmrpPktDrop:
            case bcmSwitchMmrpPktFlood:
            case bcmSwitchMmrpPktToCpu:
            case bcmSwitchSrpPktDrop:
            case bcmSwitchSrpPktFlood:
            case bcmSwitchSrpPktToCpu:
            case bcmSwitchSRPEthertype:
            case bcmSwitchMMRPEthertype:
            case bcmSwitchTimeSyncEthertype:
            case bcmSwitchSRPDestMacOui:
            case bcmSwitchMMRPDestMacOui:
            case bcmSwitchSRPDestMacNonOui:
            case bcmSwitchMMRPDestMacNonOui:
            case bcmSwitchTimeSyncDestMacNonOui:
            case bcmSwitchTimeSyncMessageTypeBitmap:
            case bcmSwitchTimeSyncClassAPktPrio:
            case bcmSwitchTimeSyncClassBPktPrio:
            case bcmSwitchTimeSyncClassAExeptionPktPrio:
            case bcmSwitchTimeSyncClassBExeptionPktPrio:
            case bcmSwitchStgInvalidToCpu:
            case bcmSwitchVlanTranslateEgressMissToCpu:
            case bcmSwitchL3PktErrToCpu:
            case bcmSwitchMtuFailureToCpu:
            case bcmSwitchSrcKnockoutToCpu:
            case bcmSwitchWlanTunnelMismatchToCpu:
            case bcmSwitchWlanTunnelMismatchDrop:
            case bcmSwitchWlanPortMissToCpu:
            case bcmSwitchUnknownVlanToCpuCosq:
            case bcmSwitchStgInvalidToCpuCosq:
            case bcmSwitchVlanTranslateEgressMissToCpuCosq:
            case bcmSwitchTunnelErrToCpuCosq:
            case bcmSwitchL3HeaderErrToCpuCosq:
            case bcmSwitchL3PktErrToCpuCosq:
            case bcmSwitchIpmcTtlErrToCpuCosq:
            case bcmSwitchMtuFailureToCpuCosq:
            case bcmSwitchHgHdrErrToCpuCosq:
            case bcmSwitchSrcKnockoutToCpuCosq:
            case bcmSwitchWlanTunnelMismatchToCpuCosq:
            case bcmSwitchWlanPortMissToCpuCosq:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchLayeredQoSResolution:
                return _bcm_gh2_layered_qos_resolution_get(unit, port, type, arg);
                break;
            case bcmSwitchEncapErrorToCpu:
            case bcmSwitchMirrorEgressTrueColorSelect:
            case bcmSwitchMirrorEgressTruePriority:
            case bcmSwitchL3UrpfRouteEnableExternal:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchLinkDownInfoSkip:
                return _bcm_esw_link_port_info_skip_get(unit, port, arg);
                break;
            case bcmSwitchDropSobmhOnLinkDown:
            case bcmSwitchCongestionNotificationIdLow:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchMeterAdjust:
                if (soc_feature(unit, soc_feature_meter_adjust)) {
                    return _bcm_gh2_meter_adjust_get(unit, port, arg);
                }
                break;
            case bcmSwitchShaperAdjust:
                if (soc_feature(unit, soc_feature_meter_adjust)) {
                    return _bcm_gh2_shaper_adjust_get(unit, port, arg);
                }
                break;

            case bcmSwitchPFCClass0Queue:
            case bcmSwitchPFCClass1Queue:
            case bcmSwitchPFCClass2Queue:
            case bcmSwitchPFCClass3Queue:
            case bcmSwitchPFCClass4Queue:
            case bcmSwitchPFCClass5Queue:
            case bcmSwitchPFCClass6Queue:
            case bcmSwitchPFCClass7Queue:
            case bcmSwitchPFCClass0McastQueue:
            case bcmSwitchPFCClass1McastQueue:
            case bcmSwitchPFCClass2McastQueue:
            case bcmSwitchPFCClass3McastQueue:
            case bcmSwitchPFCClass4McastQueue:
            case bcmSwitchPFCClass5McastQueue:
            case bcmSwitchPFCClass6McastQueue:
            case bcmSwitchPFCClass7McastQueue:
            case bcmSwitchPFCClass0DestmodQueue:
            case bcmSwitchPFCClass1DestmodQueue:
            case bcmSwitchPFCClass2DestmodQueue:
            case bcmSwitchPFCClass3DestmodQueue:
            case bcmSwitchPFCClass4DestmodQueue:
            case bcmSwitchPFCClass5DestmodQueue:
            case bcmSwitchPFCClass6DestmodQueue:
            case bcmSwitchPFCClass7DestmodQueue:
                return bcmi_gh2_cosq_port_pfc_get(unit, port, type, arg);
                break;
            case bcmSwitchEntropyHashSet1Offset:
            case bcmSwitchMiMDefaultSVP:
            case bcmSwitchAlternateStoreForward:
            case bcmSwitchMcQueueSchedMode:
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchTimesyncEgressTimestampingMode:
                if (soc_feature(unit, soc_feature_timesync_support) &&
                    soc_feature(unit, soc_feature_timesync_timestampingmode)) {
                    return _bcm_esw_port_timesync_timestamping_mode_get(unit, port,
                                        (bcm_switch_timesync_timestamping_mode_t*)arg);
                }
                return BCM_E_UNAVAIL;
                break;
            case bcmSwitchHashRoce1Field0:
            case bcmSwitchHashRoce1Field1:
            case bcmSwitchHashRoce2IP4Field0:
            case bcmSwitchHashRoce2IP4Field1:
            case bcmSwitchHashRoce2IP6Field0:
            case bcmSwitchHashRoce2IP6Field1:
                if (soc_feature(unit, soc_feature_rroce)) {
                    return bcm_gh2_switch_rtag7_fieldselect_get(unit, type,
                                                                arg);
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;

            default :
                break;
        }

        found = 0;
        bcm_gh2_switch_control_port_binding_get(unit, port, type, arg, &found);
        return (found ? BCM_E_NONE : BCM_E_UNAVAIL);

    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_gh2_switch_control_set
 * Description:
 *      Specify general switch behaviors.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      For behaviors which are port-specific, all non-stack ports
 *      will be set.
 */

int
bcm_gh2_switch_control_set(int unit,
                           bcm_switch_control_t type,
                           int arg)
{
    bcm_port_t    port;
    int                 rv, found;
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    int                 stable_select;
#endif

    /* Check for device wide, non port specific controls */
    switch (type) {
        case bcmSwitchWarmBoot:
            /* If true, set the Warm Boot state; clear otherwise */
            if (arg) {
#ifdef BCM_WARM_BOOT_SUPPORT
                /* Cannot be set to anything but FALSE */
                return BCM_E_PARAM;
#else
                return SOC_E_UNAVAIL;
#endif
            } else {
#ifdef BCM_WARM_BOOT_SUPPORT
                SOC_WARM_BOOT_DONE(unit);
#else
                return SOC_E_UNAVAIL;
#endif
            }
            return BCM_E_NONE;
            break;
        case bcmSwitchStableSelect:
            /* Cannot be called once bcm_init is complete */
            return SOC_E_UNAVAIL;
            break;
        case bcmSwitchStableSize:
            /* Cannot be called once bcm_init is complete */
            return SOC_E_UNAVAIL;
            break;
        case bcmSwitchControlSync:
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
            rv = _bcm_gh2_switch_control_sync(unit, arg);
            return rv;
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchControlAutoSync:
#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
            BCM_IF_ERROR_RETURN
                (bcm_gh2_switch_control_get(unit, bcmSwitchStableSelect,
                                            &stable_select));
            if (BCM_SWITCH_STABLE_NONE != stable_select) {
                SOC_CONTROL_LOCK(unit);
                SOC_CONTROL(unit)->autosync = arg ? 1 : 0;
                SOC_CONTROL_UNLOCK(unit);
                return BCM_E_NONE;
            } else {
                return BCM_E_NOT_FOUND;
            }
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchModuleType:
            return(_bcm_gh2_switch_module_type_set(unit,
                                               BCM_SWITCH_CONTROL_MODID_UNPACK(arg),
                                               BCM_SWITCH_CONTROL_MODTYPE_UNPACK(arg)));
            break;
#ifdef BCM_CB_ABORT_ON_ERR
        case bcmSwitchCallbackAbortOnError:
            if (arg >= 0) {
                SOC_CB_ABORT_ON_ERR(unit) = arg ? 1 : 0;
                return BCM_E_NONE;
            } else {
                return BCM_E_PARAM;
            }
            break;
#endif

        case bcmSwitchIpmcGroupMtu:
        case bcmSwitchLoopbackMtuSize:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMcastFloodDefault:
            return(_bcm_esw_vlan_flood_default_set(unit, arg));
            break;
        case bcmSwitchUseGport:
            SOC_USE_GPORT_SET(unit, arg);
            return BCM_E_NONE;
            break;
        case bcmSwitchHgHdrMcastFlood:
            return(_bcm_esw_higig_flood_l2_set(unit, arg));
            break;
        case bcmSwitchHgHdrIpMcastFlood:
        case bcmSwitchFailoverStackTrunk:
        case bcmSwitchFailoverEtherTrunk:
        case bcmSwitchMcastL2Range:
        case bcmSwitchMcastL3Range:
            return BCM_E_UNAVAIL;
            break;

        /*    coverity[equality_cond]    */
        case bcmSwitchHgHdrMcastVlanRange:
        case bcmSwitchHgHdrMcastL2Range:
        case bcmSwitchHgHdrMcastL3Range:
            {
            int bc_size, mc_size, ipmc_size;
            BCM_IF_ERROR_RETURN
                (soc_hbx_higig2_mcast_sizes_get(unit, &bc_size,
                                                      &mc_size, &ipmc_size));
            switch (type) {
            case bcmSwitchHgHdrMcastVlanRange:
                bc_size = arg;
                break;
            case bcmSwitchHgHdrMcastL2Range:
                mc_size = arg;
                break;
            case bcmSwitchHgHdrMcastL3Range:
                ipmc_size = arg;
                break;
            /* Defensive Default */
            /* coverity[dead_error_begin] */
            default:
                return BCM_E_PARAM;
            }
            return soc_hbx_higig2_mcast_sizes_set(unit, bc_size,
                                                      mc_size, ipmc_size);
            }
            break;

        case bcmSwitchHashSeed0:
            return soc_reg_field32_modify
                (unit, RTAG7_HASH_SEED_Ar, REG_PORT_ANY, HASH_SEED_Af, arg);
            break;
        case bcmSwitchHashSeed1:
            return soc_reg_field32_modify
                (unit, RTAG7_HASH_SEED_Br, REG_PORT_ANY, HASH_SEED_Bf, arg);
            break;
        case bcmSwitchMacroFlowHashMinOffset:
        case bcmSwitchMacroFlowHashMaxOffset:
        case bcmSwitchMacroFlowHashStrideOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMinOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMaxOffset:
        case bcmSwitchMacroFlowHigigTrunkHashStrideOffset:
        case bcmSwitchMacroFlowTrunkHashMinOffset:
        case bcmSwitchMacroFlowTrunkHashMaxOffset:
        case bcmSwitchMacroFlowTrunkHashStrideOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset:
        /* RTAG7 Macro Flow Concatenation */
        case bcmSwitchMacroFlowEcmpHashConcatEnable:
        case bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable:
        case bcmSwitchMacroFlowLoadBalanceHashConcatEnable:
        case bcmSwitchMacroFlowTrunkHashConcatEnable:
        case bcmSwitchMacroFlowHigigTrunkHashConcatEnable:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_macroflow_offset_set(unit,
                                                                 type, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMacroFlowHashOverlayMinOffset:
        case bcmSwitchMacroFlowHashOverlayMaxOffset:
        case bcmSwitchMacroFlowHashOverlayStrideOffset:
        case bcmSwitchMacroFlowEcmpHashOverlayConcatEnable:
            return BCM_E_UNAVAIL;
            break;
        /* RTAG7 Macro Flow Hash Seed */
        case bcmSwitchMacroFlowECMPHashSeed:
        case bcmSwitchMacroFlowECMPUnderlayHashSeed:
        case bcmSwitchMacroFlowLoadBalanceHashSeed:
        case bcmSwitchMacroFlowTrunkHashSeed:
        case bcmSwitchMacroFlowHigigTrunkHashSeed:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_macroflow_hash_set(unit,
                                                               type, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMacroFlowECMPOverlayHashSeed:
        /* Hash Field select controls */
        case bcmSwitchHashTrillPayloadSelect0:
        case bcmSwitchHashTrillPayloadSelect1:
        case bcmSwitchHashTrillTunnelSelect0:
        case bcmSwitchHashTrillTunnelSelect1:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchHashVxlanPayloadSelect0:
        case bcmSwitchHashVxlanPayloadSelect1:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_field_control_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashIP6AddrCollapseSelect0:
        case bcmSwitchHashIP6AddrCollapseSelect1:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_field_control_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashIP6NextHeaderUseExtensionHeader0:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return soc_reg_field32_modify
                    (unit, RTAG7_HASH_CONTROLr, REG_PORT_ANY,
                     DISABLE_HASH_IP_EXTENSION_HEADERS_Af, (arg) ? 0 : 1);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashIP6NextHeaderUseExtensionHeader1:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return soc_reg_field32_modify
                    (unit, RTAG7_HASH_CONTROLr, REG_PORT_ANY,
                     DISABLE_HASH_IP_EXTENSION_HEADERS_Bf, (arg) ? 0 : 1);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL2MPLSPayloadSelect0:
        case bcmSwitchHashL2MPLSPayloadSelect1:
        case bcmSwitchHashIPSecSelect0:
        case bcmSwitchHashIPSecSelect1:
        case bcmSwitchHashMiMPayloadSelect0:
        case bcmSwitchHashMiMPayloadSelect1:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchL2McastAllRouterPortsAddEnable:
            SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) = arg ? 1 : 0;
            return BCM_E_NONE;
            break;
        case bcmSwitchPFCClass0Queue:
        case bcmSwitchPFCClass1Queue:
        case bcmSwitchPFCClass2Queue:
        case bcmSwitchPFCClass3Queue:
        case bcmSwitchPFCClass4Queue:
        case bcmSwitchPFCClass5Queue:
        case bcmSwitchPFCClass6Queue:
        case bcmSwitchPFCClass7Queue:
           return BCM_E_UNAVAIL;
           break;
        case bcmSwitchL2HitClear:
        case bcmSwitchL2SrcHitClear:
        case bcmSwitchL2DstHitClear:
        case bcmSwitchL3HostHitClear:
        case bcmSwitchL3RouteHitClear:
           return _bcm_gh2_switch_hit_clear_set(unit, type, arg);
           break;
        /* This swithc control should be supported only per port. */
        case bcmSwitchEncapErrorToCpu:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchSnapNonZeroOui:
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                                        SNAP_OTHER_DECODE_ENABLEf,
                                        arg ? 1 : 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_CONFIGr, REG_PORT_ANY,
                                        SNAP_OTHER_DECODE_ENABLEf,
                                        arg ? 1 : 0));
            return BCM_E_NONE;
            break;
        case bcmSwitchL2McIdxRetType:
            return _bcm_esw_mcast_idx_ret_type_set(unit, arg);
            break;
        case bcmSwitchL2ExtLearn:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchL3McIdxRetType:
#ifdef INCLUDE_L3
            return _bcm_esw_ipmc_idx_ret_type_set(unit, arg);
#endif
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchL2SourceDiscardMoveToCpu:
            {
                soc_reg_t   reg;
                reg = ING_CONFIG_64r;
                BCM_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, reg, REG_PORT_ANY,
                                           STNMOVE_ON_L2SRC_DISCf, arg ? 1: 0));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchFabricTrunkDynamicSampleRate:
        case bcmSwitchFabricTrunkDynamicEgressBytesExponent:
        case bcmSwitchFabricTrunkDynamicQueuedBytesExponent:
        case bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold:
        case bcmSwitchTrunkDynamicSampleRate:
        case bcmSwitchTrunkDynamicAccountingSelect:
        case bcmSwitchTrunkDynamicEgressBytesExponent:
        case bcmSwitchTrunkDynamicQueuedBytesExponent:
        case bcmSwitchTrunkDynamicEgressBytesDecreaseReset:
        case bcmSwitchTrunkDynamicQueuedBytesDecreaseReset:
        case bcmSwitchTrunkDynamicEgressBytesMinThreshold:
        case bcmSwitchTrunkDynamicEgressBytesMaxThreshold:
        case bcmSwitchTrunkDynamicQueuedBytesMinThreshold:
        case bcmSwitchTrunkDynamicQueuedBytesMaxThreshold:
        case bcmSwitchTrunkDynamicExpectedLoadMinThreshold:
        case bcmSwitchTrunkDynamicExpectedLoadMaxThreshold:
        case bcmSwitchTrunkDynamicImbalanceMinThreshold:
        case bcmSwitchTrunkDynamicImbalanceMaxThreshold:
        case bcmSwitchEcmpDynamicSampleRate:
        case bcmSwitchEcmpDynamicAccountingSelect:
        case bcmSwitchEcmpDynamicEgressBytesExponent:
        case bcmSwitchEcmpDynamicQueuedBytesExponent:
        case bcmSwitchEcmpDynamicEgressBytesDecreaseReset:
        case bcmSwitchEcmpDynamicQueuedBytesDecreaseReset:
        case bcmSwitchEcmpDynamicEgressBytesMinThreshold:
        case bcmSwitchEcmpDynamicEgressBytesMaxThreshold:
        case bcmSwitchEcmpDynamicQueuedBytesMinThreshold:
        case bcmSwitchEcmpDynamicQueuedBytesMaxThreshold:
        case bcmSwitchEcmpDynamicExpectedLoadMinThreshold:
        case bcmSwitchEcmpDynamicExpectedLoadMaxThreshold:
        case bcmSwitchEcmpDynamicImbalanceMinThreshold:
        case bcmSwitchEcmpDynamicImbalanceMaxThreshold:
            return BCM_E_UNAVAIL;
            break;

#if (defined(BCM_RCPU_SUPPORT) || defined(BCM_OOB_RCPU_SUPPORT)) && \
         defined(BCM_XGS3_SWITCH_SUPPORT)
        case bcmSwitchRemoteCpuSchanEnable:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CMIC_PKT_CTRLr,
                                    REG_PORT_ANY, ENABLE_SCHAN_REQUESTf, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuMatchLocalMac:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    CMIC_PKT_CTRLr, REG_PORT_ANY, LMAC0_MATCHf, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuFromCpuEnable:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    CMIC_PKT_CTRLr, REG_PORT_ANY, ENABLE_FROMCPU_PACKETf, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuToCpuEnable:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    CMIC_PKT_CTRLr, REG_PORT_ANY, ENABLE_TOCPU_PACKETf, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuCmicEnable:
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    CMIC_PKT_CTRLr, REG_PORT_ANY, ENABLE_CMIC_REQUESTf, arg));
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuMatchVlan:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    CMIC_PKT_CTRLr, REG_PORT_ANY, VLAN_MATCHf, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuForceScheduling:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                BCM_IF_ERROR_RETURN(
                  _bcm_gh2_rcpu_switch_regall_set(unit, CMIC_PKT_COS_0r,
                                                  COSf, arg));
                BCM_IF_ERROR_RETURN(
                  _bcm_gh2_rcpu_switch_regall_set(unit, CMIC_PKT_COS_1r,
                                                  COSf, arg));
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuToCpuDestPortAllReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                int i;
                for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_DIRECT_0_TYPEr); i++) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_gh2_rcpu_switch_regall_idx_set(unit,
                                                          CMIC_PKT_REASON_DIRECT_0_TYPEr,
                                                          i, REASONSf, arg));
                }
                for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_DIRECT_1_TYPEr); i++) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_gh2_rcpu_switch_regall_idx_set(unit,
                                                          CMIC_PKT_REASON_DIRECT_1_TYPEr,
                                                          i, REASONSf, arg));
                }
                for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_DIRECT_2_TYPEr); i++) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_gh2_rcpu_switch_regall_idx_set(unit,
                                                          CMIC_PKT_REASON_DIRECT_2_TYPEr,
                                                          i, REASONSf, arg));
                }
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuToCpuDestMacAllReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                int i;
                for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_0_TYPEr); i++) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_gh2_rcpu_switch_regall_idx_set(unit,
                                                          CMIC_PKT_REASON_0_TYPEr,
                                                          i, REASONSf, arg));
                }
                for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_1_TYPEr); i++) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_gh2_rcpu_switch_regall_idx_set(unit,
                                                          CMIC_PKT_REASON_1_TYPEr,
                                                          i, REASONSf, arg));
                }
                for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_2_TYPEr); i++) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_gh2_rcpu_switch_regall_idx_set(unit,
                                                          CMIC_PKT_REASON_2_TYPEr,
                                                          i, REASONSf, arg));
                }
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectPktReasons:
                if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                    uint32 raddr;
                    int i;
                    for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_0_TYPEr); i++) {
                        raddr = soc_reg_addr(unit, CMIC_PKT_REASON_0_TYPEr,
                                             REG_PORT_ANY, i);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                    }
                    return BCM_E_NONE;
                } else
#endif
                {
                    return BCM_E_UNAVAIL;
                }
            break;
        case bcmSwitchRxRedirectPktReasonsExtended:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    uint32 raddr;
                    int i;

                    for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_1_TYPEr); i++) {
                        raddr = soc_reg_addr(unit, CMIC_PKT_REASON_1_TYPEr,
                                             REG_PORT_ANY, i);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                    }
                    return BCM_E_NONE;
                }
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
                break;
        case bcmSwitchRxRedirectHigigPktReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    uint32 raddr;
                    int i;
                    for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_DIRECT_0_TYPEr); i++) {
                        raddr = soc_reg_addr(unit, CMIC_PKT_REASON_DIRECT_0_TYPEr,
                                             REG_PORT_ANY, i);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                    }
                    return BCM_E_NONE;
                }
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectHigigPktReasonsExtended:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    uint32 raddr;
                    int i;
                    for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_DIRECT_1_TYPEr); i++) {
                       raddr = soc_reg_addr(unit, CMIC_PKT_REASON_DIRECT_1_TYPEr,
                                            REG_PORT_ANY, i);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                    }
                    return BCM_E_NONE;
                }
            }else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectTruncatedPktReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)){
                    uint32 raddr;
                    int i;
                    for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_MINI_0_TYPEr); i++) {
                        raddr = soc_reg_addr(unit, CMIC_PKT_REASON_MINI_0_TYPEr,
                                             REG_PORT_ANY, i);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                    }
                    return BCM_E_NONE;
                }
            }else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectTruncatedPktReasonsExtended:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    uint32 raddr;
                    int i;
                    for (i = 0; i < SOC_REG_NUMELS(unit, CMIC_PKT_REASON_MINI_1_TYPEr); i++) {
                        raddr = soc_reg_addr(unit, CMIC_PKT_REASON_MINI_1_TYPEr,
                                             REG_PORT_ANY, i);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                    }
                    return BCM_E_NONE;
                }
            }else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectPktCos:
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm) &&
                SOC_REG_IS_VALID(unit, CMIC_PKT_COS_QUEUES_LOr)) {
                uint32 raddr;
                raddr = soc_reg_addr(unit, CMIC_PKT_COS_QUEUES_LOr,
                                     REG_PORT_ANY, 0);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectPktCosExtended:
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm) &&
                SOC_REG_IS_VALID(unit, CMIC_PKT_COS_QUEUES_HIr)) {
                uint32 raddr;
                raddr = soc_reg_addr(unit, CMIC_PKT_COS_QUEUES_HIr,
                                     REG_PORT_ANY, 0);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(soc_pci_write(unit, raddr, arg));
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuLocalMacOui:
        case bcmSwitchRemoteCpuDestMacOui:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_switch_mac_hi_set(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuTcMappingMacOui:
            if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
                uint32 fieldval, regval;
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_DA_LSr(unit, &regval));
                fieldval = soc_reg_field_get(unit, REMOTE_CPU_DA_LSr, regval,
                                             DAf);
                fieldval = (arg << 24);
                soc_reg_field_set(unit, REMOTE_CPU_DA_LSr, &regval, DAf, fieldval);
                BCM_IF_ERROR_RETURN(
                    WRITE_REMOTE_CPU_DA_LSr(unit, regval));
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_DA_MSr(unit, &regval));
                fieldval = soc_reg_field_get(unit, REMOTE_CPU_DA_MSr, regval,
                                             DAf);
                fieldval = (arg >> 8) & 0xffff;
                soc_reg_field_set(unit, REMOTE_CPU_DA_MSr, &regval, DAf, fieldval);
                BCM_IF_ERROR_RETURN(
                    WRITE_REMOTE_CPU_DA_MSr(unit, regval));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuLocalMacNonOui:
        case bcmSwitchRemoteCpuDestMacNonOui:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_switch_mac_lo_set(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuTcMappingMacNonOui:
            if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
                uint32 fieldval, regval;
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_DA_LSr(unit, &regval));
                fieldval = soc_reg_field_get(unit, REMOTE_CPU_DA_LSr, regval,
                                             DAf);
                fieldval &= 0xff000000;
                fieldval |= ((arg << 8) >> 8);
                soc_reg_field_set(unit, REMOTE_CPU_DA_LSr, &regval, DAf, fieldval);
                BCM_IF_ERROR_RETURN(
                    WRITE_REMOTE_CPU_DA_LSr(unit,  regval));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuEthertype:
        case bcmSwitchRemoteCpuVlan:
        case bcmSwitchRemoteCpuTpid:
        case bcmSwitchRemoteCpuSignature:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_switch_vlan_tpid_sig_set(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuTcMappingEthertype:
            if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
                BCM_IF_ERROR_RETURN(WRITE_REMOTE_CPU_LENGTH_TYPEr(unit, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuDestPort:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_pipe_bypass_header_set(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuHigigDestPort:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_higig_header_set(unit, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#endif
        case bcmSwitchServiceTpidReplace:
        case bcmSwitchCongestionCntag:
        case bcmSwitchCongestionCntagEthertype:
        case bcmSwitchCongestionCnm:
        case bcmSwitchCongestionCnmEthertype:
        case bcmSwitchCongestionNotificationIdHigh:
        case bcmSwitchCongestionNotificationIdQueue:
        case bcmSwitchCongestionUseOuterTpid:
        case bcmSwitchCongestionUseOuterVlan:
        case bcmSwitchCongestionUseOuterPktPri:
        case bcmSwitchCongestionUseOuterCfi:
        case bcmSwitchCongestionUseInnerPktPri:
        case bcmSwitchCongestionUseInnerCfi:
        case bcmSwitchCongestionMissingCntag:
        case bcmSwitchCongestionCnmSrcMacNonOui:
        case bcmSwitchCongestionCnmSrcMacOui:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchLoadBalanceHashSelect:
            {
            soc_reg_t   reg;
            reg = ING_CONFIG_64r;
            return soc_reg_field32_modify(unit, reg, REG_PORT_ANY,
                                                LBID_RTAGf, arg);
            }
            break;
        case bcmSwitchTrillEthertype:
        case bcmSwitchTrillISISEthertype:
        case bcmSwitchTrillISISDestMacOui:
        case bcmSwitchTrillBroadcastDestMacOui:
        case bcmSwitchTrillEndStationDestMacOui:
        case bcmSwitchTrillISISDestMacNonOui:
        case bcmSwitchTrillBroadcastDestMacNonOui:
        case bcmSwitchTrillEndStationDestMacNonOui:
        case bcmSwitchTrillMinTtl:
        case bcmSwitchTrillTtlCheckEnable:
        case bcmSwitchFcoeEtherType:
            return BCM_E_UNAVAIL;
            break;
#if defined(INCLUDE_L3)
        case bcmSwitchL3TunnelIpV4ModeOnly:
            return soc_tunnel_term_block_size_set (unit, arg);
            break;
        case bcmSwitchL3SrcHitEnable:
            return soc_reg_field32_modify(unit, ING_CONFIG_64r,
                                          REG_PORT_ANY, L3SRC_HIT_ENABLEf,
                                          (uint32)arg);
            break;
#endif /* INCLUDE_L3*/
        case bcmSwitchL2DstHitEnable:
            return soc_reg_field32_modify(unit, ING_CONFIG_64r,
                                          REG_PORT_ANY, L2DST_HIT_ENABLEf,
                                          (uint32)arg);
            break;
        case bcmSwitchHashDualMoveDepth:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthL2:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX_L2X(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthMpls:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX_MPLS(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthVlan:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX_VLAN(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthEgressVlan:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthWlanPort:
        case bcmSwitchHashDualMoveDepthWlanClient:
        case bcmSwitchL2GreProtocolType:
        case bcmSwitchL2GreVpnIdSizeSet:
        case bcmSwitchRemoteEncapsulationMode:
            return BCM_E_UNAVAIL;
            break;
#if defined(INCLUDE_L3)
        case bcmSwitchVxlanUdpDestPortSet:
        case bcmSwitchVxlanUdpDestPortSet1:
            /* Set UDP Dest port for VXLAN */
            if (soc_feature(unit, soc_feature_vxlan_lite)) {
                  return bcmi_gh2_vxlan_udp_dest_port_set(unit, type, arg);
            } else {
                  return BCM_E_UNAVAIL;
            }
          break;

        case bcmSwitchVxlanEntropyEnable:
        case bcmSwitchVxlanEntropyEnableIP6:
            /* Enable UDP Source Port Hash for VXLAN */
            if (soc_feature(unit, soc_feature_vxlan_lite)) {
                  return bcmi_gh2_vxlan_udp_source_port_set(unit, type, arg);
            } else {
                  return BCM_E_UNAVAIL;
            }
          break;
        case bcmSwitchHashDualMoveDepthL3:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX_L3X(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL3DualLeastFull:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, L3_AUX_HASH_CONTROLr,
                                                REG_PORT_ANY,
                                                INSERT_LEAST_FULL_HALFf,
                                                (arg ? 1 : 0)));
                    return BCM_E_NONE;
                }
            }
            return BCM_E_UNAVAIL;
            break;
#endif
        case bcmSwitchHashMultiMoveDepth:
        case bcmSwitchHashMultiMoveDepthL2:
        case bcmSwitchHashMultiMoveDepthL3:
        case bcmSwitchHashMultiMoveDepthMpls:
        case bcmSwitchHashMultiMoveDepthVlan:
        case bcmSwitchHashMultiMoveDepthEgressVlan:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL2DualLeastFull:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, L2_AUX_HASH_CONTROLr,
                                                REG_PORT_ANY,
                                                INSERT_LEAST_FULL_HALFf,
                                                (arg ? 1 : 0)));
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashMPLSDualLeastFull:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchSynchronousPortClockSource:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_sync_port_select_set(unit, (uint32)arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchSynchronousPortClockSourceBkup:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_sync_port_backup_select_set(unit,(uint32)arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmSwitchSynchronousPortClockSourceDivCtrl:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_div_ctrl_select_set(unit, (uint32)arg);
            } else {
                   return BCM_E_UNAVAIL;
            }
            break;

        case bcmSwitchSynchronousPortClockSourceBkupDivCtrl:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_div_ctrl_backup_select_set(unit, (uint32)arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#if defined(INCLUDE_L3)
        case bcmSwitchNivEthertype:
            if (soc_feature(unit, soc_feature_niv)) {
                return bcm_gh_niv_ethertype_set(unit, arg);
            }
            break;
#endif /* INCLUDE_L3 */
        case bcmSwitchSubportPktTagEthertype:
            return BCM_E_UNAVAIL;
            break;
#if defined(INCLUDE_L3)
        case bcmSwitchEtagEthertype:
             if (soc_feature(unit, soc_feature_port_extension)) {
                 return bcm_gh_etag_ethertype_set(unit, arg);
             }
             return BCM_E_UNAVAIL;
             break;
        case bcmSwitchExtenderMulticastLowerThreshold:
             if (soc_feature(unit, soc_feature_port_extension)) {
                 BCM_IF_ERROR_RETURN(
                     soc_reg_field32_modify(unit, ETAG_MULTICAST_RANGEr,
                                            REG_PORT_ANY,
                                            ETAG_VID_MULTICAST_LOWERf, arg));
                 return BCM_E_NONE;
             }
             return BCM_E_UNAVAIL;
             break;
        case bcmSwitchExtenderMulticastHigherThreshold:
             if (soc_feature(unit, soc_feature_port_extension)) {
                 BCM_IF_ERROR_RETURN(
                     soc_reg_field32_modify(unit, ETAG_MULTICAST_RANGEr,
                                            REG_PORT_ANY,
                                            ETAG_VID_MULTICAST_UPPERf, arg));
                 return BCM_E_NONE;
             }
             return BCM_E_UNAVAIL;
             break;
#endif /*  INCLUDE_L3 */
        case bcmSwitchWESPProtocolEnable:
        case bcmSwitchWESPProtocol:
        case bcmSwitchIp6CompressEnable:
        case bcmSwitchIp6CompressDefaultOffset:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMirrorPktChecksEnable:
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_CONFIG_1r, REG_PORT_ANY,
                    DISABLE_MIRROR_CHECKSf, arg ? 0 : 1));
            return BCM_E_NONE;

            break;

        case bcmSwitchStableSaveLongIds:
#if defined(BCM_FIELD_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
            {
                _field_control_t *fc;

                BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

                if (arg) {
                    fc->flags |= _FP_STABLE_SAVE_LONG_IDS;
                } else {
                    fc->flags &= ~_FP_STABLE_SAVE_LONG_IDS;
                }
            }
            return (BCM_E_NONE);
#else
            return (BCM_E_UNAVAIL);
#endif
            break;
        case bcmSwitchGportAnyDefaultL2Learn:
        case bcmSwitchGportAnyDefaultL2Move:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchOamCcmToCpu:
            if (soc_feature(unit, soc_feature_oam)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, CCM_COPYTO_CPU_CONTROLr,
                                            REG_PORT_ANY, ERROR_CCM_COPY_TOCPUf,
                                            (arg ? 1 : 0)));
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchOamXconCcmToCpu:
            if (soc_feature(unit, soc_feature_oam)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, CCM_COPYTO_CPU_CONTROLr,
                                            REG_PORT_ANY, XCON_CCM_COPY_TOCPUf,
                                            (arg ? 1 : 0)));
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchOamXconOtherToCpu:
            if (soc_feature(unit, soc_feature_oam)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, CCM_COPYTO_CPU_CONTROLr,
                                            REG_PORT_ANY, XCON_OTHER_COPY_TOCPUf,
                                            (arg ? 1 : 0)));
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchSetMplsEntropyLabelTtl:
        case bcmSwitchSetMplsEntropyLabelPri:
        case bcmSwitchSetMplsEntropyLabelOffset:
            return BCM_E_UNAVAIL;
            break;
#if defined(INCLUDE_L3)
        case bcmSwitchMultipathCompress:
            if (soc_feature(unit, soc_feature_l3) &&
                !soc_feature(unit, soc_feature_l3_no_ecmp)) {
                return bcm_tr2_l3_ecmp_defragment(unit);
            }
            break;

        case bcmSwitchMultipathCompressBuffer:
            if (soc_feature(unit, soc_feature_l3) &&
                !soc_feature(unit, soc_feature_l3_no_ecmp)) {
                return bcm_tr2_l3_ecmp_defragment_buffer_set(unit, arg);
            }
            break;
#endif /* INCLUDE_L3 */
        case bcmSwitchBstEnable:
        case bcmSwitchBstTrackingMode:
        case bcmSwitchBstSnapshotEnable:
        case bcmSwitchFabricTrunkAutoIncludeDisable:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchL2OverflowEvent:
            if (soc_feature(unit, soc_feature_l2_overflow)) {
                if (arg) {
                    SOC_CONTROL_LOCK(unit);
                    SOC_CONTROL(unit)->l2_overflow_enable = TRUE;
                    SOC_CONTROL_UNLOCK(unit);
                    return soc_hr3_l2_overflow_start(unit);
                } else {
                    SOC_CONTROL_LOCK(unit);
                    SOC_CONTROL(unit)->l2_overflow_enable = FALSE;
                    SOC_CONTROL_UNLOCK(unit);
                    return soc_hr3_l2_overflow_stop(unit);
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchMiMDefaultSVPValue:
        case bcmSwitchOamVersionCheckDisable:
        case bcmSwitchOamOlpChipEtherType:
        case bcmSwitchOamOlpChipTpid:
        case bcmSwitchOamOlpChipVlan:
        case bcmSwitchOamOlpStationMacNonOui:
        case bcmSwitchOamOlpStationMacOui:
        case bcmSwitchOamStackingSupport:
        case bcmSwitchFieldStageEgressToCpu:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchSystemReservedVlan:
            return _bcm_esw_vlan_system_reserved_set(unit, arg);
            break;
        case bcmSwitchHashOamEgress:
        case bcmSwitchHashOamEgressDual:
        case bcmSwitchHashOam:
        case bcmSwitchHashOamDual:
        case bcmSwitchHashOamLeastFull:
        case bcmSwitchHashDualMoveDepthOam:
                /* Not supported for now */
        case bcmSwitchWredForAllPkts:
        case bcmSwitchFcoeNpvModeEnable:
        case bcmSwitchFcoeDomainRoutePrefixLength:
        case bcmSwitchFcoeCutThroughEnable:
        case bcmSwitchFcoeSourceBindCheckAction:
        case bcmSwitchFcoeSourceFpmaPrefixCheckAction:
        case bcmSwitchFcoeVftHopCountExpiryToCpu:
        case bcmSwitchFcoeVftHopCountExpiryAction:
        case bcmSwitchFcoeFcEofT1Value:
        case bcmSwitchFcoeFcEofT2Value:
        case bcmSwitchFcoeFcEofA1Value:
        case bcmSwitchFcoeFcEofA2Value:
        case bcmSwitchFcoeFcEofN1Value:
        case bcmSwitchFcoeFcEofN2Value:
        case bcmSwitchFcoeFcEofNI1Value:
        case bcmSwitchFcoeFcEofNI2Value:
        case bcmSwitchFcoeZoneCheckFailToCpu:
        case bcmSwitchFcoeZoneCheckMissDrop:
        case bcmSwitchUnknownSubportPktTagToCpu:
        case bcmSwitchMirrorUnicastCosq:
        case bcmSwitchMirrorMulticastCosq:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchEEEQueueThresholdProfile0:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                return soc_reg_field32_modify(unit, EEE_QUEUE_THRESH_0r,
                                              REG_PORT_ANY,
                                              EEE_QUEUE_THRESHOLDf,
                                              arg);
            }
            break;
        case bcmSwitchEEEQueueThresholdProfile1:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                return soc_reg_field32_modify(unit, EEE_QUEUE_THRESH_1r,
                                              REG_PORT_ANY,
                                              EEE_QUEUE_THRESHOLDf,
                                              arg);
            }
            break;
        case bcmSwitchEEEQueueThresholdProfile2:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                return soc_reg_field32_modify(unit, EEE_QUEUE_THRESH_2r,
                                              REG_PORT_ANY,
                                              EEE_QUEUE_THRESHOLDf,
                                              arg);
            }
            break;
        case bcmSwitchEEEQueueThresholdProfile3:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                return soc_reg_field32_modify(unit, EEE_QUEUE_THRESH_3r,
                                              REG_PORT_ANY,
                                              EEE_QUEUE_THRESHOLDf,
                                              arg);
            }
            break;
       case bcmSwitchEEEPacketLatencyProfile0:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 fval = 0;
                fval = arg;
                return soc_reg_field32_modify(unit, EEE_MAX_PKT_LAT_0r,
                                              REG_PORT_ANY,
                                              EEE_MAX_PKT_LATENCYf,
                                              fval);
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile1:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 fval = 0;
                fval = arg;
                return soc_reg_field32_modify(unit, EEE_MAX_PKT_LAT_1r,
                                              REG_PORT_ANY,
                                              EEE_MAX_PKT_LATENCYf,
                                              fval);
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile2:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 fval = 0;
                fval = arg;
                return soc_reg_field32_modify(unit, EEE_MAX_PKT_LAT_2r,
                                              REG_PORT_ANY,
                                              EEE_MAX_PKT_LATENCYf,
                                              fval);
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile3:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 fval = 0;
                fval = arg;
                return soc_reg_field32_modify(unit, EEE_MAX_PKT_LAT_3r,
                                              REG_PORT_ANY,
                                              EEE_MAX_PKT_LATENCYf,
                                              fval);
            }
            break;
        case bcmSwitchEEEGlobalCongestionThreshold:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                return soc_reg_field32_modify(unit, EEE_GLOBAL_BUF_THRESHr,
                                              REG_PORT_ANY,
                                              EEE_GLOBAL_BUFFER_THREHSOLDf,
                                              arg);
            }
            break;
        case bcmSwitchStgInvalidToCpu:
        case bcmSwitchVlanTranslateEgressMissToCpu:
        case bcmSwitchL3PktErrToCpu:
        case bcmSwitchMtuFailureToCpu:
        case bcmSwitchSrcKnockoutToCpu:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchSymmetricHashControl:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_symmetric_hash_control_set(unit, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHigigCongestionClassEnable:
            if (soc_feature(unit, soc_feature_ecn_wred)) {
                BCM_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, EGR_CONFIG_1r,
                                           REG_PORT_ANY,
                                           HIGIG2_ECN_IN_CC_ENABLEf,
                                           (arg) ? 1 : 0));
                return soc_reg_field32_modify(unit, ING_CONFIG_64r,
                                              REG_PORT_ANY,
                                              HIGIG2_ECN_IN_CC_ENABLEf,
                                              (arg) ? 1 : 0);
            }
            break;
        case bcmSwitchEcnNonIpResponsive:
            if (soc_feature(unit, soc_feature_ecn_wred)) {
                return soc_reg_field32_modify(unit, ECN_CONTROLr,
                                              REG_PORT_ANY,
                                              RESPONSIVE_DEFAULTf,
                                              (arg) ? 1 : 0);
            }
            break;
        case bcmSwitchEcnNonIpIntCongestionNotification:
            if (soc_feature(unit, soc_feature_ecn_wred)) {
                uint32 reg_val = 0, responsive;
                soc_field_t fld_idx;
                BCM_IF_ERROR_RETURN(
                    READ_ECN_CONTROLr(unit, &reg_val));
                responsive = soc_reg_field_get(unit, ECN_CONTROLr,
                                               reg_val, RESPONSIVE_DEFAULTf);
                if (responsive) {
                    fld_idx = RESPONSIVE_DEFAULT_INT_CNf;
                } else {
                    fld_idx = NON_RESPONSIVE_DEFAULT_INT_CNf;
                }
                return soc_reg_field32_modify(unit, ECN_CONTROLr,
                                              REG_PORT_ANY,
                                              fld_idx,
                                              arg);
            }
            break;
        case bcmSwitchL3RouteCache:
        case bcmSwitchL3Max128BV6Entries:
        case bcmSwitchECMPLevel1RandomSeed:
        case bcmSwitchECMPLevel2RandomSeed:
        case bcmSwitchSubportCoEEtherType:
        case bcmSwitchSubportEgressTpid:
        case bcmSwitchSubportEgressWideTpid:
        case bcmSwitchIpmcSameVlanPruningOverride:
        case bcmSwitchMimBvidInsertionControl:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchL2CpuDeleteEvent:
            return _soc_l2mod_sbus_fifo_field_set(unit, L2_MOD_FIFO_ENABLE_L2_DELETEf, arg);
        case bcmSwitchL2LearnEvent:
            return _soc_l2mod_sbus_fifo_field_set(unit, L2_MOD_FIFO_ENABLE_LEARNf, arg);
        case bcmSwitchL2CpuAddEvent:
            return _soc_l2mod_sbus_fifo_field_set(unit, L2_MOD_FIFO_ENABLE_L2_INSERTf, arg);
        case bcmSwitchL2AgingEvent:
            return _soc_l2mod_sbus_fifo_field_set(unit, L2_MOD_FIFO_ENABLE_AGEf, arg);
            break;
        case bcmSwitchUdfHashEnable:
        case bcmSwitchOlpMatchRule:
        case bcmSwitchNonUcVlanShapingEnable:
        case bcmSwitchRangeCheckersAPIType:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchCustomHeaderEncapPriorityOffset:
            if (soc_feature(unit, soc_feature_custom_header)) {
                /* The offset should be 4-bit aligned */
                if (arg % 4 != 0) {
                    return BCM_E_PARAM;
                }
                return soc_reg_field32_modify(unit,
                                              CUSTOM_PACKET_HEADER_OVERRIDEr,
                                              REG_PORT_ANY,
                                              OFFSETf,
                                              arg >> 2);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchIngCapwapEnable:
            if (soc_feature(unit, soc_feature_ing_capwap_parser)) {
                return soc_reg_field32_modify(unit, CAPWAP_ENABLEr,
                                              REG_PORT_ANY,
                                              ENABLEf,
                                              (arg) ? 1 : 0);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchCapwapDataUdpDstPort:
            if (soc_feature(unit, soc_feature_ing_capwap_parser)) {
                if ((arg < 0) || (arg > 0xffff)) {
                    return BCM_E_PARAM;
                }
                return soc_reg_field32_modify(unit, CAPWAP_ENABLEr,
                                              REG_PORT_ANY,
                                              UDP_PORT_NUMf,
                                              arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchCountCapwapPayloadBytesOnly:
            if (soc_feature(unit, soc_feature_ing_capwap_parser)) {
                return soc_reg_field32_modify(unit, CAPWAP_ENABLEr,
                                              REG_PORT_ANY,
                                              COUNT_PAYLOAD_BYTESf,
                                              (arg) ? 1 : 0);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchEcmpGroupMemberMode:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchECMPHashBitCountSelect:
            if (soc_feature(unit, soc_feature_ecmp_hash_bit_count_select)) {
                uint32 field_val;
                /*
                 * Determine the number of bits of the 16-bit ECMP hash value:
                 * 0: used hash bits are 0-9
                 * 1: used hash bits are 0-10
                 * 2: used hash bits are 0-11
                 * 3: used hash bits are 0-12
                 * 4: used hash bits are 0-13
                 * 5: used hash bits are 0-14
                 * 6: All 16 bits are used (defaul)
                 * 7: Invalid
                 */
                field_val = (arg & 0x7);
                if (field_val == 0x7) {
                    return BCM_E_PARAM;
                }

                return soc_reg_field32_modify(
                    unit, HASH_CONTROLr, REG_PORT_ANY,
                    ECMP_HASH_FIELD_UPPER_BITS_COUNTf, field_val);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchPFCDeadlockDetectionTimeInterval:
        case bcmSwitchPFCDeadlockRecoveryAction:
        case bcmSwitchSampleFlexRandomSeed:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchFlexIP6ExtHdr:
            if (soc_feature(unit, soc_feature_l3_ip6) &&
                SOC_REG_IS_VALID(unit, FLEXIBLE_IPV6_EXT_HDRr)) {
                int max_ext_hdr_id;
                max_ext_hdr_id = (1 << soc_reg_field_length(unit,
                                  FLEXIBLE_IPV6_EXT_HDRr, PROTOCOL_IDf)) - 1;
                if ((arg < 0) || (arg > max_ext_hdr_id)) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(WRITE_FLEXIBLE_IPV6_EXT_HDRr(unit, arg));
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchFlexIP6ExtHdrEgress:
            if (soc_feature(unit, soc_feature_l3_ip6) &&
                SOC_REG_IS_VALID(unit, EGR_FLEXIBLE_IPV6_EXT_HDRr)) {
                int max_egr_ext_hdr_id;
                max_egr_ext_hdr_id = (1 << soc_reg_field_length(unit,
                                      EGR_FLEXIBLE_IPV6_EXT_HDRr, PROTOCOL_IDf)) - 1;
                if ((arg < 0) || (arg > max_egr_ext_hdr_id)) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXIBLE_IPV6_EXT_HDRr(unit, arg));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchRedirectBufferThresholdPriorityLow:
        case bcmSwitchRedirectBufferThresholdPriorityMedium:
        case bcmSwitchRedirectBufferThresholdPriorityHigh:
        case bcmSwitchL3ClassIdForL2Enable:
        case bcmSwitchAlternateStoreForward:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchRoce2UdpDstPort:
            if (soc_feature(unit, soc_feature_rroce)) {
                uint32 reg32;

                if ((arg < 0) || (arg > 0xffff)) {
                    return BCM_E_PARAM;
                } else {
                    SOC_IF_ERROR_RETURN(READ_ING_ROCE_CONTROLr(unit, &reg32));
                    if (SOC_REG_FIELD_VALID(unit, ING_ROCE_CONTROLr,
                                            UDP_DST_PORT_NUMBERf)) {
                         soc_reg_field_set(unit, ING_ROCE_CONTROLr, &reg32,
                                           UDP_DST_PORT_NUMBERf, arg);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_ING_ROCE_CONTROLr(unit, reg32));
                    return BCM_E_NONE;
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRoce1Ethertype:
            if (soc_feature(unit, soc_feature_rroce)) {
                uint32 reg32;
                /* Ethernet II protocal ethertype valid range */
                if ((arg < 0x600) || (arg > 0xffff)) {
                    return BCM_E_PARAM;
                } else {
                    SOC_IF_ERROR_RETURN(READ_ING_ROCE_CONTROLr(unit, &reg32));
                    if (SOC_REG_FIELD_VALID(unit, ING_ROCE_CONTROLr,
                                            ROCEV1_ETHERTYPEf)) {
                         soc_reg_field_set(unit, ING_ROCE_CONTROLr, &reg32,
                                           ROCEV1_ETHERTYPEf, arg);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_ING_ROCE_CONTROLr(unit, reg32));
                    return BCM_E_NONE;
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashSelectControlExtended:
            if (soc_feature(unit, soc_feature_rroce)) {
                return
                bcmi_gh2_switch_rtag7_extended_roce_selectcontrol_set(unit,
                                                                      arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchE2EFCReceiveDestMacOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                fval = (fval & 0xffffff) | (arg & 0xff) << 24;
                soc_reg_field_set(unit, E2E_IBP_RX_DA_LSr,
                                  &val, DAf, fval);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_LSr(unit, val));
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_MSr(unit, &val));
                fval = (arg >> 8) & 0xffff;
                soc_reg_field_set(unit, E2E_IBP_RX_DA_MSr,
                                  &val, DAf, fval);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_MSr(unit, val));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveDestMacNonOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                fval = (fval & 0xff000000) | (arg & 0xffffff);
                soc_reg_field_set(unit, E2E_IBP_RX_DA_LSr,
                                  &val, DAf, fval);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_LSr(unit, val));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveEtherType:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_LENGTH_TYPEr(unit, &val));
                arg = (arg & 0xffff);
                soc_reg_field_set(unit, E2E_IBP_RX_LENGTH_TYPEr,
                                  &val, LENGTH_TYPEf, arg);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_LENGTH_TYPEr(unit, val));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveOpcode:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_OPCODEr(unit, &val));
                arg = (arg & 0xffff);
                soc_reg_field_set(unit, E2E_IBP_RX_OPCODEr,
                                  &val, OPCODEf, arg);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_OPCODEr(unit, val));
                return BCM_E_NONE;
            }
            break;
#ifdef BCM_FIRELIGHT_SUPPORT
        case bcmSwitch1588ClockAddressOui:
            if (soc_feature(unit, soc_feature_pdelay_req)) {
                uint64 val64;
                uint32 val_lo, val_hi;

                if ((arg & 0xFF000000) != 0) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(READ_EGR_1588_CLOCK_ADDRESSr(unit, &val64));
                COMPILER_64_TO_32_LO(val_lo, val64);
                COMPILER_64_TO_32_HI(val_hi, val64);
                val_lo = (val_lo & 0xffffff) | ((arg & 0xff) << 24);
                val_hi = (arg >> 8) & 0xffff;
                COMPILER_64_SET(val64, val_hi, val_lo);
                SOC_IF_ERROR_RETURN(WRITE_EGR_1588_CLOCK_ADDRESSr(unit, val64));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitch1588ClockAddressNonOui:
            if (soc_feature(unit, soc_feature_pdelay_req)) {
                uint64 val64;
                uint32 val_lo, val_hi;

                if ((arg & 0xFF000000) != 0) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(READ_EGR_1588_CLOCK_ADDRESSr(unit, &val64));
                COMPILER_64_TO_32_LO(val_lo, val64);
                COMPILER_64_TO_32_HI(val_hi, val64);
                val_lo = (val_lo & 0xff000000) | (arg & 0xffffff);
                COMPILER_64_SET(val64, val_hi, val_lo);
                SOC_IF_ERROR_RETURN(WRITE_EGR_1588_CLOCK_ADDRESSr(unit, val64));
                return BCM_E_NONE;
            }
            break;
#endif
        default:
            break;
    }

    /* Iterate over all ports for port specific controls */
    found = 0;

    PBMP_E_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port)) {
            continue;
        }
        /* coverity[stack_use_callee] */
        /* coverity[stack_use_overflow] */
        rv = bcm_gh2_switch_control_port_set(unit, port, type, arg);
        if (rv == BCM_E_NONE) {
          found = 1;
        } else if (rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }

    PBMP_ST_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port) &&
            (type != bcmSwitchDirectedMirroring)) {
            continue;
        }
        rv = bcm_gh2_switch_control_port_set(unit, port, type, arg);
        if (rv == BCM_E_NONE) {
           found = 1;
        } else if (rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }

    return (found ? BCM_E_NONE : BCM_E_UNAVAIL);
}


/*
 * Function:
 *      bcm_gh2_switch_control_get
 * Description:
 *      Retrieve general switch behaviors.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      For behaviors which are port-specific, retrieves the setting
 *      for one port arbitrarily chosen from those which support the
 *      setting and are non-stack.
 */

int
bcm_gh2_switch_control_get(int unit,
                           bcm_switch_control_t type,
                           int *arg)
{
    bcm_port_t  port;
    int   rv;
    uint32 rval;
    uint64 rval64;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    /* Check for device wide, non port specific controls */
    switch (type) {
        case bcmSwitchStableSelect:
#ifdef BCM_WARM_BOOT_SUPPORT
            {
                uint32 flags;
                return soc_stable_get(unit, arg, &flags);
            }
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchStableSize:
#ifdef BCM_WARM_BOOT_SUPPORT
            return soc_stable_size_get(unit, arg);
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchStableUsed:
#ifdef BCM_WARM_BOOT_SUPPORT
            return soc_stable_used_get(unit, arg);
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchControlSync:
#ifdef BCM_WARM_BOOT_SUPPORT
            *arg = SOC_CONTROL(unit)->scache_dirty;
            return BCM_E_NONE;
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchControlAutoSync:
#ifdef BCM_WARM_BOOT_SUPPORT
            *arg = SOC_CONTROL(unit)->autosync;
            return BCM_E_NONE;
#else
            return SOC_E_UNAVAIL;
#endif
            break;
        case bcmSwitchModuleType: {
            bcm_module_t modid;
            uint32 mod_type;

            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
            BCM_IF_ERROR_RETURN(_bcm_gh2_switch_module_type_get(unit, modid,
                                                                &mod_type));
            *arg = BCM_SWITCH_CONTROL_MOD_TYPE_PACK(modid, mod_type);
            return BCM_E_NONE;
        }
        break;
#ifdef BCM_CB_ABORT_ON_ERR
        case bcmSwitchCallbackAbortOnError:
            *arg = SOC_CB_ABORT_ON_ERR(unit);
            return BCM_E_NONE;
            break;
#endif
        case bcmSwitchIpmcGroupMtu:
        case bcmSwitchLoopbackMtuSize:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchMcastFloodDefault: {
            bcm_vlan_mcast_flood_t mode;
            BCM_IF_ERROR_RETURN(_bcm_esw_vlan_flood_default_get(unit, &mode));
            *arg = mode;
            return BCM_E_NONE;
        }
        break;

        case bcmSwitchHgHdrMcastFlood: {
            bcm_vlan_mcast_flood_t mode;
            BCM_IF_ERROR_RETURN(_bcm_esw_higig_flood_l2_get(unit, &mode));
            *arg = mode;
            return BCM_E_NONE;
        }
        break;

        case bcmSwitchHgHdrIpMcastFlood: {
            return BCM_E_UNAVAIL;
        }
        break;

        case bcmSwitchUseGport:
            *arg = SOC_USE_GPORT(unit);
            return BCM_E_NONE;
            break;

        case bcmSwitchFailoverStackTrunk:
            *arg = 1;
            return BCM_E_NONE;
            break;

        case bcmSwitchFailoverEtherTrunk:
            *arg = 0;
            return BCM_E_NONE;
            break;

        case bcmSwitchMcastL2Range:
        case bcmSwitchMcastL3Range:
            return BCM_E_UNAVAIL;
            break;
        /*    coverity[equality_cond]    */

        case bcmSwitchHgHdrMcastVlanRange:
        case bcmSwitchHgHdrMcastL2Range:
        case bcmSwitchHgHdrMcastL3Range:
            {
            int bc_size, mc_size, ipmc_size;
            BCM_IF_ERROR_RETURN
                    (soc_hbx_higig2_mcast_sizes_get(unit, &bc_size,
                                                    &mc_size, &ipmc_size));
            switch (type) {
            case bcmSwitchHgHdrMcastVlanRange:
                *arg = bc_size;
                break;
            case bcmSwitchHgHdrMcastL2Range:
                *arg = mc_size;
                break;
            case bcmSwitchHgHdrMcastL3Range:
                *arg = ipmc_size;
                break;
            /* Defensive Default */
            /* coverity[dead_error_begin] */
            default:
                return BCM_E_PARAM;
            }
            }
            return BCM_E_NONE;
            break;
        case bcmSwitchHashSeed0:
            rval = 0;
            SOC_IF_ERROR_RETURN(READ_RTAG7_HASH_SEED_Ar(unit, &rval));
            *arg = soc_reg_field_get(unit, RTAG7_HASH_SEED_Ar, rval,
                                     HASH_SEED_Af);
            return BCM_E_NONE;
            break;
        case bcmSwitchHashSeed1:
            rval = 0;
            SOC_IF_ERROR_RETURN(READ_RTAG7_HASH_SEED_Br(unit, &rval));
            *arg = soc_reg_field_get(unit, RTAG7_HASH_SEED_Br, rval,
                                     HASH_SEED_Bf);
            return BCM_E_NONE;
            break;
        case bcmSwitchMacroFlowHashMinOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMinOffset:
        case bcmSwitchMacroFlowTrunkHashMinOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_macroflow_param_get(unit, type,
                                                                NULL, arg,
                                                                NULL, NULL,
                                                                NULL);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMacroFlowHashMaxOffset:
        case bcmSwitchMacroFlowHigigTrunkHashMaxOffset:
        case bcmSwitchMacroFlowTrunkHashMaxOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_macroflow_param_get(unit, type,
                                                                NULL, NULL,
                                                                arg, NULL,
                                                                NULL);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMacroFlowHashStrideOffset:
        case bcmSwitchMacroFlowHigigTrunkHashStrideOffset:
        case bcmSwitchMacroFlowTrunkHashStrideOffset:
        case bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_macroflow_param_get(unit, type,
                                                                NULL, NULL,
                                                                NULL, arg,
                                                                NULL);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMacroFlowHashOverlayMinOffset:
        case bcmSwitchMacroFlowHashOverlayMaxOffset:
        case bcmSwitchMacroFlowHashOverlayStrideOffset:
        case bcmSwitchMacroFlowEcmpHashOverlayConcatEnable:
            return BCM_E_UNAVAIL;
            break;
        /* RTAG7 Macro Flow Concatenation */
        case bcmSwitchMacroFlowEcmpHashConcatEnable:
        case bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable:
        case bcmSwitchMacroFlowLoadBalanceHashConcatEnable:
        case bcmSwitchMacroFlowTrunkHashConcatEnable:
        case bcmSwitchMacroFlowHigigTrunkHashConcatEnable:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_macroflow_param_get(unit, type,
                                                                NULL, NULL,
                                                                NULL, NULL,
                                                                arg);
            }
            return BCM_E_UNAVAIL;
            break;
        /* RTAG7 Macro Flow Hash Seed */
        case bcmSwitchMacroFlowECMPHashSeed:
        case bcmSwitchMacroFlowECMPUnderlayHashSeed:
        case bcmSwitchMacroFlowLoadBalanceHashSeed:
        case bcmSwitchMacroFlowTrunkHashSeed:
        case bcmSwitchMacroFlowHigigTrunkHashSeed:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return BCM_E_PARAM;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchMacroFlowECMPOverlayHashSeed:
        /* Hash Field select controls */
        case bcmSwitchHashTrillPayloadSelect0:
        case bcmSwitchHashTrillPayloadSelect1:
        case bcmSwitchHashTrillTunnelSelect0:
        case bcmSwitchHashTrillTunnelSelect1:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashVxlanPayloadSelect0:
        case bcmSwitchHashVxlanPayloadSelect1:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_field_control_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashIP6AddrCollapseSelect0:
        case bcmSwitchHashIP6AddrCollapseSelect1:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_field_control_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashIP6NextHeaderUseExtensionHeader0:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                COMPILER_64_ZERO(rval64);
                BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &rval64));
                rval = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rval64,
                                             DISABLE_HASH_IP_EXTENSION_HEADERS_Af);
                if (rval) {
                    *arg = FALSE;
                } else {
                    *arg = TRUE;
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashIP6NextHeaderUseExtensionHeader1:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                COMPILER_64_ZERO(rval64);
                BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &rval64));
                rval = soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rval64,
                                             DISABLE_HASH_IP_EXTENSION_HEADERS_Bf);
                if (rval) {
                    *arg = FALSE;
                } else {
                    *arg = TRUE;
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL2MPLSPayloadSelect0:
        case bcmSwitchHashL2MPLSPayloadSelect1:
        case bcmSwitchHashIPSecSelect0:
        case bcmSwitchHashIPSecSelect1:
        case bcmSwitchHashMiMPayloadSelect0:
        case bcmSwitchHashMiMPayloadSelect1:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchL2McastAllRouterPortsAddEnable:
            *arg = SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit);
            return BCM_E_NONE;
            break;

        case bcmSwitchBypassMode:
            *arg = SOC_SWITCH_BYPASS_MODE(unit);
            return BCM_E_NONE;
            break;

        case bcmSwitchPFCClass0Queue:
        case bcmSwitchPFCClass1Queue:
        case bcmSwitchPFCClass2Queue:
        case bcmSwitchPFCClass3Queue:
        case bcmSwitchPFCClass4Queue:
        case bcmSwitchPFCClass5Queue:
        case bcmSwitchPFCClass6Queue:
        case bcmSwitchPFCClass7Queue:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchL2HitClear:
        case bcmSwitchL2SrcHitClear:
        case bcmSwitchL2DstHitClear:
        case bcmSwitchL3HostHitClear:
        case bcmSwitchL3RouteHitClear:
            return _bcm_gh2_switch_hit_clear_get(unit, type, arg);
            break;

        /* This swithc control should be supported only per port. */
        case bcmSwitchEncapErrorToCpu:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchSnapNonZeroOui:
            rval = 0;

            COMPILER_64_ZERO(rval64);
            SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
            *arg = soc_reg64_field32_get(unit, ING_CONFIG_64r, rval64,
                                         SNAP_OTHER_DECODE_ENABLEf);
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
            *arg = soc_reg_field_get(unit, EGR_CONFIGr, rval,
                                     SNAP_OTHER_DECODE_ENABLEf);
            return BCM_E_NONE;
            break;

        case bcmSwitchL2McIdxRetType:
            return _bcm_esw_mcast_idx_ret_type_get(unit, arg);
            break;

        case bcmSwitchL2ExtLearn:
            return BCM_E_NONE;
            break;

        case bcmSwitchL3McIdxRetType:
#ifdef INCLUDE_L3
            return _bcm_esw_ipmc_idx_ret_type_get(unit, arg);
#endif
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchL2SourceDiscardMoveToCpu:
            {
                soc_reg_t   reg;
                uint64      regval, f_val;
                reg = ING_CONFIG_64r;

                    BCM_IF_ERROR_RETURN(
                        soc_reg_get(unit, reg, REG_PORT_ANY, 0, &regval));
                    f_val = soc_reg64_field_get(unit, reg, regval,
                                                STNMOVE_ON_L2SRC_DISCf);
                    COMPILER_64_TO_32_LO(*arg, f_val);
                    return BCM_E_NONE;
            }
            break;

        case bcmSwitchFabricTrunkDynamicSampleRate:
        case bcmSwitchFabricTrunkDynamicEgressBytesExponent:
        case bcmSwitchFabricTrunkDynamicQueuedBytesExponent:
        case bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset:
        case bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold:
        case bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold:
        case bcmSwitchTrunkDynamicSampleRate:
        case bcmSwitchTrunkDynamicAccountingSelect:
        case bcmSwitchTrunkDynamicEgressBytesExponent:
        case bcmSwitchTrunkDynamicQueuedBytesExponent:
        case bcmSwitchTrunkDynamicEgressBytesDecreaseReset:
        case bcmSwitchTrunkDynamicQueuedBytesDecreaseReset:
        case bcmSwitchTrunkDynamicEgressBytesMinThreshold:
        case bcmSwitchTrunkDynamicEgressBytesMaxThreshold:
        case bcmSwitchTrunkDynamicQueuedBytesMinThreshold:
        case bcmSwitchTrunkDynamicQueuedBytesMaxThreshold:
        case bcmSwitchTrunkDynamicExpectedLoadMinThreshold:
        case bcmSwitchTrunkDynamicExpectedLoadMaxThreshold:
        case bcmSwitchTrunkDynamicImbalanceMinThreshold:
        case bcmSwitchTrunkDynamicImbalanceMaxThreshold:
        case bcmSwitchEcmpDynamicSampleRate:
        case bcmSwitchEcmpDynamicAccountingSelect:
        case bcmSwitchEcmpDynamicEgressBytesExponent:
        case bcmSwitchEcmpDynamicQueuedBytesExponent:
        case bcmSwitchEcmpDynamicEgressBytesDecreaseReset:
        case bcmSwitchEcmpDynamicQueuedBytesDecreaseReset:
        case bcmSwitchEcmpDynamicEgressBytesMinThreshold:
        case bcmSwitchEcmpDynamicEgressBytesMaxThreshold:
        case bcmSwitchEcmpDynamicQueuedBytesMinThreshold:
        case bcmSwitchEcmpDynamicQueuedBytesMaxThreshold:
        case bcmSwitchEcmpDynamicExpectedLoadMinThreshold:
        case bcmSwitchEcmpDynamicExpectedLoadMaxThreshold:
        case bcmSwitchEcmpDynamicImbalanceMinThreshold:
        case bcmSwitchEcmpDynamicImbalanceMaxThreshold:
            return BCM_E_UNAVAIL;
            break;

#if (defined(BCM_RCPU_SUPPORT) || defined(BCM_OOB_RCPU_SUPPORT)) && \
     defined(BCM_XGS3_SWITCH_SUPPORT)
        case bcmSwitchRemoteCpuSchanEnable:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                rval = 0;

                SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                         ENABLE_SCHAN_REQUESTf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuMatchLocalMac:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                rval = 0;

                SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                         LMAC0_MATCHf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuFromCpuEnable:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                rval = 0;

                SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                         ENABLE_FROMCPU_PACKETf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuToCpuEnable:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                rval = 0;

                SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                         ENABLE_TOCPU_PACKETf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuCmicEnable:
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                rval = 0;

                SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                         ENABLE_CMIC_REQUESTf);
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuMatchVlan:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                rval = 0;

                SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                         VLAN_MATCHf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuForceScheduling:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                int l_arg, h_arg;

                l_arg = h_arg = 0;
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    if (SOC_REG_IS_VALID(unit, CMIC_PKT_COS_0r)) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_gh2_rcpu_switch_regall_get(unit, CMIC_PKT_COS_0r,
                                                            &l_arg));
                    }
                    if (SOC_REG_IS_VALID(unit, CMIC_PKT_COS_1r)) {
                        BCM_IF_ERROR_RETURN(
                          _bcm_gh2_rcpu_switch_regall_get(unit, CMIC_PKT_COS_1r, &h_arg));
                    }
                } else
#endif
                {
                    BCM_IF_ERROR_RETURN(
                        _bcm_gh2_rcpu_switch_regall_get(unit, CMIC_PKT_COSr, &l_arg));
                    if (SOC_REG_IS_VALID(unit, CMIC_PKT_COS_HIr)) {
                        BCM_IF_ERROR_RETURN(
                          _bcm_gh2_rcpu_switch_regall_get(unit, CMIC_PKT_COS_HIr, &h_arg));
                    }
                }
                *arg = l_arg | h_arg;

                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuToCpuDestPortAllReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                int l_arg, h_arg;

                l_arg = h_arg = 0;
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)){
                    /* In Katana only Index 0 of the reasoning code register is used */
                    BCM_IF_ERROR_RETURN(
                        _bcm_gh2_rcpu_switch_regall_idx_get(unit, CMIC_PKT_REASON_DIRECT_0_TYPEr, 0,
                                                            &l_arg));
                    if (SOC_REG_IS_VALID(unit, CMIC_PKT_REASON_DIRECT_1_TYPEr)) {
                        BCM_IF_ERROR_RETURN(
                            _bcm_gh2_rcpu_switch_regall_idx_get(unit,
                                                                CMIC_PKT_REASON_DIRECT_1_TYPEr, 0,
                                                                &h_arg));
                    }
                }
#endif
                *arg = l_arg | h_arg;
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuToCpuDestMacAllReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                int l_arg, h_arg;

                l_arg = h_arg = 0;
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    /* In Katana only Index 0 of the reasoning code register is used */
                    BCM_IF_ERROR_RETURN(
                        _bcm_gh2_rcpu_switch_regall_idx_get(unit, CMIC_PKT_REASON_0_TYPEr, 0,
                                                            &l_arg));
                    if (SOC_REG_IS_VALID(unit, CMIC_PKT_REASON_1_TYPEr)) {
                        BCM_IF_ERROR_RETURN(
                          _bcm_gh2_rcpu_switch_regall_idx_get(unit,
                                                              CMIC_PKT_REASON_1_TYPEr, 0, &h_arg));
                    }
                }
#endif
                *arg = l_arg | h_arg;
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectPktReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                uint32 raddr;
                rval = 0;
                if (soc_feature(unit, soc_feature_cmicm)) {
                    /* Only Idx 0 is used in Katana */
                    raddr = soc_reg_addr(unit, CMIC_PKT_REASON_0_TYPEr, REG_PORT_ANY, 0);
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                }
                *arg = rval;
                return BCM_E_NONE;
#endif
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectPktReasonsExtended:
                if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                    if (soc_feature(unit, soc_feature_cmicm)) {
                        uint32 raddr;
                        rval = 0;
                        raddr = soc_reg_addr(unit, CMIC_PKT_REASON_1_TYPEr, REG_PORT_ANY, 0);
                        /* coverity[result_independent_of_operands] */
                        SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                        *arg = rval;
                        return BCM_E_NONE;
                    }
#endif
                }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchRxRedirectHigigPktReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                uint32 raddr;
                rval = 0;
                if (soc_feature(unit, soc_feature_cmicm)) {
                    raddr = soc_reg_addr(unit, CMIC_PKT_REASON_DIRECT_0_TYPEr, REG_PORT_ANY, 0);
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                }
                *arg = rval;
                return BCM_E_NONE;
#endif
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectHigigPktReasonsExtended:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    uint32 raddr;
                    rval = 0;
                    raddr = soc_reg_addr(unit, CMIC_PKT_REASON_DIRECT_1_TYPEr, REG_PORT_ANY, 0);
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                    *arg = rval;
                    return BCM_E_NONE;
                }
#endif
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchRxRedirectTruncatedPktReasons:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                uint32 raddr;
                rval = 0;
                if (soc_feature(unit, soc_feature_cmicm)) {
                    raddr = soc_reg_addr(unit, CMIC_PKT_REASON_MINI_0_TYPEr, REG_PORT_ANY, 0);
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                }
                *arg = rval;
                return BCM_E_NONE;
#endif
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectTruncatedPktReasonsExtended:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    uint32 raddr;
                    rval = 0;
                    raddr = soc_reg_addr(unit, CMIC_PKT_REASON_MINI_1_TYPEr, REG_PORT_ANY, 0);
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                    *arg = rval;
                    return BCM_E_NONE;
                }
#endif
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchRxRedirectPktCos:
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm) &&
                SOC_REG_IS_VALID(unit, CMIC_PKT_COS_QUEUES_LOr)) {
                uint32 raddr;
                rval = 0;
                raddr = soc_reg_addr(unit, CMIC_PKT_COS_QUEUES_LOr, REG_PORT_ANY, 0);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                *arg = rval;
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRxRedirectPktCosExtended:
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm) &&
                SOC_REG_IS_VALID(unit, CMIC_PKT_COS_QUEUES_HIr)) {
                uint32 raddr;
                rval = 0;
                raddr = soc_reg_addr(unit, CMIC_PKT_COS_QUEUES_HIr, REG_PORT_ANY, 0);
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, raddr, &rval));
                *arg = rval;
                return BCM_E_NONE;
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuLocalMacOui:
        case bcmSwitchRemoteCpuDestMacOui:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_switch_mac_hi_get(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuTcMappingMacOui:
            if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
                uint32 fieldval, regval;
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_DA_LSr(unit, &regval));
                fieldval = soc_reg_field_get(unit, REMOTE_CPU_DA_LSr, regval,
                                             DAf);
                *arg = fieldval >> 24;
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_DA_MSr(unit, &regval));
                fieldval = soc_reg_field_get(unit, REMOTE_CPU_DA_MSr, regval,
                                             DAf);
                *arg |= (fieldval << 8 );
 return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuLocalMacNonOui:
        case bcmSwitchRemoteCpuDestMacNonOui:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
           return _bcm_gh2_rcpu_switch_mac_lo_get(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuTcMappingMacNonOui:
            if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
                uint32 fieldval, regval;
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_DA_LSr(unit, &regval));
                fieldval = soc_reg_field_get(unit, REMOTE_CPU_DA_LSr, regval,
                                             DAf);
                *arg  = ((fieldval << 8) >> 8);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuEthertype:
        case bcmSwitchRemoteCpuVlan:
        case bcmSwitchRemoteCpuTpid:
        case bcmSwitchRemoteCpuSignature:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_switch_vlan_tpid_sig_get(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuTcMappingEthertype:
            if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
                uint32 regval;
                BCM_IF_ERROR_RETURN(
                    READ_REMOTE_CPU_LENGTH_TYPEr(unit, &regval));
                *arg = soc_reg_field_get(unit, REMOTE_CPU_LENGTH_TYPEr, regval,
                                         LENGTH_TYPEf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuDestPort:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_pipe_bypass_header_get(unit, type, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRemoteCpuHigigDestPort:
            if (soc_feature(unit, soc_feature_rcpu_1)) {
                return _bcm_gh2_rcpu_higig_header_get(unit, arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#endif
        case bcmSwitchServiceTpidReplace:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchCongestionCntag:
        case bcmSwitchCongestionCntagEthertype:
        case bcmSwitchCongestionCnm:
        case bcmSwitchCongestionCnmEthertype:
        case bcmSwitchCongestionNotificationIdHigh:
        case bcmSwitchCongestionNotificationIdQueue:
        case bcmSwitchCongestionUseOuterTpid:
        case bcmSwitchCongestionUseOuterVlan:
        case bcmSwitchCongestionUseOuterPktPri:
        case bcmSwitchCongestionUseOuterCfi:
        case bcmSwitchCongestionUseInnerPktPri:
        case bcmSwitchCongestionUseInnerCfi:
        case bcmSwitchCongestionMissingCntag:
        case bcmSwitchCongestionCnmSrcMacNonOui:
        case bcmSwitchCongestionCnmSrcMacOui:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchLoadBalanceHashSelect:
            {
                soc_reg_t   reg;

                COMPILER_64_ZERO(rval64);
                reg = ING_CONFIG_64r;
                BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0,
                                    &rval64));
                *arg = soc_reg64_field32_get(unit, reg, rval64, LBID_RTAGf);
                return BCM_E_NONE;
            }
            break;

        case bcmSwitchTrillEthertype:
        case bcmSwitchTrillISISEthertype:
        case bcmSwitchTrillISISDestMacOui:
        case bcmSwitchTrillBroadcastDestMacOui:
        case bcmSwitchTrillEndStationDestMacOui:
        case bcmSwitchTrillISISDestMacNonOui:
        case bcmSwitchTrillBroadcastDestMacNonOui:
        case bcmSwitchTrillEndStationDestMacNonOui:
        case bcmSwitchTrillMinTtl:
        case bcmSwitchTrillTtlCheckEnable:
        case bcmSwitchFcoeEtherType:
            return BCM_E_UNAVAIL;
            break;

#if defined(BCM_FIREBOLT_SUPPORT)
#if defined(INCLUDE_L3)
        case bcmSwitchL3TunnelIpV4ModeOnly:
             return soc_tunnel_term_block_size_get (unit, arg);
             break;
        case bcmSwitchL3SrcHitEnable:
            {
                soc_reg_t   reg;

                COMPILER_64_ZERO(rval64);
                reg = ING_CONFIG_64r;

                BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                *arg = soc_reg64_field32_get(unit, reg, rval64, L3SRC_HIT_ENABLEf);
                return BCM_E_NONE;
            }
            break;
#endif /* INCLUDE_L3*/
#endif /* BCM_FIREBOLT_SUPPORT */
        case bcmSwitchL2DstHitEnable:
            {
                soc_reg_t   reg;

                COMPILER_64_ZERO(rval64);
                reg = ING_CONFIG_64r;

                BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                *arg = soc_reg64_field32_get(unit, reg, rval64, L2DST_HIT_ENABLEf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchHashDualMoveDepth:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthL2:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX_L2X(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthMpls:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX_MPLS(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashDualMoveDepthVlan:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX_VLAN(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchHashDualMoveDepthEgressVlan:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX_EGRESS_VLAN(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchHashDualMoveDepthWlanPort:
        case bcmSwitchHashDualMoveDepthWlanClient:
        case bcmSwitchL2GreProtocolType:
        case bcmSwitchL2GreVpnIdSizeSet:
        case bcmSwitchRemoteEncapsulationMode:
            return BCM_E_UNAVAIL;
            break;
#if defined(INCLUDE_L3)
        case bcmSwitchVxlanUdpDestPortSet:
        case bcmSwitchVxlanUdpDestPortSet1:
            /* Get UDP Dest port for VXLAN */
            if (soc_feature(unit, soc_feature_vxlan_lite)) {
                  return bcmi_gh2_vxlan_udp_dest_port_get(unit, type, arg);
            } else {
                  return BCM_E_UNAVAIL;
            }
          break;

        case bcmSwitchVxlanEntropyEnable:
        case bcmSwitchVxlanEntropyEnableIP6:
            /* Enable UDP Source Port Hash for VXLAN */
            if (soc_feature(unit, soc_feature_vxlan_lite)) {
                  return bcmi_gh2_vxlan_udp_source_port_get(unit, type, arg);
            } else {
                  return BCM_E_UNAVAIL;
            }
          break;
        case bcmSwitchHashDualMoveDepthL3:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX_L3X(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashL3DualLeastFull:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                rval = 0;
                BCM_IF_ERROR_RETURN(READ_L3_AUX_HASH_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr,
                                         rval, INSERT_LEAST_FULL_HALFf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
#endif
        case bcmSwitchHashMultiMoveDepth:
        case bcmSwitchHashMultiMoveDepthL2:
        case bcmSwitchHashMultiMoveDepthL3:
        case bcmSwitchHashMultiMoveDepthMpls:
        case bcmSwitchHashMultiMoveDepthVlan:
        case bcmSwitchHashMultiMoveDepthEgressVlan:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchHashL2DualLeastFull:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                rval = 0;
                BCM_IF_ERROR_RETURN(READ_L2_AUX_HASH_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                         rval, INSERT_LEAST_FULL_HALFf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchHashMPLSDualLeastFull:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchSynchronousPortClockSource:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_sync_port_select_get(unit, 1, (uint32 *)arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmSwitchSynchronousPortClockSourceBkup:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_sync_port_select_get(unit, 0, (uint32 *)arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmSwitchSynchronousPortClockSourceDivCtrl:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                 return _bcm_gh2_switch_div_ctrl_select_get(unit, 1, (uint32 *)arg);
            } else {
                return BCM_E_UNAVAIL;
           }
           break;

        case bcmSwitchSynchronousPortClockSourceBkupDivCtrl:
            if (soc_feature(unit, soc_feature_gmii_clkout)) {
                return _bcm_gh2_switch_div_ctrl_select_get(unit, 0, (uint32 *)arg);
            } else {
                return BCM_E_UNAVAIL;
            }
           break;

#if defined(INCLUDE_L3)
        case bcmSwitchNivEthertype:
             if (soc_feature(unit, soc_feature_niv)) {
                 return bcm_gh_niv_ethertype_get(unit, arg);
             }
             return BCM_E_UNAVAIL;
             break;
#endif /*  INCLUDE_L3 */
        case bcmSwitchSubportPktTagEthertype:
            return BCM_E_UNAVAIL;
            break;

#if defined(INCLUDE_L3)
        case bcmSwitchEtagEthertype:
            if (soc_feature(unit, soc_feature_port_extension)) {
                return bcm_gh_etag_ethertype_get(unit, arg);
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchExtenderMulticastLowerThreshold:
             if (soc_feature(unit, soc_feature_port_extension)) {
                 uint32 etag_range = 0;
                 SOC_IF_ERROR_RETURN(READ_ETAG_MULTICAST_RANGEr(unit, &etag_range));
                 *arg = soc_reg_field_get(unit, ETAG_MULTICAST_RANGEr, etag_range,
                                          ETAG_VID_MULTICAST_LOWERf);
                 return BCM_E_NONE;
             }
             return BCM_E_UNAVAIL;
             break;
#endif /* INCLUDE_L3 */

        case bcmSwitchExtenderMulticastHigherThreshold:
             if (soc_feature(unit, soc_feature_port_extension)) {
                 uint32 etag_range = 0;
                 SOC_IF_ERROR_RETURN(READ_ETAG_MULTICAST_RANGEr(unit, &etag_range));
                 *arg = soc_reg_field_get(unit, ETAG_MULTICAST_RANGEr, etag_range,
                                          ETAG_VID_MULTICAST_UPPERf);
                 return BCM_E_NONE;
             }
             return BCM_E_UNAVAIL;
             break;

        case bcmSwitchWESPProtocolEnable:
        case bcmSwitchWESPProtocol:
        case bcmSwitchIp6CompressEnable:
        case bcmSwitchIp6CompressDefaultOffset:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchMirrorPktChecksEnable:
            rval = 0;
            BCM_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
            if (soc_reg_field_get(unit, EGR_CONFIG_1r, rval,
                                  DISABLE_MIRROR_CHECKSf)) {
                *arg = 0;
            } else {
                *arg = 1;
            }
            return BCM_E_NONE;
            break;

        case bcmSwitchStableSaveLongIds:
#if defined(BCM_FIELD_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
            {
                _field_control_t *fc;
                BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
                *arg = ((fc->flags & _FP_STABLE_SAVE_LONG_IDS) != 0);
            }
            return (BCM_E_NONE);
#else
            return (BCM_E_UNAVAIL);
#endif
            break;

        case bcmSwitchGportAnyDefaultL2Learn:
        case bcmSwitchGportAnyDefaultL2Move:
        case bcmSwitchSetMplsEntropyLabelTtl:
        case bcmSwitchSetMplsEntropyLabelPri:
        case bcmSwitchSetMplsEntropyLabelOffset:
            return BCM_E_UNAVAIL;
            break;

#if defined(INCLUDE_L3)
        case bcmSwitchMultipathCompress:
            if (soc_feature(unit, soc_feature_l3) &&
                !soc_feature(unit, soc_feature_l3_no_ecmp)) {
                *arg = TRUE;
                return BCM_E_NONE;
            }
            break;

        case bcmSwitchMultipathCompressBuffer:
            if (soc_feature(unit, soc_feature_l3) &&
                !soc_feature(unit, soc_feature_l3_no_ecmp)) {
                return bcm_tr2_l3_ecmp_defragment_buffer_get(unit, arg);
            }
            break;
#endif /* INCLUDE_L3 */

        case bcmSwitchBstEnable:
        case bcmSwitchBstTrackingMode:
        case bcmSwitchBstSnapshotEnable:
        case bcmSwitchFabricTrunkAutoIncludeDisable:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchL2OverflowEvent:
            if (soc_feature(unit, soc_feature_l2_overflow)) {
                *arg = SOC_CONTROL(unit)->l2_overflow_enable ? 1 : 0;
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchMiMDefaultSVPValue:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchOamCcmToCpu:
            if (soc_feature(unit, soc_feature_oam)) {
                rval = 0;
                SOC_IF_ERROR_RETURN(READ_CCM_COPYTO_CPU_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CCM_COPYTO_CPU_CONTROLr, rval,
                                         ERROR_CCM_COPY_TOCPUf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchOamXconCcmToCpu:
            if (soc_feature(unit, soc_feature_oam)) {
                rval = 0;
                SOC_IF_ERROR_RETURN(READ_CCM_COPYTO_CPU_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CCM_COPYTO_CPU_CONTROLr, rval,
                                         XCON_CCM_COPY_TOCPUf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchOamXconOtherToCpu:
            if (soc_feature(unit, soc_feature_oam)) {
                rval = 0;
                SOC_IF_ERROR_RETURN(READ_CCM_COPYTO_CPU_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, CCM_COPYTO_CPU_CONTROLr, rval,
                                         XCON_OTHER_COPY_TOCPUf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchOamVersionCheckDisable:
        case bcmSwitchOamOlpChipEtherType:
        case bcmSwitchOamOlpChipTpid:
        case bcmSwitchOamOlpChipVlan:
        case bcmSwitchOamOlpStationMacNonOui:
        case bcmSwitchOamOlpStationMacOui:
        case bcmSwitchOamStackingSupport:
        case bcmSwitchFieldStageEgressToCpu:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchSystemReservedVlan:
            return _bcm_esw_vlan_system_reserved_get(unit, arg);

        case bcmSwitchHashOamEgress:
        case bcmSwitchHashOamEgressDual:
        case bcmSwitchHashOam:
        case bcmSwitchHashOamDual:
        case bcmSwitchHashOamLeastFull:
        case bcmSwitchHashDualMoveDepthOam:
            /* Not supported for now */
        case bcmSwitchWredForAllPkts:
        case bcmSwitchFcoeNpvModeEnable:
        case bcmSwitchFcoeDomainRoutePrefixLength:
        case bcmSwitchFcoeCutThroughEnable:
        case bcmSwitchFcoeSourceBindCheckAction:
        case bcmSwitchFcoeSourceFpmaPrefixCheckAction:
        case bcmSwitchFcoeVftHopCountExpiryToCpu:
        case bcmSwitchFcoeVftHopCountExpiryAction:
        case bcmSwitchFcoeFcEofT1Value:
        case bcmSwitchFcoeFcEofT2Value:
        case bcmSwitchFcoeFcEofA1Value:
        case bcmSwitchFcoeFcEofA2Value:
        case bcmSwitchFcoeFcEofN1Value:
        case bcmSwitchFcoeFcEofN2Value:
        case bcmSwitchFcoeFcEofNI1Value:
        case bcmSwitchFcoeFcEofNI2Value:
        case bcmSwitchFcoeZoneCheckFailToCpu:
        case bcmSwitchFcoeZoneCheckMissDrop:
        case bcmSwitchNetworkGroupDepth:
        case bcmSwitchUnknownSubportPktTagToCpu:
        case bcmSwitchMirrorUnicastCosq:
        case bcmSwitchMirrorMulticastCosq:
        case bcmSwitchL3Max128BV6Entries:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchEEEQueueThresholdProfile0:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;

                SOC_IF_ERROR_RETURN(READ_EEE_QUEUE_THRESH_0r(unit, &rval));
                *arg = soc_reg_field_get(unit, EEE_QUEUE_THRESH_0r, rval,
                                         EEE_QUEUE_THRESHOLDf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEQueueThresholdProfile1:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;

                SOC_IF_ERROR_RETURN(READ_EEE_QUEUE_THRESH_1r(unit, &rval));
                *arg = soc_reg_field_get(unit, EEE_QUEUE_THRESH_1r, rval,
                                         EEE_QUEUE_THRESHOLDf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEQueueThresholdProfile2:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;

                SOC_IF_ERROR_RETURN(READ_EEE_QUEUE_THRESH_2r(unit, &rval));
                *arg = soc_reg_field_get(unit, EEE_QUEUE_THRESH_2r, rval,
                                         EEE_QUEUE_THRESHOLDf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEQueueThresholdProfile3:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;

                SOC_IF_ERROR_RETURN(READ_EEE_QUEUE_THRESH_3r(unit, &rval));
                *arg = soc_reg_field_get(unit, EEE_QUEUE_THRESH_3r, rval,
                                         EEE_QUEUE_THRESHOLDf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile0:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;
                uint32 fval;

                SOC_IF_ERROR_RETURN(READ_EEE_MAX_PKT_LAT_0r(unit, &rval));
                fval = soc_reg_field_get(unit, EEE_MAX_PKT_LAT_0r, rval,
                                         EEE_MAX_PKT_LATENCYf);
                *arg = fval;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile1:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;
                uint32 fval;

                SOC_IF_ERROR_RETURN(READ_EEE_MAX_PKT_LAT_1r(unit, &rval));
                fval = soc_reg_field_get(unit, EEE_MAX_PKT_LAT_1r, rval,
                                         EEE_MAX_PKT_LATENCYf);
                *arg = fval;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile2:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;
                uint32 fval;

                SOC_IF_ERROR_RETURN(READ_EEE_MAX_PKT_LAT_2r(unit, &rval));
                fval = soc_reg_field_get(unit, EEE_MAX_PKT_LAT_2r, rval,
                                         EEE_MAX_PKT_LATENCYf);
                *arg = fval;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEPacketLatencyProfile3:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;
                uint32 fval;

                SOC_IF_ERROR_RETURN(READ_EEE_MAX_PKT_LAT_3r(unit, &rval));
                fval = soc_reg_field_get(unit, EEE_MAX_PKT_LAT_3r, rval,
                                         EEE_MAX_PKT_LATENCYf);
                *arg = fval;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEEEGlobalCongestionThreshold:
            if (soc_feature(unit, soc_feature_eee_bb_mode)) {
                uint32 rval;

                SOC_IF_ERROR_RETURN(READ_EEE_GLOBAL_BUF_THRESHr(unit, &rval));
                *arg = soc_reg_field_get(unit, EEE_GLOBAL_BUF_THRESHr, rval,
                                         EEE_GLOBAL_BUFFER_THREHSOLDf);
                return BCM_E_NONE;
            }
            break;

        case bcmSwitchStgInvalidToCpu:
        case bcmSwitchVlanTranslateEgressMissToCpu:
        case bcmSwitchL3PktErrToCpu:
        case bcmSwitchMtuFailureToCpu:
        case bcmSwitchSrcKnockoutToCpu:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchSymmetricHashControl:
            if (soc_feature(unit, soc_feature_gh2_rtag7)) {
                return bcm_gh2_switch_rtag7_symmetric_hash_control_get(unit, arg);
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchHigigCongestionClassEnable:
            if (soc_feature(unit, soc_feature_ecn_wred)) {
                COMPILER_64_ZERO(rval64);
                BCM_IF_ERROR_RETURN(
                    READ_ING_CONFIG_64r(unit, &rval64));
                *arg = soc_reg64_field32_get(unit, ING_CONFIG_64r,
                                             rval64, HIGIG2_ECN_IN_CC_ENABLEf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEcnNonIpResponsive:
            if (soc_feature(unit, soc_feature_ecn_wred)) {
                BCM_IF_ERROR_RETURN(
                    READ_ECN_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, ECN_CONTROLr,
                                         rval, RESPONSIVE_DEFAULTf);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchEcnNonIpIntCongestionNotification:
            if (soc_feature(unit, soc_feature_ecn_wred)) {
                uint32 responsive;
                soc_field_t fld_idx;
                BCM_IF_ERROR_RETURN(
                    READ_ECN_CONTROLr(unit, &rval));
                responsive = soc_reg_field_get(unit, ECN_CONTROLr,
                                               rval, RESPONSIVE_DEFAULTf);
                if (responsive) {
                    fld_idx = RESPONSIVE_DEFAULT_INT_CNf;
                } else {
                    fld_idx = NON_RESPONSIVE_DEFAULT_INT_CNf;
                }
                *arg = soc_reg_field_get(unit, ECN_CONTROLr,
                                         rval, fld_idx);
                return BCM_E_NONE;
            }
            break;

        case bcmSwitchL3RouteCache:
        case bcmSwitchECMPLevel1RandomSeed:
        case bcmSwitchECMPLevel2RandomSeed:
        case bcmSwitchSynchronousPortClockSourceValid:
        case bcmSwitchSynchronousPortClockSourceBkupValid:
        case bcmSwitchSubportCoEEtherType:
        case bcmSwitchSubportEgressTpid:
        case bcmSwitchSubportEgressWideTpid:
        case bcmSwitchIpmcSameVlanPruningOverride:
        case bcmSwitchMimBvidInsertionControl:
            return BCM_E_UNAVAIL;
            break;

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
        case bcmSwitchL2CpuDeleteEvent:
            return _soc_l2mod_sbus_fifo_field_get(unit, L2_MOD_FIFO_ENABLE_L2_DELETEf, arg);
            break;

        case bcmSwitchL2LearnEvent:
            return _soc_l2mod_sbus_fifo_field_get(unit, L2_MOD_FIFO_ENABLE_LEARNf, arg);
            break;

        case bcmSwitchL2CpuAddEvent:
            return _soc_l2mod_sbus_fifo_field_get(unit, L2_MOD_FIFO_ENABLE_L2_INSERTf, arg);
            break;

        case bcmSwitchL2AgingEvent:
            return _soc_l2mod_sbus_fifo_field_get(unit, L2_MOD_FIFO_ENABLE_AGEf, arg);
            break;
#endif
        case bcmSwitchUdfHashEnable:
        case bcmSwitchOlpMatchRule:
        case bcmSwitchNonUcVlanShapingEnable:
        case bcmSwitchRangeCheckersAPIType:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchCustomHeaderEncapPriorityOffset:
            if (soc_feature(unit, soc_feature_custom_header)) {
                uint32 rval;
                SOC_IF_ERROR_RETURN
                    (READ_CUSTOM_PACKET_HEADER_OVERRIDEr(unit, &rval));

                /* The offset is 4-bit aligned */
                *arg = soc_reg_field_get(unit, CUSTOM_PACKET_HEADER_OVERRIDEr, rval,
                                         OFFSETf) * 4;

                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchIngCapwapEnable:
            if (soc_feature(unit, soc_feature_ing_capwap_parser)) {
                BCM_IF_ERROR_RETURN(
                    READ_CAPWAP_ENABLEr(unit, &rval));
                *arg = soc_reg_field_get(unit, CAPWAP_ENABLEr,
                                         rval, ENABLEf);
                 return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchCapwapDataUdpDstPort:
            if (soc_feature(unit, soc_feature_ing_capwap_parser)) {
                BCM_IF_ERROR_RETURN(
                    READ_CAPWAP_ENABLEr(unit, &rval));
                *arg = soc_reg_field_get(unit, CAPWAP_ENABLEr,
                                         rval, UDP_PORT_NUMf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchCountCapwapPayloadBytesOnly:
            if (soc_feature(unit, soc_feature_ing_capwap_parser)) {
                BCM_IF_ERROR_RETURN(
                    READ_CAPWAP_ENABLEr(unit, &rval));
                *arg = soc_reg_field_get(unit, CAPWAP_ENABLEr,
                                         rval, COUNT_PAYLOAD_BYTESf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchEcmpGroupMemberMode:
            return BCM_E_UNAVAIL;
            break;
        case bcmSwitchECMPHashBitCountSelect:
            if (soc_feature(unit, soc_feature_ecmp_hash_bit_count_select)) {
                /*
                 * Determine the number of bits of the 16-bit ECMP hash value:
                 * 0: used hash bits are 0-9
                 * 1: used hash bits are 0-10
                 * 2: used hash bits are 0-11
                 * 3: used hash bits are 0-12
                 * 4: used hash bits are 0-13
                 * 5: used hash bits are 0-14
                 * 6: All 16 bits are used (defaul)
                 * 7: Invalid
                 */
                BCM_IF_ERROR_RETURN(
                    READ_HASH_CONTROLr(unit, &rval));
                *arg = soc_reg_field_get(unit, HASH_CONTROLr, rval,
                                         ECMP_HASH_FIELD_UPPER_BITS_COUNTf);

                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchPFCDeadlockDetectionTimeInterval:
        case bcmSwitchPFCDeadlockRecoveryAction:
        case bcmSwitchSampleFlexRandomSeed:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchFlexIP6ExtHdr:
            if (soc_feature(unit, soc_feature_l3_ip6) &&
                SOC_REG_IS_VALID(unit, FLEXIBLE_IPV6_EXT_HDRr)) {
                uint32 protocol_id;
                SOC_IF_ERROR_RETURN
                    (READ_FLEXIBLE_IPV6_EXT_HDRr(unit, &protocol_id));
                *arg = soc_reg_field_get(unit, FLEXIBLE_IPV6_EXT_HDRr,
                                         protocol_id, PROTOCOL_IDf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmSwitchFlexIP6ExtHdrEgress:
            if (soc_feature(unit, soc_feature_l3_ip6) &&
                SOC_REG_IS_VALID(unit, EGR_FLEXIBLE_IPV6_EXT_HDRr)) {
                uint32 protocol_id;
                SOC_IF_ERROR_RETURN
                    (READ_EGR_FLEXIBLE_IPV6_EXT_HDRr(unit, &protocol_id));
                *arg = soc_reg_field_get(unit, EGR_FLEXIBLE_IPV6_EXT_HDRr,
                                         protocol_id, PROTOCOL_IDf);
                return BCM_E_NONE;
            }
            break;

#ifdef BCM_EP_REDIRECT_VERSION_2
        case bcmSwitchRedirectBufferThresholdPriorityLow:
        case bcmSwitchRedirectBufferThresholdPriorityMedium:
        case bcmSwitchRedirectBufferThresholdPriorityHigh:
            return BCM_E_UNAVAIL;
            break;
#endif
        case bcmSwitchL3ClassIdForL2Enable:
        case bcmSwitchAlternateStoreForward:
            return BCM_E_UNAVAIL;
            break;

        case bcmSwitchRoce2UdpDstPort:
            if (soc_feature(unit, soc_feature_rroce)) {
                uint32 reg32 = 0;

                SOC_IF_ERROR_RETURN(READ_ING_ROCE_CONTROLr(unit, &reg32));
                *arg = soc_reg_field_get(unit, ING_ROCE_CONTROLr,
                                         reg32, UDP_DST_PORT_NUMBERf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchRoce1Ethertype:
            if (soc_feature(unit, soc_feature_rroce)) {
                uint32 reg32 = 0;

                SOC_IF_ERROR_RETURN(READ_ING_ROCE_CONTROLr(unit, &reg32));
                *arg = soc_reg_field_get(unit, ING_ROCE_CONTROLr,
                                         reg32, ROCEV1_ETHERTYPEf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashSelectControlExtended:
            if (soc_feature(unit, soc_feature_rroce)) {
                return
                bcmi_gh2_switch_rtag7_extended_roce_selectcontrol_get(unit,
                                                                      arg);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchE2EFCReceiveDestMacOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, f_lo, f_hi;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                f_lo = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_MSr(unit, &val));
                f_hi = soc_reg_field_get(unit, E2E_IBP_RX_DA_MSr,
                                         val, DAf);
                *arg = (f_hi & 0xffff) << 8 | ((f_lo >> 24) & 0xff);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveDestMacNonOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                *arg = fval & 0Xffffff;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveEtherType:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_LENGTH_TYPEr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_LENGTH_TYPEr,
                                         val, LENGTH_TYPEf);
                *arg = fval & 0xffff;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveOpcode:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_OPCODEr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_OPCODEr,
                                         val, OPCODEf);
                *arg = fval & 0xffff;
                return BCM_E_NONE;
            }
            break;
#ifdef BCM_FIRELIGHT_SUPPORT
        case bcmSwitch1588ClockAddressOui:
            if (soc_feature(unit, soc_feature_pdelay_req)) {
                uint64 val64;
                uint32 val_lo, val_hi;

                SOC_IF_ERROR_RETURN(READ_EGR_1588_CLOCK_ADDRESSr(unit, &val64));
                COMPILER_64_TO_32_LO(val_lo, val64);
                COMPILER_64_TO_32_HI(val_hi, val64);
                *arg = 0xffffff & ((val_hi << 8) | ((val_lo >> 24) & 0xff));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitch1588ClockAddressNonOui:
            if (soc_feature(unit, soc_feature_pdelay_req)) {
                uint64 val64;
                uint32 val_lo;

                SOC_IF_ERROR_RETURN(READ_EGR_1588_CLOCK_ADDRESSr(unit, &val64));
                COMPILER_64_TO_32_LO(val_lo, val64);
                *arg = (val_lo & 0xffffff);
                return BCM_E_NONE;
            }
            break;
#endif

        default:
            break;
    }

    /* Stop on first supported port for port specific controls */
    PBMP_E_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port)) {
            continue;
        }
        rv = bcm_esw_switch_control_port_get(unit, port, type, arg);
        if (rv != BCM_E_UNAVAIL) { /* Found one, or real error */
            return rv;
        }
    }

    PBMP_ST_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port)) {
            continue;
        }
        rv = bcm_esw_switch_control_port_get(unit, port, type, arg);
        if (rv != BCM_E_UNAVAIL) { /* Found one, or real error */
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_gh2_switch_init
 * Description:
 *      Initialize switch controls to their different values.
 * Parameters:
 *      unit        - Device unit number
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_gh2_switch_init(int unit)
{
#ifdef BCM_RCPU_SUPPORT
    int cos = 0;
#endif
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int mod = 0;
#endif

    /* Unit validation check */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (SOC_IS_RCPU_ONLY(unit)) {
        return BCM_E_NONE;
    }

#ifdef BCM_RCPU_SUPPORT
    /* Clear encapsulation priority mappings */
    if (soc_feature(unit, soc_feature_rcpu_priority)) {
        for (cos = 0; cos < NUM_CPU_COSQ(unit); cos++) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_rcpu_encap_priority_map_set(unit, 0, cos, 0));
        }
    }
#endif

    /* Egress copy-to-cpu Enable */
    if (soc_feature(unit, soc_feature_field_egress_tocpu)) {
        BCM_IF_ERROR_RETURN(bcm_gh2_switch_control_set(unit,
               bcmSwitchFieldStageEgressToCpu, EP_COPY_TOCPU_DEFAULT));
    }

    /* bcmSwitchModuleType will eventually be deprecated,
     * as pre-xgs3 devices are phased out.
     * Once deprecated, this switch control default init will be deleted.
     */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    for(mod = 0; mod < SOC_MODID_MAX(unit); mod++) {
        BCM_IF_ERROR_RETURN(
                bcm_gh2_switch_control_set(unit,bcmSwitchModuleType,
                    BCM_SWITCH_CONTROL_MOD_TYPE_PACK(mod,BCM_SWITCH_MODULE_XGS3)));

    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    if (soc_feature(unit, soc_feature_miml) ||
        soc_feature(unit, soc_feature_custom_header)) {
        BCM_IF_ERROR_RETURN(bcm_hr3_switch_encap_init(unit));
    }

#if defined (BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(bcmi_gh2_switch_reinit(unit));
    } else {
        BCM_IF_ERROR_RETURN(bcmi_gh2_switch_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_switch_temperature_monitor_get
 * Purpose:
 *      There are temperature monitors embedded on the various points of the
 *      switch chip for the purpose of monitoring the health of the chip.
 *      This routine retrieves each temperature monitor's current value and
 *      peak value. The unit is 0.1 celsius degree.
 * Parameters:
 *      unit - (IN) Unit number.
 *      temperature_max - (IN) max number of entries of the temperature_array.
 *      temperature_array - (OUT) the buffer array to hold the retrived values.
 *      temperature_count - (OUT) number of actual entries retrieved.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_gh2_switch_temperature_monitor_get(int unit,
          int temperature_max,
          bcm_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    int rv;
    rv = soc_gh2_temperature_monitor_get(unit,
            temperature_max,temperature_array,temperature_count);

    return rv;
}

int
bcmi_gh2_switch_chip_info_get(int unit,
                              bcm_switch_chip_info_t info_type,
                              int max_size,
                              void *data_buf,
                              int *actual_size)
{
    int rv = BCM_E_UNAVAIL;

    switch(info_type) {
        case bcmSwitchChipInfoIcid:
            if (data_buf == NULL) {
                /* return required buffer size of 32 bytes (256 bits) */
                *actual_size = GH2_ICID_LENGTH_BYTES;
                return BCM_E_NONE;
            }

            if (max_size != GH2_ICID_LENGTH_BYTES) {
                LOG_VERBOSE(BSL_LS_BCM_COMMON,
                            (BSL_META_U(unit,
                                        "unit %d : Incorrect buffer size.\n"),
                                        unit));
                return BCM_E_PARAM;
            }

            if (soc_icid_default_get(unit, (uint8 *)data_buf, max_size) < 0) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d : Cannot read ICID data.\n"),
                                      unit));
                return BCM_E_UNAVAIL;
            }
            *actual_size = GH2_ICID_LENGTH_BYTES;
            rv = BCM_E_NONE;
            break;
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }

    return rv;
}

