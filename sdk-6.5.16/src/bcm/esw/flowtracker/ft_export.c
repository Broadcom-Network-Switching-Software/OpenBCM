/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_export.c
 * Purpose:     Implementation of Flowtracker Export APIs.
 * Requires:
 */


#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_field.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#include <soc/cmicm.h>


#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>

#define BCM_WB_FT_EXPORT_VERSION_1_1                SOC_SCACHE_VERSION(1,1)

/* Default warmboot version */
#define BCM_WB_FT_EXPORT_DEFAULT_VERSION            BCM_WB_FT_EXPORT_VERSION_1_1

#endif

/* Export software state. */
bcmi_ft_export_state_t *bcmi_ft_export_state[BCM_MAX_NUM_UNITS] = {0};



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

            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeExportReasons, 2, "ExportReasons"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeExportFlags, 1, "ExportFlags"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeFlowtrackerGroup, 1, "FlowtrackerGroup"},
            { bcmFlowtrackerTrackingParamTypeNone, bcmFlowtrackerExportElementTypeReserved, 1, "Reserved"}
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

    bcma_ft_export_fifo_control(unit, 0);

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
    ipfix->sequence_num = ft_coll_info->ipfix_hdr.initial_sequence_num;
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



/* Routine to install FT collector hardware. */
STATIC int
bcmi_ft_collector_hw_install(int unit,
                             bcm_flowtracker_collector_t coll_hw_id,
                             int coll_enable)
{
    int ix, rv;
    int m_idx, m_bytes, m_entries;
    int min_ptr;
    int fifo_depth;
    int coll_depth;
    int num_collectors;
    int init_seq;
    int num_records;
    uint32 num_records_enc;

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


    /* Init pkt_build. */
    sal_memset(&pkt_build, 0, sizeof(bcmi_ft_pkt_build_t));

    ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, coll_hw_id);
    collector_info = &(ft_coll_info->collector_info);

    num_records = 1;
    num_collectors = BCMI_FT_EXPORT_INFO(unit)->max_collectors;
    fifo_depth = (soc_mem_index_count(unit, BSC_EX_EXPORT_FIFOm) / 2);
    coll_depth = (fifo_depth / num_collectors);
    min_ptr = (coll_depth * coll_hw_id);
    m_entries = (soc_mem_index_count(unit, BSC_EX_COLLECTOR_IPFIX_PACKET_BUILDm) /
                 soc_mem_index_count(unit, BSC_EX_COLLECTOR_CONFIGm));

    init_seq = (ft_coll_info->ipfix_hdr).initial_sequence_num;

    /* Flush Collector before enabling. */
    collector_mem = BSC_EX_COLLECTOR_CONFIGm;
    sal_memset(&config, 0, sizeof(config));
    soc_mem_field32_set(unit, collector_mem,
                        &config, FLUSHf, 0x1);
    soc_mem_write(unit, collector_mem, COPYNO_ALL, coll_hw_id, &config);



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


    /* For uninstalling, flush collector, disable and return  */
    if (coll_enable == 0) {
        return BCM_E_NONE;
    }

    /* Populate PACKET_BUILD. */
    if (collector_info->collector_type == bcmFlowtrackerCollectorRemote) {
        collector_mem = BSC_EX_COLLECTOR_IPFIX_PACKET_BUILDm;

        sal_memset(&pkt_build, 0, sizeof(pkt_build));

        rv = bcmi_ft_collector_packet_build(unit, ft_coll_info, &pkt_build);
        BCM_IF_ERROR_RETURN(rv);

        num_records = ft_coll_info->num_records;
        if (num_records == -1) {
            num_records = ((collector_info->max_packet_length -
                            pkt_build.encap_length) /
                           (BCMI_FT_IPFIX_RECORD_SIZE));
        }

        /* Populate PACKET_BUILD */
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
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcmi_ft_num_records_enc_get(unit, num_records, &num_records_enc);
    BCM_IF_ERROR_RETURN(rv);

    /* Reset sequence number. */
    rval = 0;
    soc_reg_field_set(unit, BSC_EX_SEQ_NUMr, &rval, SEQ_NUMf, init_seq);
    rv = soc_reg32_set(unit, BSC_EX_SEQ_NUMr, REG_PORT_ANY, coll_hw_id, rval);
    BCM_IF_ERROR_RETURN(rv);

    /* Enable Collector. */
    collector_mem = BSC_EX_COLLECTOR_CONFIGm;
    soc_mem_field32_set(unit, collector_mem,
                        &config, FLUSHf, 0x0);
    soc_mem_field32_set(unit, collector_mem,
                        &config, ENABLEf, 0x1);
    soc_mem_field32_set(unit, collector_mem,
                        &config, MAX_POINTERf, (min_ptr + coll_depth - 1));
    soc_mem_field32_set(unit, collector_mem,
                        &config, MIN_POINTERf, min_ptr);
    soc_mem_field32_set(unit, collector_mem,
                        &config, MAX_RECORDS_PER_PACKETf, num_records_enc);
    soc_mem_field32_set(unit, collector_mem,
                        &config, TIME_INTERVAL_ENABLEf, 0x1);
    soc_mem_field32_set(unit, collector_mem,
                        &config, TIME_INTERVALf, 0x1);


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

        if ((info->max_packet_length < min_frame_size) ||
            (info->max_packet_length > max_frame_size)) {
            return BCM_E_PARAM;
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
                          int num_elems,
                          bcm_flowtracker_export_element_info_t *elem_list)
{
    int ix, jx;
    bcm_flowtracker_export_element_info_t *elem;

    if ((num_elems <= 0) ||
        (num_elems >= BCMI_FT_EXPORT_INFO(unit)->max_elements)) {
        return BCM_E_PARAM;
    }


    for (jx = 0; jx < num_elems; jx++) {
        elem = elem_list +jx;

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
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_template_id_alloc
 * Purpose:
 *     get free index for template id.
 * Parameters:
 *     unit - (IN) BCM device id
 *     alloc - (IN) Allocate a fresh id.
 *     template_id   - (OUT) FT export template id.
 *
 * Returns:
 *     BCM_E_XXX.
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
 *     bcmi_ft_export_state_clear
 * Purpose:
 *     Clear Flowtracker export state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
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
 *     bcmi_ft_export_state_init
 * Purpose:
 *     Initialize Flowtracker export state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
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

    return BCM_E_NONE;
}


int
bcmi_ft_export_init(int unit)
{
    int rv;
    int ipfix_obs_domain_id;
    uint32 fld_len, interval, high_val;
    uint32 interval_usecs, interval_secs;
    uint64 val64;

    rv = bcmi_ft_export_cleanup(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ft_export_state_init(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        rv = bcmi_ft_export_cb_init(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    ipfix_obs_domain_id = soc_property_get(unit,
                              spn_FLOWTRACKER_IPFIX_OBSERVATION_DOMAIN_ID,
                              BCMI_FTE_IPFIX_HDR_OBS_DOMAIN);
    BCMI_FT_EXPORT_INFO(unit)->ipfix_observation_domain_id =
                                  ipfix_obs_domain_id;

    interval_secs = soc_property_get(unit,
                        spn_FLOWTRACKER_EXPORT_INTERVAL_SECS, 0);
    interval_usecs = soc_property_get(unit,
                          spn_FLOWTRACKER_EXPORT_INTERVAL_USECS, 0);

    /* Export Interval in seconds */
    COMPILER_64_SET(val64, 0, interval_secs);

    /* Export Interval in 100 msecs */
    COMPILER_64_UMUL_32(val64, 10);

    /* Converting usecs to 100 msecs */
    interval = interval_usecs / 1000;
    interval = interval / 100;

    /* Add to get export interval in 100 msecs */
    COMPILER_64_ADD_32(val64, interval);

    COMPILER_64_TO_32_HI(high_val, val64);
    if (high_val != 0) {
        interval = 0xFFFFFFFF;
    } else {
        COMPILER_64_TO_32_LO(interval, val64);
    }
    /* In case user have configured export interval value less than 100 ms,
     * but not zero. here user intention is to set least value of export
     * interval, so set interval=1 (100 ms) */
    if ((interval == 0) && (interval_usecs != 0)) {
        interval = 1;
    }

    fld_len = soc_reg_field_length(unit, BSC_AG_PERIODIC_EXPORTr, EXPORT_PERIODf);

    /* Do not exceed field length */
    interval = MIN(interval, ((1 << fld_len) - 1));

    rv = soc_reg_field32_modify(unit, BSC_AG_PERIODIC_EXPORTr,
                                REG_PORT_ANY, EXPORT_PERIODf, interval);

    return rv;
}

int
bcmi_ft_export_cleanup(int unit)
{
    int rv;

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
 *     bcmi_ft_collector_create
 * Purpose:
 *     Create flowtracker collector id.
 * Description :
 *     Routine to create a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options for group creation
 *     coll_hw_id   - (OUT) FT collector id.
 *     collector_info - (IN) Collector info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_collector_create(
    int unit,
    uint32 options,
    bcm_flowtracker_collector_t *coll_hw_id,
    bcm_flowtracker_collector_info_t *collector_info)
{
    int rv;
    int idx;
    bcm_flowtracker_collector_info_t old_collector_info;
    bcmi_ft_collector_info_t *ft_coll_info;
    bcm_collector_ipfix_header_t *ipfix_hdr;

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
 *     bcmi_ft_collector_get
 * Purpose:
 *     Fetch flowtracker collector info.
 * Description :
 *     Routine to create a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     coll_hw_id   - (OUT) FT collector id.
 *     collector_info - (IN) Collector info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_collector_get(
    int unit,
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
 *     bcmi_ft_collector_get_all
 * Purpose:
 *     Fetch all flowtracker collectors info.
 * Description :
 *     Routine to create a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     max_size   - (OUT) Max collectors to read.
 *     collector_list - (OUT) Collector id list.
 *     list_size (OUT) - Num of collectors read.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_collector_get_all(
    int unit,
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
 *     bcmi_ft_collector_destroy
 * Purpose:
 *     Destroy flowtracker collector.
 * Description :
 *     Routine to destroy a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     coll_hw_id   - (OUT) FT collector id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_collector_destroy(
    int unit,
    bcm_flowtracker_collector_t coll_hw_id)
{
    bcmi_ft_collector_info_t *ft_coll_info;

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

    sal_memset(&(ft_coll_info->collector_info), 0,
               sizeof(ft_coll_info->collector_info));
    ft_coll_info->coll_hw_id = BCMI_FT_COLLECTOR_ID_INVALID;
    ft_coll_info->cmn_coll_id = BCMI_FT_COLLECTOR_ID_INVALID;
    ft_coll_info->num_records = -1;

    BCM_IF_ERROR_RETURN(bcmi_ft_collector_hw_install(unit, coll_hw_id, FALSE));

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
                sal_strncpy(elem_name, bcmi_ft_export_elements_map[ix].element_name,
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

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d\n\r"), idx, info_elem, info_name, key_alu->length));
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
    int length = 0;
    int rec_total_bits = 0;
    int flow_data_offset = 0;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_export_element_type_t info_elem;
    bcm_flowtracker_tracking_param_type_t tracking_param;
    bcmi_ft_group_template_list_t *ftg_template = NULL;
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];

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
        rv = bcmi_ft_tracking_param_xlate(unit,
                   tracking_param, NULL, &info_elem);
        BCM_IF_ERROR_RETURN(rv);

        length = ftg_template->info.cont_width;

        if (ftg_template->info.flags & BCMI_FT_ALU_LOAD_NEXT_ATTACH) {
            /* Special case for dos attack. */
            length += ftg_template->next->info.cont_width;
        }

        if (ftg_template->info.data_shift != 0) {
            length -= ftg_template->info.data_shift;
        }
        offset = ftg_template->info.cont_offset;
        offset = flow_data_offset - (offset + length);

        BCMI_FTE_RECORD_ELEM_IDX_GET(rec_total_bits, offset, length, idx);
        BCMI_FTE_RECORD_ELEM_SET(record, idx, info_elem);
        BCMI_FTE_RECORD_LENGTH_SET(record, idx, length);
        BCMI_FTE_RECORD_TRACKING_PARAM_SET(record, idx, tracking_param);
        BCMI_FTE_RECORD_FLOW_CHECK_SET(record, idx, check_id);

        sal_memset(info_name, 0, sizeof(char) * BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Record_Idx: %2d | Id: %2d (%20s) |"
                        " Size: %02d (0x%x)\n\r"), idx, info_elem, info_name, length,
                        check_id));

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
    }

    /* Insert hardware extracted default elements */
    /* FT_INDEX */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, FLOW_INDEXf);
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

    /* FT_Group */
    field_info = NULL;
    field_info = soc_format_fieldinfo_get(unit, format, GROUP_IDf);
    if (field_info != NULL) {
        offset = field_info->bp;
        length = field_info->len;
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
 *     bcmi_ft_export_elements_check_search
 * Purpose:
 *     Search flowcheck in export elements list.
 * Parameters:
 *     unit - (IN) BCM device id
 *     check_id - (IN) Flowtracker Check
 *     list - (IN) list of export elements.
 *     count - (IN) number of export elements.
 *
 * Returns:
 *     TRUE if found FALSE otherwise
 */
int
bcmi_ft_export_elements_check_search(
    int unit,
    bcm_flowtracker_check_t check_id,
    void *list,
    int count)
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
 *      bcmi_fte_template_create_validate
 * Purpose:
 *      Validate the template and return the list supported by the
 *      device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *      in_export_elements - (IN) List of export elements intended to be in the template.
 *      num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *      out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_fte_template_create_validate(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int num_in_export_elements,
    bcm_flowtracker_export_element_info_t *in_export_elements,
    int num_out_export_elements,
    bcm_flowtracker_export_element_info_t *out_export_elements,
    int *actual_out_export_elements)
{
    int rv = BCM_E_NONE;
    int ix_in = 0, ix_out = 0, in_c = 0;
    int idx = 0;
    int info_size = 0;
    char info_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
    bcm_flowtracker_check_t check_id;
    bcmi_fte_template_record_info_t record;
    bcm_flowtracker_export_element_type_t info_elem;
    bcm_flowtracker_export_element_info_t *info_elem_in = NULL;
    bcm_flowtracker_export_element_info_t *info_elem_out = NULL;

    if ((num_in_export_elements <= 0) || (in_export_elements == NULL)) {
        return BCM_E_PARAM;
    }
    if ((num_out_export_elements > 0) && (out_export_elements == NULL)) {
        return BCM_E_PARAM;
    }

    /* Validate export element list of the group */
    rv = bcmi_ft_group_export_checks_validate(unit,
            flow_group_id, num_in_export_elements, in_export_elements);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&record, 0, sizeof(bcmi_fte_template_record_info_t));
    rv = bcmi_fte_template_record_info_populate(unit, flow_group_id, &record);
    BCM_IF_ERROR_RETURN(rv);

    if (record.info_depth == 0) {
        /* Faied to populate template info */
        return BCM_E_INTERNAL;
    }

    /* Mark info elements not requested by user as 'Reserved' */
    for (idx = 0; idx < record.info_depth; idx++) {

        if (record.info[idx].info_elem !=
                bcmFlowtrackerExportElementTypeReserved) {
            for (ix_in = 0; ix_in < num_in_export_elements; ix_in++) {
                info_elem_in = &(in_export_elements[ix_in]);

                /* If non-primary check, skip */
                if (info_elem_in->element ==
                        bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
                    check_id = info_elem_in->check_id;

                    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
                        in_c++;
                        break;
                    }
                }

                if ((record.info[idx].info_elem == info_elem_in->element) &&
                        (record.info[idx].check_id == info_elem_in->check_id)) {
                    in_c++;
                    break;
                }
            }
            if (ix_in == num_in_export_elements) {
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
        info_elem = record.info[idx].info_elem;

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

            /* Skip next $info_size 'Reserved' elements */
            idx += ((info_size / 8) - 1);
        }
        info_size = info_size / 8;

        /* Dump FT export element list */
        (void) bcmi_ft_export_elem_name_get(unit, info_elem, NULL, info_name);
        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "[TemplateValidate]Element Id: %2d (%20s)"
                        " | Size: %02d (0x%x)\n\r"), info_elem, info_name,
                    info_size, check_id));

        info_elem_out = NULL;
        if (ix_out < num_out_export_elements) {
            info_elem_out = &(out_export_elements[ix_out]);

            info_elem_out->element = info_elem;
            info_elem_out->data_size = info_size;
            info_elem_out->check_id = check_id;
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
 *      bcmi_ft_export_template_validate
 * Purpose:
 *      Validate the template and return the list supported by the
 *      device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *      in_export_elements - (IN) List of export elements intended to be in the template.
 *      num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *      out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ft_export_template_validate(
    int unit,
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
 *     bcmi_ft_template_create
 * Purpose:
 *     Create flowtracker export template.
 * Description :
 *     Routine to create a flowtracker template
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options for template creation
 *     template_id   - (OUT) FT template id.
 *     set_id - (IN) IPFIX template set id
 *     num_elements - (IN) Number of information elements.
 *     list_of_export_elements - (IN) List of IPFIX information elems.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_template_create(
    int unit,
    uint32 options,
    bcm_flowtracker_export_template_t *template_id,
    uint16 set_id,
    int num_elements,
    bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    int rv;
    bcm_flowtracker_export_template_t index = 0;
    bcmi_ft_template_info_t template_info;

    /*
     * Validate info elems
     */
    rv = bcmi_ft_template_validate(unit,
                                   num_elements,
                                   list_of_export_elements);
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
    template_info.num_elements = num_elements;

    sal_memcpy(template_info.export_elements,
               list_of_export_elements,
               (num_elements * sizeof(bcm_flowtracker_export_element_info_t)));

    sal_memcpy(BCMI_FT_TEMPLATE_INFO(unit, index),
               &template_info, sizeof(template_info));

    *template_id = index;

    return rv;
}

/*
 * Function:
 *     bcmi_ft_template_get
 * Purpose:
 *     Ftech flowtracker export template.
 * Description :
 *     Routine to get flowtracker template info.
 * Parameters:
 *     unit - (IN) BCM device id
 *     template_id   - (IN) FT template id.
 *     *set_id - (OUT) IPFIX template set id
 *     max_size - (IN) Max num of elements
 *     list_of_export_elements - (OUT) List of IPFIX information elems.
 *     num_elements - (OUT) Actual Number of information elements.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_template_get(
    int unit,
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
 *     bcmi_ft_template_destroy
 * Purpose:
 *     Destroy flowtracker export template.
 * Description :
 *     Routine to destroy a flowtracker template
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (OUT) FT template id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_template_destroy(
    int unit,
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
 *     bcmi_ft_flow_group_collector_add
 * Purpose:
 *     Attach a collector a flow group.
 * Description :
 *     Routine to associate a collector to an flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     group_id- (IN) FT group Id.
 *     coll_hw_id   - (OUT) FT Collector Id.
 *     template_id - (IN) IPFIX template Id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_collector_t coll_hw_id,
    bcm_flowtracker_export_template_t template_id)
{
    uint16 set_id;
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

    /* Update Group table with collector Id. */
    soc_mem_field32_modify(unit, BSC_KG_GROUP_TABLEm, flow_group_id,
                           COLLECTOR_IDf, coll_hw_id);
    ft_coll_info->in_use += 1;
    BCMI_FT_GROUP(unit, flow_group_id)->collector_id = coll_hw_id;

    /* Update Group table with template SET Id. */
    set_id = BCMI_FT_TEMPLATE_INFO(unit, template_id)->set_id;
    soc_mem_field32_modify(unit, BSC_KG_GROUP_TABLEm, flow_group_id,
                           SET_IDf, set_id);
    BCMI_FT_TEMPLATE_INFO(unit, template_id)->in_use += 1;
    BCMI_FT_GROUP(unit, flow_group_id)->template_id = template_id;


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_flow_group_collector_delete_check
 * Purpose:
 *     Check the conditions that needs to be satisfied
 *     before a collector is removed from Flowtracker_group.
 * Parameters:
 *     unit         - (IN) BCM device id
 *     id           - (IN) FT group Id.
 *     coll_hw_id   - (IN) FT Collector Id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_delete_check(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_t coll_hw_id)
{
    int rv;
    int ft_enable;
    int is_busy;
    int user_entry;
    uint32 flow_limit;
    uint64 age_out_count;
    uint64 flow_learnt_count;

    user_entry = (BCMI_FT_GROUP(unit, id)->flags
                     & BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY);
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
 *     bcmi_ft_flow_group_collector_delete
 * Purpose:
 *     Detach a collector a flow group.
 * Description :
 *     Routine to dissociate a collector to an flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     group_id- (IN) FT group Id.
 *     coll_hw_id   - (OUT) FT Collector Id.
 *     template_id - (IN) IPFIX template Id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_collector_t coll_hw_id,
    bcm_flowtracker_export_template_t template_id)
{
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

    soc_mem_field32_modify(unit, BSC_KG_GROUP_TABLEm, flow_group_id,
                           COLLECTOR_IDf, 0);
    ft_coll_info->in_use -= 1;
    BCMI_FT_GROUP(unit, flow_group_id)->collector_id = BCMI_FT_COLLECTOR_ID_INVALID;

    /* Update Group table with template SET Id. */
    soc_mem_field32_modify(unit, BSC_KG_GROUP_TABLEm, flow_group_id,
                           SET_IDf, 0);
    BCMI_FT_TEMPLATE_INFO(unit, template_id)->in_use -= 1;
    BCMI_FT_GROUP(unit, flow_group_id)->template_id = BCMI_FT_TEMPLATE_ID_INVALID;

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_flow_group_collector_get_all
 * Purpose:
 *     Get collector attached to a flow group.
 * Description :
 *     Routine to fetch collector and template id
 *     associated a flowtracker group.
 * Parameters:
 *     unit                 - (IN)  BCM device id
 *     group_id             - (IN)  FT group Id.
 *     max_list_size        - (IN)  max list of collectors
 *     list_of_collectors   - (OUT) FT Collector Id list.
 *     list_of_templates    - (OUT) FT Template Id list.
 *     list_size            - (IN)  Actual list size.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_flow_group_collector_get_all(
    int unit,
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
 *      bcmi_ft_group_collector_attach
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_group_collector_attach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id)
{
    int rv;
    bcmi_ft_collector_info_t *ft_coll_info;
    bcm_flowtracker_collector_t ix, first_free;

    bcm_flowtracker_collector_info_t collector_info;
    bcm_collector_export_profile_t export_profile_info;
    bcm_collector_info_t cmn_collector_info;

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

        collector_info.max_packet_length = export_profile_info.max_packet_length;

        ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, first_free);
        ft_coll_info->ipfix_hdr_use_custom = 1;
        ft_coll_info->ipfix_hdr = cmn_collector_info.ipfix;

        if (export_profile_info.flags &
            BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
            ft_coll_info->num_records = export_profile_info.num_records;
        }

        rv = bcmi_ft_collector_create(unit,
                                      BCM_FLOWTRACKER_COLLECTOR_WITH_ID,
                                      &first_free,
                                      &collector_info);
        BCM_IF_ERROR_RETURN(rv);

        ft_coll_info->cmn_coll_id = collector_id;

        if (collector_info.collector_type == bcmFlowtrackerCollectorLocal) {
            BCMI_FT_EXPORT_INFO(unit)->dma_collector_id = ft_coll_info->cmn_coll_id;
        }

        ft_coll_info->cmn_export_prof_id = export_profile_id;
        ix = first_free;
    }

    rv = bcmi_ft_flow_group_collector_add(unit,
                                          flow_group_id,
                                          ix,
                                          template_id);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_collector_ref_count_update(unit, collector_id, 1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: collector "
                      " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
        return rv;
    }
    rv = _bcm_esw_collector_user_update(unit, collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                      "FLOWTRACKER(unit %d) Error: collector "
                      " user update failed  %s\n"), unit, bcm_errmsg(rv)));
        return rv;
    }
    rv = _bcm_esw_collector_export_profile_ref_count_update(unit,
                                                            export_profile_id,
                                                            1);
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
 *      bcmi_ft_group_collector_detach
 * Purpose:
 *      Dis-associate flow group from a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_group_collector_detach(
        int unit,
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
 *      bcmi_ft_group_collector_attach_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_group_collector_attach_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_list_size, 
    bcm_flowtracker_collector_t *list_of_collectors, 
    int *list_of_export_profile_ids,
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *list_size)
{
    int result = BCM_E_UNAVAIL;

    return result;
}





/*
 * Function:
 *     bcmi_ft_export_record_register
 * Description:
 *     To register the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     callback      user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 */
int
bcmi_ft_export_record_register(
    int unit,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_collector_callback_options_t callback_options,
    bcm_flowtracker_export_record_cb_f callback_fn,
    void *userdata)
{
    int free_index, i;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl;
    bcmi_ft_cb_entry_t *entry;


    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return BCM_E_UNAVAIL;
    }

    if (callback_fn == NULL) {
        return BCM_E_PARAM;
    }

    if (BCMI_FT_CB_CTRL(unit) == NULL) {
        BCM_IF_ERROR_RETURN(bcmi_ft_export_cb_init(unit));
    }
    ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);

    if (BCMI_FT_EXPORT_INFO(unit)->dma_collector_id != collector_id) {
        return BCM_E_PARAM;
    }

    free_index = -1;
    for (i = 0; i < BCMI_FT_CB_MAX; i++) {
        entry = &ft_export_cb_ctrl->callback_entry[i];
        if (entry->state == BCMI_FT_CALLBACK_STATE_INVALID ||
            (ft_export_cb_ctrl->pid == SAL_THREAD_ERROR &&
             entry->state == BCMI_FT_CALLBACK_STATE_UNREGISTERED)) {
            if (free_index < 0) {
                free_index = i;
            }
        } else if ((entry->state == BCMI_FT_CALLBACK_STATE_ACTIVE) ||
                   (entry->state == BCMI_FT_CALLBACK_STATE_REGISTERED)) {
            if (entry->callback == callback_fn && entry->userdata == userdata) {
                return BCM_E_NONE;
            }
        }
    }

    if (free_index < 0) {
        return BCM_E_RESOURCE;
    }

    entry = &ft_export_cb_ctrl->callback_entry[free_index];
    entry->callback = callback_fn;
    entry->callback_options = callback_options;
    entry->userdata = userdata;
    entry->state = BCMI_FT_CALLBACK_STATE_REGISTERED;

    (void)bcma_ft_export_fifo_control(unit, ft_export_cb_ctrl->interval);

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_export_record_unregister
 * Description:
 *     To unregister the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     callback      user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcmi_ft_export_record_unregister(
    int unit,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_collector_callback_options_t callback_options,
    bcm_flowtracker_export_record_cb_f callback_fn)
{
    int i;
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl;
    bcmi_ft_cb_entry_t *entry;

    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return BCM_E_UNAVAIL;
    }

    if (BCMI_FT_CB_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }
    ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);

    for (i = 0; i < BCMI_FT_CB_MAX; i++) {
        entry = &ft_export_cb_ctrl->callback_entry[i];

        if (entry->callback == callback_fn) {
            entry->state = BCMI_FT_CALLBACK_STATE_UNREGISTERED;

            (void)bcma_ft_export_fifo_control(unit, ft_export_cb_ctrl->interval);
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
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
 *     bcmi_ft_export_dma
 * Description:
 *     A task thread to process IPFIX export FIFOs via FIFO DMA
 * Parameters:
 *     unit_vp          Device number
 * Return:
 *     None
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
 *     bcma_ft_export_fifo_control
 * Description:
 *     To start/stop ipfix export fifo processing thread
 * Parameters:
 *     unit            device number
 *     sal_usecs_t     polling interval, set to 0 to stop the thread
 * Return:
 *     BCM_E_NONE
 *     BCM_E_MEMORY
 *     BCM_E_UNAVAIL
 */
int
bcma_ft_export_fifo_control(int unit, sal_usecs_t interval)
{
    bcmi_ft_export_cb_ctrl_t *ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
    int pri;
    char name[16];

    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return BCM_E_UNAVAIL;
    }


    if (ft_export_cb_ctrl == NULL) {
        BCM_IF_ERROR_RETURN(bcmi_ft_export_cb_init(unit));
        ft_export_cb_ctrl = BCMI_FT_CB_CTRL(unit);
        if (ft_export_cb_ctrl == NULL) {
            return BCM_E_INTERNAL;
        }
    }

    sal_snprintf(name, sizeof(name), "bcmFtExportDma.%d", unit);

    ft_export_cb_ctrl->interval = interval;

    if (interval) {
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
 *      bcmi_ft_export_wb_alloc_size
 * Purpose:
 *      Allocate persistent info memory for flowtracker module
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
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
 *     bcmi_ft_export_warmboot_sync
 * Purpose:
 *     Sync flowtracker export state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_export_warmboot_sync(int unit, soc_scache_handle_t scache_handle)
{
    int ix, jx;
    int size;
    bcmi_ft_collector_info_t *coll_info;
    bcmi_ft_template_info_t *template_info;
    uint8 *ft_scache_ptr;

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
            BCMI_FT_WB_SCACHE_SYNC(ft_scache_ptr, &(template_info->export_elements[jx]));
        }
    }

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
 *     bcmi_ft_export_warmboot_recover
 * Purpose:
 *     Recover flowtracker export state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */

int
bcmi_ft_export_warmboot_recover(int unit, soc_scache_handle_t scache_handle)
{
    int ix, jx;
    int size;
    bcmi_ft_collector_info_t *coll_info;
    bcmi_ft_template_info_t *template_info;
    uint8 *ft_scache_ptr;

    /* Get the size to save ft export module warmboot state. */
    bcmi_ft_export_wb_alloc_size(unit, &size);


    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 size, &ft_scache_ptr,
                                 BCM_WB_FT_EXPORT_DEFAULT_VERSION, NULL));

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
            BCMI_FT_WB_SCACHE_RECOVER(ft_scache_ptr, &(template_info->export_elements[jx]));
        }
    }


    /* Export reinit */
    BCM_IF_ERROR_RETURN(bcmi_ft_export_reinit(unit));

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_export_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */

