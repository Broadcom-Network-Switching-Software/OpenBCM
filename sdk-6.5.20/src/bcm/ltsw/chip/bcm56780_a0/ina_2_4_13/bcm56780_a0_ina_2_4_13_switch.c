/*! \file bcm56780_a0_ina_2_4_13_switch.c
 *
 * BCM56780_A0 INA_2_4_13 Switch Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/switch_int.h>
#include <bcm_int/ltsw/xfs/switch.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/util.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_SWITCH

/*!
 * \brief Drop event IDs mapping table.
 */
static const
bcmint_switch_map_info_t drop_event_id_map[] = {
    {
        .type = bcmPktDropEventIngressNoDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = NO_DROPs,
    },
    {
        .type = bcmPktDropEventIngressCmlFlagsDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = CML_FLAGS_DROPs,
    },
    {
        .type = bcmPktDropEventIngressL2SrcStaticMove,
        .table = MON_ING_DROP_EVENTs,
        .field = L2_SRC_STATIC_MOVEs,
    },
    {
        .type = bcmPktDropEventIngressL2SrcDiscard,
        .table = MON_ING_DROP_EVENTs,
        .field = L2_SRC_DISCARDs,
    },
    {
        .type = bcmPktDropEventIngressMacsaMulticast,
        .table = MON_ING_DROP_EVENTs,
        .field = MACSA_MULTICASTs,
    },
    {
        .type = bcmPktDropEventIngressOuterTpidCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = OUTER_TPID_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressIncomingPvlanCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = INCOMING_PVLAN_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressPktIntegrityCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = PKT_INTEGRITY_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressProtocolPktDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = PROTOCOL_PKT_DROPs,
    },
    {
        .type = bcmPktDropEventIngressMembershipCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressSpanningTreeCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressL2DstLookupMiss,
        .table = MON_ING_DROP_EVENTs,
        .field = L2_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktDropEventIngressL2DstDiscard,
        .table = MON_ING_DROP_EVENTs,
        .field = L2_DST_DISCARDs,
    },
    {
        .type = bcmPktDropEventIngressL3DstLookupMiss,
        .table = MON_ING_DROP_EVENTs,
        .field = L3_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktDropEventIngressL3DstDiscard,
        .table = MON_ING_DROP_EVENTs,
        .field = L3_DST_DISCARDs,
    },
    {
        .type = bcmPktDropEventIngressL3HdrError,
        .table = MON_ING_DROP_EVENTs,
        .field = L3_HDR_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressL3TtlError,
        .table = MON_ING_DROP_EVENTs,
        .field = L3_TTL_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressIpmcL3IifOrRpaIdCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressTunnelTtlCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = TUNNEL_TTL_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressTunnelObjectValidationFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = TUNNEL_OBJECT_VALIDATION_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressTunnelAdaptDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = TUNNEL_ADAPT_DROPs,
    },
    {
        .type = bcmPktDropEventIngressPvlanDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = PVLAN_DROPs,
    },
    {
        .type = bcmPktDropEventIngressVfp,
        .table = MON_ING_DROP_EVENTs,
        .field = VFPs,
    },
    {
        .type = bcmPktDropEventIngressIfp,
        .table = MON_ING_DROP_EVENTs,
        .field = IFPs,
    },
    {
        .type = bcmPktDropEventIngressIfpMeter,
        .table = MON_ING_DROP_EVENTs,
        .field = IFP_METERs,
    },
    {
        .type = bcmPktDropEventIngressDstFp,
        .table = MON_ING_DROP_EVENTs,
        .field = DST_FPs,
    },
    {
        .type = bcmPktDropEventIngressMplsProtectionDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_PROTECTION_DROPs,
    },
    {
        .type = bcmPktDropEventIngressMplsLabelActionInvalid,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_LABEL_ACTION_INVALIDs,
    },
    {
        .type = bcmPktDropEventIngressMplsTermPolicySelectTableDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_TERM_POLICY_SELECT_TABLE_DROPs,
    },
    {
        .type = bcmPktDropEventIngressMplsReservedLabelExposed,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_RESERVED_LABEL_EXPOSEDs,
    },
    {
        .type = bcmPktDropEventIngressMplsTtlError,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_TTL_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressMplsEcnError,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_ECN_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressEmFt,
        .table = MON_ING_DROP_EVENTs,
        .field = EM_FTs,
    },
    {
        .type = bcmPktDropEventIngressIvxlt,
        .table = MON_ING_DROP_EVENTs,
        .field = IVXLTs,
    },
    {
        .type = bcmPktDropEventIngressEpRecircDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = EP_RECIRC_MIRROR_DROPs,
    },
    {
        .type = bcmPktDropEventIngressExtIpv4GatewayTable,
        .table = MON_ING_DROP_EVENTs,
        .field = MTOP_IPV4_GATEWAYs,
    },
    {
        .type = bcmPktDropEventIngressUrpfCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = URPF_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressSrcPortKnockoutDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = SRC_PORT_KNOCKOUT_DROPs,
    },
    {
        .type = bcmPktDropEventIngressLagFailoverPortDown,
        .table = MON_ING_DROP_EVENTs,
        .field = LAG_FAILOVER_PORT_DOWNs,
    },
    {
        .type = bcmPktDropEventIngressSplitHorizonCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = SPLIT_HORIZON_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressDstLinkDown,
        .table = MON_ING_DROP_EVENTs,
        .field = DST_LINK_DOWNs,
    },
    {
        .type = bcmPktDropEventIngressBlockMaskDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = BLOCK_MASK_DROPs,
    },
    {
        .type = bcmPktDropEventIngressL3MtuCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = L3_MTU_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressSeqNumCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = SEQ_NUM_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressL3IifEqL3Oif,
        .table = MON_ING_DROP_EVENTs,
        .field = L3_IIF_EQ_L3_OIFs,
    },
    {
        .type = bcmPktDropEventIngressStormControlDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = STORM_CONTROL_DROPs,
    },
    {
        .type = bcmPktDropEventIngressEgrMembershipCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = EGR_MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressEgrSpanningTreeCheckFailed,
        .table = MON_ING_DROP_EVENTs,
        .field = EGR_SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressDstPbmZero,
        .table = MON_ING_DROP_EVENTs,
        .field = DST_PBM_ZEROs,
    },
    {
        .type = bcmPktDropEventIngressMplsCtrlPktDrop,
        .table = MON_ING_DROP_EVENTs,
        .field = MPLS_CTRL_PKT_DROPs,
    },
};

/*!
 * \brief Trace event IDs mapping table.
 */
static const
bcmint_switch_map_info_t trace_event_id_map[] = {
    {
        .type = bcmPktTraceEventIngressTraceDoNotCopyToCpu,
        .table = MON_ING_TRACE_EVENTs,
        .field = NO_COPY_TO_CPUs,
    },
    {
        .type = bcmPktTraceEventIngressCmlFlags,
        .table = MON_ING_TRACE_EVENTs,
        .field = CML_FLAGSs,
    },
    {
        .type = bcmPktTraceEventIngressL2SrcStaticMove,
        .table = MON_ING_TRACE_EVENTs,
        .field = L2_SRC_STATIC_MOVEs,
    },
    {
        .type = bcmPktTraceEventIngressL2SrcDiscard,
        .table = MON_ING_TRACE_EVENTs,
        .field = L2_SRC_DISCARDs,
    },
    {
        .type = bcmPktTraceEventIngressMacsaMulticast,
        .table = MON_ING_TRACE_EVENTs,
        .field = MACSA_MULTICASTs,
    },
    {
        .type = bcmPktTraceEventIngressPktIntegrityCheckFailed,
        .table = MON_ING_TRACE_EVENTs,
        .field = PKT_INTEGRITY_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressProtocolPkt,
        .table = MON_ING_TRACE_EVENTs,
        .field = PROTOCOL_PKTs,
    },
    {
        .type = bcmPktTraceEventIngressMembershipCheckFailed,
        .table = MON_ING_TRACE_EVENTs,
        .field = MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressSpanningTreeCheckFailed,
        .table = MON_ING_TRACE_EVENTs,
        .field = SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressL2DstLookupMiss,
        .table = MON_ING_TRACE_EVENTs,
        .field = L2_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktTraceEventIngressL2DstLookup,
        .table = MON_ING_TRACE_EVENTs,
        .field = L2_DST_LOOKUPs,
    },
    {
        .type = bcmPktTraceEventIngressL3DstLookupMiss,
        .table = MON_ING_TRACE_EVENTs,
        .field = L3_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktTraceEventIngressL3DstLookup,
        .table = MON_ING_TRACE_EVENTs,
        .field = L3_DST_LOOKUPs,
    },
    {
        .type = bcmPktTraceEventIngressL3HdrError,
        .table = MON_ING_TRACE_EVENTs,
        .field = L3_HDR_ERRORs,
    },
    {
        .type = bcmPktTraceEventIngressL3TtlError,
        .table = MON_ING_TRACE_EVENTs,
        .field = L3_TTL_ERRORs,
    },
    {
        .type = bcmPktTraceEventIngressIpmcL3IifOrRpaIdCheckFailed,
        .table = MON_ING_TRACE_EVENTs,
        .field = IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressLearnCacheFull,
        .table = MON_ING_TRACE_EVENTs,
        .field = LEARN_CACHE_FULLs,
    },
    {
        .type = bcmPktTraceEventIngressVfp,
        .table = MON_ING_TRACE_EVENTs,
        .field = VFPs,
    },
    {
        .type = bcmPktTraceEventIngressIfp,
        .table = MON_ING_TRACE_EVENTs,
        .field = IFPs,
    },
    {
        .type = bcmPktTraceEventIngressIfpMeter,
        .table = MON_ING_TRACE_EVENTs,
        .field = IFP_METERs,
    },
    {
        .type = bcmPktTraceEventIngressDstFp,
        .table = MON_ING_TRACE_EVENTs,
        .field = DST_FPs,
    },
    {
        .type = bcmPktTraceEventIngressSvp,
        .table = MON_ING_TRACE_EVENTs,
        .field = SVPs,
    },
    {
        .type = bcmPktTraceEventIngressEmFt,
        .table = MON_ING_TRACE_EVENTs,
        .field = EM_FTs,
    },
    {
        .type = bcmPktTraceEventIngressIvxlt,
        .table = MON_ING_TRACE_EVENTs,
        .field = IVXLTs,
    },
    {
        .type = bcmPktTraceEventIngressMirrorSamplerSampled,
        .table = MON_ING_TRACE_EVENTs,
        .field = MIRROR_SAMPLER_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressMirrorSamplerEgrSampled,
        .table = MON_ING_TRACE_EVENTs,
        .field = MIRROR_SAMPLER_EGR_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressSerDrop,
        .table = MON_ING_TRACE_EVENTs,
        .field = SER_DROPs,
    },
    {
        .type = bcmPktTraceEventIngressUrpfCheckFailed,
        .table = MON_ING_TRACE_EVENTs,
        .field = URPF_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressL3IifEqL3Oif,
        .table = MON_ING_TRACE_EVENTs,
        .field = L3_IIF_EQ_L3_OIFs,
    },
    {
        .type = bcmPktTraceEventIngressDlbEcmpMonitorEnOrMemberReassined,
        .table = MON_ING_TRACE_EVENTs,
        .field = DLB_ECMP_MONITOR_EN_OR_MEMBER_REASSINEDs,
    },
    {
        .type = bcmPktTraceEventIngressDlbLagMonitorEnOrMemberReassined,
        .table = MON_ING_TRACE_EVENTs,
        .field = DLB_LAG_MONITOR_EN_OR_MEMBER_REASSINEDs,
    },
    {
        .type = bcmPktTraceEventIngressMplsCtrlPktToCpu,
        .table = MON_ING_TRACE_EVENTs,
        .field = MPLS_CTRL_PKT_TO_CPUs,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit0,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_0s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit1,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_1s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit2,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_2s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit3,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_3s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit4,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_4s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit5,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_5s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit6,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_6s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit7,
        .table = MON_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_7s,
    },
    {
        .type = bcmPktTraceEventIngressExtIpv4GatewayTable,
        .table = MON_ING_TRACE_EVENTs,
        .field = MTOP_IPV4_GATEWAYs,
    },
    {
        .type = bcmPktTraceEventIngressDlbEcmpPktSampled,
        .table = MON_ING_TRACE_EVENTs,
        .field = DLB_ECMP_PKT_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressDlbLagPktSampled,
        .table = MON_ING_TRACE_EVENTs,
        .field = DLB_LAG_PKT_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressTraceDoNotMirror,
        .table = MON_ING_TRACE_EVENTs,
        .field = TRACE_DO_NOT_MIRRORs,
    },
    {
        .type = bcmPktTraceEventIngressTraceDoNotCopyToCpu,
        .table = MON_ING_TRACE_EVENTs,
        .field = TRACE_DO_NOT_COPY_TO_CPUs,
    },
    {
        .type = bcmPktTraceEventIngressTraceDop,
        .table = MON_ING_TRACE_EVENTs,
        .field = TRACE_DOPs,
    },
};

/*!
 * \brief EP Recirculate drop event IDs mapping table.
 */
