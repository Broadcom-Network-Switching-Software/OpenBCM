/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_export.c
 * Purpose:     Implementation of Flowtracker Export APIs.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_field.h>
#include <include/bcm_int/esw/flowtracker/ft_user.h>
#include <bcm_int/esw/flowtracker/ft_interface.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#include <soc/cmicm.h>


#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>

#define BCM_WB_FT_EXPORT_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_FT_EXPORT_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
/* 6.5.20 Sync/Recv for Export element info direction */
#define BCM_WB_FT_EXPORT_VERSION_1_3                SOC_SCACHE_VERSION(1,3)

/* Default warmboot version */
#define BCM_WB_FT_EXPORT_DEFAULT_VERSION            BCM_WB_FT_EXPORT_VERSION_1_3

#endif

#define BCMI_FT_COLLECTOR_FLUSH_TIMEOUT_RETRIES 250

/* Export software state. */
bcmi_ft_export_state_t *bcmi_ft_export_state[BCM_MAX_NUM_UNITS] = {0};

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
bcmi_aft_export_sw_ctrl_t *bcmi_aft_export_sw_ctrl[BCM_MAX_NUM_UNITS];

#define BCMI_AFT_EXPORT_SW_THREAD_PRI 50

#define BCMI_AFT_EXPORT_SW_COL_LOCK(_u_, _i_) \
    sal_mutex_take(bcmi_aft_export_sw_ctrl[_u_]->col_lock[_u_], sal_mutex_FOREVER)

#define BCMI_AFT_EXPORT_SW_COL_UNLOCK(_u_, _i_) \
    sal_mutex_give(bcmi_aft_export_sw_ctrl[_u_]->col_lock[_u_])

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#define BCMI_FT_XLATE_PARAM(_name, _size) \
        bcmFlowtrackerTrackingParamType##_name, \
        bcmFlowtrackerExportElementType##_name, \
        _size, \
        #_name

typedef struct bcmi_fte_template_record_elem_info_s {
    int length;
    bcm_flowtracker_export_element_type_t info_elem;
    bcm_flowtracker_tracking_param_type_t tracking_param;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_direction_t direction;
    uint16 custom_id;
} bcmi_fte_template_record_elem_info_t;

typedef struct bcmi_fte_template_record_info_s {
    int info_depth;
    int total_bits;
    int flow_key_offset;
    int flow_key_length;
    int flow_data_offset;
    int flow_data_length;
    bcmi_fte_template_record_elem_info_t *info;
} bcmi_fte_template_record_info_t;

#define BCMI_FTE_RECORD_ELEM_IDX_GET(_start_, _offset_, _len_, _idx_)       \
        _idx_ = ((_start_) - ((_offset_) + (_len_)))/ 8

#define BCMI_FTE_RECORD_ELEM_SET(_record_, _idx_, _info_elem_)              \
        (_record_)->info[(_idx_)].info_elem = (_info_elem_)

#define BCMI_FTE_RECORD_TRACKING_PARAM_SET(_record_, _idx_, _tracking_p_)   \
        (_record_)->info[(_idx_)].tracking_param = (_tracking_p_)

#define BCMI_FTE_RECORD_LENGTH_SET(_record_, _idx_, _length_)               \
        do {                                                                \
            int reset_count = 0;                                            \
            (_record_)->info[(_idx_)].length = (_length_);                  \
            reset_count = (_length_ - 8) / 8;                               \
            while (reset_count > 0) {                                       \
                (_record_)->info[(_idx_) + reset_count].length = 0;         \
                reset_count--;                                              \
            }                                                               \
        } while(0)

#define BCMI_FTE_RECORD_FLOW_CHECK_SET(_record_, _idx_, _check_id_)         \
        (_record_)->info[(_idx_)].check_id = (_check_id_)

#define BCMI_FTE_RECORD_FLOW_DIRECTION_SET(_record_, _idx_, _direction_)    \
        (_record_)->info[(_idx_)].direction = (_direction_)

#define BCMI_FTE_RECORD_ELEM_CUSTOMID_SET(_record_, _idx_, _custom_id_)     \
        (_record_)->info[(_idx_)].custom_id = (_custom_id_)

typedef struct bcmi_ft_export_elements_map_s {
    bcm_flowtracker_tracking_param_type_t tracking_param;
    bcm_flowtracker_export_element_type_t export_element;
    int default_data_size;
    char element_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
} bcmi_ft_export_elements_map_t;

static bcmi_ft_export_elements_map_t bcmi_ft_export_elements_map[] = {
            {   BCMI_FT_XLATE_PARAM(SrcIPv4,               4  )   },
            {   BCMI_FT_XLATE_PARAM(DstIPv4,               4  )   },
            {   BCMI_FT_XLATE_PARAM(L4SrcPort,             2  )   },
            {   BCMI_FT_XLATE_PARAM(L4DstPort,             2  )   },
            {   BCMI_FT_XLATE_PARAM(SrcIPv6,               16 )   },
            {   BCMI_FT_XLATE_PARAM(DstIPv6,               16 )   },
            {   BCMI_FT_XLATE_PARAM(IPProtocol,            1  )   },
            {   BCMI_FT_XLATE_PARAM(PktCount,              4  )   },
            {   BCMI_FT_XLATE_PARAM(ByteCount,             4  )   },
            {   BCMI_FT_XLATE_PARAM(VRF,                   2  )   },
            {   BCMI_FT_XLATE_PARAM(TTL,                   1  )   },
            {   BCMI_FT_XLATE_PARAM(IPLength,              2  )   },
            {   BCMI_FT_XLATE_PARAM(IP6Length,             2  )   },
            {   BCMI_FT_XLATE_PARAM(TcpWindowSize,         2  )   },
            {   BCMI_FT_XLATE_PARAM(DosAttack,             4  )   },
            {   BCMI_FT_XLATE_PARAM(VxlanNetworkId,        3  )   },
            {   BCMI_FT_XLATE_PARAM(NextHeader,            1  )   },
            {   BCMI_FT_XLATE_PARAM(HopLimit,              1  )   },
            {   BCMI_FT_XLATE_PARAM(InnerSrcIPv4,          4  )   },
            {   BCMI_FT_XLATE_PARAM(InnerDstIPv4,          4  )   },
            {   BCMI_FT_XLATE_PARAM(InnerL4SrcPort,        2  )   },
            {   BCMI_FT_XLATE_PARAM(InnerL4DstPort,        2  )   },
            {   BCMI_FT_XLATE_PARAM(InnerSrcIPv6,          16 )   },
            {   BCMI_FT_XLATE_PARAM(InnerDstIPv6,          16 )   },
            {   BCMI_FT_XLATE_PARAM(InnerIPProtocol,       1  )   },
            {   BCMI_FT_XLATE_PARAM(InnerTTL,              1  )   },
            {   BCMI_FT_XLATE_PARAM(InnerIPLength,         2  )   },
            {   BCMI_FT_XLATE_PARAM(InnerNextHeader,       1  )   },
            {   BCMI_FT_XLATE_PARAM(InnerHopLimit,         1  )   },
            {   BCMI_FT_XLATE_PARAM(InnerIP6Length,        2  )   },
            {   BCMI_FT_XLATE_PARAM(TcpFlags,              1  )   },
            {   BCMI_FT_XLATE_PARAM(OuterVlanTag,          2  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampNewLearn,     6  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampFlowStart,    6  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampFlowEnd,      6  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampCheckEvent1, 6  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampCheckEvent2, 6  )   },
            {   BCMI_FT_XLATE_PARAM(InnerDosAttack,        4  )   },
            {   BCMI_FT_XLATE_PARAM(TunnelClass,           3  )   },
            {   BCMI_FT_XLATE_PARAM(FlowtrackerCheck,      4  )   },
            {   BCMI_FT_XLATE_PARAM(ChipDelay,      4  )   },
            {   BCMI_FT_XLATE_PARAM(E2EDelay,      4  )   },
            {   BCMI_FT_XLATE_PARAM(IPATDelay,      4  )   },
            {   BCMI_FT_XLATE_PARAM(IPDTDelay,      4  )   },
            {   BCMI_FT_XLATE_PARAM(IngPortPGMinCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPortPGSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPortPoolMinCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPortPoolSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPoolSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPortPGHeadroomCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPortPoolHeadroomCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngGlobalHeadroomCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrPoolSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrQueueMinCongestionLevel,     1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrQueueSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrQueueGroupMinCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrQueueGroupSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrPortPoolSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrRQEPoolSharedCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(CFAPCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(MMUCongestionLevel,      1  )   },
            {   BCMI_FT_XLATE_PARAM(MMUQueueId,      2  )   },
            {   BCMI_FT_XLATE_PARAM(ECMPGroupIdLevel1,      1  )   },
            {   BCMI_FT_XLATE_PARAM(ECMPGroupIdLevel2,      1  )   },
            {   BCMI_FT_XLATE_PARAM(TrunkMemberId,      1  )   },
            {   BCMI_FT_XLATE_PARAM(TrunkGroupId,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPort,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngDropReasonGroupIdVector,      2  )   },
            {   BCMI_FT_XLATE_PARAM(NextHopB,      2  )   },
            {   BCMI_FT_XLATE_PARAM(NextHopA,      2  )   },
            {   BCMI_FT_XLATE_PARAM(IPPayloadLength,      2  )   },
            {   BCMI_FT_XLATE_PARAM(QosAttr,      2  )   },
            {   BCMI_FT_XLATE_PARAM(EgrPort,      1  )   },
            {   BCMI_FT_XLATE_PARAM(MMUCos,      2  )   },
            {   BCMI_FT_XLATE_PARAM(EgrDropReasonGroupIdVector,      2  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampCheckEvent3,      6  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampCheckEvent4,      6  )   },
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeExportReasons, 2, "ExportReasons"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeExportFlags, 1, "ExportFlags"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeFlowtrackerGroup, 1, "FlowtrackerGroup"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeReserved, 1, "Reserved"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeFlowIdentifier,      4, "FlowIdentifier"},
            {   BCMI_FT_XLATE_PARAM(MMUQueueGroupId,      1  )   },
            {   BCMI_FT_XLATE_PARAM(MMUPacketTos,      1  )   },
            {   BCMI_FT_XLATE_PARAM(EgrPacketTos,      1  )   },
            {   BCMI_FT_XLATE_PARAM(IngPacketTos,      1  )   },
            {   BCMI_FT_XLATE_PARAM(L2ForwardingField, 2  )   },
            {   BCMI_FT_XLATE_PARAM(InnerTcpFlags,     1  )   },
            {   BCMI_FT_XLATE_PARAM(GbpSrcId,          2  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampIngress,  6  )   },
            {   BCMI_FT_XLATE_PARAM(TimestampEgress,   6  )   },
            {   BCMI_FT_XLATE_PARAM(TcpSeqNumber,      4  )   },
            {   BCMI_FT_XLATE_PARAM(Custom,            0  )   }
};

/* FT Export internal routines. */
STATIC int
bcmi_ft_export_cb_init(int unit)
{
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);

    if (ft_export_cb_ctrl == NULL) {
        ft_export_cb_ctrl = sal_alloc(sizeof(bcmi_ft_export_cb_ctrl_t),
                                      "IPFIX Control Data");
        if (ft_export_cb_ctrl == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(ft_export_cb_ctrl, 0, sizeof(bcmi_ft_export_cb_ctrl_t));

        BCMI_FT_EXPORT_INFO(unit)->export_cb_ctrl = ft_export_cb_ctrl;

        ft_export_cb_ctrl->pid = SAL_THREAD_ERROR;
        ft_export_cb_ctrl->dma_chan = SOC_MEM_FIFO_DMA_CHANNEL_2;

    }

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_export_cb_deinit(int unit)
{
    soc_timeout_t timeout;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
    int i;

    if (NULL == ft_export_cb_ctrl) {
        return (BCM_E_NONE);
    }

    bcma_ft_export_fifo_control(unit, -1, FALSE);

    soc_timeout_init(&timeout, SAL_BOOT_BCMSIM ? 30000000 : 10000000, 0);

    while (ft_export_cb_ctrl->pid != SAL_THREAD_ERROR) {
        if (soc_timeout_check(&timeout) != 0) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "IPFIX thread did not exit.\n")));

            return BCM_E_INTERNAL;
        }
    }

    for (i = 0; i < BCMI_FT_CB_MAX; i++) {
        ft_export_cb_ctrl->callback_entry[i].state =
            BCMI_FT_CALLBACK_STATE_INVALID;
    }


    sal_free(ft_export_cb_ctrl);
    ft_export_cb_ctrl = NULL;

    return BCM_E_NONE;
}

#define BCMI_FTE_IPFIX_HDR_VERSION      0xa
#define BCMI_FTE_IPFIX_HDR_LENGTH       0x10
#define BCMI_FTE_IPFIX_HDR_TIMESTAMP    0x0
#define BCMI_FTE_IPFIX_HDR_SEQNUM       0x0
#define BCMI_FTE_IPFIX_HDR_OBS_DOMAIN   0x1

/* IpV4 based IPFIX encap header */
#define BCMI_FTE_IPV4_HDR_CTRL_IP_CHKSUM    0
#define BCMI_FTE_IPV4_HDR_CTRL_IP_LENGTH    1
#define BCMI_FTE_UDP_HDR_CTRL_LENGTH        2
#define BCMI_FTE_UDP_HDR_CTRL_CHKSUM        3
#define BCMI_FTE_IPFIX_HDR_CTRL_LENGTH      4
#define BCMI_FTE_IPFIX_HDR_CTRL_TIMESTAMP   5
#define BCMI_FTE_IPFIX_HDR_CTRL_SEQ_NUM     6
#define BCMI_FTE_IPV6_HDR_CTRL_IP_LENGTH    7

typedef struct bcmi_fte_flex_hdr_upd_info_s {
    uint32 double_for_chksum;
    uint32 edit_type;
    uint32 num_bytes;
    uint32 offset_type;
    uint32 offset;
    uint32 payload_chksum;
    uint32 chksum_type;
} bcmi_fte_flex_hdr_upd_info_t;

/* HDR UPD CTRL EDIT TYPE hardware values. DO NOT MODIFY */
#define BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_LENGTH      0
#define BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_CHKSUM      1
#define BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_TIMESTAMP   2
#define BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_SEQ_NUM     3

/* Offset Type */
#define BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IP        1
#define BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_UDP       2
#define BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IPFIX     3

/* CheckSum Type */
#define BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_NONE      0
#define BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR    1
#define BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_SET       2

static bcmi_fte_flex_hdr_upd_info_t flex_hdr_upd_info_array[] = {
    /* BCMI_FTE_IPV4_HDR_CTRL_IP_CHKSUM */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_CHKSUM, 0x2,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IP, 10,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_SET},

    /* BCMI_FTE_IPV4_HDR_CTRL_IP_LENGTH */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_LENGTH, 0x2,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IP, 2,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR},

    /* BCMI_FTE_UDP_HDR_CTRL_LENGTH */
    {1, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_LENGTH, 0x2,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_UDP, 4,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR},

    /* BCMI_FTE_UDP_HDR_CTRL_CHKSUM */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_CHKSUM, 0x2,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_UDP, 6,
        1, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_SET},

    /* BCMI_FTE_IPFIX_HDR_CTRL_LENGTH */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_LENGTH, 0x2,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IPFIX, 2,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR},

    /* BCMI_FTE_IPFIX_HDR_CTRL_TIMESTAMP */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_TIMESTAMP, 0x4,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IPFIX, 4,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR},

    /* BCMI_FTE_IPFIX_HDR_CTRL_SEQ_NUM */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_SEQ_NUM, 0x4,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IPFIX, 8,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR},

    /* BCMI_FTE_IPV6_HDR_CTRL_IP_LENGTH */
    {0, BCMI_FTE_HDR_UDP_CTRL_EDIT_TYPE_LENGTH, 0x2,
        BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IP, 4,
        0, BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_NONE}
};

