/*! \file shr_util_pack.c
 *
 * Packet pack/unpack utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_util_pack.h>

uint8_t *
shr_util_pack_udp_header_pack(uint8_t *buf, shr_util_pack_udp_header_t *udp)
{
    SHR_UTIL_PACK_U16(buf, udp->src);
    SHR_UTIL_PACK_U16(buf, udp->dst);
    SHR_UTIL_PACK_U16(buf, udp->length);
    SHR_UTIL_PACK_U16(buf, udp->sum);

    return buf;
}

uint8_t *
shr_util_pack_ipv4_header_pack(uint8_t *buf, shr_util_pack_ipv4_header_t *ip)
{
    uint32_t tmp;

    tmp = ((ip->version & 0xf) << 28) | ((ip->h_length & 0xf) << 24) |
        ((ip->dscp & 0x3f) << 18) | ((ip->ecn & 0x3)<< 16) | (ip->length);
    SHR_UTIL_PACK_U32(buf, tmp);

    tmp = (ip->id << 16) | ((ip->flags & 0x7) << 13) | (ip->f_offset & 0x1fff);
    SHR_UTIL_PACK_U32(buf, tmp);

    tmp = (ip->ttl << 24) | (ip->protocol << 16) | ip->sum;
    SHR_UTIL_PACK_U32(buf, tmp);

    SHR_UTIL_PACK_U32(buf, ip->src);
    SHR_UTIL_PACK_U32(buf, ip->dst);

    return buf;
}

uint8_t *
shr_util_pack_ipv6_header_pack(uint8_t *buf, shr_util_pack_ipv6_header_t *ip)
{
    uint32_t tmp, idx;

    tmp = ((ip->version & 0xf) << 28) | (ip->t_class << 20) |
        (ip->f_label & 0xfffff);
    SHR_UTIL_PACK_U32(buf, tmp);

    tmp = (ip->p_length << 16 ) | (ip->next_header << 8) | ip->hop_limit;
    SHR_UTIL_PACK_U32(buf, tmp);

    for (idx = 0; idx < SHR_IPV6_ADDR_LEN; idx++) {
        SHR_UTIL_PACK_U8(buf, ip->src[idx]);
    }

    for (idx = 0; idx < SHR_IPV6_ADDR_LEN; idx++) {
        SHR_UTIL_PACK_U8(buf, ip->dst[idx]);
    }

    return buf;
}

uint8_t *
shr_util_pack_gre_header_pack(uint8_t *buf, shr_util_pack_gre_header_t *gre)
{
    uint32_t tmp;

    tmp = ((gre->checksum & 0x1) << 31) | ((gre->routing & 0x1) << 30) |
        ((gre->key & 0x1) << 29) | ((gre->sequence_num & 0x1) << 28) |
        ((gre->strict_src_route & 0x1) << 27) |
        ((gre->recursion_control & 0x7) << 24) | ((gre->flags & 0x1f) << 19) |
        ((gre->version & 0x7) << 16) | (gre->protocol & 0xffff);

    SHR_UTIL_PACK_U32(buf, tmp);

    return buf;
}

uint8_t *
shr_util_pack_ach_header_pack(uint8_t *buf, shr_util_pack_ach_header_t *ach)
{
    uint32_t tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) | ((ach->version & 0xf) << 24) |
        (ach->reserved << 16) | ach->channel_type;

    SHR_UTIL_PACK_U32(buf, tmp);

    return buf;
}

uint8_t *
shr_util_pack_mpls_label_pack(uint8_t *buf, shr_util_pack_mpls_label_t *mpls)
{
    uint32_t tmp;

    tmp = ((mpls->label & 0xfffff) << 12) | ((mpls->exp & 0x7) << 9) |
        ((mpls->s & 0x1) << 8) | mpls->ttl;
    SHR_UTIL_PACK_U32(buf, tmp);

    return buf;
}

uint8_t *
shr_util_pack_l2_header_pack(uint8_t *buf, shr_util_pack_l2_header_t *l2)
{
    uint32_t tmp, idx;

    for (idx = 0; idx < SHR_MAC_ADDR_LEN; idx++) {
        SHR_UTIL_PACK_U8(buf, l2->dst_mac[idx]);
    }

    for (idx = 0; idx < SHR_MAC_ADDR_LEN; idx++) {
        SHR_UTIL_PACK_U8(buf, l2->src_mac[idx]);
    }

    /*
     * VLAN Tag
     *
     * A TPID value of 0 indicates this is an untagged frame.
     */
    if (l2->outer_vlan_tag.tpid != 0) {
        /* Add VLAN tag */
        tmp = (l2->outer_vlan_tag.tpid << 16) |
            ((l2->outer_vlan_tag.tci.prio & 0x7) << 13) |
            ((l2->outer_vlan_tag.tci.cfi & 0x1) << 12) |
            (l2->outer_vlan_tag.tci.vid & 0xfff);
        SHR_UTIL_PACK_U32(buf, tmp);
    }

    /* Packet inner vlan tag */

    if (l2->inner_vlan_tag.tpid != 0) {
        /* Add VLAN tag */
        tmp = (l2->inner_vlan_tag.tpid << 16) |
            ((l2->inner_vlan_tag.tci.prio & 0x7) << 13) |
            ((l2->inner_vlan_tag.tci.cfi & 0x1) << 12) |
            (l2->inner_vlan_tag.tci.vid & 0xfff);
        SHR_UTIL_PACK_U32(buf, tmp);
    }

    SHR_UTIL_PACK_U16(buf, l2->etype);

    return buf;
}