static const
bcmint_switch_map_info_t ep_recirc_drop_event_id_map[] = {
    {
        .type = 0,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = NO_DROPs,
    },
    {
        .type = 1,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = ING_PG_LIMITs,
    },
    {
        .type = 2,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = ING_PORTSP_LIMITs,
    },
    {
        .type = 4,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = ING_HEADROOM_POOL_LIMITs,
    },
    {
        .type = 5,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_QUEUE_LIMITs,
    },
    {
        .type = 6,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_PORTSP_LIMITs,
    },
    {
        .type = 7,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = WRED_CHECKSs,
    },
    {
        .type = 8,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = CML_FLAGS_DROPs,
    },
    {
        .type = 9,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L2_SRC_STATIC_MOVEs,
    },
    {
        .type = 10,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L2_SRC_DISCARDs,
    },
    {
        .type = 11,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MACSA_MULTICASTs,
    },
    {
        .type = 12,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = OUTER_TPID_CHECK_FAILEDs,
    },
    {
        .type = 13,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = INCOMING_PVLAN_CHECK_FAILEDs,
    },
    {
        .type = 14,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = PKT_INTEGRITY_CHECK_FAILEDs,
    },
    {
        .type = 15,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = PROTOCOL_PKT_DROPs,
    },
    {
        .type = 16,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = 17,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = 18,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L2_DST_LOOKUP_MISSs,
    },
    {
        .type = 19,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L2_DST_DISCARDs,
    },
    {
        .type = 20,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_DST_LOOKUP_MISSs,
    },
    {
        .type = 21,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_DST_DISCARDs,
    },
    {
        .type = 22,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_HDR_ERRORs,
    },
    {
        .type = 23,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_TTL_ERRORs,
    },
    {
        .type = 24,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILEDs,
    },
    {
        .type = 25,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = TUNNEL_TTL_CHECK_FAILEDs,
    },
    {
        .type = 27,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = TUNNEL_OBJECT_VALIDATION_FAILEDs,
    },
    {
        .type = 28,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = TUNNEL_ADAPT_DROPs,
    },
    {
        .type = 29,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = PVLAN_DROPs,
    },
    {
        .type = 30,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = VFPs,
    },
    {
        .type = 31,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = IFPs,
    },
    {
        .type = 32,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = IFP_METERs,
    },
    {
        .type = 33,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = DST_FPs,
    },
    {
        .type = 34,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_PROTECTION_DROPs,
    },
    {
        .type = 35,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_LABEL_ACTION_INVALIDs,
    },
    {
        .type = 36,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_TERM_POLICY_SELECT_TABLE_DROPs,
    },
    {
        .type = 37,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_RESERVED_LABEL_EXPOSEDs,
    },
    {
        .type = 38,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_TTL_ERRORs,
    },
    {
        .type = 39,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_ECN_ERRORs,
    },
    {
        .type = 40,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EM_FTs,
    },
    {
        .type = 41,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = IVXLTs,
    },
    {
        .type = 42,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EP_RECIRC_MIRROR_DROPs,
    },
    {
        .type = 43,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MTOP_IPV4_GATEWAYs,
    },
    {
        .type = 97,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = URPF_CHECK_FAILEDs,
    },
    {
        .type = 98,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = SRC_PORT_KNOCKOUT_DROPs,
    },
    {
        .type = 99,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = LAG_FAILOVER_PORT_DOWNs,
    },
    {
        .type = 100,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = SPLIT_HORIZON_CHECK_FAILEDs,
    },
    {
        .type = 101,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = DST_LINK_DOWNs,
    },
    {
        .type = 102,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = BLOCK_MASK_DROPs,
    },
    {
        .type = 103,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_MTU_CHECK_FAILEDs,
    },
    {
        .type = 104,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = SEQ_NUM_CHECK_FAILEDs,
    },
    {
        .type = 105,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_IIF_EQ_L3_OIFs,
    },
    {
        .type = 106,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = STORM_CONTROL_DROPs,
    },
    {
        .type = 107,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = 108,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = 109,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = DST_PBM_ZEROs,
    },
    {
        .type = 110,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MPLS_CTRL_PKT_DROPs,
    },
    {
        .type = 128,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L2_OIF_DROPs,
    },
    {
        .type = 129,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = MEMBERSHIP_DROPs,
    },
    {
        .type = 130,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = DVP_MEMBERSHIP_DROPs,
    },
    {
        .type = 131,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = TTL_DROPs,
    },
    {
        .type = 132,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L3_SAME_INTF_DROPs,
    },
    {
        .type = 133,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = L2_SAME_PORT_DROPs,
    },
    {
        .type = 134,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = SPLIT_HORIZON_DROPs,
    },
    {
        .type = 135,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = STG_DISABLE_DROPs,
    },
    {
        .type = 136,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = STG_BLOCK_DROPs,
    },
    {
        .type = 137,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EFP_DROPs,
    },
    {
        .type = 138,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_METER_DROPs,
    },
    {
        .type = 139,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_MTU_DROPs,
    },
    {
        .type = 140,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_VXLT_DROPs,
    },
    {
        .type = 141,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_CELL_TOO_LARGEs,
    },
    {
        .type = 142,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EGR_CELL_TOO_SMALLs,
    },
    {
        .type = 143,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = QOS_REMARKING_DROPs,
    },
    {
        .type = 144,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = SVP_EQUAL_DVP_DROPs,
    },
    {
        .type = 145,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = INVALID_1588_PKTs,
    },
    {
        .type = 146,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = FLEX_EDITOR_DROPs,
    },
    {
        .type = 147,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = EP_RECIRC_DROPs,
    },
    {
        .type = 148,
        .table = MON_REDIRECT_DROP_EVENT_CONTROLs,
        .field = IFA_MD_DELETE_DROPs,
    },
};

/*!
 * \brief EP Recirculate trace event IDs mapping table.
 */
static const
bcmint_switch_map_info_t ep_recirc_trace_event_id_map[] = {
    {
        .type = bcmPktTraceEventEgressEfp,
        .table = MON_EGR_REDIRECT_TRACE_EVENTs,
        .field = EFPs,
    },
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Set entry fields into entry handle.
 *
 * \param [in] unit Unit number.
 * \param [in] eh Entry handle.
 * \param [in] entry Entry info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
switch_fields_set(int unit, bcmlt_entry_handle_t eh, bcmi_lt_entry_t *entry)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_SET)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh,
                                   entry->fields[i].fld_name,
                                   entry->fields[i].u.val));
    }

    if (entry->attr) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_attrib_set(eh, entry->attr));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get entry fields from entry handle.
 *
 * \param [in] unit Unit number.
 * \param [in] eh Entry handle.
 * \param [out] entry Entry info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
switch_fields_get(int unit, bcmlt_entry_handle_t eh, bcmi_lt_entry_t *entry)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < entry->nfields; i++) {

        if (!(entry->fields[i].flags & BCMI_LT_FIELD_F_GET)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh,
                                   entry->fields[i].fld_name,
                                   &(entry->fields[i].u.val)));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set default value for key fields for switch protocol LT.
 *
 * \param [in] unit Unit number.
 * \param [in] eh Entry handle.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
switch_protocol_key_default_set(int unit, bcmlt_entry_handle_t eh)
{
    int i;
    uint32_t field_num = 0, out_num, alloc_size;
    bcmlt_field_def_t *field_defs_array = NULL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit,
                                    PROTOCOL_PKT_FORWARD_TABLEs,
                                    0,
                                    NULL,
                                    &field_num));

    alloc_size = field_num * sizeof(bcmlt_field_def_t);
    field_defs_array = sal_alloc(alloc_size, "ltswSwitchProtoCtrl");
    SHR_NULL_CHECK(field_defs_array, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit,
                                    PROTOCOL_PKT_FORWARD_TABLEs,
                                    field_num,
                                    field_defs_array,
                                    &out_num));

    for (i = 0; i < field_num; i++) {
        /* Skip non-key fields. */
        if (!field_defs_array[i].key) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh,
                                   field_defs_array[i].name,
                                   field_defs_array[i].def));
    }

exit:
    SHR_FREE(field_defs_array);
    SHR_FUNC_EXIT();
}