STATIC int
bcmi_ft_ipfix_header_get(int unit,
                         bcmi_ft_collector_info_t *ft_coll_info,
                         shr_ipfix_header_t *ipfix)
{
    /* Set IPFIX */
    sal_memset(ipfix, 0, sizeof(*ipfix));

    ipfix->version = ft_coll_info->ipfix_hdr.version;
    ipfix->length = BCMI_FTE_IPFIX_HDR_LENGTH;
    ipfix->export_time = BCMI_FTE_IPFIX_HDR_TIMESTAMP;
    ipfix->sequence_num = BCMI_FTE_IPFIX_HDR_SEQNUM;
    ipfix->observation_id = BCMI_FT_EXPORT_INFO(unit)->ipfix_observation_domain_id;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_udp_header_get(int unit,
                       bcm_flowtracker_collector_info_t *collector_info,
                       shr_udp_header_t *udp)
{
    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = collector_info->udp.src_port;
    udp->dst = collector_info->udp.dst_port;

    /* To be calculated later.  */
    udp->sum = 0;
    udp->length = 0;

    return BCM_E_NONE;
}


STATIC int
bcmi_ft_ipv4_header_get(int unit,
                        bcm_flowtracker_collector_info_t *collector_info,
                        uint16 *etype,
                        shr_ipv4_header_t *ipv4)
{

    *etype = SHR_L2_ETYPE_IPV4;

    sal_memset(ipv4, 0, sizeof(*ipv4));

    ipv4->version  = SHR_IPV4_VERSION;
    ipv4->h_length = SHR_IPV4_HEADER_LENGTH_WORDS;
    ipv4->dscp     = collector_info->ipv4.dscp;
    ipv4->ecn      = 0;

    /* Length populated by EDIT_CTRL function.
     */
    ipv4->length   = 0;
    ipv4->id       = 0;
    ipv4->flags    = 0;
    ipv4->f_offset = 0;
    ipv4->ttl      = collector_info->ipv4.ttl;
    ipv4->protocol = SHR_IP_PROTOCOL_UDP;
    ipv4->sum      = 0;
    ipv4->src      = collector_info->ipv4.src_ip;
    ipv4->dst      = collector_info->ipv4.dst_ip;

    /* Calculate IP header checksum. */
    ipv4->sum = 0;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_ipv6_header_get(int unit,
                        bcm_flowtracker_collector_info_t *collector_info,
                        uint16 *etype,
                        shr_ipv6_header_t *ipv6)
{
    *etype = SHR_L2_ETYPE_IPV6;

    sal_memset(ipv6, 0, sizeof(*ipv6));

    ipv6->version = SHR_IPV6_VERSION;
    ipv6->t_class = collector_info->ipv6.traffic_class;
    ipv6->f_label = collector_info->ipv6.flow_label;
    ipv6->next_header = SHR_IP_PROTOCOL_UDP;
    ipv6->hop_limit = collector_info->ipv6.hop_limit;
    sal_memcpy(ipv6->src, collector_info->ipv6.src_ip, sizeof(bcm_ip6_t));
    sal_memcpy(ipv6->dst, collector_info->ipv6.dst_ip, sizeof(bcm_ip6_t));

    /* To be calculated by EDIT_CTRL function */
    ipv6->p_length = 0;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_l2_header_get(int unit,
                      bcm_flowtracker_collector_info_t *collector_info,
                      uint16 etype, shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, collector_info->eth.dst_mac, SHR_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, collector_info->eth.src_mac, SHR_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag(s)
     */
    if (collector_info->eth.vlan_tag_structure ==
        BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_UNTAGGED) {

        /* Set to 0 to indicate untagged */
        l2->outer_vlan_tag.tpid = 0;
        l2->inner_vlan_tag.tpid = 0;
    } else {

        /* Fill outer vlan */
        if ((collector_info->eth.vlan_tag_structure ==
             BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED) ||
            (collector_info->eth.vlan_tag_structure ==
             BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_DOUBLE_TAGGED))
        {
            l2->outer_vlan_tag.tpid     =
                collector_info->eth.outer_tpid;
            l2->outer_vlan_tag.tci.prio =
                BCM_VLAN_CTRL_PRIO(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.cfi  =
                BCM_VLAN_CTRL_CFI(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.vid  =
                BCM_VLAN_CTRL_ID(collector_info->eth.outer_vlan_tag);
        } else {
            /* Set to 0 to indicate not inner tagged */
            l2->inner_vlan_tag.tpid = 0;
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_collector_packet_build(int unit,
                               bcmi_ft_collector_info_t *ft_coll_info,
                               bcmi_ft_pkt_build_t *pkt_build)
{
    int rv;
    uint8 *cur_ptr;
    uint8 *buffer;

    bcm_ethertype_t   etype;
    shr_ipfix_header_t ipfix;
    shr_udp_header_t  udp;
    shr_ipv4_header_t ipv4;
    shr_ipv6_header_t ipv6;
    shr_l2_header_t   l2;
    uint16            flex_offset;

    bcm_flowtracker_collector_info_t *collector_info;

    collector_info = &(ft_coll_info->collector_info);

    etype = 0;
    sal_memset(&ipfix, 0, sizeof(ipfix));
    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));
    sal_memset(&l2, 0, sizeof(l2));

    switch(collector_info->transport_type) {
        case bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp:

            /* Get IPFIX Header fields */
            rv = bcmi_ft_ipfix_header_get(unit, ft_coll_info, &ipfix);
            BCM_IF_ERROR_RETURN(rv);

            /* Get UDP Header Fields */
            rv = bcmi_ft_udp_header_get(unit, collector_info, &udp);
            BCM_IF_ERROR_RETURN(rv);

            /* Get IPv4 Header Fields */
            rv = bcmi_ft_ipv4_header_get(unit, collector_info, &etype, &ipv4);
            BCM_IF_ERROR_RETURN(rv);

            /* Get L2 Header Fields */
            rv = bcmi_ft_l2_header_get(unit, collector_info, etype, &l2);
            break;

        case bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp:

            /* Get IPFIX Header fields */
            rv = bcmi_ft_ipfix_header_get(unit, ft_coll_info, &ipfix);
            BCM_IF_ERROR_RETURN(rv);

            /* Get UDP Header Fields */
            rv = bcmi_ft_udp_header_get(unit, collector_info, &udp);
            BCM_IF_ERROR_RETURN(rv);

            /* Get IPv6 Header Fields */
            rv = bcmi_ft_ipv6_header_get(unit, collector_info, &etype, &ipv6);
            BCM_IF_ERROR_RETURN(rv);

            /* Get L2 Header Fields */
            rv = bcmi_ft_l2_header_get(unit, collector_info, etype, &l2);
            break;

        default:
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(rv);

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    buffer = pkt_build->encap;
    cur_ptr = buffer;

    switch(collector_info->transport_type) {
        case bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp:

            cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

            /* Set Flowtracker export pkt IP base offset */
            pkt_build->ip_offset = cur_ptr - buffer;
            cur_ptr = shr_ipv4_header_pack(cur_ptr, &ipv4);

            /* Set Flowtracker export pkt UDP base offset */
            pkt_build->udp_offset = cur_ptr - buffer;
            cur_ptr = shr_udp_header_pack(cur_ptr, &udp);

            /* Set Flowtracker export pkt IPFIX base offset */
            pkt_build->ipfix_offset = cur_ptr - buffer;
            cur_ptr = shr_ipfix_header_pack(cur_ptr, &ipfix);

            /* Set Flowtracker export pkt encapsulation length */
            pkt_build->encap_length = cur_ptr - buffer;

            /* Set Flowtracker Export pkt UDP Length */
            udp.length = pkt_build->encap_length - pkt_build->udp_offset;

            /* pack udp length */
            flex_offset = pkt_build->udp_offset +
                flex_hdr_upd_info_array[BCMI_FTE_UDP_HDR_CTRL_LENGTH].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, udp.length);

            /* Set Flowtracker export pkt UDP Base checksum */
            cur_ptr = buffer + pkt_build->ipfix_offset;
            pkt_build->udp_base_checksum =
                shr_udp_initial_checksum_get(0, &ipv4, NULL, cur_ptr, &udp);

            /* Initial Udp checksum calculation */
            udp.sum = (pkt_build->udp_base_checksum >> 16) & 0xffff;
            udp.sum += (pkt_build->udp_base_checksum & 0xffff);
            udp.sum = (~udp.sum);

            /* pack UDP checksum */
            flex_offset = pkt_build->udp_offset +
                flex_hdr_upd_info_array[BCMI_FTE_UDP_HDR_CTRL_CHKSUM].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, udp.sum);

            /* Set Flowtracker export pkt IP Length */
            ipv4.length = pkt_build->encap_length - pkt_build->ip_offset;

            /* pack ip length */
            flex_offset = pkt_build->ip_offset +
                flex_hdr_upd_info_array[BCMI_FTE_IPV4_HDR_CTRL_IP_LENGTH].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, ipv4.length);

            /* Set Flowtracker export pkt IP Base checksum */
            cur_ptr = buffer + pkt_build->ip_offset;
            pkt_build->ip_base_checksum =
                shr_initial_chksum_get(SHR_IPV4_HEADER_LENGTH, cur_ptr);

            /* Initial Ip Header checksum calculation */
            ipv4.sum = (pkt_build->ip_base_checksum >> 16) & 0xffff;
            ipv4.sum += (pkt_build->ip_base_checksum & 0xffff);
            ipv4.sum = (~ipv4.sum);

            /* pack checksum */
            flex_offset = pkt_build->ip_offset +
                flex_hdr_upd_info_array[BCMI_FTE_IPV4_HDR_CTRL_IP_CHKSUM].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, ipv4.sum);
            break;

        case bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp:

            cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

            /* Set Flowtracker export pkt IP base offset */
            pkt_build->ip_offset = cur_ptr - buffer;
            cur_ptr = shr_ipv6_header_pack(cur_ptr, &ipv6);

            /* Set Flowtracker export pkt UDP base offset */
            pkt_build->udp_offset = cur_ptr - buffer;
            cur_ptr = shr_udp_header_pack(cur_ptr, &udp);

            /* Set Flowtracker export pkt IPFIX base offset */
            pkt_build->ipfix_offset = cur_ptr - buffer;
            cur_ptr = shr_ipfix_header_pack(cur_ptr, &ipfix);

            /* Set Flowtracker export pkt encapsulation length */
            pkt_build->encap_length = cur_ptr - buffer;

            /* Set Flowtracker Export pkt UDP Length */
            udp.length = pkt_build->encap_length - pkt_build->udp_offset;

            /* pack udp length */
            flex_offset = pkt_build->udp_offset +
                flex_hdr_upd_info_array[BCMI_FTE_UDP_HDR_CTRL_LENGTH].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, udp.length);

            /* Set Flowtracker export pkt UDP Base checksum */
            cur_ptr = buffer + pkt_build->ipfix_offset;
            pkt_build->udp_base_checksum =
                shr_udp_initial_checksum_get(1, NULL, &ipv6, cur_ptr, &udp);

            /* Initial UDP Checksum calculation */
            udp.sum = (pkt_build->udp_base_checksum >> 16) & 0xffff;
            udp.sum += (pkt_build->udp_base_checksum & 0xffff);
            udp.sum = (~udp.sum);

            /* pack UDP checksum */
            flex_offset = pkt_build->udp_offset +
                flex_hdr_upd_info_array[BCMI_FTE_UDP_HDR_CTRL_CHKSUM].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, udp.sum);

            /* Set Flowtracker export pkt IP payload Length */
            ipv6.p_length = pkt_build->encap_length - pkt_build->udp_offset;

            /* pack ip length */
            flex_offset = pkt_build->ip_offset +
                flex_hdr_upd_info_array[BCMI_FTE_IPV6_HDR_CTRL_IP_LENGTH].offset;
            cur_ptr = buffer + flex_offset;
            SHR_PKT_PACK_U16(cur_ptr, ipv6.p_length);
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_collector_flex_hdr_upd_ctrl_set(int unit,
                                        soc_mem_t mem,
                                        uint32 ctrl,
                                        int ctrl_bin,
                                        bcmi_ft_pkt_build_t *pkt_build,
                                        bsc_ex_hdr_construct_cfg_entry_t *hdr,
                                        uint32 *chksum_bin_map)
{
    int offset;
    uint32 hdr_upd_ctrl;
    soc_field_t hdr_upd_fld;
    soc_format_t collector_fmt;
    bcmi_fte_flex_hdr_upd_info_t *flex_hdr_upd_info = NULL;

    soc_field_t fte_flx_hdr_upd_flds[] = {
        HDR_UPDATE_CTRL_0f, HDR_UPDATE_CTRL_1f,
        HDR_UPDATE_CTRL_2f, HDR_UPDATE_CTRL_3f,
        HDR_UPDATE_CTRL_4f, HDR_UPDATE_CTRL_5f,
        HDR_UPDATE_CTRL_6f, HDR_UPDATE_CTRL_7f
    };

    collector_fmt = HDR_UPDATE_CTRLfmt;

    offset = 0;
    hdr_upd_ctrl = 0x0;
    hdr_upd_fld = fte_flx_hdr_upd_flds[ctrl_bin];
    flex_hdr_upd_info = &flex_hdr_upd_info_array[ctrl];

    if (flex_hdr_upd_info->offset_type ==
            BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IP) {
        offset = pkt_build->ip_offset;
    } else if (flex_hdr_upd_info->offset_type ==
            BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_UDP) {
        offset = pkt_build->udp_offset;
    } else if (flex_hdr_upd_info->offset_type ==
            BCMI_FTE_HDR_UPD_CTRL_OFFSET_TYPE_IPFIX) {
        offset = pkt_build->ipfix_offset;
    }
    offset += flex_hdr_upd_info->offset;

    soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
            FLEX_HDR_EDIT_TYPEf, flex_hdr_upd_info->edit_type);
    soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
            FLEX_HDR_EDIT_OFFSETf, offset);
    soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
            FLEX_HDR_EDIT_NUM_BYTESf, flex_hdr_upd_info->num_bytes);
    soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
            FLEX_HDR_EDIT_VALIDf, 1);

    if (flex_hdr_upd_info->double_for_chksum == 1) {
        soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
                DOUBLE_FOR_CHKSUMf, 0x1);
    }
    if (flex_hdr_upd_info->payload_chksum == 1) {
        soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
                PAYLOAD_CHKSUM_ENf, 0x1);
    }
    if (flex_hdr_upd_info->chksum_type ==
            BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_BIT_OR) {
        (*chksum_bin_map) |= (1 << ctrl_bin);
    } else if (flex_hdr_upd_info->chksum_type ==
            BCMI_FTE_HDR_UPD_CTRL_CHKSUM_TYPE_SET) {
        soc_format_field32_set(unit, collector_fmt, &hdr_upd_ctrl,
                CHKSUM_EN_BITMAPf, *chksum_bin_map);
        *chksum_bin_map = 0;
    }

    soc_mem_field32_set(unit, mem, hdr, hdr_upd_fld, hdr_upd_ctrl);

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_collector_hdr_construct(int unit,
                                bcm_flowtracker_collector_t coll_hw_id,
                                bcm_flowtracker_collector_info_t *collector_info,
                                bcmi_ft_pkt_build_t *pkt_build)
{
    int rv;
    int index = 0;
    int ctrl_count = 0;
    uint32 chksum_bin_map;
    uint32 ctrl_array[8];
    uint32 ctrl_bin_array[8];
    soc_mem_t mem;
    bsc_ex_hdr_construct_cfg_entry_t hdr;

    mem = BSC_EX_HDR_CONSTRUCT_CFGm;
    sal_memset(&hdr, 0, sizeof(bsc_ex_hdr_construct_cfg_entry_t));
    chksum_bin_map = 0;

    switch(collector_info->transport_type) {
        case bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp:

            /* Do Not Change Order */
            ctrl_array[0] = BCMI_FTE_IPFIX_HDR_CTRL_LENGTH;
            ctrl_bin_array[0] = 1;
            ctrl_array[1] = BCMI_FTE_IPFIX_HDR_CTRL_TIMESTAMP;
            ctrl_bin_array[1] = 2;
            ctrl_array[2] = BCMI_FTE_IPFIX_HDR_CTRL_SEQ_NUM;
            ctrl_bin_array[2] = 3;
            ctrl_array[3] = BCMI_FTE_UDP_HDR_CTRL_LENGTH;
            ctrl_bin_array[3] = 4;
            ctrl_array[4] = BCMI_FTE_UDP_HDR_CTRL_CHKSUM;
            ctrl_bin_array[4] = 5;
            ctrl_array[5] = BCMI_FTE_IPV4_HDR_CTRL_IP_LENGTH;
            ctrl_bin_array[5] = 6;
            ctrl_array[6] = BCMI_FTE_IPV4_HDR_CTRL_IP_CHKSUM;
            ctrl_bin_array[6] = 0;
            ctrl_count = 7;

            for (index = 0; index < ctrl_count; index++) {
                rv = bcmi_ft_collector_flex_hdr_upd_ctrl_set(unit,
                        mem, ctrl_array[index], ctrl_bin_array[index],
                        pkt_build, &hdr, &chksum_bin_map);
                BCM_IF_ERROR_RETURN(rv);
            }
            break;

        case bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp:

            /* Do Not Change Order */
            ctrl_array[0] = BCMI_FTE_IPFIX_HDR_CTRL_LENGTH;
            ctrl_bin_array[0] = 0;
            ctrl_array[1] = BCMI_FTE_IPFIX_HDR_CTRL_TIMESTAMP;
            ctrl_bin_array[1] = 1;
            ctrl_array[2] = BCMI_FTE_IPFIX_HDR_CTRL_SEQ_NUM;
            ctrl_bin_array[2] = 2;
            ctrl_array[3] = BCMI_FTE_UDP_HDR_CTRL_LENGTH;
            ctrl_bin_array[3] = 3;
            ctrl_array[4] = BCMI_FTE_UDP_HDR_CTRL_CHKSUM;
            ctrl_bin_array[4] = 4;
            ctrl_array[5] = BCMI_FTE_IPV6_HDR_CTRL_IP_LENGTH;
            ctrl_bin_array[5] = 5;
            ctrl_count = 6;

            for (index = 0; index < ctrl_count; index++) {
                rv = bcmi_ft_collector_flex_hdr_upd_ctrl_set(unit,
                        mem, ctrl_array[index], ctrl_bin_array[index],
                        pkt_build, &hdr, &chksum_bin_map);
                BCM_IF_ERROR_RETURN(rv);
            }
            break;

        default:
            return BCM_E_PARAM;
    }

    /* Encap length. */
    soc_mem_field32_set(unit, mem, &hdr, HDR_BYTE_LENGTHf,
            pkt_build->encap_length);

    soc_mem_write(unit, mem, COPYNO_ALL, coll_hw_id, &hdr);

    return BCM_E_NONE;
}

int
bcmi_ft_num_records_enc_get(int unit, int val, uint32 *num_rec_enc)
{
    int idx = 0;

    uint32 record_enc_array[] = { 0, 1, 4, 8, 12, 16, 20, 24,
                                 28, 32, 40, 48, 56, 64, 96, 128};

    if (num_rec_enc == NULL) {
        return BCM_E_PARAM;
    }

    /* Round off to lower nearest encoding */
    for (idx = 0; idx < (COUNTOF(record_enc_array) - 1); idx++) {
        if ((val >= record_enc_array[idx]) &&
            (val < record_enc_array[idx + 1])) {
            break;
        }
    }
    *num_rec_enc = idx;

    return BCM_E_NONE;
}


int
bcmi_ft_export_time_interval_enc_get(int unit, uint32 export_interval,
        uint32 *time_interval_enc)
{
    int idx = 0;

    uint32 interval_array_msec[] = {
        /* 1 millisecond       */       1,
        /* 5 milliseconds      */       5,
        /* 100 milliseconds    */     100,
        /* 500 milliseconds    */     500,
        /* 1 second            */    1000,
        /* 10 seconds          */   10000,
        /* 1 minute            */   60000,
        /* 10 minutes          */  600000
    };

    if (time_interval_enc == NULL) {
        return BCM_E_PARAM;
    }

    /* Rounding off to lower nearest encoding */
    for (idx = 1; idx < COUNTOF(interval_array_msec); idx++) {

        if (export_interval < (interval_array_msec[idx] * 1000)) {
            break;
        }
    }
    *time_interval_enc = idx - 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_collector_fifo_entry_alloc
 * Purpose:
 *   Allocate min poiner to the collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   req_entries - (IN) Number of fifo entries required for the collector.
 *   min_pointer   - (OUT) Min pointer for the collector.
 * Returns:
 *   BCM_E_XXX.
 * Notes:
 *   User can dynamically allocate export FIFO entries per collector. We
 *   maintain a list of fifo entries for free and in use stages. -ve sign
 *   represent that the index is in used by collector and number represent
 *   number of entries allocated to the block including that index. Similarly,
 *   +ve sign represents free block.
 *   To allocate range of entries to collector, we search for the hole
 *   which is best fit for the required number of entries.
 */
STATIC int
bcmi_ft_collector_fifo_entry_alloc(int unit,
                                   int req_entries,
                                   int *min_pointer)
{
    int idx = 0, base_idx = 0;
    int num_fifo_entries = 0;
    int free_entries = 0;
    int diff = 0, min_diff = 0;
    int *fifo_coll_map = NULL;

    num_fifo_entries = BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries;
    fifo_coll_map = BCMI_FT_EXPORT_INFO(unit)->fifo_collector_map;

    idx = 0; min_diff = num_fifo_entries + 1;
    while ((idx + req_entries) <= num_fifo_entries) {

        /* Unexpected case */
        if (fifo_coll_map[idx] == 0) {
            return BCM_E_INTERNAL;
        }
        /* block used by Collector */
        if (fifo_coll_map[idx] < 0) {
            idx += (-fifo_coll_map[idx]);
            continue;
        }

        /* Get difference */
        diff = (fifo_coll_map[idx] - req_entries);

        /* block has less free entries */
        if (diff < 0) {
            idx += fifo_coll_map[idx];
            continue;
        }

        /* Exact match found */
        if (diff == 0) {
            base_idx = idx;
            free_entries = fifo_coll_map[idx];
            break;
        }

        /* Find best match */
        if (diff < min_diff) {
            base_idx = idx;
            free_entries = fifo_coll_map[idx];
            min_diff = diff;
        }

        /* Jump to next block */
        idx += fifo_coll_map[idx];
    }

    /* Use this block to allocate */
    if (free_entries != 0) {
        /* If extra free entries in the block */
        if ((free_entries != req_entries) &&
                ((base_idx + req_entries) < num_fifo_entries)) {
            fifo_coll_map[base_idx + req_entries] =
                fifo_coll_map[base_idx] - req_entries;
        }
        /* Mark the block in use */
        fifo_coll_map[base_idx] = -req_entries;
    } else {
        return BCM_E_RESOURCE;
    }

    *min_pointer = base_idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_collector_fifo_entry_free
 * Purpose:
 *   Free block of fifo entries allocated to the collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   min_pointer   - (OUT) Min pointer for the collector.
 * Returns:
 *   BCM_E_XXX.
 * Notes:
 *   While freeing a range of entries currently in use by the collector, we
 *   check block before and after the targeted range. If any of the range is
 *   free, we merge it back to form a hole including all consequtive free
 *   blocks.
 */
STATIC int
bcmi_ft_collector_fifo_entry_free(int unit,
                                  int min_pointer)
{
    int idx = 0, base_idx = -1;
    int jump_blocks = 0, block_size = 0;
    int prev_block_idx = -1, next_block_idx = -1;
    int num_fifo_entries = 0;
    int *fifo_coll_map = NULL;

    num_fifo_entries = BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries;
    fifo_coll_map = BCMI_FT_EXPORT_INFO(unit)->fifo_collector_map;

    idx = 0; base_idx = -1;
    while (idx < num_fifo_entries) {
        /* Found the block */
        if (idx == min_pointer) {
            break;
        }

        jump_blocks = 0;
        if (fifo_coll_map[idx] < 0) {
            /* Block used by collector */
            jump_blocks = -fifo_coll_map[idx];
            prev_block_idx = -1;
        } else {
            /* Block has free entries */
            jump_blocks = fifo_coll_map[idx];
            prev_block_idx = idx;
        }

        idx += jump_blocks;
    }

    /* Free if block found */
    if (idx < num_fifo_entries) {

        /* Calculate Block size */
        block_size = -fifo_coll_map[idx];
        fifo_coll_map[idx] = 0;
        next_block_idx = idx + block_size;

        /* Merge if next block is free */
        if (next_block_idx < num_fifo_entries) {
            if (fifo_coll_map[next_block_idx] > 0) {
                block_size += fifo_coll_map[next_block_idx];
                fifo_coll_map[next_block_idx] = 0;
            }
        }

        /* Merge if previous block is free */
        if (prev_block_idx != -1) {
            block_size += fifo_coll_map[prev_block_idx];
            base_idx = prev_block_idx;
        } else {
            base_idx = idx;
        }

        fifo_coll_map[base_idx] = block_size;
    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* Routine to install FT collector hardware. */
STATIC int
bcmi_ft_collector_hw_install(int unit,
                             bcm_flowtracker_collector_t coll_hw_id,
                             int coll_enable)
{
    int ix, rv;
    int m_idx, m_bytes, m_entries;
    int init_seq;
    int num_records;
    uint32 num_records_enc;
    uint32 time_interval_enc;
    int flush_timeout_count = BCMI_FT_COLLECTOR_FLUSH_TIMEOUT_RETRIES;

    uint8 *buffer;
    uint32 rval = 0;
    uint32 dma_en, dma_coll_id;
    uint32 m_entry_val[2];
    bcmi_ft_pkt_build_t pkt_build;

    soc_mem_t collector_mem;
    soc_reg_t collector_reg;

    bsc_ex_collector_ipfix_packet_build_entry_t m_entry;
    bsc_ex_collector_config_entry_t config;

    bcmi_ft_collector_info_t *ft_coll_info;
    bcm_flowtracker_collector_info_t *collector_info;
    int is_sim = 0;

    is_sim = (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) ? 1 : 0;

    /* Init pkt_build. */
    sal_memset(&pkt_build, 0, sizeof(bcmi_ft_pkt_build_t));

    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, coll_hw_id);
    collector_info = &(ft_coll_info->collector_info);

    if (coll_enable == 0) {
        /* Flush Collector before enabling. */
        collector_mem = BSC_EX_COLLECTOR_CONFIGm;
        sal_memset(&config, 0, sizeof(config));

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, collector_mem,
                    MEM_BLOCK_ANY, coll_hw_id, &config));

        soc_mem_field32_set(unit, collector_mem,
                &config, ENABLEf, 0x0);
        soc_mem_field32_set(unit, collector_mem,
                &config, FLUSHf, 0x1);
        soc_mem_field32_set(unit, collector_mem,
                &config, TIME_INTERVAL_ENABLEf, 0x0);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
                (ft_coll_info->ft_type == BCMI_FT_COLLECTOR_FT_TYPE_MICRO)) {
            soc_mem_field32_set(unit, collector_mem,
                    &config, RA_FLUSHf, 0x1);
        }
#endif /*BCM_FLOWTRACKER_V2_SUPPORT */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
                (ft_coll_info->ft_type == BCMI_FT_COLLECTOR_FT_TYPE_AGGREGATE)) {
            soc_mem_field32_set(unit, collector_mem,
                    &config, AGG_FLUSHf, 0x1);
        }
#endif /*BCM_FLOWTRACKER_V3_SUPPORT */

        soc_mem_write(unit, collector_mem, COPYNO_ALL, coll_hw_id, &config);

        /*
         * Wait for collector flush to complete
         * Count value is 250 to make 250*400 = 100ms delay but
         * hardware gurantees to come out in a max time of 16 msecs.
         */
        while ((flush_timeout_count > 0) && (is_sim != 1)) {
            flush_timeout_count--;
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, collector_mem,
                              COPYNO_ALL, coll_hw_id, &config));

            if ((soc_mem_field32_get(unit, collector_mem, &config, FLUSHf)) == 0) {
                LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "The"
                                " collector Flush is done in %d iterations.\n"),
                            BCMI_FT_COLLECTOR_FLUSH_TIMEOUT_RETRIES - flush_timeout_count));
                break;
            } else if (flush_timeout_count == 0) {
                /* Last Iteration. */
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit, "The collector Flush is timed out.\n")));
                return BCM_E_INTERNAL;
            }
            sal_usleep(400);
        }
    }

    /* SDK-144839: Flusing collector is not resetting the running
     *  write and read pointers for the FIFO in BCMSIM.
     *  Attempting to force FIFO flush so that the pointers are synced.
     */
    if (SAL_BOOT_BCMSIM) {
        soc_reg_field32_modify(unit, BSC_EX_EXPORT_TRIGGERr, 0, EXPORT_ENABLEf, 1);
    }

    /* Disable DMA for Local collector */
    collector_reg = BSC_EX_EXPORT_CONTROLr;
    rv = soc_reg32_get(unit, collector_reg, REG_PORT_ANY, 0, &rval);
    BCM_IF_ERROR_RETURN(rv);

    dma_en = soc_reg_field_get(unit, collector_reg, rval, DMA_ENf);
    dma_coll_id = soc_reg_field_get(unit, collector_reg, rval, DMA_COLLECTOR_IDf);

    if ((dma_en == 1) && (coll_hw_id == dma_coll_id)) {
        soc_reg_field_set(unit, collector_reg, &rval, DMA_ENf, 0x0);
        soc_reg_field_set(unit, collector_reg, &rval, DMA_COLLECTOR_IDf, 0x0);

        rv = soc_reg32_set(unit, collector_reg, REG_PORT_ANY, 0, rval);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set IPFIX TOD Select register */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        rv = bcmi_ftv2_export_tod_set(unit, coll_hw_id, collector_info, coll_enable);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    /* For uninstalling, flush collector, disable and return  */
    if (coll_enable == 0) {

        sal_memset(&config, 0, sizeof(config));
        soc_mem_write(unit, collector_mem, COPYNO_ALL, coll_hw_id, &config);

        bcmi_ft_collector_fifo_entry_free(unit, ft_coll_info->min_pointer);
        return BCM_E_NONE;
    }

    m_entries = (soc_mem_index_count(unit, BSC_EX_COLLECTOR_IPFIX_PACKET_BUILDm) /
            soc_mem_index_count(unit, BSC_EX_COLLECTOR_CONFIGm));

    /* Populate PACKET_BUILD. */
    if (collector_info->collector_type == bcmFlowtrackerCollectorRemote) {
        collector_mem = BSC_EX_COLLECTOR_IPFIX_PACKET_BUILDm;

        sal_memset(&pkt_build, 0, sizeof(pkt_build));

        rv = bcmi_ft_collector_packet_build(unit, ft_coll_info, &pkt_build);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Num records threshold */
    num_records = ft_coll_info->num_records;
    if (num_records == -1) {
        num_records = ((collector_info->max_packet_length -
                    pkt_build.encap_length) / (BCMI_FT_IPFIX_RECORD_SIZE));
    }

    if (num_records > ft_coll_info->max_records_reserve) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "FT Export records threshold (%d) more than reserved"
                        " entries (%d)\n"), num_records,
                        ft_coll_info->max_records_reserve));
        return BCM_E_CONFIG;
    }

    rv = bcmi_ft_num_records_enc_get(unit, num_records, &num_records_enc);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate min/max pointers for collector */
    rv = bcmi_ft_collector_fifo_entry_alloc(unit,
            ft_coll_info->max_records_reserve, &ft_coll_info->min_pointer);
    BCM_IF_ERROR_RETURN(rv);

    /* Configure collector for IPFIX packet format */
    if (collector_info->collector_type == bcmFlowtrackerCollectorRemote) {

        buffer = &(pkt_build.encap[0]);
        m_bytes = soc_mem_entry_bytes(unit, collector_mem);
        m_idx = (coll_hw_id * m_entries);

        for (ix = 0; ix < (m_entries * m_bytes); ix += m_bytes) {
            /* 64 bit FIELDS field is to be written in Network byte order */
            SHR_PKT_UNPACK_U32(buffer, m_entry_val[1]);
            SHR_PKT_UNPACK_U32(buffer, m_entry_val[0]);

            soc_mem_field_set(unit, collector_mem, (uint32 *)&m_entry,
                              FIELDSf, m_entry_val);
            soc_mem_write(unit, collector_mem,
                          COPYNO_ALL, (m_idx + (ix / m_bytes)), &m_entry);
        }

        /* Configure HDR_CONSTRUCT fields. */
        rv = bcmi_ft_collector_hdr_construct(unit,
                    coll_hw_id, collector_info, &pkt_build);
    }
    if (BCM_FAILURE(rv)) {
        (void) bcmi_ft_collector_fifo_entry_free(unit, ft_coll_info->min_pointer);
        return rv;
    }

    init_seq = (ft_coll_info->ipfix_hdr).initial_sequence_num;

    /* Reset sequence number. */
    rval = 0;
    soc_reg_field_set(unit, BSC_EX_SEQ_NUMr, &rval, SEQ_NUMf, init_seq);
    rv = soc_reg32_set(unit, BSC_EX_SEQ_NUMr, REG_PORT_ANY, coll_hw_id, rval);
    if (BCM_FAILURE(rv)) {
        (void) bcmi_ft_collector_fifo_entry_free(unit, ft_coll_info->min_pointer);
        return rv;
    }

    /* Enable Collector. */
    collector_mem = BSC_EX_COLLECTOR_CONFIGm;

    sal_memset(&config, 0, sizeof(config));
    soc_mem_field32_set(unit, collector_mem,
                        &config, MAX_POINTERf, ft_coll_info->min_pointer +
                                        ft_coll_info->max_records_reserve - 1);
    soc_mem_field32_set(unit, collector_mem,
                        &config, MIN_POINTERf,  ft_coll_info->min_pointer);
    soc_mem_field32_set(unit, collector_mem,
                        &config, MAX_RECORDS_PER_PACKETf, num_records_enc);

    /* Export Time Interval */
    bcmi_ft_export_time_interval_enc_get(unit, ft_coll_info->export_interval,
                        &time_interval_enc);
    soc_mem_field32_set(unit, collector_mem,
                        &config, TIME_INTERVAL_ENABLEf, 0x1);
    soc_mem_field32_set(unit, collector_mem,
                        &config, TIME_INTERVALf, time_interval_enc);

    /* Enable threshold based drain. */
    if (SAL_BOOT_BCMSIM) {
        soc_mem_field32_set(unit, collector_mem,
                            &config, MAX_RECORDS_PER_PACKETf, 0x1);
        soc_mem_field32_set(unit, collector_mem,
                            &config, TIME_INTERVAL_ENABLEf, 0x0);
        soc_mem_field32_set(unit, collector_mem,
                            &config, TIME_INTERVALf, 0x0);
    }

    if (collector_info->collector_type == bcmFlowtrackerCollectorRemote) {
        soc_mem_field32_set(unit, collector_mem,
                            &config, PACKET_BUILD_ENABLEf, 0x1);
    } else {
        /* Local collector */
        rval = 0;
        collector_reg = BSC_EX_EXPORT_CONTROLr;

        soc_reg_field_set(unit, collector_reg, &rval, DMA_ENf, 0x1);
        soc_reg_field_set(unit, collector_reg, &rval, DMA_COLLECTOR_IDf, coll_hw_id);

        rv = soc_reg32_set(unit, collector_reg, REG_PORT_ANY, 0, rval);
        BCM_IF_ERROR_RETURN(rv);
    }

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
        (ft_coll_info->ft_type == BCMI_FT_COLLECTOR_FT_TYPE_MICRO)) {
        soc_mem_field32_set(unit, collector_mem,
                            &config, RA_ENABLEf, 0x1);
    }
#endif /*BCM_FLOWTRACKER_V2_SUPPORT */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
        (ft_coll_info->ft_type == BCMI_FT_COLLECTOR_FT_TYPE_AGGREGATE)) {
        soc_mem_field32_set(unit, collector_mem,
                            &config, AGG_ENABLEf, 0x1);
    }
#endif /*BCM_FLOWTRACKER_V3_SUPPORT */

    soc_mem_field32_set(unit, collector_mem, &config, ENABLEf, 0x1);
    soc_mem_write(unit, collector_mem, COPYNO_ALL, coll_hw_id, &config);

    return BCM_E_NONE;
}

/* Routine to validate collector params. */
STATIC int
bcmi_ft_collector_info_validate(int unit,
                                bcm_flowtracker_collector_info_t *info)
{
    int min_frame_size = 0, max_frame_size = 0;

    /* Check if the encap mode is supported. */
    if (info->collector_type == bcmFlowtrackerCollectorRemote) {
        switch (info->transport_type) {
            case bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp:
            case bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp:
                /* Currently supported encap types for remote collector. */
                break;
            default:
                return BCM_E_PARAM;
        }
    } else if (info->collector_type == bcmFlowtrackerCollectorLocal) {
        switch (info->transport_type) {
            case bcmFlowtrackerCollectorTransportTypeRaw:
                /* Only rawmode transport is supported for local collector. */
                break;
            default:
                return BCM_E_PARAM;
        }
    } else {
        /* Unknown collector type  */
        return BCM_E_PARAM;
    }

    /* Calculate min_frame_size and max_frame_size - TBD */
    if (info->collector_type == bcmFlowtrackerCollectorRemote) {
        /*
         * IpfixIpv4Udp: L2_HDR + IPV4_HDR + UDP_HDR
         * IpfixIpv6Udp: L2_HDR + IPV6_HDR + UDP_HDR
         *
         * L2_HDR: MACDA (6) + MACSA (6) + TPID (2) = 14 Bytes.
         * IPV4_HDR = 20 Bytes.
         * IPV6_HDR = 40 Bytes.
         * UDP_HDR = 8 Bytes.
 */
        if (info->transport_type ==
                   bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) {
            min_frame_size = 46;
        } else if (info->transport_type ==
                   bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp) {
            min_frame_size = 66;
        } else {
            return BCM_E_PARAM;
        }

        max_frame_size = (BCMI_FT_IPFIX_PACKET_BUILD_MAX_SIZE +
                         (BCMI_FT_IPFIX_MAX_RECORDS_PER_PACKET *
                          BCMI_FT_IPFIX_RECORD_SIZE)
                         );

        /* Verify only if user have given max_packet_length */
        if (info->max_packet_length != 0) {
            if ((info->max_packet_length < min_frame_size) ||
                    (info->max_packet_length > max_frame_size)) {
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_NONE;
}

#if 0
int hx5_supported_elems[BCMI_FT_MAX_TEMPLATES] = {
    bcmFlowtrackerExportElementTypeSrcIPv4,
    bcmFlowtrackerExportElementTypeDstIPv4,
    bcmFlowtrackerExportElementTypeSrcIPv6,
    bcmFlowtrackerExportElementTypeDstIPv6,
    bcmFlowtrackerExportElementTypeL4SrcPort,
    bcmFlowtrackerExportElementTypeL4DstPort,
    bcmFlowtrackerExportElementTypeIPProtocol,
    bcmFlowtrackerExportElementTypePktCount,
    bcmFlowtrackerExportElementTypeByteCount,
    bcmFlowtrackerExportElementTypeFlowtrackerGroup,
    bcmFlowtrackerExportElementTypeReserved
};

struct hx5_support_elem_s {
    bcm_flowtracker_export_element_type_t elem;
    char elem_name[20];
} hx5_supported_elems[BCMI_FT_MAX_TEMPLATES] = {
    {bcmFlowtrackerExportElementTypeSrcIPv4,
     "SrcIpv4"},
    {bcmFlowtrackerExportElementTypeDstIPv4,
     "DstIpv4"},
    {bcmFlowtrackerExportElementTypeSrcIPv6,
     "SrcIpv6"},
    {bcmFlowtrackerExportElementTypeDstIPv6,
     "DstIpv6"},
    {bcmFlowtrackerExportElementTypeL4SrcPort,
     "SrcL4Port"},
    {bcmFlowtrackerExportElementTypeL4DstPort,
     "DstL4Port"},
    {bcmFlowtrackerExportElementTypeIPProtocol,
     "IpProto"},
    {bcmFlowtrackerExportElementTypePktCount,
     "PktCount"},
    {bcmFlowtrackerExportElementTypeByteCount,
     "ByteCount"},
    {bcmFlowtrackerExportElementTypeFlowtrackerGroup,
     "FlowGroup"},
    {bcmFlowtrackerExportElementTypeReserved,
     "Reserved"},
};
#endif


/* Routine to validate template params. */
STATIC int
bcmi_ft_template_validate(int unit,
                          uint16 set_id, int num_elems,
                          bcm_flowtracker_export_element_info_t *elem_list,
                          int *actual_export_elems)
{
    int ix, jx, rv = BCM_E_NONE;
    bcm_flowtracker_export_element_info_t *elem;
    uint16 ft_type_bmp = 0;

    if ((num_elems <= 0) ||
        (num_elems >= BCMI_FT_EXPORT_INFO(unit)->max_elements)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    /* Validate set identifier. */
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
        soc_feature(unit, soc_feature_flex_flowtracker_ver_2_setid_war)) {

        if (!((set_id >= 0x8000) && (set_id <= 0x803F))) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "SET_ID is not in allowed range 0x8000-0x803F\n")));
            return BCM_E_PARAM;
        }
    }
