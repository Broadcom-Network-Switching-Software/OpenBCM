/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:     ft_drop.c
 * Purpose:  The purpose of this file is to set flowtracker drop reason
 *           group methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_drop.h>

#if defined(BCM_FLOWTRACKER_SUPPORT) && defined(BCM_FLOWTRACKER_V2_SUPPORT)

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
STATIC int
bcmi_ftv3_drop_reason_agg_ing_mmu_idx_is_invalid(int unit, int idx);

STATIC int
bcmi_ftv3_drop_reason_agg_egr_idx_is_invalid(int unit, int idx);
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/* Global flowtracker drop reason group sructure */
bcmi_ft_drop_reason_group_state_t
        bcmi_ft_drop_reason_group_state[BCM_MAX_NUM_UNITS];

/* Software Drop Reason Enum to Hw Bit encoding.
 * Indexed by drop reason enum to retreive hw bit value.
 * Below values are as per HR4 hardware.
 * If any enum value is changed, re-arrange this encoding array also. */
int16
bcmi_ftv2_drop_bit_sw_to_hw_encoding[bcmFlowtrackerDropReasonCount] = {
     -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
     31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  45,  46,  47,
     48,  49,  50,  51,  52,  64,  65,  66,  69,  70,  71,  72,  73,  75,  76,  77,
     78,  79,  80,  81,  82,  84,  85,  86,  87,  88,  89,  92,  93,  94,  96,  97,
     98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 113, 114, 115, 116,
    117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132,
    133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157,   0,   1,   2,   3,   4,   5,   6,   7,
      8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
     24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34
};

/* Software Drop Reason Ingress/Mmu Hw Bit to Enum encoding.
 * Indexed by drop hw bit value to retreive drop sw enum.
 * Below values are as per HR4 hardware.
 * If any enum value is changed, re-arrange this encoding array also. */
int16
bcmi_ftv2_ing_mmu_drop_bit_hw_to_sw_encoding[BCMI_FT_DROP_BIT_ING_MMU_MAX] = {
              bcmFlowtrackerDropReasonMmuIngressPortPG,              bcmFlowtrackerDropReasonMmuIngressPortPool,
           bcmFlowtrackerDropReasonMmuIngressPortGreen,            bcmFlowtrackerDropReasonMmuIngressPortYellow,
             bcmFlowtrackerDropReasonMmuIngressPortRed,              bcmFlowtrackerDropReasonMmuIngressHeadroom,
         bcmFlowtrackerDropReasonMmuEgressUCQueueGreen,          bcmFlowtrackerDropReasonMmuEgressUCQueueYellow,
           bcmFlowtrackerDropReasonMmuEgressUCQueueRed,      bcmFlowtrackerDropReasonMmuEgressUCQueueGroupGreen,
   bcmFlowtrackerDropReasonMmuEgressUCQueueGroupYellow,        bcmFlowtrackerDropReasonMmuEgressUCQueueGroupRed,
      bcmFlowtrackerDropReasonMmuEgressUCPortPoolGreen,       bcmFlowtrackerDropReasonMmuEgressUCPortPoolYellow,
        bcmFlowtrackerDropReasonMmuEgressUCPortPoolRed,            bcmFlowtrackerDropReasonMmuEgressUCPoolGreen,
         bcmFlowtrackerDropReasonMmuEgressUCPoolYellow,              bcmFlowtrackerDropReasonMmuEgressUCPoolRed,
     bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGreen,      bcmFlowtrackerDropReasonMmuEgressWREDUCQueueYellow,
       bcmFlowtrackerDropReasonMmuEgressWREDUCQueueRed,  bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupGreen,
bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupYellow,   bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupRed,
  bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolGreen,   bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolYellow,
    bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolRed,              bcmFlowtrackerDropReasonMmuEgressWREDUCMin,
           bcmFlowtrackerDropReasonMmuEgressWREDUCPool,          bcmFlowtrackerDropReasonMmuEgressRQEQueueGreen,
       bcmFlowtrackerDropReasonMmuEgressRQEQueueYellow,            bcmFlowtrackerDropReasonMmuEgressRQEQueueRed,
         bcmFlowtrackerDropReasonMmuEgressRQEPoolGreen,          bcmFlowtrackerDropReasonMmuEgressRQEPoolYellow,
           bcmFlowtrackerDropReasonMmuEgressRQEPoolRed,           bcmFlowtrackerDropReasonMmuEgressMCQueueGreen,
        bcmFlowtrackerDropReasonMmuEgressMCQueueYellow,             bcmFlowtrackerDropReasonMmuEgressMCQueueRed,
      bcmFlowtrackerDropReasonMmuEgressMCPortPoolGreen,       bcmFlowtrackerDropReasonMmuEgressMCPortPoolYellow,
        bcmFlowtrackerDropReasonMmuEgressMCPortPoolRed,            bcmFlowtrackerDropReasonMmuEgressMCPoolGreen,
         bcmFlowtrackerDropReasonMmuEgressMCPoolYellow,              bcmFlowtrackerDropReasonMmuEgressMCPoolRed,
                                                    -1,                         bcmFlowtrackerDropReasonMmuCFAP,
                bcmFlowtrackerDropReasonMmuIngressWRED,                     bcmFlowtrackerDropReasonMmuEgressUC,
                    bcmFlowtrackerDropReasonMmuIngress,                     bcmFlowtrackerDropReasonMmuEgressMC,
                  bcmFlowtrackerDropReasonMmuEgressRQE,                             bcmFlowtrackerDropReasonMmu,
                      bcmFlowtrackerDropReasonMmuIpMtu,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
               bcmFlowtrackerDropReasonIngControlFrame,                  bcmFlowtrackerDropReasonIngL3DosAttack,
              bcmFlowtrackerDropReasonIngHigigLoopback,                                                      -1,
                                                    -1,          bcmFlowtrackerDropReasonIngiHigigUnknownOpcode,
              bcmFlowtrackerDropReasonIngHigigHdrError,              bcmFlowtrackerDropReasonIngLagFailLoopback,
              bcmFlowtrackerDropReasonIngMacSAEqualsDA,                  bcmFlowtrackerDropReasonIngL4DosAttack,
                                                    -1,                     bcmFlowtrackerDropReasonIngNIVDot1p,
            bcmFlowtrackerDropReasonIngNIVVntagPresent,           bcmFlowtrackerDropReasonIngNIVVntagNotPresent,
             bcmFlowtrackerDropReasonIngNIVVntagFormat,        bcmFlowtrackerDropReasonIngUnknownSubtendingPort,
        bcmFlowtrackerDropReasonIngIPinIPTunnelProcess,             bcmFlowtrackerDropReasonIngMiMTunnelProcess,
       bcmFlowtrackerDropReasonIngTunnelInterfaceCheck,                                                      -1,
     bcmFlowtrackerDropReasonIngMplsInvalidControlWord,                bcmFlowtrackerDropReasonIngMplsGalNotBos,
         bcmFlowtrackerDropReasonIngMplsInvalidPayload,             bcmFlowtrackerDropReasonIngMplsEntropyLabel,
        bcmFlowtrackerDropReasonIngMplsLabelLookupMiss,            bcmFlowtrackerDropReasonIngMplsInvalidAction,
                                                    -1,                                                      -1,
             bcmFlowtrackerDropReasonIngTunnelTTLCheck,           bcmFlowtrackerDropReasonIngTunnelSHIMHDRError,
              bcmFlowtrackerDropReasonIngObjValidation,                                                      -1,
                bcmFlowtrackerDropReasonIngInvalidVlan,          bcmFlowtrackerDropReasonIngPortNotInVlanMember,
                bcmFlowtrackerDropReasonIngInvalidTpid,                bcmFlowtrackerDropReasonIngPvlanMismatch,
             bcmFlowtrackerDropReasonIngMACIPBindCheck,       bcmFlowtrackerDropReasonIngTunnelAdapt4LookupMiss,
     bcmFlowtrackerDropReasonIngTunnelAdapt3LookupMiss,       bcmFlowtrackerDropReasonIngTunnelAdapt2LookupMiss,
     bcmFlowtrackerDropReasonIngTunnelAdapt1LookupMiss,         bcmFlowtrackerDropReasonIngFieldProcessorLookup,
                    bcmFlowtrackerDropReasonIngFcoeVft,             bcmFlowtrackerDropReasonIngFcoeSrcBindCheck,
           bcmFlowtrackerDropReasonIngFcoeSrcFpmaCheck,                     bcmFlowtrackerDropReasonIngVfiPt2Pt,
                                                    -1,                                                      -1,
                                                    -1,    bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding,
            bcmFlowtrackerDropReasonIngMcastIndexError,               bcmFlowtrackerDropReasonIngCompositeError,
                       bcmFlowtrackerDropReasonIngBpdu,                     bcmFlowtrackerDropReasonIngProtocol,
                        bcmFlowtrackerDropReasonIngCml,                     bcmFlowtrackerDropReasonIngSrcRoute,
               bcmFlowtrackerDropReasonIngL2SrcDiscard,              bcmFlowtrackerDropReasonIngL2SrcStaticMove,
               bcmFlowtrackerDropReasonIngL2DstDiscard,                    bcmFlowtrackerDropReasonIngL3Disable,
                 bcmFlowtrackerDropReasonIngSrcMacZero,                       bcmFlowtrackerDropReasonIngVLANCC,
               bcmFlowtrackerDropReasonIngClassBasedSM,                  bcmFlowtrackerDropReasonIngTimeSyncPkt,
                bcmFlowtrackerDropReasonIngIPMCProcess,                    bcmFlowtrackerDropReasonIngMyStation,
              bcmFlowtrackerDropReasonIngPvlanVpFilter,            bcmFlowtrackerDropReasonIngPktFlowSelectMiss,
               bcmFlowtrackerDropReasonIngL3DstDiscard,               bcmFlowtrackerDropReasonIngTunnelDecapECN,
       bcmFlowtrackerDropReasonIngPktFlowSelectAction,                 bcmFlowtrackerDropReasonIngL3SrcDiscard,
         bcmFlowtrackerDropReasonIngFcoeZoneLookupMiss,                        bcmFlowtrackerDropReasonIngL3Ttl,
                      bcmFlowtrackerDropReasonIngL3Hdr,                        bcmFlowtrackerDropReasonIngL2Hdr,
            bcmFlowtrackerDropReasonIngL3SrcLookupMiss,              bcmFlowtrackerDropReasonIngL3DstLookupMiss,
                 bcmFlowtrackerDropReasonIngL2TtlError,                   bcmFlowtrackerDropReasonIngL2RpfCheck,
                        bcmFlowtrackerDropReasonIngPfm,                                                      -1,
            bcmFlowtrackerDropReasonIngTagUntagDiscard,               bcmFlowtrackerDropReasonIngEcmpResolution,
               bcmFlowtrackerDropReasonIngStormControl,                bcmFlowtrackerDropReasonIngFcoeZoneCheck,
bcmFlowtrackerDropReasonIngFPChangeL2FieldNoRedirectDrop,                    bcmFlowtrackerDropReasonIngMulticast,
                bcmFlowtrackerDropReasonIngTunnelError,                      bcmFlowtrackerDropReasonIngNextHop,
                  bcmFlowtrackerDropReasonIngUrpfCheck,                          bcmFlowtrackerDropReasonIngNat,
             bcmFlowtrackerDropReasonIngProtectionData,                 bcmFlowtrackerDropReasonIngStageIngress,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
};

