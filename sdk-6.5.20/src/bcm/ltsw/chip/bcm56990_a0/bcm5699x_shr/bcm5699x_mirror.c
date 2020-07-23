/*! \file bcm5699x_mirror.c
 *
 * BCM5699x Mirror subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/mirror.h>
#include <bcm_int/ltsw/mirror.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MIRROR

/*! Mirror container size. */
#define MIRROR_CONTAINER_SIZE 4

/*! Mirror session array size. */
#define MIRROR_SESSION_ARRAY_SIZE 1

/*! Mirror instance size. */
#define MIRROR_INSTANCE_SIZE 8

/*! Mirror-on-Drop destination size. */
#define MIRROR_MOD_DEST_SIZE 4

/*!
 * \brief Chip specific ingress mirror-ond-drop event mapping table.
 */
static bcmint_mirror_ingress_mod_event_map_info_t bcm5699x_ltsw_mirror_ingress_mod_event_map_tab[] = {
    {bcmMirrorIngModEventInvalidVlan, INVALID_VLANs},
    {bcmMirrorIngModEventTagUntagDiscard, TAG_UNTAG_DISCARDs},
    {bcmMirrorIngModEventInvalidTpid, INVALID_TPIDs},
    {bcmMirrorIngModEventPvlanVidMismatch, PVLAN_VID_MISMATCHs},
    {bcmMirrorIngModEventPortIngVlanMembership, PORT_ING_VLAN_MEMBERSHIPs},
    {bcmMirrorIngModEventVlanCcOrPbt, VLAN_CC_OR_PBTs},
    {bcmMirrorIngModEventVlanFp, VLAN_FPs},
    {bcmMirrorIngModEventMcIndexErr, MC_INDEX_ERRs},
    {bcmMirrorIngModEventMplsTtlCheckFail, MPLS_TTL_CHECK_FAILs},
    {bcmMirrorIngModEventMplsLabelMiss, MPLS_LABEL_MISSs},
    {bcmMirrorIngModEventMplsInvalidPayload, MPLS_INVALID_PAYLOADs},
    {bcmMirrorIngModEventMplsInvalidCw, MPLS_INVALID_CWs},
    {bcmMirrorIngModEventMplsInvalidAction, MPLS_INVALID_ACTIONs},
    {bcmMirrorIngModEventMplsGalLabel, MPLS_GAL_LABELs},
    {bcmMirrorIngModEventTnlDecapEcnDrop, TNL_DECAP_ECN_DROPs},
    {bcmMirrorIngModEventTimeSyncDrop, TIME_SYNC_DROPs},
    {bcmMirrorIngModEventSrcRoute, SRC_ROUTEs},
    {bcmMirrorIngModEventSpanningTreeState, SPANNING_TREE_STATEs},
    {bcmMirrorIngModEventTnlErr, TNL_ERRs},
    {bcmMirrorIngModEventProtectionDataDrop, PROTECTION_DATA_DROPs},
    {bcmMirrorIngModEventNhopDrop, NHOP_DROPs},
    {bcmMirrorIngModEventFpIngDrop, FP_ING_DROPs},
    {bcmMirrorIngModEventFpRedirectDrop, FP_REDIRECT_DROPs},
    {bcmMirrorIngModEventEcmpResolutionErr, ECMP_RESOLUTION_ERRs},
    {bcmMirrorIngModEventProtcolPktCtrlDrop, PROTCOL_PKT_CTRL_DROPs},
    {bcmMirrorIngModEventPortFilteringMode, PORT_FILTERING_MODEs},
    {bcmMirrorIngModEventL2MyStation, L2_MY_STATIONs},
    {bcmMirrorIngModEventMplsStage, MPLS_STAGEs},
    {bcmMirrorIngModEventSrcMacZero, SRC_MAC_ZEROs},
    {bcmMirrorIngModEventL3TtlErr, L3_TTL_ERRs},
    {bcmMirrorIngModEventL3HdrErr, L3_HDR_ERRs},
    {bcmMirrorIngModEventDstL3LookupMiss, DST_L3_LOOKUP_MISSs},
    {bcmMirrorIngModEventDstL3Discard, DST_L3_DISCARDs},
    {bcmMirrorIngModEventSrcL2StaticMove, SRC_L2_STATIC_MOVEs},
    {bcmMirrorIngModEventSrcL2Discard, SRC_L2_DISCARDs},
    {bcmMirrorIngModEventDstL2Discard, DST_L2_DISCARDs},
    {bcmMirrorIngModEventIpMcDrop, IP_MC_DROPs},
    {bcmMirrorIngModEventSrcMacEqualsDstMac, SRC_MAC_EQUALS_DST_MACs},
    {bcmMirrorIngModEventTrunkFailLoopback, TRUNK_FAIL_LOOPBACKs},
    {bcmMirrorIngModEventDosL4Attack, DOS_L4_ATTACKs},
    {bcmMirrorIngModEventDosL3Attack, DOS_L3_ATTACKs},
    {bcmMirrorIngModEventIpv6ProtocolErr, IPV6_PROTOCOL_ERRs},
    {bcmMirrorIngModEventIpv4ProtocolErr, IPV4_PROTOCOL_ERRs},
    {bcmMirrorIngModEventMacControlFrame, MAC_CONTROL_FRAMEs},
    {bcmMirrorIngModEventCompositeError, COMPOSITE_ERRORs},
    {bcmMirrorIngModEventCpuManagedLearning, CPU_MANAGED_LEARNINGs},
    {bcmMirrorIngModEventCfiOrL3Disable, CFI_OR_L3_DISABLEs},
    {bcmMirrorIngModEventBpdu, BPDUs},
    {bcmMirrorIngModEventBfdTerminated, BFD_TERMINATEDs},
    {bcmMirrorIngModEventBfdUnknownVerOrDiscard, BFD_UNKNOWN_VER_OR_DISCARDs},
    {bcmMirrorIngModEventBfdUnknownAchErr, BFD_UNKNOWN_ACH_ERRs},
    {bcmMirrorIngModEventBfdUnknownCtrlPkt, BFD_UNKNOWN_CTRL_PKTs},
    {bcmMirrorIngModEventDosL2, DOS_L2s},
    {bcmMirrorIngModEventParityErrDrop, PARITY_ERR_DROPs},
    {bcmMirrorIngModEventTrunkFailoverLoopbackDiscard, TRUNK_FAILOVER_LOOPBACK_DISCARDs},
    {bcmMirrorIngModEventTrunkFailoverLoopback, TRUNK_FAILOVER_LOOPBACKs},
    {bcmMirrorIngModEventDosFragment, DOS_FRAGMENTs},
    {bcmMirrorIngModEventDosIcmp, DOS_ICMPs},
    {bcmMirrorIngModEventRxTnlErr, RX_TNL_ERRs},
    {bcmMirrorIngModEventRxTnl, RX_TNLs},
    {bcmMirrorIngModEventRxDrop, RX_DROPs},
    {bcmMirrorIngModEventRxMcDrop, RX_MC_DROPs},
    {bcmMirrorIngModEventMcBridged, MC_BRIDGEDs},
    {bcmMirrorIngModEventRxPortDrop, RX_PORT_DROPs},
    {bcmMirrorIngModEventRxUc, RX_UCs},
    {bcmMirrorIngModEventIpv6McRouted, IPV6_MC_ROUTEDs},
    {bcmMirrorIngModEventRxIpv6HdrErr, RX_IPV6_HDR_ERRs},
    {bcmMirrorIngModEventRxIpv6, RX_IPV6s},
    {bcmMirrorIngModEventRxIpv6Discard, RX_IPV6_DISCARDs},
    {bcmMirrorIngModEventIpv4McRouted, IPV4_MC_ROUTEDs},
    {bcmMirrorIngModEventRxIpv4HdrErr, RX_IPV4_HDR_ERRs},
    {bcmMirrorIngModEventRxIpv4, RX_IPV4s},
    {bcmMirrorIngModEventRxIpv4Discard, RX_IPV4_DISCARDs},
    {bcmMirrorIngModEventVlanMembershipDrop, VLAN_MEMBERSHIP_DROPs},
    {bcmMirrorIngModEventVlanBlockedDrop, VLAN_BLOCKED_DROPs},
    {bcmMirrorIngModEventVlanStgDrop, VLAN_STG_DROPs},
    {bcmMirrorIngModEventNonucTrunkResolutionMask, NONUC_TRUNK_RESOLUTION_MASKs},
    {bcmMirrorIngModEventNonucMask, NONUC_MASKs},
    {bcmMirrorIngModEventMcDrop, MC_DROPs},
    {bcmMirrorIngModEventMacMask, MAC_MASKs},
    {bcmMirrorIngModEventIngEgrMask, ING_EGR_MASKs},
    {bcmMirrorIngModEventFpIngDelayedDrop, FP_ING_DELAYED_DROPs},
    {bcmMirrorIngModEventFpRedirectMask, FP_REDIRECT_MASKs},
    {bcmMirrorIngModEventEgrMask, EGR_MASKs},
    {bcmMirrorIngModEventSrcPortKnockoutDrop, SRC_PORT_KNOCKOUT_DROPs},
    {bcmMirrorIngModEventNonIpDiscard, NON_IP_DISCARDs},
    {bcmMirrorIngModEventMtuCheckFail, MTU_CHECK_FAILs},
    {bcmMirrorIngModEventBlockMaskDrop, BLOCK_MASK_DROPs},
    {bcmMirrorIngModEventBfdError, BFD_ERRORs},
    {bcmMirrorIngModEventBfdSlowpath, BFD_SLOWPATHs},
    {bcmMirrorIngModEventTimeSyncToCpu, TIME_SYNC_TO_CPUs},
    {bcmMirrorIngModEventIeee1588UnknownVersion, IEEE1588_UNKNOWN_VERSIONs},
    {bcmMirrorIngModEventMplsUnknownAchType, MPLS_UNKNOWN_ACH_TYPEs},
    {bcmMirrorIngModEventMplsIllegalReservedLabel, MPLS_ILLEGAL_RESERVED_LABELs},
    {bcmMirrorIngModEventMplsAlertLabelExposed, MPLS_ALERT_LABEL_EXPOSEDs},
    {bcmMirrorIngModEventMplsTtlCheckFailToCpu, MPLS_TTL_CHECK_FAIL_TO_CPUs},
    {bcmMirrorIngModEventMplsInvalidPayloadToCpu, MPLS_INVALID_PAYLOAD_TO_CPUs},
    {bcmMirrorIngModEventMplsInvalidActionToCpu, MPLS_INVALID_ACTION_TO_CPUs},
    {bcmMirrorIngModEventMplsLabelMissToCpu, MPLS_LABEL_MISS_TO_CPUs},
    {bcmMirrorIngModEventSflowFlexSample, SFLOW_FLEX_SAMPLEs},
    {bcmMirrorIngModEventSflowIngSample, SFLOW_ING_SAMPLEs},
    {bcmMirrorIngModEventSflowEgrSample, SFLOW_EGR_SAMPLEs},
    {bcmMirrorIngModEventTnlDecapEcnToCpu, TNL_DECAP_ECN_TO_CPUs},
    {bcmMirrorIngModEventTnlErrToCpu, TNL_ERR_TO_CPUs},
    {bcmMirrorIngModEventCbsmPrevented, CBSM_PREVENTEDs},
    {bcmMirrorIngModEventDlbMonitor, DLB_MONITORs},
    {bcmMirrorIngModEventEtrapMonitor, ETRAP_MONITORs},
    {bcmMirrorIngModEventFpIng, FP_INGs},
    {bcmMirrorIngModEventVfp, VFPs},
    {bcmMirrorIngModEventMcIndexErrToCpu, MC_INDEX_ERR_TO_CPUs},
    {bcmMirrorIngModEventIcmpRedirect, ICMP_REDIRECTs},
    {bcmMirrorIngModEventNhop, NHOPs},
    {bcmMirrorIngModEventMartianAddr, MARTIAN_ADDRs},
    {bcmMirrorIngModEventL3HdrErrToCpu, L3_HDR_ERR_TO_CPUs},
    {bcmMirrorIngModEventIpMcMiss, IP_MC_MISSs},
    {bcmMirrorIngModEventIpMcL3IifMismatch, IP_MC_L3_IIF_MISMATCHs},
    {bcmMirrorIngModEventL3SrcMove, L3_SRC_MOVEs},
    {bcmMirrorIngModEventL3DstMiss, L3_DST_MISSs},
    {bcmMirrorIngModEventL3SrcMiss, L3_SRC_MISSs},
    {bcmMirrorIngModEventMtuCheckFailToCpu, MTU_CHECK_FAIL_TO_CPUs},
    {bcmMirrorIngModEventIpOptionsPkt, IP_OPTIONS_PKTs},
    {bcmMirrorIngModEventTtl1, TTL_1s},
    {bcmMirrorIngModEventPbtNonucPkt, PBT_NONUC_PKTs},
    {bcmMirrorIngModEventL2, L2s},
    {bcmMirrorIngModEventL2Move, L2_MOVEs},
    {bcmMirrorIngModEventL2DstLookupFailure, L2_DST_LOOKUP_FAILUREs},
    {bcmMirrorIngModEventL2SrcLookupFailure, L2_SRC_LOOKUP_FAILUREs},
    {bcmMirrorIngModEventL2MyStationHit, L2_MY_STATION_HITs},
    {bcmMirrorIngModEventL2McMiss, L2_MC_MISSs},
    {bcmMirrorIngModEventParityError, PARITY_ERRORs},
    {bcmMirrorIngModEventPktTrace, PKT_TRACEs},
    {bcmMirrorIngModEventDosAttack, DOS_ATTACKs},
    {bcmMirrorIngModEventPvlanIdMismatchToCpu, PVLAN_ID_MISMATCH_TO_CPUs},
    {bcmMirrorIngModEventUnknownVlan, UNKNOWN_VLANs},
    {bcmMirrorIngModEventL2Proto, L2_PROTOs},
    {bcmMirrorIngModEventSrpProtocol, SRP_PROTOCOLs},
    {bcmMirrorIngModEventMmrpProtocol, MMRP_PROTOCOLs},
    {bcmMirrorIngModEventArpProtocol, ARP_PROTOCOLs},
    {bcmMirrorIngModEventIgmpProtocol, IGMP_PROTOCOLs},
    {bcmMirrorIngModEventDhcpProtocol, DHCP_PROTOCOLs},
    {bcmMirrorIngModEventIpMcRsvdProtocol, IP_MC_RSVD_PROTOCOLs},
    {bcmMirrorIngModEventBpduProtocol, BPDU_PROTOCOLs},
    {bcmMirrorIngModEventVfiIngAdaptLookupMiss, VFI_ING_ADAPT_LOOKUP_MISSs},
    {bcmMirrorIngModEventVfiIngAdaptLookupMissToCpu, VFI_ING_ADAPT_LOOKUP_MISS_TO_CPUs},
    {bcmMirrorIngModEventOlUlNhopEifRangeError, OVERLAY_UNDERLAY_NHOP_EIF_RANGE_ERRORs},
};


