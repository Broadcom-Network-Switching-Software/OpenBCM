/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * File:    ft.h
 * Purpose: FT definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_FT_H
#define _SOC_SHARED_FT_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

/*
 * FT uC Error codes
 */
typedef enum shr_ft_uc_error_e {
    SHR_FT_UC_E_NONE      = 0,
    SHR_FT_UC_E_INTERNAL  = 1,
    SHR_FT_UC_E_MEMORY    = 2,
    SHR_FT_UC_E_UNIT      = 3,
    SHR_FT_UC_E_PARAM     = 4,
    SHR_FT_UC_E_EMPTY     = 5,
    SHR_FT_UC_E_FULL      = 6,
    SHR_FT_UC_E_NOT_FOUND = 7,
    SHR_FT_UC_E_EXISTS    = 8,
    SHR_FT_UC_E_TIMEOUT   = 9,
    SHR_FT_UC_E_BUSY      = 10,
    SHR_FT_UC_E_FAIL      = 11,
    SHR_FT_UC_E_DISABLED  = 12,
    SHR_FT_UC_E_BADID     = 13,
    SHR_FT_UC_E_RESOURCE  = 14,
    SHR_FT_UC_E_CONFIG    = 15,
    SHR_FT_UC_E_UNAVAIL   = 16,
    SHR_FT_UC_E_INIT      = 17,
    SHR_FT_UC_E_PORT      = 18
} shr_ft_uc_error_t;

/* Max flex counter pools across all devices supporting FT */
#define SHR_FT_MAX_CTR_POOLS  20

/* Key Word size */
#define SHR_FT_EM_KEY_DWORD_SIZE 32

/* Max number of parts of a qualifier */
#define SHR_FT_EM_KEY_MAX_QUAL_PARTS 8

/* Supported qualifiers in EM key */
typedef enum shr_ft_em_qual_e {
    SHR_FT_EM_QUAL_SRC_IPV4       = 0,
    SHR_FT_EM_QUAL_DST_IPV4       = 1,
    SHR_FT_EM_QUAL_L4_SRC_PORT    = 2,
    SHR_FT_EM_QUAL_L4_DST_PORT    = 3,
    SHR_FT_EM_QUAL_IP_PROTOCOL    = 4,
    SHR_FT_EM_QUAL_VNID           = 5,
    SHR_FT_EM_QUAL_UDF_CHUNK0     = 6,
    SHR_FT_EM_QUAL_UDF_CHUNK1     = 7,
    SHR_FT_EM_QUAL_UDF_CHUNK2     = 8,
    SHR_FT_EM_QUAL_UDF_CHUNK3     = 9,
    SHR_FT_EM_QUAL_UDF_CHUNK4     = 10,
    SHR_FT_EM_QUAL_UDF_CHUNK5     = 11,
    SHR_FT_EM_QUAL_UDF_CHUNK6     = 12,
    SHR_FT_EM_QUAL_UDF_CHUNK7     = 13,
    SHR_FT_EM_QUAL_UDF_CHUNK8     = 14,
    SHR_FT_EM_QUAL_UDF_CHUNK9     = 15,
    SHR_FT_EM_QUAL_UDF_CHUNK10    = 16,
    SHR_FT_EM_QUAL_UDF_CHUNK11    = 17,
    SHR_FT_EM_QUAL_UDF_CHUNK12    = 18,
    SHR_FT_EM_QUAL_UDF_CHUNK13    = 19,
    SHR_FT_EM_QUAL_UDF_CHUNK14    = 20,
    SHR_FT_EM_QUAL_UDF_CHUNK15    = 21,
    SHR_FT_EM_QUAL_LT_ID          = 22,
    SHR_FT_EM_QUAL_IN_PORT        = 23,
    SHR_FT_EM_QUAL_COUNT
} shr_ft_em_qual_t;

/* Maximum number of EM qualifiers that can be part of an EM group. */
#define SHR_FT_EM_MAX_QUAL (19)

#define SHR_FT_EM_QUAL_NAMES                            \
    {                                                   \
        "Src IPv4     ",                                \
        "Dst IPv4     ",                                \
        "L4 Src Port  ",                                \
        "L4 Dst Port  ",                                \
        "IP Protocol  ",                                \
        "VNID         ",                                \
        "UDF Chunk 0  ",                                \
        "UDF Chunk 1  ",                                \
        "UDF Chunk 2  ",                                \
        "UDF Chunk 3  ",                                \
        "UDF Chunk 4  ",                                \
        "UDF Chunk 5  ",                                \
        "UDF Chunk 6  ",                                \
        "UDF Chunk 7  ",                                \
        "UDF Chunk 8  ",                                \
        "UDF Chunk 9  ",                                \
        "UDF Chunk 10 ",                                \
        "UDF Chunk 11 ",                                \
        "UDF Chunk 12 ",                                \
        "UDF Chunk 13 ",                                \
        "UDF Chunk 14 ",                                \
        "UDF Chunk 15 ",                                \
        "LT ID        ",                                \
        "In Port      "                                 \
    }

/* Supported tracking params */
typedef enum shr_ft_tp_e {
    SHR_FT_TP_SRC_IPV4             = 0,
    SHR_FT_TP_DST_IPV4             = 1,
    SHR_FT_TP_L4_SRC_PORT          = 2,
    SHR_FT_TP_L4_DST_PORT          = 3,
    SHR_FT_TP_IP_PROTOCOL          = 4,
    SHR_FT_TP_VNID                 = 5,
    SHR_FT_TP_CUSTOM               = 6,
    SHR_FT_TP_INNER_SRC_IPV4       = 7,
    SHR_FT_TP_INNER_DST_IPV4       = 8,
    SHR_FT_TP_INNER_L4_SRC_PORT    = 9,
    SHR_FT_TP_INNER_L4_DST_PORT    = 10,
    SHR_FT_TP_INNER_IP_PROTOCOL    = 11,
    SHR_FT_TP_IN_PORT              = 12,
    SHR_FT_TP_INNER_SRC_IPV6       = 13,
    SHR_FT_TP_INNER_DST_IPV6       = 14,
    SHR_FT_TP_COUNT
} shr_ft_tp_t;

#define SHR_FT_TP_NAMES                                 \
    {                                                   \
        "Src IPv4          ",                           \
        "Dst IPv4          ",                           \
        "L4 Src Port       ",                           \
        "L4 Dst Port       ",                           \
        "IP Protocol       ",                           \
        "VNID              ",                           \
        "Custom            ",                           \
        "Inner Src IPv4    ",                           \
        "Inner Dst IPv4    ",                           \
        "Inner L4 Src Port ",                           \
        "Inner L4 Dst Port ",                           \
        "Inner IP Protocol ",                           \
        "In Port           ",                           \
        "Inner Src IPv6    ",                           \
        "Inner Dst IPv6    ",                           \
    }

/* Maximum number of tracking params that can be attached to a group. */
#define SHR_FT_MAX_TP (7)

/* Supported UDF base offsets */
typedef enum shr_ft_udf_base_offset_e {
    SHR_FT_UDF_BASE_OFFSET_INVALID   = 0,
    SHR_FT_UDF_BASE_OFFSET_OUTER_L2  = 1,
    SHR_FT_UDF_BASE_OFFSET_OUTER_L3  = 2,
    SHR_FT_UDF_BASE_OFFSET_OUTER_L4  = 3,
    SHR_FT_UDF_BASE_OFFSET_OUTER_L5  = 4,
    SHR_FT_UDF_BASE_OFFSET_MAX
} shr_ft_udf_base_offset_t;

#define SHR_FT_BASE_OFFSET_NAMES                        \
    {                                                   \
        "Invalid  ",                                    \
        "Outer L2 ",                                    \
        "Outer L3 ",                                    \
        "Outer L4 ",                                    \
        "Outer L5 "                                     \
    }

/* Max size of the IPFIX packet encap */
#define SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH 128

/* Max size of the template set encap */
#define SHR_FT_MAX_TEMPLATE_SET_ENCAP_LENGTH 256

/* Minimum export length required */
#define SHR_FT_MIN_EXPORT_LENGTH 128

/* Max export length supported 9K - Jumbo */
#define SHR_FT_MAX_EXPORT_LENGTH 9000

/* Max number of pipes across all devices */
#define SHR_FT_MAX_PIPES 8

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_FT_MAX_UC_MSG_TIMEOUT 5000000 /* 5 secs */

/* Max number of counters per pipe */
#define SHR_FT_MAX_COUNTERS 2

/* Max number of gports Congestion monitoring */
#define SHR_FT_QCM_MAX_PORTS 164

/* Minimum export length required */
#define SHR_FT_QCM_MIN_EXPORT_LENGTH 768

