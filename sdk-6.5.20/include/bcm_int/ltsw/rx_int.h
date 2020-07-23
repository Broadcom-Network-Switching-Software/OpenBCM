/* \file rx_int.h
 *
 * ltsw RX module internal header file.
 * Include structure definitions and function declarations used by RX module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMI_LTSW_RX_INT_H
#define BCMI_LTSW_RX_INT_H

#include <bcm/types.h>
#include <sal/sal_types.h>

/*
 * \brief TM_COS_Q_CPU_MAP_INFO_ID_T type enum.
 */
typedef enum bcmint_tm_cos_q_cpu_map_info_id_type_s {
    /* Enumeration for TM_BASE_VIEW. */
    BCMINT_TM_BASE_VIEW = 0,

    /* The last one, not valid. */
    BCMINT_TM_COS_Q_CPU_MAP_INFO_ID_TYPE_COUNT = 1

} bcmint_tm_cos_q_cpu_map_info_id_type_t;

/*
 * \brief RX string map structure.
 */
typedef struct bcmint_rx_str_map_s {

    /*! Index of the mapped object. */
    int idx;

    /*! Map string. */
    const char * str;

    /*! key0. */
    const char * key0;

    /*! key1. */
    const char * ifp_opaque_ctrl_id;

    /*! key2. */
    const char * int_pri;

} bcmint_rx_str_map_t;

/*
 * \brief rx string map handle.
 */
/* TM_COS_Q_CPU_MAP_INFO_ID type handle. */
#define BCMINT_TM_COS_Q_CPU_MAP_INFO_ID 0

/*
 * \brief RX string map database structure.
 */
typedef struct bcmint_rx_str_map_db_s {

    /* rx string map handle. */
    int hdl;

    /* Map info. */
    const bcmint_rx_str_map_t *info;

    /* Num of elements in map info. */
    int num;

} bcmint_rx_str_map_db_t;

typedef enum bcmint_rx_pkt_high_drop_code_e {
    rxPktDropCodeNoDrop = 0,
    rxPktDropCodeCmlFlagsDrop = 1,
    rxPktDropCodeL2SrcStaticMove = 2,
    rxPktDropCodeL2SrcDiscard = 3,
    rxPktDropCodeMacsaMulticast = 4,
    rxPktDropCodeOuterTpidCheckFailed = 5,
    rxPktDropCodeIncomingPvlanCheckFailed = 6,
    rxPktDropCodePktIntegrityCheckFailed = 7,
    rxPktDropCodeProtocolPktDrop = 8,
    rxPktDropCodeMembershipCheckFailed = 9,
    rxPktDropCodeSpanningTreeCheckFailed = 10,
    rxPktDropCodeL2DstLookupMiss = 11,
    rxPktDropCodeL2DstDiscard = 12,
    rxPktDropCodeL3DstLookupMiss = 13,
    rxPktDropCodeL3DstDiscard = 14,
    rxPktDropCodeL3HdrError = 15,
    rxPktDropCodeL3TtlError = 16,
    rxPktDropCodeIpmcL3IifOrRpaIdCheckFailed = 17,
    rxPktDropCodeTunnelTtlCheckFailed = 18,
    rxPktDropCodeTunnelShimHdrError = 19,
    rxPktDropCodeTunnelObjectValidationFailed = 20,
    rxPktDropCodeTunnelAdaptDrop = 21,
    rxPktDropCodePvlanDrop = 22,
    rxPktDropCodeVfp = 23,
    rxPktDropCodeIfp = 24,
    rxPktDropCodeIfpMeter = 25,
    rxPktDropCodeDstFp = 26,
    rxPktDropCodeMplsProtectionDrop = 27,
    rxPktDropCodeMplsLabelActionInvalid = 28,
    rxPktDropCodeMplsTermPolicySelectTableDrop = 29,
    rxPktDropCodeMplsReservedLabelExposed = 30,
    rxPktDropCodeMplsTtlError = 31,
    rxPktDropCodeMplsEcnError = 32,
    rxPktDropCodeEmFt = 33,
    rxPktDropCodeIvxlt = 34,
    rxPktDropCodeUrpfCheckFailed = 90,
    rxPktDropCodeSrcPortKnockoutDrop = 91,
    rxPktDropCodeLagFailoverPortDown = 92,
    rxPktDropCodeSplitHorizonCheckFailed = 93,
    rxPktDropCodeDstLinkDown = 94,
    rxPktDropCodeBlockMaskDrop = 95,
    rxPktDropCodeL3MtuCheckFailed = 96,
    rxPktDropCodeSeqNumCheckFailed = 97,
    rxPktDropCodeL3IifEqL3Oif = 98,
    rxPktDropCodeStormControlDrop = 99,
    rxPktDropCodeEgrMembershipCheckFailed = 100,
    rxPktDropCodeEgrSpanningTreeCheckFailed = 101,
    rxPktDropCodeDstPbmZero = 102,
    rxPktDropCodeMplsCtrlPktDrop = 103,
    rxPktDropCodeCount = 104,
} bcmint_rx_pkt_high_drop_code_t;

