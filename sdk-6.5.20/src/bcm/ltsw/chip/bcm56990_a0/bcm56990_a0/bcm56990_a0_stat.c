/*! \file bcm56990_a0_stat.c
 *
 * BCM56990_A0 stat subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/stat_int.h>
#include <bcm_int/ltsw/mbcm/stat.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STAT

/*!
 * \brief Chip specific stat counter mapping table.
 */
static bcmint_stat_ctr_map_info_t bcm56990_a0_ltsw_stat_ctr_map_tab[] = {
    /*
     * RFC 1213 MIBs
     *
     * The following RFC 1213 MIBs are supported:
     *
     * snmpIfInDiscards = 3
     * snmpIfOutDiscards = 9
     * snmpIpInReceives = 12
     * snmpIpInHdrErrors = 13
     * snmpIpForwDatagrams = 14
     * snmpIpInDiscards = 15
     */
    /* snmpIfInDiscards = 3 */
    {snmpIfInDiscards, CTR_ING_DEBUGs, DEBUG_0s, FALSE},
    /* snmpIfOutDiscards = 9 */
    {snmpIfOutDiscards, CTR_EGR_DEBUGs, DEBUG_3s, FALSE},
    /* snmpIpInReceives = 12 */
    {snmpIpInReceives, CTR_L3s, RX_IPV4_UCs, FALSE},
    /* snmpIpInHdrErrors = 13 */
    {snmpIpInHdrErrors, CTR_L3s, RX_IPV4_DISCARDs, FALSE},
    /* snmpIpForwDatagrams = 14 */
    {snmpIpForwDatagrams, CTR_EGR_DEBUGs, DEBUG_4s, FALSE},
    /* snmpIpInDiscards = 15 */
    {snmpIpInDiscards, CTR_L3s, RX_IPV4_HDR_ERRs, FALSE},
    {snmpIpInDiscards, CTR_L3s, RX_IPV4_DISCARDs, FALSE},

    /*
     * RFC 1493 MIBs
     *
     * The following RFC 1493 MIBs are supported:
     *
     * snmpDot1dPortInDiscards = 20
     */
    /* snmpDot1dPortInDiscards = 20 */
    {snmpDot1dPortInDiscards, CTR_ING_DEBUGs, DEBUG_0s, FALSE},

    /*
     * RFC 1757 MIBs
     */

    /*
     * RFC 2665 MIBs
     */

    /*
     * RFC 2233 MIBs
     */

    /*
     * RFC 2465 MIBs
     *
     * The following RFC 2465 MIBs are supported:
     *
     * snmpIpv6IfStatsInReceives = 66
     * snmpIpv6IfStatsInHdrErrors = 67
     * snmpIpv6IfStatsInAddrErrors = 68
     * snmpIpv6IfStatsInDiscards = 69
     * snmpIpv6IfStatsOutForwDatagrams = 70
     * snmpIpv6IfStatsOutDiscards = 71
     * snmpIpv6IfStatsInMcastPkts = 72
     * snmpIpv6IfStatsOutMcastPkts = 73
 */
    /* snmpIpv6IfStatsInReceives = 66 */
    {snmpIpv6IfStatsInReceives, CTR_L3s, RX_IPV6_UCs, FALSE},
    {snmpIpv6IfStatsInReceives, CTR_L3s, RX_IPV6_ROUTE_MCs, FALSE},
    /* snmpIpv6IfStatsInHdrErrors = 67 */
    {snmpIpv6IfStatsInHdrErrors, CTR_L3s, RX_IPV6_DISCARDs, FALSE},
    /* snmpIpv6IfStatsInAddrErrors = 68 */
    {snmpIpv6IfStatsInAddrErrors, CTR_L3s, RX_IPV6_HDR_ERRs, FALSE},
    /* snmpIpv6IfStatsInDiscards = 69 */
    {snmpIpv6IfStatsInDiscards, CTR_L3s, RX_IPV6_HDR_ERRs, FALSE},
    {snmpIpv6IfStatsInDiscards, CTR_L3s, RX_IPV6_DISCARDs, FALSE},
    /* snmpIpv6IfStatsOutForwDatagrams = 70 */
    {snmpIpv6IfStatsOutForwDatagrams, CTR_EGR_DEBUGs, DEBUG_0s, FALSE},
    /* snmpIpv6IfStatsOutDiscards = 71 */
    {snmpIpv6IfStatsOutDiscards, CTR_EGR_DEBUGs, DEBUG_1s, FALSE},
    /* snmpIpv6IfStatsInMcastPkts = 72 */
    {snmpIpv6IfStatsInMcastPkts, CTR_L3s, RX_IPV6_ROUTE_MCs, FALSE},
    /* snmpIpv6IfStatsOutMcastPkts = 73 */
    {snmpIpv6IfStatsOutMcastPkts, CTR_EGR_DEBUGs, DEBUG_2s, FALSE},

    /*
     * RFC 1573 MIBs
     */

    /*
     * RFC IEEE 802.1bb MIBs
     */

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are supported:
     *
     * snmpBcmIPMCBridgedPckts = 82
     * snmpBcmIPMCRoutedPckts = 83
     * snmpBcmIPMCInDroppedPckts = 84
     * snmpBcmIPMCOutDroppedPckts = 85
     * snmpBcmCustomReceive0 = 130
     * snmpBcmCustomReceive1 = 131
     * snmpBcmCustomReceive2 = 132
     * snmpBcmCustomReceive3 = 133
     * snmpBcmCustomReceive4 = 134
     * snmpBcmCustomReceive5 = 135
     * snmpBcmCustomReceive6 = 136
     * snmpBcmCustomReceive7 = 137
     * snmpBcmCustomReceive8 = 138
     * snmpBcmCustomTransmit0 = 139
     * snmpBcmCustomTransmit1 = 140
     * snmpBcmCustomTransmit2 = 141
     * snmpBcmCustomTransmit3 = 142
     * snmpBcmCustomTransmit4 = 143
     * snmpBcmCustomTransmit5 = 144
     * snmpBcmCustomTransmit6 = 145
     * snmpBcmCustomTransmit7 = 146
     * snmpBcmCustomTransmit8 = 147
     * snmpBcmCustomTransmit9 = 148
     * snmpBcmCustomTransmit10 = 149
     * snmpBcmCustomTransmit11 = 150
 */
    /* snmpBcmIPMCBridgedPckts = 82 */
    {snmpBcmIPMCBridgedPckts, CTR_ING_DEBUGs, DEBUG_1s, FALSE},
    /* snmpBcmIPMCRoutedPckts = 83 */
    {snmpBcmIPMCRoutedPckts, CTR_L3s, RX_IPV4_ROUTE_MCs, FALSE},
    {snmpBcmIPMCRoutedPckts, CTR_L3s, RX_IPV6_ROUTE_MCs, FALSE},
    /* snmpBcmIPMCInDroppedPckts = 84 */
    {snmpBcmIPMCInDroppedPckts, CTR_ING_DEBUGs, DEBUG_2s, FALSE},
    /* snmpBcmIPMCOutDroppedPckts = 85 */
    {snmpBcmIPMCOutDroppedPckts, CTR_EGR_DEBUGs, DEBUG_5s, FALSE},
    /* snmpBcmCustomReceive0 = 130 */
    {snmpBcmCustomReceive0, CTR_ING_DEBUGs, DEBUG_0s, FALSE},
    /* snmpBcmCustomReceive1 = 131 */
    {snmpBcmCustomReceive1, CTR_ING_DEBUGs, DEBUG_1s, FALSE},
    /* snmpBcmCustomReceive2 = 132 */
    {snmpBcmCustomReceive2, CTR_ING_DEBUGs, DEBUG_2s, FALSE},
    /* snmpBcmCustomReceive3 = 133 */
    {snmpBcmCustomReceive3, CTR_ING_DEBUGs, DEBUG_3s, FALSE},
    /* snmpBcmCustomReceive4 = 134 */
    {snmpBcmCustomReceive4, CTR_ING_DEBUGs, DEBUG_4s, FALSE},
    /* snmpBcmCustomReceive5 = 135 */
    {snmpBcmCustomReceive5, CTR_ING_DEBUGs, DEBUG_5s, FALSE},
    /* snmpBcmCustomReceive6 = 136 */
    {snmpBcmCustomReceive6, CTR_ING_DEBUGs, DEBUG_6s, FALSE},
    /* snmpBcmCustomReceive7 = 137 */
    {snmpBcmCustomReceive7, CTR_ING_DEBUGs, DEBUG_7s, FALSE},
    /* snmpBcmCustomReceive8 = 138 */
    {snmpBcmCustomReceive8, CTR_ING_DEBUGs, DEBUG_8s, FALSE},
    /* snmpBcmCustomTransmit0 = 139 */
    {snmpBcmCustomTransmit0, CTR_EGR_DEBUGs, DEBUG_0s, FALSE},
    /* snmpBcmCustomTransmit1 = 140 */
    {snmpBcmCustomTransmit1, CTR_EGR_DEBUGs, DEBUG_1s, FALSE},
    /* snmpBcmCustomTransmit2 = 141 */
    {snmpBcmCustomTransmit2, CTR_EGR_DEBUGs, DEBUG_2s, FALSE},
    /* snmpBcmCustomTransmit3 = 142 */
    {snmpBcmCustomTransmit3, CTR_EGR_DEBUGs, DEBUG_3s, FALSE},
    /* snmpBcmCustomTransmit4 = 143 */
    {snmpBcmCustomTransmit4, CTR_EGR_DEBUGs, DEBUG_4s, FALSE},
    /* snmpBcmCustomTransmit5 = 144 */
    {snmpBcmCustomTransmit5, CTR_EGR_DEBUGs, DEBUG_5s, FALSE},
    /* snmpBcmCustomTransmit6 = 145 */
    {snmpBcmCustomTransmit6, CTR_EGR_DEBUGs, DEBUG_6s, FALSE},
    /* snmpBcmCustomTransmit7 = 146 */
    {snmpBcmCustomTransmit7, CTR_EGR_DEBUGs, DEBUG_7s, FALSE},
    /* snmpBcmCustomTransmit8 = 147 */
    {snmpBcmCustomTransmit8, CTR_EGR_DEBUGs, DEBUG_8s, FALSE},
    /* snmpBcmCustomTransmit9 = 148 */
    {snmpBcmCustomTransmit9, CTR_EGR_DEBUGs, DEBUG_9s, FALSE},
    /* snmpBcmCustomTransmit10 = 149 */
    {snmpBcmCustomTransmit10, CTR_EGR_DEBUGs, DEBUG_10s, FALSE},
    /* snmpBcmCustomTransmit11 = 150 */
    {snmpBcmCustomTransmit11, CTR_EGR_DEBUGs, DEBUG_11s, FALSE},

    /*
     * RFC 4837 MIBs
     */

    /*
     * Broadcom-specific MIBs
     */

    /*
     * PFC MIBs
     */

    /*
     * RFC 4044 MIBs
     */

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are supported:
     *
     * snmpBcmSampleIngressPkts = 214
     * snmpBcmSampleIngressSnapshotPkts = 215
     * snmpBcmSampleIngressSampledPkts = 216
     */
    /* snmpBcmSampleIngressPkts = 214 */
    {snmpBcmSampleIngressPkts, CTR_MIRROR_ING_PORT_SFLOWs,
     SAMPLE_POOL_PKTs, FALSE},
    /* snmpBcmSampleIngressSnapshotPkts = 215 */
    {snmpBcmSampleIngressSnapshotPkts, CTR_MIRROR_ING_PORT_SFLOWs,
     SAMPLE_POOL_PKT_SNAPSHOTs, FALSE},
    /* snmpBcmSampleIngressSampledPkts = 216 */
    {snmpBcmSampleIngressSampledPkts, CTR_MIRROR_ING_PORT_SFLOWs,
     SAMPLE_PKTs, FALSE},

    /*
     * RFC 1757 MIBs
     */

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are supported:
     *
     * snmpBcmRxDot3LengthMismatches = 240
     * snmpBcmRxTunnelDecapEcnPkts = 288
     * snmpBcmTxEcnPkts = 289
 */
    /* snmpBcmRxDot3LengthMismatches = 240 */
    {snmpBcmRxDot3LengthMismatches, CTR_MAC_ERRs, RX_LEN_OUT_OF_RANGE_PKTs, FALSE},
    /* snmpBcmRxTunnelDecapEcnPkts = 288 */
    {snmpBcmRxTunnelDecapEcnPkts, CTR_ECNs, RX_TNL_DECAP_ECN_PKTs, FALSE},
    /* snmpBcmTxEcnPkts = 289 */
    {snmpBcmTxEcnPkts, CTR_ECNs, TX_ECN_PKTs, FALSE},
};