#endif

    for (jx = 0; jx < num_elems; jx++) {
        elem = elem_list +jx;

        if (elem->data_size == 0) {
            *actual_export_elems = jx;
            break;
        }

        for (ix = 0; ix < COUNTOF(bcmi_ft_export_elements_map); ix++) {
            if (elem->element == bcmi_ft_export_elements_map[ix].export_element) {
                /* Break the loop when found. */
                break;
            }
        }

        /* Reached end of supported list. Not found.  */
        if (ix >= COUNTOF(bcmi_ft_export_elements_map)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "Could not find name for element %d\n"), elem->element));
            return BCM_E_PARAM;
        }

        /* Check tracking param is supported in DB or not */
        rv = _bcm_field_ft_tracking_param_type_get(unit,
                bcmi_ft_export_elements_map[ix].tracking_param,
                elem->custom_id, 0, &ft_type_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (*actual_export_elems == 0) {
        *actual_export_elems = num_elems;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_template_id_alloc
 * Purpose:
 *   get free index for template id.
 * Parameters:
 *   unit - (IN) BCM device id
 *   alloc - (IN) Allocate a fresh id.
 *   template_id   - (OUT) FT export template id.
 * Returns:
 *   BCM_E_XXX.
 */
STATIC int
bcmi_ft_template_id_alloc(int unit, int alloc, int *template_id)
{

    int idx, num_templates;

    num_templates = BCMI_FT_EXPORT_INFO(unit)->max_templates;

    if (!alloc) {
        idx = *template_id;
        if ((idx >= num_templates) ||
            (idx <= BCMI_FT_TEMPLATE_ID_INVALID)) {
            return BCM_E_PARAM;
        }

        if ((SHR_BITGET(BCMI_FT_TEMPLATE_BITMAP(unit), idx))) {
            return BCM_E_EXISTS;
        }

        /* Reserve new id. */
        SHR_BITSET(BCMI_FT_TEMPLATE_BITMAP(unit), idx);

        return BCM_E_NONE;
    }

    for (idx = 0; idx < num_templates; idx++) {

        if (!(SHR_BITGET(BCMI_FT_TEMPLATE_BITMAP(unit), idx))) {

            /* Found an unused template id. */
            SHR_BITSET(BCMI_FT_TEMPLATE_BITMAP(unit), idx);

            *template_id = idx;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *   bcmi_ft_export_state_clear
 * Purpose:
 *   Clear Flowtracker export state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ft_export_state_clear(int unit)
{
    int ix;
    bcmi_ft_template_info_t *template_info;

    if (NULL == BCMI_FT_EXPORT_INFO(unit)) {
        return (BCM_E_NONE);
    }

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_templates; ix++) {
        template_info = BCMI_FT_TEMPLATE_INFO(unit, ix);

        if (template_info != NULL) {
            sal_free(template_info);
            template_info = NULL;
        }
    }

    if (bcmi_ft_export_state[unit] != NULL) {
        sal_free(bcmi_ft_export_state[unit]);
        bcmi_ft_export_state[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_export_state_init
 * Purpose:
 *   Initialize Flowtracker export state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ft_export_state_init(int unit)
{
    int ix;
    int num_collectors;
    int record_length;
    soc_mem_t collector_config_mem;
    bcmi_ft_collector_info_t *collector_info;
    bcmi_ft_template_info_t *template_info;

    collector_config_mem = BSC_EX_COLLECTOR_CONFIGm;

    /* Allocate export software state. */
    bcmi_ft_export_state[unit] =
        sal_alloc(sizeof(bcmi_ft_export_state_t),
                  "flowtracker export sw state");

    if (bcmi_ft_export_state[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(bcmi_ft_export_state[unit], 0, sizeof(bcmi_ft_export_state_t));

    /* total number of collectors. */
    num_collectors = soc_mem_index_count(unit, collector_config_mem);
    BCMI_FT_EXPORT_INFO(unit)->max_collectors = num_collectors;

    /* Max number of templates cannot be greater than the max groups.
     * Note: EXPORT_FIFO will have half of the record in each entry.
     */
    record_length = (soc_mem_entry_words(unit, BSC_EX_EXPORT_FIFOm) * 2) * 4;
    BCMI_FT_EXPORT_INFO(unit)->record_length = record_length;

    BCMI_FT_EXPORT_INFO(unit)->max_templates =
        soc_mem_index_count(unit, BSC_KG_GROUP_TABLEm);

    /* Max number of elements in a template cannot be more than the
     * number of octets in a record. */
    BCMI_FT_EXPORT_INFO(unit)->max_elements =
        BCMI_FT_EXPORT_INFO(unit)->record_length;

    /* Initialize independent collector_info structures. */
    for (ix = 0; ix < num_collectors; ix++) {
        collector_info = BCMI_FT_COLLECTOR_INFO(unit, ix);
        collector_info->coll_hw_id = BCMI_FT_COLLECTOR_ID_INVALID;
        collector_info->cmn_coll_id = BCMI_FT_COLLECTOR_ID_INVALID;
        collector_info->num_records = -1;
    }

    /* Allocate memory for each template_info struct. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_templates; ix++) {

        template_info = sal_alloc(sizeof(*template_info), "template info");
        if (template_info == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(template_info, 0, sizeof(*template_info));
        template_info->template_id = BCMI_FT_TEMPLATE_ID_INVALID;

        BCMI_FT_TEMPLATE_INFO(unit, ix) = template_info;
    }

    /* Set Dma collector to Invalid */
    BCMI_FT_EXPORT_INFO(unit)->dma_collector_id = BCMI_FT_COLLECTOR_ID_INVALID;

    /* Export FIFO to Collector */
    BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries =
        (soc_mem_index_count(unit, BSC_EX_EXPORT_FIFOm) / 2);

    BCMI_FT_EXPORT_INFO(unit)->fifo_collector_map[0] =
            BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_export_periodic_interval_get
 * Purpose:
 *    Get the interval value (hw) from given interval
 *    in seconds and useconds.
 * Parameters:
 *     unit           - (IN) BCM device id
 *     interval_secs  - (IN) Interval in seconds
 *     interval_usecs - (IN) Interval in useconds
 *     fld_len        - (IN) size of field to fit
 *     interval       - (OUT) Calculated interval
 *
 * Returns:
 *     None.
 */
void
bcmi_ft_export_periodic_interval_get(int unit,
        uint32 interval_secs, uint32 interval_usecs,
        uint32 fld_len, uint32 *interval)
{
    uint32 high_val;
    uint64 val64;

    /* Export Interval in seconds */
    COMPILER_64_SET(val64, 0, interval_secs);

    /* Export Interval in 100 msecs */
    COMPILER_64_UMUL_32(val64, 10);

    /* Converting usecs to 100 msecs */
    *interval = interval_usecs / 1000;
    *interval = *interval / 100;

    /* Add to get export interval in 100 msecs */
    COMPILER_64_ADD_32(val64, *interval);

    COMPILER_64_TO_32_HI(high_val, val64);
    if (high_val != 0) {
        *interval = 0xFFFFFFFF;
    } else {
        COMPILER_64_TO_32_LO(*interval, val64);
    }
    /* In case user have configured export interval value less than 100 ms,
     * but not zero. here user intention is to set least value of export
     * interval, so set interval=1 (100 ms) */
    if ((*interval == 0) && (interval_usecs != 0)) {
        *interval = 1;
    }

    /* Do not exceed field length */
    *interval = MIN(*interval, ((1 << fld_len) - 1));
}

int
bcmi_ft_export_init(int unit)
{
    int rv;
    int ipfix_obs_domain_id;
    uint32 fld_len, interval;
    uint32 interval_usecs, interval_secs;

    rv = bcmi_ft_export_cleanup(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ft_export_state_init(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        rv = bcmi_ft_export_cb_init(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
        rv = bcmi_ftv3_aft_export_sw_ctrl_init(unit);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

    ipfix_obs_domain_id = soc_property_get(unit,
                              spn_FLOWTRACKER_IPFIX_OBSERVATION_DOMAIN_ID,
                              BCMI_FTE_IPFIX_HDR_OBS_DOMAIN);
    BCMI_FT_EXPORT_INFO(unit)->ipfix_observation_domain_id =
                                  ipfix_obs_domain_id;

    interval_secs = soc_property_get(unit,
                        spn_FLOWTRACKER_EXPORT_INTERVAL_SECS, 0);
    interval_usecs = soc_property_get(unit,
                          spn_FLOWTRACKER_EXPORT_INTERVAL_USECS, 0);

    fld_len = soc_reg_field_length(unit, BSC_AG_PERIODIC_EXPORTr,
                EXPORT_PERIODf);

    bcmi_ft_export_periodic_interval_get(unit, interval_secs,
            interval_usecs, fld_len, &interval);

    rv = soc_reg_field32_modify(unit, BSC_AG_PERIODIC_EXPORTr,
                                REG_PORT_ANY, EXPORT_PERIODf, interval);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        interval_secs = soc_property_get(unit,
                spn_FLOWTRACKER_AGGREGATE_EXPORT_INTERVAL_SECS, 0);
        interval_usecs = soc_property_get(unit,
                spn_FLOWTRACKER_AGGREGATE_EXPORT_INTERVAL_USECS, 0);

        fld_len = soc_reg_field_length(unit, BSC_AG_AFT_PERIODIC_EXPORTr,
                EXPORT_PERIODf);

        bcmi_ft_export_periodic_interval_get(unit, interval_secs,
                interval_usecs, fld_len, &interval);

        if (soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
            rv = bcmi_ftv3_aft_export_sw_scan_update(unit, interval);
        } else {
            rv = soc_reg_field32_modify(unit, BSC_AG_AFT_PERIODIC_EXPORTr,
                REG_PORT_ANY, EXPORT_PERIODf, interval);
        }
    }
#endif

    return rv;
}

int
bcmi_ft_export_cleanup(int unit)
{
    int rv;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
        rv = bcmi_ftv3_aft_export_sw_ctrl_cleanup(unit);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        rv = bcmi_ft_export_cb_deinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcmi_ft_export_state_clear(unit);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_collector_create
 * Purpose:
 *   Create flowtracker collector id.
 * Description :
 *   Routine to create a flowtracker collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   options - (IN) options for group creation
 *   coll_hw_id   - (OUT) FT collector id.
 *   collector_info - (IN) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_collector_create(int unit,
                    uint32 options,
                    bcm_flowtracker_collector_t *coll_hw_id,
                    bcm_flowtracker_collector_info_t *collector_info)
{
    int rv;
    int idx;
    bcmi_ft_collector_info_t *ft_coll_info;
    bcm_collector_ipfix_header_t *ipfix_hdr;
    bcm_flowtracker_collector_info_t old_collector_info;

    /* Check if remote collector is supported. */
    if (collector_info->collector_type ==
                        bcmFlowtrackerCollectorRemote) {

        if (!(soc_feature(unit,
            soc_feature_flowtracker_ver_1_ipfix_remote_collector))) {
            return BCM_E_UNAVAIL;
        }
    }

    /*
     * Validate collector_info for any unsupported encap or
     * export length etc.
     */
    rv = bcmi_ft_collector_info_validate(unit, collector_info);
    BCM_IF_ERROR_RETURN(rv);

    /* REPLACE should be set along with WITH_ID. */
    if ((options & BCM_FLOWTRACKER_COLLECTOR_REPLACE) &&
       !(options & BCM_FLOWTRACKER_COLLECTOR_WITH_ID)) {
        return BCM_E_PARAM;
    }

    /* While replacing, original entry should be already created. */
    if (options & BCM_FLOWTRACKER_COLLECTOR_REPLACE) {
        sal_memset(&old_collector_info, 0, sizeof(old_collector_info));

        rv = bcmi_ft_collector_get(unit, *coll_hw_id, &old_collector_info);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* WITH_ID and not replacing. */
    if (options & BCM_FLOWTRACKER_COLLECTOR_WITH_ID) {

        if (options & BCM_FLOWTRACKER_COLLECTOR_REPLACE) {
            /* Reserve Id. */
            ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, *coll_hw_id);
            ft_coll_info->coll_hw_id = *coll_hw_id;
        }

    } else {
        for (idx = 0; idx < BCMI_FT_MAX_COLLECTORS; idx++) {
            ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, idx);
            if (ft_coll_info->coll_hw_id ==
                    BCMI_FT_COLLECTOR_ID_INVALID) {
                break;
            }
        }
        if (idx < BCMI_FT_MAX_COLLECTORS) {
            *coll_hw_id = idx;
            ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, *coll_hw_id);
            ft_coll_info->coll_hw_id = *coll_hw_id;
        } else {
            return BCM_E_PARAM;
        }
    }

    /* Copy the collector info to system cache. */
    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, *coll_hw_id);
    sal_memcpy(&(ft_coll_info->collector_info),
               collector_info, sizeof(*collector_info));

    if (!(ft_coll_info->ipfix_hdr_use_custom)) {

        ipfix_hdr = &(ft_coll_info->ipfix_hdr);
        ipfix_hdr->version = BCMI_FTE_IPFIX_HDR_VERSION;
        ipfix_hdr->initial_sequence_num = 0;
    }

    /* Install in hardware. */
    rv = bcmi_ft_collector_hw_install(unit, *coll_hw_id, TRUE);

    if (BCM_SUCCESS(rv)) {
        ft_coll_info->coll_hw_id = *coll_hw_id;
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_collector_get
 * Purpose:
 *   Fetch flowtracker collector info.
 * Description :
 *   Routine to create a flowtracker collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   coll_hw_id   - (OUT) FT collector id.
 *   collector_info - (IN) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_collector_get(int unit,
    bcm_flowtracker_collector_t coll_hw_id,
    bcm_flowtracker_collector_info_t *collector_info)
{
    bcmi_ft_collector_info_t *ft_coll_info;

    if (collector_info == NULL) {
        return BCM_E_PARAM;
    }
    if ((coll_hw_id <= BCMI_FT_COLLECTOR_ID_INVALID) ||
        (coll_hw_id >= BCMI_FT_MAX_COLLECTORS)) {
        return BCM_E_PARAM;
    }

    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, coll_hw_id);

    if (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID) {
        return BCM_E_PARAM;
    }

    sal_memcpy(collector_info,
               &(ft_coll_info->collector_info),
               sizeof(*collector_info));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_collector_get_all
 * Purpose:
 *   Fetch all flowtracker collectors info.
 * Description :
 *   Routine to create a flowtracker collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   max_size   - (OUT) Max collectors to read.
 *   collector_list - (OUT) Collector id list.
 *   list_size (OUT) - Num of collectors read.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_collector_get_all(int unit,
                        int max_size,
                        bcm_flowtracker_collector_t *collector_list,
                        int *list_size)
{
    int idx, cnt;
    bcm_flowtracker_collector_t *coll_hw_id;
    bcmi_ft_collector_info_t *ft_coll_info;

    if ((max_size != 0) && (collector_list == NULL)) {
        return BCM_E_PARAM;
    }
    if (list_size == NULL) {
        return BCM_E_PARAM;
    }

    if ((max_size <= BCMI_FT_COLLECTOR_ID_INVALID) ||
        (max_size >= BCMI_FT_MAX_COLLECTORS)) {
        return BCM_E_PARAM;
    }

    *list_size = cnt = 0;

    for (idx = 0; idx < BCMI_FT_MAX_COLLECTORS; idx++) {
        ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, idx);
        coll_hw_id = &(collector_list[(*list_size)]);

        if (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID) {
            continue;
        }

        if (cnt < max_size) {
            *coll_hw_id = ft_coll_info->coll_hw_id;
            cnt++;
        }

        (*list_size) += 1;
    }

    if (max_size == 0) {
        *list_size = cnt;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_collector_destroy
 * Purpose:
 *   Destroy flowtracker collector.
 * Description :
 *   Routine to destroy a flowtracker collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   coll_hw_id   - (OUT) FT collector id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_collector_destroy(int unit,
            bcm_flowtracker_collector_t coll_hw_id)
{
    int dma_collector_id;
    bcmi_ft_collector_info_t *ft_coll_info;
    bcm_flowtracker_collector_callback_options_t callback_options;

    if ((coll_hw_id <= BCMI_FT_COLLECTOR_ID_INVALID) ||
        (coll_hw_id >= BCMI_FT_MAX_COLLECTORS)) {
        return BCM_E_PARAM;
    }

    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, coll_hw_id);
    if (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    /* Allow destroying collector only if it is detached from all groups. */
    if (ft_coll_info->in_use != 0) {
        return BCM_E_BUSY;
    }

    if (ft_coll_info->cmn_coll_id ==
            BCMI_FT_EXPORT_INFO(unit)->dma_collector_id) {
        dma_collector_id = BCMI_FT_EXPORT_INFO(unit)->dma_collector_id;
    } else {
        dma_collector_id = BCMI_FT_COLLECTOR_ID_INVALID;
    }

    sal_memset(&(ft_coll_info->collector_info), 0,
            sizeof(ft_coll_info->collector_info));
    ft_coll_info->coll_hw_id = BCMI_FT_COLLECTOR_ID_INVALID;
    ft_coll_info->cmn_coll_id = BCMI_FT_COLLECTOR_ID_INVALID;
    ft_coll_info->cmn_export_prof_id =
        BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID;
    ft_coll_info->num_records = -1;

    BCM_IF_ERROR_RETURN(bcmi_ft_collector_hw_install(unit, coll_hw_id, FALSE));

    if (dma_collector_id != BCMI_FT_COLLECTOR_ID_INVALID) {
        /* Unregister callbacks */
        bcm_flowtracker_collector_callback_options_t_init(&callback_options);
        (void) bcmi_ft_export_record_unregister(unit,
                dma_collector_id, callback_options, NULL);

        /* DMA collector config is also removed */
        BCMI_FT_EXPORT_INFO(unit)->dma_collector_id =
            BCMI_FT_COLLECTOR_ID_INVALID;
    }

    return BCM_E_NONE;
}

#if 0
STATIC int
bcmi_ft_templ_fill_elem(int unit, int num_bits, int elem_size,
                        bcm_flowtracker_export_element_type_t elem_type,
                        bcm_flowtracker_export_element_info_t *out_elems,
                        int *num_elems)
{
    bcm_flowtracker_export_element_info_t *l_elem_list;

    *num_elems = 0;
    while (num_bits) {
        l_elem_list = out_elems;

        l_elem_list->element = elem_type;
        l_elem_list->data_size = 1;

        num_bits -= elem_size;
        out_elems++;
        (*num_elems)++;
    }

    return BCM_E_NONE;
}
#endif

STATIC int
bcmi_ft_tracking_param_xlate(int unit,
                             bcm_flowtracker_tracking_param_type_t elem,
                             int *size,
                             bcm_flowtracker_export_element_type_t *info_elem)
{
    int ix;

    for (ix = 0; ix < COUNTOF(bcmi_ft_export_elements_map); ix++) {
        if (bcmi_ft_export_elements_map[ix].tracking_param == elem) {
            if (size) {
                *size = bcmi_ft_export_elements_map[ix].default_data_size;
            }
            if (info_elem) {
                *info_elem = bcmi_ft_export_elements_map[ix].export_element;
            }

            break;
        }
    }

    return BCM_E_NONE;
}

#if 0
typedef enum pdd_field_type_e {
    bcmiFtePddFldTypeAlu32,
    bcmiFtePddFldTypeLoad16,
    bcmiFtePddFldTypeAlu16,
    bcmiFtePddFldTypeLoad8,
    bcmiFtePddFldTypeReserved,
} pdd_field_type_t;

struct bcmi_fte_pdd_fields_s {
    int length;
    pdd_field_type_t field_type;
    soc_field_t pdd_field;
} bcmi_fte_flex_pdd_action_list[] = {
        {4, bcmiFtePddFldTypeReserved, BSD_FLEX_TIMESTAMP_0f,              },
        {4, bcmiFtePddFldTypeReserved, BSD_FLEX_TIMESTAMP_1f,              },
        {4, bcmiFtePddFldTypeReserved, BSD_FLEX_TIMESTAMP_2f,              },
        {4, bcmiFtePddFldTypeReserved, BSD_FLEX_TIMESTAMP_3f,              },
        {4, bcmiFtePddFldTypeReserved, BSD_FLEX_FLOW_METERf,               },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_0f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_1f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_2f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_3f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_4f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_5f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_6f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_7f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_8f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_9f,   },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_10f,  },
        {4, bcmiFtePddFldTypeAlu32,    BSD_FLEX_ALU32_TRACKING_PARAM_11f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_0f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_1f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_2f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_3f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_4f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_5f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_6f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_7f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_8f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_9f,  },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_10f, },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_11f, },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_12f, },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_13f, },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_14f, },
        {2, bcmiFtePddFldTypeLoad16,   BSD_FLEX_LOAD16_TRACKING_PARAM_15f, },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_0f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_1f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_2f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_3f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_4f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_5f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_6f,   },
        {2, bcmiFtePddFldTypeAlu16,    BSD_FLEX_ALU16_TRACKING_PARAM_7f,   },
        {2, bcmiFtePddFldTypeReserved, BSD_FLEX_FLOW_COUNT_TO_CPUf,        },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_0f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_1f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_2f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_3f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_4f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_5f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_6f,   },
        {1, bcmiFtePddFldTypeLoad8,    BSD_FLEX_LOAD8_TRACKING_PARAM_7f,   },
        {1, bcmiFtePddFldTypeReserved, BSD_FLEX_FLOW_STATEf,               },
        {0, bcmiFtePddFldTypeReserved, INVALIDf,                            },
};
#endif

STATIC int
bcmi_ft_export_elem_name_get(int unit,
                             bcm_flowtracker_export_element_type_t info_elem,
                             int *data_size,
                             char *elem_name)
{
    int ix;

    for (ix = 0; ix < COUNTOF(bcmi_ft_export_elements_map); ix++) {
        if (bcmi_ft_export_elements_map[ix].export_element == info_elem) {
            if (elem_name) {
                sal_strlcpy(elem_name,
                            bcmi_ft_export_elements_map[ix].element_name,
                            BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
            }
            if (data_size) {
                *data_size = bcmi_ft_export_elements_map[ix].default_data_size;
            }
            break;
        }
    }

    if (ix == COUNTOF(bcmi_ft_export_elements_map)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_fte_template_record_flow_key_populate(int unit,
        bcm_flowtracker_group_t flow_group,
        bcmi_fte_template_record_info_t *record)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int key_idx = 0;
    int offset = 0;
    int num_key_info = 0;
    int rec_total_bits = 0;
    int flow_key_offset = 0;
    bcmi_ft_alu_key_t *key_alu = NULL;
    bcmi_ft_group_alu_info_t *key_ext_info = NULL;
    bcm_flowtracker_export_element_type_t info_elem;
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];

    /* Get Session Key info from FT Group */
    key_ext_info = BCMI_FT_GROUP_EXT_KEY_INFO(unit, flow_group);
    num_key_info = BCMI_FT_GROUP_EXT_INFO(unit, flow_group).num_key_info;

    rec_total_bits = record->total_bits;
    flow_key_offset = record->flow_key_offset;

    /* 1st Byte at Flow_key_offset is skipped as it represent KeyType + Mode */
    flow_key_offset += 8;

    for (key_idx = 0; key_idx < num_key_info; key_idx++) {
        key_alu = &(key_ext_info->key1);
        offset = flow_key_offset + key_alu->location;

        info_elem = bcmFlowtrackerExportElementTypeReserved;
        rv = bcmi_ft_tracking_param_xlate(unit,
                key_ext_info->element_type1, NULL, &info_elem);
        BCM_IF_ERROR_RETURN(rv);

        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, key_alu->length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, key_alu->length);
        BCMI_FTE_RECORD_TRACKING_PARAM_SET(record, idx, key_ext_info->element_type1);
        BCMI_FTE_RECORD_ELEM_CUSTOMID_SET(record, idx, key_ext_info->custom_id1);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | "
                        "Id: %2d %20s(%d) | Size: %02d\n\r"), idx, info_elem,
                    info_name, key_ext_info->custom_id1, key_alu->length));
        key_ext_info++;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_fte_template_record_flow_data_polpulate(int unit,
                    bcm_flowtracker_group_t flow_group,
                    bcmi_fte_template_record_info_t *record)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int offset = 0;
    int data_bits = 0;
    int rec_total_bits = 0;
    int flow_data_offset = 0;
    uint16 custom_id = 0;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_export_element_type_t info_elem;
    bcm_flowtracker_tracking_param_type_t tracking_param;
    bcmi_ft_group_template_list_t *ftg_template = NULL;
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
    char *record_dir[] = {"None", "Fwd", "Rev", "Bidir"};

    /* Flow Data is populated from higher bits to lower bits */
    rec_total_bits = record->total_bits;
    flow_data_offset = record->flow_data_offset + record->flow_data_length;

    ftg_template = ((BCMI_FT_GROUP(unit, flow_group))->template_head);
    while(ftg_template) {
        switch(ftg_template->info.param_type) {
            case bcmiFtGroupParamTypeTracking:
                tracking_param =
                    ftg_template->info.param;
                break;

            case bcmiFtGroupParamTypeTsNewLearn:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
                break;
            case bcmiFtGroupParamTypeTsFlowStart:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampFlowStart;
                break;
            case bcmiFtGroupParamTypeTsFlowEnd:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampFlowEnd;
                break;
            case bcmiFtGroupParamTypeTsCheckEvent1:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
                break;
            case bcmiFtGroupParamTypeTsCheckEvent2:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
                break;
            case bcmiFtGroupParamTypeTsCheckEvent3:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3;
                break;
            case bcmiFtGroupParamTypeTsCheckEvent4:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4;
                break;
            case bcmiFtGroupParamTypeTsIngress:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampIngress;
                break;
            case bcmiFtGroupParamTypeTsEgress:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeTimestampEgress;
                break;
            case bcmiFtGroupParamTypeFlowchecker:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeFlowtrackerCheck;
                break;

            default:
                tracking_param =
                    bcmFlowtrackerTrackingParamTypeCount;
                break;
        }
        check_id = ftg_template->info.check_id;
        info_elem = bcmFlowtrackerExportElementTypeReserved;
        custom_id = ftg_template->info.custom_id;
        rv = bcmi_ft_tracking_param_xlate(unit,
                   tracking_param, NULL, &info_elem);
        BCM_IF_ERROR_RETURN(rv);

        data_bits = ftg_template->info.cont_width;

        if (ftg_template->info.flags & BCMI_FT_ALU_LOAD_NEXT_ATTACH) {
            /* Special case for dos attack. */
            data_bits += ftg_template->next->info.cont_width;
        }

        if (ftg_template->info.data_shift != 0) {
            data_bits -= ftg_template->info.data_shift;
        }

        offset = ftg_template->info.cont_offset;
        offset = flow_data_offset - (offset + data_bits);

        /* Number of data bits */
        if (ftg_template->info.data_width > 0) {
            data_bits = ftg_template->info.data_width;

            /* If attached, add up next to get total bits */
            if (ftg_template->info.flags & BCMI_FT_ALU_LOAD_NEXT_ATTACH) {
                data_bits += ftg_template->next->info.data_width;
            }
        }

        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, data_bits, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, data_bits);
        BCMI_FTE_RECORD_TRACKING_PARAM_SET(record, idx, tracking_param);
        BCMI_FTE_RECORD_FLOW_CHECK_SET(record, idx, check_id);
        BCMI_FTE_RECORD_FLOW_DIRECTION_SET(record, idx, ftg_template->info.direction);
        BCMI_FTE_RECORD_ELEM_CUSTOMID_SET(record, idx, custom_id);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d"
                        " (%20s(%d), %5s) | Size: %02d (0x%x)\n\r"), idx,
                    info_elem, info_name, custom_id,
                    record_dir[ftg_template->info.direction],
                    data_bits, check_id));

        if (ftg_template->info.flags & BCMI_FT_ALU_LOAD_NEXT_ATTACH) {
            /* This is surely at least second last. so we can blindly go next.*/
            ftg_template = ftg_template->next;
        }

        ftg_template = ftg_template->next;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_fte_template_record_info_populate(int unit,
        bcm_flowtracker_group_t flow_group,
        bcmi_fte_template_record_info_t *record)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int offset = 0;
    int length = 0;
    int rec_total_bits = 0;
    soc_field_info_t *field_info = NULL;
    soc_format_t format;
    bcm_flowtracker_export_element_type_t info_elem;
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];

    format = BSC_EX_EXPORT_RECORDfmt;

    /* Get record total bits, base offset of SET HEADER */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, SET_HEADERf);
    if (field_info != NULL) {
        rec_total_bits = field_info->bp;
    }
    record->total_bits = rec_total_bits;
    record->info_depth = (record->total_bits / 8);

    /* Allocate memory for record */
    record->info = sal_alloc(record->info_depth *
                sizeof(bcmi_fte_template_record_elem_info_t),
                "Export Record element info");
    if (record->info == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initialize with 'Reserved' element */
    for (idx = 0; idx < record->info_depth; idx++) {
        record->info[idx].info_elem = bcmFlowtrackerExportElementTypeReserved;
        record->info[idx].length = 8;
        record->info[idx].check_id = 0;
        record->info[idx].tracking_param = 0;
        record->info[idx].direction = bcmFlowtrackerFlowDirectionNone;
        record->info[idx].custom_id = 0;
    }

    /* Insert hardware extracted default elements */
    /* FT_INDEX */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, FLOW_INDEXf);
    if (field_info != NULL) {
        offset = field_info->bp;
        length = field_info->len;
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
            /* In BS3.0, FLOW_INDEX is 20b, where as 16b in earlier version.
               This export element needs to rounded off in BS3.0. User
               needs to mask off unwanted bits.
             */
            length = 16;
        }
        info_elem = bcmFlowtrackerExportElementTypeReserved;
        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d\n\r"), idx, info_elem, info_name, length));
    }

    /* FT_Group */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, GROUP_IDf);
    if (field_info != NULL) {
        offset = field_info->bp;
        length = field_info->len;
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
            /* In BS3.0, FLOW_GROUP is 12b, where as 16b in earlier version.
               This export element needs to rounded off in BS3.0. User
               needs to mask off unwanted bits.
             */
            length = 16;
        }
        info_elem = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d\n\r"), idx, info_elem, info_name, length));
    }

    /* FT_EXPORT_REASON */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, EXPORT_REASONf);
    if (field_info != NULL) {
        offset = field_info->bp;
        length = field_info->len;
        info_elem = bcmFlowtrackerExportElementTypeExportReasons;
        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d\n\r"), idx, info_elem, info_name, length));
    }

    /* EXPORT_FLAGS */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, EXPORT_FLAGSf);
    if (field_info != NULL) {
        offset = field_info->bp;
        length = field_info->len;
        info_elem = bcmFlowtrackerExportElementTypeExportFlags;
        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d\n\r"), idx, info_elem, info_name, length));
    }

    /* TIMESTAMP */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, TIMESTAMPf);
    if (field_info != NULL) {
        offset = field_info->bp;
        length = field_info->len;
        info_elem = bcmFlowtrackerExportElementTypeReserved;
        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d\n\r"), idx, info_elem, info_name, length));
    }

    /* FLOW_KEY */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, FLOW_KEYf);
    if (field_info != NULL) {
        record->flow_key_offset = field_info->bp;
        record->flow_key_length = field_info->len;
    }
    rv = bcmi_fte_template_record_flow_key_populate(unit, flow_group, record);
    if(BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* FLOW_DATA */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, FLOW_DATAf);
    if (field_info != NULL) {
        record->flow_data_offset = field_info->bp;
        record->flow_data_length = field_info->len;
    }
    rv = bcmi_fte_template_record_flow_data_polpulate(unit, flow_group, record);

cleanup:

    if (BCM_FAILURE(rv)) {
        if (record->info != NULL) {
            sal_free(record->info);
            record->info = NULL;
        }
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_export_elements_check_search
 * Purpose:
 *   Search flowcheck in export elements list.
 * Parameters:
 *   unit - (IN) BCM device id
 *   check_id - (IN) Flowtracker Check
 *   list - (IN) list of export elements.
 *   count - (IN) number of export elements.
 * Returns:
 *   TRUE if found FALSE otherwise
 */
int
bcmi_ft_export_elements_check_search(int unit,
                bcm_flowtracker_check_t check_id,
                void *list, int count)
{
    int idx = 0;
    bcm_flowtracker_export_element_info_t *export_elements = NULL;

    if ((list == NULL) || (count == 0)) {
        return FALSE;
    }
    export_elements = (bcm_flowtracker_export_element_info_t *) list;

    for (idx = 0; idx < count; idx++) {
        if ((export_elements[idx].element ==
                bcmFlowtrackerExportElementTypeFlowtrackerCheck) &&
            (export_elements[idx].check_id == check_id)) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Function:
 *   bcmi_fte_template_record_info_update
 * Purpose:
 *   Update internal strcut record info  for
 *   overlapping export elements.
 * Parameters:
 *   unit - (IN) Unit number.
 *   num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *   in_export_elements - (IN) List of export elements intended to be in the template.
 *   record - (IN/OUT) Record Info
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_fte_template_record_info_update(
    int unit,
    int num_in_export_elements,
    bcm_flowtracker_export_element_info_t *in_export_elements,
    bcmi_fte_template_record_info_t *record)
{
    int ix = 0;
    int idx = 0;
    int offset = 0;
    int length = 0;
    int rec_total_bits = 0;
    int flow_group_present = FALSE;
    int flow_identifier_present = FALSE;
    soc_field_info_t *field_info = NULL;
    soc_format_t format;
    bcm_flowtracker_export_element_type_t info_elem;
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];

    format = BSC_EX_EXPORT_RECORDfmt;
    rec_total_bits = record->total_bits;

    for (ix = 0; ix < num_in_export_elements; ix++) {

        switch(in_export_elements[ix].element) {

            case bcmFlowtrackerExportElementTypeFlowIdentifier:
                flow_identifier_present = TRUE;
                /* FLOW_INDEX_GROUP_ID */
                field_info = soc_format_fieldinfo_get(unit, format,
                        GROUP_IDf);
                if (field_info != NULL) {
                    offset = field_info->bp;
                    length = 32;
                    info_elem =
                        bcmFlowtrackerExportElementTypeFlowIdentifier;
                    BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits,
                            offset, length, idx);
                    BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
                    BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);

                    sal_memset(info_name, 0, sizeof(char) * \
                            BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
                    (void) bcmi_ft_export_elem_name_get(unit,
                            info_elem, NULL, info_name);
                    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                    "[TemplateValidate]Element"
                                    " Record_Idx: %2d | Id: %2d (%20s) |"
                                    " Size: %02d\n\r"), idx, info_elem,
                                info_name, length));
                }
                break;
            case bcmFlowtrackerExportElementTypeFlowtrackerGroup:
                flow_group_present = TRUE;
                break;
            default:
                break;
        }
    }

    /* Checks for conflicting export elements */
    if (flow_group_present && flow_identifier_present) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Conflicting Export elements"
                        " are present in export element list\n\r")));
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_export_element_info_for_record_find
 * Purpose:
 *   Find export element info corresponding to
 *   record
 * Parameters:
 *   unit       - (IN) Unit number.
 *   id         - (IN) Flowtracker group Id
 *   record_info- (IN) Record info
 *   num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *   in_export_elements - (IN) List of export elements intended to be in the template.
 *   mapped_export_elements - (IN/OUT) Mapped export element till function is called.
 *   found      - (OUT) Match found
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
STATIC void
bcmi_ft_export_element_info_for_record_find(int unit,
        bcm_flowtracker_group_t id,
        bcmi_fte_template_record_elem_info_t *record_info,
        int num_in_export_elements,
        bcm_flowtracker_export_element_info_t *in_export_elements,
        int *mapped_export_elements,
        int *found)
{
    int ix_in = 0;
    bcm_flowtracker_direction_t grp_dir, rec_dir, ele_dir;
    bcm_flowtracker_export_element_info_t *info_elem_in = NULL;

    /* Set found to FALSE */
    *found = FALSE;
    grp_dir = BCMI_FT_GROUP_FTFP_DATA(unit, id).direction;

    for (ix_in = 0; ix_in < num_in_export_elements; ix_in++) {
        info_elem_in = &(in_export_elements[ix_in]);
        ele_dir = info_elem_in->direction;
        rec_dir = record_info->direction;

        /* Check if export element is already mapped to record */
        if (mapped_export_elements[ix_in] == TRUE) {
            continue;
        }

        /* If non-primary check, skip */
        if (info_elem_in->element ==
                bcmFlowtrackerExportElementTypeFlowtrackerCheck) {

            if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(info_elem_in->check_id)) {
                *found = TRUE;
                mapped_export_elements[ix_in] = TRUE;
                return;
            }

            if ((record_info->info_elem == info_elem_in->element) &&
                (record_info->check_id == info_elem_in->check_id)) {
                *found = TRUE;
                mapped_export_elements[ix_in] = TRUE;
                if (rec_dir == bcmFlowtrackerFlowDirectionNone) {
                    record_info->direction = grp_dir;
                }
                break;
            }
            continue;
        }

        /* Skip export element info if direction mismatch with record
           or group direction
         */
        if (rec_dir == bcmFlowtrackerFlowDirectionNone) {
            if (grp_dir == bcmFlowtrackerFlowDirectionNone) {
                if (ele_dir != bcmFlowtrackerFlowDirectionNone) {
                    continue;
                }
            } else if (grp_dir == bcmFlowtrackerFlowDirectionForward) {
                if ((ele_dir != bcmFlowtrackerFlowDirectionNone) &&
                    (ele_dir != bcmFlowtrackerFlowDirectionForward)) {
                    continue;
                }
                rec_dir = bcmFlowtrackerFlowDirectionForward;
            } else if (grp_dir == bcmFlowtrackerFlowDirectionBidirectional) {
                if ((ele_dir != bcmFlowtrackerFlowDirectionNone) &&
                    (ele_dir != bcmFlowtrackerFlowDirectionBidirectional)) {
                    continue;
                }
                rec_dir = bcmFlowtrackerFlowDirectionBidirectional;
            }
        } else if (rec_dir == bcmFlowtrackerFlowDirectionForward) {
            if (grp_dir == bcmFlowtrackerFlowDirectionNone) {
                continue;
            } else if (grp_dir == bcmFlowtrackerFlowDirectionForward) {
                if ((ele_dir != bcmFlowtrackerFlowDirectionNone) &&
                    (ele_dir != bcmFlowtrackerFlowDirectionForward)) {
                    continue;
                }
            } else if (grp_dir == bcmFlowtrackerFlowDirectionBidirectional) {
                if (ele_dir != bcmFlowtrackerFlowDirectionForward) {
                    continue;
                }
            }
        } else if (rec_dir == bcmFlowtrackerFlowDirectionReverse) {
            if ((grp_dir == bcmFlowtrackerFlowDirectionNone) ||
                (grp_dir == bcmFlowtrackerFlowDirectionForward)) {
                continue;
            } else if (grp_dir == bcmFlowtrackerFlowDirectionBidirectional) {
                if (ele_dir != bcmFlowtrackerFlowDirectionReverse) {
                    continue;
                }
            }
        } else if (rec_dir == bcmFlowtrackerFlowDirectionBidirectional) {
            if ((grp_dir == bcmFlowtrackerFlowDirectionNone) ||
                (grp_dir == bcmFlowtrackerFlowDirectionForward)) {
                continue;
            } else if (grp_dir == bcmFlowtrackerFlowDirectionBidirectional) {
                if ((ele_dir != bcmFlowtrackerFlowDirectionNone) &&
                    (ele_dir != bcmFlowtrackerFlowDirectionBidirectional)) {
                    continue;
                }
            }
        }

        if ((info_elem_in->element == bcmFlowtrackerExportElementTypeCustom) &&
                (record_info->info_elem == info_elem_in->element)) {
           if (record_info->custom_id != info_elem_in->custom_id) {
                continue;
            }
        }

        if ((record_info->info_elem == info_elem_in->element) &&
                (record_info->check_id == info_elem_in->check_id)) {
            *found = TRUE;
            record_info->direction = rec_dir;
            mapped_export_elements[ix_in] = TRUE;
            break;
        }
    }
}


