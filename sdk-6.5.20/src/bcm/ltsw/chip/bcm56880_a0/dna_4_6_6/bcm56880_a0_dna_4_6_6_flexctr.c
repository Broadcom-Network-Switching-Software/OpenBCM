/*! \file bcm56880_a0_dna_4_6_6_flexctr.c
 *
 * BCM56880_A0 DNA_4_6_6 FLEXCTR Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/flexctr.h>
#include <bcm/switch.h>

#include <bcm_int/ltsw/flexctr_int.h>
#include <bcm_int/ltsw/mbcm/flexctr.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLEXCTR

/* Flexctr IFP opaque object1 bit width. */
#define FLEXCTR_IFP_OPAQUE_OBJ1_BIT_WIDTH 4

/* Flexctr IFP opaque object1 mask. */
#define FLEXCTR_IFP_OPAQUE_OBJ1_MASK \
    ((1 << FLEXCTR_IFP_OPAQUE_OBJ1_BIT_WIDTH) - 1)

/* Flexctr BITP profile bus object offset for mirror packet control. */
#define FLEXCTR_BITP_PROF_BUS_OBJ_MIRROR_PKT_CTRL 57

/* Object mirror_pkt_ctrl bit definition for mirror_copy. */
#define FLEXCTR_BITP_MIRROR_PKT_CTRL_MIRROR_COPY 1

/* Object mirror_pkt_ctrl bit definition for flex_ctr_presel. */
#define FLEXCTR_BITP_MIRROR_PKT_CTRL_FLEXCTR_PRESEL 3

/* Flexctr control for copy_to_cpy only. */
#define FLEXCTR_CTRL_COPY_TO_CPU 0x0

/* Flexctr control for copy_to_cpu ORd with mirror_copy on presel. */
#define FLEXCTR_CTRL_MIRROR_AND_COPY_TO_CPU 0x1

/*!
 * \brief Flex counter source database.
 */
