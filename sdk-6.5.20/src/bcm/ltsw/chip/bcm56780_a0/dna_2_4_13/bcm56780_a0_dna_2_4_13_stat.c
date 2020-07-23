/*! \file bcm56780_a0_dna_2_4_13_stat.c
 *
 * BCM56780_A0 DNA_2_4_13 Stat Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/stat.h>
#include <bcm_int/ltsw/stat_int.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STAT

/*!
 * \brief Drop event mapping table.
 */
static const
bcmint_stat_map_info_t drop_event_id_map[] = {
    {
        .type = bcmPktDropEventIngressNoDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = NO_DROPs,
    },
    {
        .type = bcmPktDropEventIngressCmlFlagsDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = CML_FLAGS_DROPs,
    },
    {
        .type = bcmPktDropEventIngressL2SrcStaticMove,
        .table = CTR_ING_DROP_EVENTs,
        .field = L2_SRC_STATIC_MOVEs,
    },
    {
        .type = bcmPktDropEventIngressL2SrcDiscard,
        .table = CTR_ING_DROP_EVENTs,
        .field = L2_SRC_DISCARDs,
    },
    {
        .type = bcmPktDropEventIngressMacsaMulticast,
        .table = CTR_ING_DROP_EVENTs,
        .field = MACSA_MULTICASTs,
    },
    {
        .type = bcmPktDropEventIngressOuterTpidCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = OUTER_TPID_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressIncomingPvlanCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = INCOMING_PVLAN_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressPktIntegrityCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = PKT_INTEGRITY_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressProtocolPktDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = PROTOCOL_PKT_DROPs,
    },
    {
        .type = bcmPktDropEventIngressMembershipCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressSpanningTreeCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressL2DstLookupMiss,
        .table = CTR_ING_DROP_EVENTs,
        .field = L2_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktDropEventIngressL2DstDiscard,
        .table = CTR_ING_DROP_EVENTs,
        .field = L2_DST_DISCARDs,
    },
    {
        .type = bcmPktDropEventIngressL3DstLookupMiss,
        .table = CTR_ING_DROP_EVENTs,
        .field = L3_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktDropEventIngressL3DstDiscard,
        .table = CTR_ING_DROP_EVENTs,
        .field = L3_DST_DISCARDs,
    },
    {
        .type = bcmPktDropEventIngressL3HdrError,
        .table = CTR_ING_DROP_EVENTs,
        .field = L3_HDR_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressL3TtlError,
        .table = CTR_ING_DROP_EVENTs,
        .field = L3_TTL_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressIpmcL3IifOrRpaIdCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressTunnelTtlCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = TUNNEL_TTL_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressTunnelObjectValidationFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = TUNNEL_OBJECT_VALIDATION_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressTunnelAdaptDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = TUNNEL_ADAPT_DROPs,
    },
    {
        .type = bcmPktDropEventIngressPvlanDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = PVLAN_DROPs,
    },
    {
        .type = bcmPktDropEventIngressVfp,
        .table = CTR_ING_DROP_EVENTs,
        .field = VFPs,
    },
    {
        .type = bcmPktDropEventIngressIfp,
        .table = CTR_ING_DROP_EVENTs,
        .field = IFPs,
    },
    {
        .type = bcmPktDropEventIngressIfpMeter,
        .table = CTR_ING_DROP_EVENTs,
        .field = IFP_METERs,
    },
    {
        .type = bcmPktDropEventIngressDstFp,
        .table = CTR_ING_DROP_EVENTs,
        .field = DST_FPs,
    },
    {
        .type = bcmPktDropEventIngressMplsProtectionDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_PROTECTION_DROPs,
    },
    {
        .type = bcmPktDropEventIngressMplsLabelActionInvalid,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_LABEL_ACTION_INVALIDs,
    },
    {
        .type = bcmPktDropEventIngressMplsTermPolicySelectTableDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_TERM_POLICY_SELECT_TABLE_DROPs,
    },
    {
        .type = bcmPktDropEventIngressMplsReservedLabelExposed,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_RESERVED_LABEL_EXPOSEDs,
    },
    {
        .type = bcmPktDropEventIngressMplsTtlError,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_TTL_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressMplsEcnError,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_ECN_ERRORs,
    },
    {
        .type = bcmPktDropEventIngressEmFt,
        .table = CTR_ING_DROP_EVENTs,
        .field = EM_FTs,
    },
    {
        .type = bcmPktDropEventIngressIvxlt,
        .table = CTR_ING_DROP_EVENTs,
        .field = IVXLTs,
    },
    {
        .type = bcmPktDropEventIngressEpRecircDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = EP_RECIRC_MIRROR_DROPs,
    },
    {
        .type = bcmPktDropEventIngressExtIpv4GatewayTable,
        .table = CTR_ING_DROP_EVENTs,
        .field = MTOP_IPV4_GATEWAYs,
    },
    {
        .type = bcmPktDropEventIngressUrpfCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = URPF_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressSrcPortKnockoutDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = SRC_PORT_KNOCKOUT_DROPs,
    },
    {
        .type = bcmPktDropEventIngressLagFailoverPortDown,
        .table = CTR_ING_DROP_EVENTs,
        .field = LAG_FAILOVER_PORT_DOWNs,
    },
    {
        .type = bcmPktDropEventIngressSplitHorizonCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = SPLIT_HORIZON_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressDstLinkDown,
        .table = CTR_ING_DROP_EVENTs,
        .field = DST_LINK_DOWNs,
    },
    {
        .type = bcmPktDropEventIngressBlockMaskDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = BLOCK_MASK_DROPs,
    },
    {
        .type = bcmPktDropEventIngressL3MtuCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = L3_MTU_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressSeqNumCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = SEQ_NUM_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressL3IifEqL3Oif,
        .table = CTR_ING_DROP_EVENTs,
        .field = L3_IIF_EQ_L3_OIFs,
    },
    {
        .type = bcmPktDropEventIngressStormControlDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = STORM_CONTROL_DROPs,
    },
    {
        .type = bcmPktDropEventIngressEgrMembershipCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = EGR_MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressEgrSpanningTreeCheckFailed,
        .table = CTR_ING_DROP_EVENTs,
        .field = EGR_SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = bcmPktDropEventIngressDstPbmZero,
        .table = CTR_ING_DROP_EVENTs,
        .field = DST_PBM_ZEROs,
    },
    {
        .type = bcmPktDropEventIngressMplsCtrlPktDrop,
        .table = CTR_ING_DROP_EVENTs,
        .field = MPLS_CTRL_PKT_DROPs,
    },
    {
        .type = bcmPktDropEventEgressNoDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = NO_DROPs,
    },
    {
        .type = bcmPktDropEventEgressL2OifDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = L2_OIF_DROPs,
    },
    {
        .type = bcmPktDropEventEgressMembershipDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = MEMBERSHIP_DROPs,
    },
    {
        .type = bcmPktDropEventEgressDvpMembershipDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = DVP_MEMBERSHIP_DROPs,
    },
    {
        .type = bcmPktDropEventEgressTtlDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = TTL_DROPs,
    },
    {
        .type = bcmPktDropEventEgressL3SameIntfDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = L3_SAME_INTF_DROPs,
    },
    {
        .type = bcmPktDropEventEgressL2SamePortDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = L2_SAME_PORT_DROPs,
    },
    {
        .type = bcmPktDropEventEgressSplitHorizonDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = SPLIT_HORIZON_DROPs,
    },
    {
        .type = bcmPktDropEventEgressStgDisableDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = STG_DISABLE_DROPs,
    },
    {
        .type = bcmPktDropEventEgressStgBlockDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = STG_BLOCK_DROPs,
    },
    {
        .type = bcmPktDropEventEgressEfpDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EFP_DROPs,
    },
    {
        .type = bcmPktDropEventEgressEgrMeterDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EGR_METER_DROPs,
    },
    {
        .type = bcmPktDropEventEgressEgrMtuDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EGR_MTU_DROPs,
    },
    {
        .type = bcmPktDropEventEgressEgrVxltDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EGR_VXLT_DROPs,
    },
    {
        .type = bcmPktDropEventEgressEgrCellTooLarge,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EGR_CELL_TOO_LARGEs,
    },
    {
        .type = bcmPktDropEventEgressEgrCellTooSmall,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EGR_CELL_TOO_SMALLs,
    },
    {
        .type = bcmPktDropEventEgressQosRemarkingDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = QOS_REMARKING_DROPs,
    },
    {
        .type = bcmPktDropEventEgressSvpEqualDvpDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = SVP_EQUAL_DVP_DROPs,
    },
    {
        .type = bcmPktDropEventEgressInvalid1588Pkt,
        .table = CTR_EGR_DROP_EVENTs,
        .field = INVALID_1588_PKTs,
    },
    {
        .type = bcmPktDropEventEgressFlexEditorDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = FLEX_EDITOR_DROPs,
    },
    {
        .type = bcmPktDropEventEgressEpRecircDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = EP_RECIRC_DROPs,
    },
    {
        .type = bcmPktDropEventEgressIfaMdDeleteDrop,
        .table = CTR_EGR_DROP_EVENTs,
        .field = IFA_MD_DELETE_DROPs,
    }
};