/*
 * Function:
 *   bcmi_fte_template_create_validate
 * Purpose:
 *   Validate the template and return the list supported by the
 *   device.
 * Parameters:
 *   unit - (IN) Unit number.
 *   flow_group_id - (IN) Flowtracker flow group software ID.
 *   num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *   in_export_elements - (IN) List of export elements intended to be in the template.
 *   num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *   out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_fte_template_create_validate(int unit,
        bcm_flowtracker_group_t flow_group_id,
        int num_in_export_elements,
        bcm_flowtracker_export_element_info_t *in_export_elements,
        int num_out_export_elements,
        bcm_flowtracker_export_element_info_t *out_export_elements,
        int *actual_out_export_elements)
{
    int rv = BCM_E_NONE;
    int ix_out = 0, in_c = 0;
    int idx = 0;
    int info_size = 0;
    int found = 0;
    uint16 custom_id = 0;
    int mapped_export_elements[30] = {0};
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
    bcm_flowtracker_check_t check_id;
    bcmi_fte_template_record_info_t record;
    bcm_flowtracker_export_element_type_t info_elem;
    bcm_flowtracker_export_element_info_t *info_elem_out = NULL;
    bcm_flowtracker_direction_t direction;
    char *info_dir[] = {"None", "Fwd", "Rev", "Bidir"};

    if ((num_in_export_elements <= 0) || (in_export_elements == NULL)) {
        return BCM_E_PARAM;
    }
    if ((num_out_export_elements > 0) && (out_export_elements == NULL)) {
        return BCM_E_PARAM;
    }

    sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);

    /* Validate export element list of the group */
    rv = bcmi_ft_group_export_checks_validate(unit,
            flow_group_id, num_in_export_elements, in_export_elements);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&record, 0, sizeof(bcmi_fte_template_record_info_t));
    rv = bcmi_fte_template_record_info_populate(unit, flow_group_id, &record);
    BCM_IF_ERROR_RETURN(rv);

    /* Need to update initial record info for overlapping elements
       if applicable
     */
    rv = bcmi_fte_template_record_info_update(unit,
            num_in_export_elements, in_export_elements, &record);
    BCM_IF_ERROR_RETURN(rv);

    if (record.info_depth == 0) {
        /* Faied to populate template info */
        return BCM_E_INTERNAL;
    }

    /* Mark info elements not requested by user as 'Reserved' */
    for (idx = 0; idx < record.info_depth; idx++) {

        if (record.info[idx].info_elem !=
                bcmFlowtrackerExportElementTypeReserved) {

            bcmi_ft_export_element_info_for_record_find(unit,
                    flow_group_id, &record.info[idx], num_in_export_elements,
                    in_export_elements, mapped_export_elements, &found);

            if (found == TRUE) {
                in_c++;
            } else {
                record.info[idx].info_elem =
                    bcmFlowtrackerExportElementTypeReserved;
                record.info[idx].check_id = 0;
            }
        }
    }

    idx = 0;
    ix_out = 0;
    info_elem = bcmFlowtrackerExportElementTypeReserved;

    /* Form out export elements list */
    while(idx < record.info_depth) {

        check_id = 0;
        info_size = 0;
        direction = bcmFlowtrackerFlowDirectionNone;
        info_elem = record.info[idx].info_elem;
        custom_id = 0;

        if (info_elem == bcmFlowtrackerExportElementTypeReserved) {
            info_size = record.info[idx].length;

            /* Add up all adjacent 'Reserved' element */
            while(((idx + 1) < record.info_depth) &&
                    (record.info[idx + 1].info_elem ==
                     bcmFlowtrackerExportElementTypeReserved)) {
                info_size += record.info[idx + 1].length;
                idx++;
            }
        } else {
            info_size = record.info[idx].length;
            check_id = record.info[idx].check_id;
            direction = record.info[idx].direction;
            custom_id = record.info[idx].custom_id;

            /* Skip next $info_size 'Reserved' elements */
            idx += ((info_size / 8) - 1);
        }
        info_size = info_size / 8;

        /* Dump FT export element list */
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Id: %2d (%20s(%d), %5s)"
                        " | Size: %02d (0x%x)\n\r"), info_elem, info_name,
                    custom_id, info_dir[direction], info_size, check_id));

        info_elem_out = NULL;
        if (ix_out < num_out_export_elements) {
            info_elem_out = &(out_export_elements[ix_out]);

            info_elem_out->element = info_elem;
            info_elem_out->data_size = info_size;
            info_elem_out->check_id = check_id;
            info_elem_out->direction = direction;
            info_elem_out->custom_id = custom_id;
        }
        idx++;
        ix_out++;
     }

     *actual_out_export_elements = ix_out;

     if (in_c < num_in_export_elements) {
         /* Export element list contains extra elements */
         LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                         "In-Export Elements array contain"
                         " additional elements which are not present"
                         " in FT Group tracking list.\n\r")));
         for (idx = 0; idx < num_in_export_elements; idx++) {
             if (mapped_export_elements[idx] == FALSE) {
                 (void) bcmi_ft_export_elem_name_get(unit,
                         in_export_elements[idx].element, NULL, info_name);
                 LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                 "[%d] %s(%d) Check=%d dir=%d FOUND=%d\n"),
                             idx, info_name,
                             in_export_elements[idx].custom_id,
                             in_export_elements[idx].check_id,
                             in_export_elements[idx].direction,
                             mapped_export_elements[idx]));
             }
         }
         rv = BCM_E_CONFIG;
    } else if ((num_out_export_elements != 0) &&
            (num_out_export_elements < ix_out)) {

         LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                         "Size of Out-Export Elements array is insufficient."
                         " Required=(%d) but given Size=(%d)\n\r"),
                     ix_out, num_out_export_elements));
         rv = BCM_E_PARAM;
     }

    /* Cleanup table */
    if (record.info != NULL) {
        sal_free(record.info);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_export_template_validate
 * Purpose:
 *   Validate the template and return the list supported by the
 *   device.
 * Parameters:
 *   unit - (IN) Unit number.
 *   flow_group_id - (IN) Flowtracker flow group software ID.
 *   num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *   in_export_elements - (IN) List of export elements intended to be in the template.
 *   num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *   out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *   BCM_E_xxx
 * Notes:
 */
int
bcmi_ft_export_template_validate(int unit,
        bcm_flowtracker_group_t flow_group_id,
        int num_in_export_elements,
        bcm_flowtracker_export_element_info_t *in_export_elements,
        int num_out_export_elements,
        bcm_flowtracker_export_element_info_t *out_export_elements,
        int *actual_out_export_elements)
{
    int rv = BCM_E_NONE;
    int ft_group_installed = FALSE;

    if ((num_in_export_elements <= 0) || (in_export_elements == NULL)) {
        return BCM_E_PARAM;
    }
    if ((num_out_export_elements > 0) && (out_export_elements == NULL)) {
        return BCM_E_PARAM;
    }

    /* If FT Group is not created, fail */
    rv = bcmi_ft_group_is_invalid(unit, flow_group_id);
    BCM_IF_ERROR_RETURN(rv);

    /* If FT Group is installed, fail */
    if (BCMI_FT_GROUP_IS_BUSY(unit, flow_group_id)) {
        return BCM_E_BUSY;
    }

    /* If FT Group is validated, uninstall */
    if (BCMI_FT_GROUP_IS_VALIDATED(unit, flow_group_id)) {
        rv = bcmi_esw_ft_group_hw_uninstall(unit, flow_group_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Extract FT Group tracking params and flow checks */
    rv = _bcm_field_ft_group_tracking_info_process(unit, flow_group_id, 1);

    /* Validate and Install FT Group to Hw */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_ft_group_hw_install(unit, flow_group_id);
    }

    /* Validate and Create Export template */
    if (BCM_SUCCESS(rv)) {
        ft_group_installed = TRUE;
        rv = bcmi_fte_template_create_validate(unit,
                flow_group_id, num_in_export_elements, in_export_elements,
                num_out_export_elements, out_export_elements,
                actual_out_export_elements);
    }

    /* Cleanup in case of failure */
    if (BCM_FAILURE(rv)) {
        /* Uninstalled FT Group */
        if (ft_group_installed == TRUE) {
            (void) bcmi_esw_ft_group_hw_uninstall(unit, flow_group_id);
        }

        /* Cleanup FT Group Extraction information */
        (void) _bcm_field_ft_group_extraction_clear(unit, flow_group_id);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_template_create
 * Purpose:
 *   Create flowtracker export template.
 * Description :
 *   Routine to create a flowtracker template
 * Parameters:
 *   unit - (IN) BCM device id
 *   options - (IN) options for template creation
 *   template_id   - (OUT) FT template id.
 *   set_id - (IN) IPFIX template set id
 *   num_elements - (IN) Number of information elements.
 *   list_of_export_elements - (IN) List of IPFIX information elems.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_template_create(int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *template_id,
        uint16 set_id,
        int num_elements,
        bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    int rv;
    int actual_num_elements = 0;
    bcm_flowtracker_export_template_t index = 0;
    bcmi_ft_template_info_t template_info;

    /*
     * Validate info elems
     */
    rv = bcmi_ft_template_validate(unit,
                                   set_id, num_elements,
                                   list_of_export_elements,
                                   &actual_num_elements);
    BCM_IF_ERROR_RETURN(rv);

    /* If WITH_ID, reserve the ID */
    if (options & BCM_FLOWTRACKER_EXPORT_TEMPLATE_WITH_ID) {
        index = *template_id;

        rv = bcmi_ft_template_id_alloc(unit, FALSE, &index);
        BCM_IF_ERROR_RETURN(rv);

    } else {

        rv = bcmi_ft_template_id_alloc(unit, TRUE, &index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Init template_info. */
    sal_memset(&template_info, 0, sizeof(bcmi_ft_template_info_t));

    template_info.template_id = index;
    template_info.set_id = set_id;
    template_info.num_elements = actual_num_elements;

    sal_memcpy(template_info.export_elements,
               list_of_export_elements,
               (actual_num_elements * sizeof(bcm_flowtracker_export_element_info_t)));

    sal_memcpy(BCMI_FT_TEMPLATE_INFO(unit, index),
               &template_info, sizeof(template_info));

    *template_id = index;

    return rv;
}

/*
 * Function:
 *   bcmi_ft_template_get
 * Purpose:
 *   Fetch flowtracker export template.
 * Description :
 *   Routine to get flowtracker template info.
 * Parameters:
 *   unit - (IN) BCM device id
 *   template_id   - (IN) FT template id.
 *   *set_id - (OUT) IPFIX template set id
 *   max_size - (IN) Max num of elements
 *   list_of_export_elements - (OUT) List of IPFIX information elems.
 *   num_elements - (OUT) Actual Number of information elements.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_template_get(int unit,
        bcm_flowtracker_export_template_t template_id,
        uint16 *set_id,
        int max_elems,
        bcm_flowtracker_export_element_info_t *list_of_elems,
        int *num_elems)
{
    int idx;
    bcm_flowtracker_export_element_info_t *elem;
    bcmi_ft_template_info_t *template_info;

    if (max_elems != 0 && list_of_elems == NULL) {
        return BCM_E_PARAM;
    }
    if (num_elems == NULL) {
        return BCM_E_PARAM;
    }

    if ((template_id > BCMI_FT_EXPORT_INFO(unit)->max_templates) ||
        (template_id <= BCMI_FT_TEMPLATE_ID_INVALID)) {
        return BCM_E_PARAM;
    }

    if (!SHR_BITGET(BCMI_FT_TEMPLATE_BITMAP(unit), template_id)) {
        return BCM_E_NOT_FOUND;
    }

    template_info = BCMI_FT_TEMPLATE_INFO(unit, template_id);
    *set_id = template_info->set_id;
    *num_elems = template_info->num_elements;

    for (idx = 0; idx < max_elems; idx++) {
        elem = &(list_of_elems[idx]);
        sal_memcpy(elem,
                   &(template_info->export_elements[idx]),
                   sizeof(*elem));
    }

    if ((max_elems != 0) && (max_elems <= *num_elems)) {
        *num_elems = max_elems;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_template_destroy
 * Purpose:
 *   Destroy flowtracker export template.
 * Description :
 *   Routine to destroy a flowtracker template
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (OUT) FT template id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_template_destroy(int unit,
        bcm_flowtracker_export_template_t template_id)
{
    bcmi_ft_template_info_t *template_info;

    if ((template_id > BCMI_FT_EXPORT_INFO(unit)->max_templates) ||
        (template_id <= BCMI_FT_TEMPLATE_ID_INVALID)) {
        return BCM_E_PARAM;
    }

    if (!SHR_BITGET(BCMI_FT_TEMPLATE_BITMAP(unit), template_id)) {
        return BCM_E_NOT_FOUND;
    }

    template_info = BCMI_FT_TEMPLATE_INFO(unit, template_id);

    /* Allow destroying template only if it is detached from all groups. */
    if (template_info->in_use != 0) {
        return BCM_E_BUSY;
    }

    sal_memset(template_info->export_elements, 0,
               (template_info->num_elements *
                sizeof(bcm_flowtracker_export_element_info_t)));

    template_info->template_id = template_id;
    template_info->set_id = 0;
    template_info->num_elements = 0;

    /* Reset bit in template bitmap */
    SHR_BITCLR(BCMI_FT_TEMPLATE_BITMAP(unit), template_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_group_collector_add
 * Purpose:
 *   Attach a collector a flow group.
 * Description :
 *   Routine to associate a collector to an flowtracker group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   group_id- (IN) FT group Id.
 *   coll_hw_id   - (OUT) FT Collector Id.
 *   template_id - (IN) IPFIX template Id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_add(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t coll_hw_id,
        bcm_flowtracker_export_template_t template_id)
{
    uint8 group_ft_type;
    uint16 set_id;
    soc_mem_t mem = INVALIDm;
    soc_mem_t dg_mem = INVALIDm;
    bcmi_ft_collector_info_t *ft_coll_info;

    /*
     * Validate group id
     */
    if (bcmi_ft_group_is_invalid(unit, flow_group_id)) {
        return BCM_E_NOT_FOUND;
    }

    /* Validate Template Id. */
    if ((template_id > BCMI_FT_EXPORT_INFO(unit)->max_templates) ||
        (template_id <= BCMI_FT_TEMPLATE_ID_INVALID)) {
        return BCM_E_PARAM;
    }

    if (!SHR_BITGET(BCMI_FT_TEMPLATE_BITMAP(unit), template_id)) {
        return BCM_E_NOT_FOUND;
    }

    if ((coll_hw_id <= BCMI_FT_COLLECTOR_ID_INVALID) ||
        (coll_hw_id >= BCMI_FT_MAX_COLLECTORS)) {
        return BCM_E_PARAM;
    }

    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, coll_hw_id);

    if (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    if (BCMI_FT_TEMPLATE_ID_INVALID !=
        BCMI_FT_GROUP(unit, flow_group_id)->template_id) {
        return BCM_E_BUSY;
    }

    if (BCMI_FT_COLLECTOR_ID_INVALID !=
        BCMI_FT_GROUP(unit, flow_group_id)->collector_id) {
        return BCM_E_BUSY;
    }

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(flow_group_id)) {
        group_ft_type = BCMI_FT_COLLECTOR_FT_TYPE_AGGREGATE;
    } else {
        group_ft_type = BCMI_FT_COLLECTOR_FT_TYPE_MICRO;
    }

    if (ft_coll_info->ft_type != group_ft_type) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: Mismatch FT Type"
                      " in Group and Collector\r\n"), unit));
        return BCM_E_CONFIG;
    }

    /* Get KG Group table */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_type_mem_get(unit, 1,
                BCMI_FT_GROUP_TYPE_GET(flow_group_id), &mem));

    /* Get DG Group Table */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_type_mem_get(unit, 0,
                BCMI_FT_GROUP_TYPE_GET(flow_group_id), &dg_mem));

    /* Update Group table with collector Id. */
    soc_mem_field32_modify(unit, mem,
            BCMI_FT_GROUP_INDEX_GET(flow_group_id),
            COLLECTOR_IDf, coll_hw_id);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        soc_mem_field32_modify(unit, dg_mem,
                BCMI_FT_GROUP_INDEX_GET(flow_group_id),
                COLLECTOR_IDf, coll_hw_id);
    }
#endif

    ft_coll_info->in_use += 1;
    BCMI_FT_GROUP(unit, flow_group_id)->collector_id = coll_hw_id;

    /* Update Group table with template SET Id. */
    set_id = BCMI_FT_TEMPLATE_INFO(unit, template_id)->set_id;
    soc_mem_field32_modify(unit, mem,
            BCMI_FT_GROUP_INDEX_GET(flow_group_id),
            SET_IDf, set_id);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
            soc_feature(unit, soc_feature_flex_flowtracker_ver_2_setid_war)) {
        soc_mem_field32_modify(unit, dg_mem,
                BCMI_FT_GROUP_INDEX_GET(flow_group_id),
                SET_IDf, (set_id & 0x003f));
    }
#endif

    BCMI_FT_TEMPLATE_INFO(unit, template_id)->in_use += 1;
    BCMI_FT_GROUP(unit, flow_group_id)->template_id = template_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_group_collector_delete_check
 * Purpose:
 *   Check the conditions that needs to be satisfied
 *   before a collector is removed from Flowtracker_group.
 * Parameters:
 *   unit         - (IN) BCM device id
 *   id           - (IN) FT group Id.
 *   coll_hw_id   - (IN) FT Collector Id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_delete_check(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_collector_t coll_hw_id)
{
    int rv;
    int ft_enable;
    int is_busy;
    int user_entry = 0;
    uint32 flow_limit;
    uint64 age_out_count;
    uint64 flow_learnt_count;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "FLOWTRACKER(unit %d) Error: FT Group is"
                            " installed in Field Entry\r\n"), unit));
            return BCM_E_BUSY;
        }
        /* Check User entry configured */
        if (BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
            rv = bcmi_ftv3_aggregate_user_entry_get_all(unit,
                    id, 0, NULL, &user_entry);
            BCM_IF_ERROR_RETURN(rv);

            if (user_entry != 0) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "FLOWTRACKER(unit %d) Error: User entries are"
                                " installed to FT Group\r\n"), unit));
                return BCM_E_BUSY;
            }
        }
        return BCM_E_NONE;
    }
#endif

    user_entry = (BCMI_FT_GROUP(unit, id)->flags
                     & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY);
    is_busy = BCMI_FT_GROUP_IS_BUSY(unit, id);
    ft_enable = BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack;
    rv = bcmi_ft_group_flow_limit_get(unit, id, &flow_limit);
    if (rv != BCM_E_NONE) {
        flow_limit = 0;
    }

    /*
     * For sw managed groups, is_busy and ft_enable are valid, flow_limit is
     * not significant. Where as for hw managed groups, is_busy,ft_enable and
     * flow_limit are verified.
     */
    if (((user_entry == TRUE) && (is_busy == TRUE) && (ft_enable == TRUE)) ||
        ((is_busy == TRUE) && (ft_enable == TRUE) && (flow_limit != 0))) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: FT Group is installed in"
                      " Field Entry or Flowtracking is enabled\r\n"), unit));
        return BCM_E_BUSY;
    }

    if (SAL_BOOT_BCMSIM) {
        return BCM_E_NONE;
    }

    COMPILER_64_SET(age_out_count, 0, 0);
    COMPILER_64_SET(flow_learnt_count, 0, 0);
    BCM_IF_ERROR_RETURN(soc_counter_sync_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_AGE_OUT_CNT, id, &age_out_count));

    BCM_IF_ERROR_RETURN(soc_counter_sync_get(unit, REG_PORT_ANY,
        SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT, id, &flow_learnt_count));

    if (COMPILER_64_LT(age_out_count, flow_learnt_count)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: FT Group"
                      " has active flows\r\n"), unit));
        return BCM_E_BUSY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_group_collector_delete
 * Purpose:
 *   Detach a collector a flow group.
 * Description :
 *   Routine to dissociate a collector to an flowtracker group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   group_id- (IN) FT group Id.
 *   coll_hw_id   - (OUT) FT Collector Id.
 *   template_id - (IN) IPFIX template Id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_delete(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t coll_hw_id,
        bcm_flowtracker_export_template_t template_id)
{
    soc_mem_t mem = INVALIDm;
    soc_mem_t dg_mem = INVALIDm;
    bcmi_ft_collector_info_t *ft_coll_info;

    /*
     * Validate group id
     */
    if (bcmi_ft_group_is_invalid(unit, flow_group_id)) {
        return BCM_E_NOT_FOUND;
    }

    /* Validate Template Id. */
    if ((template_id > BCMI_FT_EXPORT_INFO(unit)->max_templates) ||
        (template_id <= BCMI_FT_TEMPLATE_ID_INVALID)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: Incorrect"
                      " template Id\r\n"), unit));
        return BCM_E_PARAM;
    }

    if (!SHR_BITGET(BCMI_FT_TEMPLATE_BITMAP(unit), template_id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: template Id"
                      " not found\r\n"), unit));
        return BCM_E_NOT_FOUND;
    }

    if ((coll_hw_id <= BCMI_FT_COLLECTOR_ID_INVALID) ||
        (coll_hw_id >= BCMI_FT_MAX_COLLECTORS)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: Incorrect"
                      " collector\r\n"), unit));
        return BCM_E_PARAM;
    }

    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, coll_hw_id);

    if (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: Collector"
                      " is not associated with Flowtracker Group \r\n"), unit));
        return BCM_E_NOT_FOUND;
    }

    if (template_id !=
        BCMI_FT_GROUP(unit, flow_group_id)->template_id) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: Template"
                      " is not associated with Flowtracker Group\r\n"), unit));
        return BCM_E_NOT_FOUND;
    }

    if (coll_hw_id !=
        BCMI_FT_GROUP(unit, flow_group_id)->collector_id) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: Collector"
                      " is not associated with Flowtracker Group \r\n"), unit));
        return BCM_E_NOT_FOUND;
    }

    /* Update Group table with collector Id. */
    BCM_IF_ERROR_RETURN(bcmi_ft_flow_group_collector_delete_check(unit,
                flow_group_id, coll_hw_id));

    /* Get KG Group memory */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_type_mem_get(unit, 1,
                BCMI_FT_GROUP_TYPE_GET(flow_group_id), &mem));

    /* Get DG Group memory */
    BCM_IF_ERROR_RETURN(bcmi_ft_group_type_mem_get(unit, 0,
                BCMI_FT_GROUP_TYPE_GET(flow_group_id), &dg_mem));

    soc_mem_field32_modify(unit, mem,
                BCMI_FT_GROUP_INDEX_GET(flow_group_id),
                COLLECTOR_IDf, 0);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        soc_mem_field32_modify(unit, dg_mem,
                BCMI_FT_GROUP_INDEX_GET(flow_group_id),
                COLLECTOR_IDf, 0);
    }
#endif

    ft_coll_info->in_use -= 1;
    BCMI_FT_GROUP(unit, flow_group_id)->collector_id =
        BCMI_FT_COLLECTOR_ID_INVALID;

    /* Update Group table with template SET Id. */
    soc_mem_field32_modify(unit, mem,
            BCMI_FT_GROUP_INDEX_GET(flow_group_id),
            SET_IDf, 0);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2) &&
            soc_feature(unit, soc_feature_flex_flowtracker_ver_2_setid_war)) {
        soc_mem_field32_modify(unit, dg_mem,
                BCMI_FT_GROUP_INDEX_GET(flow_group_id), SET_IDf, 0);
    }