uint32_t
shr_util_pack_initial_chksum_get(uint16_t length, uint8_t *data)
{
    uint32_t chksum, idx;
    uint16_t w16;

    chksum = 0;
    idx = 0;
    while (length > 1) {
        w16 = (((uint16_t)data[idx]) << 8) + data[idx+1];
        chksum += w16;
        idx += 2;
        length -= 2;
    }
    if (length) {
        w16 = (((uint16_t)data[idx]) << 8) + 0;
        chksum += w16;
    }

    return chksum;
}

uint32_t
shr_util_pack_udp_initial_checksum_get(int v6,
                                       shr_util_pack_ipv4_header_t *ipv4,
                                       shr_util_pack_ipv6_header_t *ipv6,
                                       uint8_t *payload,
                                       shr_util_pack_udp_header_t  *udp)
{
    uint8_t buf[SHR_UDP_HDR_LEN + (SHR_IPV6_ADDR_LEN *2 ) +
                8 + SHR_UDP_PAYLOAD_LEN];
    uint8_t *cur_ptr = buf;
    uint32_t idx, length;

    /* Build IP Pseudo-Header */
    if (v6) {
        /* IPv6 Pseudo-Header
         *     Source address
         *     Destination address
         *     UDP length  (32-bit)
         *     Next Header (lower 8 bits of 32-bit)
         */
        for (idx = 0; idx < SHR_IPV6_ADDR_LEN; idx++) {
            SHR_UTIL_PACK_U8(cur_ptr, ipv6->src[idx]);
        }
        for (idx = 0; idx < SHR_IPV6_ADDR_LEN; idx++) {
            SHR_UTIL_PACK_U8(cur_ptr, ipv6->dst[idx]);
        }

        SHR_UTIL_PACK_U16(cur_ptr, udp->length);
        SHR_UTIL_PACK_U8(cur_ptr, 0);
        SHR_UTIL_PACK_U8(cur_ptr, ipv6->next_header);
    } else {
        /* IPv4 Pseudo-Header
         *     Source address
         *     Destination address
         *     Protocol    (8 bits)
         *     UDP length  (16-bit)
         */
        SHR_UTIL_PACK_U32(cur_ptr, ipv4->src);
        SHR_UTIL_PACK_U32(cur_ptr, ipv4->dst);
        /*
         * Added 0 before protcol value to compute checksum
         * accurately
         */
        SHR_UTIL_PACK_U8(cur_ptr, 0);
        SHR_UTIL_PACK_U8(cur_ptr, ipv4->protocol);
        SHR_UTIL_PACK_U16(cur_ptr, udp->length);
    }

    /* Add UDP header */
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, udp);

    /* Add payload */
    if (payload != NULL) {
        length = udp->length - SHR_UDP_HDR_LEN;
        for (idx = 0; idx < length; idx++) {
            SHR_UTIL_PACK_U8(cur_ptr, payload[idx]);
        }
    }

    /* Calculate initial UDP checksum */
    length = cur_ptr - buf;
    return shr_util_pack_initial_chksum_get(length, buf);
}
