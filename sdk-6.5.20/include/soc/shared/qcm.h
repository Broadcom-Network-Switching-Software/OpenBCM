/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File:    qcm.h
 * Purpose: QCM definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_QCM_H
#define _SOC_SHARED_QCM_H

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

#define SHR_QCM_CONFIG_STATUS_BIT            0x1
#define SHR_QCM_INIT_STATUS_BIT              0x2
#define SHR_QCM_CCMDMA_STATUS_BIT            0x4
#define SHR_QCM_APP_STATUS_1                 0x51434D5F
#define SHR_QCM_APP_STATUS_2                 0x5F415050
#define SHR_QCM_TIME_USEC                    0x100000
#define SHR_QCM_MAX_GPORT_MONITOR            0xA4
#define SHR_QCM_MAX_FLOW_VIEWS               0x10
#define SHR_QCM_ERR_OFFSET                   3
#define SHR_QCM_MEM_MIN_SIZE                 0x4000
#define SHR_QCM_MAX_FLOW_MONITOR             1000
#define SHR_QCM_MIN_SCAN_INTERVAL            100
#define SHR_QCM_MAX_NUM_SAMPLE               16
#define SHR_QCM_TUPLE_SIZE                   40
#define SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID  12
#define SHR_QCM_FLOW_VIEW_SUMMARY_SIZE       176
#define SHR_QCM_NUM_BYTE_COUNT               6
#define SHR_QCM_NUM_PKT_COUNT                2
#define SHR_QCM_NUM_SHARE_COUNT              3
#define SHR_QCM_PORT_QUEUE_SIZE              3
#define SHR_QCM_IPV6_ADDR_LEN                16

/* Max number of supported EM groups */
#define SHR_QCM_MAX_EM_GROUPS 2

/* Max number of supported Flow groups */
#define SHR_QCM_MAX_FLOW_GROUPS 15

/*
 * QCM uC Error codes
 */
typedef enum shr_qcm_uc_error_e {
    SHR_QCM_UC_E_NONE      = 0,
    SHR_QCM_UC_E_INTERNAL  = 1,
    SHR_QCM_UC_E_MEMORY    = 2,
    SHR_QCM_UC_E_UNIT      = 3,
    SHR_QCM_UC_E_PARAM     = 4,
    SHR_QCM_UC_E_EMPTY     = 5,
    SHR_QCM_UC_E_FULL      = 6,
    SHR_QCM_UC_E_NOT_FOUND = 7,
    SHR_QCM_UC_E_EXISTS    = 8,
    SHR_QCM_UC_E_TIMEOUT   = 9,
    SHR_QCM_UC_E_BUSY      = 10,
    SHR_QCM_UC_E_FAIL      = 11,
    SHR_QCM_UC_E_DISABLED  = 12,
    SHR_QCM_UC_E_BADID     = 13,
    SHR_QCM_UC_E_RESOURCE  = 14,
    SHR_QCM_UC_E_CONFIG    = 15,
    SHR_QCM_UC_E_UNAVAIL   = 16,
    SHR_QCM_UC_E_INIT      = 17,
    SHR_QCM_UC_E_PORT      = 18
} shr_qcm_uc_error_t;

/* List of supported Info elements that can be exported by app */
typedef enum shr_qcm_template_info_elem_e {
    SHR_QCM_TEMPLATE_INFO_ELEM_SRC_IP                  = 0,
    SHR_QCM_TEMPLATE_INFO_ELEM_DST_IP                  = 1,
    SHR_QCM_TEMPLATE_INFO_ELEM_L4_SRC_PORT             = 2,
    SHR_QCM_TEMPLATE_INFO_ELEM_L4_DST_PORT             = 3,
    SHR_QCM_TEMPLATE_INFO_ELEM_IP_PROTOCOL             = 4,
    SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_RX_BYTE_COUNT      = 5,
    SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_VIEW_START_TS      = 6,
    SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_VIEW_ID            = 7,
    SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_COUNT              = 8,
    SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_DROP_BYTE_COUNT   = 9,
    SHR_QCM_TEMPLATE_INFO_ELEM_EGR_PORT                = 10,
    SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_ID                = 11,
    SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_BUF_USAGE         = 12,
    SHR_QCM_TEMPLATE_INFO_ELEM_ING_PORT                = 13,
    SHR_QCM_TEMPLATE_INFO_ELEM_IP_VERSION              = 14,
    SHR_QCM_TEMPLATE_INFO_ELEM_BUF_USAGE               = 15,
    SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_RX_BYTE_COUNT     = 16,
    SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_RX_PKT_COUNT       = 17,
    SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_DROP_PKT_COUNT    = 18,
    SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_RX_PKT_COUNT      = 19,
    SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS
} shr_qcm_template_info_elem_t;

/* Supported qualifiers in EM key */
typedef enum shr_qcm_em_qual_e {
    SHR_QCM_EM_QUAL_SRC_IP         = 0,
    SHR_QCM_EM_QUAL_DST_IP         = 1,
    SHR_QCM_EM_QUAL_L4_SRC_PORT    = 2,
    SHR_QCM_EM_QUAL_L4_DST_PORT    = 3,
    SHR_QCM_EM_QUAL_IP_PROTOCOL    = 4,
    SHR_QCM_EM_QUAL_ING_PORT       = 5,
    SHR_QCM_EM_QUAL_LT_ID          = 6,
    SHR_QCM_EM_MAX_QUAL
} shr_qcm_em_qual_t;