static bcmint_flexctr_source_info_t bcm56880_a0_dna_4_6_6_ltsw_flexctr_source_db[] = {
    {
        bcmFlexctrSourceIngPort,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_SYSTEM_PORT_TABLEs,
        BCMINT_FLEXCTR_ING_SYSTEM_PORT_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngVlan,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_VFI_TABLEs,
        BCMINT_FLEXCTR_ING_VFI_TBL_BIT,
    },
    {
        bcmFlexctrSourceEgrVlan,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_VFIs,
        BCMINT_FLEXCTR_EGR_VFI_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngVpn,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_VFI_TABLEs,
        BCMINT_FLEXCTR_ING_VFI_TBL_BIT,
    },
    {
        bcmFlexctrSourceEgrVpn,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_VFIs,
        BCMINT_FLEXCTR_EGR_VFI_TBL_BIT,
    },
    {
        bcmFlexctrSourceSvp,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_SVP_TABLEs,
        BCMINT_FLEXCTR_ING_SVP_TBL_BIT,
    },
    {
        bcmFlexctrSourceDvp,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_DVPs,
        BCMINT_FLEXCTR_EGR_DVP_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngL3Intf,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_L3_IIF_TABLEs,
        BCMINT_FLEXCTR_ING_L3_IIF_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngVrf,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_VRF_TABLEs,
        BCMINT_FLEXCTR_ING_VRF_TBL_BIT,
    },
    {
        bcmFlexctrSourceIfp,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        DT_IFP_ENTRYs,
        BCMINT_FLEXCTR_ING_IFP_TBL_BIT,
    },
    {
        bcmFlexctrSourceVfp,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        DT_VFP_ENTRYs,
        BCMINT_FLEXCTR_ING_VFP_TBL_BIT,
    },
    {
        bcmFlexctrSourceEfp,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        DT_EFP_ENTRYs,
        BCMINT_FLEXCTR_EGR_EFP_TBL_BIT,
    },
    {
        bcmFlexctrSourceL3EgressOverlay,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_L3_NEXT_HOP_1s,
        BCMINT_FLEXCTR_EGR_L3_NEXT_HOP_1_TBL_BIT,
    },
    {
        bcmFlexctrSourceL3EgressUnderlay,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_L3_NEXT_HOP_2s,
        BCMINT_FLEXCTR_EGR_L3_NEXT_HOP_2_TBL_BIT,
    },
    {
        bcmFlexctrSourceExactMatch,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        DT_EM_FP_ENTRYs,
        BCMINT_FLEXCTR_ING_EXACTMATCH_TBL_BIT,
    },
    {
        bcmFlexctrSourceIpmc,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        L3_IPV4_MULTICAST_1_TABLEs,
        BCMINT_FLEXCTR_IPMC_TBL_BIT,
    },
    {
        bcmFlexctrSourceFieldDestination,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        DESTINATION_FP_TABLEs,
        BCMINT_FLEXCTR_DESTINATION_FP_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngSamplePool,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        MIRROR_SFLOW_CONTROLs,
        BCMINT_FLEXCTR_ING_SAMPLE_POOL_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngSampleCount,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        MIRROR_SFLOW_CONTROLs,
        BCMINT_FLEXCTR_ING_SAMPLE_COUNT_TBL_BIT,
    },
    {
        bcmFlexctrSourceFlexSamplePool,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        MIRROR_SFLOW_CONTROLs,
        BCMINT_FLEXCTR_FLEX_SAMPLE_POOL_TBL_BIT,
    },
    {
        bcmFlexctrSourceFlexSampleCount,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        MIRROR_SFLOW_CONTROLs,
        BCMINT_FLEXCTR_FLEX_SAMPLE_COUNT_TBL_BIT,
    },
    {
        bcmiFlexctrSourceL2Dst,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        L2_HOST_TABLEs,
        BCMINT_FLEXCTR_L2DST_TBL_BIT,
    },
    {
        bcmiFlexctrSourceL2Src,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        L2_HOST_TABLEs,
        BCMINT_FLEXCTR_L2SRC_TBL_BIT,
    },
    {
        bcmiFlexctrSourceEM_FT,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        DT_EM_FT_ENTRYs,
        BCMINT_FLEXCTR_EM_FT_TBL_BIT,
    },
    {
        bcmFlexctrSourceEgrPort,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_L2_OIFs,
        BCMINT_FLEXCTR_EGR_L2_OIF_TBL_BIT,
    },
    {
        bcmFlexctrSourceIngVlanTranslate,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        ING_VLAN_XLATE_1_TABLEs,
        BCMINT_FLEXCTR_ING_VLAN_XLATE_TBL_BIT,
    },
    {
        bcmFlexctrSourceEgrVlanTranslate,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        EGR_VLAN_TRANSLATION_DWs,
        BCMINT_FLEXCTR_EGR_VLAN_XLATE_TBL_BIT,
    },
    /* Always last one. */
    {
        bcmFlexctrSourceCount,
    },
};

/*!
 * \brief Flex counter object database.
 */