static int
switch_protocol_control_validate(int unit, bcmlt_opcode_t opcode,
                                 bcm_switch_pkt_protocol_match_t *match,
                                 bcm_switch_pkt_control_action_t *action,
                                 int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    /* SIP < DIP and SIP = DIP cannot be matched together. */
    if ((match->fixed_hve_result_set_1 & 0x5) == 0x5) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* MAC_SA < MAC_DA and MAC_SA = MAC_DA cannot be matched together. */
    if ((match->fixed_hve_result_set_1 & 0xa) == 0xa) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (opcode == BCMLT_OPCODE_INSERT || opcode == BCMLT_OPCODE_UPDATE) {
        SHR_NULL_CHECK(action, SHR_E_PARAM);
        SHR_NULL_CHECK(priority, SHR_E_PARAM);

        if (*priority < BCMINT_SWITCH_PKT_CONTROL_RSVD_PRI) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (action->bpdu > 1 ||
            action->copy_to_cpu > 1 ||
            action->discard > 1 ||
            action->flood > 1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (opcode == BCMLT_OPCODE_LOOKUP) {
        SHR_NULL_CHECK(action, SHR_E_PARAM);
        SHR_NULL_CHECK(priority, SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse LT entry and construct API structure.
 *
 * \param [in] unit Unit Number.
 * \param [in] eh LT entry handle.
 * \param [out] match Protocol match structure.
 * \param [out] action Packet control action structure.
 * \param [out] priority Entry priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
switch_protocol_control_entry_parse(int unit, bcmlt_entry_handle_t eh,
                                    bcm_switch_pkt_protocol_match_t *match,
                                    bcm_switch_pkt_control_action_t *action,
                                    int *priority)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {VFIs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*1*/ {VFI_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*2*/ {MACDAs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3*/ {MACDA_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4*/ {ARP_RARP_OPCODEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*5*/ {ARP_RARP_OPCODE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*6*/ {ICMP_TYPEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*7*/ {ICMP_TYPE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*8*/ {IGMP_TYPEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*9*/ {IGMP_TYPE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*10*/ {L4_VALIDs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*11*/ {L4_VALID_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*12*/ {L4_SRC_PORTs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*13*/ {L4_SRC_PORT_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*14*/ {L4_DST_PORTs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*15*/ {L4_DST_PORT_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*16*/ {IP_LAST_PROTOCOLs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*17*/ {IP_LAST_PROTOCOL_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*18*/ {FIXED_HVE_RESULT_SET_1s, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*19*/ {FIXED_HVE_RESULT_SET_1_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*20*/ {FIXED_HVE_RESULT_SET_5s, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*21*/ {FIXED_HVE_RESULT_SET_5_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*22*/ {FLEX_HVE_RESULT_SET_1s, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*23*/ {FLEX_HVE_RESULT_SET_1_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*24*/ {TUNNEL_PROCESSING_RESULTS_1s, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*25*/ {TUNNEL_PROCESSING_RESULTS_1_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*26*/ {BPDUs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*27*/ {DROPs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*28*/ {FLOODs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*29*/ {COPY_TO_CPUs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*30*/ {ETHERTYPEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*31*/ {ETHERTYPE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*32*/ {L2_IIF_OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*33*/ {L2_IIF_OPAQUE_CTRL_ID_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*34*/ {ENTRY_PRIORITYs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*35*/ {L5_BYTES_0_1s, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*36*/ {L5_BYTES_0_1_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*37*/ {OUTER_L5_BYTES_0_1s, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*38*/ {OUTER_L5_BYTES_0_1_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*39*/ {OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    uint64_t mac = 0, mac_mask = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);
    SHR_NULL_CHECK(priority, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (switch_fields_get(unit, eh, &lt_entry));

    match->vfi      = field[0].u.val;
    match->vfi_mask = field[1].u.val;
    mac             = field[2].u.val;
    mac_mask        = field[3].u.val;
    bcmi_ltsw_util_uint64_to_mac(match->macda, &mac);
    bcmi_ltsw_util_uint64_to_mac(match->macda_mask, &mac_mask);
    match->arp_rarp_opcode      = field[4].u.val;
    match->arp_rarp_opcode_mask = field[5].u.val;
    match->icmp_type            = field[6].u.val;
    match->icmp_type_mask       = field[7].u.val;
    match->igmp_type            = field[8].u.val;
    match->igmp_type_mask       = field[9].u.val;
    match->l4_valid             = field[10].u.val;
    match->l4_valid_mask        = field[11].u.val;
    match->l4_src_port          = field[12].u.val;
    match->l4_src_port_mask     = field[13].u.val;
    match->l4_dst_port          = field[14].u.val;
    match->l4_dst_port_mask     = field[15].u.val;
    match->ip_last_protocol     = field[16].u.val;
    match->ip_last_protocol_mask        = field[17].u.val;
    match->fixed_hve_result_set_1       = field[18].u.val;
    match->fixed_hve_result_set_1_mask  = field[19].u.val;
    match->fixed_hve_result_set_5       = field[20].u.val;
    match->fixed_hve_result_set_5_mask  = field[21].u.val;
    match->flex_hve_result_set_1        = field[22].u.val;
    match->flex_hve_result_set_1_mask   = field[23].u.val;
    match->tunnel_processing_results_1  = field[24].u.val;
    match->tunnel_processing_results_1_mask = field[25].u.val;
    match->ethertype                    = field[30].u.val;
    match->ethertype_mask               = field[31].u.val;
    match->l2_iif_opaque_ctrl_id        = field[32].u.val;
    match->l2_iif_opaque_ctrl_id_mask   = field[33].u.val;
    match->l5_bytes_0_1                 = field[35].u.val;
    match->l5_bytes_0_1_mask            = field[36].u.val;
    match->outer_l5_bytes_0_1           = field[37].u.val;
    match->outer_l5_bytes_0_1_mask      = field[38].u.val;
    action->bpdu                        = field[26].u.val;
    action->discard                     = field[27].u.val;
    action->flood                       = field[28].u.val;
    action->copy_to_cpu                 = field[29].u.val;
    action->opaque_ctrl_id              = field[39].u.val;
    *priority = field[34].u.val - BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check it the entry is reserved one.
 *
 * \param [in] unit Unit Number.
 * \param [in] eh LT entry handle.
 * \param [out] is_reserved True for reserved entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
switch_protocol_control_entry_reserved_check(int unit, bcmlt_entry_handle_t eh,
                                             bool *is_reserved)
{
    uint64_t vfi = 0, vfi_mask = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(is_reserved, SHR_E_PARAM);

    *is_reserved = false;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VFIs, &vfi));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VFI_MASKs, &vfi_mask));

    /*
     * There is a trick on reserved entries.
     * The vfi is programmed as 1 and vfi_mask is programmed as 0, for TCAM
     * table, this is equal to vfi=0 & vfi_mask=0, so vfi is meaningless here.
     */
    if ((vfi == 1) && (vfi_mask == 0)) {
        *is_reserved = true;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the variant specific database.
 *
 * \param [in] unit Unit Number.
 * \param [out] switch_db Switch database structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_switch_db_get(int unit, bcmint_switch_db_t *switch_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(switch_db, SHR_E_PARAM);

    switch_db->drop_event_table_info = drop_event_id_map;
    switch_db->num_drop_event = COUNTOF(drop_event_id_map);

    switch_db->trace_event_table_info = trace_event_id_map;
    switch_db->num_trace_event = COUNTOF(trace_event_id_map);

    switch_db->ep_recirc_drop_event_table_info = ep_recirc_drop_event_id_map;
    switch_db->num_ep_recirc_drop_event = COUNTOF(ep_recirc_drop_event_id_map);

    switch_db->ep_recirc_trace_event_table_info = ep_recirc_trace_event_id_map;
    switch_db->num_ep_recirc_trace_event = COUNTOF(ep_recirc_trace_event_id_map);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Switch monitor trace event initialization.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_switch_mon_trace_event_init(int unit)
{
    int i;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {MON_ING_TRACE_EVENT_IDs, BCMI_LT_FIELD_F_SET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */ {CPUs, BCMI_LT_FIELD_F_SET, 0, {0}}
    };

    /* Comply with TD family behavior. */
    const char *trace_array[] = {
        CML_FLAGSs,
        PKT_INTEGRITY_CHECK_FAILEDs,
        PROTOCOL_PKTs,
        L2_DST_LOOKUPs,
        L3_DST_LOOKUPs,
        IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILEDs,
        VFPs,
        IFPs,
        IFP_METERs,
        DST_FPs,
        SVPs,
        EM_FTs,
        IVXLTs,
        MIRROR_SAMPLER_SAMPLEDs,
        MIRROR_SAMPLER_EGR_SAMPLEDs
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    /* Enable to copy to CPU. */
    fields[1].u.val = 1;

    for (i = 0; i < COUNTOF(trace_array); i++) {
        fields[0].u.sym_val = trace_array[i];
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_set(unit, MON_ING_TRACE_EVENTs, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init PROTOCOL_PKT_FORWARD_TABLE.
 *
 * It is possible that in INA some object/command bus container/fields are not
 * initialized properly before using, this will cause unexpected behavior.
 * Thus we need to insert dummy entries to TCAM tables to serve the purpose
 * of initializing these fields.
 *
 * In addition, in legacy chips, these are some default l2_user_entry entries
 * initialized for BPDU packets, so that BPDU packets can be taken as BPDU
 * and copied to cpu by default. To keep the same behavior as legacy chips,
 * we need to add several PROTOCOL_PKT_FORWARD_TABLE entries for BPDU copy to
 * cpu action during init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_init(int unit)
{
    int profile_index;
    bcmi_ltsw_sbr_profile_tbl_hdl_t profile_table = BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD;
    bcmi_ltsw_sbr_profile_ent_type_t profile_type = BCMI_LTSW_SBR_PET_NONE;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t protocol_field[] =
    {
        /*0*/ {VFIs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {VFI_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MACDAs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {MACDA_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {ARP_RARP_OPCODEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*5*/ {ARP_RARP_OPCODE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*6*/ {ICMP_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*7*/ {ICMP_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*8*/ {IGMP_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*9*/ {IGMP_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*10*/ {L4_VALIDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*11*/ {L4_VALID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*12*/ {L4_SRC_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*13*/ {L4_SRC_PORT_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {L4_DST_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*15*/ {L4_DST_PORT_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*16*/ {IP_LAST_PROTOCOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*17*/ {IP_LAST_PROTOCOL_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*18*/ {FIXED_HVE_RESULT_SET_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*19*/ {FIXED_HVE_RESULT_SET_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*20*/ {FIXED_HVE_RESULT_SET_5s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*21*/ {FIXED_HVE_RESULT_SET_5_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*22*/ {FLEX_HVE_RESULT_SET_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*23*/ {FLEX_HVE_RESULT_SET_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*24*/ {TUNNEL_PROCESSING_RESULTS_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*25*/ {TUNNEL_PROCESSING_RESULTS_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*26*/ {ETHERTYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*27*/ {ETHERTYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*28*/ {L2_IIF_OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*29*/ {L2_IIF_OPAQUE_CTRL_ID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*30*/ {L5_BYTES_0_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*31*/ {L5_BYTES_0_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*32*/ {BPDUs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*33*/ {DROPs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*34*/ {FLOODs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*35*/ {COPY_TO_CPUs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*36*/ {ENTRY_PRIORITYs, BCMI_LT_FIELD_F_SET, 0,
                {BCMINT_SWITCH_PKT_CONTROL_RSVD_PRI}},
        /*37*/ {OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*38*/ {STRENGTH_PROFILE_INDEXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcm_mac_t bpdu_mac = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x00};
    bcm_mac_t bpdu_mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint64_t mac = 0, mac_mask = 0;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, PROTOCOL_PKT_FORWARD_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (switch_protocol_key_default_set(unit, eh));

    lt_entry.fields = protocol_field;
    lt_entry.nfields = sizeof(protocol_field)/sizeof(protocol_field[0]);
    lt_entry.attr = 0;

    /*
     * For keyed TCAM table, inserting entry with same key(data and mask) will
     * return E_EXISTS error code, while it is ok with same mask but different
     * data. For packet hit, data=DATA&MASK has the same effect as data=DATA.
     * To avoid the case where user adds same entry as reserved one,
     * we play a trick on reserved entry (setting VFI=0x1, VFI_MASK=0)
     * so that user can insert any entry.
     */
    protocol_field[0].u.val = 0x1;

    /* Strength profile index for CLASS_ID. */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, profile_table, profile_type,
                                             &profile_index));
    protocol_field[38].u.val = profile_index;

    SHR_IF_ERR_EXIT
        (switch_fields_set(unit, eh, &lt_entry));

    /* Reserve last entry of PROTOCOL_PKT_FORWARD_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    /* Set default BPDU addresses (01:80:c2:00:00:00). */
    bcmi_ltsw_util_mac_to_uint64(&mac, bpdu_mac);
    bcmi_ltsw_util_mac_to_uint64(&mac_mask, bpdu_mac_mask);
    protocol_field[2].u.val = mac;
    protocol_field[3].u.val = mac_mask;
    protocol_field[32].u.val = 1;
    protocol_field[35].u.val = 1;
    protocol_field[36].u.val = BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET;
    SHR_IF_ERR_EXIT
        (switch_fields_set(unit, eh, &lt_entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    /* Set 01:80:c2:00:00:10. */
    bpdu_mac[5] = 0x10;
    bcmi_ltsw_util_mac_to_uint64(&mac, bpdu_mac);
    protocol_field[2].u.val = mac;
    SHR_IF_ERR_EXIT
        (switch_fields_set(unit, eh, &lt_entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    /* Set 01:80:c2:00:00:0x. */
    bpdu_mac[5] = 0x00;
    bpdu_mac_mask[5] = 0xf0;
    bcmi_ltsw_util_mac_to_uint64(&mac, bpdu_mac);
    bcmi_ltsw_util_mac_to_uint64(&mac_mask, bpdu_mac_mask);
    protocol_field[2].u.val = mac;
    protocol_field[3].u.val = mac_mask;
    SHR_IF_ERR_EXIT
        (switch_fields_set(unit, eh, &lt_entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    /* Set 01:80:c2:00:00:2x. */
    bpdu_mac[5] = 0x20;
    bcmi_ltsw_util_mac_to_uint64(&mac, bpdu_mac);
    protocol_field[2].u.val = mac;
    SHR_IF_ERR_EXIT
        (switch_fields_set(unit, eh, &lt_entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    /* Set 01:80:c2:00:00:01 for pfc and pause. */
    bpdu_mac[5] = 0x01;
    bpdu_mac_mask[5] = 0xff;
    bcmi_ltsw_util_mac_to_uint64(&mac, bpdu_mac);
    bcmi_ltsw_util_mac_to_uint64(&mac_mask, bpdu_mac_mask);
    protocol_field[2].u.val = mac;
    protocol_field[3].u.val = mac_mask;
    protocol_field[26].u.val = 0x8808;
    protocol_field[27].u.val = 0xffff;
    protocol_field[32].u.val = 0;
    protocol_field[33].u.val = 1;
    protocol_field[34].u.val = 0;
    protocol_field[35].u.val = 0;
    protocol_field[36].u.val = BCMINT_SWITCH_FLOW_CONTROL_PRI_OFFSET;
    SHR_IF_ERR_VERBOSE_EXIT
        (switch_fields_set(unit, eh, &lt_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_handle(int unit,
                                                   bcmlt_opcode_t opcode,
                                                   bcm_switch_pkt_protocol_match_t *match,
                                                   bcm_switch_pkt_control_action_t *action,
                                                   int *priority)
{
    int profile_index;
    bcmi_ltsw_sbr_profile_tbl_hdl_t profile_table = BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD;
    bcmi_ltsw_sbr_profile_ent_type_t profile_type = BCMI_LTSW_SBR_PET_NONE;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t protocol_field[] =
    {
        /*0*/ {VFIs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {VFI_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MACDAs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {MACDA_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {ARP_RARP_OPCODEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*5*/ {ARP_RARP_OPCODE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*6*/ {ICMP_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*7*/ {ICMP_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*8*/ {IGMP_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*9*/ {IGMP_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*10*/ {L4_VALIDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*11*/ {L4_VALID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*12*/ {L4_SRC_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*13*/ {L4_SRC_PORT_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {L4_DST_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*15*/ {L4_DST_PORT_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*16*/ {IP_LAST_PROTOCOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*17*/ {IP_LAST_PROTOCOL_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*18*/ {FIXED_HVE_RESULT_SET_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*19*/ {FIXED_HVE_RESULT_SET_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*20*/ {FIXED_HVE_RESULT_SET_5s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*21*/ {FIXED_HVE_RESULT_SET_5_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*22*/ {FLEX_HVE_RESULT_SET_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*23*/ {FLEX_HVE_RESULT_SET_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*24*/ {TUNNEL_PROCESSING_RESULTS_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*25*/ {TUNNEL_PROCESSING_RESULTS_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*26*/ {BPDUs, 0, 0, {0}},
        /*27*/ {DROPs, 0, 0, {0}},
        /*28*/ {FLOODs, 0, 0, {0}},
        /*29*/ {COPY_TO_CPUs, 0, 0, {0}},
        /*30*/ {ETHERTYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*31*/ {ETHERTYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*32*/ {L2_IIF_OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*33*/ {L2_IIF_OPAQUE_CTRL_ID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*34*/ {ENTRY_PRIORITYs, 0, 0, {0}},
        /*35*/ {L5_BYTES_0_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*36*/ {L5_BYTES_0_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*37*/ {OUTER_L5_BYTES_0_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*38*/ {OUTER_L5_BYTES_0_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*39*/ {OPAQUE_CTRL_IDs, 0, 0, {0}},
        /*40*/ {STRENGTH_PROFILE_INDEXs, 0, 0, {0}},
        /*41*/ {VFP_OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*42*/ {VFP_OPAQUE_CTRL_ID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*43*/ {VLAN_XLATE_OPAQUE_CTRL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*44*/ {VLAN_XLATE_OPAQUE_CTRL_ID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*45*/ {VLAN_XLATE_OPAQUE_CTRL_ID_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*46*/ {VLAN_XLATE_OPAQUE_CTRL_ID_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*47*/ {CLASS_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*48*/ {L2_IIF_OPAQUE_CTRL_ID_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*49*/ {L2_IIF_OPAQUE_CTRL_ID_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint64_t mac = 0, mac_mask = 0;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters. */
    SHR_IF_ERR_VERBOSE_EXIT
        (switch_protocol_control_validate(unit, opcode, match,
                                          action, priority));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PROTOCOL_PKT_FORWARD_TABLEs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (switch_protocol_key_default_set(unit, eh));

    lt_entry.fields = protocol_field;
    lt_entry.nfields = sizeof(protocol_field)/sizeof(protocol_field[0]);
    lt_entry.attr = 0;

    protocol_field[0].u.val = match->vfi & match->vfi_mask;
    protocol_field[1].u.val = match->vfi_mask;

    bcmi_ltsw_util_mac_to_uint64(&mac, match->macda);
    bcmi_ltsw_util_mac_to_uint64(&mac_mask, match->macda_mask);
    protocol_field[2].u.val = mac & mac_mask;
    protocol_field[3].u.val = mac_mask;

    protocol_field[4].u.val = match->arp_rarp_opcode &
                              match->arp_rarp_opcode_mask;
    protocol_field[5].u.val = match->arp_rarp_opcode_mask;

    protocol_field[6].u.val = match->icmp_type & match->icmp_type_mask;
    protocol_field[7].u.val = match->icmp_type_mask;

    protocol_field[8].u.val = match->igmp_type & match->igmp_type_mask;
    protocol_field[9].u.val = match->igmp_type_mask;

    protocol_field[10].u.val = match->l4_valid & match->l4_valid_mask;
    protocol_field[11].u.val = match->l4_valid_mask;

    protocol_field[12].u.val = match->l4_src_port &
                               match->l4_src_port_mask;
    protocol_field[13].u.val = match->l4_src_port_mask;

    protocol_field[14].u.val = match->l4_dst_port &
                               match->l4_dst_port_mask;
    protocol_field[15].u.val = match->l4_dst_port_mask;

    protocol_field[16].u.val = match->ip_last_protocol&
                               match->ip_last_protocol_mask;
    protocol_field[17].u.val = match->ip_last_protocol_mask;

    protocol_field[18].u.val = match->fixed_hve_result_set_1 &
                               match->fixed_hve_result_set_1_mask;
    protocol_field[19].u.val = match->fixed_hve_result_set_1_mask;

    protocol_field[20].u.val = match->fixed_hve_result_set_5 &
                               match->fixed_hve_result_set_5_mask;
    protocol_field[21].u.val = match->fixed_hve_result_set_5_mask;

    protocol_field[22].u.val = match->flex_hve_result_set_1 &
                               match->flex_hve_result_set_1_mask;
    protocol_field[23].u.val = match->flex_hve_result_set_1_mask;

    protocol_field[24].u.val = match->tunnel_processing_results_1 &
                               match->tunnel_processing_results_1_mask;
    protocol_field[25].u.val = match->tunnel_processing_results_1_mask;

    protocol_field[30].u.val = match->ethertype & match->ethertype_mask;
    protocol_field[31].u.val = match->ethertype_mask;
    protocol_field[32].u.val = match->l2_iif_opaque_ctrl_id &
                               match->l2_iif_opaque_ctrl_id_mask;
    protocol_field[33].u.val = match->l2_iif_opaque_ctrl_id_mask;

    protocol_field[35].u.val = match->l5_bytes_0_1 &
                               match->l5_bytes_0_1_mask;
    protocol_field[36].u.val = match->l5_bytes_0_1_mask;

    protocol_field[37].u.val = match->outer_l5_bytes_0_1 &
                               match->outer_l5_bytes_0_1_mask;
    protocol_field[38].u.val = match->outer_l5_bytes_0_1_mask;

    protocol_field[41].u.val = match->vfp_opaque_ctrl_id &
                               match->vfp_opaque_ctrl_id_mask;
    protocol_field[42].u.val = match->vfp_opaque_ctrl_id_mask;

    protocol_field[43].u.val = match->vlan_xlate_opaque_ctrl_id &
                               match->vlan_xlate_opaque_ctrl_id_mask;
    protocol_field[44].u.val = match->vlan_xlate_opaque_ctrl_id_mask;

    protocol_field[45].u.val = match->vlan_xlate_opaque_ctrl_id_1 &
                               match->vlan_xlate_opaque_ctrl_id_1_mask;
    protocol_field[46].u.val = match->vlan_xlate_opaque_ctrl_id_1_mask;

    protocol_field[48].u.val = match->l2_iif_opaque_ctrl_id_1 &
                               match->l2_iif_opaque_ctrl_id_1_mask;
    protocol_field[49].u.val = match->l2_iif_opaque_ctrl_id_1_mask;

    if (opcode == BCMLT_OPCODE_INSERT || opcode == BCMLT_OPCODE_UPDATE) {
        if (action->class_id > 0) {
            /* Strength profile index for CLASS_ID. */
            profile_type = BCMI_LTSW_SBR_PET_DEF;
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_sbr_profile_ent_index_get(unit, profile_table, profile_type,
                                                     &profile_index));
            protocol_field[40].flags = BCMI_LT_FIELD_F_SET;
            protocol_field[40].u.val = profile_index;
            protocol_field[47].flags = BCMI_LT_FIELD_F_SET;
            protocol_field[47].u.val = action->class_id;
        } else {
            /* Strength profile index for CLASS_ID. */
            profile_type = BCMI_LTSW_SBR_PET_NONE;
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_sbr_profile_ent_index_get(unit, profile_table, profile_type,
                                                     &profile_index));
            protocol_field[40].flags = BCMI_LT_FIELD_F_SET;
            protocol_field[40].u.val = profile_index;
        }

        if (action->bpdu) {
            protocol_field[26].u.val = 1;
            protocol_field[26].flags = BCMI_LT_FIELD_F_SET;
        } else {
            protocol_field[26].u.val = 0;
            protocol_field[26].flags = BCMI_LT_FIELD_F_SET;
        }
        if (action->discard) {
            protocol_field[27].u.val = 1;
            protocol_field[27].flags = BCMI_LT_FIELD_F_SET;
        } else {
            protocol_field[27].u.val = 0;
            protocol_field[27].flags = BCMI_LT_FIELD_F_SET;
        }
        if (action->flood) {
            protocol_field[28].u.val = 1;
            protocol_field[28].flags = BCMI_LT_FIELD_F_SET;
        } else {
            protocol_field[28].u.val = 0;
            protocol_field[28].flags = BCMI_LT_FIELD_F_SET;
        }
        if (action->copy_to_cpu) {
            protocol_field[29].u.val = 1;
            protocol_field[29].flags = BCMI_LT_FIELD_F_SET;
        } else {
            protocol_field[29].u.val = 0;
            protocol_field[29].flags = BCMI_LT_FIELD_F_SET;
        }
        protocol_field[39].flags = BCMI_LT_FIELD_F_SET;
        protocol_field[39].u.val = action->opaque_ctrl_id;

        protocol_field[34].u.val = *priority +
                                   BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET;
        protocol_field[34].flags = BCMI_LT_FIELD_F_SET;

        SHR_IF_ERR_VERBOSE_EXIT
            (switch_fields_set(unit, eh, &lt_entry));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh,
                                opcode,
                                BCMLT_PRIORITY_NORMAL));
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (switch_fields_set(unit, eh, &lt_entry));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh,
                                opcode,
                                BCMLT_PRIORITY_NORMAL));
    } else if (opcode == BCMLT_OPCODE_LOOKUP) {
        protocol_field[26].flags = BCMI_LT_FIELD_F_GET;
        protocol_field[27].flags = BCMI_LT_FIELD_F_GET;
        protocol_field[28].flags = BCMI_LT_FIELD_F_GET;
        protocol_field[29].flags = BCMI_LT_FIELD_F_GET;
        protocol_field[34].flags = BCMI_LT_FIELD_F_GET;
        protocol_field[39].flags = BCMI_LT_FIELD_F_GET;
        protocol_field[47].flags = BCMI_LT_FIELD_F_GET;

        SHR_IF_ERR_VERBOSE_EXIT
            (switch_fields_set(unit, eh, &lt_entry));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh,
                                opcode,
                                BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (switch_fields_get(unit, eh, &lt_entry));

        if (protocol_field[26].u.val == 1) {
            action->bpdu = 1;
        }
        if (protocol_field[27].u.val == 1) {
            action->discard = 1;
        }
        if (protocol_field[28].u.val == 1) {
            action->flood = 1;
        }
        if (protocol_field[29].u.val == 1) {
            action->copy_to_cpu = 1;
        }
        action->class_id = protocol_field[47].u.val;
        action->opaque_ctrl_id = protocol_field[39].u.val;
        *priority = protocol_field[34].u.val - BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init PKT_INTEGRITY_CHECK_TABLE.
 *
 * It is possible that in INA some object/command bus container/fields are not
 * initialized properly before using, this will cause unexpected behavior.
 * Thus we need to insert dummy entries to TCAM tables to serve the purpose
 * of initializing these fields.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_switch_pkt_integrity_check_init(int unit)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t lt_field[] = {
        /*0*/ {TUNNEL_PROCESSING_RESULTS_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {TUNNEL_PROCESSING_RESULTS_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MPLS_FLOW_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {MPLS_FLOW_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {MY_STATION_1_HITs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*5*/ {MY_STATION_1_HIT_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*6*/ {L4_VALIDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*7*/ {L4_VALID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*8*/ {ICMP_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*9*/ {ICMP_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*10*/ {IP_LAST_PROTOCOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*11*/ {IP_LAST_PROTOCOL_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*12*/ {TCP_HDR_LEN_AND_FLAGSs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*13*/ {TCP_HDR_LEN_AND_FLAGS_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {FIXED_HVE_RESULT_0s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*15*/ {FIXED_HVE_RESULT_0_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*16*/ {FIXED_HVE_RESULT_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*17*/ {FIXED_HVE_RESULT_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*18*/ {FIXED_HVE_RESULT_2s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*19*/ {FIXED_HVE_RESULT_2_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*20*/ {FIXED_HVE_RESULT_3s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*21*/ {FIXED_HVE_RESULT_3_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*22*/ {FIXED_HVE_RESULT_4s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*23*/ {FIXED_HVE_RESULT_4_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*24*/ {FIXED_HVE_RESULT_5s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*25*/ {FIXED_HVE_RESULT_5_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*26*/ {FLEX_HVE_RESULT_0s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*27*/ {FLEX_HVE_RESULT_0_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*28*/ {FLEX_HVE_RESULT_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*29*/ {FLEX_HVE_RESULT_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*30*/ {FLEX_HVE_RESULT_2s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*31*/ {FLEX_HVE_RESULT_2_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*32*/ {DROPs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*33*/ {COPY_TO_CPUs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*34*/ {ENTRY_PRIORITYs, BCMI_LT_FIELD_F_SET, 0,
                   {BCMINT_SWITCH_PKT_CONTROL_RSVD_PRI}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = lt_field;
    lt_entry.nfields = sizeof(lt_field)/sizeof(lt_field[0]);
    lt_entry.attr = 0;

    /*
     * Insert the reserved dummy entry which matches all packets with NOOP action.
     * The FIXED_HVE_RESULT_0=1 & FIXED_HVE_RESULT_0_MASK=0 makes this entry
     * invisible from customers.
     */
    lt_field[14].u.val = 0x1;

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_insert(unit, PKT_INTEGRITY_CHECK_TABLEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_switch_pkt_integrity_handle(int unit,
                                                        bcmlt_opcode_t opcode,
                                                        bcm_switch_pkt_integrity_match_t *match,
                                                        bcm_switch_pkt_control_action_t *action,
                                                        int *priority)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t lt_field[] = {
        /*0*/ {TUNNEL_PROCESSING_RESULTS_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {TUNNEL_PROCESSING_RESULTS_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MPLS_FLOW_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {MPLS_FLOW_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4*/ {MY_STATION_1_HITs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*5*/ {MY_STATION_1_HIT_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*6*/ {L4_VALIDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*7*/ {L4_VALID_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*8*/ {ICMP_TYPEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*9*/ {ICMP_TYPE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*10*/ {IP_LAST_PROTOCOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*11*/ {IP_LAST_PROTOCOL_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*12*/ {TCP_HDR_LEN_AND_FLAGSs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*13*/ {TCP_HDR_LEN_AND_FLAGS_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {FIXED_HVE_RESULT_0s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*15*/ {FIXED_HVE_RESULT_0_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*16*/ {FIXED_HVE_RESULT_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*17*/ {FIXED_HVE_RESULT_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*18*/ {FIXED_HVE_RESULT_2s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*19*/ {FIXED_HVE_RESULT_2_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*20*/ {FIXED_HVE_RESULT_3s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*21*/ {FIXED_HVE_RESULT_3_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*22*/ {FIXED_HVE_RESULT_4s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*23*/ {FIXED_HVE_RESULT_4_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*24*/ {FIXED_HVE_RESULT_5s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*25*/ {FIXED_HVE_RESULT_5_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*26*/ {FLEX_HVE_RESULT_0s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*27*/ {FLEX_HVE_RESULT_0_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*28*/ {FLEX_HVE_RESULT_1s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*29*/ {FLEX_HVE_RESULT_1_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*30*/ {FLEX_HVE_RESULT_2s, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*31*/ {FLEX_HVE_RESULT_2_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*32*/ {DROPs, 0, 0, {0}},
        /*33*/ {COPY_TO_CPUs, 0, 0, {0}},
        /*34*/ {ENTRY_PRIORITYs, 0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = lt_field;
    lt_entry.nfields = sizeof(lt_field)/sizeof(lt_field[0]);
    lt_entry.attr = 0;

    lt_field[0].u.val = match->tunnel_processing_results_1 &
                        match->tunnel_processing_results_1_mask;
    lt_field[1].u.val = match->tunnel_processing_results_1_mask;
    lt_field[2].u.val = match->mpls_flow_type & match->mpls_flow_type_mask;
    lt_field[3].u.val = match->mpls_flow_type_mask;
    lt_field[4].u.val = match->my_station_1_hit & match->my_station_1_hit_mask;
    lt_field[5].u.val = match->my_station_1_hit_mask;
    lt_field[6].u.val = match->l4_valid & match->l4_valid_mask;
    lt_field[7].u.val = match->l4_valid_mask;
    lt_field[8].u.val = match->icmp_type & match->icmp_type_mask;
    lt_field[9].u.val = match->icmp_type_mask;
    lt_field[10].u.val = match->ip_last_protocol & match->ip_last_protocol_mask;
    lt_field[11].u.val = match->ip_last_protocol_mask;
    lt_field[12].u.val = match->tcp_hdr_len_and_flags &
                         match->tcp_hdr_len_and_flags_mask;
    lt_field[13].u.val = match->tcp_hdr_len_and_flags_mask;
    lt_field[14].u.val = match->fixed_hve_result_0 &
                         match->fixed_hve_result_0_mask;
    lt_field[15].u.val = match->fixed_hve_result_0_mask;
    lt_field[16].u.val = match->fixed_hve_result_1 &
                         match->fixed_hve_result_1_mask;
    lt_field[17].u.val = match->fixed_hve_result_1_mask;
    lt_field[18].u.val = match->fixed_hve_result_2 &
                         match->fixed_hve_result_2_mask;
    lt_field[19].u.val = match->fixed_hve_result_2_mask;
    lt_field[20].u.val = match->fixed_hve_result_3 &
                         match->fixed_hve_result_3_mask;
    lt_field[21].u.val = match->fixed_hve_result_3_mask;
    lt_field[22].u.val = match->fixed_hve_result_4 &
                         match->fixed_hve_result_4_mask;
    lt_field[23].u.val = match->fixed_hve_result_4_mask;
    lt_field[24].u.val = match->fixed_hve_result_5 &
                         match->fixed_hve_result_5_mask;
    lt_field[25].u.val = match->fixed_hve_result_5_mask;
    lt_field[26].u.val = match->flex_hve_result_0 &
                         match->flex_hve_result_0_mask;
    lt_field[27].u.val = match->flex_hve_result_0_mask;
    lt_field[28].u.val = match->flex_hve_result_1 &
                         match->flex_hve_result_1_mask;
    lt_field[29].u.val = match->flex_hve_result_1_mask;
    lt_field[30].u.val = match->flex_hve_result_2 &
                         match->flex_hve_result_2_mask;
    lt_field[31].u.val = match->flex_hve_result_2_mask;

    if (opcode == BCMLT_OPCODE_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_delete(unit, PKT_INTEGRITY_CHECK_TABLEs, &lt_entry,
                                  NULL));
    } else if (opcode == BCMLT_OPCODE_LOOKUP) {
        lt_field[32].flags = BCMI_LT_FIELD_F_GET;
        lt_field[33].flags = BCMI_LT_FIELD_F_GET;
        lt_field[34].flags = BCMI_LT_FIELD_F_GET;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PKT_INTEGRITY_CHECK_TABLEs, &lt_entry,
                               NULL, NULL));
        if (lt_field[32].u.val == 1) {
            action->discard = 1;
        }
        if (lt_field[33].u.val == 1) {
            action->copy_to_cpu = 1;
        }
        *priority = lt_field[34].u.val - BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET;
    } else {
        if (action->discard) {
            lt_field[32].u.val = 1;
            lt_field[32].flags = BCMI_LT_FIELD_F_SET;
        } else {
            lt_field[32].u.val = 0;
            lt_field[32].flags = BCMI_LT_FIELD_F_SET;
        }
        if (action->copy_to_cpu) {
            lt_field[33].u.val = 1;
            lt_field[33].flags = BCMI_LT_FIELD_F_SET;
        } else {
            lt_field[33].u.val = 0;
            lt_field[33].flags = BCMI_LT_FIELD_F_SET;
        }
        lt_field[34].u.val = *priority + BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET;
        lt_field[34].flags = BCMI_LT_FIELD_F_SET;
        if (opcode == BCMLT_OPCODE_UPDATE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_update(unit, PKT_INTEGRITY_CHECK_TABLEs,
                                      &lt_entry, NULL));
        } else if (opcode == BCMLT_OPCODE_INSERT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_insert(unit, PKT_INTEGRITY_CHECK_TABLEs,
                                      &lt_entry, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_traverse(int unit,
    bcm_switch_pkt_protocol_control_traverse_cb cb,
    void *user_data)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcm_switch_pkt_protocol_match_t match;
    bcm_switch_pkt_control_action_t action;
    int priority, rv, cb_rv, dunit;
    bool is_reserved = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PROTOCOL_PKT_FORWARD_TABLEs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                              BCMLT_PRIORITY_NORMAL);

    while (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (switch_protocol_control_entry_reserved_check(unit,
                                                          eh,
                                                          &is_reserved));
        /* Skip reserved entries. */
        if (is_reserved) {
            /* Traverse to the next entry */
            rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL);
            continue;
        }
        bcm_switch_pkt_protocol_match_t_init(&match);
        bcm_switch_pkt_control_action_t_init(&action);
        SHR_IF_ERR_VERBOSE_EXIT
            (switch_protocol_control_entry_parse(unit,
                                                 eh,
                                                 &match,
                                                 &action,
                                                 &priority));
        /* Traverse to the next entry */
        rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL);

        cb_rv = cb(unit, &match, &action, &priority, user_data);
        if (SHR_FAILURE(cb_rv)) {
            SHR_ERR_EXIT(cb_rv);
        }
    }

    if (rv == SHR_E_NOT_FOUND) {
        /* If not found, it must be the last one */
        SHR_EXIT();
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_delete_all(int unit)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv, dunit;
    bool is_reserved = false;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PROTOCOL_PKT_FORWARD_TABLEs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                              BCMLT_PRIORITY_NORMAL);

    while (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (switch_protocol_control_entry_reserved_check(unit,
                                                          eh,
                                                          &is_reserved));
        /* Skip reserved entries. */
        if (is_reserved) {
            /* Traverse to the next entry */
            rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL);
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, PROTOCOL_PKT_FORWARD_TABLEs, &eh));
        /* Traverse to the next entry */
        rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE, BCMLT_PRIORITY_NORMAL);
    }

    if (rv == SHR_E_NOT_FOUND) {
        /* If not found, it must be the last one */
        SHR_EXIT();
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Switch driver function variable for bcm56780_a0 INA_2_4_13 device.
 */
static mbcm_ltsw_sc_drv_t bcm56780_a0_ina_2_4_13_ltsw_sc_drv = {
    .switch_db_get = bcm56780_a0_ina_2_4_13_ltsw_switch_db_get,
    .switch_mon_trace_event_init = bcm56780_a0_ina_2_4_13_ltsw_switch_mon_trace_event_init,
    .switch_protocol_init = bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_init,
    .switch_protocol_handle = bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_handle,
    .switch_pkt_integrity_check_init = bcm56780_a0_ina_2_4_13_ltsw_switch_pkt_integrity_check_init,
    .switch_pkt_integrity_handle = bcm56780_a0_ina_2_4_13_ltsw_switch_pkt_integrity_handle,
    .spc_trav = bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_traverse,
    .spc_del_all = bcm56780_a0_ina_2_4_13_ltsw_switch_protocol_delete_all
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_switch_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_sc_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_sc_drv));

exit:
    SHR_FUNC_EXIT();
}