#endif

    BCMI_FT_TEMPLATE_INFO(unit, template_id)->in_use -= 1;
    BCMI_FT_GROUP(unit, flow_group_id)->template_id =
        BCMI_FT_TEMPLATE_ID_INVALID;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_group_collector_get_all
 * Purpose:
 *   Get collector attached to a flow group.
 * Description :
 *   Routine to fetch collector and template id
 *   associated a flowtracker group.
 * Parameters:
 *   unit                 - (IN)  BCM device id
 *   group_id             - (IN)  FT group Id.
 *   max_list_size        - (IN)  max list of collectors
 *   list_of_collectors   - (OUT) FT Collector Id list.
 *   list_of_templates    - (OUT) FT Template Id list.
 *   list_size            - (IN)  Actual list size.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_get_all(int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_list_size,
        bcm_flowtracker_collector_t *list_of_collectors,
        bcm_flowtracker_export_template_t *list_of_templates,
        int *list_size)
{
    if (list_size == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * Validate group id
     */
    if (bcmi_ft_group_is_invalid(unit, flow_group_id)) {
        return BCM_E_NOT_FOUND;
    }

    /* HX5 Hardware supports only one collector id and templat per group */
    if (max_list_size > 0) {
        if (list_of_collectors != NULL) {
            list_of_collectors[0] = BCMI_FT_GROUP(unit, flow_group_id)->collector_id;
        }

        if (list_of_templates != NULL) {
            list_of_templates[0] = BCMI_FT_GROUP(unit, flow_group_id)->template_id;
        }
    }

    *list_size = 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_collector_attach
 * Purpose:
 *   Associate flow group to a collector with an export template.
 * Parameters:
 *   unit            - (IN)  BCM device number
 *   flow_group_id   - (IN)  Flow group Id
 *   collector_id    - (IN)  Collector Id
 *   template_id     - (IN)  Template Id
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_group_collector_attach(int unit,
            bcm_flowtracker_group_t flow_group_id,
            bcm_collector_t collector_id,
            int export_profile_id,
            bcm_flowtracker_export_template_t template_id)
{
    int rv;
    int new_created = FALSE;
    int group_collector_add = FALSE;
    bcmi_ft_collector_info_t *ft_coll_info;
    bcm_flowtracker_collector_t ix, first_free;

    bcm_flowtracker_collector_info_t collector_info;
    bcm_collector_export_profile_t export_profile_info;
    bcm_collector_info_t cmn_collector_info;

    /* Validate Flowtracker Group */
    if (bcmi_ft_group_is_invalid(unit, flow_group_id)) {
        return BCM_E_NOT_FOUND;
    }

    first_free = BCMI_FT_COLLECTOR_ID_INVALID;

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);
        if ((first_free == BCMI_FT_COLLECTOR_ID_INVALID) &&
            (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID)) {
            first_free = ix;
        }

        if (ft_coll_info->cmn_coll_id == collector_id) {
            if (ft_coll_info->cmn_export_prof_id != export_profile_id) {
                /* Export profile associated earlier is not same */
                return BCM_E_PARAM;
            }
            break;
        }
    }

    if (ix == BCMI_FT_EXPORT_INFO(unit)->max_collectors) {
        if (first_free == BCMI_FT_COLLECTOR_ID_INVALID) {
            /* Free hw collector not available. */
            return BCM_E_RESOURCE;
        }

        /* Collector not installed yet */
        bcm_collector_info_t_init(&cmn_collector_info);
        bcm_collector_export_profile_t_init(&export_profile_info);
        bcm_flowtracker_collector_info_t_init(&collector_info);

        rv = _bcm_xgs5_collector_export_profile_get(unit,
                                                    export_profile_id,
                                                    &export_profile_info);
        BCM_IF_ERROR_RETURN(rv);

        if (export_profile_info.wire_format != bcmCollectorWireFormatIpfix) {
            return BCM_E_CONFIG;
        }

        if (_bcm_esw_collector_check_user_is_other(unit, collector_id,
                    _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "FLOWTRACKER(unit %d) Error: Collector(%d)"
                            " is associated with other app\n\r"), unit,
                        collector_id));
            return BCM_E_CONFIG;
        }

        rv = _bcm_xgs5_collector_get(unit, collector_id, &cmn_collector_info);
        BCM_IF_ERROR_RETURN(rv);

        switch (cmn_collector_info.collector_type) {
            case bcmCollectorLocal:
                collector_info.collector_type = bcmFlowtrackerCollectorLocal;
                break;
            case bcmCollectorRemote:
                collector_info.collector_type = bcmFlowtrackerCollectorRemote;
                break;
            default:
                return (BCM_E_PARAM);
        }

        if ((collector_info.collector_type == bcmFlowtrackerCollectorLocal) &&
                (BCMI_FT_EXPORT_INFO(unit)->dma_collector_id != BCMI_FT_COLLECTOR_ID_INVALID)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "FLOWTRACKER Error: Only one local collector can be"
                            " attached to flowtracker group at a time\n\r")));
            return (BCM_E_RESOURCE);
        }

        switch (cmn_collector_info.transport_type) {
            case bcmCollectorTransportTypeIpv4Udp:
                collector_info.transport_type = bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp;
                break;
            case bcmCollectorTransportTypeIpv6Udp:
                collector_info.transport_type = bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp;
                break;
            case bcmCollectorTransportTypeRaw:
                collector_info.transport_type = bcmFlowtrackerCollectorTransportTypeRaw;
                break;
            default:
                return (BCM_E_PARAM);
        }

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            switch (cmn_collector_info.src_ts) {
                case bcmCollectorTimestampSourceLegacy:
                    collector_info.src_ts = bcmFlowtrackerTimestampSourceLegacy;
                    break;
                case bcmCollectorTimestampSourceNTP:
                    collector_info.src_ts = bcmFlowtrackerTimestampSourceNTP;
                    break;
                case bcmCollectorTimestampSourcePTP:
                    collector_info.src_ts = bcmFlowtrackerTimestampSourcePTP;
                    break;
                default:
                    return (BCM_E_PARAM);
            }
        }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

        sal_memcpy(&(collector_info.eth.src_mac),
                   &(cmn_collector_info.eth.src_mac), sizeof(bcm_mac_t));
        sal_memcpy(&(collector_info.eth.dst_mac),
                   &(cmn_collector_info.eth.dst_mac), sizeof(bcm_mac_t));
        collector_info.eth.vlan_tag_structure = cmn_collector_info.eth.vlan_tag_structure;
        collector_info.eth.outer_tpid = cmn_collector_info.eth.outer_tpid;
        collector_info.eth.inner_tpid = cmn_collector_info.eth.inner_tpid;
        collector_info.eth.outer_vlan_tag = cmn_collector_info.eth.outer_vlan_tag;
        collector_info.eth.inner_vlan_tag = cmn_collector_info.eth.inner_vlan_tag;

        collector_info.ipv4.src_ip = cmn_collector_info.ipv4.src_ip;
        collector_info.ipv4.dst_ip = cmn_collector_info.ipv4.dst_ip;
        collector_info.ipv4.dscp = cmn_collector_info.ipv4.dscp;
        collector_info.ipv4.ttl = cmn_collector_info.ipv4.ttl;

        sal_memcpy(&(collector_info.ipv6.src_ip),
                   &(cmn_collector_info.ipv6.src_ip), sizeof(bcm_ip6_t));
        sal_memcpy(&(collector_info.ipv6.dst_ip),
                   &(cmn_collector_info.ipv6.dst_ip), sizeof(bcm_ip6_t));
        collector_info.ipv6.traffic_class = cmn_collector_info.ipv6.traffic_class;
        collector_info.ipv6.flow_label = cmn_collector_info.ipv6.flow_label;
        collector_info.ipv6.hop_limit = cmn_collector_info.ipv6.hop_limit;

        collector_info.udp.dst_port = cmn_collector_info.udp.dst_port;
        collector_info.udp.src_port = cmn_collector_info.udp.src_port;

        ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, first_free);
        ft_coll_info->ipfix_hdr_use_custom = 1;
        ft_coll_info->ipfix_hdr = cmn_collector_info.ipfix;

        if ((cmn_collector_info.max_records_reserve < 0) ||
                (cmn_collector_info.max_records_reserve >
                 BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                            "FLOWTRACKER(unit %d) Error: Requested records"
                            " to reserve (%d) for collector exceeds available"
                            " limits%s\n"), unit,
                        cmn_collector_info.max_records_reserve, bcm_errmsg(rv)));
            return BCM_E_CONFIG;
        }
        ft_coll_info->max_records_reserve = cmn_collector_info.max_records_reserve;

        /* Assign Ft type */
        if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(flow_group_id)) {
            ft_coll_info->ft_type = BCMI_FT_COLLECTOR_FT_TYPE_AGGREGATE;
        } else {
            ft_coll_info->ft_type = BCMI_FT_COLLECTOR_FT_TYPE_MICRO;
        }

        /* Get length */
        if (export_profile_info.flags &
            BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
            ft_coll_info->num_records = export_profile_info.num_records;
        } else if (export_profile_info.max_packet_length != 0) {
            collector_info.max_packet_length =
                        export_profile_info.max_packet_length;
        } else {
            return BCM_E_PARAM;
        }

        if (export_profile_info.export_interval !=
                BCM_COLLECTOR_EXPORT_INTERVAL_INVALID) {
            ft_coll_info->export_interval = export_profile_info.export_interval;
        }

        rv = bcmi_ft_collector_create(unit,
                                      BCM_FLOWTRACKER_COLLECTOR_WITH_ID,
                                      &first_free,
                                      &collector_info);
        BCM_IF_ERROR_RETURN(rv);

        new_created = TRUE;
        ft_coll_info->cmn_coll_id = collector_id;
        ft_coll_info->cmn_export_prof_id = export_profile_id;

        if (collector_info.collector_type == bcmFlowtrackerCollectorLocal) {
            BCMI_FT_EXPORT_INFO(unit)->dma_collector_id = collector_id;
        }
        ix = first_free;
    }

    rv = bcmi_ft_flow_group_collector_add(unit,
                                          flow_group_id,
                                          ix,
                                          template_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "FLOWTRACKER(unit %d) Error: collector and"
                        " template add failed  %s\n"), unit, bcm_errmsg(rv)));
        goto cleanup;
    }
    group_collector_add = TRUE;

    rv = _bcm_esw_collector_ref_count_update(unit, collector_id, 1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: collector "
                      " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
        goto cleanup;
    }
    rv = _bcm_esw_collector_export_profile_ref_count_update(unit,
                                                            export_profile_id,
                                                            1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: export profile "
                      " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
        (void) _bcm_esw_collector_ref_count_update(unit, collector_id, -1);
        goto cleanup;
    }
    rv = _bcm_esw_collector_user_update(unit, collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: collector "
                      " user update failed  %s\n"), unit, bcm_errmsg(rv)));
        (void) _bcm_esw_collector_ref_count_update(unit, collector_id, -1);
        (void) _bcm_esw_collector_export_profile_ref_count_update(unit, export_profile_id, -1);
        goto cleanup;
    }

    return BCM_E_NONE;

cleanup:

    /* If Attached to Group, detach it */
    if (group_collector_add == TRUE) {
        (void) bcmi_ft_flow_group_collector_delete(unit,
                flow_group_id, first_free, template_id);
    }
    /* If collector was created, destroy it */
    if (new_created == TRUE) {
        (void) bcmi_ft_collector_destroy(unit, first_free);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_collector_detach
 * Purpose:
 *   Dis-associate flow group from a collector with an export template.
 * Parameters:
 *   unit            - (IN)  BCM device number
 *   flow_group_id   - (IN)  Flow group Id
 *   collector_id    - (IN)  Collector Id
 *   template_id     - (IN)  Template Id
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_group_collector_detach(int unit,
            bcm_flowtracker_group_t flow_group_id,
            bcm_flowtracker_collector_t collector_id,
            int export_profile_id,
            bcm_flowtracker_export_template_t template_id)
{
    int rv;
    bcmi_ft_collector_info_t *ft_coll_info = NULL;
    bcm_flowtracker_collector_t ix;

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        if (ft_coll_info->cmn_coll_id == collector_id) {
            if (ft_coll_info->cmn_export_prof_id != export_profile_id) {
                /* Export profile associated earlier is not same */
                return BCM_E_PARAM;
            }
            break;
        }
    }

    if (ix == BCMI_FT_EXPORT_INFO(unit)->max_collectors) {
        return BCM_E_NOT_FOUND;
    }

    rv = bcmi_ft_flow_group_collector_delete(unit,
                                             flow_group_id,
                                             ix,
                                             template_id);
    BCM_IF_ERROR_RETURN(rv);

    if (ft_coll_info->in_use == 0) {
        /* Destroy collector if not attached to any group. */
        rv = bcmi_ft_collector_destroy(unit, ix);
        BCM_IF_ERROR_RETURN(rv);

    }

    rv = _bcm_esw_collector_ref_count_update(unit, collector_id, -1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: collector "
                      " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
        return rv;
    }
    rv = _bcm_esw_collector_export_profile_ref_count_update(unit,
                                                            export_profile_id,
                                                            -1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: export profile "
                      " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_collector_attach_get_all
 * Purpose:
 *   Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *   unit                - (IN)  BCM device number
 *   flow_group_id       - (IN)  Flow group Id
 *   max_list_size       - (IN)  Size of the list arrays
 *   list_of_collectors  - (OUT) Collector list
 *   list_of_templates   - (OUT) Template list
 *   list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_group_collector_attach_get_all(int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_list_size,
        bcm_flowtracker_collector_t *list_of_collectors,
        int *list_of_export_profile_ids,
        bcm_flowtracker_export_template_t *list_of_templates,
        int *list_size)
{
    bcm_flowtracker_collector_t hw_coll_id;
    bcmi_ft_collector_info_t *ft_coll_info = NULL;

    if (bcmi_ft_group_is_invalid(unit, flow_group_id)) {
        return BCM_E_NOT_FOUND;
    }

    /* max_list_size == 0, just populate list_size and return */
    if (max_list_size == 0) {
        if (BCMI_FT_GROUP(unit, flow_group_id)->collector_id ==
                BCMI_FT_COLLECTOR_ID_INVALID) {
            *list_size = 0;
        } else {
            /* Only one collector and template can be attached to a group */
            *list_size = 1;
        }
        return BCM_E_NONE;
    }

    if (BCMI_FT_COLLECTOR_ID_INVALID ==
            BCMI_FT_GROUP(unit, flow_group_id)->collector_id) {
        *list_size = 0;
        return BCM_E_NONE;
    }

    /* Get the collector info */
    hw_coll_id = BCMI_FT_GROUP(unit, flow_group_id)->collector_id;
    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, hw_coll_id);

    *list_size                    = 1;
    list_of_templates[0]          = BCMI_FT_GROUP(unit, flow_group_id)->template_id;
    list_of_collectors[0]         = ft_coll_info->cmn_coll_id;
    list_of_export_profile_ids[0] = ft_coll_info->cmn_export_prof_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_export_record_cb_install
 * Purpose:
 *   Install callback function entry
 * Parameters:
 *   unit            - (IN)  BCM device number
 *   collector_id    - (IN)  Collector Id
 *   callback_source - (IN)  Caller function
 *   entry           - (IN)  Callback function entry
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_export_record_cb_install(int unit,
        bcm_flowtracker_collector_t collector_id,
        int callback_source,
        bcmi_ft_cb_entry_t *entry)
{
    bcmi_ft_cb_entry_t *curr_entry = NULL;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl = NULL;

    ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
    if (ft_export_cb_ctrl == NULL) {
        return BCM_E_INIT;
    }
    curr_entry = &ft_export_cb_ctrl->callback_entry[callback_source];

    if (!((curr_entry->state == BCMI_FT_CALLBACK_STATE_INVALID) ||
                (ft_export_cb_ctrl->pid == SAL_THREAD_ERROR &&
                 curr_entry->state == BCMI_FT_CALLBACK_STATE_UNREGISTERED))) {

        if ((curr_entry->callback == entry->callback) &&
                (curr_entry->callback_options.callback_prio ==
                 entry->callback_options.callback_prio)) {
            return BCM_E_NONE;
        }

        /* Callback is already registered */
        return BCM_E_RESOURCE;
    }

    curr_entry->callback = entry->callback;
    curr_entry->callback_options = entry->callback_options;
    curr_entry->userdata = entry->userdata;
    curr_entry->state = BCMI_FT_CALLBACK_STATE_REGISTERED;

    return BCM_E_NONE;
}


/*
 * Function:
 *   bcmi_ft_export_record_cb_remove
 * Purpose:
 *   Remove callback function entry
 * Parameters:
 *   unit            - (IN)  BCM device number
 *   collector_id    - (IN)  Collector Id
 *   callback_source - (IN)  Caller function
 *   entry           - (IN)  Callback function entry
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */

int
bcmi_ft_export_record_cb_remove(int unit,
    bcm_flowtracker_collector_t collector_id,
    int callback_source,
    bcmi_ft_cb_entry_t *entry)
{
    bcmi_ft_cb_entry_t *curr_entry = NULL;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl = NULL;

    ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
    if (ft_export_cb_ctrl == NULL) {
        return BCM_E_INIT;
    }
    curr_entry = &ft_export_cb_ctrl->callback_entry[callback_source];

    if ((curr_entry->state == BCMI_FT_CALLBACK_STATE_ACTIVE) ||
            (curr_entry->state == BCMI_FT_CALLBACK_STATE_REGISTERED)) {

        /* If callback entry is not provided, unregister all */
        if (entry->callback == NULL) {
            curr_entry->state = BCMI_FT_CALLBACK_STATE_UNREGISTERED;
            return BCM_E_NONE;
        }

        /* If callback entry is provided, unregister matched */
        if ((curr_entry->callback == entry->callback) &&
                (curr_entry->callback_options.callback_prio ==
                 entry->callback_options.callback_prio)) {
            curr_entry->state = BCMI_FT_CALLBACK_STATE_UNREGISTERED;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_export_record_register
 * Description:
 *   To register the callback function for flow info export
 * Parameters:
 *   unit          device number
 *   callback      user callback function
 *   userdata      user data used as argument during callback
 * Return:
 *   BCM_E_NONE
 *   BCM_E_PARAM
 *   BCM_E_RESOURCE
 *   BCM_E_UNAVAIL
 */
int
bcmi_ft_export_record_register(int unit,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_collector_callback_options_t callback_options,
    bcm_flowtracker_export_record_cb_f callback_fn,
    void *userdata)
{
    int rv = BCM_E_NONE;
    bcmi_ft_cb_entry_t entry;

    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return BCM_E_UNAVAIL;
    }

    if (callback_fn == NULL) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_CB_CTRL(unit) == NULL) {
        BCM_IF_ERROR_RETURN(bcmi_ft_export_cb_init(unit));
    }

    if (BCMI_FT_EXPORT_INFO(unit)->dma_collector_id != collector_id) {
        return BCM_E_PARAM;
    }

    sal_memset(&entry, 0, sizeof(entry));
    entry.callback = callback_fn;
    entry.callback_options = callback_options;
    entry.userdata = userdata;

    rv = bcmi_ft_export_record_cb_install(unit, collector_id, 1, &entry);
    BCM_IF_ERROR_RETURN(rv);

    (void)bcma_ft_export_fifo_control(unit, 1, TRUE);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_export_record_unregister
 * Description:
 *   To unregister the callback function for flow info export
 * Parameters:
 *   unit          device number
 *   callback      user callback function
 *   userdata      user data used as argument during callback
 * Return:
 *   BCM_E_NONE
 *   BCM_E_NOT_FOUND
 *   BCM_E_UNAVAIL
 *   BCM_E_INIT
 */
int
bcmi_ft_export_record_unregister(int unit,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_collector_callback_options_t callback_options,
    bcm_flowtracker_export_record_cb_f callback_fn)
{
    int rv = BCM_E_NONE;
    bcmi_ft_cb_entry_t entry;

    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return BCM_E_UNAVAIL;
    }

    if (BCMI_FT_CB_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }
    sal_memset(&entry, 0, sizeof(entry));
    entry.callback = callback_fn;
    entry.callback_options = callback_options;
    entry.userdata = NULL;

    rv = bcmi_ft_export_record_cb_remove(unit, collector_id, 1, &entry);
    BCM_IF_ERROR_RETURN(rv);

    (void) bcma_ft_export_fifo_control(unit, 1, FALSE);
    return rv;
}

STATIC int
_bcm_ft_process_export_entry(int unit,
                             void *entry,
                             bcm_flowtracker_export_record_t *data)
{
    int ix, count, num_words;
    static int first_half = 1;
    static uint8 data_rec[BCM_FLOWTRACKER_EXPORT_RECORD_LENGTH] = {0};
    uint32 *entry_p, *data_p;

    count = (BCM_FLOWTRACKER_EXPORT_RECORD_LENGTH / 2);
    num_words = count / 4;

    entry_p = entry;

    if (first_half) {
        data_p = (uint32 *) &(data_rec[0]);
        for (ix = 0; ix < num_words; ix++) {
            data_p[num_words - ix - 1] = soc_htonl(entry_p[ix]);
        }
        first_half = 0;

        return SOC_E_BUSY;
    } else {

        data_p = (uint32 *) &(data_rec[count]);
        for (ix = 0; ix < num_words; ix++) {
            data_p[num_words - ix - 1] = soc_htonl(*entry_p);
            entry_p++;
        }

        sal_memset(data, 0, sizeof(*data));
        data->size = BCM_FLOWTRACKER_EXPORT_RECORD_LENGTH;
        sal_memcpy(data->data_record, data_rec,
                   data->size);

        first_half = 1;
        sal_memset(&data_rec[0], 0, (BCM_FLOWTRACKER_EXPORT_RECORD_LENGTH));

        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_export_dma
 * Description:
 *   A task thread to process IPFIX export FIFOs via FIFO DMA
 * Parameters:
 *   unit_vp          Device number
 * Return:
 *   None
 */
STATIC void
bcmi_ft_export_dma(void *unit_vp)
{
    soc_mem_t export_fifo;
    bcmi_ft_cb_entry_t *cb_entry;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl;
    bcm_flowtracker_export_record_t export_record;

    int unit;
    int handled;
    int rv, cb_rv;
    int entries_per_buf, adv_threshold;
    int interval, count, i, j, non_empty;
    int chan, entry_words;
    uint32 *buff_max;
    void *host_buf, *export_entry;
    uint32 rval, hostmem_timeout, overflow_ch, done, error, fail;

    unit = PTR_TO_INT(unit_vp);
    ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
    export_fifo = BSC_EX_EXPORT_FIFOm;

    entries_per_buf = soc_property_get(unit,
                          spn_FLOWTRACKER_EXPORT_FIFO_HOSTBUF_SIZE, 1024);
    adv_threshold = entries_per_buf / 2;

    chan = ft_export_cb_ctrl->dma_chan;

    entry_words = soc_mem_entry_words(unit, export_fifo);
    host_buf = soc_cm_salloc(unit, entries_per_buf * entry_words * sizeof(uint32),
                      "FT EXPORT DMA Buffer");

    if (host_buf == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_IPFIX, __LINE__,
                           BCM_E_MEMORY);
        goto cleanup_exit;
    }

    soc_fifodma_stop(unit, chan);

    rv = soc_fifodma_masks_get(unit, &hostmem_timeout, &overflow_ch, &error, &done);
    if (SOC_FAILURE(rv)) {
        goto cleanup_exit;
    }

    rv = soc_fifodma_start(unit, chan, TRUE, export_fifo, 0,
                               MEM_BLOCK_ANY, 0, entries_per_buf,
                               host_buf);
    if (BCM_FAILURE(rv)) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_IPFIX, __LINE__, rv);
        goto cleanup_exit;
    }

    export_entry = host_buf;
    buff_max = (uint32 *)export_entry + (entries_per_buf * entry_words);


    while ((interval = ft_export_cb_ctrl->interval)) {
        fail = FALSE;
        if (SOC_CONTROL(unit)->ftExportIntrEnb) {

            soc_fifodma_intr_enable(unit, chan);


            if (sal_sem_take(SOC_CONTROL(unit)->ftExportIntr, interval) < 0) {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "FT Export DMA polling timeout\n")));
            } else {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "FT Export DMA woken up\n")));

                soc_fifodma_status_get(unit, chan, &rval);
                fail = ((rval & hostmem_timeout) | (rval & overflow_ch));
            }
        } else {
            /* Polling mode: Sleep for thread interval */
            sal_usleep(interval);
        }

        /* Update current Callback Function Status */
        for (i = 0; i < BCMI_FT_CB_MAX; i++) {
            cb_entry = &ft_export_cb_ctrl->callback_entry[i];
            switch (cb_entry->state) {
            case BCMI_FT_CALLBACK_STATE_REGISTERED:
                cb_entry->state = BCMI_FT_CALLBACK_STATE_ACTIVE;
                break;
            case BCMI_FT_CALLBACK_STATE_UNREGISTERED:
                cb_entry->state = BCMI_FT_CALLBACK_STATE_INVALID;
                break;
            default:
                break;
            }
        }


        do {
            non_empty = FALSE;

            rv = soc_fifodma_num_entries_get(unit, chan, &count);
            if (SOC_SUCCESS(rv)) {
                non_empty = TRUE;
                if (count > adv_threshold) {
                    count = adv_threshold;
                }

                /* Invalidate DMA memory to read */
                if (((uint32 *)export_entry + count * entry_words) > buff_max) {
                    /* roll-over cases */
                    soc_cm_sinval(unit, export_entry,
                        (buff_max - (uint32 *)export_entry) * sizeof(uint32));
                    soc_cm_sinval(unit, host_buf,
                        ((count * entry_words) - (buff_max - (uint32 *)export_entry)) * sizeof(uint32));
                }
                else {
                    soc_cm_sinval(unit, export_entry, (count * entry_words) * sizeof(uint32));
                }


                for (i = 0; i < count; i++) {

                    rv = _bcm_ft_process_export_entry(unit, export_entry, &export_record);

                    export_entry = (uint32 *)export_entry + entry_words;

                    /* From the begining of host buffer */
                    if ((((uint32 *)export_entry - (uint32 *)host_buf) /
                         entry_words) == entries_per_buf) {
                        export_entry = host_buf;
                    }

                    if (rv < 0) {
                        /* Skip calling CB functions for
                         *  first half records - i.e, when rv != SOC_E_NONE.
                         */
                        continue;
                    }


                    handled = FALSE;

                    for (j = 0; j < BCMI_FT_CB_MAX; j++) {
                        cb_entry = &ft_export_cb_ctrl->callback_entry[j];
                        if (cb_entry->state == BCMI_FT_CALLBACK_STATE_ACTIVE) {
                            cb_rv = cb_entry->callback(unit,
                                BCMI_FT_EXPORT_INFO(unit)->dma_collector_id,
                                &export_record, cb_entry->userdata);

                            switch (cb_rv) {
                            case BCM_FLOWTRACKER_EXPORT_RECORD_NOT_HANDLED:
                                break;
                            case BCM_FLOWTRACKER_EXPORT_RECORD_HANDLED:
                                handled = TRUE;
                                break;
                            case BCM_FLOWTRACKER_EXPORT_RECORD_HANDLED_OWNED:
                                handled = TRUE;
                                BCMI_FT_CB_CTRL(unit)->owned++;
                                break;
                            default:
                                BCMI_FT_CB_CTRL(unit)->bad_handled++;
                                break;
                            }

                            if (handled) {
                                break;
                            }
                        }
                    }
                }

                /* Mark entries read in fifo dma channel */
                soc_fifodma_set_entries_read(unit, chan, count);
            }

            /* check and clear error */
            soc_fifodma_status_get(unit, chan, &rval);
            if (rval & done) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FIFO DMA engine terminated for chan[%d]\n"),
                           chan));
                if (rval & error) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "FIFO DMA engine encountered error: [0x%x]\n"),
                               rval));
                }
                goto cleanup_exit;
            }

        } while (non_empty);

        if (fail) {
            /* Clearing of the FIFO_CH_DMA_INT interrupt by resetting
             * overflow & timeout status in FIFO_CHx_RD_DMA_STAT_CLR reg
             */
            soc_fifodma_status_clear(unit, chan);
        }
    }