/* Max number of parts of a qualifier */
#define SHR_FT_QCM_EM_KEY_MAX_QUAL_PARTS 64

/* Maximum number of IPFIX templates. */
#define SHR_FT_MAX_TEMPLATES (3)

/* Group update params */
typedef enum shr_ft_group_update_e {
    SHR_FT_GROUP_UPDATE_INVALID                    = 0,
    SHR_FT_GROUP_UPDATE_FLOW_THRESHOLD             = 1,
    SHR_FT_GROUP_UPDATE_FLUSH                      = 2,
    SHR_FT_GROUP_UPDATE_COUNTER_CLEAR              = 3,
    SHR_FT_GROUP_UPDATE_OBSERVATION_DOMAIN         = 4,
    SHR_FT_GROUP_UPDATE_AGING_INTERVAL             = 5,
    SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_SET     = 6,
    SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_UNSET   = 7,
    SHR_FT_GROUP_UPDATE_ELPH_PROFILE               = 8,
    SHR_FT_GROUP_UPDATE_ELPH_QOS_PROFILE_ID        = 9,
    SHR_FT_GROUP_UPDATE_EM_GROUP_IDX               = 10,
    SHR_FT_GROUP_UPDATE_EXPORT_TRIGGERS            = 11,
    SHR_FT_GROUP_UPDATE_DEFAULT_GROUP              = 12,
    SHR_FT_GROUP_UPDATE_TEMPLATE_SET               = 13,
    SHR_FT_GROUP_UPDATE_TEMPLATE_UNSET             = 14,
    SHR_FT_GROUP_UPDATE_COLLECTOR_SET              = 15,
    SHR_FT_GROUP_UPDATE_COLLECTOR_UNSET            = 16,
    SHR_FT_GROUP_UPDATE_COUNT
} shr_ft_group_update_t;

/* List of supported Info elements that can be exported by app */
typedef enum shr_ft_template_info_elem_e {
    SHR_FT_TEMPLATE_INFO_ELEM_SRC_IPV4                = 0,
    SHR_FT_TEMPLATE_INFO_ELEM_DST_IPV4                = 1,
    SHR_FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT             = 2,
    SHR_FT_TEMPLATE_INFO_ELEM_L4_DST_PORT             = 3,
    SHR_FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL             = 4,
    SHR_FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT         = 5,
    SHR_FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT        = 6,
    SHR_FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC      = 7,
    SHR_FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC             = 8,
    SHR_FT_TEMPLATE_INFO_ELEM_GROUP_ID                = 9,
    SHR_FT_TEMPLATE_INFO_ELEM_VNID                    = 10,
    SHR_FT_TEMPLATE_INFO_ELEM_CUSTOM                  = 11,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4          = 12,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4          = 13,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT       = 14,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT       = 15,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL       = 16,
    SHR_FT_TEMPLATE_INFO_ELEM_QUEUE_DROP_BYTE_COUNT   = 17,
    SHR_FT_TEMPLATE_INFO_ELEM_QUEUE_DROP_PKT_COUNT    = 18,
    SHR_FT_TEMPLATE_INFO_ELEM_QUEUE_RX_BYTE_COUNT     = 19,
    SHR_FT_TEMPLATE_INFO_ELEM_QUEUE_RX_PKT_COUNT      = 20,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6          = 21,
    SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6          = 22,
    SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS
} shr_ft_template_info_elem_t;

/* Maximum number of filters (promotion + demotion) in an elephant profile */
#define SHR_FT_ELPH_MAX_FILTERS 3

/* Learn status */
typedef enum shr_ft_learn_pkt_status_e {
    SHR_FT_LEARN_PKT_NOT_READY             = 0,
    SHR_FT_LEARN_PKT_RUNT                  = 1,
    SHR_FT_LEARN_PKT_NON_IP                = 2,
    SHR_FT_LEARN_PKT_NON_IPV4              = 3,
    SHR_FT_LEARN_PKT_INVALID_GROUP         = 4,
    SHR_FT_LEARN_PKT_GROUP_NOT_ENABLED     = 5,
    SHR_FT_LEARN_PKT_GROUP_THRESHOLD       = 6,
    SHR_FT_LEARN_PKT_PIPE_THRESHOLD        = 7,
    SHR_FT_LEARN_PKT_EM_LOOKUP_SCHAN_FAIL  = 8,
    SHR_FT_LEARN_PKT_DUPLICATE             = 9,
    SHR_FT_LEARN_PKT_NEW_FLOW              = 10,
    SHR_FT_LEARN_PKT_EM_INSERT_FAIL        = 11,
    SHR_FT_LEARN_PKT_UNKNOWN_PIPE          = 12,
    SHR_FT_LEARN_PKT_EM_GROUP_NOT_ATTACHED = 13,
    SHR_FT_LEARN_PKT_PARSE_ERROR           = 14,
    SHR_FT_LEARN_PKT_DROP_MONITOR_EVENT    = 15,
    SHR_FT_LEARN_PKT_DEFAULT_GROUP         = 16,
    SHR_FT_LEARN_PKT_DROP_NO_CHANGE        = 17,
    SHR_FT_LEARN_PKT_MAX_STATUS
} shr_ft_learn_pkt_status_t;