/* Software Drop Reason Egress Hw Bit to Enum encoding.
 * Indexed by drop hw bit value to retreive drop sw enum.
 * Below values are as per HR4 hardware.
 * If any enum value is changed, re-arrange this encoding array also. */
int16
bcmi_ftv2_egr_drop_bit_hw_to_sw_encoding[BCMI_FT_DROP_BIT_EGR_MAX] = {
                 bcmFlowtrackerDropReasonEgrAdapt1Miss,                   bcmFlowtrackerDropReasonEgrAdapt2Miss,
                        bcmFlowtrackerDropReasonEgrTtl,            bcmFlowtrackerDropReasonEgrFwdDomainNotFound,
         bcmFlowtrackerDropReasonEgrFwdDomainNotMember,                     bcmFlowtrackerDropReasonEgrStgBlock,
                 bcmFlowtrackerDropReasonEgrIpmcL2Self,                bcmFlowtrackerDropReasonEgrNonUCastPrune,
             bcmFlowtrackerDropReasonEgrSrcVirtualPort,                   bcmFlowtrackerDropReasonEgrVplagPrune,
               bcmFlowtrackerDropReasonEgrSplitHorizon,               bcmFlowtrackerDropReasonEgrIpmcL3SelfIntf,
                   bcmFlowtrackerDropReasonEgrPurgePkt,                   bcmFlowtrackerDropReasonEgrStgDisable,
                    bcmFlowtrackerDropReasonEgrAgedPkt,                    bcmFlowtrackerDropReasonEgrL2MtuFail,
                     bcmFlowtrackerDropReasonEgrParity,                   bcmFlowtrackerDropReasonEgrPortEgress,
             bcmFlowtrackerDropReasonEgrFieldProcessor,           bcmFlowtrackerDropReasonEgrEPAdditionTooLarge,
                   bcmFlowtrackerDropReasonEgrNivPrune,             bcmFlowtrackerDropReasonEgrPacketFlowSelect,
             bcmFlowtrackerDropReasonEgrPacketTooSmall,             bcmFlowtrackerDropReasonEgrVisibilityPacket,
          bcmFlowtrackerDropReasonEgrInvalid1588Packet,                     bcmFlowtrackerDropReasonEgrEcnTable,
              bcmFlowtrackerDropReasonEgrHiGigReserved,               bcmFlowtrackerDropReasonEgrHiGig2Reserved,
               bcmFlowtrackerDropReasonEgrBadTunnelDip,                bcmFlowtrackerDropReasonEgrEsipLinkLocal,
                        bcmFlowtrackerDropReasonEgrCfi,               bcmFlowtrackerDropReasonEgrFlowSelectMiss,
                 bcmFlowtrackerDropReasonEgrProtection,                   bcmFlowtrackerDropReasonEgrFlexEditor,
                    bcmFlowtrackerDropReasonEgrQosCtrl,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
                                                    -1,                                                      -1,
};

/*
 * Function:
 *   bcmi_ftv2_drop_reason_ing_mmu_idx_is_invalid
 * Purpose:
 *   Check if the flowtracker ing_mmu drop reason
 *   group index is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   idx  - (IN) Ingress/Mmu Drop Reason Group Index
 * Returns:
 *   BCM_E_XXX
 */
static int
bcmi_ftv2_drop_reason_ing_mmu_idx_is_invalid(int unit, int idx)
{
    if (idx >= BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Ingress/Mmu drop reason id exceeds maximum allowed.\n")));
        return BCM_E_PARAM;
    }

    if (BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit) == NULL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Internal error.\n")));
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), idx))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Ingress/Mmu drop reason id not found.\n")));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_egr_idx_is_invalid
 * Purpose:
 *   Check if the flowtracker egr drop reason
 *   group index is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   idx -  (IN) Egress Drop Reason Group Index
 * Returns:
 *   BCM_E_XXX
 */
static int
bcmi_ftv2_drop_reason_egr_idx_is_invalid(int unit, int idx)
{
    if (idx >= BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Egress drop reason id exceeds maximum allowed.\n")));
        return BCM_E_PARAM;
    }

    if (BCMI_FT_DROP_REASON_EGR_BITMAP(unit) == NULL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Internal error.\n")));
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), idx))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Egress drop reason id not found.\n")));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_incr_ref_count
 * Purpose:
 *   Increment reference count for provided
 *   drop reason group id
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) flowtracker drop reason group id
 * Returns:
 *   -
 * Notes:
 *   This api will not validate drop reason group id.
 *   It should be validated by caller.
 */
void
bcmi_ftv2_drop_reason_group_incr_ref_count(int unit,
                                         bcm_flowtracker_drop_reason_group_t id)
{
    int idx = -1;

    idx = BCMI_FT_DROP_REASON_GROUP_INDEX_GET(id);
    if (BCMI_FT_DROP_REASON_GROUP_ID_IS_ING_MMU(id)) {
        BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_INC(unit, idx);
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_EGR(id)) {
        BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_INC(unit, idx);
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_ING_MMU(id)) {
        BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_INC(unit, idx);
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_EGR(id)) {
        BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_INC(unit, idx);
#endif
    }
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_decr_ref_count
 * Purpose:
 *   Decrement reference count for provided
 *   drop reason group id
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) flowtracker drop reason group id
 * Returns:
 *   -
 * Notes:
 *  This api will not validate drop reason group id.
 *  It should be validated by caller.
 */
void
bcmi_ftv2_drop_reason_group_decr_ref_count(int unit,
                                         bcm_flowtracker_drop_reason_group_t id)
{
    int idx = -1;

    idx = BCMI_FT_DROP_REASON_GROUP_INDEX_GET(id);
    if (BCMI_FT_DROP_REASON_GROUP_ID_IS_ING_MMU(id)) {
        if (BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT(unit, idx) > 0) {
            BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_DEC(unit, idx);
        }
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_EGR(id)) {
        if (BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT(unit, idx) > 0) {
            BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_DEC(unit, idx);
        }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_ING_MMU(id)) {
        if (BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT(unit, idx) > 0) {
            BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_DEC(unit, idx);
        }
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_EGR(id)) {
        if (BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT(unit, idx) > 0) {
            BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_DEC(unit, idx);
        }
#endif
    }
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_id_is_invalid
 * Purpose:
 *   Check if the flowtracker drop reason group
 *   id is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) flowtracker drop reason group id
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_drop_reason_group_id_is_invalid(int unit,
                                        bcm_flowtracker_drop_reason_group_t id)
{
    int idx = -1;

    idx = BCMI_FT_DROP_REASON_GROUP_INDEX_GET(id);

    if (BCMI_FT_DROP_REASON_GROUP_ID_IS_ING_MMU(id)) {
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_drop_reason_ing_mmu_idx_is_invalid(unit, idx));
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_EGR(id)) {
        BCM_IF_ERROR_RETURN
            (bcmi_ftv2_drop_reason_egr_idx_is_invalid(unit, idx));
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_ING_MMU(id)) {
        BCM_IF_ERROR_RETURN
            (bcmi_ftv3_drop_reason_agg_ing_mmu_idx_is_invalid(unit, idx));
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_EGR(id)) {
        BCM_IF_ERROR_RETURN
            (bcmi_ftv3_drop_reason_agg_egr_idx_is_invalid(unit, idx));
#endif
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Invalid drop reason group id.\n")));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_vector_get
 * Purpose:
 *   Get drop reason group vector value from
 *   flow checker.
 * Parameters:
 *   unit                      - (IN) BCM device id
 *   fc_info                   - (IN) FlowChecker Info
 *   dg_check_pri_vector_value - (IN) Primary Check Drop Reason
 *                                    Group Vector Value
 *   dg_check_sec_vector_value - (IN) Secondary Check Drop Reason
 *                                    Group Vector Value
 * Returns:
 *   BCM_E_XXX
 * Notes:
 *   this api should be called only for tracking param
 *   bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector
 *   bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector
 */
int
bcmi_ftv2_drop_reason_group_vector_get(int unit,
                          bcmi_flowtracker_flowchecker_info_t *fc_info,
                          int *dg_check_pri_vector_value,
                          int *dg_check_sec_vector_value)
{
    int iter = 0;

    /* Validate input params */
    if (fc_info == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Retreive primary check drop vector value */
    if (dg_check_pri_vector_value != NULL) {
        *dg_check_pri_vector_value = 0;
        for (iter = 0;
             iter < fc_info->check1.num_drop_reason_group_id; iter++) {
            *dg_check_pri_vector_value |= (1 <<
                    BCMI_FT_DROP_REASON_GROUP_INDEX_GET
                       (fc_info->check1.drop_reason_group_id[iter]));
        }
    }

    /* Retreive secondary check drop vector value */
    if (dg_check_sec_vector_value != NULL) {
        *dg_check_sec_vector_value = 0;
        for (iter = 0;
             iter < fc_info->check2.num_drop_reason_group_id; iter++) {
            *dg_check_sec_vector_value |= (1 <<
                    BCMI_FT_DROP_REASON_GROUP_INDEX_GET
                       (fc_info->check2.drop_reason_group_id[iter]));

        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_get_free_index
 * Purpose:
 *   Get free drop reason group index
 * Parameters:
 *   unit      - (IN)  BCM device id
 *   drop_type - (IN) drop reason type
 *   idx       - (OUT) check drop reason group idx
 * Returns:
 *   BCM_E_XXX
 */
static int
bcmi_ftv2_drop_reason_group_get_free_index(int unit,
                                           uint16 drop_type, int *idx)
{
    int iter = 0;
    int max_ing_mmu_idx = 0, max_egr_idx = 0;

    max_ing_mmu_idx = (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)) - 1;
    max_egr_idx = (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)) - 1;

    /* adjust indexes range to iterate */
    if (*idx != -1) {
        iter = *idx;
        max_ing_mmu_idx = *idx;
        max_egr_idx = *idx;
    }

    /* get free drop reason group index and reserve it */
    if (drop_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU) {
        for (; iter <= max_ing_mmu_idx; iter++) {
            if (!(SHR_BITGET(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), iter))) {
                SHR_BITSET(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), iter);
                *idx = iter;
                return BCM_E_NONE;
            }
        }
    } else if (drop_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR) {
        for (; iter <= max_egr_idx; iter++) {
            if (!(SHR_BITGET(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), iter))) {
                SHR_BITSET(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), iter);
                *idx = iter;
                return BCM_E_NONE;
            }
        }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (drop_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_ING_MMU) {
        for (; iter <= max_ing_mmu_idx; iter++) {
            if (!(SHR_BITGET(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), iter))) {
                SHR_BITSET(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), iter);
                *idx = iter;
                return BCM_E_NONE;
            }
        }
    } else if (drop_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_EGR) {
        for (; iter <= max_egr_idx; iter++) {
            if (!(SHR_BITGET(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), iter))) {
                SHR_BITSET(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), iter);
                *idx = iter;
                return BCM_E_NONE;
            }
        }
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
       "No more hardware resource to create drop reason group.\n")));
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_get type
 * Purpose:
 *   get type of drop reason
 * Parameters:
 *   unit        - (IN) BCM device id
 *   drop_reason - (IN) drop reason enum
 *   drop_type   - (OUT) drop reason type
 * Returns:
 *   BCM_E_XXX
 */
