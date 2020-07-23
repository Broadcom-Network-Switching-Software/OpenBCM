/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Flowtracker - Flow Tracking embedded APP interface
 * Purpose: APIs to configure flowtracker embedded app interface.
 */

#if defined(INCLUDE_FLOWTRACKER)

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/uc.h>
#include <shared/alloc.h>
#include <soc/shared/ft.h>
#include <soc/shared/ft_msg.h>
#include <soc/shared/ft_pack.h>
#include <soc/shared/shr_pkt.h>


#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/flowtracker.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/collector.h>


/* List of flowtracker export templates info */
_bcm_int_ft_export_template_info_t *ft_export_template_info_list[BCM_MAX_NUM_UNITS] = {0};
/* List of flowtracker collector info*/
_bcm_int_ft_collector_info_t *ft_collector_info_list[BCM_MAX_NUM_UNITS] = {0};
_bcm_int_ft_v2_collector_info_t *ft_v2_collector_info_list[BCM_MAX_NUM_UNITS] = {0};
_bcm_int_ft_v2_export_profile_info_t *ft_v2_export_profile_info_list[BCM_MAX_NUM_UNITS] = {0};

/* List of flowtracker flow_group info*/
_bcm_int_ft_flow_group_info_t *ft_flow_group_info_list[BCM_MAX_NUM_UNITS] = {0};

/* List of flowtracker elph_profile info*/
_bcm_int_ft_elph_profile_info_t *ft_elph_profile_info_list[BCM_MAX_NUM_UNITS] = {0};


/******************************************************
*                                                     *
*                 UTILITY FUNCTIONS                   *
 */

/*
 * Functions:
 *      _bcm_xgs5_ft_<header>_get
 * Purpose:
 *      The following set of _get() functions builds a given header for
 *      a given collector.
 * Parameters:
 *      collector_info -  (IN) Pointer to Collector information structure.
 *      <... other IN args ...>
 *      <header>  - (OUT) Returns header.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_xgs5_ft_udp_header_get(
                bcm_flowtracker_collector_info_t *collector_info,
                shr_udp_header_t *udp)
{
    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = collector_info->udp.src_port;
    udp->dst = collector_info->udp.dst_port;
    /* Do not set UDP length since this will be filled by
     * EAPP.
     */
    udp->length = 0;
    udp->sum = 0;    /* Calculated later */

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_v2_udp_header_get(
                bcm_collector_info_t *collector_info,
                shr_udp_header_t *udp)
{
    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = collector_info->udp.src_port;
    udp->dst = collector_info->udp.dst_port;
    /* Do not set UDP length since this will be filled by
     * EAPP.
     */
    udp->length = 0;
    udp->sum = 0;    /* Calculated later */

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_ipv4_header_get(uint8 protocol,
                              uint8 tos, uint8 ttl,
                              bcm_ip_t src_ip_addr, bcm_ip_t dst_ip_addr,
                              shr_ipv4_header_t *ip)
{
    uint8 buffer[SHR_IPV4_HEADER_LENGTH];

    sal_memset(ip, 0, sizeof(*ip));

    ip->version  = SHR_IPV4_VERSION;
    ip->h_length = SHR_IPV4_HEADER_LENGTH_WORDS;
    ip->dscp     = tos;
    ip->ecn      = 0;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ip->length   = 0;
    ip->id       = 0;
    ip->flags    = 0;
    ip->f_offset = 0;
    ip->ttl      = ttl;
    ip->protocol = protocol;
    ip->sum      = 0;
    ip->src      = src_ip_addr;
    ip->dst      = dst_ip_addr;

    /* Calculate IP header checksum */
    shr_ipv4_header_pack(buffer, ip);
    ip->sum = 0;

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_ipv6_header_get(uint8 protocol,
                             uint32 f_label, uint8 t_class, uint8 hop_limit,
                             bcm_ip6_t src_ip_addr, bcm_ip6_t dst_ip_addr,
                             shr_ipv6_header_t *ip)
{
    sal_memset(ip, 0, sizeof(*ip));
    ip->version  = SHR_IPV6_VERSION;
    ip->next_header = protocol;
    ip->t_class = t_class;
    ip->hop_limit = hop_limit;
    ip->f_label = f_label;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ip->p_length   = 0;

    /* Destination and Source IP address */
    sal_memcpy(ip->src, src_ip_addr, sizeof(bcm_ip6_t));
    sal_memcpy(ip->dst, dst_ip_addr, sizeof(bcm_ip6_t));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_ipv6_headers_get(bcm_flowtracker_collector_info_t *collector_info,
                             uint16 *etype,
                             shr_ipv6_header_t *ipv6)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;
    *etype = SHR_L2_ETYPE_IPV6;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ft_ipv6_header_get(ip_protocol,
                                      collector_info->ipv6.flow_label,
                                      collector_info->ipv6.traffic_class,
                                      collector_info->ipv6.hop_limit,
                                      collector_info->ipv6.src_ip,
                                      collector_info->ipv6.dst_ip,
                                      ipv6));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_v2_ipv6_headers_get(bcm_collector_info_t *collector_info,
                               uint16 *etype,
                               shr_ipv6_header_t *ipv6)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;
    *etype = SHR_L2_ETYPE_IPV6;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ft_ipv6_header_get(ip_protocol,
                                      collector_info->ipv6.flow_label,
                                      collector_info->ipv6.traffic_class,
                                      collector_info->ipv6.hop_limit,
                                      collector_info->ipv6.src_ip,
                                      collector_info->ipv6.dst_ip,
                                      ipv6));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_ipv4_headers_get(bcm_flowtracker_collector_info_t *collector_info,
                             uint16 *etype,
                             shr_ipv4_header_t *ipv4)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;
    *etype = SHR_L2_ETYPE_IPV4;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ft_ipv4_header_get( ip_protocol,
                                       collector_info->ipv4.dscp,
                                       collector_info->ipv4.ttl,
                                       collector_info->ipv4.src_ip,
                                       collector_info->ipv4.dst_ip,
                                       ipv4));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_v2_ipv4_headers_get(bcm_collector_info_t *collector_info,
                               uint16 *etype,
                               shr_ipv4_header_t *ipv4)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;
    *etype = SHR_L2_ETYPE_IPV4;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ft_ipv4_header_get(ip_protocol,
                                      collector_info->ipv4.dscp,
                                      collector_info->ipv4.ttl,
                                      collector_info->ipv4.src_ip,
                                      collector_info->ipv4.dst_ip,
                                      ipv4));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_l2_header_get(bcm_flowtracker_collector_info_t *collector_info,
                           uint16 etype, shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, collector_info->eth.dst_mac, SHR_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, collector_info->eth.src_mac, SHR_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag(s)
     */
    if (collector_info->eth.vlan_tag_structure == BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_UNTAGGED) { 
        /* Set to 0 to indicate untagged */
        l2->outer_vlan_tag.tpid = 0;
        l2->inner_vlan_tag.tpid = 0;
    } else {
        /* Fill outer vlan */
        if ((collector_info->eth.vlan_tag_structure == BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED) ||
            (collector_info->eth.vlan_tag_structure == BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_DOUBLE_TAGGED))
        {
            l2->outer_vlan_tag.tpid     = collector_info->eth.outer_tpid;
            l2->outer_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector_info->eth.outer_vlan_tag);
        } else {
            return BCM_E_PARAM;
        }
        /* Fill inner vlan if double tagged */
        if (collector_info->eth.vlan_tag_structure == BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
            l2->inner_vlan_tag.tpid     = collector_info->eth.inner_tpid;
            l2->inner_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector_info->eth.inner_vlan_tag);
        } else {
            l2->inner_vlan_tag.tpid = 0;  /* Set to 0 to indicate not inner tagged */
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ft_v2_l2_header_get(bcm_collector_info_t *collector_info,
                              uint16 etype, shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, collector_info->eth.dst_mac, SHR_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, collector_info->eth.src_mac, SHR_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag(s)
     */
    if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_UNTAGGED) { 
        /* Set to 0 to indicate untagged */
        l2->outer_vlan_tag.tpid = 0;
        l2->inner_vlan_tag.tpid = 0;
    } else {
        /* Fill outer vlan */
        if ((collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED) ||
            (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED))
        {
            l2->outer_vlan_tag.tpid     = collector_info->eth.outer_tpid;
            l2->outer_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector_info->eth.outer_vlan_tag);
        } else {
            return BCM_E_PARAM;
        }
        /* Fill inner vlan if double tagged */
        if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
            l2->inner_vlan_tag.tpid     = collector_info->eth.inner_tpid;
            l2->inner_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector_info->eth.inner_vlan_tag);
        } else {
            l2->inner_vlan_tag.tpid = 0;  /* Set to 0 to indicate not inner tagged */
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_collector_encap_build_pack
 * Purpose:
 *      Builds and packs the Flowtracker export packet
 *      encapsulation for a given collector
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      collector_id         - (IN)  Collector Id
 *      collector_create_msg - (OUT) Message which contains the encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only all the
 *      encapsulation headers and does not include
 *      the IPFIX control packet.
 */
STATIC int
_bcm_xgs5_ft_eapp_collector_encap_build_pack(
                       int unit,
                       int collector_id,
                       shr_ft_msg_ctrl_collector_create_t *collector_create_msg)
{
    uint8          *buffer  = collector_create_msg->encap;
    uint8          *cur_ptr;
    uint16         etype = 0;
    shr_udp_header_t  udp;
    shr_ipv4_header_t ipv4;
    shr_ipv6_header_t ipv6;
    shr_l2_header_t   l2;
    uint16         ip_offset = 0;
    uint16         udp_offset = 0;
    _bcm_int_ft_collector_info_t collector_int_info;
    int rv;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    sal_memset(&collector_int_info, 0, sizeof(collector_int_info));
    /* Get the collector info for this collector id */
    rv = _bcm_xgs5_ft_collector_list_collector_get(unit, collector_id, &collector_int_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if ((!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR)) &&
        (collector_int_info.collector_info.transport_type ==
                      bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                  "QCM only support IPv6\n")));
        return BCM_E_PARAM;
    }

    sal_memset(collector_create_msg, 0, sizeof(shr_ft_msg_ctrl_collector_create_t));
    collector_create_msg->id = collector_id;
    collector_create_msg->mtu = collector_int_info.collector_info.max_packet_length;

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));
    sal_memset(&l2, 0, sizeof(l2));

    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if ((collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) ||
        (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ft_udp_header_get(&(collector_int_info.collector_info), &udp));
    }

    if (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ft_ipv4_headers_get(&(collector_int_info.collector_info),
                                          &etype,
                                          &ipv4));
    }

    if (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ft_ipv6_headers_get(&(collector_int_info.collector_info),
                                          &etype,
                                          &ipv6));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ft_l2_header_get(&(collector_int_info.collector_info),
                                    etype, &l2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* L2 Header is always present */
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    if (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) {
        /*
         * IP offset
         */
        ip_offset = cur_ptr - buffer;

        cur_ptr = shr_ipv4_header_pack(cur_ptr, &ipv4);
    }

    if (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp) {
        /*
         * IP offset
         */
        ip_offset = cur_ptr - buffer;

        cur_ptr = shr_ipv6_header_pack(cur_ptr, &ipv6);
    }

    if ((collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) ||
        (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp)) {
        /*
         * UDP offset
         */
        udp_offset = cur_ptr - buffer;
        cur_ptr = shr_udp_header_pack(cur_ptr, &udp);
    }

    /* Set Flowtracker export pkt encapsulation length */
    collector_create_msg->encap_length = cur_ptr - buffer;

    /* Set Flowtracker export pkt IP base offset */
    collector_create_msg->ip_offset = ip_offset;

    /* Set Flowtracker export pkt IP Base checksum */
    collector_create_msg->ip_base_checksum =
             shr_initial_chksum_get(SHR_IPV4_HEADER_LENGTH, buffer + ip_offset);

    /* Set Flowtracker export pkt UDP Base checksum */
    if (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) {
        collector_create_msg->udp_base_checksum =
                              shr_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);
    }

    if (collector_int_info.collector_info.transport_type ==
                     bcmFlowtrackerCollectorTransportTypeIpfixIpv6Udp) {
        collector_create_msg->udp_base_checksum =
                              shr_udp_initial_checksum_get(1, NULL, &ipv6, NULL, &udp);
    }

    /* Set Flowtracker export pkt UDP base offset */
    collector_create_msg->udp_offset = udp_offset;


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ft_v2_eapp_collector_encap_build_pack
 * Purpose:
 *      Builds and packs the Flowtracker export packet
 *      encapsulation for a given collector
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      collector_id        -  (IN) Collector Id
 *      collector_create_msg - (OUT) Message which contains the encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only all the
 *      encapsulation headers and does not include
 *      the IPFIX control packet.
 */
