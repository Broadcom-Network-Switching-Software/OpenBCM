/*! \file bcm56780_a0_ina_2_4_13_pktio.c
 *
 * BCM56780_A0 INA_2_4_13 PKTIO Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/pktio.h>
#include <bcm/pktio_defs.h>

#include <bcm_int/ltsw/mbcm/pktio.h>

#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex_defs.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t_defs.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t_defs.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_extension_0_t_defs.h>
#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PKTIO

static
bcmint_pktio_fid_info_t bcm56780_a0_ina_2_4_13_rxpmd_flex_db[] = {
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_EVENT_TRACE_VECTOR_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_31_16,
        .lt_fid = BCMPKT_RXPMD_FLEX_EVENT_TRACE_VECTOR_31_16,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_47_32,
        .lt_fid = BCMPKT_RXPMD_FLEX_EVENT_TRACE_VECTOR_47_32,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_DROP_CODE_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_DROP_CODE_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_PARSER_VHLEN_0_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_PARSER_VHLEN_0_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_SYSTEM_SOURCE_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_SYSTEM_SOURCE_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_SYSTEM_DESTINATION_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_SYSTEM_DESTINATION_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_IFP_TO_EP_MACSEC_INFO_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_IFP_TO_EP_MACSEC_INFO_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EFFECTIVE_TTL_7_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_EFFECTIVE_TTL_7_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_L3_IIF_13_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_L3_IIF_13_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_NHOP_2_OR_ECMP_GROUP_INDEX_1_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_NHOP_2_OR_ECMP_GROUP_INDEX_1_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_L2_IIF_10_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_L2_IIF_10_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_I2E_CLASS_ID_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_I2E_CLASS_ID_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_ENTROPY_LABEL_LOW_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_ENTROPY_LABEL_LOW_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_SVP_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_SVP_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_NHOP_INDEX_1_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_NHOP_INDEX_1_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_L3_OIF_1_13_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_L3_OIF_1_13_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_INGRESS_PP_PORT_7_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_INGRESS_PP_PORT_7_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EP_NIH_HDR_TIMESTAMP_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_EP_NIH_HDR_TIMESTAMP_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EP_NIH_HDR_TIMESTAMP_31_16,
        .lt_fid = BCMPKT_RXPMD_FLEX_EP_NIH_HDR_TIMESTAMP_31_16,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_VFI_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_VFI_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_DVP_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_DVP_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_INT_PRI_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_INT_PRI_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_IFP_TS_CONTROL_ACTION_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_IFP_TS_CONTROL_ACTION_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_IFP_IOAM_GBP_ACTION_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_IFP_IOAM_GBP_ACTION_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_TUNNEL_PROCESSING_RESULTS_1_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_TUNNEL_PROCESSING_RESULTS_1_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_TIMESTAMP_CTRL_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_TIMESTAMP_CTRL_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_SVP_NETWORK_GROUP_BITMAP_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_SVP_NETWORK_GROUP_BITMAP_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EGR_MTOP_INDEX_HI_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_EGR_MTOP_INDEX_HI_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_INGRESS_QOS_REMARK_CTRL_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_INGRESS_QOS_REMARK_CTRL_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_SYSTEM_OPCODE_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_SYSTEM_OPCODE_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_PKT_MISC_CTRL_0_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_PKT_MISC_CTRL_0_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_L2_OIF_10_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_L2_OIF_10_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_EGR_MTOP_INDEX_LO_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_EGR_MTOP_INDEX_LO_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_ING_TIMESTAMP_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_ING_TIMESTAMP_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_ENTROPY_LABEL_HIGH_3_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_ENTROPY_LABEL_HIGH_3_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_DLB_ECMP_DESTINATION_15_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_DLB_ECMP_DESTINATION_15_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_ING_TIMESTAMP_31_16,
        .lt_fid = BCMPKT_RXPMD_FLEX_ING_TIMESTAMP_31_16,
    },
    {
        /* Always last. */
        .fid = BCM_PKTIO_FID_INVALID,
    },
};

static
bcmint_pktio_fid_info_t bcm56780_a0_ina_2_4_13_generic_loopback_db[] = {
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_INPUT_PRIORITY,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_INPUT_PRIORITY,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_RESERVED_1,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_RESERVED_1,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_INTERFACE_OBJ,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_INTERFACE_OBJ,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_PROCESSING_CTRL_1,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_PROCESSING_CTRL_1,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_PROCESSING_CTRL_0,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_PROCESSING_CTRL_0,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_DESTINATION_OBJ,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_DESTINATION_OBJ,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_SOURCE_SYSTEM_PORT,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_SOURCE_SYSTEM_PORT,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_INTERFACE_CTRL,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_INTERFACE_CTRL,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_HEADER_TYPE,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_HEADER_TYPE,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_ENTROPY_OBJ,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_ENTROPY_OBJ,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_DESTINATION_TYPE,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_DESTINATION_TYPE,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_RESERVED_2,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_RESERVED_2,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_FLAGS,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_FLAGS,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_START_BYTE,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_START_BYTE,
    },
    {
        .fid = BCM_PKTIO_GENERIC_LOOPBACK_T_QOS_OBJ,
        .lt_fid = BCMPKT_GENERIC_LOOPBACK_T_QOS_OBJ,
    },
    {
        /* Always last. */
        .fid = BCM_PKTIO_FID_INVALID,
    },
};

static
bcmint_pktio_fid_info_t bcm56780_a0_ina_2_4_13_hg3_db[] = {
    {
        .fid = BCM_PKTIO_HG3_BASE_T_SYSTEM_SOURCE,
        .lt_fid = BCMPKT_HG3_BASE_T_SYSTEM_SOURCE,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_EXT_HDR_PRESENT,
        .lt_fid = BCMPKT_HG3_BASE_T_EXT_HDR_PRESENT,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_CN,
        .lt_fid = BCMPKT_HG3_BASE_T_CN,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_HG3_RESERVED,
        .lt_fid = BCMPKT_HG3_BASE_T_HG3_RESERVED,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_MIRROR_COPY,
        .lt_fid = BCMPKT_HG3_BASE_T_MIRROR_COPY,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_L3_ROUTED,
        .lt_fid = BCMPKT_HG3_BASE_T_L3_ROUTED,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_CNG,
        .lt_fid = BCMPKT_HG3_BASE_T_CNG,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_RESERVED_ETYPE,
        .lt_fid = BCMPKT_HG3_BASE_T_RESERVED_ETYPE,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_VERSION,
        .lt_fid = BCMPKT_HG3_BASE_T_VERSION,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_SYSTEM_DESTINATION,
        .lt_fid = BCMPKT_HG3_BASE_T_SYSTEM_DESTINATION,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_ENTROPY,
        .lt_fid = BCMPKT_HG3_BASE_T_ENTROPY,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_TC,
        .lt_fid = BCMPKT_HG3_BASE_T_TC,
    },
    {
        .fid = BCM_PKTIO_HG3_BASE_T_SYSTEM_DESTINATION_TYPE,
        .lt_fid = BCMPKT_HG3_BASE_T_SYSTEM_DESTINATION_TYPE,
    },
    {
        .fid = BCM_PKTIO_HG3_EXTENSION_0_T_FORWARDING_DOMAIN,
        .lt_fid = BCMPKT_HG3_EXTENSION_0_T_FORWARDING_DOMAIN,
    },
    {
        .fid = BCM_PKTIO_HG3_EXTENSION_0_T_DVP_OR_L3_IIF,
        .lt_fid = BCMPKT_HG3_EXTENSION_0_T_DVP_OR_L3_IIF,
    },
    {
        .fid = BCM_PKTIO_HG3_EXTENSION_0_T_CLASS_ID,
        .lt_fid = BCMPKT_HG3_EXTENSION_0_T_CLASS_ID,
    },
    {
        .fid = BCM_PKTIO_HG3_EXTENSION_0_T_FLAGS,
        .lt_fid = BCMPKT_HG3_EXTENSION_0_T_FLAGS,
    },
    {
        .fid = BCM_PKTIO_HG3_EXTENSION_0_T_SVP,
        .lt_fid = BCMPKT_HG3_EXTENSION_0_T_SVP,
    },
    {
        /* Always last. */
        .fid = BCM_PKTIO_FID_INVALID,
    },
};

static
bcmint_pktio_fid_info_t bcm56780_a0_ina_2_4_13_rxpmd_flex_reason_db[] = {
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_CML_FLAGS,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_CML_FLAGS,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_STATIC_MOVE,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L2_SRC_STATIC_MOVE,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_PKT_INTEGRITY_CHECK_FAILED,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_PKT_INTEGRITY_CHECK_FAILED,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_PROTOCOL_PKT,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_PROTOCOL_PKT,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L2_DST_LOOKUP_MISS,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L2_DST_LOOKUP_MISS,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L2_DST_LOOKUP,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L2_DST_LOOKUP,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L3_DST_LOOKUP_MISS,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L3_DST_LOOKUP_MISS,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L3_DST_LOOKUP,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L3_DST_LOOKUP,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L3_HDR_ERROR,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L3_HDR_ERROR,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_L3_TTL_ERROR,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_L3_TTL_ERROR,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILED,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILED,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_LEARN_CACHE_FULL,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_LEARN_CACHE_FULL,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_VFP,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_VFP,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_IFP,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_IFP,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_IFP_METER,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_IFP_METER,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_DST_FP,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_DST_FP,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_SVP,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_SVP,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_EM_FT,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_EM_FT,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_IVXLT,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_IVXLT,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_0,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_0,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_1,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_1,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_2,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_2,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_3,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_3,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_4,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_4,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_5,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_5,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_6,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_6,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_7,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_7,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_MTOP_IPV4_GATEWAY,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_MTOP_IPV4_GATEWAY,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_TRACE_DO_NOT_MIRROR,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_TRACE_DO_NOT_MIRROR,
    },
    {
        .fid = BCM_PKTIO_RXPMD_FLEX_REASON_TRACE_DO_NOT_COPY_TO_CPU,
        .lt_fid = BCMPKT_RXPMD_FLEX_REASON_TRACE_DO_NOT_COPY_TO_CPU,
    },
    {
        /* Always last. */
        .fid = BCM_PKTIO_FID_INVALID,
    },
};

/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_ina_2_4_13_ltsw_pktio_fid_support_get(
    int unit,
    bcmint_pktio_hdr_info_t *hdr_info,
    bcm_pktio_bitmap_t *support)
{
    bcmint_pktio_fid_info_t *fid_db, *fid_p;
    int i = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdr_info, SHR_E_PARAM);
    SHR_NULL_CHECK(support, SHR_E_PARAM);

    switch (hdr_info->pmd_type) {
        case bcmPktioPmdTypeRxFlex:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_rxpmd_flex_db;
            break;

        case bcmPktioPmdTypeGih:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_generic_loopback_db;
            break;

        case bcmPktioPmdTypeHigig3:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_hg3_db;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(fid_p, SHR_E_INTERNAL);

    while (fid_p->fid != BCM_PKTIO_FID_INVALID) {
        SHR_BITSET(support->pbits, fid_p->fid);
        fid_p = &(fid_db[i++]);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_pktio_fid_info_get(
    int unit,
    bcmint_pktio_hdr_info_t *hdr_info,
    bcmint_pktio_fid_info_t **fid_info)
{
    bcmint_pktio_fid_info_t *fid_db, *fid_p;
    int i = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdr_info, SHR_E_PARAM);
    SHR_NULL_CHECK(fid_info, SHR_E_PARAM);

    switch (hdr_info->pmd_type) {
        case bcmPktioPmdTypeRxFlex:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_rxpmd_flex_db;
            break;

        case bcmPktioPmdTypeGih:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_generic_loopback_db;
            break;

        case bcmPktioPmdTypeHigig3:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_hg3_db;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(fid_p, SHR_E_INTERNAL);

    while (fid_p->fid != BCM_PKTIO_FID_INVALID) {
        if (fid_p->fid == hdr_info->fid) {
            found = TRUE;
            break;
        }

        fid_p = &(fid_db[i++]);
    }

    if (found) {
        *fid_info = fid_p;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ina_2_4_13_ltsw_pktio_fid_info_lookup(
    int unit,
    int opcode,
    bcmint_pktio_hdr_info_t *hdr_info,
    bcmint_pktio_fid_info_t **fid_info)
{
    bcmint_pktio_fid_info_t *fid_db, *fid_p;
    int i = 0, chk_fid;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdr_info, SHR_E_PARAM);
    SHR_NULL_CHECK(fid_info, SHR_E_PARAM);

    switch (hdr_info->pmd_type) {
        case bcmPktioPmdTypeRxFlex:
            fid_p = fid_db = bcm56780_a0_ina_2_4_13_rxpmd_flex_reason_db;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(fid_p, SHR_E_INTERNAL);

    while (fid_p->fid != BCM_PKTIO_FID_INVALID) {
        switch (opcode) {
            case BCMINT_PKTIO_OP_API2LT:
                chk_fid = fid_p->fid;
                break;

            case BCMINT_PKTIO_OP_LT2API:
                chk_fid = fid_p->lt_fid;
                break;

            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (chk_fid == hdr_info->fid) {
            found = TRUE;
            break;
        }

        fid_p = &(fid_db[i++]);
    }

    if (found) {
        *fid_info = fid_p;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pktio driver function variable for bcm56780_a0_ina_2_4_13 device.
 */
static mbcm_ltsw_pktio_drv_t bcm56780_a0_ina_2_4_13_ltsw_pktio_drv = {
    .pktio_fid_support_get = bcm56780_a0_ina_2_4_13_ltsw_pktio_fid_support_get,
    .pktio_fid_info_get = bcm56780_a0_ina_2_4_13_ltsw_pktio_fid_info_get,
    .pktio_fid_info_lookup = bcm56780_a0_ina_2_4_13_ltsw_pktio_fid_info_lookup
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_pktio_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_pktio_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_pktio_drv));

exit:
    SHR_FUNC_EXIT();
}