/*!
 * \brief Chip specific stat debug counter mapping table.
 */
static bcmint_stat_map_info_t bcm56990_a0_ltsw_stat_dbg_map_tab[] = {
    {bcmDbgCntRIPD4, CTR_ING_DEBUG_SELECTs, RX_IPV4_DISCARDs},
    {bcmDbgCntRIPC4, CTR_ING_DEBUG_SELECTs, RX_IPV4s},
    {bcmDbgCntRIPHE4, CTR_ING_DEBUG_SELECTs, RX_IPV4_HDR_ERRs},
    {bcmDbgCntIMRP4, CTR_ING_DEBUG_SELECTs, IPV4_MC_ROUTEDs},
    {bcmDbgCntRIPD6, CTR_ING_DEBUG_SELECTs, RX_IPV6_DISCARDs},
    {bcmDbgCntRIPC6, CTR_ING_DEBUG_SELECTs, RX_IPV6s},
    {bcmDbgCntRIPHE6, CTR_ING_DEBUG_SELECTs, RX_IPV6_HDR_ERRs},
    {bcmDbgCntIMRP6, CTR_ING_DEBUG_SELECTs, IPV6_MC_ROUTEDs},
    /* bcmDbgCntRDISC = 8 */
    {bcmDbgCntRUC, CTR_ING_DEBUG_SELECTs, RX_UCs},
    {bcmDbgCntRPORTD, CTR_ING_DEBUG_SELECTs, RX_PORT_DROPs},
    {bcmDbgCntPDISC, CTR_ING_DEBUG_SELECTs, NON_IP_DISCARDs},
    {bcmDbgCntIMBP, CTR_ING_DEBUG_SELECTs, MC_BRIDGEDs},
    {bcmDbgCntRFILDR, CTR_ING_DEBUG_SELECTs, FP_ING_DROPs},
    {bcmDbgCntRIMDR, CTR_ING_DEBUG_SELECTs, RX_MC_DROPs},
    {bcmDbgCntRDROP, CTR_ING_DEBUG_SELECTs, RX_DROPs},
    {bcmDbgCntIRPSE, CTR_ING_DEBUG_SELECTs, BFD_TERMINATED_DROPs},
    /* bcmDbgCntIRHOL = 17 */
    /* bcmDbgCntIRIBP = 18 */
    {bcmDbgCntDSL3HE, CTR_ING_DEBUG_SELECTs, DOS_L3_ATTACKs},
    /* bcmDbgCntIUNKHDR = 20 */
    {bcmDbgCntDSL4HE, CTR_ING_DEBUG_SELECTs, DOS_L4_ATTACKs},
    /* bcmDbgCntIMIRROR = 22 */
    {bcmDbgCntDSICMP, CTR_ING_DEBUG_SELECTs, DOS_ICMPs},
    {bcmDbgCntDSFRAG, CTR_ING_DEBUG_SELECTs, DOS_FRAGMENTs},
    {bcmDbgCntMTUERR, CTR_ING_DEBUG_SELECTs, MTU_CHECK_FAILs},
    {bcmDbgCntRTUN, CTR_ING_DEBUG_SELECTs, RX_TNLs},
    {bcmDbgCntRTUNE, CTR_ING_DEBUG_SELECTs, RX_TNL_ERRs},
    /* bcmDbgCntVLANDR = 28 */
    /* bcmDbgCntRHGUC = 29 */
    /* bcmDbgCntRHGMC = 30 */
    /* bcmDbgCntMPLS = 31 */
    /* bcmDbgCntMACLMT = 32 */
    {bcmDbgCntMPLSERR, CTR_ING_DEBUG_SELECTs, MPLS_STAGEs},
    /* bcmDbgCntURPFERR = 34 */
    /* bcmDbgCntHGHDRE = 35 */
    {bcmDbgCntMCIDXE, CTR_ING_DEBUG_SELECTs, MC_INDEX_ERRs},
    {bcmDbgCntLAGLUP, CTR_ING_DEBUG_SELECTs, TRUNK_FAILOVER_LOOPBACKs},
    {bcmDbgCntLAGLUPD, CTR_ING_DEBUG_SELECTs, TRUNK_FAILOVER_LOOPBACK_DISCARDs},
    {bcmDbgCntPARITYD, CTR_ING_DEBUG_SELECTs, PARITY_ERR_DROPs},
    {bcmDbgCntVFPDR, CTR_ING_DEBUG_SELECTs, VLAN_FPs},
    /* bcmDbgCntURPF = 41 */
    /*
     * bcmDbgCntDSTDISCARDDROP = 42,
     * Replaced by
     * bcmDbgCntRxL2DstDiscardDrop and bcmDbgCntRxL3DstDiscardDrop
     */
    /* bcmDbgCntCLASSBASEDMOVEDROP = 43 */
    /* bcmDbgCntMACLMT_NODROP = 44 */
    {bcmDbgCntMACSAEQUALMACDA, CTR_ING_DEBUG_SELECTs, SRC_MAC_EQUALS_DST_MACs},
    /* bcmDbgCntMACLMT_DROP = 46 */
    {bcmDbgCntTGIP4, CTR_EGR_DEBUG_SELECTs, IPV4_PKTs},
    {bcmDbgCntTIPD4, CTR_EGR_DEBUG_SELECTs, IPV4_DROPs},
    {bcmDbgCntTGIPMC4, CTR_EGR_DEBUG_SELECTs, IPMCV4_PKTs},
    {bcmDbgCntTIPMCD4, CTR_EGR_DEBUG_SELECTs, IPMCV4_DROPs},
    {bcmDbgCntTGIP6, CTR_EGR_DEBUG_SELECTs, IPV6_PKTs},
    {bcmDbgCntTIPD6, CTR_EGR_DEBUG_SELECTs, IPV6_DROPs},
    {bcmDbgCntTGIPMC6, CTR_EGR_DEBUG_SELECTs, IPMCV6_PKTs},
    {bcmDbgCntTIPMCD6, CTR_EGR_DEBUG_SELECTs, IPMCV6_DROPs},
    {bcmDbgCntTTNL, CTR_EGR_DEBUG_SELECTs, TNL_PKTs},
    {bcmDbgCntTTNLE, CTR_EGR_DEBUG_SELECTs, TNL_ERRs},
    {bcmDbgCntTTTLD, CTR_EGR_DEBUG_SELECTs, TTL_DROPs},
    {bcmDbgCntTCFID, CTR_EGR_DEBUG_SELECTs, CFI_DROPs},
    {bcmDbgCntTVLAN, CTR_EGR_DEBUG_SELECTs, VLAN_TAGGEDs},
    {bcmDbgCntTVLAND, CTR_EGR_DEBUG_SELECTs, INVALID_VLANs},
    /* bcmDbgCntTVXLTMD = 61 (VLAN translation not supported) */
    {bcmDbgCntTSTGD, CTR_EGR_DEBUG_SELECTs, STG_DROPs},
    /* bcmDbgCntTAGED = 63 */
    {bcmDbgCntTL2MCD, CTR_EGR_DEBUG_SELECTs, L2_MC_DROPs},
    {bcmDbgCntTPKTD, CTR_EGR_DEBUG_SELECTs, PKT_DROPs},
    {bcmDbgCntTMIRR, CTR_EGR_DEBUG_SELECTs, MIRRORs},
    /* bcmDbgCntTSIPL = 67 (HG not supported) */
    /* bcmDbgCntTHGUC = 68 (HG not supported) */
    /* bcmDbgCntTHGMC = 69 (HG not supported) */
    /* bcmDbgCntTHIGIG2 = 70 */
    /* bcmDbgCntTHGI = 71 */
    {bcmDbgCntTL2_MTU, CTR_EGR_DEBUG_SELECTs, L2_MTU_FAILs},
    {bcmDbgCntTPARITY_ERR, CTR_EGR_DEBUG_SELECTs, PARITY_ERRs},
    {bcmDbgCntTIP_LEN_FAIL, CTR_EGR_DEBUG_SELECTs, PKT_TOO_SMALLs},
    /* bcmDbgCntTMTUD = 75 */
    /* bcmDbgCntTSLLD = 76 */
    /* bcmDbgCntTL2MPLS = 77 */
    /* bcmDbgCntTL3MPLS = 78 */
    /* bcmDbgCntTMPLS = 79 */
    /* bcmDbgCntTMODIDTOOLARGEDROP = 80 (HG not supported) */
    {bcmDbgCntPKTMODTOOLARGEDROP, CTR_EGR_DEBUG_SELECTs, PKT_TOO_LARGEs},
    /* bcmDbgCntRX_SYMBOL = 82 */
    /* bcmDbgCntTIME_ALIGNMENT_EVEN = 83 */
    /* bcmDbgCntTIME_ALIGNMENT_ODD = 84 */
    /* bcmDbgCntBIT_INTERLEAVED_PARITY_EVEN = 85 */
    /* bcmDbgCntBIT_INTERLEAVED_PARITY_ODD = 86 */
    /* bcmDbgCntFcmPortClass3RxFrames = 87 */
    /* bcmDbgCntFcmPortClass3RxDiscards = 88 */
    /* bcmDbgCntFcmPortClass3TxFrames = 89 */
    /* bcmDbgCntFcmPortClass2RxFrames = 90 */
    /* bcmDbgCntFcmPortClass2RxDiscards = 91 */
    /* bcmDbgCntFcmPortClass2TxFrames = 92 */
    /* bcmDbgCntNonHGoE = 93 */
    /* bcmDbgCntHGoE = 94 */
    /* bcmDbgCntEgressProtectionDataDrop = 95 */
    /* bcmDbgCntVntagRxError = 96 */
    /* bcmDbgCntNivRxForwardingError = 97 */
    /* bcmDbgCntIfpTriggerPfcRxDrop = 98 */
    /* bcmDbgCntEcnTxError = 99 */
    /* bcmDbgCntNivTxPrune = 100 */
    /* bcmDbgCntRxHigigLoopbackDrop = 101 */
    /* bcmDbgCntRxHigigUnknownOpcodeDrop = 102 */
    /* bcmDbgCntRxIpInIpDrop = 103 */
    /* bcmDbgCntRxMimDrop = 104 */
    /* bcmDbgCntRxTunnelInterfaceError = 105 */
    {bcmDbgCntRxMplsControlWordInvalid, CTR_ING_DEBUG_SELECTs, MPLS_INVALID_CWs},
    {bcmDbgCntRxMplsGalNotBosDrop, CTR_ING_DEBUG_SELECTs, MPLS_GAL_LABELs},
    {bcmDbgCntRxMplsPayloadInvalid, CTR_ING_DEBUG_SELECTs, MPLS_INVALID_PAYLOADs},
    /* bcmDbgCntRxMplsEntropyDrop = 109 */
    {bcmDbgCntRxMplsLabelLookupMiss, CTR_ING_DEBUG_SELECTs, MPLS_LABEL_MISSs},
    {bcmDbgCntRxMplsActionInvalid, CTR_ING_DEBUG_SELECTs, MPLS_INVALID_ACTIONs},
    {bcmDbgCntRxTunnelTtlError, CTR_ING_DEBUG_SELECTs, MPLS_TTL_CHECK_FAILs},
    /* bcmDbgCntRxTunnelShimError = 113 */
    /* bcmDbgCntRxTunnelObjectValidationFail = 114 */
    {bcmDbgCntRxVlanMismatch, CTR_ING_DEBUG_SELECTs, INVALID_VLANs},
    {bcmDbgCntRxVlanMemberMismatch, CTR_ING_DEBUG_SELECTs, PORT_ING_VLAN_MEMBERSHIPs},
    {bcmDbgCntRxTpidMismatch, CTR_ING_DEBUG_SELECTs, INVALID_TPIDs},
    {bcmDbgCntRxPrivateVlanMismatch, CTR_ING_DEBUG_SELECTs, PVLAN_VID_MISMATCHs},
    /* bcmDbgCntRxMacIpBindError = 119 */
    /* bcmDbgCntRxVlanTranslate2LookupMiss = 120 */
    /* bcmDbgCntRxL3TunnelLookupMiss = 121 */
    /* bcmDbgCntRxMplsLookupMiss = 122 */
    /* bcmDbgCntRxVlanTranslate1LookupMiss = 123 */
    /* bcmDbgCntRxFcoeVftDrop = 124 */
    /* bcmDbgCntRxFcoeSrcBindError = 125 */
    /* bcmDbgCntRxFcoeSrcFpmaError = 126 */
    /* bcmDbgCntRxVfiP2pDrop = 127 */
    {bcmDbgCntRxStgDrop, CTR_ING_DEBUG_SELECTs, SPANNING_TREE_STATEs},
    {bcmDbgCntRxCompositeError, CTR_ING_DEBUG_SELECTs, COMPOSITE_ERRORs},
    {bcmDbgCntRxBpduDrop, CTR_ING_DEBUG_SELECTs, BPDUs},
    {bcmDbgCntRxProtocolDrop, CTR_ING_DEBUG_SELECTs, PROTCOL_PKT_CTRL_DROPs},
    {bcmDbgCntRxUnknownMacSaDrop, CTR_ING_DEBUG_SELECTs, CPU_MANAGED_LEARNINGs},
    {bcmDbgCntRxSrcRouteDrop, CTR_ING_DEBUG_SELECTs, SRC_ROUTEs},
    {bcmDbgCntRxL2SrcDiscardDrop, CTR_ING_DEBUG_SELECTs, SRC_L2_DISCARDs},
    {bcmDbgCntRxL2SrcStaticMoveDrop, CTR_ING_DEBUG_SELECTs, SRC_L2_STATIC_MOVEs},
    {bcmDbgCntRxL2DstDiscardDrop, CTR_ING_DEBUG_SELECTs, DST_L2_DISCARDs},
    {bcmDbgCntRxL3DisableDrop, CTR_ING_DEBUG_SELECTs, CFI_OR_L3_DISABLEs},
    {bcmDbgCntRxMacSaEqual0Drop, CTR_ING_DEBUG_SELECTs, SRC_MAC_ZEROs},
    {bcmDbgCntRxVlanCrossConnectOrNonUnicastDrop, CTR_ING_DEBUG_SELECTs, VLAN_CC_OR_PBTs},
    {bcmDbgCntRxTimeSyncDrop, CTR_ING_DEBUG_SELECTs, TIME_SYNC_PKTs},
    {bcmDbgCntRxIpmcDrop, CTR_ING_DEBUG_SELECTs, IP_MC_DROPs},
    {bcmDbgCntRxMyStationDrop, CTR_ING_DEBUG_SELECTs, L2_MY_STATIONs},
    /* bcmDbgCntRxPrivateVlanVpFilterDrop = 143 */
    {bcmDbgCntRxL3DstDiscardDrop, CTR_ING_DEBUG_SELECTs, DST_L3_DISCARDs},
    {bcmDbgCntRxTunnelDecapEcnError, CTR_ING_DEBUG_SELECTs, TNL_DECAP_ECNs},
    /* bcmDbgCntRxL3SrcDiscardDrop = 146 */
    /* bcmDbgCntRxFcoeZoneLookupMiss = 147 */
    {bcmDbgCntRxL3TtlError, CTR_ING_DEBUG_SELECTs, L3_TTL_ERRs},
    {bcmDbgCntRxL3HeaderError, CTR_ING_DEBUG_SELECTs, L3_HDR_ERRs},
    /* bcmDbgCntRxL2HeaderError = 150 */
    {bcmDbgCntRxL3DstLookupDrop, CTR_ING_DEBUG_SELECTs, DST_L3_LOOKUP_MISSs},
    /* bcmDbgCntRxL2TtlError = 152 */
    /* bcmDbgCntRxL2RpfError = 153 */
    {bcmDbgCntRxTagUntagDiscardDrop, CTR_ING_DEBUG_SELECTs, TAG_UNTAG_DISCARDs},
    /* bcmDbgCntRxStormControlDrop = 155 */
    /* bcmDbgCntRxFcoeZoneError = 156 */
    {bcmDbgCntRxFieldChangeL2NoRedirectDrop, CTR_ING_DEBUG_SELECTs, FP_REDIRECT_DROPs},
    {bcmDbgCntRxNextHopDrop, CTR_ING_DEBUG_SELECTs, NHOP_DROPs},
    /* bcmDbgCntRxNatDrop = 159 */
    {bcmDbgCntIngressProtectionDataDrop, CTR_ING_DEBUG_SELECTs, PROTECTION_DATA_DROPs},
    {bcmDbgCntRxSrcKnockoutDrop, CTR_ING_DEBUG_SELECTs, SRC_PORT_KNOCKOUT_DROPs},
    /* bcmDbgCntRxMplsSeqNumberError = 162 */
    {bcmDbgCntRxBlockMaskDrop, CTR_ING_DEBUG_SELECTs, BLOCK_MASK_DROPs},
    /* bcmDbgCntRxDlbRhResolutionError = 164 */
    /* bcmDbgCntTxFwdDomainNotFoundDrop = 165 */
    /* bcmDbgCntTxNotFwdDomainMemberDrop = 166 */
    {bcmDbgCntTxIpmcL2SrcPortPruneDrop, CTR_EGR_DEBUG_SELECTs, IPMC_L3_SELF_INTFs},
    /* bcmDbgCntTxNonUnicastPruneDrop = 168 */
    /* bcmDbgCntTxSvpRemoveDrop = 169 */
    /* bcmDbgCntTxVplagPruneDrop = 170 */
    /* bcmDbgCntTxSplitHorizonDrop = 171 */
    /* bcmDbgCntTxMmuPurgeDrop = 172 */
    /* bcmDbgCntTxStgDisableDrop = 173 */
    /* bcmDbgCntTxEgressPortDrop = 174 */
    /* bcmDbgCntTxEgressFilterDrop = 175 */
    /* bcmDbgCntTxVisibilityDrop = 176 */
    {bcmDbgCntRxMacControlDrop, CTR_ING_DEBUG_SELECTs, MAC_CONTROL_FRAMEs},
    {bcmDbgCntRxProtocolErrorIP4, CTR_ING_DEBUG_SELECTs, IPV4_PROTOCOL_ERRs},
    {bcmDbgCntRxProtocolErrorIP6, CTR_ING_DEBUG_SELECTs, IPV6_PROTOCOL_ERRs},
    {bcmDbgCntRxLAGFailLoopback, CTR_ING_DEBUG_SELECTs, TRUNK_FAIL_LOOPBACKs},
    {bcmDbgCntRxEcmpResolutionError, CTR_ING_DEBUG_SELECTs, ECMP_RESOLUTION_ERRs},
    {bcmDbgCntRxPfmDrop, CTR_ING_DEBUG_SELECTs, PORT_FILTERING_MODEs},
    {bcmDbgCntRxTunnelError, CTR_ING_DEBUG_SELECTs, TNL_ERRORs},
    {bcmDbgCntRxBFDUnknownAchError, CTR_ING_DEBUG_SELECTs, BFD_UNKNOWN_ACH_ERRs},
    {bcmDbgCntRxBFDUnknownControlFrames, CTR_ING_DEBUG_SELECTs, BFD_UNKNOWN_CTRL_PKTs},
    {bcmDbgCntRxBFDUnknownVersionDiscards, CTR_ING_DEBUG_SELECTs, BFD_UNKNOWN_VER_OR_DISCARDs},
    {bcmDbgCntRxDSL2HE, CTR_ING_DEBUG_SELECTs, DOS_L2s},
    {bcmDbgCntRxEgressMaskDrop, CTR_ING_DEBUG_SELECTs, EGR_MASKs},
    {bcmDbgCntRxFieldRedirectMaskDrop, CTR_ING_DEBUG_SELECTs, FP_REDIRECT_MASKs},
    {bcmDbgCntRxFieldDelayDrop, CTR_ING_DEBUG_SELECTs, FP_ING_DELAYED_DROPs},
    {bcmDbgCntRxEgressMaskSrcPortDrop, CTR_ING_DEBUG_SELECTs, ING_EGR_MASKs},
    {bcmDbgCntRxMacBlockMaskDrop, CTR_ING_DEBUG_SELECTs, MAC_MASKs},
    {bcmDbgCntRxMCError, CTR_ING_DEBUG_SELECTs, MC_DROPs},
    {bcmDbgCntRxNonUnicastMaskDrop, CTR_ING_DEBUG_SELECTs, NONUC_MASKs},
    {bcmDbgCntRxNonUnicastLAGMaskDrop, CTR_ING_DEBUG_SELECTs, NONUC_TRUNK_RESOLUTION_MASKs},
    {bcmDbgCntRxStgMaskDrop, CTR_ING_DEBUG_SELECTs, VLAN_STG_DROPs},
    {bcmDbgCntRxVlanBlockMaskDrop, CTR_ING_DEBUG_SELECTs, VLAN_BLOCKED_DROPs},
    {bcmDbgCntRxEgressVlanMemberMismatch, CTR_ING_DEBUG_SELECTs, VLAN_MEMBERSHIP_DROPs},
    {bcmDbgCntTxCellTooSmall, CTR_EGR_DEBUG_SELECTs, CELL_TOO_SMALLs},
    {bcmDbgCntTxLoopbackToLoopbackDrop, CTR_EGR_DEBUG_SELECTs, LB_TO_LB_DROPs},
    {bcmDbgCntRxINTDataplaneProbeDrop, CTR_ING_DEBUG_SELECTs, INBAND_TELEMETRY_DATAPLANE_EXCEPTION_DROPs},
    {bcmDbgCntRxINTVectorMismatch, CTR_ING_DEBUG_SELECTs, INBAND_TELEMETRY_VECTOR_MISS_MATCH_DROPs},
};

/******************************************************************************
 * Private functions
 */


/* Per-chip stat device information. */
static bcmint_stat_dev_info_t bcm56990_a0_stat_devinfo = {
    bcm56990_a0_ltsw_stat_ctr_map_tab,
    COUNTOF(bcm56990_a0_ltsw_stat_ctr_map_tab),
    bcm56990_a0_ltsw_stat_dbg_map_tab,
    COUNTOF(bcm56990_a0_ltsw_stat_dbg_map_tab)
};


static int
bcm56990_a0_ltsw_stat_dev_info_get(
    int unit,
    const bcmint_stat_dev_info_t **devinfo)
{
    *devinfo = &bcm56990_a0_stat_devinfo;

    return SHR_E_NONE;
}

/*!
 * \brief Stat sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_stat_drv_t bcm56990_a0_stat_sub_drv = {
    .stat_dev_info_get = bcm56990_a0_ltsw_stat_dev_info_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_stat_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_drv_set(unit, &bcm56990_a0_stat_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