#define SHR_FT_LEARN_PKT_STATUS_NAMES                    \
    {                                                    \
            "Not Ready             ",                    \
            "Runt                  ",                    \
            "Non IP                ",                    \
            "Non IPv4              ",                    \
            "Invalid Group         ",                    \
            "Group Not enabled     ",                    \
            "Group Threshold       ",                    \
            "Pipe Threshold        ",                    \
            "EM lookup schan fail  ",                    \
            "Duplicate             ",                    \
            "New flow              ",                    \
            "EM insert fail        ",                    \
            "Unknown Pipe          ",                    \
            "EM group not attached ",                    \
            "Pkt parse error       ",                    \
            "Drop Monitor Event    ",                    \
            "Default Group         ",                    \
            "Drop No Change        "                     \
    }


/*
 * Flow states
 */
typedef enum shr_ft_elph_flow_state_e {
    SHR_FT_ELPH_FLOW_STATE_NEW           = 0,
    SHR_FT_ELPH_FLOW_STATE_UNDETERMINED  = 1,
    SHR_FT_ELPH_FLOW_STATE_MOUSE         = 2,
    SHR_FT_ELPH_FLOW_STATE_CANDIDATE     = 3,
    SHR_FT_ELPH_FLOW_STATE_ELEPHANT      = 4,
    SHR_FT_ELPH_FLOW_STATE_COUNT
} shr_ft_elph_flow_state_t;

#define SHR_FT_ELPH_STATES                          \
    {                                               \
            "New              ",                    \
            "Undetermined     ",                    \
            "Mouse            ",                    \
            "Candidate        ",                    \
            "Elephant         ",                    \
}

/* Elephant monitor actions */
typedef enum shr_ft_elph_action_e {
    SHR_FT_ELPH_ACTION_NONE         = 0,
    SHR_FT_ELPH_ACTION_PROMOTE      = 1,
    SHR_FT_ELPH_ACTION_DEMOTE       = 2,
    SHR_FT_ELPH_ACTION_COUNT
} shr_ft_elph_action_t;

#define SHR_FT_ELPH_ACTIONS                         \
{                                                   \
            "None             ",                    \
            "Promote          ",                    \
            "Demote           ",                    \
}

/* Flowtracker config modes */
typedef enum shr_ft_cfg_modes_e {
    /*
     * V1 - Implicit EM group creation
     *    - Dedicated collector APIs
     *    - Fixed IPv4 5-tuple tracking
     *    - Only IPFIX export
     */
    SHR_FT_CFG_MODE_V1 = 1,

    /*
     * V2 - EM group creation by customer
     *    - Use common collector infra
     *    - Support IPFIX & protoBuf based export
     *    - Flexible tracking params
     */
    SHR_FT_CFG_MODE_V2 = 2,

    SHR_FT_CFG_MODE_COUNT
} shr_ft_cfg_modes_t;

/* Features supported by uC */
#define SHR_FT_UC_FEATURE_ELPH                      (1 << 0)
#define SHR_FT_UC_FEATURE_IPFIX_EXPORT              (1 << 1)
#define SHR_FT_UC_FEATURE_PROTOBUF_EXPORT           (1 << 2)
#define SHR_FT_UC_FEATURE_DROP_MONITOR              (1 << 3)
#define SHR_FT_UC_FEATURE_FSP_REINJECT              (1 << 4)
#define SHR_FT_UC_FEATURE_EXPORT_TRIGGER_NEW_LEARN  (1 << 5)
#define SHR_FT_UC_FEATURE_EXPORT_TRIGGER_AGE_OUT    (1 << 6)
#define SHR_FT_UC_FEATURE_EXPORT_TRIGGER_DROP       (1 << 7)
#define SHR_FT_UC_FEATURE_EXPORT_INTERVAL_CHANGE    (1 << 8)
#define SHR_FT_UC_FEATURE_CONGESTION_MONITOR        (1 << 9)
#define SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES  (1 << 10)
/* Max number of supported EM groups */
#define SHR_FT_MAX_EM_GROUPS 4