static bcmint_flexctr_object_info_t bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_db[] = {
    /* CTR_EFLEX_OBJ_SRC_T */
    {
        bcmFlexctrObjectConstZero,
        BCMI_LTSW_FLEXCTR_STAGE_COUNT,
        FlexctrObjectTypeConst,
        USE_0s,
    },
    {
        bcmFlexctrObjectConstOne,
        BCMI_LTSW_FLEXCTR_STAGE_COUNT,
        FlexctrObjectTypeConst,
        USE_1s,
    },
    {
        bcmFlexctrObjectTriggerInterval,
        BCMI_LTSW_FLEXCTR_STAGE_COUNT,
        FlexctrObjectTypeTrigger,
        USE_TRIG_INTERVALs,
    },
    {
        bcmFlexctrObjectQuant,
        BCMI_LTSW_FLEXCTR_STAGE_COUNT,
        FlexctrObjectTypeQuant,
        USE_RANGE_CHK_1s,
    },
    /* bcm56880_a0_dna_4_6_6_CTR_ING_EFLEX_OBJ_BUS_T */
    {
        bcmFlexctrObjectIngSystemSource,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        SYSTEM_SOURCEs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        EM_FT_OPAQUE_OBJ_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngSvp,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        SVPs,
    },
    {
        bcmFlexctrObjectIngIngressPpPort,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        INGRESS_PP_PORTs,
    },
    {
        bcmFlexctrObjectIngL2Iif,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L2_IIFs,
    },
    {
        bcmFlexctrObjectIngL3Iif,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L3_IIFs,
    },
    {
        bcmFlexctrObjectIngEmFtHitIndex0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        EM_FT_HIT_INDEX_LO_OR_IFP_OPAQUE_OBJ_0s,
    },
    {
        bcmFlexctrObjectIngEmFtHitIndex1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        EM_FT_HIT_INDEX_LO_OR_IFP_OPAQUE_OBJ_1s,
    },
    {
        bcmFlexctrObjectIngNhopIndex1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        NHOP_INDEX_1s,
    },
    {
        bcmFlexctrObjectIngL2DstLookupHitIndex0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L2_DST_LOOKUP_HIT_INDEX_0s,
    },
    {
        bcmFlexctrObjectIngL2DstLookupHitIndex1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L2_DST_LOOKUP_HIT_INDEX_1s,
    },
    {
        bcmFlexctrObjectIngL2SrcLookupHitIndex0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L2_SRC_LOOKUP_HIT_INDEX_0s,
    },
    {
        bcmFlexctrObjectIngL2SrcLookupHitIndex1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L2_SRC_LOOKUP_HIT_INDEX_1s,
    },
    {
        bcmFlexctrObjectIngNhop2OrEcmpGroupIndex1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        NHOP_INDEX_2s,
    },
    {
        bcmFlexctrObjectIngVfi,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        VFIs,
    },
    {
        bcmFlexctrObjectIngVfpOpaqueObj0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        VFP_VLAN_XLATE_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngDstFpFlexCtrIndex,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        DST_FP_FLEX_CTR_INDEXs,
    },
    {
        bcmFlexctrObjectIngL3DstClassId,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L3_DST_CLASS_ID_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIntPri,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        INT_PRIs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj2,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_OPAQUE_OBJ2s,
    },
    {
        bcmFlexctrObjectIngIngressQoSRemapValue,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        INGRESS_QOS_REMAP_VALUE_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngFlexDigestA0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        FLEX_DIGEST_A0s,
    },
    {
        bcmFlexctrObjectIngFlexDigestA1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        FLEX_DIGEST_A1s,
    },
    {
        bcmFlexctrObjectIngFlexDigestB0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        FLEX_DIGEST_B0s,
    },
    {
        bcmFlexctrObjectIngFlexDigestB1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        FLEX_DIGEST_B1s,
    },
    {
        bcmFlexctrObjectIngFlexDigestC0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        FLEX_DIGEST_C0s,
    },
    {
        bcmFlexctrObjectIngFlexDigestC1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        FLEX_DIGEST_C1s,
    },
    {
        bcmFlexctrObjectIngIpHdrTotalLength,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IP_HDR_TOTAL_LENGTHs,
    },
    {
        bcmFlexctrObjectIngI2eClassId,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        I2E_CLASS_IDs,
    },
    {
        bcmFlexctrObjectIngVlanXlate,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        VFP_VLAN_XLATE_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpLagDgmActionSet,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_LAG_DGM_ACTION_SET_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpSamplerIndex,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_SAMPLER_INDEX_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngEmFtOpaqueObj,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        EM_FT_OPAQUE_OBJ_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj1_0,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_OPAQUE_OBJ1_0s,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj1_1,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_OPAQUE_OBJ1_1s,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj1_2,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_OPAQUE_OBJ1_2s,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj1_3,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_OPAQUE_OBJ1_3s,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj3,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        INGRESS_QOS_REMAP_VALUE_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj4,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        EM_FT_HIT_INDEX_LO_OR_IFP_OPAQUE_OBJ_0s,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj5,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        EM_FT_HIT_INDEX_LO_OR_IFP_OPAQUE_OBJ_1s,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj6,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        L3_DST_CLASS_ID_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj7,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        VFP_VLAN_XLATE_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj8,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_LAG_DGM_ACTION_SET_OR_IFP_OPAQUE_OBJs,
    },
    {
        bcmFlexctrObjectIngIfpOpaqueObj9,
        BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        FlexctrObjectTypeMenu,
        IFP_SAMPLER_INDEX_OR_IFP_OPAQUE_OBJs,
    },
    /* bcm56880_a0_dna_4_6_6_CTR_EGR_EFLEX_OBJ_BUS_T */
    {
        bcmFlexctrObjectEgrL3NextHop2FlexCtrIndex,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        L3_NEXT_HOP_2_FLEX_CTR_INDEXs,
    },
    {
        bcmFlexctrObjectEgrL3NextHop1FlexCtrIndex,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        L3_NEXT_HOP_1_FLEX_CTR_INDEXs,
    },
    {
        bcmFlexctrObjectEgrVfi,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        VFIs,
    },
    {
        bcmFlexctrObjectEgrResidenceTimeShifted0,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        RESIDENCE_TIME_SHIFTED_0s,
    },
    {
        bcmFlexctrObjectEgrResidenceTimeShifted1,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        RESIDENCE_TIME_SHIFTED_1s,
    },
    {
        bcmFlexctrObjectEgrEcnMapInputVector,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        ECN_MAP_INPUT_VECTORs,
    },
    {
        bcmFlexctrObjectEgrPipePortQueueNum,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        PIPE_PORT_QUEUE_NUMs,
    },
    {
        bcmFlexctrObjectEgrEfpOpaqueObj1,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        EFP_OPAQUE_OBJ1s,
    },
    {
        bcmFlexctrObjectEgrEfpOpaqueObj2,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        EVXLT_FLEX_CTR_INDEXs,
    },
    {
        bcmFlexctrObjectEgrL3Oif1,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        L3_OIF_1s,
    },
    {
        bcmFlexctrObjectEgrL2Oif,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        L2_OIFs,
    },
    {
        bcmFlexctrObjectEgrDvp,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        DVPs,
    },
    {
        bcmFlexctrObjectEgrTxPpPort,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        TX_PP_PORTs,
    },
    {
        bcmFlexctrObjectEgrIntcnIntpriCng0,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        INTCN_INTPRI_CNG_0s,
    },
    {
        bcmFlexctrObjectEgrIntcnIntpriCng,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        INTCN_INTPRI_CNGs,
    },
    {
        bcmFlexctrObjectEgrL3Oif216VfiClassid,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        L3_OIF_2_16_VFI_CLASSIDs,
    },
    {
        bcmFlexctrObjectEgrEvxltFlexCtrIndex,
        BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        FlexctrObjectTypeMenu,
        EVXLT_FLEX_CTR_INDEXs,
    },
    /* Always last one. */
    {
        bcmFlexctrObjectCount,
    },
};