cleanup_exit:
    (void)soc_mem_fifo_dma_stop(unit, chan);

    if (host_buf != NULL) {
        soc_cm_sfree(unit, host_buf);
    }
    ft_export_cb_ctrl->pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}


/* Functions called from diag shell */
/*
 * Function:
 *   bcma_ft_export_fifo_control
 * Description:
 *   To start/stop ipfix export fifo processing thread
 * Parameters:
 *   unit            device number
 *   callback_source callback source
 *   enable          enable/disable
 * Return:
 *   BCM_E_NONE
 *   BCM_E_MEMORY
 *   BCM_E_UNAVAIL
 */
int
bcma_ft_export_fifo_control(int unit, int callback_source, int enable)
{
    sal_usecs_t interval;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
    int pri;
    char name[16];

    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return BCM_E_UNAVAIL;
    }

    if (ft_export_cb_ctrl == NULL) {
        return BCM_E_INTERNAL;
    }

    sal_snprintf(name, sizeof(name), "bcmFtExportDma.%d", unit);

    interval = 1000000;
    if (enable == FALSE) {
        if (callback_source == -1) {
            interval = 0;
            ft_export_cb_ctrl->callback_sources = 0;
        } else {
            ft_export_cb_ctrl->callback_sources -= 1;
            if (ft_export_cb_ctrl->callback_sources <= 0) {
                interval = 0;
                ft_export_cb_ctrl->callback_sources = 0;
            }
        }
    } else {
        ft_export_cb_ctrl->callback_sources += 1;
    }

    ft_export_cb_ctrl->interval = interval;

    if (enable) {
        if (ft_export_cb_ctrl->pid == SAL_THREAD_ERROR) {
            pri = soc_property_get(unit,
                      spn_FLOWTRACKER_EXPORT_FIFO_THREAD_PRI, 100);
            ft_export_cb_ctrl->pid = sal_thread_create(name,
                                                       SAL_THREAD_STKSZ,
                                                       pri,
                                                       bcmi_ft_export_dma,
                                                       INT_TO_PTR(unit));
            if (ft_export_cb_ctrl->pid == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                          (BSL_META_U(unit,
                                      "Could not start thread\n")));
                return BCM_E_MEMORY;
            }
        }
    } else {
        /* Wake up thread so it will check the changed interval value */
        sal_sem_give(SOC_CONTROL(unit)->ftExportIntr);
    }

    return BCM_E_NONE;
}

int
bcma_ft_export_dma_collector_get(int unit,
                                 bcm_collector_t *id)
{
    bcm_collector_t collector_id;

    collector_id = BCMI_FT_EXPORT_INFO(unit)->dma_collector_id;

    if (collector_id == BCMI_FT_COLLECTOR_ID_INVALID) {
        return BCM_E_PARAM;
    }

    *id = collector_id;

    return BCM_E_NONE;
}

int
bcma_ft_setid_template_id_get(int unit, uint16 setid,
                              bcm_flowtracker_export_template_t *template_id)
{
    int ix;
    bcmi_ft_template_info_t *template_info;

    for (ix = 0; ix < BCMI_FT_MAX_TEMPLATES; ix++) {

        template_info = BCMI_FT_TEMPLATE_INFO(unit, ix);

        if (template_info == NULL) {
            continue;
        }

        if (setid == template_info->set_id) {
            break;
        }
    }

    if (ix < BCMI_FT_MAX_TEMPLATES) {
        *template_id = ix;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCMI_FT_WB_SCACHE_SIZE(_data_)  sizeof(*(_data_))

#define BCMI_FT_WB_SCACHE_SYNC(_scache_, _data_)            \
    do {                                                    \
        sal_memcpy((_scache_), (_data_), sizeof(*(_data_)));\
        (_scache_) +=  sizeof(*(_data_));                   \
    } while (0)

#define BCMI_FT_WB_SCACHE_RECOVER(_scache_, _data_)         \
    do {                                                    \
        sal_memcpy((_data_), (_scache_), sizeof(*(_data_)));\
        (_scache_) +=  sizeof(*(_data_));                   \
    } while (0)

/*
 * Function:
 *   bcmi_ft_export_wb_alloc_size
 * Purpose:
 *   Allocate persistent info memory for flowtracker module
 * Parameters:
 *   unit - StrataSwitch unit number.
 * Returns:
 *   BCM_E_XXX
 */
STATIC void
bcmi_ft_export_wb_alloc_size(int unit, int *size)
{
    int ix, jx;
    int alloc_size;
    bcmi_ft_collector_info_t *coll_info;
    bcmi_ft_template_info_t *template_info;

    alloc_size = 0;

    /* DMA collector Id */
    alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(BCMI_FT_EXPORT_INFO(unit)->dma_collector_id));

    /* Sync collector_info structs. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->cmn_coll_id));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->cmn_export_prof_id));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->coll_hw_id));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->in_use));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->collector_info));
    }

    /* Sync template_info structs. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_templates; ix++) {
        template_info = BCMI_FT_TEMPLATE_INFO(unit, ix);

        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(template_info->template_id));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(template_info->in_use));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(template_info->set_id));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(template_info->num_elements));


        for (jx = 0; jx < BCMI_FT_TEMPLATE_MAX_ELEMS; jx++) {
            alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(template_info->export_elements[jx]));
        }
    }

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->ft_type));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->max_records_reserve));
        alloc_size += BCMI_FT_WB_SCACHE_SIZE(&(coll_info->min_pointer));
    }


    /* Return the calculated size. */
    *size = alloc_size;

    return;
}

int
bcmi_ft_export_warmboot_alloc(int unit, soc_scache_handle_t scache_handle)
{

    uint8 *ft_scache_ptr;
    int size = 0;
    int rv = BCM_E_NONE;

    bcmi_ft_export_wb_alloc_size(unit, &size);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 size, &ft_scache_ptr,
                                 BCM_WB_FT_EXPORT_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_export_warmboot_sync
 * Purpose:
 *   Sync flowtracker export state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_export_warmboot_sync(int unit, soc_scache_handle_t scache_handle)
{
    int ix, jx;
    int size;
    bcmi_ft_collector_info_t *coll_info;
    bcmi_ft_template_info_t *template_info;
    uint8 *ft_scache_ptr;
    bcm_flowtracker_export_element_info_t *cur_elem_info = NULL;

    /* Get the size to save ft export module warmboot state. */
    bcmi_ft_export_wb_alloc_size(unit, &size);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 size, &ft_scache_ptr,
                                 BCM_WB_FT_EXPORT_DEFAULT_VERSION, NULL));

    /* DMA collector Id */
    BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(BCMI_FT_EXPORT_INFO(unit)->dma_collector_id));

    /* Sync collector_info structs. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->cmn_coll_id));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->cmn_export_prof_id));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->coll_hw_id));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->in_use));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->collector_info));

    }

    /* Sync template_info structs. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_templates; ix++) {
        template_info = BCMI_FT_TEMPLATE_INFO(unit, ix);

        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(template_info->template_id));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(template_info->in_use));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(template_info->set_id));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(template_info->num_elements));

        for (jx = 0; jx < BCMI_FT_TEMPLATE_MAX_ELEMS; jx++) {
            cur_elem_info = &(template_info->export_elements[jx]);
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->flags));
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->element));
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->data_size));
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->info_elem_id));
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->check_id));
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->direction));
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(cur_elem_info->custom_id));
        }
    }

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->ft_type));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->max_records_reserve));
        BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(coll_info->min_pointer));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_collector_fifo_entry_free
 * Purpose:
 *   Free block of fifo entries allocated to the collector
 * Parameters:
 *   unit - (IN) BCM device id
 *   req_entries - (IN) Number of entries to reserve
 *   min_pointer - (IN) Min pointer of the collector.
 * Returns:
 *   BCM_E_XXX.
 * Notes:
 *   Reserve FIFO entries used by the collector. This function is called
 *   in warmboot to populate collector fifo entry map.
 */
STATIC int
bcmi_ft_collector_fifo_entry_reserve(int unit,
        int req_entries, int min_pointer)
{
    int idx = 0, prev_block_idx = 0;
    int free_entries = 0, jump_blocks = 0;
    int num_fifo_entries = 0;
    int *fifo_coll_map = NULL;

    num_fifo_entries = BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries;
    fifo_coll_map = BCMI_FT_EXPORT_INFO(unit)->fifo_collector_map;

    while(idx < num_fifo_entries) {
        /* Unexpected case */
        if (fifo_coll_map[idx] == 0) {
            return BCM_E_INTERNAL;
        }
        if (fifo_coll_map[idx] > 0) {
            if (min_pointer < (idx + fifo_coll_map[idx])) {
                prev_block_idx = idx;
                break;
            }
            jump_blocks = fifo_coll_map[idx];
        } else {
            jump_blocks = -fifo_coll_map[idx];
        }

        idx += jump_blocks;
    }

    if (idx != min_pointer) {
        free_entries = fifo_coll_map[prev_block_idx];
        fifo_coll_map[prev_block_idx] = min_pointer - prev_block_idx;
        fifo_coll_map[min_pointer] = free_entries - fifo_coll_map[prev_block_idx];
    }
    idx = min_pointer;
    free_entries = fifo_coll_map[idx];
    if ((free_entries != req_entries) &&
            ((idx + req_entries) < num_fifo_entries)) {
        fifo_coll_map[idx + req_entries] = fifo_coll_map[idx] - req_entries;
    }
    /* Mark the block in use */
    fifo_coll_map[idx] = -req_entries;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_export_reinit(int unit)
{

    int ix, rv;
    bcmi_ft_template_info_t *template_info;
    bcmi_ft_collector_info_t *coll_info;

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        if (coll_info->in_use) {
            rv = _bcm_esw_collector_ref_count_update(unit,
                            coll_info->cmn_coll_id,
                            coll_info->in_use);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_esw_collector_user_update(unit,
                            coll_info->cmn_coll_id,
                            _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_esw_collector_export_profile_ref_count_update(unit,
                            coll_info->cmn_export_prof_id, coll_info->in_use);
            BCM_IF_ERROR_RETURN(rv);

            rv = bcmi_ft_collector_fifo_entry_reserve(unit,
                            coll_info->max_records_reserve,
                            coll_info->min_pointer);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_templates; ix++) {
        template_info = BCMI_FT_TEMPLATE_INFO(unit, ix);
        if (template_info->template_id != BCMI_FT_TEMPLATE_ID_INVALID) {
            SHR_BITSET(BCMI_FT_TEMPLATE_BITMAP(unit), ix);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_export_warmboot_recover
 * Purpose:
 *   Recover flowtracker export state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_export_warmboot_recover(int unit, soc_scache_handle_t scache_handle)
{
    int ix, jx;
    int size;
    bcmi_ft_collector_info_t *coll_info;
    bcmi_ft_template_info_t *template_info;
    uint8 *ft_scache_ptr = NULL;
    uint16 recovered_ver;
    bcm_flowtracker_export_element_info_t *cur_elem_info = NULL;
    bcmi_compat6519_flowtracker_export_element_info_t compat6519_elem_info;

    /* Get the size to save ft export module warmboot state. */
    bcmi_ft_export_wb_alloc_size(unit, &size);


    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 size, &ft_scache_ptr,
                                 BCM_WB_FT_EXPORT_DEFAULT_VERSION, &recovered_ver));

    /* DMA collector Id */
    BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(BCMI_FT_EXPORT_INFO(unit)->dma_collector_id));

    /* Sync collector_info structs. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->cmn_coll_id));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->cmn_export_prof_id));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->coll_hw_id));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->in_use));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->collector_info));
    }

    /* Sync template_info structs. */
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_templates; ix++) {
        template_info = BCMI_FT_TEMPLATE_INFO(unit, ix);

        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(template_info->template_id));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(template_info->in_use));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(template_info->set_id));
        BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(template_info->num_elements));


        for (jx = 0; jx < BCMI_FT_TEMPLATE_MAX_ELEMS; jx++) {
            if (recovered_ver <= BCM_WB_FT_EXPORT_VERSION_1_2) {
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &compat6519_elem_info);
                cur_elem_info = &(template_info->export_elements[jx]);
                cur_elem_info->flags        = compat6519_elem_info.flags;
                cur_elem_info->element      = compat6519_elem_info.element;
                cur_elem_info->data_size    = compat6519_elem_info.data_size;
                cur_elem_info->info_elem_id = compat6519_elem_info.info_elem_id;
                cur_elem_info->check_id     = compat6519_elem_info.check_id;
            } else {
                cur_elem_info = &(template_info->export_elements[jx]);
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->flags));
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->element));
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->data_size));
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->info_elem_id));
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->check_id));
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->direction));
                BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(cur_elem_info->custom_id));
            }
        }
    }

    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        if (recovered_ver >= BCM_WB_FT_EXPORT_VERSION_1_2) {
            BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->ft_type));
            BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->max_records_reserve));
            BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(coll_info->min_pointer));
        } else {
            coll_info->ft_type = BCMI_FT_COLLECTOR_FT_TYPE_MICRO;
            coll_info->max_records_reserve = BCMI_FT_COLLECTOR_FIFO_ENTRIES_DEF;
            coll_info->min_pointer = 16 * coll_info->coll_hw_id;
        }
    }

    /* Export reinit */
    BCM_IF_ERROR_RETURN(bcmi_ft_export_reinit(unit));

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
int
bcmi_ftv2_export_tod_set(int unit,
                         bcm_flowtracker_collector_t coll_hw_id,
                         bcm_flowtracker_collector_info_t *collector_info,
                         int coll_enable)
{
    uint32 hw_ts = 0;
    soc_reg_t tod_reg[BCMI_FT_MAX_COLLECTORS] = {
        IPFIX_TOD_SELECT_0r, IPFIX_TOD_SELECT_1r,
        IPFIX_TOD_SELECT_2r, IPFIX_TOD_SELECT_3r,
        IPFIX_TOD_SELECT_4r, IPFIX_TOD_SELECT_5r,
        IPFIX_TOD_SELECT_6r, IPFIX_TOD_SELECT_7r
    };

    /* input params check */
    if ((collector_info == NULL) ||
        (coll_hw_id >= BCMI_FT_MAX_COLLECTORS)) {
        return BCM_E_INTERNAL;
    }

    /* set timestamp source as per input */
    if (coll_enable) {

        switch(collector_info->src_ts) {

            case bcmFlowtrackerTimestampSourceLegacy:
                hw_ts = 0;
                break;
            case bcmFlowtrackerTimestampSourceNTP:
                hw_ts = 1;
                break;
            case bcmFlowtrackerTimestampSourcePTP:
                hw_ts = 2;
                break;

            default:
                return BCM_E_PARAM;
        }

    } else {
        /* once collector is detached, setting
         * register back to default value 0 */
        hw_ts = 0;
    }

    /* write into register */
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, tod_reg[coll_hw_id], REG_PORT_ANY, 0, hw_ts));

    return BCM_E_NONE;
}
#endif /*  BCM_FLOWTRACKER_V2_SUPPORT */

/*
 * Function:
 *  bcmi_ft_export_period_hw_val_get
 * Purpose:
 *  Get H/w Value for given periodic interval.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  interval      - (IN) Interval in milliseconds
 *  fld_len       - (IN) Field length to update
 *  hw_value      - (OUT) Hw value
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_export_period_hw_val_get(
    int unit,
    uint32 interval,
    int fld_len,
    uint32 *hw_value)
{
    int gran = 0;

    gran = BCMI_FT_PERIODIC_EXPORT_INTERVAL_GRAN;

    /* Granularity of field is 100 msec. If user configures
       less than 100 msec but not 0, then use 100 msec.
     */
    if ((interval < gran) && (interval != 0)) {
        *hw_value = 1;
    } else {
        /* Use interval nearest to the configured value */
        if ((interval % gran) <= (gran/2)) {
            *hw_value = interval / gran;
        } else {
            *hw_value = (interval / gran) + 1;
        }
    }

    /* interval cannot exceed max configuration interval */
    if (*hw_value > ((1 << fld_len) - 1)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Interval is out of range")));
        return BCM_E_PARAM;
    }

    if (*hw_value != (interval/gran)) {
        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Interval"
                        " configured is not supported. Setting to nearest"
                        " supported value=%d\n\r"), (*hw_value) * gran));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_peiodic_export_config_set
 * Purpose:
 *  Set Periodic export configurations.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  ft_type_flags - (IN) Flowtracker type Flags.
 *  config        - (IN) Periodic export configurations.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_periodic_export_config_set(
    int unit,
    uint32 ft_type_flags,
    bcm_flowtracker_periodic_export_config_t *config)
{
    int rv = BCM_E_NONE;
    int fld_len = 0;
    uint32 hw_value = 0;

    if ((ft_type_flags & BCM_FLOWTRACKER_TYPE_AGGREGATE) &&
            (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3))) {
        return BCM_E_UNAVAIL;
    }

    if (ft_type_flags & BCM_FLOWTRACKER_TYPE_MICRO) {
        fld_len = soc_reg_field_length(unit,
                BSC_AG_PERIODIC_EXPORTr, EXPORT_PERIODf);
        rv = bcmi_ft_export_period_hw_val_get(unit,
                config->interval_msec, fld_len, &hw_value);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_reg_field32_modify(unit, BSC_AG_PERIODIC_EXPORTr,
                REG_PORT_ANY, EXPORT_PERIODf, hw_value);
        BCM_IF_ERROR_RETURN(rv);

        if (config->flags & BCM_FLOWTRACKER_MICRO_PERIODIC_EXPORT_GUARANTEE) {
            hw_value = 1;
        } else {
            hw_value = 0;
        }
        rv = soc_reg_field32_modify(unit, BSC_AG_PERIODIC_EXPORTr,
                REG_PORT_ANY, DO_NOT_EXPORT_WHEN_FIFO_FULLf,
                hw_value);
        BCM_IF_ERROR_RETURN(rv);
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (ft_type_flags & BCM_FLOWTRACKER_TYPE_AGGREGATE) {
        fld_len = soc_reg_field_length(unit,
                BSC_AG_AFT_PERIODIC_EXPORTr, EXPORT_PERIODf);
        rv = bcmi_ft_export_period_hw_val_get(unit,
                config->interval_msec, fld_len, &hw_value);
        BCM_IF_ERROR_RETURN(rv);

        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {

            BCMI_FT_GROUP_INFO(unit)->aft_periodic_interval = hw_value;
            rv = bcmi_ftv3_aft_export_sw_scan_update(unit, hw_value);
        } else {
            rv = soc_reg_field32_modify(unit,
                    BSC_AG_AFT_PERIODIC_EXPORTr,
                    REG_PORT_ANY, EXPORT_PERIODf, hw_value);
        }
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_peiodic_export_config_get
 * Purpose:
 *  Get Periodic export configurations.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  ft_type_flags - (IN) Flowtracker type Flags.
 *  interval_msec - (OUT) Interval in milliseconds
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_periodic_export_config_get(
    int unit,
    uint32 ft_type_flags,
    bcm_flowtracker_periodic_export_config_t *config)
{
    int rv = BCM_E_NONE;
    uint64 reg_val;
    uint32 hw_val = 0;
    uint32 interval = 0;
    uint32 agg_interval = 0;
    uint32 micro_interval = 0;
    uint32 local_ft_type_flags = 0;

    if (!(ft_type_flags & (BCM_FLOWTRACKER_TYPE_MICRO |
                    BCM_FLOWTRACKER_TYPE_AGGREGATE))) {
        return BCM_E_PARAM;
    }

    if ((ft_type_flags & BCM_FLOWTRACKER_TYPE_AGGREGATE) &&
            (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3))) {
        return BCM_E_UNAVAIL;
    }

    COMPILER_64_SET(reg_val, 0, 0);
    if (ft_type_flags & BCM_FLOWTRACKER_TYPE_MICRO) {
        rv = READ_BSC_AG_PERIODIC_EXPORTr(unit, &reg_val);
        BCM_IF_ERROR_RETURN(rv);

        micro_interval = soc_reg64_field32_get(unit,
                BSC_AG_PERIODIC_EXPORTr, reg_val, EXPORT_PERIODf);

        hw_val = soc_reg64_field32_get(unit,
                BSC_AG_PERIODIC_EXPORTr, reg_val,
                DO_NOT_EXPORT_WHEN_FIFO_FULLf);
        if (hw_val == 0) {
            config->flags &=
                (~BCM_FLOWTRACKER_MICRO_PERIODIC_EXPORT_GUARANTEE);
        }
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (ft_type_flags & BCM_FLOWTRACKER_TYPE_AGGREGATE) {
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {

            agg_interval = BCMI_FT_GROUP_INFO(unit)->aft_periodic_interval;
        } else {
            rv = READ_BSC_AG_AFT_PERIODIC_EXPORTr(unit, &reg_val);
            BCM_IF_ERROR_RETURN(rv);

            agg_interval = soc_reg64_field32_get(unit,
                    BSC_AG_AFT_PERIODIC_EXPORTr, reg_val, EXPORT_PERIODf);
        }
    }
#endif

    /* If both flags are set, check if interval is same */
    local_ft_type_flags = BCM_FLOWTRACKER_TYPE_MICRO |
                          BCM_FLOWTRACKER_TYPE_AGGREGATE;
    if ((ft_type_flags & local_ft_type_flags) == local_ft_type_flags) {
        if (micro_interval != agg_interval) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Interval is not"
                                " same for flowtracker types.\n")));
            return BCM_E_PARAM;
        }
        interval = micro_interval;
    } else if (ft_type_flags & BCM_FLOWTRACKER_TYPE_MICRO) {
        interval = micro_interval;
    } else {
        interval = agg_interval;
    }

    config->interval_msec = interval * BCMI_FT_PERIODIC_EXPORT_INTERVAL_GRAN;

    return rv;
}