static int
bcmi_ftv2_drop_reason_get_type(int unit,
                               bcm_flowtracker_drop_reason_t drop_reason,
                               uint16 *drop_type)
{
    switch(drop_reason) {
        case bcmFlowtrackerDropReasonMmuIngressPortPG:
        case bcmFlowtrackerDropReasonMmuIngressPortPool:
        case bcmFlowtrackerDropReasonMmuIngressPortGreen:
        case bcmFlowtrackerDropReasonMmuIngressPortYellow:
        case bcmFlowtrackerDropReasonMmuIngressPortRed:
        case bcmFlowtrackerDropReasonMmuIngressHeadroom:
        case bcmFlowtrackerDropReasonMmuEgressUCQueueGreen:
        case bcmFlowtrackerDropReasonMmuEgressUCQueueYellow:
        case bcmFlowtrackerDropReasonMmuEgressUCQueueRed:
        case bcmFlowtrackerDropReasonMmuEgressUCQueueGroupGreen:
        case bcmFlowtrackerDropReasonMmuEgressUCQueueGroupYellow:
        case bcmFlowtrackerDropReasonMmuEgressUCQueueGroupRed:
        case bcmFlowtrackerDropReasonMmuEgressUCPortPoolGreen:
        case bcmFlowtrackerDropReasonMmuEgressUCPortPoolYellow:
        case bcmFlowtrackerDropReasonMmuEgressUCPortPoolRed:
        case bcmFlowtrackerDropReasonMmuEgressUCPoolGreen:
        case bcmFlowtrackerDropReasonMmuEgressUCPoolYellow:
        case bcmFlowtrackerDropReasonMmuEgressUCPoolRed:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGreen:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCQueueYellow:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCQueueRed:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupGreen:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupYellow:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCQueueGroupRed:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolGreen:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolYellow:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCPortPoolRed:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCMin:
        case bcmFlowtrackerDropReasonMmuEgressWREDUCPool:
        case bcmFlowtrackerDropReasonMmuEgressRQEQueueGreen:
        case bcmFlowtrackerDropReasonMmuEgressRQEQueueYellow:
        case bcmFlowtrackerDropReasonMmuEgressRQEQueueRed:
        case bcmFlowtrackerDropReasonMmuEgressRQEPoolGreen:
        case bcmFlowtrackerDropReasonMmuEgressRQEPoolYellow:
        case bcmFlowtrackerDropReasonMmuEgressRQEPoolRed:
        case bcmFlowtrackerDropReasonMmuEgressMCQueueGreen:
        case bcmFlowtrackerDropReasonMmuEgressMCQueueYellow:
        case bcmFlowtrackerDropReasonMmuEgressMCQueueRed:
        case bcmFlowtrackerDropReasonMmuEgressMCPortPoolGreen:
        case bcmFlowtrackerDropReasonMmuEgressMCPortPoolYellow:
        case bcmFlowtrackerDropReasonMmuEgressMCPortPoolRed:
        case bcmFlowtrackerDropReasonMmuEgressMCPoolGreen:
        case bcmFlowtrackerDropReasonMmuEgressMCPoolYellow:
        case bcmFlowtrackerDropReasonMmuEgressMCPoolRed:
        case bcmFlowtrackerDropReasonMmuCFAP:
        case bcmFlowtrackerDropReasonMmuIngressWRED:
        case bcmFlowtrackerDropReasonMmuEgressUC:
        case bcmFlowtrackerDropReasonMmuIngress:
        case bcmFlowtrackerDropReasonMmuEgressMC:
        case bcmFlowtrackerDropReasonMmuEgressRQE:
        case bcmFlowtrackerDropReasonMmu:
        case bcmFlowtrackerDropReasonMmuIpMtu:
        case bcmFlowtrackerDropReasonIngControlFrame:
        case bcmFlowtrackerDropReasonIngL3DosAttack:
        case bcmFlowtrackerDropReasonIngHigigLoopback:
        case bcmFlowtrackerDropReasonIngiHigigUnknownOpcode:
        case bcmFlowtrackerDropReasonIngHigigHdrError:
        case bcmFlowtrackerDropReasonIngLagFailLoopback:
        case bcmFlowtrackerDropReasonIngMacSAEqualsDA:
        case bcmFlowtrackerDropReasonIngL4DosAttack:
        case bcmFlowtrackerDropReasonIngNIVDot1p:
        case bcmFlowtrackerDropReasonIngNIVVntagPresent:
        case bcmFlowtrackerDropReasonIngNIVVntagNotPresent:
        case bcmFlowtrackerDropReasonIngNIVVntagFormat:
        case bcmFlowtrackerDropReasonIngUnknownSubtendingPort:
        case bcmFlowtrackerDropReasonIngIPinIPTunnelProcess:
        case bcmFlowtrackerDropReasonIngMiMTunnelProcess:
        case bcmFlowtrackerDropReasonIngTunnelInterfaceCheck:
        case bcmFlowtrackerDropReasonIngMplsInvalidControlWord:
        case bcmFlowtrackerDropReasonIngMplsGalNotBos:
        case bcmFlowtrackerDropReasonIngMplsInvalidPayload:
        case bcmFlowtrackerDropReasonIngMplsEntropyLabel:
        case bcmFlowtrackerDropReasonIngMplsLabelLookupMiss:
        case bcmFlowtrackerDropReasonIngMplsInvalidAction:
        case bcmFlowtrackerDropReasonIngTunnelTTLCheck:
        case bcmFlowtrackerDropReasonIngTunnelSHIMHDRError:
        case bcmFlowtrackerDropReasonIngObjValidation:
        case bcmFlowtrackerDropReasonIngInvalidVlan:
        case bcmFlowtrackerDropReasonIngPortNotInVlanMember:
        case bcmFlowtrackerDropReasonIngInvalidTpid:
        case bcmFlowtrackerDropReasonIngPvlanMismatch:
        case bcmFlowtrackerDropReasonIngMACIPBindCheck:
        case bcmFlowtrackerDropReasonIngTunnelAdapt4LookupMiss:
        case bcmFlowtrackerDropReasonIngTunnelAdapt3LookupMiss:
        case bcmFlowtrackerDropReasonIngTunnelAdapt2LookupMiss:
        case bcmFlowtrackerDropReasonIngTunnelAdapt1LookupMiss:
        case bcmFlowtrackerDropReasonIngFieldProcessorLookup:
        case bcmFlowtrackerDropReasonIngFcoeVft:
        case bcmFlowtrackerDropReasonIngFcoeSrcBindCheck:
        case bcmFlowtrackerDropReasonIngFcoeSrcFpmaCheck:
        case bcmFlowtrackerDropReasonIngVfiPt2Pt:
        case bcmFlowtrackerDropReasonIngSpanningTreeNotForwarding:
        case bcmFlowtrackerDropReasonIngMcastIndexError:
        case bcmFlowtrackerDropReasonIngCompositeError:
        case bcmFlowtrackerDropReasonIngBpdu:
        case bcmFlowtrackerDropReasonIngProtocol:
        case bcmFlowtrackerDropReasonIngCml:
        case bcmFlowtrackerDropReasonIngSrcRoute:
        case bcmFlowtrackerDropReasonIngL2SrcDiscard:
        case bcmFlowtrackerDropReasonIngL2SrcStaticMove:
        case bcmFlowtrackerDropReasonIngL2DstDiscard:
        case bcmFlowtrackerDropReasonIngL3Disable:
        case bcmFlowtrackerDropReasonIngSrcMacZero:
        case bcmFlowtrackerDropReasonIngVLANCC:
        case bcmFlowtrackerDropReasonIngClassBasedSM:
        case bcmFlowtrackerDropReasonIngTimeSyncPkt:
        case bcmFlowtrackerDropReasonIngIPMCProcess:
        case bcmFlowtrackerDropReasonIngMyStation:
        case bcmFlowtrackerDropReasonIngPvlanVpFilter:
        case bcmFlowtrackerDropReasonIngPktFlowSelectMiss:
        case bcmFlowtrackerDropReasonIngL3DstDiscard:
        case bcmFlowtrackerDropReasonIngTunnelDecapECN:
        case bcmFlowtrackerDropReasonIngPktFlowSelectAction:
        case bcmFlowtrackerDropReasonIngL3SrcDiscard:
        case bcmFlowtrackerDropReasonIngFcoeZoneLookupMiss:
        case bcmFlowtrackerDropReasonIngL3Ttl:
        case bcmFlowtrackerDropReasonIngL3Hdr:
        case bcmFlowtrackerDropReasonIngL2Hdr:
        case bcmFlowtrackerDropReasonIngL3SrcLookupMiss:
        case bcmFlowtrackerDropReasonIngL3DstLookupMiss:
        case bcmFlowtrackerDropReasonIngL2TtlError:
        case bcmFlowtrackerDropReasonIngL2RpfCheck:
        case bcmFlowtrackerDropReasonIngPfm:
        case bcmFlowtrackerDropReasonIngTagUntagDiscard:
        case bcmFlowtrackerDropReasonIngEcmpResolution:
        case bcmFlowtrackerDropReasonIngStormControl:
        case bcmFlowtrackerDropReasonIngFcoeZoneCheck:
        case bcmFlowtrackerDropReasonIngFPChangeL2FieldNoRedirectDrop:
        case bcmFlowtrackerDropReasonIngMulticast:
        case bcmFlowtrackerDropReasonIngTunnelError:
        case bcmFlowtrackerDropReasonIngNextHop:
        case bcmFlowtrackerDropReasonIngUrpfCheck:
        case bcmFlowtrackerDropReasonIngNat:
        case bcmFlowtrackerDropReasonIngProtectionData:
        case bcmFlowtrackerDropReasonIngStageIngress:
            *drop_type = BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU;
            break;
        case bcmFlowtrackerDropReasonEgrAdapt1Miss:
        case bcmFlowtrackerDropReasonEgrAdapt2Miss:
        case bcmFlowtrackerDropReasonEgrTtl:
        case bcmFlowtrackerDropReasonEgrFwdDomainNotFound:
        case bcmFlowtrackerDropReasonEgrFwdDomainNotMember:
        case bcmFlowtrackerDropReasonEgrStgBlock:
        case bcmFlowtrackerDropReasonEgrIpmcL2Self:
        case bcmFlowtrackerDropReasonEgrNonUCastPrune:
        case bcmFlowtrackerDropReasonEgrSrcVirtualPort:
        case bcmFlowtrackerDropReasonEgrVplagPrune:
        case bcmFlowtrackerDropReasonEgrSplitHorizon:
        case bcmFlowtrackerDropReasonEgrIpmcL3SelfIntf:
        case bcmFlowtrackerDropReasonEgrPurgePkt:
        case bcmFlowtrackerDropReasonEgrStgDisable:
        case bcmFlowtrackerDropReasonEgrAgedPkt:
        case bcmFlowtrackerDropReasonEgrL2MtuFail:
        case bcmFlowtrackerDropReasonEgrParity:
        case bcmFlowtrackerDropReasonEgrPortEgress:
        case bcmFlowtrackerDropReasonEgrFieldProcessor:
        case bcmFlowtrackerDropReasonEgrEPAdditionTooLarge:
        case bcmFlowtrackerDropReasonEgrNivPrune:
        case bcmFlowtrackerDropReasonEgrPacketFlowSelect:
        case bcmFlowtrackerDropReasonEgrPacketTooSmall:
        case bcmFlowtrackerDropReasonEgrVisibilityPacket:
        case bcmFlowtrackerDropReasonEgrInvalid1588Packet:
        case bcmFlowtrackerDropReasonEgrEcnTable:
        case bcmFlowtrackerDropReasonEgrHiGigReserved:
        case bcmFlowtrackerDropReasonEgrHiGig2Reserved:
        case bcmFlowtrackerDropReasonEgrBadTunnelDip:
        case bcmFlowtrackerDropReasonEgrEsipLinkLocal:
        case bcmFlowtrackerDropReasonEgrCfi:
        case bcmFlowtrackerDropReasonEgrFlowSelectMiss:
        case bcmFlowtrackerDropReasonEgrProtection:
        case bcmFlowtrackerDropReasonEgrFlexEditor:
        case bcmFlowtrackerDropReasonEgrQosCtrl:
            *drop_type = BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_param_sanity
 * Purpose:
 *   validate drop reasons and return type it belongs to
 * Parameters:
 *   unit              - (IN) BCM device id
 *   drop_reasons      - (IN) drop reasons
 *   drop_reason_type  - (OUT) drop reason type
 * Returns:
 *   BCM_E_XXX
 */
static int
bcmi_ftv2_drop_reason_group_param_sanity(int unit,
                    bcm_flowtracker_drop_reasons_t drop_reasons,
                    uint16 *drop_reason_type)
{
    int idx = 0;
    uint16 first_flag = 0, drop_type = 0;

    /* check if all reasons mentions below to same stage in FT */
    for (idx = 0; idx < bcmFlowtrackerDropReasonCount; idx++) {
        if (BCM_FLOWTRACKER_DROP_REASON_GET(drop_reasons, idx)) {

            BCM_IF_ERROR_RETURN
                (bcmi_ftv2_drop_reason_get_type(unit, idx, &drop_type));

            if (!first_flag) {
                first_flag = 1;
                *drop_reason_type = drop_type;
            } else {
                if (drop_type != *drop_reason_type) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "Drop Reasons across different stages cannot be "
                       "mixed.\n")));
                    return BCM_E_PARAM;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_init
 * Purpose:
 *   Initialize software state of flowtracker
 *   drop reason group
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ftv2_drop_reason_group_init(int unit)
{
    int max_ing_mmu_idx = 0, max_egr_idx = 0;

    /* cleanup if db exist already */
    bcmi_ftv2_drop_reason_group_cleanup(unit);

    /* first set max ing & egr group id allowed in system */
    max_ing_mmu_idx = BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) =
                      BCMI_FT_DROP_REASON_GROUP_ID_ING_MMU_MAX(unit);
    max_egr_idx = BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) =
                      BCMI_FT_DROP_REASON_GROUP_ID_EGR_MAX(unit);

    /* allocate space for ingress drop reason group id bitmap */
    BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit) =
                 sal_alloc(SHR_BITALLOCSIZE(max_ing_mmu_idx),
                 "flowtracker drop reason ing_mmu idx bitmap");
    if (BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), 0,
                         SHR_BITALLOCSIZE(max_ing_mmu_idx));

    /* allocate space for egress drop reason group id bitmap */
    BCMI_FT_DROP_REASON_EGR_BITMAP(unit) =
                sal_alloc(SHR_BITALLOCSIZE(max_egr_idx),
                "flowtracker drop reason egr idx bitmap");
    if (BCMI_FT_DROP_REASON_EGR_BITMAP(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), 0,
                         SHR_BITALLOCSIZE(max_egr_idx));

    /* allocate space for ingress group id refcount */
    BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit) = (uint16 *)
                sal_alloc((max_ing_mmu_idx * sizeof(uint16)),
                "flowtracker ing mmu drop reason group ref count");
    if (BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit), 0,
                                      (max_ing_mmu_idx * sizeof(uint16)));

    /* allocate space for egress group id refcount */
    BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit) = (uint16 *)
                sal_alloc((max_egr_idx * sizeof(uint16)),
                "flowtracker egr drop reason group ref count");
    if (BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit) == NULL) {
        goto error;
    }
    sal_memset(BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit), 0,
                                      (max_egr_idx * sizeof(uint16)));

    /* assign drop reason sw <-> hw encoding array */
    BCMI_FT_DROP_REASON_SW_TO_HW_STATE(unit) =
                              bcmi_ftv2_drop_bit_sw_to_hw_encoding;
    BCMI_FT_DROP_REASON_ING_MMU_HW_TO_SW_STATE(unit) =
                              bcmi_ftv2_ing_mmu_drop_bit_hw_to_sw_encoding;
    BCMI_FT_DROP_REASON_EGR_HW_TO_SW_STATE(unit) =
                              bcmi_ftv2_egr_drop_bit_hw_to_sw_encoding;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* allocate space for ingress drop reason group id bitmap */
        BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit) =
            sal_alloc(SHR_BITALLOCSIZE(max_ing_mmu_idx),
                    "flowtracker drop reason agg_ing_mmu idx bitmap");
        if (BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit) == NULL) {
            goto error;
        }
        sal_memset(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), 0,
                SHR_BITALLOCSIZE(max_ing_mmu_idx));

        /* allocate space for egress drop reason group id bitmap */
        BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit) =
            sal_alloc(SHR_BITALLOCSIZE(max_egr_idx),
                    "flowtracker drop reason agg_egr idx bitmap");
        if (BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit) == NULL) {
            goto error;
        }
        sal_memset(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), 0,
                SHR_BITALLOCSIZE(max_egr_idx));

        /* allocate space for ingress group id refcount */
        BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit) = (uint16 *)
            sal_alloc((max_ing_mmu_idx * sizeof(uint16)),
                    "flowtracker agg ing mmu drop reason group ref count");
        if (BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit) == NULL) {
            goto error;
        }
        sal_memset(BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit), 0,
                (max_ing_mmu_idx * sizeof(uint16)));

        /* allocate space for egress group id refcount */
        BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit) = (uint16 *)
            sal_alloc((max_egr_idx * sizeof(uint16)),
                    "flowtracker egr drop reason group ref count");
        if (BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit) == NULL) {
            goto error;
        }
        sal_memset(BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit), 0,
                (max_egr_idx * sizeof(uint16)));
    }
#endif
    return BCM_E_NONE;

error:
    bcmi_ftv2_drop_reason_group_cleanup(unit);
    return BCM_E_MEMORY;
}

/*
 * Function:
 *   bcmi_ft_drop_reason_group_cleanup
 * Purpose:
 *   cleanup software state of flowtracker
 *   drop reason group id.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
void
bcmi_ftv2_drop_reason_group_cleanup(int unit)
{
    if (BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit) != NULL) {
        sal_free(BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit));
        BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit) = NULL;
    }

    if (BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit) != NULL) {
        sal_free(BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit));
        BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit) = NULL;
    }

    if (BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit) != NULL) {
        sal_free(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit));
        BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit) = NULL;
    }

    if (BCMI_FT_DROP_REASON_EGR_BITMAP(unit) != NULL) {
        sal_free(BCMI_FT_DROP_REASON_EGR_BITMAP(unit));
        BCMI_FT_DROP_REASON_EGR_BITMAP(unit) = NULL;
    }

    BCMI_FT_DROP_REASON_SW_TO_HW_STATE(unit) = NULL;
    BCMI_FT_DROP_REASON_ING_MMU_HW_TO_SW_STATE(unit) = NULL;
    BCMI_FT_DROP_REASON_EGR_HW_TO_SW_STATE(unit) = NULL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit) != NULL) {
            sal_free(BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit));
            BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit) = NULL;
        }

        if (BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit) != NULL) {
            sal_free(BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit));
            BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit) = NULL;
        }

        if (BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit) != NULL) {
            sal_free(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit));
            BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit) = NULL;
        }

        if (BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit) != NULL) {
            sal_free(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit));
            BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit) = NULL;
        }
    }
#endif
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_create
 * Purpose:
 *   Create a flowtracker drop reason group
 * Parameters:
 *   unit         - (IN)  BCM device id
 *   drop_reasons - (IN)  Drop Reasons
 *   id           - (OUT) Drop Reason Group Id
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_drop_reason_group_create(int unit,
                                   uint32 flags,
                                   bcm_flowtracker_drop_reasons_t drop_reasons,
                                   bcm_flowtracker_drop_reason_group_t *id)
{
    int rv = BCM_E_NONE;
    int idx = -1, iter = 0;
    int16 drop_hw_bit = 0;
    uint32 hw_bit_map[6] = {0};
    uint16 drop_reason_type = 0;
    bsc_dt_drop_mask_2_entry_t egr_entry;
    bsc_dt_drop_mask_1_entry_t ing_mmu_entry;

    /* input param check */
    if (id == NULL) {
        return BCM_E_PARAM;
    }

    /* initialize structures */
    sal_memset(&ing_mmu_entry, 0, sizeof(ing_mmu_entry));
    sal_memset(&egr_entry, 0, sizeof(egr_entry));

    /* drop reason params sanity */
    BCM_IF_ERROR_RETURN
            (bcmi_ftv2_drop_reason_group_param_sanity(unit,
                                            drop_reasons, &drop_reason_type));

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (flags & BCM_FLOWTRACKER_DROP_REASON_GROUP_AGGREGATE_MMU) {
            if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU) {
                drop_reason_type = BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_ING_MMU;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Drop Reasons does not belong to group type.\n")));
                return BCM_E_CONFIG;
            }
        } else if (flags & BCM_FLOWTRACKER_DROP_REASON_GROUP_AGGREGATE_EGRESS) {
            if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR) {
                drop_reason_type = BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_EGR;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Drop Reasons does not belong to group type.\n")));
                return BCM_E_CONFIG;
            }
        }
    } else
#endif
    {
        if (flags) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "group type is not supported.\n")));
            return BCM_E_PARAM;
        }
    }

    /* retreive free index */
    BCM_IF_ERROR_RETURN(
        bcmi_ftv2_drop_reason_group_get_free_index(unit, drop_reason_type, &idx));

    /* set hw bitmap */
    for (iter = 0; iter < bcmFlowtrackerDropReasonCount; iter++) {
        if (BCM_FLOWTRACKER_DROP_REASON_GET(drop_reasons, iter)) {
            drop_hw_bit = BCMI_FT_DROP_REASON_SW_TO_HW(unit, iter);
            if (drop_hw_bit == -1) {
                goto cleanup;
            }
            SHR_BITSET(hw_bit_map, drop_hw_bit);
        }
    }

    /* write into hardware */
    if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU) {
        soc_mem_field_set(unit, BSC_DT_DROP_MASK_1m, (uint32 *)&ing_mmu_entry,
                                              DROP_SELECT_BITMAPf, hw_bit_map);
        rv = soc_mem_write(unit, BSC_DT_DROP_MASK_1m, MEM_BLOCK_ALL,
                                                idx, &ing_mmu_entry);
    } else if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR){
        soc_mem_field_set(unit, BSC_DT_DROP_MASK_2m, (uint32 *)&egr_entry,
                                          DROP_SELECT_BITMAPf, hw_bit_map);
        rv = soc_mem_write(unit, BSC_DT_DROP_MASK_2m, MEM_BLOCK_ALL,
                                                    idx, &egr_entry);
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_ING_MMU) {
        soc_mem_field_set(unit, BSC_DT_AGG_DROP_MASK_1m, (uint32 *)&ing_mmu_entry,
                                              DROP_SELECT_BITMAPf, hw_bit_map);
        rv = soc_mem_write(unit, BSC_DT_AGG_DROP_MASK_1m, MEM_BLOCK_ALL,
                                                idx, &ing_mmu_entry);
    } else {
        soc_mem_field_set(unit, BSC_DT_AGG_DROP_MASK_2m, (uint32 *)&egr_entry,
                                          DROP_SELECT_BITMAPf, hw_bit_map);
        rv = soc_mem_write(unit, BSC_DT_AGG_DROP_MASK_2m, MEM_BLOCK_ALL,
                                                    idx, &egr_entry);
#endif
    }

    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* return unique drop reason group id to user */
    *id = BCMI_FT_DROP_REASON_GROUP_ID_SET(idx, drop_reason_type);

    return BCM_E_NONE;