STATIC int
_bcm_xgs5_ft_v2_eapp_collector_encap_build_pack(
                       int unit,
                       bcm_collector_t collector_id,
                       int export_profile_id,
                       shr_ft_msg_ctrl_collector_create_t *collector_create_msg)
{
    uint8             *buffer  = collector_create_msg->encap;
    uint8             *cur_ptr;
    uint16            etype = 0;
    shr_udp_header_t  udp;
    shr_ipv4_header_t ipv4;
    shr_ipv6_header_t ipv6;
    shr_l2_header_t   l2;
    uint16            ip_offset = 0;
    uint16            udp_offset = 0;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t export_profile_info;
    int coll_int_id;
    int rv;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Get the collector internal info for this collector id */
    rv = _bcm_xgs5_ft_v2_collector_list_collector_get(unit, collector_id,
                                                      &coll_int_id);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Get the collector info */
    bcm_collector_info_t_init(&collector_info);
    rv = bcm_esw_collector_get(unit, collector_id, &collector_info);
    BCM_IF_ERROR_RETURN(rv);

    if ((!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR)) &&
        (collector_info.transport_type == bcmCollectorTransportTypeIpv6Udp)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                  "QCM only support IPv6\n")));
        return BCM_E_PARAM;
    }


    /* Get the export profile info */
    bcm_collector_export_profile_t_init(&export_profile_info);
    rv = bcm_esw_collector_export_profile_get(unit,
                                              export_profile_id,
                                              &export_profile_info);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(collector_create_msg, 0, sizeof(shr_ft_msg_ctrl_collector_create_t));
    collector_create_msg->id = coll_int_id;
    collector_create_msg->mtu = export_profile_info.max_packet_length;

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));
    sal_memset(&l2, 0, sizeof(l2));

    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if ((collector_info.transport_type == bcmCollectorTransportTypeIpv4Udp) ||
        (collector_info.transport_type == bcmCollectorTransportTypeIpv6Udp)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ft_v2_udp_header_get(&collector_info, &udp));
    }

    if (collector_info.transport_type == bcmCollectorTransportTypeIpv4Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ft_v2_ipv4_headers_get(&collector_info,
                                          &etype,
                                          &ipv4));
    }

    if (collector_info.transport_type == bcmCollectorTransportTypeIpv6Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ft_v2_ipv6_headers_get(&collector_info,
                                          &etype,
                                          &ipv6));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ft_v2_l2_header_get(&collector_info, etype, &l2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* L2 Header is always present */
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    if (collector_info.transport_type == bcmCollectorTransportTypeIpv4Udp) {
        /*
         * IPv4 offset
         */
        ip_offset = cur_ptr - buffer;
        cur_ptr = shr_ipv4_header_pack(cur_ptr, &ipv4);
    }

    if (collector_info.transport_type == bcmCollectorTransportTypeIpv6Udp) {
        /*
         * IPv6 offset
         */
        ip_offset = cur_ptr - buffer;
        cur_ptr = shr_ipv6_header_pack(cur_ptr, &ipv6);
    }

    if ((collector_info.transport_type == bcmCollectorTransportTypeIpv4Udp) ||
        (collector_info.transport_type == bcmCollectorTransportTypeIpv6Udp)) {
        /*
         * UDP offset
         */
        udp_offset = cur_ptr - buffer;
        cur_ptr = shr_udp_header_pack(cur_ptr, &udp);
    }


    /* Set Flowtracker export pkt encapsulation length */
    collector_create_msg->encap_length = cur_ptr - buffer;

    /* Set Flowtracker export pkt IP base offset */
    collector_create_msg->ip_offset = ip_offset;

    /* Set Flowtracker export pkt IP Base checksum */
    collector_create_msg->ip_base_checksum =
             shr_initial_chksum_get(SHR_IPV4_HEADER_LENGTH, buffer + ip_offset);

    /* Set Flowtracker export pkt UDP Base checksum */
    if (collector_info.transport_type == bcmCollectorTransportTypeIpv4Udp) {
        collector_create_msg->udp_base_checksum =
                              shr_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);
    }

    if (collector_info.transport_type == bcmCollectorTransportTypeIpv6Udp) {
        collector_create_msg->udp_base_checksum =
                              shr_udp_initial_checksum_get(1, NULL, &ipv6, NULL, &udp);
    }

    if (collector_info.udp.flags & BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE) {
        collector_create_msg->flags |= SHR_FT_MSG_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE;
    }

    /* Set Flowtracker export pkt UDP base offset */
    collector_create_msg->udp_offset = udp_offset;

    if (export_profile_info.wire_format == bcmCollectorWireFormatProtoBuf) {
        /* Set Flowtracker export pkt Component ID */
        collector_create_msg->component_id = collector_info.protobuf.component_id;

        /* Set Flowtracker export pkt System ID length */
        collector_create_msg->system_id_length = collector_info.protobuf.system_id_length;

        /* Set Flowtracker export pkt System ID */
        memcpy(&collector_create_msg->system_id, &collector_info.protobuf.system_id,
               collector_info.protobuf.system_id_length);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_template_encap_build_pack
 * Purpose:
 *      Build the template set encap
 * Parameters:
 *      template_int_info - (IN)   Template Information
 *      encap_length      - (OUT)  Encap length
 *      encap             - (OUT)  Encap
 * Returns:
 *      None
 */
STATIC void
_bcm_xgs5_ft_eapp_template_encap_build_pack(
                  int unit,
                  _bcm_int_ft_export_template_info_t *template_int_info,
                  uint16 *encap_length, uint8 *buf)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8  *cur_ptr = buf;
    uint16  element_id;
    int     i;

    /*
     * +----------------------+------------------+
     * |     Bits 0...15      |   Bits 16...31   |
     * +----------------------+------------------+
     * | Set ID = 2(Template) |    Set Length    |
     * +----------------------+------------------+
     * |     Template ID      | Number of fields |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     */

    SHR_PKT_PACK_U16(cur_ptr, FT_INT_TEMPLATE_XMIT_SET_ID);

    /* Skip over length */
    cur_ptr += 2;

    SHR_PKT_PACK_U16(cur_ptr, template_int_info->set_id);
    SHR_PKT_PACK_U16(cur_ptr, template_int_info->num_export_elements);

    for (i = 0; i < template_int_info->num_export_elements; i++) {
        if (template_int_info->elements[i].enterprise == 0) {
            /* Non enterprise element, format below */
            /*
             * +-+--------------------+------------------+
             * |0|      1...15        |     16...31      |
             * +-+--------------------+------------------+
             * |0|      Type          |   Field Length   |
             * +----------------------+------------------+
             */
            SHR_PKT_PACK_U16(cur_ptr, template_int_info->elements[i].id);
            SHR_PKT_PACK_U16(cur_ptr, template_int_info->elements[i].data_size);
        } else {
            /* Enterprise specific */
            /*
             * +-+--------------------+------------------+
             * |0|      1...15        |     16...31      |
             * +-+--------------------+------------------+
             * |1|      Type          |   Field Length   |
             * +----------------------+------------------+
             * |             Enterprise number           |
             * +----------------------+------------------+
             */
            /* Set the enterprise bit */
            element_id = (1 << 15) | template_int_info->elements[i].id;
            SHR_PKT_PACK_U16(cur_ptr, element_id);
            SHR_PKT_PACK_U16(cur_ptr, template_int_info->elements[i].data_size);
            SHR_PKT_PACK_U32(cur_ptr, ft_info->enterprise_number);
        }
    }

    *encap_length = cur_ptr - buf;
}
/******************************************************
*                                                     *
*        EXPORT TEMPLATE LIST MANAGEMENT FUNCTIONS    *
 */

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_template_add
 * Purpose:
 *     Add a template to the template list.
 * Parameters:
 *     unit              - (IN) BCM device number
 *     template_id       - (IN) The template ID
 *     template_int_info - (IN) Template info to be added
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_template_list_template_add(int unit,
                           bcm_flowtracker_export_template_t template_id,
                           _bcm_int_ft_export_template_info_t *template_int_info)
{
    _bcm_int_ft_export_template_info_t *template_info_list_node =
                   &(ft_export_template_info_list[unit][template_id]);

    if (template_info_list_node->template_id != 0) {
        /* Not free */
        return BCM_E_EXISTS;
    }

    template_int_info->template_id = template_id;
    memcpy(template_info_list_node, template_int_info,
           sizeof(_bcm_int_ft_export_template_info_t));


    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit,
                        "FT(unit %d) Info: %s" 
                        " (Template=0x%x).\n"), unit, FUNCTION_NAME(),
                         template_id));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_template_delete
 * Purpose:
 *     Delete a template from the template list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     template_id   - (IN) The template ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_template_list_template_delete (int unit,
                              bcm_flowtracker_export_template_t template_id)
{
    _bcm_int_ft_export_template_info_t *template_info_list_node =
        &(ft_export_template_info_list[unit][template_id]);

    if (template_info_list_node->template_id != template_id) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: Template is not present"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(template_info_list_node, 0,
                            sizeof(_bcm_int_ft_export_template_info_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_template_modify
 * Purpose:
 *     Modfiy a template in the template list.
 * Parameters:
 *     unit            - (IN) BCM device number
 *     template_id     - (IN) Template Id
 *     template_info   - (IN) Template informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_template_list_template_modify(
                               int unit,
                               bcm_flowtracker_export_template_t template_id,
                               _bcm_int_ft_export_template_info_t *template_info)
{
    _bcm_int_ft_export_template_info_t *list_node =
                              &(ft_export_template_info_list[unit][template_id]);

    if (list_node->template_id != template_id) {
        /* Not found */
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: Template ID does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(list_node, template_info,
               sizeof(_bcm_int_ft_export_template_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_template_get
 * Purpose:
 *     Get a template from the template list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     template_id   - (IN) The template ID
 *     template_info - (OUT) template information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_template_list_template_get (int unit,
                              bcm_flowtracker_export_template_t template_id,
                              _bcm_int_ft_export_template_info_t *template_info)
{
    _bcm_int_ft_export_template_info_t *list_node =
                  &(ft_export_template_info_list[unit][template_id]);

    if (list_node->template_id == template_id) {
        sal_memcpy(template_info, list_node, sizeof(_bcm_int_ft_export_template_info_t));
    } else {
        return (BCM_E_NOT_FOUND);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_destroy
 * Purpose:
 *     Destroy the template list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_template_list_destroy (int unit)
{
    if (ft_export_template_info_list[unit] != NULL) {
        sal_free(ft_export_template_info_list[unit]);
    }
    ft_export_template_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_create
 * Purpose:
 *     Create the template list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_template_list_create (int unit)
{
    if (ft_export_template_info_list[unit] == NULL) {
        /* Allocate template list */
        _BCM_FT_ALLOC(ft_export_template_info_list[unit], _bcm_int_ft_export_template_info_t,
                      sizeof(_bcm_int_ft_export_template_info_t) *
                         (BCM_INT_FT_MAX_TEMPLATES + BCM_INT_FT_TEMPLATE_START_ID),
                      "template list");
    }

    if (ft_export_template_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    return BCM_E_NONE;
}

/******************************************************
*                                                     *
*        COLLECTOR LIST MANAGEMENT FUNCTIONS          *
 */

/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_collector_add
 * Purpose:
 *     Add a collector to the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     collector_info - (IN) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_collector_list_collector_add(int unit,
                           bcm_flowtracker_collector_t collector_id,
                           bcm_flowtracker_collector_info_t *collector_info)
{
    _bcm_int_ft_collector_info_t *collector_info_list_node =
        &(ft_collector_info_list[unit][collector_id]);

    if (collector_info_list_node->collector_id != 0) {
        return BCM_E_EXISTS;
    }
    collector_info_list_node->collector_id = collector_id;

    sal_memcpy(&(collector_info_list_node->collector_info), collector_info,
               sizeof(bcm_flowtracker_collector_info_t));

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit,
                        "FT(unit %d) Info: %s" 
                        " (Collector=0x%x).\n"), unit, FUNCTION_NAME(),
                         collector_id));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_collector_delete
 * Purpose:
 *     Delete a collector from the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_collector_list_collector_delete (int unit,
                              bcm_flowtracker_collector_t collector_id)
{
    _bcm_int_ft_collector_info_t *collector_info_list_node =
        &(ft_collector_info_list[unit][collector_id]);

    if (collector_info_list_node->collector_id != collector_id) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: collector ID does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(collector_info_list_node, 0,
                                 sizeof(_bcm_int_ft_collector_info_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_collector_modify
 * Purpose:
 *     Modfiy a collector in the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     collector_info - (IN) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_collector_list_collector_modify(int unit,
                           bcm_flowtracker_collector_t collector_id,
                           bcm_flowtracker_collector_info_t *collector_info)
{
    _bcm_int_ft_collector_info_t *list_node =
        &(ft_collector_info_list[unit][collector_id]);

    if (list_node->collector_id != collector_id) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: collector ID does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(&(list_node->collector_info), collector_info,
               sizeof(bcm_flowtracker_collector_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_collector_get
 * Purpose:
 *     Get a collector from the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     collector_info - (OUT) collector information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_collector_list_collector_get (int unit,
                              bcm_flowtracker_collector_t collector_id,
                              _bcm_int_ft_collector_info_t *collector_info)
{
    _bcm_int_ft_collector_info_t *list_node =
        &(ft_collector_info_list[unit][collector_id]);

    if (list_node->collector_id != collector_id) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: collector ID does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(collector_info, list_node, 
                sizeof(_bcm_int_ft_collector_info_t));


    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_destroy
 * Purpose:
 *     Destroy the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_collector_list_destroy (int unit)
{
    if (ft_collector_info_list[unit] != NULL) {
        sal_free(ft_collector_info_list[unit]);
    }

    ft_collector_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_create
 * Purpose:
 *     Create the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_collector_list_create (int unit)
{
    if (ft_collector_info_list[unit] == NULL) {
        _BCM_FT_ALLOC(ft_collector_info_list[unit], _bcm_int_ft_collector_info_t,
                      sizeof(_bcm_int_ft_collector_info_t) *
                       (BCM_INT_FT_MAX_COLLECTORS + BCM_INT_FT_COLLECTOR_START_ID),
                "collector list");

    }

    if (ft_collector_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    return BCM_E_NONE;
}

/*******************************************************
 *                                                     *
 *        FTv2 COLLECTOR LIST MANAGEMENT FUNCTIONS     *
 */

/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_collector_add
 * Purpose:
 *     Add a collector to the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id  - (IN) The collector ID
 *     coll_int_id   - (IN) Collector internal Id
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_collector_add(int unit,
                                             bcm_collector_t collector_id,
                                             int coll_int_id)
{
    _bcm_int_ft_v2_collector_info_t *collector_info =
                             &(ft_v2_collector_info_list[unit][collector_id]);

    if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if (collector_info->int_id != 0) {
        return BCM_E_EXISTS;
    }

    collector_info->int_id = coll_int_id;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_collector_delete
 * Purpose:
 *     Delete a collector from the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id  - (IN) The collector ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_collector_delete(int unit,
                                                bcm_collector_t collector_id)
{
    _bcm_int_ft_v2_collector_info_t *collector_info =
                            &(ft_v2_collector_info_list[unit][collector_id]);

    if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if (collector_info->int_id == 0) {
        return BCM_E_NOT_FOUND;
    }

    collector_info->int_id = 0;

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_collector_get
 * Purpose:
 *     Get a collector from the collector list.
 * Parameters:
 *     unit          - (IN)  BCM device number
 *     collector_id  - (IN)  The collector ID
 *     collint_id    - (OUT) Collector Internal Id
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_collector_get(int unit,
                                             bcm_collector_t collector_id,
                                             int *coll_int_id)
{
    _bcm_int_ft_v2_collector_info_t *collector_info =
                            &(ft_v2_collector_info_list[unit][collector_id]);

    if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if (collector_info->int_id == 0) {
        return BCM_E_NOT_FOUND;
    }

    *coll_int_id = collector_info->int_id;

    return BCM_E_NONE;
}



/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_collector_in_use
 * Purpose:
 *     Check if a collector is attached to any flow groups
 * Parameters:
 *     unit           - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     coll_int_id    - (OUT) Internal collector ID
 * Returns:
 *     None
 */
void
_bcm_xgs5_ft_v2_collector_list_collector_in_use(int unit,
                                                bcm_collector_t collector_id,
                                                int *in_use)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_info = NULL;
    int flow_group_id;

    *in_use = 0;
    for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
         flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
         flow_group_id++) {
        flow_group_info = &(ft_flow_group_info_list[unit][flow_group_id]);

        if (flow_group_info->flow_group_id != flow_group_id) {
            continue;
        }

        if (flow_group_info->collector_id == collector_id) {
            *in_use = 1;
            return;
        }
    }
}

/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_destroy
 * Purpose:
 *     Destroy the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_destroy(int unit)
{
    if (ft_v2_collector_info_list[unit] != NULL) {
        sal_free(ft_v2_collector_info_list[unit]);
    }

    ft_v2_collector_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_create
 * Purpose:
 *     Destroy the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_create(int unit)
{
    if (ft_v2_collector_info_list[unit] == NULL) {
        _BCM_FT_ALLOC(ft_v2_collector_info_list[unit], _bcm_int_ft_v2_collector_info_t,
                      sizeof(_bcm_int_ft_v2_collector_info_t) *
                       (BCM_INT_COLLECTOR_COLLECTOR_START_ID + BCM_INT_COLLECTOR_MAX_COLLECTORS),
                "collector list");

    }

    if (ft_v2_collector_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_export_profile_add
 * Purpose:
 *     Add a export profile to the list.
 * Parameters:
 *     unit                  - (IN) BCM device number
 *     export_profile_id     - (IN) Export profile Id
 *     export_profile_int_id - (IN) Export profile Internal Id
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_export_profile_add(int unit,
                                                  int export_profile_id,
                                                  int export_profile_int_id)
{
    _bcm_int_ft_v2_export_profile_info_t *export_profile_info =
                 &(ft_v2_export_profile_info_list[unit][export_profile_id]);

    if (export_profile_id > BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID) {
        return BCM_E_PARAM;
    }

    if (export_profile_info->int_id != 0) {
        return BCM_E_EXISTS;
    }

    export_profile_info->int_id = export_profile_int_id;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_export_profile_delete
 * Purpose:
 *     Delete an export profile from the list
 * Parameters:
 *     unit              - (IN) BCM device number
 *     export_profile_id - (IN) export_profile ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_export_profile_delete(int unit,
                                                     int export_profile_id)
{
    _bcm_int_ft_v2_export_profile_info_t *export_profile_info =
                 &(ft_v2_export_profile_info_list[unit][export_profile_id]);

    if (export_profile_id > BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID) {
        return BCM_E_PARAM;
    }

    if (export_profile_info->int_id == 0) {
        return BCM_E_NOT_FOUND;
    }

    export_profile_info->int_id = 0;

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_export_profile_get
 * Purpose:
 *     Get a collector from the collector list.
 * Parameters:
 *     unit                   - (IN)  BCM device number
 *     export_profile_id      - (IN)  export_profile ID
 *     export_profile_int_id  - (IN)  export_profile internal ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_export_profile_get(int unit,
                                                  int export_profile_id,
                                                  int *export_profile_int_id)
{
    _bcm_int_ft_v2_export_profile_info_t *export_profile_info =
                 &(ft_v2_export_profile_info_list[unit][export_profile_id]);

    if (export_profile_id > BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID) {
        return BCM_E_PARAM;
    }

    if (export_profile_info->int_id == 0) {
        return BCM_E_NOT_FOUND;
    }

    *export_profile_int_id = export_profile_info->int_id;

    return BCM_E_NONE;
}



/*
 * Function:
 *     _bcm_xgs5_ft_v2_collector_list_export_profile_in_use
 * Purpose:
 *     Check if a export_profile is attached to any flow groups
 * Parameters:
 *     unit              - (IN)  BCM device number
 *     export_profile_id - (IN)  export_profile ID
 *     in_use            - (OUT) Indicates if the export profile is in use
 * Returns:
 *     None
 */
void
_bcm_xgs5_ft_v2_collector_list_export_profile_in_use(int unit,
                                                     int export_profile_id,
                                                     int *in_use)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_info = NULL;
    int flow_group_id;

    *in_use = 0;
    for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
         flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
         flow_group_id++) {
        flow_group_info = &(ft_flow_group_info_list[unit][flow_group_id]);

        if (flow_group_info->flow_group_id != flow_group_id) {
            continue;
        }

        if (flow_group_info->export_profile_id == export_profile_id) {
            *in_use = 1;
            return;
        }
    }
}


/*
 * Function:
 *     _bcm_xgs5_ft_v2_export_profile_list_destroy
 * Purpose:
 *     Destroy the export_profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_export_profile_destroy(int unit)
{
    if (ft_v2_export_profile_info_list[unit] != NULL) {
        sal_free(ft_v2_export_profile_info_list[unit]);
    }

    ft_v2_export_profile_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_v2_export_profile_list_create
 * Purpose:
 *     Destroy the export_profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_v2_collector_list_export_profile_create(int unit)
{
    if (ft_v2_export_profile_info_list[unit] == NULL) {
        _BCM_FT_ALLOC(ft_v2_export_profile_info_list[unit], _bcm_int_ft_v2_export_profile_info_t,
                      sizeof(_bcm_int_ft_v2_export_profile_info_t) *
                       (BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID +
                                           BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES),
                "export_profile list");

    }

    if (ft_v2_export_profile_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    return BCM_E_NONE;
}

/******************************************************
*                                                     *
*        FLOW GROUP LIST MANAGEMENT FUNCTIONS         *
 */

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_flow_group_add
 * Purpose:
 *     Add a flow_group to the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     flow_group_id   - (IN) The flow_group ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_flow_group_add(int unit,
                                            bcm_flowtracker_group_t flow_group_id)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_info_list_node =
        &(ft_flow_group_info_list[unit][flow_group_id]);

    if (flow_group_info_list_node->flow_group_id != 0) {
        /* Not free */
        return BCM_E_EXISTS;
    }

    flow_group_info_list_node->flow_group_id = flow_group_id;
    if (ft_info->cfg_mode >= SHR_FT_CFG_MODE_V2) {
        flow_group_info_list_node->collector_id =
                                 BCM_INT_COLLECTOR_INVALID_COLLECTOR_ID;
        flow_group_info_list_node->export_profile_id =
                                BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID;
    }

    LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
            (BSL_META_U(unit,
                        "FT(unit %d) Info: %s" 
                        " (Flow_group=0x%x).\n"), unit, FUNCTION_NAME(),
                         flow_group_id));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_flow_group_delete
 * Purpose:
 *     Delete a flow_group from the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     flow_group_id   - (IN) The flow_group ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_flow_group_delete (int unit,
                              bcm_flowtracker_group_t flow_group_id)
{
    _bcm_int_ft_flow_group_info_t *flow_group_info_list_node =
        &(ft_flow_group_info_list[unit][flow_group_id]);

    if (flow_group_info_list_node->flow_group_id != flow_group_id) {
        /* Not found */
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: flow group ID does not exist"), unit));
        return BCM_E_NOT_FOUND;
    }

    sal_memset(flow_group_info_list_node, 0,
                                 sizeof(_bcm_int_ft_flow_group_info_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_flow_group_modify
 * Purpose:
 *     Modfiy a flow_group in the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     flow_group_id   - (IN) The flow_group ID
 *     flow_group_info - (IN) flow_group informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_flow_group_modify(
                           int unit,
                           bcm_flowtracker_group_t flow_group_id,
                           _bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    _bcm_int_ft_flow_group_info_t *list_node =
        &(ft_flow_group_info_list[unit][flow_group_id]);

    if (list_node->flow_group_id != flow_group_id) {
        /* Not found */
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: flow_group ID does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(list_node, flow_group_int_info, 
            sizeof(_bcm_int_ft_flow_group_info_t));


    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_flow_group_get
 * Purpose:
 *     Get a flow_group from the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     flow_group_id   - (IN) The flow_group ID
 *     flow_group_int_info - (OUT) flow_group information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_flow_group_get (
                              int unit,
                              bcm_flowtracker_group_t flow_group_id,
                              _bcm_int_ft_flow_group_info_t *flow_group_int_info)
{
    _bcm_int_ft_flow_group_info_t *list_node =
        &(ft_flow_group_info_list[unit][flow_group_id]);

    if (list_node->flow_group_id != flow_group_id) {
        /* Not found */
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Info: flow_group ID does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }
    sal_memcpy(flow_group_int_info, list_node,
               sizeof(_bcm_int_ft_flow_group_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_xgs5_ft_flow_group_list_get_all_ids 
 * Purpose:
 *     Get list of flow group ids configured.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     flow_group_id_list   - (OUT) The list of flow_group IDs.
 *                            The caller is expected to have allocated it
 *                            for max number of flow groups.
 *     num_flow_groups      - (OUT) The number of flow groups configured.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_get_all_ids(
            int unit,
            bcm_flowtracker_group_t *flow_group_id_list,
            int *num_flow_groups)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *list_node = NULL;
    int flow_group_id = 0, cnt = 0;

    for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
         flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
         flow_group_id++) {
        list_node = &(ft_flow_group_info_list[unit][flow_group_id]);
        /* Non zero flow_group_id indicates configured flow groups.*/
        if (list_node->flow_group_id != 0) {
            flow_group_id_list[cnt] = list_node->flow_group_id;
            cnt++;
        }
    }
    *num_flow_groups = cnt;
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_xgs5_ft_flow_group_list_collector_in_use_check 
 * Purpose:
 *     Find if any flow group in the flow_group_list uses the passed collector.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) collector_id
 * Returns:
 *     BCM_E_EXISTS - If collector_id is used by any of the flow groups.
 *     BCM_E_NONE   - otherwise.
 */
int
_bcm_xgs5_ft_flow_group_list_collector_in_use_check(int unit, 
                    bcm_flowtracker_collector_t collector_id)
{
    _bcm_int_ft_flow_group_info_t *list_node = NULL;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int flow_group_id;

    for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
         flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
         flow_group_id++) { 
        list_node = &(ft_flow_group_info_list[unit][flow_group_id]);
        /* Valid flow group ids have non zero flow group ID. */
        if (list_node->flow_group_id != 0) {
            if (list_node->collector_id == collector_id) {
                return BCM_E_EXISTS;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_xgs5_ft_flow_group_list_template_in_use_check 
 * Purpose:
 *     Find if any flow group in the flow_group_list uses the passed template.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     template_id   - (IN) template_id
 * Returns:
 *     BCM_E_EXISTS - If template_id is used by any of the flow groups.
 *     BCM_E_NONE   - otherwise.
 */
int
_bcm_xgs5_ft_flow_group_list_template_in_use_check(int unit, 
                    bcm_flowtracker_export_template_t template_id)
{
    _bcm_int_ft_flow_group_info_t *list_node = NULL;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int flow_group_id;

    for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
         flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
         flow_group_id++) { 
        list_node = &(ft_flow_group_info_list[unit][flow_group_id]);
        /* Valid flow group ids have non zero flow group ID. */
        if (list_node->flow_group_id != 0) {
            if (list_node->template_id == template_id) {
                return BCM_E_EXISTS;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_xgs5_ft_flow_group_list_elph_profile_in_use_check
 * Purpose:
 *     Find if any flow group in the flow_group_list uses the elephant profile
 * Parameters:
 *     unit         - (IN) BCM device number
 *     profile_id   - (IN) profile id
 * Returns:
 *     BCM_E_EXISTS - If elephant profile id is used by any of the flow groups.
 *     BCM_E_NONE   - otherwise.
 */
int
_bcm_xgs5_ft_flow_group_list_elph_profile_in_use_check(int unit,
                                    bcm_flowtracker_export_template_t profile_id)
{
    _bcm_int_ft_flow_group_info_t *list_node = NULL;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int flow_group_id;

    for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
         flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
         flow_group_id++) {
        list_node = &(ft_flow_group_info_list[unit][flow_group_id]);
        /* Valid flow group ids have non zero flow group ID. */
        if (list_node->flow_group_id != 0) {
            if (list_node->elph_profile_id == profile_id) {
                return BCM_E_EXISTS;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_destroy
 * Purpose:
 *     Destroy the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_destroy (int unit)
{
    _bcm_int_ft_flow_group_info_t *list_node = NULL;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int flow_group_id;

    if (ft_flow_group_info_list[unit] != NULL) {
        for (flow_group_id = BCM_INT_FT_GROUP_START_ID;
             flow_group_id < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups);
             flow_group_id++) {
            list_node = &(ft_flow_group_info_list[unit][flow_group_id]);
            if (list_node->action_list != NULL) {
                sal_free(list_node->action_list);
            }
            if (list_node->tracking_params != NULL) {
                sal_free(list_node->tracking_params);
            }
        }
        sal_free(ft_flow_group_info_list[unit]);
    }

    ft_flow_group_info_list[unit] = NULL;

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_create
 * Purpose:
 *     Create the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_flow_group_list_create (int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_flow_group_info_list[unit] == NULL) {
        _BCM_FT_ALLOC(ft_flow_group_info_list[unit],
                      _bcm_int_ft_flow_group_info_t,
                      sizeof(_bcm_int_ft_flow_group_info_t) *
                      (ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID),
                      "Flow_group list");
    }

    if (ft_flow_group_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*******************************************************
 *                                                     *
 *                    Elephant APIs                    *
 */

/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_list_add
 * Purpose:
 *     Add a elephant profile to the list.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     id           - (IN) Elephant profile ID
 *     profile      - (IN) Elephant profile
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_elph_profile_list_add(int unit,
                                   bcm_flowtracker_elephant_profile_t id,
                                   bcm_flowtracker_elephant_profile_info_t *profile)
{
    _bcm_int_ft_elph_profile_info_t *elph_profile_int_info =
                                           &(ft_elph_profile_info_list[unit][id]);

    elph_profile_int_info->id = id;

    sal_memcpy(&(elph_profile_int_info->profile), profile,
               sizeof(bcm_flowtracker_elephant_profile_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_list_get
 * Purpose:
 *     Get a elephant profile from the list.
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     id           - (IN)  Elephant profile ID
 *     profile      - (OUT) Elephant profile
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_elph_profile_list_get(int unit,
                                   bcm_flowtracker_elephant_profile_t id,
                                   _bcm_int_ft_elph_profile_info_t *profile)
{
    _bcm_int_ft_elph_profile_info_t *list_node =
                               &(ft_elph_profile_info_list[unit][id]);

    if (list_node->id != id) {
        return BCM_E_NOT_FOUND;
    }
    sal_memcpy(profile, list_node, sizeof(_bcm_int_ft_elph_profile_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_list_delete
 * Purpose:
 *     Add a elephant profile to the list.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     id           - (IN) Elephant profile ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_elph_profile_list_delete(int unit,
                                      bcm_flowtracker_elephant_profile_t id)
{
    _bcm_int_ft_elph_profile_info_t *profile_int_info =
                                           &(ft_elph_profile_info_list[unit][id]);

    if (profile_int_info->id != id) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(profile_int_info, 0, sizeof(_bcm_int_ft_elph_profile_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_list_create
 * Purpose:
 *     Create the elephant profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_elph_profile_list_create(int unit)
{
    if (ft_elph_profile_info_list[unit] == NULL) {
        _BCM_FT_ALLOC(ft_elph_profile_info_list[unit], _bcm_int_ft_elph_profile_info_t,
                      sizeof(_bcm_int_ft_elph_profile_info_t) *
                      (BCM_INT_FT_ELPH_PROFILE_START_ID + BCM_INT_FT_MAX_ELEPHANT_PROFILES),
                      "Elephant profile list");
    }

    if (ft_elph_profile_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_destroy
 * Purpose:
 *     Destroy the elephant profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_ft_elph_profile_list_destroy(int unit)
{
    if (ft_elph_profile_info_list[unit] != NULL) {
        sal_free(ft_elph_profile_info_list[unit]);
    }
    ft_elph_profile_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/******************************************************
*                                                     *
*        FLOWTRACKER EMB APP MSGING FUNCTIONS         *
 */

/*
 * Function:
 *      _bcm_xgs5_ft_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_xgs5_ft_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_FT_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_FT_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_FT_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_FT_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_FT_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_FT_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_FT_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_FT_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_FT_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_FT_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_FT_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_FT_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_FT_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_FT_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_FT_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_FT_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_FT_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_FT_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_FT_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

STATIC int
_bcm_xgs5_ft_msg_send_receive(int unit, uint8 s_subclass,
                               uint16 s_len, uint32 s_data,
                               uint8 r_subclass, uint16 *r_len,
                               sal_usecs_t timeout)
{
    int rv;
    mos_msg_data_t send, reply;

    uint32 uc_rv;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8 *dma_buffer;
    int dma_buffer_len;

    dma_buffer = ft_info->dma_buffer;
    dma_buffer_len = ft_info->dma_buffer_len;

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_FT;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, ft_info->uc_num,
                                      &send, &reply,
                                      timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert FT uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_xgs5_ft_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_FT) ||
        (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_send_features_exchange_msg
 * Purpose:
 *      Exchange features with the EApp
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_xgs5_ft_eapp_send_features_exchange_msg(int unit)
{
    shr_ft_msg_ctrl_sdk_features_t send_msg;
    shr_ft_msg_ctrl_uc_features_t  rcv_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv;

    sal_memset(&send_msg, 0, sizeof(send_msg));
    sal_memset(&rcv_msg, 0, sizeof(rcv_msg));

    /* Populate the SDK features */
    send_msg.cfg_mode = ft_info->cfg_mode;

    /* Exchange supported features with the FW */
    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_sdk_features_pack(buffer_req, &send_msg);
    buffer_len = buffer_ptr - buffer_req;

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_FEATURES_EXCHANGE,
                                       buffer_len, 0,
                                       MOS_MSG_SUBCLASS_FT_FEATURES_EXCHANGE_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv == BCM_E_UNAVAIL) {
        /* Older FW not supporting this message, assume default feature support */
        ft_info->uc_features = (SHR_FT_UC_FEATURE_IPFIX_EXPORT |
                                SHR_FT_UC_FEATURE_ELPH);
        if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
            /* Only FTv1 is supported when working with older FW */
            return BCM_E_CONFIG;
        }
        return BCM_E_NONE;
    } else if (rv != BCM_E_NONE) {
        return rv;
    }

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_uc_features_unpack(buffer_req, &rcv_msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    /* Check if soc properties configured aligns with the app features */
    if (ft_info->elph_mode && !(rcv_msg.features & SHR_FT_UC_FEATURE_ELPH)) {
        return BCM_E_CONFIG;
    }

    if ((ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) &&
        !(rcv_msg.features & SHR_FT_UC_FEATURE_CONGESTION_MONITOR)) {
        return BCM_E_CONFIG;
    }

    ft_info->uc_features = rcv_msg.features;

    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_ctrl_init_msg (int unit) 
{
    shr_ft_msg_ctrl_init_t ctrl_init_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    int i = 0, pool;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&ctrl_init_msg, 0, sizeof(ctrl_init_msg));

    ctrl_init_msg.max_groups = ft_info->max_flow_groups;
    ctrl_init_msg.num_pipes = ft_info->num_pipes; 
    for (i = 0; i < ctrl_init_msg.num_pipes; i++) {
        ctrl_init_msg.pipe[i].max_flows = ft_info->pipe_info[i].max_flows;
        ctrl_init_msg.pipe[i].num_ctr_pools = ft_info->pipe_info[i].num_ctr_pools;
        for (pool = 0; pool < ctrl_init_msg.pipe[i].num_ctr_pools; pool++) {
            ctrl_init_msg.pipe[i].ctr_pools[pool] = 
                ft_info->pipe_info[i].ctr_pools[pool];
            ctrl_init_msg.pipe[i].ctr_pool_size[pool] = 
                        ft_info->pipe_info[i].ctr_pool_size[pool];
        }
    }
    ctrl_init_msg.rx_channel = BCM_INT_FT_EAPP_RX_CHANNEL;
    ctrl_init_msg.max_export_pkt_length = ft_info->max_export_pkt_length;
    ctrl_init_msg.cur_time_secs = sal_time();

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        ctrl_init_msg.flags |= SHR_FT_MSG_INIT_FLAGS_CONGESTION_MONITOR;
    } else {

        /* Timer intervals */
        ctrl_init_msg.export_interval_msecs = ft_info->export_interval;
        ctrl_init_msg.scan_interval_usecs   = ft_info->scan_interval_usecs;
        ctrl_init_msg.age_timer_tick_msecs  = BCM_INT_FT_AGING_INTERVAL_STEP_MSECS;

        /* Initialization flags */
        if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_FLOW_START_TS) {
            ctrl_init_msg.flags |= SHR_FT_MSG_INIT_FLAGS_FLOW_START_TS;
        }
        if (ft_info->elph_mode) {
            ctrl_init_msg.flags |= SHR_FT_MSG_INIT_FLAGS_ELEPHANT;
            ctrl_init_msg.num_elph_profiles = BCM_INT_FT_MAX_ELEPHANT_PROFILES;
        }
        if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_MONITOR) {
            ctrl_init_msg.flags |= SHR_FT_MSG_INIT_FLAGS_DROP_MONITOR;
        }
        if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_HOSTMEM_ENABLE) {
            ctrl_init_msg.flags |= SHR_FT_MSG_INIT_FLAGS_HOSTMEM_ENABLE;
        }
        ctrl_init_msg.fsp_reinject_max_length = ft_info->fsp_reinject_max_length;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_init_pack(buffer_req, &ctrl_init_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_INIT,
                                                     buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_INIT_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_em_key_format_msg (int unit, 
            shr_ft_msg_ctrl_em_key_format_t *em_key_format) 
{
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_em_key_format_pack(buffer_req, em_key_format);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_EM_KEY_FORMAT,
                                                     buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_EM_KEY_FORMAT_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_em_group_create_msg(int unit,
                                               shr_ft_msg_ctrl_em_group_create_t *msg)
{
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_em_group_create_pack(buffer_req, msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
           _bcm_xgs5_ft_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_FT_EM_GROUP_CREATE,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_FT_EM_GROUP_CREATE_REPLY,
                                         &reply_len,
                                         SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;

}

int _bcm_xgs5_ft_eapp_send_em_group_delete_msg(int unit,
                                               int em_group_id)
{
    uint16 reply_len = 0;

    BCM_IF_ERROR_RETURN(
           _bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_EM_GROUP_DELETE,
                                         em_group_id, 0,
                                         MOS_MSG_SUBCLASS_FT_EM_GROUP_DELETE_REPLY,
                                         &reply_len,
                                         SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;

}

int _bcm_xgs5_ft_eapp_send_flow_group_create_msg (int unit, 
                    bcm_flowtracker_group_t flow_group_id) 
{
    shr_ft_msg_ctrl_group_create_t group_create_msg;
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;
    
    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    /* Get the flow group info for this group id */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id, &flow_group_int_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    sal_memset(&group_create_msg, 0, sizeof(group_create_msg));
    group_create_msg.group_idx = flow_group_id;
    group_create_msg.domain_id = ft_info->observation_domain_id;
    /* Creating group with 0 as flow limit, so that you dont learn until flow limit is set */
    group_create_msg.flow_limit = 0;

    /* Creating group with default number for aging time. */
    group_create_msg.aging_interval_msecs = BCM_INT_FT_DEF_AGING_INTERVAL_MSECS;

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_group_create_pack(buffer_req, &group_create_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_GROUP_CREATE,
                                                     buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_GROUP_CREATE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_flow_group_delete_msg (int unit, 
                    bcm_flowtracker_group_t flow_group_id) 
{
    uint16 reply_len = 0;
    
    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_GROUP_DELETE,
                                                      flow_group_id, 0,
                                    MOS_MSG_SUBCLASS_FT_GROUP_DELETE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/* NOTE: currently expecting only one flag out of multiple flags and its updated value
 * is expected.
 */
int _bcm_xgs5_ft_eapp_send_flow_group_update_msg (int unit, 
                    bcm_flowtracker_group_t flow_group_id,
                    uint32 update_flags, uint32 update_value)
{
    shr_ft_msg_ctrl_group_update_t group_update_msg;
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    /* Get the flow group info for this group id */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id, &flow_group_int_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    sal_memset(&group_update_msg, 0, sizeof(group_update_msg));
    group_update_msg.group_idx = flow_group_id;
    group_update_msg.update    = update_flags;
    group_update_msg.param0    = update_value;

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_group_update_pack(buffer_req, &group_update_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_GROUP_UPDATE,
                                                     buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_GROUP_UPDATE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/* NOTE: Currently we use flow group get msg to EAPP
 *       only for getting flow_count.
 */
int _bcm_xgs5_ft_eapp_send_flow_group_get_msg (int unit, 
                    bcm_flowtracker_group_t flow_group_id,
                    uint32 *flow_count)
{
    shr_ft_msg_ctrl_group_get_t group_get_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&group_get_msg, 0, sizeof(group_get_msg));


    /* Do a group get for flow_group_id */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_GROUP_GET,
                                                      flow_group_id, 0,
                                    MOS_MSG_SUBCLASS_FT_GROUP_GET_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_group_get_unpack(buffer_req, &group_get_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }
    
    *flow_count = group_get_msg.flow_count;
    
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_template_create_msg (int unit, 
                    bcm_flowtracker_export_template_t template_id) 
{
    _bcm_int_ft_export_template_info_t template_int_info;
    shr_ft_msg_ctrl_template_create_t template_create_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;
    int i = 0;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&template_int_info, 0, sizeof(template_int_info));
    /* Get the template info for this template id */
    rv = _bcm_xgs5_ft_template_list_template_get(unit, template_id, &template_int_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        template_id = template_id - 1;
    }
    sal_memset(&template_create_msg, 0, sizeof(template_create_msg));
    template_create_msg.id             = template_id;
    template_create_msg.set_id         = template_int_info.set_id;
    template_create_msg.num_info_elems = template_int_info.num_export_elements;

    for (i = 0; i < template_create_msg.num_info_elems; i++) {
        template_create_msg.info_elems[i] = template_int_info.elements[i].element;
        template_create_msg.data_size[i]  = template_int_info.elements[i].data_size;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_template_create_pack(buffer_req, &template_create_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_TEMPLATE_CREATE,
                                                     buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_TEMPLATE_CREATE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_template_delete_msg (int unit, 
                    bcm_flowtracker_export_template_t template_id) 
{
    uint16 reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        template_id = template_id - 1;
    }
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, 
                                    MOS_MSG_SUBCLASS_FT_TEMPLATE_DELETE,
                                    template_id, 0,
                                    MOS_MSG_SUBCLASS_FT_TEMPLATE_DELETE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_collector_create_msg(int unit,
                                                int collector_id,
                                                int export_profile_id)
{
    shr_ft_msg_ctrl_collector_create_t collector_create_msg;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        rv = _bcm_xgs5_ft_eapp_collector_encap_build_pack(unit, collector_id,
                                                          &collector_create_msg);
    } else {
        rv = _bcm_xgs5_ft_v2_eapp_collector_encap_build_pack(unit, collector_id,
                                                             export_profile_id,
                                                             &collector_create_msg);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Check if there is space to insert at least the headers  */
    if ((collector_create_msg.encap_length +
         BCM_INT_FT_IPFIX_MSG_HDR_LENGTH   +
         BCM_INT_FT_SET_HDR_LENGTH         +
         BCM_INT_FT_L2_CRC_LENGTH)  > collector_create_msg.mtu) {
        return BCM_E_PARAM;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_collector_create_pack(buffer_req, &collector_create_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_COLLECTOR_CREATE,
                                                     buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_COLLECTOR_CREATE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_collector_delete_msg (int unit,
                                                 uint16 collector_id)
{
    uint16 reply_len = 0;
    
    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, 
                                        MOS_MSG_SUBCLASS_FT_COLLECTOR_DELETE,
                                                      collector_id, 0,
                                    MOS_MSG_SUBCLASS_FT_COLLECTOR_DELETE_REPLY, &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_eapp_send_export_interval_update_msg (int unit,
                                                 uint32 export_interval)
{
    uint16 reply_len = 0;
    
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, 
                                        MOS_MSG_SUBCLASS_FT_EXPORT_INTERVAL_UPDATE,
                                                     0, export_interval,
                                    MOS_MSG_SUBCLASS_FT_EXPORT_INTERVAL_UPDATE_REPLY, 
                                    &reply_len, 
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_xgs5_ft_send_eapp_template_xmit_start
 * Purpose:
 *      Start the template set transmit
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      template_id   - (IN)  Template Id
 *      collector_id  - (IN)  Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_xgs5_ft_eapp_send_template_xmit_start(int unit,
                                  bcm_flowtracker_export_template_t template_id,
                                  bcm_flowtracker_collector_t collector_id)
{
    _bcm_int_ft_export_template_info_t template_int_info;
    shr_ft_msg_ctrl_template_xmit_t             msg;
    _bcm_int_ft_info_t                *ft_info = FT_INFO_GET(unit);
    uint8                                      *buffer_req = NULL;
    uint8                                      *buffer_ptr = NULL;
    uint16                                      buffer_len = 0;
    uint16                                      reply_len = 0;
    int                                         rv = BCM_E_NONE;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&template_int_info, 0, sizeof(template_int_info));

    /* Get the template info for this template id */
    rv = _bcm_xgs5_ft_template_list_template_get(unit, template_id,
                                                 &template_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    sal_memset(&msg, 0, sizeof(msg));

    /* Build the template encap */
    _bcm_xgs5_ft_eapp_template_encap_build_pack(unit,
                                                &template_int_info,
                                                &(msg.encap_length),
                                                msg.encap);
    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        template_id = template_id - 1;
    }
    msg.template_id   = template_id;
    msg.collector_id  = collector_id;
    msg.interval_secs = template_int_info.interval_secs;
    msg.initial_burst = template_int_info.initial_burst;

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_template_xmit_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
          _bcm_xgs5_ft_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_FT_TEMPLATE_XMIT,
                                        buffer_len, 0,
                                        MOS_MSG_SUBCLASS_FT_TEMPLATE_XMIT_REPLY,
                                        &reply_len,
                                        SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_send_group_actions_set_msg
 * Purpose:
 *      Set actions on a group
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_xgs5_ft_eapp_send_group_actions_set_msg(int unit,
                                             shr_ft_msg_ctrl_group_actions_set_t *msg)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8 *buffer_req = NULL;
    uint8 *buffer_ptr = NULL;
    uint16 buffer_len = 0;
    uint16 reply_len = 0;

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_group_actions_set_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
          _bcm_xgs5_ft_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_FT_GROUP_ACTIONS_SET,
                                        buffer_len, 0,
                                        MOS_MSG_SUBCLASS_FT_GROUP_ACTIONS_SET_REPLY,
                                        &reply_len,
                                        SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/******************************************************
*                                                     *
*        FLOWTRACKER EMB APP APIs                     *
 */

int bcm_xgs5_ft_eapp_init(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;
    int uc = 0;
    int status;
    int qcm_enable = 0;

    uint32 share_mem_addr, host_mem_size = 0;
    uint32 *host_mem_addr = NULL;

    
    if (!soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        /* Init the app */
        for (uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
            rv = soc_uc_status(unit, uc, &status);
            if ((rv == SOC_E_NONE) && (status != 0)) {
                rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_FT,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT,
                                       FT_MSG_VERSION, FT_MSG_BASE_VERSION,
                                       NULL, NULL);
                if (SOC_E_NONE == rv) {
                    /* FT eapp started successfully */
                    ft_info->uc_num = uc;
                    break;
                }
            }
        }
    }

    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (uc >= SOC_INFO(unit).num_ucs) {
        /* Could not find or start FT eapp */
        if (qcm_enable) {
            LOG_WARN(BSL_LS_BCM_FLOWTRACKER,
                     (BSL_META_U(unit,
                                 "uKernel QCM application not available\n")));
        } else {
            LOG_WARN(BSL_LS_BCM_FLOWTRACKER,
                     (BSL_META_U(unit,
                                 "uKernel FT application not available\n")));
        }
        BCM_IF_ERROR_RETURN(bcm_xgs5_ft_eapp_detach(unit));
        return BCM_E_NONE;
    }

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    if (qcm_enable) {
        ft_info->dma_buffer_len = sizeof(shr_ft_qcm_msg_ctrl_t);
    } else {
        ft_info->dma_buffer_len = sizeof(shr_ft_msg_ctrl_t);
    }
    ft_info->dma_buffer = soc_cm_salloc(unit, ft_info->dma_buffer_len,
                                         "FT DMA buffer");
    if (!ft_info->dma_buffer) {
        BCM_IF_ERROR_RETURN(bcm_xgs5_ft_eapp_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(ft_info->dma_buffer, 0, ft_info->dma_buffer_len);

    if (!SOC_WARM_BOOT(unit)) {
        /* Send the init config to UKERNEL */
        rv = _bcm_xgs5_ft_eapp_send_ctrl_init_msg(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bcm_xgs5_ft_eapp_detach(unit));
            return rv;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_eapp_send_features_exchange_msg(unit));

    /* Create template list, collector list and flow group list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_template_list_create(unit));
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_collector_list_create(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_v2_collector_list_create(unit));
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_v2_collector_list_export_profile_create(unit));
    }
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_flow_group_list_create(unit));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_elph_profile_list_create(unit));

    if ((SOC_WARM_BOOT(unit)) &&
        (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR)) {
        /* Set the host memory size and address for QCM App */
        rv = _bcm_xgs5_ft_qcm_eapp_host_buf_get(unit, &host_mem_size, &host_mem_addr);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "QCM warm boot get host memory message communication failed \n")));
            return rv;
        }

        /* Ensure QCM app host memory is not NULL */
        if (host_mem_size) {
            host_mem_addr = (uint32 *) soc_cm_salloc(unit, host_mem_size, "Host buffer");
            if (host_mem_addr == NULL) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                          "host memory address NULL\n")));
                return BCM_E_PARAM;
            }
        }
        sal_memset(host_mem_addr, 0, host_mem_size);
        share_mem_addr = (uint32)soc_cm_l2p(unit, host_mem_addr);

        rv = _bcm_xgs5_ft_qcm_eapp_host_buf_set(unit, host_mem_size, share_mem_addr);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "QCM warm boot set host memory message communication failed \n")));
            return rv;
        }
    }

    return BCM_E_NONE;
}

int bcm_xgs5_ft_eapp_detach(int unit)
{
    _bcm_int_ft_info_t          *ft_info = FT_INFO_GET(unit);
    int                          rv = BCM_E_NONE;
    int                          uc = 0, status = 0;
    uint16                       reply_len = 0;

    
    if (!soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
    if (!SOC_WARM_BOOT(unit)) {
        /* Un Init the app */
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, uc, &status));
        if (status) {
            rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                               MOS_MSG_SUBCLASS_FT_SHUTDOWN,
                                               0, 0,
                                               MOS_MSG_SUBCLASS_FT_SHUTDOWN_REPLY,
                                               &reply_len,
                                               SHR_FT_MAX_UC_MSG_TIMEOUT);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                return BCM_E_INTERNAL;
            }
        }
    }
    }

    /*
     * Free DMA buffer
 */
    if (ft_info->dma_buffer != NULL) {
        soc_cm_sfree(unit, ft_info->dma_buffer);
    }

    /* Free up template list, collector list and flow group list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_template_list_destroy(unit));
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_collector_list_destroy(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_v2_collector_list_destroy(unit));
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_v2_collector_list_export_profile_destroy(unit));
    }
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_flow_group_list_destroy(unit));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_elph_profile_list_destroy(unit));

    return BCM_E_NONE;
}

/* Drop reasons, DO NOT change the order */
static bcm_rx_reason_t ft_drop_reasons[] = {
    bcmRxReasonInvalid,
    bcmRxReasonL3SourceMiss,
    bcmRxReasonL3DestMiss,
    bcmRxReasonMcastMiss,
    bcmRxReasonIpMcastMiss,
    bcmRxReasonUnknownVlan,
    bcmRxReasonL3HeaderMismatch,
    bcmRxReasonDosAttack,
    bcmRxReasonMartianAddr,
    bcmRxReasonTunnelError,
    bcmRxReasonParityError,
    bcmRxReasonL3MtuFail,
    bcmRxReasonHigigHdrError,
    bcmRxReasonMcastIdxError,
    bcmRxReasonClassBasedMove,
    bcmRxReasonL3AddrBindFail,
    bcmRxReasonMplsLabelMiss,
    bcmRxReasonMplsInvalidAction,
    bcmRxReasonMplsInvalidPayload,
    bcmRxReasonTunnelObjectValidationFail,
    bcmRxReasonMplsSequenceNumber,
    bcmRxReasonL2NonUnicastMiss,
    bcmRxReasonNivPrioDrop,
    bcmRxReasonNivRpfFail,
    bcmRxReasonUnknownSubtendingPort,
    bcmRxReasonTunnelAdaptLookupMiss,
    bcmRxReasonPacketFlowSelectMiss,
    bcmRxReasonTunnelDecapEcnError,
    bcmRxReasonFailoverDrop,
    bcmRxReasonOtherLookupMiss,
    bcmRxReasonInvalidTpid,
    bcmRxReasonTunnelTtlError,
    bcmRxReasonMplsIllegalReservedLabel,
    bcmRxReasonL3HeaderError,
    bcmRxReasonL2HeaderError,
    bcmRxReasonTtl1,
    bcmRxReasonTtl,
    bcmRxReasonFcoeZoneCheckFail,
    bcmRxReasonIpmcInterfaceMismatch,
    bcmRxReasonMplsTtl,
    bcmRxReasonMplsUnknownAch,
    bcmRxReasonOAMError,
    bcmRxReasonL2GreSipMiss,
    bcmRxReasonL2GreVpnIdMiss,
    bcmRxReasonBfd,
    bcmRxReasonCongestionCnmProxyError,
    bcmRxReasonVxlanSipMiss,
    bcmRxReasonVxlanVpnIdMiss,
    bcmRxReasonNivInterfaceMiss,
    bcmRxReasonNivTagInvalid,
    bcmRxReasonNivTagDrop,
    bcmRxReasonNivUntagDrop,
    bcmRxReasonTrillInvalid,
    bcmRxReasonTrillMiss,
    bcmRxReasonTrillRpfFail,
    bcmRxReasonTrillTtl,
    bcmRxReasonNat,
    bcmRxReasonTcpUdpNatMiss,
    bcmRxReasonIcmpNatMiss,
    bcmRxReasonNatFragment,
    bcmRxReasonNatMiss
};

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_send_flow_group_flow_data_msg
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int _bcm_xgs5_ft_eapp_send_flow_group_flow_data_get_msg(
                                           int unit,
                                           bcm_flowtracker_group_t flow_group_id,
                                           bcm_flowtracker_flow_key_t *flow_key,
                                           bcm_flowtracker_flow_data_t *flow_data)
{
    shr_ft_msg_ctrl_group_flow_data_get_t       send_msg;
    shr_ft_msg_ctrl_group_flow_data_get_reply_t reply_msg;
    _bcm_int_ft_info_t                         *ft_info = FT_INFO_GET(unit);
    uint8                                      *buffer_req = NULL;
    uint8                                      *buffer_ptr = NULL;
    uint16                                      buffer_len = 0, reply_len = 0;
    int                                         i;
    bcm_rx_reason_t                             drop_reason;
    int                                         rv = BCM_E_NONE;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&send_msg, 0, sizeof(send_msg));
    send_msg.group_idx         = flow_group_id;
    send_msg.src_ip            = flow_key->src_ip.addr.ipv4_addr;
    send_msg.dst_ip            = flow_key->dst_ip.addr.ipv4_addr;
    send_msg.l4_src_port       = flow_key->l4_src_port;
    send_msg.l4_dst_port       = flow_key->l4_dst_port;
    send_msg.ip_protocol       = flow_key->ip_protocol;
    send_msg.inner_src_ip      = flow_key->inner_src_ip.addr.ipv4_addr;
    send_msg.inner_dst_ip      = flow_key->inner_dst_ip.addr.ipv4_addr;
    sal_memcpy(send_msg.inner_src_ipv6_addr, flow_key->inner_src_ip.addr.ipv6_addr,
        sizeof(send_msg.inner_src_ipv6_addr));
    sal_memcpy(send_msg.inner_dst_ipv6_addr, flow_key->inner_dst_ip.addr.ipv6_addr,
        sizeof(send_msg.inner_dst_ipv6_addr));
    send_msg.inner_l4_src_port = flow_key->inner_l4_src_port;
    send_msg.inner_l4_dst_port = flow_key->inner_l4_dst_port;
    send_msg.inner_ip_protocol = flow_key->inner_ip_protocol;
    send_msg.vnid              = flow_key->vxlan_network_id;
    send_msg.custom_length     = SHR_FT_CUSTOM_KEY_MAX_LENGTH;
    sal_memcpy(send_msg.custom, flow_key->custom, send_msg.custom_length);
    send_msg.in_port           = flow_key->in_port;

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_group_flow_data_get_pack(buffer_req, &send_msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
      _bcm_xgs5_ft_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET_REPLY,
                                    &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_group_flow_data_get_reply_unpack(buffer_req, &reply_msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    COMPILER_64_SET(flow_data->packet_count, reply_msg.pkt_count_upper,
                    reply_msg.pkt_count_lower);

    COMPILER_64_SET(flow_data->byte_count, reply_msg.byte_count_upper,
                    reply_msg.byte_count_lower);

    COMPILER_64_SET(flow_data->flow_start_timestamp_msecs,
                    reply_msg.flow_start_ts_upper,
                    reply_msg.flow_start_ts_lower);

    COMPILER_64_SET(flow_data->observation_timestamp_msecs,
                    reply_msg.obs_ts_upper, reply_msg.obs_ts_lower);

    /* Get the drop reasons */
    BCM_RX_REASON_CLEAR_ALL(flow_data->pkt_drop_reasons);
    for (i = 0; i < reply_msg.num_drop_reasons; i++) {
        if (reply_msg.drop_reasons[i] == SHR_FT_DROP_RX_REASON_INVALID) {
            continue;
        } else if (reply_msg.drop_reasons[i] >= SHR_FT_DROP_RX_REASON_MAX_VALUE) {
            return BCM_E_PARAM;
        }

        drop_reason = ft_drop_reasons[reply_msg.drop_reasons[i]];
        BCM_RX_REASON_SET(flow_data->pkt_drop_reasons, drop_reason);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_send_ser_event_msg
 * Purpose:
 *      Send SER event msg to app
 * Parameters:
 *      unit      - (IN) BCM device number
 *      pipe      - (IN) Pipe index
 *      mem       - (IN) Memory (EM)
 *      index     - (IN) Index
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int
_bcm_xgs5_ft_eapp_send_ser_event_msg(int unit, int pipe, soc_mem_t mem, int index)
{
    shr_ft_msg_ctrl_ser_event_t  ser_event_msg;
    _bcm_int_ft_info_t          *ft_info = FT_INFO_GET(unit);
    uint8                       *buffer_req = NULL, *buffer_ptr = NULL;
    uint16                       buffer_len = 0, reply_len = 0;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    sal_memset(&ser_event_msg, 0, sizeof(ser_event_msg));
    ser_event_msg.pipe  = pipe;
    ser_event_msg.mem   = mem;
    ser_event_msg.index = index;

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_ser_event_pack(buffer_req, &ser_event_msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
            _bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_SER_EVENT,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_FT_SER_EVENT_REPLY,
                                          &reply_len,
                                          SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_elph_filter_copy
 * Purpose:
 *      Copy the filter cfg from API to app format
 * Parameters:
 *      unit  - (IN)  BCM device number
 *      src   - (IN)  Filter API format
 *      dst   - (OUT) Filter app format
 * Returns:
 *      None
 */
STATIC void
_bcm_xgs5_ft_eapp_elph_filter_copy(int unit,
                                   bcm_flowtracker_elephant_filter_t *src,
                                   shr_ft_msg_ctrl_elph_profile_filter_t *dst)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    COMPILER_64_TO_32_HI(dst->size_threshold_bytes_upper_32,
                         src->size_threshold_bytes);

    COMPILER_64_TO_32_LO(dst->size_threshold_bytes_lower_32,
                         src->size_threshold_bytes);

    /* Copy the rate thresholds, API configures it as kbits/sec, whereas app
     * expects it as bytes/100usec
     */
    dst->rate_low_threshold  = src->rate_low_threshold_kbits_sec / 80;
    dst->rate_high_threshold = src->rate_high_threshold_kbits_sec / 80;

    /* Convert the monitoring interval to scan count */
    dst->scan_count = src->monitor_interval_usecs / ft_info->scan_interval_usecs;

    if (src->flags & BCM_FLOWTRACKER_ELEPHANT_FILTER_FLAGS_INCREASING_RATE) {
        dst->incr_rate = 1;
    }
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_send_elph_profile_create_msg
 * Purpose:
 *      Send Elephant profile create msg
 * Parameters:
 *      unit       - (IN) BCM device number
 *      profile_id - (IN) Elephant profile Id
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int
_bcm_xgs5_ft_eapp_send_elph_profile_create_msg(int unit, int profile_id)
{
    shr_ft_msg_ctrl_elph_profile_create_t  msg;
    uint8                                 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16                                 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t                    *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_elph_profile_info_t        profile_info;

    sal_memset(&msg, 0, sizeof(msg));
    msg.profile_idx = profile_id;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_elph_profile_list_get(unit, profile_id,
                                                           &profile_info));

    /* App expects 2 promotion filters followed by a demotion filter always.
     * Dummy filters should have already been inserted by now in profile_info
     */
    msg.num_filters = BCM_FLOWTRACKER_ELEPHANT_MAX_PROMOTION_FILTERS + 1;
    _bcm_xgs5_ft_eapp_elph_filter_copy(unit,
                                       &(profile_info.profile.promotion_filters[0]),
                                       &(msg.filters[0]));
    _bcm_xgs5_ft_eapp_elph_filter_copy(unit,
                                       &(profile_info.profile.promotion_filters[1]),
                                       &(msg.filters[1]));
    _bcm_xgs5_ft_eapp_elph_filter_copy(unit,
                                       &(profile_info.profile.demotion_filter),
                                       &(msg.filters[2]));


    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_msg_ctrl_elph_profile_create_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
        _bcm_xgs5_ft_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_FT_ELPH_PROFILE_CREATE,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_FT_ELPH_PROFILE_CREATE_REPLY,
                                      &reply_len,
                                      SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ft_eapp_send_elph_profile_delete_msg
 * Purpose:
 *      Send Elephant profile delete msg
 * Parameters:
 *      unit       - (IN) BCM device number
 *      profile_id - (IN) Elephant profile Id
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int
_bcm_xgs5_ft_eapp_send_elph_profile_delete_msg(int unit, int profile_id)
{
    uint16 reply_len = 0;

    
    if (soc_feature(unit, soc_feature_mv2_dummy_flowtracker)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
       _bcm_xgs5_ft_msg_send_receive(unit,
                                     MOS_MSG_SUBCLASS_FT_ELPH_PROFILE_DELETE,
                                     profile_id, 0,
                                     MOS_MSG_SUBCLASS_FT_ELPH_PROFILE_DELETE_REPLY,
                                     &reply_len,
                                     SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}


/******************************************************
*                                                     *
*                Debug functions                      *
 */
char *_bcm_xgs5_ft_template_element_str[bcmFlowtrackerExportElementTypeCount] = 
{
    "SrcIPv4",
    "DstIPv4",
    "Unknown",
    "Unknown",
    "L4SrcPort",
    "L4DstPort",
    "IPProtocol",
    "PktCount",
    "ByteCount",
    "Unknown",
    "Unknown",
    "Unknown",
    "ObservationTimeMsecs",
    "FlowStartTimeMsecs",
    "GroupId",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
};

/*
 * Function:
 *     _bcm_xgs5_ft_template_dump
 * Purpose:
 *     Dump the template.
 * Parameters:
 *     template_node - (IN) Template info.
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_template_dump (
   _bcm_int_ft_export_template_info_t *template_node)
{
    int i;

    LOG_CLI((BSL_META("Template ID = %d\r\n"),template_node->template_id));
    LOG_CLI((BSL_META("\tSet ID = %u\r\n"), template_node->set_id));
    LOG_CLI((BSL_META("\tNum of export elements = %d\r\n"), template_node->num_export_elements));
    for (i = 0; i < template_node->num_export_elements;
         i++) {
        LOG_CLI((BSL_META("\t\tElement[%d]=%s\r\n"), i,
         _bcm_xgs5_ft_template_element_str[template_node->elements[i].element]));
    }
}

/*
 * Function:
 *     _bcm_xgs5_ft_template_list_dump
 * Purpose:
 *     Dump the template list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_template_list_dump (int unit)
{
    _bcm_int_ft_export_template_info_t *template_node = NULL;
    int i;

    if (ft_export_template_info_list[unit] == NULL) {
        return;
    }

    for (i = BCM_INT_FT_TEMPLATE_START_ID; i <= BCM_INT_FT_TEMPLATE_END_ID; i++) {
        template_node = &(ft_export_template_info_list[unit][i]);
        if (template_node->template_id != 0) {
            _bcm_xgs5_ft_template_dump(template_node);
        }
    } 
}

void
_bcm_xgs5_ft_collector_encap_l2_dump(
                    bcm_flowtracker_collector_eth_header_t *eth)
{
   LOG_CLI((BSL_META("\t\tDST_MAC = %x:%x:%x:%x:%x:%x\r\n"),
                    eth->dst_mac[0],
                    eth->dst_mac[1],
                    eth->dst_mac[2],
                    eth->dst_mac[3],
                    eth->dst_mac[4],
                    eth->dst_mac[5])); 
   LOG_CLI((BSL_META("\t\tSRC_MAC = %x:%x:%x:%x:%x:%x\r\n"),
                    eth->src_mac[0],
                    eth->src_mac[1],
                    eth->src_mac[2],
                    eth->src_mac[3],
                    eth->src_mac[4],
                    eth->src_mac[5])); 
     LOG_CLI((BSL_META("\t\tVlan tag structure = %d\r\n"),
                    eth->vlan_tag_structure));
     LOG_CLI((BSL_META("\t\touter_tpid = %u\r\n"),
                    eth->outer_tpid));
     LOG_CLI((BSL_META("\t\tinner_tpid = %u\r\n"),
                    eth->inner_tpid));
     LOG_CLI((BSL_META("\t\touter_vlan_tag = %u\r\n"),
                    eth->outer_vlan_tag));
     LOG_CLI((BSL_META("\t\tinner_vlan_tag = %u\r\n"),
                    eth->inner_vlan_tag));
}

void
_bcm_xgs5_ft_collector_encap_ipv4_dump(
        bcm_flowtracker_collector_ipv4_header_t *ipv4)
{
     LOG_CLI((BSL_META("\t\tsrc_ip = %x\r\n"),
                    ipv4->src_ip));
     LOG_CLI((BSL_META("\t\tdst_ip = %x\r\n"),
                    ipv4->dst_ip));
     LOG_CLI((BSL_META("\t\tdscp = %u\r\n"),
                    ipv4->dscp));
     LOG_CLI((BSL_META("\t\tttl = %u\r\n"),
                    ipv4->ttl));
}

void
_bcm_xgs5_ft_collector_encap_udp_dump(
        bcm_flowtracker_collector_udp_header_t *udp)
{
     LOG_CLI((BSL_META("\t\tDst L4 port = %d\r\n"),
                    udp->dst_port));
     LOG_CLI((BSL_META("\t\tSrc L4 port = %d\r\n"),
                    udp->src_port));
}

char *_bcm_xgs5_ft_collector_transport_type_str[bcmFlowtrackerCollectorTransportTypeCount] =
{
    "IPv4 UDP",
    "IPv6 UDP",
    "Raw"
};

/*
 * Function:
 *     _bcm_xgs5_ft_collector_dump
 * Purpose:
 *     Dump the collector info.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_node - (IN) Collector information.
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_collector_dump ( 
    _bcm_int_ft_collector_info_t *collector_node)
{
     LOG_CLI((BSL_META("Collector ID = %d\r\n"),collector_node->collector_id));
     LOG_CLI((BSL_META("\tTransport type = %s\r\n"),
  _bcm_xgs5_ft_collector_transport_type_str[collector_node->collector_info.transport_type]));
     LOG_CLI((BSL_META("\tEncap info:\r\n")));
     _bcm_xgs5_ft_collector_encap_l2_dump(&(collector_node->collector_info.eth));
     _bcm_xgs5_ft_collector_encap_ipv4_dump(
             &(collector_node->collector_info.ipv4));
     _bcm_xgs5_ft_collector_encap_udp_dump(&(collector_node->collector_info.udp));
     LOG_CLI((BSL_META("\tMax packet length = %u\r\n"), 
                    collector_node->collector_info.max_packet_length));
}
/*
 * Function:
 *     _bcm_xgs5_ft_collector_list_dump
 * Purpose:
 *     Dump the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_collector_list_dump (int unit)
{
    _bcm_int_ft_collector_info_t *collector_node = NULL;
    int i;

    if (ft_collector_info_list[unit] == NULL) {
        return;
    }

    for (i = BCM_INT_FT_COLLECTOR_START_ID; i <= BCM_INT_FT_COLLECTOR_END_ID; i++) {
        collector_node = &(ft_collector_info_list[unit][i]);
        if (collector_node->collector_id != 0) {
            _bcm_xgs5_ft_collector_dump(collector_node);
        }
    } 
}

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_dump
 * Purpose:
 *     Dump the flow_group info.
 * Parameters:
 *     flow_group_node - (IN) flow_group information.
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_flow_group_dump (
                         int unit,
                         _bcm_int_ft_flow_group_info_t *flow_group_node)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

     LOG_CLI((BSL_META("Flow group ID = %u\r\n"),
                        flow_group_node->flow_group_id));
     LOG_CLI((BSL_META("\tObservation domain ID = %u\r\n"),
                        ft_info->observation_domain_id));
     LOG_CLI((BSL_META("\tExport interval(us) = %u\r\n"),
                        ft_info->export_interval));
     LOG_CLI((BSL_META("\tAging interval(ms) = %u\r\n"),
                        flow_group_node->aging_interval_msecs));
     LOG_CLI((BSL_META("\tFlow limit = %u\r\n"),flow_group_node->flow_limit));
     LOG_CLI((BSL_META("\tCollector ID = %u\r\n"),flow_group_node->collector_id));
     LOG_CLI((BSL_META("\tTemplate ID = %u\r\n"),flow_group_node->template_id));
     if (ft_info->elph_mode == TRUE) {
         LOG_CLI((BSL_META("\Elephant Profile ID = %u\r\n"),
                                             flow_group_node->elph_profile_id));
     }
}

/*
 * Function:
 *     _bcm_xgs5_ft_flow_group_list_dump
 * Purpose:
 *     Dump the flow_group list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_flow_group_list_dump (int unit)
{
    _bcm_int_ft_flow_group_info_t *flow_group_node = NULL;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;

    if (ft_flow_group_info_list[unit] == NULL) {
        return;
    }

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups); i++) {
        flow_group_node = &(ft_flow_group_info_list[unit][i]);
        if (flow_group_node->flow_group_id != 0) {
            _bcm_xgs5_ft_flow_group_dump(unit, flow_group_node);
        }
    } 
}
/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_dump
 * Purpose:
 *     Dump an elephant profile info.
 * Parameters:
 *     elph_profile - (IN) Elephant profile information.
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_ft_elph_profile_dump(_bcm_int_ft_elph_profile_info_t *profile_info)
{
    int i;

    LOG_CLI((BSL_META("Profile Id            = %d\r\n"), profile_info->id));
    LOG_CLI((BSL_META("Num promotion filters = %d\r\n"),
             profile_info->profile.num_promotion_filters));
    for (i = 0; i < profile_info->profile.num_promotion_filters; i++) {
        LOG_CLI((BSL_META("Promotion Filter : %d\r\n"), i));
        LOG_CLI((BSL_META("\tFlags               = 0x%08x\r\n"),
                 profile_info->profile.promotion_filters[i].flags));
        LOG_CLI((BSL_META("\tMonitor Interval    = %u usecs\r\n"),
                 profile_info->profile.promotion_filters[i].monitor_interval_usecs));
        LOG_CLI((BSL_META("\tRate Low threshold  = %u kbps\r\n"),
                 profile_info->profile.promotion_filters[i].rate_low_threshold_kbits_sec));
        LOG_CLI((BSL_META("\tRate High threshold = %u kbps\r\n"),
                 profile_info->profile.promotion_filters[i].rate_high_threshold_kbits_sec));
        LOG_CLI((BSL_META("\tSize threshold      = {0x%x, 0x%x} bytes\r\n"),
                 COMPILER_64_HI(profile_info->profile.promotion_filters[i].size_threshold_bytes),
                 COMPILER_64_LO(profile_info->profile.promotion_filters[i].size_threshold_bytes)));
    }
    LOG_CLI((BSL_META("Demotion Filter : \r\n")));
    LOG_CLI((BSL_META("\tFlags               = 0x%08x\r\n"),
             profile_info->profile.demotion_filter.flags));
    LOG_CLI((BSL_META("\tMonitor Interval    = %u usecs\r\n"),
             profile_info->profile.demotion_filter.monitor_interval_usecs));
    LOG_CLI((BSL_META("\tRate Low threshold  = %u kbps\r\n"),
             profile_info->profile.demotion_filter.rate_low_threshold_kbits_sec));
    LOG_CLI((BSL_META("\tRate High threshold = %u kbps\r\n"),
             profile_info->profile.demotion_filter.rate_high_threshold_kbits_sec));
    LOG_CLI((BSL_META("\tSize threshold      = {0x%x, 0x%x} bytes\r\n"),
             COMPILER_64_HI(profile_info->profile.demotion_filter.size_threshold_bytes),
             COMPILER_64_LO(profile_info->profile.demotion_filter.size_threshold_bytes)));

}

/*
 * Function:
 *     _bcm_xgs5_ft_elph_profile_list_dump
 * Purpose:
 *     Dump the elephant profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     None
 */
void
_bcm_xgs5_ft_elph_profile_list_dump(int unit)
{
    int i;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_elph_profile_info_t *profile_int_info = NULL;

    if (ft_info->elph_mode != TRUE) {
        return;
    }

    LOG_CLI((BSL_META("Dumping elephant profile Info : \r\n")));
    for (i = BCM_INT_FT_ELPH_PROFILE_START_ID;
         i <= BCM_INT_FT_ELPH_PROFILE_END_ID; i++) {

        profile_int_info = &(ft_elph_profile_info_list[unit][i]);
        if (profile_int_info->id != i) {
            continue;
        }
        _bcm_xgs5_ft_elph_profile_dump(profile_int_info);
    }
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_learn
 * Purpose:
 *      Dump the learn statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_flowtracker_dump_stats_learn(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_msg_ctrl_stats_learn_t msg;
    char *status[] = SHR_FT_LEARN_PKT_STATUS_NAMES;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int max_status;
    int rv;
    int i = 0;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    sal_memset(&msg, 0, sizeof(msg));

    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_STATS_LEARN_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_STATS_LEARN_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_stats_learn_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
    }

    /* Get the max status, they maybe different depending on SDK, FW version */
    max_status  = _BCM_FT_MIN(msg.num_status, SHR_FT_LEARN_PKT_MAX_STATUS);

    LOG_CLI((BSL_META(" +------------------------+-------------+--------------------+ \n")));
    LOG_CLI((BSL_META(" |         Status         |    Count    |   Average (usec)   | \n")));
    LOG_CLI((BSL_META(" +------------------------+-------------+--------------------+ \n")));
    for (i = 0; i < max_status; i++) {
        LOG_CLI((BSL_META(" | %s | %11u | %18u |\n"),
                          status[i], msg.count[i], msg.avg_usecs[i]));
        LOG_CLI((BSL_META(" +------------------------+-------------+--------------------+ \n")));
    }
    LOG_CLI((BSL_META("\n")));

}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_export
 * Purpose:
 *      Dump the export statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_flowtracker_dump_stats_export(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_msg_ctrl_stats_export_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    sal_memset(&msg, 0, sizeof(msg));

    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_STATS_EXPORT_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_STATS_EXPORT_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_stats_export_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
    }

    LOG_CLI((BSL_META("Export packets      : %u \n"), msg.num_pkts));
    LOG_CLI((BSL_META("Export flows        : %u \n"), msg.num_flows));
    LOG_CLI((BSL_META("Export time stamp   : %u \n"), msg.ts));
    LOG_CLI((BSL_META("Sequence number     : %u \n"), msg.seq_num));
    LOG_CLI((BSL_META("\n")));

    return;
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_age
 * Purpose:
 *      Dump the age statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_flowtracker_dump_stats_age(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_msg_ctrl_stats_age_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    if (ft_info->elph_mode == TRUE) {
        LOG_CLI((BSL_META("Cannot dump age stats in elephant mode \r\n")));
        return;
    }


    sal_memset(&msg, 0, sizeof(msg));

    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_STATS_AGE_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_STATS_AGE_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_stats_age_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
    }

    LOG_CLI((BSL_META("Num flows aged : %u \n"), msg.num_flows));
    LOG_CLI((BSL_META("\n")));

    return;
}

/*
 * Function:
 *      _bcm_esw_flowtracker_dump_stats_elephant
 * Purpose:
 *      Dump the elephant statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_flowtracker_dump_stats_elephant(int unit)
{

    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_msg_ctrl_stats_elph_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    char *states[SHR_FT_ELPH_FLOW_STATE_COUNT] = SHR_FT_ELPH_STATES;
    int num_states, num_actions;
    int i, j;
    int rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    if (ft_info->elph_mode != TRUE) {
        LOG_CLI((BSL_META("Not in elephant mode \r\n")));
        return;
    }


    sal_memset(&msg, 0, sizeof(msg));

    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_STATS_ELPH_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_STATS_ELPH_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_msg_ctrl_stats_elph_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
    }

    /* Take the minimum value to avoid any version difference issues */
    num_states  = _BCM_FT_MIN(msg.num_states, SHR_FT_ELPH_FLOW_STATE_COUNT);
    num_actions = _BCM_FT_MIN(msg.num_actions, SHR_FT_ELPH_ACTION_COUNT);

    LOG_CLI((BSL_META(" +-------------------+------------+------------+------------+ \n")));
    LOG_CLI((BSL_META(" |                   |    None    |   Promote  |   Demote   | \n")));
    LOG_CLI((BSL_META(" +-------------------+------------+------------+------------+ \n")));
    for (i = 0; i < num_states; i++) {
        LOG_CLI((BSL_META(" | %10s |"), states[i]));
        for (j = 0; j < num_actions; j++) {
            LOG_CLI((BSL_META(" %10u |"), msg.trans_count[i][j]));
        }
        LOG_CLI((BSL_META("\n")));
        LOG_CLI((BSL_META(" +-------------------+------------+------------+------------+ \n")));
    }
    LOG_CLI((BSL_META("\n")));

    return;
}


/* Dump routine for dumping flow group list,
 * collector list and template list.
 */
void _bcm_xgs5_flowtracker_sw_dump(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return;
    }

    _bcm_xgs5_ft_flow_group_list_dump(unit);
    _bcm_xgs5_ft_collector_list_dump(unit);
    _bcm_xgs5_ft_template_list_dump(unit);
    _bcm_xgs5_ft_elph_profile_list_dump(unit);
    _bcm_xgs5_flowtracker_dump_stats_learn(unit);
    _bcm_xgs5_flowtracker_dump_stats_export(unit);
    if (ft_info->elph_mode == TRUE) {
        _bcm_xgs5_flowtracker_dump_stats_elephant(unit);
    } else {
        _bcm_xgs5_flowtracker_dump_stats_age(unit);
    }
}

/* Helper functions to get unit specific details for QCM */

/*
 * Function:
 *     _bcm_xgs5_qcm_max_tm_per_pipe_get 
 * Purpose:
 *     Provides the number of Traffic manager blocks 
 *     present per pipe in a given device.
 * Parameters:
 *     unit           - (IN) BCM device number
 * Returns:
 *     Number of Traffic manager blocks present per pipe.
 */
static int _bcm_xgs5_qcm_max_tm_per_pipe_get(int unit)
{
    if (SOC_IS_TOMAHAWK3(unit)) {
        return 2; /* Number of ITMs per pipe */
    } else if (SOC_IS_TOMAHAWK(unit)) {
        return 4; /* Number of XPEs per pipe */
    }
    return 0;
}

static int _bcm_xgs5_qcm_global_flow_view_size_get(int unit)
{
    int max_tm_per_pipe      = 0;
    int timestamp_size       = 8; /* 8 bytes of timestamp data */
    int num_flows_size       = 2; /* 2 bytes of num_flows data */
    int view_id_size         = 2; /* 2 bytes of view_id data */
    int shared_buf_util_size = 0;

    max_tm_per_pipe = _bcm_xgs5_qcm_max_tm_per_pipe_get(unit);
    shared_buf_util_size = (SHR_QCM_MAX_NUM_SAMPLE * max_tm_per_pipe);
    return (timestamp_size + num_flows_size + view_id_size + shared_buf_util_size);
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_cfg_init_set
 * Purpose:
 *     Customer app provides the buffer address and size to the QCM App.
 * Parameters:
 *     unit           - (IN) BCM device number
 *     cfg_int        - (IN) User allocated buffer size
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_cfg_init_set(int unit,
                                       _bcm_esw_ft_qcm_init_cfg_t cfg_init)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_qcm_init_cfg_t cfg_init_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int i;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&cfg_init_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_qcm_init_cfg_t));

    cfg_init_msg.max_port_num          = cfg_init.max_port_num;
    cfg_init_msg.queues_per_pipe       = cfg_init.queues_per_pipe;
    cfg_init_msg.queues_per_port       = cfg_init.queues_per_port;
    cfg_init_msg.ports_per_pipe        = cfg_init.ports_per_pipe;
    cfg_init_msg.num_pipes             = cfg_init.num_pipes;
    cfg_init_msg.num_counters_per_pipe = cfg_init.num_counters_per_pipe;
    for (i = 0;
         i < (cfg_init_msg.num_pipes * cfg_init_msg.num_counters_per_pipe);
         ++i) {
        cfg_init_msg.mmu_util_base[i]      = cfg_init.util_index[i];
        cfg_init_msg.mmu_drop_base[i]      = cfg_init.drop_index[i];
        cfg_init_msg.mmu_acc_type[i]       = cfg_init.acc_type[i];
    }

    for (i = 0; i < cfg_init_msg.num_pipes; ++i) {
        cfg_init_msg.mmu_egr_base[i]       = cfg_init.egr_index[i];
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_qcm_init_cfg_pack(buffer_req, &cfg_init_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_QCM_CFG_INIT_SET,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_QCM_CFG_INIT_SET_REPLY, &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_host_buf_set
 * Purpose:
 *     Customer app provides the buffer address and size to the QCM App.
 *     This is a one time config and no dynamic change in the address or the size is permitted.
 *     QCM App writes the processed information into this buffer in a
 *     circular fashion and expect customer application to read periodically.
 * Parameters:
 *     unit           - (IN) BCM device number
 *     host_mem_size  - (IN) User allocated buffer size
 *     host_mem_addr   - (IN) User allocated buffer address.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_host_buf_set(int unit,
                                       uint32 host_mem_size,
                                       uint32 host_mem_addr)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_host_buf_t host_buf_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&host_buf_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_host_buf_t));

    host_buf_msg.host_buf_addr         = host_mem_addr;
    host_buf_msg.host_buf_size         = host_mem_size;

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_pack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_CREATE_REPLY, &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_host_buf_get
 * Purpose:
 *     Customer app provides the buffer address and size to the QCM App.
 *     This is a one time config and no dynamic change in the address or the size is permitted.
 *     QCM App writes the processed information into this buffer in a
 *     circular fashion and expect customer application to read periodically.
 * Parameters:
 *     unit           - (IN)  BCM device number
 *     host_mem_size  - (OUT) User allocated buffer size
 *     host_mem_addr   - (OUT) User allocated buffer address.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_host_buf_get(int unit,
                                       uint32 *host_mem_size,
                                       uint32 **host_mem_addr)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_host_buf_t host_buf_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&host_buf_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_host_buf_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_unpack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    *host_mem_size = host_buf_msg.host_buf_size;
    *host_mem_addr = (uint32 *)soc_cm_p2l(unit, host_buf_msg.host_buf_addr);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_config_set
 * Purpose:
 *     Array gports that are being monitored by the QCM App.
 * Parameters:
 *     unit                 - (IN) BCM device number
 *     num_gports           - (IN) Number of gports to be monitor by App
 *     cfg_info             - (IN) Gports config information that are being monitored by App
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_config_set(int unit,
                                     int num_gports,
                                     _bcm_esw_ft_qcm_port_cfg_t *cfg_info)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_config_t config_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int i, j, *index, tmp;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }

    index = (int *) soc_cm_salloc(unit, (num_gports * sizeof(int)), "microburst temp linked list");
    if (num_gports && (index == NULL)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Failed to create local memory\n")));
        return BCM_E_MEMORY;
    }
    sal_memset(index, 0, (num_gports * sizeof(int)));
    sal_memset(&config_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_config_t));
    config_msg.num_gports         = num_gports;

    for (i = 0; i < num_gports; i++) {
        index[i] = i;
    }
    for (i = 0; i < num_gports; i++) {
        for (j = (i + 1); j < num_gports; j++) {
            if (cfg_info[index[i]].global_num > cfg_info[index[j]].global_num) {
                tmp = index[i];
                index[i] = index[j];
                index[j] = tmp;
            }
        }
    }

    for (i = 0; i < num_gports; i++) {
        config_msg.gport[i]                     = cfg_info[index[i]].gport;
        config_msg.port_pipe_queue_num[i]       = cfg_info[index[i]].port_pipe_queue_num;
        config_msg.pport[i]                     = cfg_info[index[i]].pport;
    }
    if (index != NULL) {
        soc_cm_sfree(unit, index);
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_config_pack(buffer_req, &config_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_QCM_CONFIG_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_QCM_CONFIG_CREATE_REPLY, &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_config_get
 * Purpose:
 *     Array gports that are being monitored by the QCM App.
 * Parameters:
 *     unit           - (IN)  BCM device number
 *     max_gports     - (IN)  Maximum number of gport entries to be read
 *     num_gports     - (OUT) Number of gports to be monitor by App
 *     gport_list     - (OUT) Array of gports that are being monitored by App
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_config_get(int unit,
        int max_gports, bcm_gport_t *gport_list, int *num_gports)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_config_t config_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int i, rv = BCM_E_NONE;
    uint32  entries_transfer;

    if (!max_gports) {
        return BCM_E_NONE;
    }

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&config_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_config_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_CONFIG_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_CONFIG_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM config get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_config_unpack(buffer_req, &config_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM config get failure rv=%d\r\n"), rv));
        return rv;
    }

    *num_gports = config_msg.num_gports;
    if (*num_gports > max_gports) {
        entries_transfer = max_gports;
    } else {
        entries_transfer = *num_gports;
    }
    for (i = 0; i < entries_transfer; i++) {
        gport_list[i]   =   config_msg.gport[i];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_set
 * Purpose:
 *     Set Queue congestion monitoring flow view configuration.
 * Parameters:
 *     unit                 - (IN) BCM device number
 *     flow_view            - (IN) QCM flow view information.
 *     options              - (IN) Update and clear options
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_set(int unit, uint32 options,
        bcm_cosq_burst_monitor_flow_view_info_t *flow_view)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_flow_view_t flow_cfg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int    i;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }

    sal_memset(&flow_cfg, 0, sizeof(shr_ft_qcm_msg_ctrl_flow_view_t));
    if (options == BCM_QCM_CONFIG_UPDATE) {
        if (!flow_view->age_threshold) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "Age threshold is too low\n")));
            return BCM_E_PARAM;
        }
        if (flow_view->num_samples > SHR_QCM_MAX_NUM_SAMPLE) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "Number of sample is too High\n")));
            return BCM_E_PARAM;
        }
        if (flow_view->num_flows > ft_info->max_flows_total) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "num_flows : %d is greater than max_flows_total: %d\n"),
                      flow_view->num_flows, ft_info->max_flows_total));
            return BCM_E_PARAM;
        }
        if (flow_view->num_flows &&
            (flow_view->scan_interval_usecs < SHR_QCM_MIN_SCAN_INTERVAL)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "scan interval is too low\n")));
            return BCM_E_PARAM;
        }
        if (flow_view->num_flows_clear > ((ft_info->max_flows_total * 2) / 10)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "FT QCM(unit %d) Error: Number of flow clear %d: "
                      "is too High. Maximum :%d \n"),
                      unit, flow_view->num_flows_clear,
                      ((ft_info->max_flows_total * 2) / 10)));
            return BCM_E_PARAM;
        }

        for (i = 0; i < SHR_FT_QCM_INTEREST_FACTOR_WEIGHT_MAX_COUNT; i++) {
            flow_cfg.weights[i]    = flow_view->export_param.weights[i];
        }
        flow_cfg.scan_interval     = flow_view->scan_interval_usecs;
        flow_cfg.age_threshold     = flow_view->age_threshold;
        flow_cfg.num_samples       = flow_view->num_samples;
        flow_cfg.num_flows         = flow_view->num_flows;
        flow_cfg.num_flows_clear   = flow_view->num_flows_clear;
        if (flow_view->flags & BCM_BURST_MONITOR_SCAN_INTERVAL_COLLECTION) {
            flow_cfg.flags  |= SHR_FT_QCM_MSG_SCAN_INTERVAL_COLLECTION_ENABLE;
        }

        COMPILER_64_TO_32_HI(flow_cfg.export_threshold_hi,
                             flow_view->export_param.export_threshold);

        COMPILER_64_TO_32_LO(flow_cfg.export_threshold_lo,
                             flow_view->export_param.export_threshold);

        COMPILER_64_TO_32_HI(flow_cfg.start_timer_hi,
                             flow_view->start_timer_usecs);

        COMPILER_64_TO_32_LO(flow_cfg.start_timer_lo,
                             flow_view->start_timer_usecs);

        COMPILER_64_TO_32_HI(flow_cfg.age_timer_hi,
                             flow_view->age_timer_usecs);

        COMPILER_64_TO_32_LO(flow_cfg.age_timer_lo,
                             flow_view->age_timer_usecs);
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_flow_view_cfg_pack(buffer_req, &flow_cfg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_QCM_FLOW_VIEW_CONFIG_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_QCM_FLOW_VIEW_CONFIG_CREATE_REPLY, &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_get
 * Purpose:
 *     Set Queue congestion monitoring flow view configuration.
 * Parameters:
 *     unit           - (IN)  BCM device number
 *     flow_view      - (OUT) QCM flow view information.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_get(int unit,
        bcm_cosq_burst_monitor_flow_view_info_t *flow_view)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_flow_view_t flow_cfg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int i, rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&flow_cfg, 0, sizeof(shr_ft_qcm_msg_ctrl_flow_view_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_FLOW_VIEW_CONFIG_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_FLOW_VIEW_CONFIG_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM config get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_flow_view_cfg_unpack(buffer_req, &flow_cfg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM config get failure rv=%d\r\n"), rv));
        return rv;
    }

    for (i = 0; i < SHR_FT_QCM_INTEREST_FACTOR_WEIGHT_MAX_COUNT; i++) {
        flow_view->export_param.weights[i]     = flow_cfg.weights[i];
    }
    flow_view->scan_interval_usecs             = flow_cfg.scan_interval;
    flow_view->age_threshold                   = flow_cfg.age_threshold;
    flow_view->num_samples                     = flow_cfg.num_samples;
    flow_view->num_flows                       = flow_cfg.num_flows;
    flow_view->num_flows_clear                 = flow_cfg.num_flows_clear;
    if (flow_cfg.flags & SHR_FT_QCM_MSG_SCAN_INTERVAL_COLLECTION_ENABLE) {
        flow_view->flags  |= BCM_BURST_MONITOR_SCAN_INTERVAL_COLLECTION;
    }

    COMPILER_64_SET(flow_view->export_param.export_threshold,
                    flow_cfg.export_threshold_hi,
                    flow_cfg.export_threshold_lo);
    COMPILER_64_SET(flow_view->start_timer_usecs, flow_cfg.start_timer_hi,
                    flow_cfg.start_timer_lo);
    COMPILER_64_SET(flow_view->age_timer_usecs, flow_cfg.age_timer_hi,
                    flow_cfg.age_timer_lo);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_flow_view_data_clear
 * Purpose:
 *     Queue congestion monitoring clear flow view data.
 * Parameters:
 *     unit                 - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_flow_view_data_clear(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_host_buf_t host_buf_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;
    uint32 host_mem_size;
    uint32 *host_mem_addr = NULL;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&host_buf_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_host_buf_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_unpack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    if ((!host_buf_msg.host_buf_addr) ||
        (!host_buf_msg.host_buf_size)) {
        LOG_CLI((BSL_META("QCM host buffer Not configured \n")));
        return BCM_E_PARAM;
    }
    host_mem_size = host_buf_msg.host_buf_size;
    host_mem_addr = (uint32 *)soc_cm_p2l(unit, host_buf_msg.host_buf_addr);
    host_buf_msg.host_buf_addr      = 0;
    host_buf_msg.host_buf_size      = 0;

    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_pack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_CREATE_REPLY, &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));
    sal_memset(host_mem_addr, 0, host_mem_size);
    host_buf_msg.host_buf_size      = host_mem_size;
    host_buf_msg.host_buf_addr      = (uint32)soc_cm_l2p(unit, host_mem_addr);

    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_pack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_msg_send_receive(unit, MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_CREATE_REPLY, &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

int _bcm_xgs5_ft_qcm_eapp_flow_view_data_get(int unit,
        uint32 usr_mem_size, uint8 *usr_mem_addr)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_host_buf_t host_buf_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint8 *host_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0, idx, flow_idx;
    int rv = BCM_E_NONE;
    uint32 *host_mem_addr = NULL;
    uint32 host_mem_size;
    uint16 num_flows, view_id;

    if ((!usr_mem_size) || (!usr_mem_addr)) {
        LOG_CLI((BSL_META("QCM usr buffer is  not configure:\n")));
        return BCM_E_PARAM;
    }

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&host_buf_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_host_buf_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_unpack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    if ((!host_buf_msg.host_buf_addr) ||
        (!host_buf_msg.host_buf_size)) {
        LOG_CLI((BSL_META("QCM host buffer Not configured \n")));
        return BCM_E_INIT;
    }

    if (!host_buf_msg.num_flow_views) {
        LOG_CLI((BSL_META("QCM host buffer is too small minimum flow view size is %d\n"),
                           host_buf_msg.flow_view_size));
        return BCM_E_PARAM;
    }

    host_mem_addr = (uint32 *)soc_cm_p2l(unit, host_buf_msg.host_buf_addr);
    host_mem_size = host_buf_msg.host_buf_size;
    host_ptr = (uint8 *)host_mem_addr;

    /* Flow view summary information */
    if ((usr_mem_size >= (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS)) &&
        (host_mem_size >= (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS))) {
        sal_memcpy(usr_mem_addr, host_ptr,
                (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS));
    } else {
        return BCM_E_NONE;
    }
    usr_mem_size -= (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS);
    host_mem_size -= (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS);
    usr_mem_addr += (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS);
    host_ptr += (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS);

    for (idx = 0; idx < host_buf_msg.num_flow_views; ++idx) {
        num_flows     = (uint16)(host_ptr[8]);
        num_flows     = (uint16)((num_flows << 8) | host_ptr[9]);
        view_id       = (uint16)(host_ptr[10]);
        view_id       = (uint16)((view_id << 8) | host_ptr[11]);

        if ((usr_mem_size >= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID) &&
            (host_mem_size >= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID)) {
            sal_memcpy(usr_mem_addr, host_ptr, SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID);
        } else {
            return BCM_E_NONE;
        }
        usr_mem_size -= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;
        host_mem_size -= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;
        usr_mem_addr += SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;
        host_ptr += SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;

        if ((usr_mem_size >= (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit))) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit)))) {
            sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit)));
        } else {
            return BCM_E_NONE;
        }
        usr_mem_size -= (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));
        usr_mem_addr += (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));

        for (flow_idx = 0; flow_idx < num_flows; ++flow_idx) {
            /* 5- Tuple */
            if ((usr_mem_size >= SHR_QCM_TUPLE_SIZE) &&
                (host_mem_size >= SHR_QCM_TUPLE_SIZE)) {
                sal_memcpy(usr_mem_addr, host_ptr, SHR_QCM_TUPLE_SIZE);
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= SHR_QCM_TUPLE_SIZE;
            host_mem_size -= SHR_QCM_TUPLE_SIZE;
            usr_mem_addr += SHR_QCM_TUPLE_SIZE;
            host_ptr += SHR_QCM_TUPLE_SIZE;

            /* Rx flow byte count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);

            /* Rx flow pkt count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);

            /* Queue Drop byte count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);

            /* Queue Drop pkt count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);

            /* Queue Rx byte count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);

            /* Queue Rx pkt count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);

            /* Queue Shared pool Utilization count */
            if ((usr_mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT)) &&
                (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_SHARE_COUNT))) {
                sal_memcpy(usr_mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT));
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT);
            host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_SHARE_COUNT);
            usr_mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT);
            host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_SHARE_COUNT);

            /* Queue Port Number and Queue ID */
            if ((usr_mem_size >= SHR_QCM_PORT_QUEUE_SIZE) &&
                (host_mem_size >= SHR_QCM_PORT_QUEUE_SIZE)) {
                sal_memcpy(usr_mem_addr, host_ptr, SHR_QCM_PORT_QUEUE_SIZE);
            } else {
                return BCM_E_NONE;
            }
            usr_mem_size -= SHR_QCM_PORT_QUEUE_SIZE;
            host_mem_size -= SHR_QCM_PORT_QUEUE_SIZE;
            usr_mem_addr += SHR_QCM_PORT_QUEUE_SIZE;
            host_ptr += SHR_QCM_PORT_QUEUE_SIZE;

            /* Queue and Port is 3 bytes size, 1 bytes of Padding*/
            host_mem_size -= 1;
            host_ptr += 1;

        }
        host_ptr += ((ft_info->max_flows_total - flow_idx) *
                     (SHR_QCM_TUPLE_SIZE +
                      (SHR_QCM_MAX_NUM_SAMPLE *
                       (SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT)) +
                      (SHR_QCM_MAX_NUM_SAMPLE *
                       (((SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT) * 2) +
                        SHR_QCM_NUM_SHARE_COUNT)) +
                      (SHR_QCM_PORT_QUEUE_SIZE + 1 /* Padding */)));
        host_mem_size -= ((ft_info->max_flows_total - flow_idx) *
                          (SHR_QCM_TUPLE_SIZE +
                           (SHR_QCM_MAX_NUM_SAMPLE *
                            (SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT)) +
                           (SHR_QCM_MAX_NUM_SAMPLE *
                            (((SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT) * 2) +
                             SHR_QCM_NUM_SHARE_COUNT)) +
                           (SHR_QCM_PORT_QUEUE_SIZE + 1 /* Padding */)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_ft_qcm_eapp_get_current_time
 * Purpose:
 *     Get Queue congestion monitoring FW current time.
 * Parameters:
 *     unit           - (IN)  BCM device number
 *     time_usecs     - (OUT) Current qcm time in usec
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_xgs5_ft_qcm_eapp_get_current_time(int unit, uint64 *time_usecs)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_cur_time_t cur_msg_time;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&cur_msg_time, 0, sizeof(shr_ft_qcm_msg_cur_time_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_CURRENT_TIME_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_CURRENT_TIME_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM get fw current time failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_cur_time_unpack(buffer_req, &cur_msg_time);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM get fw current time failure rv=%d\r\n"), rv));
        return rv;
    }

    COMPILER_64_SET(*time_usecs, cur_msg_time.time_usecs_hi,
                    cur_msg_time.time_usecs_lo);

    return BCM_E_NONE;
}

int _bcm_xgs5_ft_qcm_eapp_view_summary_get(int unit, uint32 mem_size,
                                           uint8 *mem_addr, int max_num_views,
                                           int *num_views)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_host_buf_t host_buf_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint8 *host_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;
    uint32 *host_mem_addr = NULL;

    if ((!mem_size) || (!mem_addr)) {
        LOG_CLI((BSL_META("QCM usr buffer is  not configure:\n")));
        return BCM_E_PARAM;
    }

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&host_buf_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_host_buf_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_unpack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    if ((!host_buf_msg.host_buf_addr) ||
        (!host_buf_msg.host_buf_size)) {
        LOG_CLI((BSL_META("QCM host buffer Not configured \n")));
        return BCM_E_INIT;
    }

    if (!host_buf_msg.num_flow_views) {
        LOG_CLI((BSL_META("QCM host buffer is too small minimum flow view size is %d\n"),
                           host_buf_msg.flow_view_size));
        return BCM_E_PARAM;
    }

    host_mem_addr = (uint32 *)soc_cm_p2l(unit, host_buf_msg.host_buf_addr);
    host_ptr = (uint8 *)host_mem_addr;

    /* Flow view summary information */
    if (max_num_views > (mem_size / SHR_QCM_FLOW_VIEW_SUMMARY_SIZE)) {
        *num_views = (mem_size / SHR_QCM_FLOW_VIEW_SUMMARY_SIZE);
    } else {
        *num_views = max_num_views;
    }

    if (*num_views > host_buf_msg.num_flow_views) {
        *num_views = host_buf_msg.num_flow_views;
    }
    sal_memcpy(mem_addr, host_ptr,
            (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * (*num_views)));
    return BCM_E_NONE;
}

int _bcm_xgs5_ft_qcm_eapp_flow_view_stats_get(int unit, uint32 mem_size,
                                              uint8 *mem_addr, int max_num_flows,
                                              int view_id, uint32 flags, int *num_flows)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_host_buf_t host_buf_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint8 *host_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0, idx, flow_idx;
    int rv = BCM_E_NONE;
    uint32 *host_mem_addr = NULL;
    uint32 host_mem_size;
    uint16 total_num_flows = 0, flow_view_id;

    if ((!mem_size) || (!mem_addr)) {
        LOG_CLI((BSL_META("QCM usr buffer is  not configure:\n")));
        return BCM_E_PARAM;
    }

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return BCM_E_CONFIG;
    }
    sal_memset(&host_buf_msg, 0, sizeof(shr_ft_qcm_msg_ctrl_host_buf_t));

    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_HOST_BUF_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_host_buf_unpack(buffer_req, &host_buf_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("QCM host buffer get failure rv=%d\r\n"), rv));
        return rv;
    }

    if ((!host_buf_msg.host_buf_addr) ||
        (!host_buf_msg.host_buf_size)) {
        LOG_CLI((BSL_META("QCM host buffer Not configured \n")));
        return BCM_E_INIT;
    }

    if (!host_buf_msg.num_flow_views) {
        LOG_CLI((BSL_META("QCM host buffer is too small minimum flow view size is %d\n"),
                           host_buf_msg.flow_view_size));
        return BCM_E_PARAM;
    }

    host_mem_addr = (uint32 *)soc_cm_p2l(unit, host_buf_msg.host_buf_addr);
    host_mem_size = host_buf_msg.host_buf_size;
    host_ptr = (uint8 *)host_mem_addr;

    /* Flow view summary information */
    host_mem_size -= (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS);
    host_ptr += (SHR_QCM_FLOW_VIEW_SUMMARY_SIZE * SHR_QCM_MAX_FLOW_VIEWS);

    for (idx = 0; idx < host_buf_msg.num_flow_views; ++idx) {
        total_num_flows     = (uint16)(host_ptr[8]);
        total_num_flows     = (uint16)((total_num_flows << 8) | host_ptr[9]);
        flow_view_id        = (uint16)(host_ptr[10]);
        flow_view_id        = (uint16)((flow_view_id << 8) | host_ptr[11]);
        if (view_id == flow_view_id) {
            break;
        }
        host_ptr += (_bcm_xgs5_qcm_global_flow_view_size_get(unit) +
                     (ft_info->max_flows_total *
                      (SHR_QCM_TUPLE_SIZE +
                       (SHR_QCM_MAX_NUM_SAMPLE *
                        (SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT)) +
                       (SHR_QCM_MAX_NUM_SAMPLE *
                        (((SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT) * 2) +
                         SHR_QCM_NUM_SHARE_COUNT)) +
                       (SHR_QCM_PORT_QUEUE_SIZE + 1 /* Padding */))));
        host_mem_size -= (_bcm_xgs5_qcm_global_flow_view_size_get(unit) +
                          (ft_info->max_flows_total *
                           (SHR_QCM_TUPLE_SIZE +
                            (SHR_QCM_MAX_NUM_SAMPLE *
                             (SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT)) +
                            (SHR_QCM_MAX_NUM_SAMPLE *
                             (((SHR_QCM_NUM_BYTE_COUNT + SHR_QCM_NUM_PKT_COUNT) * 2) +
                              SHR_QCM_NUM_SHARE_COUNT)) +
                            (SHR_QCM_PORT_QUEUE_SIZE + 1 /* Padding */))));
    }

    if (idx == host_buf_msg.num_flow_views) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                  "QCM invalid view ID \n")));
        return BCM_E_PARAM;
    }

    if ((mem_size >= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID) &&
        (host_mem_size >= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID)) {
        sal_memcpy(mem_addr, host_ptr, SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID);
    } else {
        return BCM_E_NONE;
    }
    mem_size -= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;
    host_mem_size -= SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;
    mem_addr += SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;
    host_ptr += SHR_QCM_TIME_STAMP_FLOW_NUM_VIEW_ID;

    if ((mem_size >= (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit))) &&
        (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit)))) {
        sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit)));
    } else {
        return BCM_E_NONE;
    }
    mem_size -= (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));
    host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));
    mem_addr += (host_buf_msg.num_samples * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));
    host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * _bcm_xgs5_qcm_max_tm_per_pipe_get(unit));

    for (flow_idx = 0; flow_idx < total_num_flows; ++flow_idx) {
        /* 5- Tuple */
        if ((mem_size >= SHR_QCM_TUPLE_SIZE) &&
            (host_mem_size >= SHR_QCM_TUPLE_SIZE)) {
            sal_memcpy(mem_addr, host_ptr, SHR_QCM_TUPLE_SIZE);
        } else {
            return BCM_E_NONE;
        }
        mem_size -= SHR_QCM_TUPLE_SIZE;
        host_mem_size -= SHR_QCM_TUPLE_SIZE;
        mem_addr += SHR_QCM_TUPLE_SIZE;
        host_ptr += SHR_QCM_TUPLE_SIZE;

        /* Rx flow byte count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);

        /* Rx flow pkt count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);

        /* Queue Drop byte count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);

        /* Queue Drop pkt count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);

        /* Queue Rx byte count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_BYTE_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_BYTE_COUNT);

        /* Queue Rx pkt count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_PKT_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_PKT_COUNT);

        /* Queue Shared pool Utilization count */
        if ((mem_size >= (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT)) &&
            (host_mem_size >= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_SHARE_COUNT))) {
            sal_memcpy(mem_addr, host_ptr, (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT));
        } else {
            return BCM_E_NONE;
        }
        mem_size -= (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT);
        host_mem_size -= (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_SHARE_COUNT);
        mem_addr += (host_buf_msg.num_samples * SHR_QCM_NUM_SHARE_COUNT);
        host_ptr += (SHR_QCM_MAX_NUM_SAMPLE * SHR_QCM_NUM_SHARE_COUNT);

        /* Queue Port Number and Queue ID */
        if ((mem_size >= SHR_QCM_PORT_QUEUE_SIZE) &&
            (host_mem_size >= SHR_QCM_PORT_QUEUE_SIZE)) {
            sal_memcpy(mem_addr, host_ptr, SHR_QCM_PORT_QUEUE_SIZE);
        } else {
            return BCM_E_NONE;
        }
        mem_size -= SHR_QCM_PORT_QUEUE_SIZE;
        host_mem_size -= SHR_QCM_PORT_QUEUE_SIZE;
        mem_addr += SHR_QCM_PORT_QUEUE_SIZE;
        host_ptr += SHR_QCM_PORT_QUEUE_SIZE;

        /* Queue and Port is 3 bytes size, 1 bytes of Padding*/
        host_mem_size -= 1;
        host_ptr += 1;

        ++(*num_flows);
        if ((*num_flows) >= max_num_flows) {
            return BCM_E_NONE;
        }
    }

    return BCM_E_NONE;
}