/******************************************************************************
 * Private functions
 */
static int
bcm5699x_ltsw_mirror_ctrl_info_init(int unit,
                                    bcmint_mirror_ctrl_info_t *mirror_ctrl)
{
    int alloc_size = 0;
    void *ptr = NULL;

    /*
    * TH4 Mirror H/W resources introduction:
    * 1. Supports 4 containers.
    * 2. Container source type can be port, svp, fp action, sFlow, etc.
    * 3. Each container provides an index.
    * 4. Supports 1 session table with 8 entries.
    * 5. The session table is shared by the 4 mirror containers.
    * 6. The index in container 'n' is used as index to select
    *    one of 8 entries in the session table.
    * 7. Each session entry provides Mirror destination port, encap index, etc.
    *
    * TH4 Mirror S/W resources introduction:
    * 1. The shared model indicates that the mirror session is shared by
    *    different mirror containers.
    * 2. The total number of session entries is 8 (1*8).
    * 3. Support Mirror Destination number is 8.
    */

    mirror_ctrl->container_num = MIRROR_CONTAINER_SIZE;
    mirror_ctrl->session_attrs.model = bcmintMirrorSessionModelShared;
    mirror_ctrl->session_attrs.session_array_num = MIRROR_SESSION_ARRAY_SIZE;
    mirror_ctrl->session_attrs.instance_num = MIRROR_INSTANCE_SIZE;
    mirror_ctrl->session_attrs.session_num =
        mirror_ctrl->session_attrs.session_array_num *
        mirror_ctrl->session_attrs.instance_num;
    mirror_ctrl->session_attrs.rsvd_instance_zero = false;
    mirror_ctrl->session_attrs.mod_dest_shared_encap = true;
    mirror_ctrl->dest_num = mirror_ctrl->session_attrs.session_array_num *
        mirror_ctrl->session_attrs.instance_num ;

    alloc_size = mirror_ctrl->container_num * sizeof(uint32_t);
    ptr = sal_alloc(alloc_size, "ltswMcUserFlags");
    if (ptr == NULL) {
        return SHR_E_MEMORY;
    }
    sal_memset(ptr, 0, alloc_size);
    mirror_ctrl->mc_st_flags = ptr;

    /*
    * List all supported source types for each mirror container.
    * In TH4, all containers support the same source types:
    * FP, INT, sFlow, DLB_MONITOR, ELEPHANT, ingress and egress port.
    */
    mirror_ctrl->mc_st_flags[0] = BCMINT_MIRROR_ST_F_FP_ING |
                                  BCMINT_MIRROR_ST_F_FP_EGR |
                                  BCMINT_MIRROR_ST_F_INT |
                                  BCMINT_MIRROR_ST_F_SAMPLER |
                                  BCMINT_MIRROR_ST_F_DLB_MONITOR |
                                  BCMINT_MIRROR_ST_F_ING_MOD |
                                  BCMINT_MIRROR_ST_F_ELEPHANT |
                                  BCMINT_MIRROR_ST_F_PORT_ING |
                                  BCMINT_MIRROR_ST_F_PORT_EGR;
    mirror_ctrl->mc_st_flags[1] = mirror_ctrl->mc_st_flags[0];
    mirror_ctrl->mc_st_flags[2] = mirror_ctrl->mc_st_flags[0];
    mirror_ctrl->mc_st_flags[3] = mirror_ctrl->mc_st_flags[0];

    mirror_ctrl->mod_dest_num = MIRROR_MOD_DEST_SIZE;
    mirror_ctrl->ingress_mod_event_map_tab =
        bcm5699x_ltsw_mirror_ingress_mod_event_map_tab;
    mirror_ctrl->ingress_mod_event_map_tab_size =
        COUNTOF(bcm5699x_ltsw_mirror_ingress_mod_event_map_tab);

    return SHR_E_NONE;
}

/*!
 * \brief Mirror sub driver functions for bcm5699x.
 */
static mbcm_ltsw_mirror_drv_t bcm5699x_mirror_sub_drv = {
    .mirror_ctrl_info_init = bcm5699x_ltsw_mirror_ctrl_info_init,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_mirror_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_drv_set(unit, &bcm5699x_mirror_sub_drv));

exit:
    SHR_FUNC_EXIT();
}