cleanup:
    if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU) {
        SHR_BITCLR(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), idx);
    } else if (drop_reason_type == BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR){
        SHR_BITCLR(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), idx);
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (drop_reason_type ==
                    BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_ING_MMU) {
        SHR_BITCLR(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), idx);
    } else {
        SHR_BITCLR(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), idx);
#endif
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_delete
 * Purpose:
 *   Delete drop reason group id
 * Parameters:
 *   unit   - (IN) BCM device id
 *   id     - (IN) Drop reason group id
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_drop_reason_group_delete(int unit,
                                   bcm_flowtracker_drop_reason_group_t id)
{
    int idx = -1;
    bsc_dt_drop_mask_2_entry_t egr_entry;
    bsc_dt_drop_mask_1_entry_t ing_mmu_entry;

    /* initialize variables */
    sal_memset(&ing_mmu_entry, 0, sizeof(ing_mmu_entry));
    sal_memset(&egr_entry, 0, sizeof(egr_entry));

    /* validate drop reason group id */
    BCM_IF_ERROR_RETURN
        (bcmi_ftv2_drop_reason_group_id_is_invalid(unit, id));

    /* retreive index from group id */
    idx = BCMI_FT_DROP_REASON_GROUP_INDEX_GET(id);

    /* clear entry */
    if (BCMI_FT_DROP_REASON_GROUP_ID_IS_ING_MMU(id)) {

        if (BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT(unit, idx)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
             "drop reason group is attached to a flowtracker check.\n")));
            return BCM_E_BUSY;
        }

        SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, BSC_DT_DROP_MASK_1m, MEM_BLOCK_ALL,
                                                    idx, &ing_mmu_entry));

        SHR_BITCLR(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), idx);

    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_EGR(id)) {

        if (BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT(unit, idx)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
             "drop reason group is attached to a flowtracker check.\n")));
            return BCM_E_BUSY;
        }

        SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, BSC_DT_DROP_MASK_2m, MEM_BLOCK_ALL,
                                                        idx, &egr_entry));

        SHR_BITCLR(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), idx);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_ING_MMU(id)) {

        if (BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT(unit, idx)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
             "drop reason group is attached to a flowtracker check.\n")));
            return BCM_E_BUSY;
        }

        SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, BSC_DT_AGG_DROP_MASK_1m, MEM_BLOCK_ALL,
                                                    idx, &ing_mmu_entry));

        SHR_BITCLR(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), idx);

    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_EGR(id)) {

        if (BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT(unit, idx)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
             "drop reason group is attached to a flowtracker check.\n")));
            return BCM_E_BUSY;
        }

        SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, BSC_DT_AGG_DROP_MASK_2m, MEM_BLOCK_ALL,
                                                        idx, &egr_entry));

        SHR_BITCLR(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), idx);