int _bcm_xgs5_ft_qcm_eapp_send_em_group_create_msg(int unit,
                                               shr_ft_qcm_msg_ctrl_em_key_format_t *msg)
{
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_em_key_format_pack(buffer_req, msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
           _bcm_xgs5_ft_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_FT_QCM_EM_GROUP_CREATE,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_FT_QCM_EM_GROUP_CREATE_REPLY,
                                         &reply_len,
                                         SHR_FT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_xgs5_ft_qcm_eapp_send_flow_group_flow_data_msg
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int _bcm_xgs5_ft_qcm_eapp_send_flow_group_flow_data_get_msg(
                                           int unit,
                                           bcm_flowtracker_group_t flow_group_id,
                                           bcm_flowtracker_flow_key_t *flow_key)
{
    _bcm_int_ft_info_t                         *ft_info = FT_INFO_GET(unit);
    uint8                                      *buffer_req = NULL;
    uint8                                      *buffer_ptr = NULL;
    uint16                                      buffer_len = 0, reply_len = 0;
    shr_ft_qcm_msg_ctrl_group_flow_data_get_t   send_msg;

    if ((flow_key->src_ip.addr.ipv4_addr) && (flow_key->src_ip.addr.ipv6_addr[0])) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                  "Input valid IPv4 or IPv6, not both\n")));
        return BCM_E_PARAM;
    }

    sal_memset(&send_msg, 0, sizeof(send_msg));
    send_msg.group_idx         = flow_group_id;
    if (flow_key->src_ip.addr.ipv4_addr) {
        send_msg.src_ip            = flow_key->src_ip.addr.ipv4_addr;
        send_msg.dst_ip            = flow_key->dst_ip.addr.ipv4_addr;
    } else {
        sal_memcpy(send_msg.src_ipv6, flow_key->src_ip.addr.ipv6_addr, SHR_QCM_IPV6_ADDR_LEN);
        sal_memcpy(send_msg.dst_ipv6, flow_key->dst_ip.addr.ipv6_addr, SHR_QCM_IPV6_ADDR_LEN);
    }
    send_msg.l4_src_port       = flow_key->l4_src_port;
    send_msg.l4_dst_port       = flow_key->l4_dst_port;
    send_msg.ip_protocol       = flow_key->ip_protocol;
    send_msg.in_port           = flow_key->in_port;

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_qcm_msg_ctrl_group_flow_data_get_pack(buffer_req, &send_msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
      _bcm_xgs5_ft_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET_REPLY,
                                    &reply_len,
                                    SHR_FT_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Dump routine for dumping QCM info
 */
void _bcm_xgs5_qcm_sw_dump(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return;
    }

    _bcm_xgs5_qcm_dump_stats_learn(unit);
    _bcm_xgs5_qcm_dump_stats_scan(unit);
    _bcm_xgs5_qcm_dump_stats_export(unit);
    _bcm_xgs5_qcm_dump_stats_age(unit);
}