/* Max length of the custom key */
#define SHR_FT_CUSTOM_KEY_MAX_LENGTH 16

/* EM actions supported */
#define SHR_FT_EM_ACTION_METER (1 << 0)
#define SHR_FT_EM_ACTION_CTR   (1 << 1)

/* Flow group actions */
#define SHR_FT_GROUP_ACTION_FSP_REINJECT            (1 << 0)
#define SHR_FT_GROUP_ACTION_DROP_MONITOR            (1 << 1)
#define SHR_FT_GROUP_ACTION_CONGESTION_MONITOR      (1 << 2)

/* Max length of the system ID */
#define SHR_FT_PB_SYSTEM_ID_MAX_LENGTH 32

/* Export Triggers */
#define SHR_FT_EXPORT_TRIGGER_PERIODIC              (1 << 0)
#define SHR_FT_EXPORT_TRIGGER_NEW_LEARN             (1 << 1)
#define SHR_FT_EXPORT_TRIGGER_AGE_OUT               (1 << 2)
#define SHR_FT_EXPORT_TRIGGER_DROP                  (1 << 3)
#define SHR_FT_EXPORT_TRIGGER_CONGESTION_MONITOR    (1 << 4)

/* EM Class Id */
#define SHR_FT_EM_DROP_CLASS_ID   (1 << 0)

/* Max number of drop reasons per flow */
#define SHR_FT_DROP_REASONS_MAX_COUNT 2

/* Max number of Interest Factor computation weights */
#define SHR_FT_QCM_INTEREST_FACTOR_WEIGHT_MAX_COUNT 24