/*!
 * \brief Flex counter trigger interval database.
 */
static bcmint_flexctr_interval_info_t
bcm56880_a0_dna_4_6_6_ltsw_flexctr_interval_db[] = {
    /* Unlimited collection. */
    {bcmFlexctrTriggerIntervalUnlimited},
    /* 100 nanosecond interval. */
    {bcmFlexctrTriggerInterval100ns},
    /* 500 nanosecond interval. */
    {bcmFlexctrTriggerInterval500ns},
    /* 1 microsecond interval. */
    {bcmFlexctrTriggerInterval1us},
    /* 10 microsecond interval. */
    {bcmFlexctrTriggerInterval10us},
    /* 100 microsecond interval. */
    {bcmFlexctrTriggerInterval100us},
    /* 1 millisecond interval. */
    {bcmFlexctrTriggerInterval1ms},
    /* 10 millisecond interval. */
    {bcmFlexctrTriggerInterval10ms},
};

/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_source_info_get(
    int unit,
    int source,
    bcmint_flexctr_source_info_t **source_info)
{
    bcmint_flexctr_source_info_t *source_db, *si;
    size_t i, count;

    count = COUNTOF(bcm56880_a0_dna_4_6_6_ltsw_flexctr_source_db);
    source_db = bcm56880_a0_dna_4_6_6_ltsw_flexctr_source_db;

    if ((source < 0 || source >= bcmFlexctrSourceCount) &&
        (source < bcmiFlexctrSourceStart || source >= bcmiFlexctrSourceEnd)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid flex counter source %d\n"),
                   source));

        *source_info = NULL;
        return SHR_E_PARAM;
    }

    for (i = 0; i < count; i++) {
        si = &(source_db[i]);
        if (source == si->source) {
            *source_info = si;
            break;
        }
    }

    /* Not found. */
    if (i >= count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Flex counter source %d not found\n"),
                  source));

        *source_info = NULL;
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_info_get(
    int unit,
    int object,
    bcmint_flexctr_object_info_t **object_info)
{
    bcmint_flexctr_object_info_t *object_db, *oi;
    size_t i, count;

    count = COUNTOF(bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_db);
    object_db = bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_db;

    if ((object < 0) || (object >= bcmFlexctrObjectCount)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid flex counter object %d\n"),
                   object));

        *object_info = NULL;
        return SHR_E_PARAM;
    }

    for (i = 0; i < count; i++) {
        oi = &(object_db[i]);
        if (object == oi->object) {
            *object_info = oi;
            break;
        }
    }

    /* Not found. */
    if (i >= count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Flex counter object %d not found\n"),
                  object));

        *object_info = NULL;
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_trigger_interval_validate(
    int unit,
    int interval)
{
    bcmint_flexctr_interval_info_t *interval_db, *ii;
    size_t i, count;

    count = COUNTOF(bcm56880_a0_dna_4_6_6_ltsw_flexctr_interval_db);
    interval_db = bcm56880_a0_dna_4_6_6_ltsw_flexctr_interval_db;

    if (interval < bcmFlexctrTriggerIntervalUnlimited ||
        interval >= bcmFlexctrTriggerIntervalCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid trigger interval %d\n"),
                   interval));
        return SHR_E_PARAM;
    }

    for (i = 0; i < count; i++) {
        ii = &(interval_db[i]);
        if (interval == ii->interval) {
            break;
        }
    }

    /* Not found. */
    if (i >= count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Flex counter interval %d not found\n"),
                  interval));

        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_overlay_multi_get(int unit,
    bcm_flexctr_object_t object,
    int array_size,
    bcm_flexctr_object_t *overlay_object_array,
    int *count)
{
    bcmint_flexctr_object_info_t *object_db, *oi;
    const char *symbol = NULL;
    size_t i, size, count_tmp = 0;

    SHR_FUNC_ENTER(unit);

    if (array_size != 0) {
        SHR_NULL_CHECK(overlay_object_array, SHR_E_PARAM);
    }
    if ((object < 0) || (object >= bcmFlexctrObjectCount) ||
        (array_size < 0) || (count == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    size = COUNTOF(bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_db);
    object_db = bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_db;

    for (i = 0; i < size; i++) {
        oi = &(object_db[i]);
        if (object == oi->object) {
            symbol = oi->symbol;
            break;
        }
    }

    /* Not found. */
    if (i >= size || symbol == NULL) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Flex counter object %d not found\n"),
                  object));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = 0; i < size; i++) {
        oi = &(object_db[i]);
        if (oi->symbol && (sal_strcmp(symbol, oi->symbol) == 0)) {
            if (overlay_object_array && (count_tmp < array_size)) {
                overlay_object_array[count_tmp] = oi->object;
            }
            count_tmp++;
        }
    }

    *count = (count_tmp > 1) ? count_tmp : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_field_opaque_object_value_get(int unit,
    uint32_t stat_counter_id,
    uint32_t counter_index,
    uint32_t size,
    uint32_t *obj_val)
{
    bcmi_ltsw_flexctr_counter_info_t info;
    bcm_flexctr_object_t obj0, obj1;
    uint32_t obj1_0_val = 0, obj1_1_val = 0, obj1_2_val = 0, obj1_3_val = 0;
    uint8_t shift0 = 0, shift1 = 0;
    uint8_t mask0 = 0, mask1 = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(obj_val, SHR_E_PARAM);

    sal_memset(&info, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &info));

    obj0 = info.act_cfg.index_operation.object[0];
    obj1 = info.act_cfg.index_operation.object[1];
    shift0 = info.act_cfg.index_operation.shift[0];
    shift1 = info.act_cfg.index_operation.shift[1];
    mask0 = (1 << info.act_cfg.index_operation.mask_size[0]) - 1;
    mask1 = (1 << info.act_cfg.index_operation.mask_size[1]) - 1;

    if (((obj0 >= bcmFlexctrObjectIngIfpOpaqueObj1_0 &&
          obj0 <= bcmFlexctrObjectIngIfpOpaqueObj1_3) ||
         (obj0 == bcmFlexctrObjectConstZero)) &&
        ((obj1 >= bcmFlexctrObjectIngIfpOpaqueObj1_0 &&
          obj1 <= bcmFlexctrObjectIngIfpOpaqueObj1_3) ||
         (obj1 == bcmFlexctrObjectConstZero))) {
        switch (obj0) {
            case bcmFlexctrObjectIngIfpOpaqueObj1_0:
                obj1_0_val = ((counter_index & mask0) << shift0);
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_1:
                obj1_1_val = ((counter_index & mask0) << shift0);
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_2:
                obj1_2_val = ((counter_index & mask0) << shift0);
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_3:
                obj1_3_val = ((counter_index & mask0) << shift0);
                break;
            default:
                break;
        }

        switch (obj1) {
            case bcmFlexctrObjectIngIfpOpaqueObj1_0:
                obj1_0_val = ((counter_index >> shift1) & mask1) &
                             FLEXCTR_IFP_OPAQUE_OBJ1_MASK;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_1:
                obj1_1_val = ((counter_index >> shift1) & mask1) &
                             FLEXCTR_IFP_OPAQUE_OBJ1_MASK;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_2:
                obj1_2_val = ((counter_index >> shift1) & mask1) &
                             FLEXCTR_IFP_OPAQUE_OBJ1_MASK;
                break;
            case bcmFlexctrObjectIngIfpOpaqueObj1_3:
                obj1_3_val = ((counter_index >> shift1) & mask1) &
                             FLEXCTR_IFP_OPAQUE_OBJ1_MASK;
                break;
            default:
                break;
        }

        *obj_val = obj1_0_val | obj1_1_val | obj1_2_val | obj1_3_val;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_egr_presel_set(int unit, int val)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MUX0_SELs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {MIRROR_COPY_VALUEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {MIRROR_COPY_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t flex_field[] =
    {
        /*0*/ {PRESEL_OVERLAY_MODEs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = FLEXCTR_BITP_PROF_BUS_OBJ_MIRROR_PKT_CTRL;
    switch (val) {
        case bcmSwitchEgressFlexCounterPreselMirror:
            field[1].u.val = 1 << FLEXCTR_BITP_MIRROR_PKT_CTRL_MIRROR_COPY;
            field[2].u.val = field[1].u.val;
            break;
        case bcmSwitchEgressFlexCounterPreselCopyToCpu:
        case bcmSwitchEgressFlexCounterPreselMirrorAndCopyToCpu:
            field[1].u.val = 1 << FLEXCTR_BITP_MIRROR_PKT_CTRL_FLEXCTR_PRESEL;
            field[2].u.val = field[1].u.val;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, CTR_EGR_EFLEX_BITP_PROFILEs, &lt_entry, NULL));

    lt_entry.fields = flex_field;
    lt_entry.nfields = sizeof(flex_field)/ sizeof(flex_field[0]);
    lt_entry.attr = 0;

    switch (val) {
        case bcmSwitchEgressFlexCounterPreselMirror:
            SHR_EXIT();
        case bcmSwitchEgressFlexCounterPreselCopyToCpu:
            flex_field[0].u.val = FLEXCTR_CTRL_COPY_TO_CPU;
            break;
        case bcmSwitchEgressFlexCounterPreselMirrorAndCopyToCpu:
            flex_field[0].u.val = FLEXCTR_CTRL_MIRROR_AND_COPY_TO_CPU;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, R_FLEX_CTR_CTRLs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_egr_presel_get(int unit, int *val)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MUX0_SELs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*1*/ {MIRROR_COPY_VALUEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*2*/ {MIRROR_COPY_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_lt_field_t flex_field[] =
    {
        /*0*/ {PRESEL_OVERLAY_MODEs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    int rv;
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    rv = bcmi_lt_entry_get(unit,
                           CTR_EGR_EFLEX_BITP_PROFILEs,
                           &lt_entry,
                           NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        *val = bcmSwitchEgressFlexCounterPreselMirror;
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (field[1].u.val == (1 << FLEXCTR_BITP_MIRROR_PKT_CTRL_MIRROR_COPY)) {
        *val = bcmSwitchEgressFlexCounterPreselMirror;
        SHR_EXIT();
    }

    lt_entry.fields = flex_field;
    lt_entry.nfields = sizeof(flex_field)/ sizeof(flex_field[0]);
    lt_entry.attr = 0;
    rv = bcmi_lt_entry_get(unit,
                           R_FLEX_CTR_CTRLs,
                           &lt_entry,
                           NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        *val = bcmSwitchEgressFlexCounterPreselMirror;
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (flex_field[0].u.val == FLEXCTR_CTRL_COPY_TO_CPU) {
        *val = bcmSwitchEgressFlexCounterPreselCopyToCpu;
    } else if (flex_field[0].u.val == FLEXCTR_CTRL_MIRROR_AND_COPY_TO_CPU) {
        *val = bcmSwitchEgressFlexCounterPreselMirrorAndCopyToCpu;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flexctr driver function variable for bcm56880_a0_dna_4_6_6 device.
 */
static mbcm_ltsw_flexctr_drv_t bcm56880_a0_dna_4_6_6_ltsw_flexctr_drv = {
    .flexctr_source_info_get = bcm56880_a0_dna_4_6_6_ltsw_flexctr_source_info_get,
    .flexctr_object_info_get = bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_info_get,
    .flexctr_trigger_interval_validate = bcm56880_a0_dna_4_6_6_ltsw_flexctr_trigger_interval_validate,
    .flexctr_object_overlay_multi_get = bcm56880_a0_dna_4_6_6_ltsw_flexctr_object_overlay_multi_get,
    .flexctr_field_opaque_object_value_get = bcm56880_a0_dna_4_6_6_ltsw_flexctr_field_opaque_object_value_get,
    .flexctr_egr_presel_set = bcm56880_a0_dna_4_6_6_ltsw_flexctr_egr_presel_set,
    .flexctr_egr_presel_get = bcm56880_a0_dna_4_6_6_ltsw_flexctr_egr_presel_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_6_6_ltsw_flexctr_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_drv_set(unit, &bcm56880_a0_dna_4_6_6_ltsw_flexctr_drv));

exit:
    SHR_FUNC_EXIT();
}