/*
 * Function:
 *      _bcm_xgs5_qcm_stats_enable
 * Purpose:
 *      Enable the stats update
 * Parameters:
 *      unit             - (IN)    BCM device number
        enable           - (IN)    enable/disable
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_qcm_stats_enable(int unit, uint8 enable)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8                                      *buffer_req = NULL;
    uint8                                      *buffer_ptr = NULL;
    uint16                                      buffer_len = 0, reply_len = 0;
    shr_ft_qcm_msg_ctrl_stats_enable_t msg;
    int rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        LOG_CLI((BSL_META("QCM not enabled \r\n")));
        return;
    }
    sal_memset(&msg, 0, sizeof(msg));
    msg.enable = enable;

    buffer_req = ft_info->dma_buffer;
    buffer_ptr = shr_ft_qcm_msg_ctrl_stats_enable_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_STATS_ENABLE,
                                       buffer_len, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_STATS_ENABLE_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }
}

/*
 * Function:
 *      _bcm_esw_qcm_dump_stats_learn
 * Purpose:
 *      Dump the learn statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_qcm_dump_stats_learn(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_node = NULL;
    shr_ft_qcm_msg_ctrl_stats_learn_t msg;
    char *status[] = SHR_FT_QCM_LEARN_PKT_STATUS_NAMES;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int max_status;
    int rv;
   int i = 0;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        LOG_CLI((BSL_META("QCM not enabled \r\n")));
        return;
    }
    sal_memset(&msg, 0, sizeof(msg));

    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_STATS_LEARN_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_STATS_LEARN_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }
    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_stats_learn_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
       LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
   }

    /* Get the max status, they maybe different depending on SDK, FW version */
    max_status  = _BCM_FT_MIN(msg.num_status, SHR_FT_LEARN_PKT_MAX_STATUS);

    LOG_CLI((BSL_META(" +------------------------+-------------+ \n")));
    LOG_CLI((BSL_META(" |         Status         |    Count    | \n")));
    LOG_CLI((BSL_META(" +------------------------+-------------+ \n")));
    for (i = 0; i < max_status; i++) {
        LOG_CLI((BSL_META(" | %s | %11u |\n"),
                          status[i], msg.count[i]));
        LOG_CLI((BSL_META(" +------------------------+-------------+ \n")));
    }

    for (i = 0; i < ft_info->num_pipes; i++) {
        LOG_CLI((BSL_META(" | Pipe %d                 | %11u |\n"),
                          i, msg.pipe_flowcount[i]));
        LOG_CLI((BSL_META(" +------------------------+-------------+ \n")));
    }

    for (i = BCM_INT_FT_GROUP_START_ID;
        i < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups); i++) {
        flow_group_node = &(ft_flow_group_info_list[unit][i]);
        if (flow_group_node->flow_group_id != 0) {
            LOG_CLI((BSL_META(" | Group %d                | %11u |\n"),
                              flow_group_node->flow_group_id,
                              msg.group_flowcount[i]));
            LOG_CLI((BSL_META(" +------------------------+-------------+ \n")));
        }
    }
    LOG_CLI((BSL_META("\n")));
}

/*
 * Function:
 *      _bcm_esw_qcm_dump_stats_export
 * Purpose:
 *      Dump the export statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_qcm_dump_stats_export(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_stats_export_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        return;
    }

    sal_memset(&msg, 0, sizeof(msg));
    /* Get the statistics from the FW */
   rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_FT_STATS_EXPORT_GET,
                                      0, 0,
                                      MOS_MSG_SUBCLASS_FT_STATS_EXPORT_GET_REPLY,
                                      &reply_len,
                                      SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }

    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_stats_export_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
    }

    LOG_CLI((BSL_META("Total No of flow views exported    : %u \n"), msg.num_flow_views));
    LOG_CLI((BSL_META("Total No of Packets exported       : %u \n"), msg.num_pkts));
    LOG_CLI((BSL_META("IPfix Sequence number              : %u \n"), msg.seq_num));
    LOG_CLI((BSL_META("Min InterestFactor                 : 0x%08x %08x\n"), msg.min_interest_factor_upper, msg.min_interest_factor_lower));
    LOG_CLI((BSL_META("Max InterestFactor                 : 0x%08x %08x\n"), msg.max_interest_factor_upper, msg.max_interest_factor_lower));
    LOG_CLI((BSL_META("Last Exported View ID              : %u \n"), msg.view_id));
    LOG_CLI((BSL_META("\tNum Pkts                   : 0x%08x %08x\n"), msg.pkts_per_view_upper, msg.pkts_per_view_lower));
    LOG_CLI((BSL_META("\tNum Flows                  : %u\n"), msg.flows_per_view));
    LOG_CLI((BSL_META("\tInterestFactor             : 0x%08x %08x\n"), msg.interest_factor_upper, msg.interest_factor_lower));

    LOG_CLI((BSL_META("\n")));
    return;
}

/*
 * Function:
 *      _bcm_esw_qcm_dump_stats_age
 * Purpose:
 *      Dump the age statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_qcm_dump_stats_age(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
   _bcm_int_ft_flow_group_info_t *flow_group_node = NULL;
    shr_ft_qcm_msg_ctrl_stats_age_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
   int i, rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

    if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        LOG_CLI((BSL_META("QCM not enabled \r\n")));
        return;
    }

    sal_memset(&msg, 0, sizeof(msg));

    /* Get the statistics from the FW */
     rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_STATS_AGE_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_STATS_AGE_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

    if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }
    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_stats_age_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
        return;
    }

    LOG_CLI((BSL_META("Num flows aged                     : %u \n"), msg.num_flows));
    LOG_CLI((BSL_META("Num flows aged (Forced)            : %u \n"), msg.num_flows_forced));
    LOG_CLI((BSL_META("Num flows aged (Threshold)         : %u \n"), (msg.num_flows - msg.num_flows_forced)));

    if (ft_flow_group_info_list[unit] == NULL) {
        return;
    }

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (BCM_INT_FT_GROUP_START_ID + ft_info->max_flow_groups); i++) {
        flow_group_node = &(ft_flow_group_info_list[unit][i]);
        if (flow_group_node->flow_group_id != 0) {
            LOG_CLI((BSL_META("Aging interval(ms) (Group %d)       : %u \n"),
                              flow_group_node->flow_group_id,
                              flow_group_node->aging_interval_msecs));
        }
    }
    LOG_CLI((BSL_META("\n")));

    return;
}

/*
 * Function:
 *      _bcm_xgs5_qcm_dump_stats_scan
 * Purpose:
 *      Dump the scan statistics
 * Parameters:
 *      unit             - (IN)    BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void _bcm_xgs5_qcm_dump_stats_scan(int unit)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    shr_ft_qcm_msg_ctrl_stats_scan_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv;

    if (ft_info == NULL) {
        LOG_CLI((BSL_META("Flowtracker not enabled \r\n")));
        return;
    }

   if ((ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) ||
        (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR))) {
        LOG_CLI((BSL_META("QCM not enabled \r\n")));
       return;
    }

    sal_memset(&msg, 0, sizeof(msg));
    /* Get the statistics from the FW */
    rv = _bcm_xgs5_ft_msg_send_receive(unit,
                                       MOS_MSG_SUBCLASS_FT_QCM_STATS_SCAN_GET,
                                       0, 0,
                                       MOS_MSG_SUBCLASS_FT_QCM_STATS_SCAN_GET_REPLY,
                                       &reply_len,
                                       SHR_FT_MAX_UC_MSG_TIMEOUT);

   if (rv != BCM_E_NONE) {
        LOG_CLI((BSL_META("Messaging failure rv=%d\r\n"), rv));
        return;
    }


    buffer_req      = ft_info->dma_buffer;
    buffer_ptr      = shr_ft_qcm_msg_ctrl_stats_scan_get_unpack(buffer_req, &msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        LOG_CLI((BSL_META("Messaging failure \r\n")));
      return;
    }

    LOG_CLI((BSL_META("Min  sample interval obs           : %u\n"), msg.min_sample_interval));
    LOG_CLI((BSL_META("Max  sample interval obs           : %u\n"), msg.max_sample_interval));
    LOG_CLI((BSL_META("Scan interval view obs             : %u\n"), msg.total_scan_interval_view));
    LOG_CLI((BSL_META("Num Flows scanned per view         : %u\n"), msg.num_flows));
    LOG_CLI((BSL_META("\n")));

    return;
}

#else /* INCLUDE_FLOWTRACKER */
typedef int _bcm_xgs5_flowtracker_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_FLOWTRACKER */