/*
 * \brief RX drop event structure.
 */
typedef struct bcmint_rx_drop_event_convert_s {

    /*! Pkt drop event. */
    bcm_pkt_drop_event_t drop_event;

    /*! Rx drop code. */
    bcmint_rx_pkt_high_drop_code_t drop_code;

} bcmint_rx_drop_event_convert_t;

typedef enum bcmint_rx_pkt_trace_code_e {
    rxPktTraceCodeNoCopyToCpu = 0,                    /* NO_COPY_TO_CPU = 0,                     */
    rxPktTraceCodeCmlFlags = 1,                       /* CML_FLAGS = 1,                          */
    rxPktTraceCodeL2SrcStaticMove = 2,                /* L2_SRC_STATIC_MOVE = 2,                 */
    rxPktTraceCodeL2SrcDiscard = 3,                   /* L2_SRC_DISCARD = 3,                     */
    rxPktTraceCodeMacsaMulticast = 4,                 /* MACSA_MULTICAST = 4,                    */
    rxPktTraceCodePktIntegrityCheckFailed = 5,        /* PKT_INTEGRITY_CHECK_FAILED = 5,         */
    rxPktTraceCodeProtocolPkt = 6,                    /* PROTOCOL_PKT = 6,                       */
    rxPktTraceCodeMembershipCheckFailed = 7,          /* MEMBERSHIP_CHECK_FAILED = 7,            */
    rxPktTraceCodeSpanningTreeCheckFailed = 8,        /* SPANNING_TREE_CHECK_FAILED = 8,         */
    rxPktTraceCodeL2DstLookupMiss = 9,                /* L2_DST_LOOKUP_MISS = 9,                 */
    rxPktTraceCodeL2DstLookup = 10,                   /* L2_DST_LOOKUP = 10,                     */
    rxPktTraceCodeL3DstLookup_miss = 11,              /* L3_DST_LOOKUP_MISS = 11,                */
    rxPktTraceCodeL3DstLookup = 12,                   /* L3_DST_LOOKUP = 12,                     */
    rxPktTraceCodeL3HdrError = 13,                    /* L3_HDR_ERROR = 13,                      */
    rxPktTraceCodeL3TtlError = 14,                    /* L3_TTL_ERROR = 14,                      */
    rxPktTraceCodeIpmcL3IifOrRpaIdCheckFailed =15,    /* IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILED =15, */
    rxPktTraceCodeLearnCacheFull = 16,                /* LEARN_CACHE_FULL = 16,                  */
    rxPktTraceCodeVfp = 17,                           /* VFP = 17,                               */
    rxPktTraceCodeIfp = 18,                           /* IFP = 18,                               */
    rxPktTraceCodeIfpMeter = 19,                      /* IFP_METER = 19,                         */
    rxPktTraceCodeDstFp = 20,                         /* DST_FP = 20,                            */
    rxPktTraceCodeSvp = 21,                               /*  SVP = 21,                                     */
    rxPktTraceCodeEmFt = 22,                              /*  EM_FT = 22,                                   */
    rxPktTraceCodeIvxlt = 23,                             /*  IVXLT = 23,                                   */
    rxPktTraceCodeMirrorSamplerSampled = 24,              /*  MIRROR_SAMPLER_SAMPLED = 24,                  */
    rxPktTraceCodeMirrorSamplerEgrSampled = 25,           /*  MIRROR_SAMPLER_EGR_SAMPLED = 25,              */
    rxPktTraceCodeSerDrop = 26,                           /*  SER_DROP = 26,                                */
    rxPktTraceCodeUrpfCheckFailed = 27,                   /*  URPF_CHECK_FAILED = 27,                       */
    rxPktTraceCodeL3IifEqL3Oif = 28,                      /*  L3_IIF_EQ_L3_OIF = 28,                        */
    rxPktTraceCodeDlbEcmpMonitorEnOrMemberReassined = 29, /*  DLB_ECMP_MONITOR_EN_OR_MEMBER_REASSINED = 29, */
    rxPktTraceCodeDlbLagMonitorEnOrMemberEeassined = 30,  /*  DLB_LAG_MONITOR_EN_OR_MEMBER_REASSINED = 30,  */
    rxPktTraceCodeMplsCtrlPktToCpu = 31,                  /*  MPLS_CTRL_PKT_TO_CPU = 31,                    */
    rxPktTraceCodeMatchedRuleBit0 = 32,                   /*  MATCHED_RULE_BIT_0 = 32,                      */
    rxPktTraceCodeMatchedRuleBit1 = 33,                   /*  MATCHED_RULE_BIT_1 = 33,                      */
    rxPktTraceCodeMatchedRuleBit2 = 34,                   /*  MATCHED_RULE_BIT_2 = 34,                      */
    rxPktTraceCodeMatchedRuleBit3 = 35,                   /*  MATCHED_RULE_BIT_3 = 35,                      */
    rxPktTraceCodeMatchedRuleBit4 = 36,                   /*  MATCHED_RULE_BIT_4 = 36,                      */
    rxPktTraceCodeMatchedRuleBit5 = 37,                   /*  MATCHED_RULE_BIT_5 = 37,                      */
    rxPktTraceCodeMatchedRuleBit6 = 38,                   /*  MATCHED_RULE_BIT_6 = 38,                      */
    rxPktTraceCodeMatchedRuleBit7 = 39,                   /*  MATCHED_RULE_BIT_7 = 39,                      */
    rxPktTraceCodeTraceDoNotMirror = 45,                  /*  TRACE_DO_NOT_MIRROR = 45,                     */
    rxPktTraceCodeTraceDoNotCopyToCpu = 46,               /*  TRACE_DO_NOT_COPY_TO_CPU = 46,                */
    rxPktTraceCodeTraceDop = 47                           /*  TRACE_DOP = 47                                */
} bcmint_rx_pkt_trace_code_t;