#endif
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "invalid drop reason group id 0x%08x\n"),id));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv2_drop_reason_group_get
 * Purpose:
 *   Get flow tracker drop reasons from group id
 * Parameters:
 *   unit         - (IN) BCM device id
 *   id           - (IN) Drop Reason Group Id
 *   drop_reasons - (OUT) Drop Reasons
 * Returns:
 *   BCM_E_XXX.
 */
int
bcmi_ftv2_drop_reason_group_get(int unit,
                                bcm_flowtracker_drop_reason_group_t id,
                                bcm_flowtracker_drop_reasons_t *drop_reasons)
{
    int idx = -1, iter = 0;
    int16 drop_reason_tmp = 0;
    uint32 hw_bit_map[6] = {0};
    bsc_dt_drop_mask_2_entry_t egr_entry;
    bsc_dt_drop_mask_1_entry_t ing_mmu_entry;

    /* initialize variables */
    sal_memset(&ing_mmu_entry, 0, sizeof(ing_mmu_entry));
    sal_memset(&egr_entry, 0, sizeof(egr_entry));

    /* validate drop reason group id */
    BCM_IF_ERROR_RETURN
        (bcmi_ftv2_drop_reason_group_id_is_invalid(unit, id));

    /* retreived index from group id */
    idx = BCMI_FT_DROP_REASON_GROUP_INDEX_GET(id);

    /* populate drop reasons */
    if (BCMI_FT_DROP_REASON_GROUP_ID_IS_ING_MMU(id)) {

        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, BSC_DT_DROP_MASK_1m, MEM_BLOCK_ALL,
                                             idx, &ing_mmu_entry));

        soc_mem_field_get(unit, BSC_DT_DROP_MASK_1m, (uint32 *)&ing_mmu_entry,
                                              DROP_SELECT_BITMAPf, hw_bit_map);

        for (iter = 0; iter < BCMI_FT_DROP_BIT_ING_MMU_MAX; iter++) {
            if (SHR_BITGET(hw_bit_map, iter)) {
                drop_reason_tmp =
                      BCMI_FT_DROP_REASON_ING_MMU_HW_TO_SW(unit, iter);
                if (drop_reason_tmp == -1) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "could not map ingress/mmu drop hw bit %d\n"),iter));
                    return BCM_E_INTERNAL;
                }
                SHR_BITSET(drop_reasons->rbits, drop_reason_tmp);
            }
        }

    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_EGR(id)) {

        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, BSC_DT_DROP_MASK_2m, MEM_BLOCK_ALL,
                                                 idx, &egr_entry));

        soc_mem_field_get(unit, BSC_DT_DROP_MASK_2m, (uint32 *)&egr_entry,
                                          DROP_SELECT_BITMAPf, hw_bit_map);

        for (iter = 0; iter < BCMI_FT_DROP_BIT_EGR_MAX; iter++) {
            if (SHR_BITGET(hw_bit_map, iter)) {
                drop_reason_tmp = BCMI_FT_DROP_REASON_EGR_HW_TO_SW(unit, iter);
                if (drop_reason_tmp == -1) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "could not map egr drop hw bit %d\n"),iter));
                    return BCM_E_INTERNAL;
                }
                SHR_BITSET(drop_reasons->rbits, drop_reason_tmp);
            }
        }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_ING_MMU(id)) {

        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, BSC_DT_AGG_DROP_MASK_1m, MEM_BLOCK_ALL,
                                             idx, &ing_mmu_entry));

        soc_mem_field_get(unit, BSC_DT_AGG_DROP_MASK_1m, (uint32 *)&ing_mmu_entry,
                                              DROP_SELECT_BITMAPf, hw_bit_map);

        for (iter = 0; iter < BCMI_FT_DROP_BIT_ING_MMU_MAX; iter++) {
            if (SHR_BITGET(hw_bit_map, iter)) {
                drop_reason_tmp =
                      BCMI_FT_DROP_REASON_ING_MMU_HW_TO_SW(unit, iter);
                if (drop_reason_tmp == -1) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "could not map ingress/mmu drop hw bit %d\n"),iter));
                    return BCM_E_INTERNAL;
                }
                SHR_BITSET(drop_reasons->rbits, drop_reason_tmp);
            }
        }

    } else if (BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_EGR(id)) {

        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, BSC_DT_AGG_DROP_MASK_2m, MEM_BLOCK_ALL,
                                                 idx, &egr_entry));

        soc_mem_field_get(unit, BSC_DT_AGG_DROP_MASK_2m, (uint32 *)&egr_entry,
                                          DROP_SELECT_BITMAPf, hw_bit_map);

        for (iter = 0; iter < BCMI_FT_DROP_BIT_EGR_MAX; iter++) {
            if (SHR_BITGET(hw_bit_map, iter)) {
                drop_reason_tmp = BCMI_FT_DROP_REASON_EGR_HW_TO_SW(unit, iter);
                if (drop_reason_tmp == -1) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "could not map egr drop hw bit %d\n"),iter));
                    return BCM_E_INTERNAL;
                }
                SHR_BITSET(drop_reasons->rbits, drop_reason_tmp);
            }
        }
#endif
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
         "invalid drop reason group id 0x%08x\n"),id));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
/*
 * Function:
 *   bcmi_ftv3_drop_reason_agg_ing_mmu_idx_is_invalid
 * Purpose:
 *   Check if the flowtracker aggregate ing_mmu drop reason
 *   group index is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   idx  - (IN) Aggregate Ingress/Mmu Drop Reason Group Index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ftv3_drop_reason_agg_ing_mmu_idx_is_invalid(int unit, int idx)
{
    if (idx >= BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Aggregate"
            " Ingress/Mmu drop reason id exceeds maximum allowed.\n")));
        return BCM_E_PARAM;
    }

    if (BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit) == NULL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Internal error.\n")));
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), idx))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Aggregate Ingress/Mmu drop reason id not found.\n")));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ftv3_drop_reason_agg_egr_idx_is_invalid
 * Purpose:
 *   Check if the flowtracker aggregate egr drop reason
 *   group index is valid
 * Parameters:
 *   unit - (IN) BCM device id
 *   idx -  (IN) Aggregagte Egress Drop Reason Group Index
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ftv3_drop_reason_agg_egr_idx_is_invalid(int unit, int idx)
{
    if (idx >= BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Aggregate Egress drop reason id exceeds maximum allowed.\n")));
        return BCM_E_PARAM;
    }

    if (BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit) == NULL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Internal error.\n")));
        return BCM_E_INIT;
    }

    if (!(SHR_BITGET(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), idx))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "Aggregate Egress drop reason id not found.\n")));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#else /* BCM_FLOWTRACKER_SUPPORT & BCM_FLOWTRACKER_V2_SUPPORT */
typedef int bcmi_ftv2_drop_reason_group_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT & BCM_FLOWTRACKER_V2_SUPPORT */