#define SHR_QCM_EM_QUAL_NAMES                           \
    {                                                   \
        "Src IP       ",                                \
        "Dst IP       ",                                \
        "L4 Src Port  ",                                \
        "L4 Dst Port  ",                                \
        "IP Protocol  ",                                \
        "Ing Port     "                                 \
    }

/* Supported tracking params */
typedef enum shr_qcm_tp_e {
    SHR_QCM_TP_SRC_IP               = 0,
    SHR_QCM_TP_DST_IP               = 1,
    SHR_QCM_TP_L4_SRC_PORT          = 2,
    SHR_QCM_TP_L4_DST_PORT          = 3,
    SHR_QCM_TP_IP_PROTOCOL          = 4,
    SHR_QCM_TP_ING_PORT             = 5,
    SHR_QCM_MAX_TP
} shr_qcm_tp_t;

#define SHR_QCM_TP_NAMES                                \
    {                                                   \
        "Src IP            ",                           \
        "Dst IP            ",                           \
        "L4 Src Port       ",                           \
        "L4 Dst Port       ",                           \
        "IP Protocol       ",                           \
        "Ing Port          "                            \
    }

/* Queue congestion monitor per flow queue data. */
typedef struct shr_qcm_queue_data_s {
    /* Queue Drop Byte Count */
    uint8 drop_byte_cnt[SHR_QCM_MAX_NUM_SAMPLE][SHR_QCM_NUM_BYTE_COUNT];

    /* Queue Drop Pkt Count */
    uint8 drop_pkt_cnt[SHR_QCM_MAX_NUM_SAMPLE][SHR_QCM_NUM_PKT_COUNT];

    /* Queue Rx Byte Count */
    uint8 rx_byte_cnt[SHR_QCM_MAX_NUM_SAMPLE][SHR_QCM_NUM_BYTE_COUNT];

    /* Queue Rx Pkt Count */
    uint8 rx_pkt_cnt[SHR_QCM_MAX_NUM_SAMPLE][SHR_QCM_NUM_PKT_COUNT];

    /* Queue shared buffer utilization */
    uint16 shared_buf_util[SHR_QCM_MAX_NUM_SAMPLE];

    /* Port Number */
    uint16 port_num;

    /* Queue ID */
    uint8 queue_id;
} shr_qcm_queue_data_t;

/* Queue congestion monitor per flow data. */
typedef struct shr_qcm_flow_data_s {
    /* flow tuple information */
    uint8 tuple[SHR_QCM_TUPLE_SIZE];

    /* Flow Rx Byte Count */
    uint8 byte_cnt[SHR_QCM_MAX_NUM_SAMPLE][SHR_QCM_NUM_BYTE_COUNT];

    /* Flow Rx Pkt Count */
    uint8 pkt_cnt[SHR_QCM_MAX_NUM_SAMPLE][SHR_QCM_NUM_PKT_COUNT];

    /* Queue congestion monitor per flow queue information */
    shr_qcm_queue_data_t queue_data;
} shr_qcm_flow_data_t;

/* Queue congestion monitor flow view data. */
typedef struct shr_qcm_flow_view_data_s {
    /* Start Time stamp of flow view */
    uint64 time_stamp;

    /* Number of flows in current view */
    uint16 num_flows;

    /* Flow view ID */
    uint16 view_id;
} shr_qcm_flow_view_data_t;

typedef enum shr_ft_qcm_learn_pkt_status_e {
    SHR_FT_QCM_LEARN_PKT_NOT_READY             = 0,
    SHR_FT_QCM_LEARN_PKT_RUNT                  = 1,
    SHR_FT_QCM_LEARN_PKT_NON_IP                = 2,
    SHR_FT_QCM_LEARN_PKT_INVALID_GROUP         = 3,
    SHR_FT_QCM_LEARN_PKT_GROUP_NOT_ENABLED     = 4,
    SHR_FT_QCM_LEARN_PKT_GROUP_THRESHOLD       = 5,
    SHR_FT_QCM_LEARN_PKT_FLOW_THRESHOLD        = 6,
    SHR_FT_QCM_LEARN_PKT_EM_LOOKUP_SCHAN_FAIL  = 7,
    SHR_FT_QCM_LEARN_PKT_DUPLICATE             = 8,
    SHR_FT_QCM_LEARN_PKT_NEW_FLOW              = 9,
    SHR_FT_QCM_LEARN_PKT_EM_INSERT_FAIL        = 10,
    SHR_FT_QCM_LEARN_PKT_EM_GROUP_NOT_ATTACHED = 11,
    SHR_FT_QCM_LEARN_PKT_UNKNOWN_QUEUE         = 12,
    SHR_FT_QCM_LEARN_PKT_MAX_STATUS
} shr_qcm_learn_pkt_status_t;

#define SHR_FT_QCM_LEARN_PKT_STATUS_NAMES               \
{                                                \
    "Not Ready             ",                    \
    "Runt                  ",                    \
    "Non IP                ",                    \
    "Invalid Group         ",                    \
    "Group Not enabled     ",                    \
    "Group Threshold       ",                    \
    "Flow Threshold        ",                    \
    "EM lookup schan fail  ",                    \
    "Duplicate             ",                    \
    "New flow              ",                    \
    "EM insert fail        ",                    \
    "EM group not attached ",                    \
    "Unknown Queue         "                     \
}
#endif /* _SOC_SHARED_QCM_H */