/*
 * \brief RX drop event structure.
 */
typedef struct bcmint_rx_trace_event_convert_s {

    /*! Pkt trace event. */
    bcm_pkt_trace_event_t trace_event;

    /*! Rx trace code. */
    bcmint_rx_pkt_trace_code_t trace_code;

} bcmint_rx_trace_event_convert_t;

/*
 * \brief rx event handle.
 */
/* RX_EVENT_INFO_ID type handle. */
#define BCMINT_RX_EVENT_INFO_ID 0

/*
 * \brief RX event database structure.
 */
typedef struct bcmint_rx_event_db_s {

    /* rx event handle. */
    int hdl;

    /* drop info. */
    const bcmint_rx_drop_event_convert_t *drop_info;

    /* Num of elements in drop info. */
    int drop_num;

    /* trace info. */
    const bcmint_rx_trace_event_convert_t *trace_info;

    /* Num of elements in trace info. */
    int trace_num;

} bcmint_rx_event_db_t;

/*
 * \brief RX map database structure.
 */
typedef struct bcmint_rx_map_db_s {

    /*! String database. */
    const bcmint_rx_str_map_db_t *str_db;

    /*! Num of elements in string database. */
    int str_num;

    /*! Event database. */
    const bcmint_rx_event_db_t *event_db;

    /*! Number of elements in event database. */
    int event_num;

} bcmint_rx_map_db_t;

typedef struct bcmint_tm_cos_q_cpu_map_info_s {

    /* key0. */
    const char *key0;

    /* key1. */
    const char *key1;

    /* key2. */
    const char *key2;

} bcmint_tm_cos_q_cpu_map_info_t;

#endif /* !BCMI_LTSW_RX_INT_H */