typedef enum shr_ft_drop_rx_reasons_e {
    /* These errors have a direct mapping to the drop reasons in proto file,
     * do not change the order
     */
    SHR_FT_DROP_RX_REASON_INVALID                       = 0,
    SHR_FT_DROP_RX_REASON_L3_SOURCE_MISS                = 1,
    SHR_FT_DROP_RX_REASON_L3_DEST_MISS                  = 2,
    SHR_FT_DROP_RX_REASON_MCAST_MISS                    = 3,
    SHR_FT_DROP_RX_REASON_IP_MCAST_MISS                 = 4,
    SHR_FT_DROP_RX_REASON_UNKNOWN_VLAN                  = 5,
    SHR_FT_DROP_RX_REASON_L3_HEADER_MISMATCH            = 6,
    SHR_FT_DROP_RX_REASON_DOS_ATTACK                    = 7,
    SHR_FT_DROP_RX_REASON_MARTIAN_ADDR                  = 8,
    SHR_FT_DROP_RX_REASON_TUNNEL_ERROR                  = 9,
    SHR_FT_DROP_RX_REASON_PARITY_ERROR                  = 10,
    SHR_FT_DROP_RX_REASON_L3_MTU_FAIL                   = 11,
    SHR_FT_DROP_RX_REASON_HIGIG_HDR_ERROR               = 12,
    SHR_FT_DROP_RX_REASON_MCAST_IDX_ERROR               = 13,
    SHR_FT_DROP_RX_REASON_CLASS_BASED_MOVE              = 14,
    SHR_FT_DROP_RX_REASON_L3_ADDR_BIND_FAIL             = 15,
    SHR_FT_DROP_RX_REASON_MPLS_LABEL_MISS               = 16,
    SHR_FT_DROP_RX_REASON_MPLS_INVALID_ACTION           = 17,
    SHR_FT_DROP_RX_REASON_MPLS_INVALID_PAYLOAD          = 18,
    SHR_FT_DROP_RX_REASON_TUNNEL_OBJECT_VALIDATION_FAIL = 19,
    SHR_FT_DROP_RX_REASON_MPLS_SEQUENCE_NUMBER          = 20,
    SHR_FT_DROP_RX_REASON_L2_NON_UNICAST_MISS           = 21,
    SHR_FT_DROP_RX_REASON_NIV_PRIO_DROP                 = 22,
    SHR_FT_DROP_RX_REASON_NIV_RPF_FAIL                  = 23,
    SHR_FT_DROP_RX_REASON_UNKNOWN_SUBTENDING_PORT       = 24,
    SHR_FT_DROP_RX_REASON_TUNNEL_ADAPT_LOOKUP_MISS      = 25,
    SHR_FT_DROP_RX_REASON_PACKET_FLOW_SELECT_MISS       = 26,
    SHR_FT_DROP_RX_REASON_TUNNEL_DECAP_ECN_ERROR        = 27,
    SHR_FT_DROP_RX_REASON_FAILOVER_DROP                 = 28,
    SHR_FT_DROP_RX_REASON_OTHER_LOOKUP_MISS             = 29,
    SHR_FT_DROP_RX_REASON_INVALID_TPID                  = 30,
    SHR_FT_DROP_RX_REASON_TUNNEL_TTL_ERROR              = 31,
    SHR_FT_DROP_RX_REASON_MPLS_ILLEGAL_RESERVED_LABEL   = 32,
    SHR_FT_DROP_RX_REASON_L3_HEADER_ERROR               = 33,
    SHR_FT_DROP_RX_REASON_L2_HEADER_ERROR               = 34,
    SHR_FT_DROP_RX_REASON_TTL1                          = 35,
    SHR_FT_DROP_RX_REASON_TTL                           = 36,
    SHR_FT_DROP_RX_REASON_FCOE_ZONE_CHECK_FAIL          = 37,
    SHR_FT_DROP_RX_REASON_IPMC_INTERFACE_MISMATCH       = 38,
    SHR_FT_DROP_RX_REASON_MPLS_TTL                      = 39,
    SHR_FT_DROP_RX_REASON_MPLS_UNKNOWN_ACH              = 40,
    SHR_FT_DROP_RX_REASON_OAM_ERROR                     = 41,
    SHR_FT_DROP_RX_REASON_L2_GRE_SIP_MISS               = 42,
    SHR_FT_DROP_RX_REASON_L2_GRE_VPNID_MISS             = 43,
    SHR_FT_DROP_RX_REASON_BFD_ERROR                     = 44,
    SHR_FT_DROP_RX_REASON_CONGESTION_CNM_PROXY_ERROR    = 45,
    SHR_FT_DROP_RX_REASON_VXLAN_SIP_MISS                = 46,
    SHR_FT_DROP_RX_REASON_VXLAN_VPNID_MISS              = 47,
    SHR_FT_DROP_RX_REASON_NIV_INTERFACE_MISS            = 48,
    SHR_FT_DROP_RX_REASON_NIV_TAG_INVALID               = 49,
    SHR_FT_DROP_RX_REASON_NIV_TAG_DROP                  = 50,
    SHR_FT_DROP_RX_REASON_NIV_UNTAG_DROP                = 51,
    SHR_FT_DROP_RX_REASON_TRILL_INVALID                 = 52,
    SHR_FT_DROP_RX_REASON_TRILL_MISS                    = 53,
    SHR_FT_DROP_RX_REASON_TRILL_RPF_FAIL                = 54,
    SHR_FT_DROP_RX_REASON_TRILL_TTL                     = 55,
    SHR_FT_DROP_RX_REASON_NAT_ERROR                     = 56,
    SHR_FT_DROP_RX_REASON_TCP_UDP_NAT_MISS              = 57,
    SHR_FT_DROP_RX_REASON_ICMP_NAT_MISS                 = 58,
    SHR_FT_DROP_RX_REASON_NAT_FRAGMENT                  = 59,
    SHR_FT_DROP_RX_REASON_NAT_MISS                      = 60,

    /* Indicates end of directly usable drop reasons */
    SHR_FT_DROP_RX_REASON_MAX_VALUE,

    /* These are errors with encoding which must be resolved, always must be
     * last
     */
    /* Dummy error to mark start of encoded errors */
    SHR_FT_DROP_RX_REASON_ENCODED_ERRORS_START = SHR_FT_DROP_RX_REASON_MAX_VALUE,
    SHR_FT_DROP_RX_REASON_NIV_ENCODING,
    SHR_FT_DROP_RX_REASON_TRILL_ENCODING,
    SHR_FT_DROP_RX_REASON_NAT_ENCODING,
    SHR_FT_DROP_RX_REASON_MPLS_ENCODING,

    /* Always last, not usable */
    SHR_FT_DROP_RX_REASONS_COUNT
} shr_ft_drop_rx_reasons_t;

#endif /* _SOC_SHARED_FT_H */