/*
 * Function:
 *  bcmi_ft_export_record_data_info_get
 * Purpose:
 *  Get the decoded data for the export record
 * Parameters:
 *  unit          - (IN) BCM device number
 *  record        - (IN) Export Record
 *  max_elements  - (IN) Size of array of export_element_info and data_info
 *  export_element_info - (IN) Export Elements used to decode record
 *  data_info     - (OUT) decoded Data info per export element
 *  count         - (OUT) Number of valid data_info
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_export_record_data_info_get(
    int unit,
    bcm_flowtracker_export_record_t *record,
    int max_elements,
    bcm_flowtracker_export_element_info_t *export_element_info,
    bcm_flowtracker_export_record_data_info_t *data_info,
    int *count)
{
    int offset = 0;
    int idx = 0, rec_data = 0;
    soc_format_t format;
    soc_field_info_t *field_info = NULL;
    bcm_flowtracker_export_element_info_t *elem_info = NULL;

    if ((record == NULL) ||
        (export_element_info == NULL) ||
        (max_elements == 0) ||
        (count == NULL)) {
        return BCM_E_PARAM;
    }

    format = BSC_EX_EXPORT_RECORDfmt;

    /* offset by SET HEADER */
    field_info = soc_format_fieldinfo_get(unit, format, SET_HEADERf);
    if (field_info != NULL) {
        offset = field_info->len / 8;
    }

    for (idx = 0, rec_data = 0; idx < max_elements; idx++) {
        elem_info = &export_element_info[idx];

        if (elem_info->element == bcmFlowtrackerExportElementTypeReserved) {
            offset += elem_info->data_size;
            continue;
        }

        if (data_info != NULL) {

            sal_memcpy(&data_info[rec_data].export_element_info,
                    elem_info, sizeof(bcm_flowtracker_export_element_info_t));

            sal_memcpy(data_info[rec_data].data,
                    &record->data_record[offset], elem_info->data_size);
        }

        /* Increment data offset */
        offset += elem_info->data_size;
        rec_data++;
    }

    *count = rec_data;

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)

STATIC int
bcmi_ftv3_aft_group_export_hw_enable(
        int unit,
        int *count,
        bcm_flowtracker_group_t *group_list,
        int enable)
{
    int rv = BCM_E_NONE;
    int idx;
    int type, group_idx;
    soc_mem_t mem = INVALIDf;

    if ((count == NULL) || (group_list == NULL)) {
        return BCM_E_NONE;
    }

    for (idx = 0; idx < *count; idx++) {

        type = BCMI_FT_GROUP_TYPE_GET(group_list[idx]);
        group_idx = BCMI_FT_GROUP_INDEX_GET(group_list[idx]);
        rv = bcmi_ft_group_type_mem_get(unit, 1, type, &mem);
        BCM_IF_ERROR_RETURN(rv);

        soc_mem_field32_modify(unit, mem,
                group_idx, PERIODIC_EXPORT_ENf, enable);
    }

    return BCM_E_NONE;
}

void bcmi_ftv3_aft_export_sw_thread(void *up)
{
    int unit = PTR_TO_INT(up);
    int credit_avail = 0;
    int loop = 0, group_count[2];
    int col_idx = 0, first = 0;
    int col_parsed = 0, col_grp_parsed = 0;
    bcm_flowtracker_group_t ngid = -1;
    bcmi_aft_export_sw_ctrl_t *sw_ctrl = NULL;
    bcmi_ft_collector_agg_group_info_t *col_info = NULL;
    bcm_flowtracker_group_t en_group_id_list[2][128];
    bcm_flowtracker_group_t *next_group_id_list = NULL;
    bcm_flowtracker_group_t *prev_group_id_list = NULL;
    int *next_group_count = NULL;
    int *prev_group_count = NULL;

    sw_ctrl = bcmi_aft_export_sw_ctrl[unit];
    if (sw_ctrl == NULL) {
        sal_thread_exit(0);
        return;
    }

    group_count[0] = group_count[1] = 0;

    sal_sem_give(sw_ctrl->sema);

    while (TRUE) {

        /* Sleep for 100 ms */
        sal_usleep(MILLISECOND_USEC * sw_ctrl->scan_interval);

        /* Exit if user disabled periodic export */
        if (sw_ctrl->user_interval == 0) {
            break;
        }

        /* Disable groups in prev list */
        if (prev_group_id_list) {
            bcmi_ftv3_aft_group_export_hw_enable(unit,
                    prev_group_count, prev_group_id_list, FALSE);
        }

        /* Enable groups in next list */
        if (next_group_id_list) {
             bcmi_ftv3_aft_group_export_hw_enable(unit,
                    next_group_count, next_group_id_list, TRUE);
        }

        /* Switch next/prev group id list */
        prev_group_id_list = en_group_id_list[loop];
        prev_group_count = &group_count[loop];
        loop = (loop == 0) ? 1 : 0;
        next_group_id_list = en_group_id_list[loop];
        next_group_count = &group_count[loop];
        *next_group_count = 0;

        col_parsed = 0;
        col_idx = sw_ctrl->last_col_idx;

#define SCAN_COND_CHECK (col_parsed < BCMI_FT_MAX_COLLECTORS)
#define NEXT_COL_SET(col_idx)  col_idx = (col_idx+1) % BCMI_FT_MAX_COLLECTORS; col_parsed++

        while(SCAN_COND_CHECK) {

            /* Collector Lock */
            BCMI_AFT_EXPORT_SW_COL_LOCK(unit, col_idx);

            col_info = BCMI_AFT_COL_GROUP_INFO(unit, col_idx);

            /* If Collector is not valid, continue */
            if (col_info->num_groups == FALSE) {
                BCMI_AFT_EXPORT_SW_COL_UNLOCK(unit, col_idx);
                NEXT_COL_SET(col_idx);
                continue;
            }

            first = TRUE;
            col_grp_parsed = 0;
            credit_avail = col_info->num_fifo_entries;
            col_info->next_scan_idx %= col_info->num_groups;

#define COL_SCAN_COND_CHECK (col_grp_parsed < col_info->num_groups)
#define COL_GRP_NEXT_SET(_i_, _n_) (_i_) = (((_i_) + 1) % (_n_)); col_grp_parsed++

            while (COL_SCAN_COND_CHECK) {

                ngid = col_info->group_id_list[col_info->next_scan_idx];
                credit_avail -= BCMI_AFT_GRP_USER_ENTRY_INSTALLED(unit, ngid);

                if ((first == TRUE) || (credit_avail >= 0)) {
                    next_group_id_list[*next_group_count] = ngid;
                    (*next_group_count)++;
                }
                if ((first == FALSE) && (credit_avail < 0)) {
                    break;
                }
                first = FALSE;
                COL_GRP_NEXT_SET(col_info->next_scan_idx, col_info->num_groups);
            }

            /* Collector Unlock */
            BCMI_AFT_EXPORT_SW_COL_UNLOCK(unit, col_idx);

            NEXT_COL_SET(col_idx);
        }
        sw_ctrl->last_col_idx = col_idx % BCMI_FT_MAX_COLLECTORS;
    }

    /* Diable all groups */
    if (prev_group_id_list) {
        bcmi_ftv3_aft_group_export_hw_enable(unit,
            prev_group_count, prev_group_id_list, FALSE);
    }

    sw_ctrl->pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/* Export sw scan thread start/stop */
STATIC int
bcmi_ftv3_aft_export_sw_scan_enable(int unit, int enable)
{
    soc_timeout_t timeout;
    bcmi_aft_export_sw_ctrl_t *sw_ctrl = NULL;

    sw_ctrl = bcmi_aft_export_sw_ctrl[unit];
    if (sw_ctrl == NULL) {
        return BCM_E_INIT;
    }

    if (enable) {
        /* Check if thread is running */
        if (sw_ctrl->pid != SAL_THREAD_ERROR) {
            return BCM_E_NONE;
        }

        /* Create semaphore */
        if (sw_ctrl->sema == 0) {
            sw_ctrl->sema = sal_sem_create("bcmAFTEx", sal_sem_BINARY, 0);
            if (sw_ctrl->sema == 0) {
                return BCM_E_MEMORY;
            }
        }

        sw_ctrl->pid = sal_thread_create("bcmAFTEx",
                SAL_THREAD_STKSZ,
                BCMI_AFT_EXPORT_SW_THREAD_PRI,
                bcmi_ftv3_aft_export_sw_thread, INT_TO_PTR(unit));
        if (sw_ctrl->pid == SAL_THREAD_ERROR) {
            sal_sem_destroy(sw_ctrl->sema);
            return BCM_E_MEMORY;
        }

        /* Sync with thread */
        if (sal_sem_take(sw_ctrl->sema, sal_sem_FOREVER)) {
            sal_sem_destroy(sw_ctrl->sema);
            return BCM_E_MEMORY;
        }

    } else if (sw_ctrl->pid != SAL_THREAD_ERROR) {
        sw_ctrl->user_interval = 0;

        soc_timeout_init(&timeout, 10000000, 0);

        while (sw_ctrl->pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&timeout) != 0) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "AFT Export Sw thread did not exit.\n")));

                sw_ctrl->pid = SAL_THREAD_ERROR;
                return BCM_E_TIMEOUT;
            }
        }

        /* Destroy semaphore */
        if (sw_ctrl->sema != 0) {
            sal_sem_destroy(sw_ctrl->sema);
            sw_ctrl->sema = 0;
        }
    }

    return BCM_E_NONE;
}

/* Update export sw scan interval */
int
bcmi_ftv3_aft_export_sw_scan_update(int unit, uint32 interval)
{
    int rv = BCM_E_NONE;
    bcmi_aft_export_sw_ctrl_t *sw_ctrl = NULL;

    sw_ctrl = bcmi_aft_export_sw_ctrl[unit];
    if (sw_ctrl == NULL) {
        return BCM_E_INIT;
    }

    if (interval == 0) {
        /* Set Interval in hw register */
        rv = soc_reg_field32_modify(unit, BSC_AG_AFT_PERIODIC_EXPORTr,
                REG_PORT_ANY, EXPORT_PERIODf, 0);
        BCM_IF_ERROR_RETURN(rv);

        sw_ctrl->user_interval = 0;
        rv = bcmi_ftv3_aft_export_sw_scan_enable(unit, 0);
        BCM_IF_ERROR_RETURN(rv);

    } else {

        sw_ctrl->user_interval = interval * 100;
        rv = bcmi_ftv3_aft_export_sw_scan_enable(unit, 1);
        BCM_IF_ERROR_RETURN(rv);

        /* Set Interval in hw register */
        rv = soc_reg_field32_modify(unit, BSC_AG_AFT_PERIODIC_EXPORTr,
                REG_PORT_ANY, EXPORT_PERIODf, sw_ctrl->scan_interval/100);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/* Initialize aft export sw ctrl */
int
bcmi_ftv3_aft_export_sw_ctrl_init(int unit)
{
    int group_type;
    int idx = 0, num_groups = 0;
    int total_groups = 0;
    bcmi_aft_export_sw_ctrl_t *sw_ctrl = NULL;
    bcmi_ft_collector_agg_group_info_t *col_info = NULL;

    if (bcmi_aft_export_sw_ctrl[unit]) {
        bcmi_ftv3_aft_export_sw_ctrl_cleanup(unit);
    }

    BCMI_FT_ALLOC(bcmi_aft_export_sw_ctrl[unit],
            sizeof(bcmi_aft_export_sw_ctrl_t), " aft export sw ctrl");
    sw_ctrl = bcmi_aft_export_sw_ctrl[unit];

    /* Set defaults */
    sw_ctrl->user_interval = 0;
    sw_ctrl->scan_interval = 100; /* multiple of msec */
    sw_ctrl->pid = SAL_THREAD_ERROR;

    /* Group user info */
    for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
            group_type <= bcmFlowtrackerGroupTypeAggregateEgress;
            group_type++) {

        num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);
        total_groups += num_groups;

        BCMI_FT_ALLOC(BCMI_AFT_GRP_USER_ENTRY_TYPE_INFO(unit, group_type),
                sizeof(bcmi_ft_group_hw_user_entry_info_t *) * num_groups,
                "ft group user info");

        if (BCMI_AFT_GRP_USER_ENTRY_TYPE_INFO(unit, group_type) == NULL) {
            bcmi_ftv3_aft_export_sw_ctrl_cleanup(unit);
            return BCM_E_MEMORY;
        }
    }

    /* Collector group info */
    for (idx = 0; idx < BCMI_FT_MAX_COLLECTORS; idx++) {
        BCMI_FT_ALLOC(BCMI_AFT_COL_GROUP_INFO(unit, idx),
                sizeof(bcmi_ft_collector_agg_group_info_t),
                "ft collector group info");

        if (BCMI_AFT_COL_GROUP_INFO(unit, idx) == NULL) {
            bcmi_ftv3_aft_export_sw_ctrl_cleanup(unit);
            return BCM_E_MEMORY;
        }

        /* Allocate for Group id list */
        col_info = BCMI_AFT_COL_GROUP_INFO(unit, idx);
        BCMI_FT_ALLOC(col_info->group_id_list,
            sizeof(bcm_flowtracker_group_t) * total_groups,
            "ft col group info");
        if (col_info->group_id_list == NULL) {
            bcmi_ftv3_aft_export_sw_ctrl_cleanup(unit);
            return BCM_E_MEMORY;
        }

        sw_ctrl->col_lock[idx] = sal_mutex_create("aftPeriodicExColLock");
        if (!sw_ctrl->col_lock[idx]) {
            bcmi_ftv3_aft_export_sw_ctrl_cleanup(unit);
            return BCM_E_MEMORY;
        }
    }

    return BCM_E_NONE;
}

/* Cleanup aft export sw ctrl */
int
bcmi_ftv3_aft_export_sw_ctrl_cleanup(int unit)
{
    int group_type = 0, idx = 0;
    bcmi_aft_export_sw_ctrl_t *sw_ctrl = NULL;
    bcmi_ft_collector_agg_group_info_t *col_info = NULL;

    sw_ctrl = bcmi_aft_export_sw_ctrl[unit];
    if (sw_ctrl == NULL) {
        return BCM_E_NONE;
    }

    /* Stop scan thread */
    if (sw_ctrl->pid != SAL_THREAD_ERROR) {
        bcmi_ftv3_aft_export_sw_scan_enable(unit, 0);
    }

    /* Group user info */
    for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
            group_type <= bcmFlowtrackerGroupTypeAggregateEgress;
            group_type++) {

        if (BCMI_AFT_GRP_USER_ENTRY_TYPE_INFO(unit, group_type) != NULL) {
            sal_free(BCMI_AFT_GRP_USER_ENTRY_TYPE_INFO(unit, group_type));
            BCMI_AFT_GRP_USER_ENTRY_TYPE_INFO(unit, group_type) = NULL;
        }
    }

    /* Collector group info */
    for (idx = 0; idx < BCMI_FT_MAX_COLLECTORS; idx++) {

        col_info = BCMI_AFT_COL_GROUP_INFO(unit, idx);
        if (col_info != NULL) {

            if (col_info->group_id_list != NULL) {
                sal_free(col_info->group_id_list);
                col_info->group_id_list = NULL;
            }
            sal_free(BCMI_AFT_COL_GROUP_INFO(unit, idx));
            BCMI_AFT_COL_GROUP_INFO(unit, idx) = NULL;
        }

        if (sw_ctrl->col_lock[idx]) {
            sal_mutex_destroy(sw_ctrl->col_lock[idx]);
        }
    }

    sal_free(bcmi_aft_export_sw_ctrl[unit]);
    bcmi_aft_export_sw_ctrl[unit] = NULL;

    return BCM_E_NONE;
}

/* Add Collector-Group to AFT Export Sw ctrl */
STATIC int
bcmi_ftv3_aft_export_sw_col_grp_add(
        int unit,
        bcm_flowtracker_collector_t col_id,
        bcm_flowtracker_group_t group_id)
{
    bcmi_ft_collector_agg_group_info_t *col_info = NULL;

    /* If Periodic export is not enabled, return */
    if (!(BCMI_FT_GROUP(unit, group_id)->flags &
                BCMI_FT_GROUP_INFO_F_PERIODIC_EXPORT)) {
        return BCM_E_NONE;
    }

    if (BCMI_AFT_GRP_USER_ENTRY_INSTALLED(unit, group_id) == 0) {
        return BCM_E_NONE;
    }

    BCMI_AFT_EXPORT_SW_COL_LOCK(unit, col_id);

    col_info = BCMI_AFT_COL_GROUP_INFO(unit, col_id);
    col_info->group_id_list[col_info->num_groups] = group_id;
    col_info->num_groups++;
    col_info->num_fifo_entries =
            (BCMI_FT_COLLECTOR_INFO(unit, col_id))->max_records_reserve;

    BCMI_AFT_EXPORT_SW_COL_UNLOCK(unit, col_id);

    return BCM_E_NONE;
}

/* Remove Group-Collector from scanning */
STATIC int
bcmi_ftv3_aft_export_sw_col_grp_remove(
        int unit,
        bcm_flowtracker_collector_t col_id,
        bcm_flowtracker_group_t group_id)
{
    int idx = 0;
    bcmi_ft_collector_agg_group_info_t *col_info = NULL;

    BCMI_AFT_EXPORT_SW_COL_LOCK(unit, col_id);

    col_info = BCMI_AFT_COL_GROUP_INFO(unit, col_id);
    for (idx = 0; idx < col_info->num_groups; idx++) {
        if (col_info->group_id_list[idx] == group_id) {
            break;
        }
    }
    if (idx == col_info->num_groups) {
        BCMI_AFT_EXPORT_SW_COL_UNLOCK(unit, col_id);
        return BCM_E_NONE;
    }

    if (col_info->num_groups != 1) {

        if (idx >= col_info->next_scan_idx) {
            col_info->group_id_list[idx] =
                col_info->group_id_list[col_info->num_groups - 1];
        } else if (idx < col_info->next_scan_idx) {
            col_info->next_scan_idx--;
            col_info->group_id_list[idx] =
                col_info->group_id_list[col_info->next_scan_idx];
            col_info->group_id_list[col_info->next_scan_idx] =
                col_info->group_id_list[col_info->num_groups - 1];
        }
    }

    col_info->num_groups--;

    BCMI_AFT_EXPORT_SW_COL_UNLOCK(unit, col_id);

    return BCM_E_NONE;
}

/* Enable/Disable periodic export on a group */
int
bcmi_ftv3_aft_export_sw_grp_timer_set(
        int unit,
        bcm_flowtracker_group_t group_id,
        uint32 enable)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_collector_t collector_id;

    collector_id = BCMI_FT_GROUP(unit, group_id)->collector_id;

    if (enable == TRUE) {
        rv = bcmi_ftv3_aft_export_sw_col_grp_add(unit, collector_id, group_id);
    } else {
        rv = bcmi_ftv3_aft_export_sw_col_grp_remove(unit, collector_id, group_id);
    }

    return rv;
}

/* User entry installed/Removed */
int
bcmi_ftv3_aft_export_sw_grp_hw_entry_update(
        int unit,
        bcm_flowtracker_group_t group_id,
        int count)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_collector_t collector_id;

    collector_id = BCMI_FT_GROUP(unit, group_id)->collector_id;

    /* If no user entries are installed to hardware, return */
    BCMI_AFT_GRP_USER_ENTRY_INSTALLED(unit, group_id) += count;

    if (count >= 0) {
        if (BCMI_AFT_GRP_USER_ENTRY_INSTALLED(unit, group_id) == count) {
            rv = bcmi_ftv3_aft_export_sw_col_grp_add(unit, collector_id, group_id);
        }
    } else {
        if (BCMI_AFT_GRP_USER_ENTRY_INSTALLED(unit, group_id) == 0) {
            rv = bcmi_ftv3_aft_export_sw_col_grp_remove(unit, collector_id,  group_id);
        }
    }

    return rv;
}

/* Show aggregate export sw control */
int
bcmi_ftv3_aft_export_sw_ctrl_show(int unit)
{
    int group_type = 0, ii = 0;
    int num_groups = 0, idx = 0;
    bcm_flowtracker_group_t group_id;
    bcmi_ft_group_hw_user_entry_info_t *grp_info = NULL;
    bcmi_ft_collector_agg_group_info_t *col_info = NULL;

    LOG_CLI((BSL_META_U(unit, "Group hw user entries\n\r")));
    for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
            group_type <= bcmFlowtrackerGroupTypeAggregateEgress;
            group_type++) {

        num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);
        for (idx = 0; idx < num_groups; idx++) {

            BCM_FLOWTRACKER_GROUP_ID_SET(group_id, group_type, idx);

            grp_info = BCMI_AFT_GRP_USER_ENTRY_INFO(unit, group_id);
            if (grp_info->hw_user_entries == 0) {
                continue;
            }
            LOG_CLI((BSL_META_U(unit, "0x%x (%d) | "), group_id, grp_info->hw_user_entries));
            if (((idx + 1) % 6) == 0) {
                LOG_CLI((BSL_META_U(unit, "\n\r")));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n\r")));
    }

    LOG_CLI((BSL_META_U(unit, "\n\r")));

    for (idx = 0; idx < BCMI_FT_MAX_COLLECTORS; idx++) {

        LOG_CLI((BSL_META_U(unit, "Collector[%d] ---\n\r"), idx));
        col_info = BCMI_AFT_COL_GROUP_INFO(unit, idx);
        if (col_info->num_groups) {

            LOG_CLI((BSL_META_U(unit, "FIFO=%d Groups=%d\n\r"),
                col_info->num_fifo_entries, col_info->num_groups));
        }
        for (ii = 0; ii < col_info->num_groups; ii++) {
            LOG_CLI((BSL_META_U(unit, "0x%x | "), col_info->group_id_list[ii]));
            if (((ii + 1) % 6) == 0) {
                LOG_CLI((BSL_META_U(unit, "\n\r")));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n\r\n\r")));
    }

    return BCM_E_NONE;
}


#if defined(BCM_WARM_BOOT_SUPPORT)
/* reinit */
int
bcmi_ftv3_aft_export_sw_ctrl_recover(int unit)
{
    int rv = BCM_E_NONE;
    int group_type = 0, ix = 0;

    for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
            group_type <= bcmFlowtrackerGroupTypeAggregateEgress;
            group_type++) {

        if (BCMI_FTK_USER_ENTRY_TYPE(unit, group_type) == NULL) {
            continue;
        }

        for (ix = 0;
                ix < BCMI_FTK_USER_ENTRY_MAX_COUNT(unit, group_type);
                ix++) {
            if (BCMI_FTK_USER_ENTRY_INFO(unit, group_type, ix) != NULL) {
                rv = bcmi_ftv3_aft_export_sw_grp_hw_entry_update(unit,
                        BCMI_FTK_USER_ENTRY_INFO(unit, group_type, ix)->group_id,
                        BCMI_FTK_USER_ENTRY_INFO(unit, group_type, ix)->ref_count);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#else /* BCM_FLOWTRACKER_SUPPORT */
typedef int bcmi_ft_export_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