/*!
 * \brief Trace event mapping table.
 */
static const
bcmint_stat_map_info_t trace_event_id_map[] = {
    {
        .type = bcmPktTraceEventIngressTraceDoNotCopyToCpu,
        .table = CTR_ING_TRACE_EVENTs,
        .field = NO_COPY_TO_CPUs,
    },
    {
        .type = bcmPktTraceEventIngressCmlFlags,
        .table = CTR_ING_TRACE_EVENTs,
        .field = CML_FLAGSs,
    },
    {
        .type = bcmPktTraceEventIngressL2SrcStaticMove,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L2_SRC_STATIC_MOVEs,
    },
    {
        .type = bcmPktTraceEventIngressL2SrcDiscard,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L2_SRC_DISCARDs,
    },
    {
        .type = bcmPktTraceEventIngressMacsaMulticast,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MACSA_MULTICASTs,
    },
    {
        .type = bcmPktTraceEventIngressPktIntegrityCheckFailed,
        .table = CTR_ING_TRACE_EVENTs,
        .field = PKT_INTEGRITY_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressProtocolPkt,
        .table = CTR_ING_TRACE_EVENTs,
        .field = PROTOCOL_PKTs,
    },
    {
        .type = bcmPktTraceEventIngressMembershipCheckFailed,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MEMBERSHIP_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressSpanningTreeCheckFailed,
        .table = CTR_ING_TRACE_EVENTs,
        .field = SPANNING_TREE_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressL2DstLookupMiss,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L2_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktTraceEventIngressL2DstLookup,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L2_DST_LOOKUPs,
    },
    {
        .type = bcmPktTraceEventIngressL3DstLookupMiss,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L3_DST_LOOKUP_MISSs,
    },
    {
        .type = bcmPktTraceEventIngressL3DstLookup,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L3_DST_LOOKUPs,
    },
    {
        .type = bcmPktTraceEventIngressL3HdrError,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L3_HDR_ERRORs,
    },
    {
        .type = bcmPktTraceEventIngressL3TtlError,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L3_TTL_ERRORs,
    },
    {
        .type = bcmPktTraceEventIngressIpmcL3IifOrRpaIdCheckFailed,
        .table = CTR_ING_TRACE_EVENTs,
        .field = IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressLearnCacheFull,
        .table = CTR_ING_TRACE_EVENTs,
        .field = LEARN_CACHE_FULLs,
    },
    {
        .type = bcmPktTraceEventIngressVfp,
        .table = CTR_ING_TRACE_EVENTs,
        .field = VFPs,
    },
    {
        .type = bcmPktTraceEventIngressIfp,
        .table = CTR_ING_TRACE_EVENTs,
        .field = IFPs,
    },
    {
        .type = bcmPktTraceEventIngressIfpMeter,
        .table = CTR_ING_TRACE_EVENTs,
        .field = IFP_METERs,
    },
    {
        .type = bcmPktTraceEventIngressDstFp,
        .table = CTR_ING_TRACE_EVENTs,
        .field = DST_FPs,
    },
    {
        .type = bcmPktTraceEventIngressSvp,
        .table = CTR_ING_TRACE_EVENTs,
        .field = SVPs,
    },
    {
        .type = bcmPktTraceEventIngressEmFt,
        .table = CTR_ING_TRACE_EVENTs,
        .field = EM_FTs,
    },
    {
        .type = bcmPktTraceEventIngressIvxlt,
        .table = CTR_ING_TRACE_EVENTs,
        .field = IVXLTs,
    },
    {
        .type = bcmPktTraceEventIngressMirrorSamplerSampled,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MIRROR_SAMPLER_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressMirrorSamplerEgrSampled,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MIRROR_SAMPLER_EGR_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressSerDrop,
        .table = CTR_ING_TRACE_EVENTs,
        .field = SER_DROPs,
    },
    {
        .type = bcmPktTraceEventIngressUrpfCheckFailed,
        .table = CTR_ING_TRACE_EVENTs,
        .field = URPF_CHECK_FAILEDs,
    },
    {
        .type = bcmPktTraceEventIngressL3IifEqL3Oif,
        .table = CTR_ING_TRACE_EVENTs,
        .field = L3_IIF_EQ_L3_OIFs,
    },
    {
        .type = bcmPktTraceEventIngressDlbEcmpMonitorEnOrMemberReassined,
        .table = CTR_ING_TRACE_EVENTs,
        .field = DLB_ECMP_MONITOR_EN_OR_MEMBER_REASSINEDs,
    },
    {
        .type = bcmPktTraceEventIngressDlbLagMonitorEnOrMemberReassined,
        .table = CTR_ING_TRACE_EVENTs,
        .field = DLB_LAG_MONITOR_EN_OR_MEMBER_REASSINEDs,
    },
    {
        .type = bcmPktTraceEventIngressMplsCtrlPktToCpu,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MPLS_CTRL_PKT_TO_CPUs,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit0,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_0s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit1,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_1s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit2,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_2s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit3,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_3s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit4,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_4s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit5,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_5s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit6,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_6s,
    },
    {
        .type = bcmPktTraceEventIngressMatchedRuleBit7,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MATCHED_RULE_BIT_7s,
    },
    {
        .type = bcmPktTraceEventIngressExtIpv4GatewayTable,
        .table = CTR_ING_TRACE_EVENTs,
        .field = MTOP_IPV4_GATEWAYs,
    },
    {
        .type = bcmPktTraceEventIngressDlbEcmpPktSampled,
        .table = CTR_ING_TRACE_EVENTs,
        .field = DLB_ECMP_PKT_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressDlbLagPktSampled,
        .table = CTR_ING_TRACE_EVENTs,
        .field = DLB_LAG_PKT_SAMPLEDs,
    },
    {
        .type = bcmPktTraceEventIngressTraceDoNotMirror,
        .table = CTR_ING_TRACE_EVENTs,
        .field = TRACE_DO_NOT_MIRRORs,
    },
    {
        .type = bcmPktTraceEventIngressTraceDoNotCopyToCpu,
        .table = CTR_ING_TRACE_EVENTs,
        .field = TRACE_DO_NOT_COPY_TO_CPUs,
    },
    {
        .type = bcmPktTraceEventIngressTraceDop,
        .table = CTR_ING_TRACE_EVENTs,
        .field = TRACE_DOPs,
    },
    {
        .type = bcmPktTraceEventEgressEfp,
        .table = CTR_EGR_TRACE_EVENTs,
        .field = EFPs,
    }
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the variant specific database.
 *
 * \param [in] unit Unit Number.
 * \param [out] stat_db Stat database structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_dna_2_4_13_ltsw_stat_db_get(int unit, bcmint_stat_db_t *stat_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stat_db, SHR_E_PARAM);

    stat_db->drop_event_table_info = drop_event_id_map;
    stat_db->num_drop_event = COUNTOF(drop_event_id_map);

    stat_db->trace_event_table_info = trace_event_id_map;
    stat_db->num_trace_event = COUNTOF(trace_event_id_map);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stat driver function variable for bcm56780_a0 DNA_2_4_13 device.
 */
static mbcm_ltsw_stat_drv_t bcm56780_a0_dna_2_4_13_ltsw_stat_drv = {
    .stat_db_get = bcm56780_a0_dna_2_4_13_ltsw_stat_db_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_stat_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_drv_set(unit, &bcm56780_a0_dna_2_4_13_ltsw_stat_drv));

exit:
    SHR_FUNC_EXIT();
}

