/*! \file bcma_testutil_packet.c
 *
 * Packet / Packet device utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST
static int
reflect(uint32_t data, uint32_t nbits)
{
    uint32_t reflection, i;

    reflection = 0;
    for (i = 0; i < nbits; i++) {
        if (data & 0x00000001) {
            reflection |= (1 << ((nbits - 1) - i));
        }
        data = data >> 1;
    }
    return (reflection);
}

static void
populate_crc_table(void)
{
    uint32_t remainder, topbit;
    uint32_t dividend;
    uint32_t i;
    uint32_t polynomial = 0x04c11db7;

    topbit = (1 << (32 - 1));

    for (dividend = 0; dividend < BCMA_TESTUTIL_PACKET_NUM_ENTRIES_CRC_TABLE; dividend++) {
        remainder = dividend << (32 - 8);

        for (i = 8; i > 0; i--) {
            if (remainder & topbit) {
                remainder = (remainder << 1) ^ polynomial;
            } else {
                remainder = (remainder << 1);
            }
        }
        bcma_testutil_packet_crc_table[dividend] = remainder;
    }


}
static int
generate_calculate_crc(uint8_t *pkt_ptr, uint32_t pkt_size, uint32_t *crc) {
#define NUM_ENTRIES_CRC_TABLE 256
    uint32_t remainder;
    uint32_t i, reflected_remainder;
    uint8_t data;
    uint32_t initial_remainder = 0xffffffff;
    uint32_t final_xor_value = 0xffffffff;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(pkt_ptr, SHR_E_PARAM);

    remainder = initial_remainder;


    for (i = 0; i < (pkt_size - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC); i++) {
        data = reflect(pkt_ptr[i],8) ^ (remainder >> (32 -8));
        remainder = bcma_testutil_packet_crc_table[data] ^ (remainder << 8);
    }
    reflected_remainder = reflect(remainder, 32);


    *crc = (reflected_remainder ^ final_xor_value);

exit:
    SHR_FUNC_EXIT();

}

static int
calculate_hdr_chksum(uint8_t *pkt_ptr, uint32_t hdr_offset,
                                  uint32_t hdr_len, uint32_t *chksum)
{
    uint32_t i, sum = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(pkt_ptr, SHR_E_PARAM);

    for (i = hdr_offset; i < hdr_offset + hdr_len; i+=2) {
        sum += (pkt_ptr[i] << 8) + pkt_ptr[i+1];
    }

    *chksum = ~((sum >> 16 & 0x0000ffff) + (sum & 0x0000ffff));


exit:
    SHR_FUNC_EXIT();
}

void
bcma_testutil_packet_init(void)
{
   populate_crc_table();

}

int
bcma_testutil_packet_fill_svtag(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          bool need_vlan, bool need_svtag, uint32_t svtag,
                           uint32_t vid, uint16_t ethertype,
                          uint32_t pattern, uint32_t pattern_inc)
{
#define ENET_TPID        (0x8100)

    uint8_t *cur_addr;
    uint32_t payload_len;
    uint32_t payload_content;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);
    SHR_NULL_CHECK(src_mac, SHR_E_PARAM);
    SHR_NULL_CHECK(dst_mac, SHR_E_PARAM);

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_len);

    cur_addr = buf->data;

    /* Ethernet header */
    sal_memcpy(cur_addr, dst_mac, sizeof(shr_mac_t));
    cur_addr += sizeof(shr_mac_t);
    sal_memcpy(cur_addr, src_mac, sizeof(shr_mac_t));
    cur_addr += sizeof(shr_mac_t);

    /* VLAN header */
    if (need_vlan == true) {
        *cur_addr++ = ENET_TPID >> 8;
        *cur_addr++ = ENET_TPID & 0xff;
        *cur_addr++ = vid >> 8;
        *cur_addr++ = vid & 0xff;
    }

    if (need_svtag == true) {
        *cur_addr++ = svtag >> 24;
        *cur_addr++ = svtag >> 16;
        *cur_addr++ = svtag >> 8;
        *cur_addr++ = svtag & 0xff;
    }


    /* Ethertype */
    *cur_addr++ = ethertype >> 8;
    *cur_addr++ = ethertype & 0xff;

    /* Fill payload */
    if (pkt_len < (uint32_t)(cur_addr - buf->data)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    payload_len = pkt_len - (int)(cur_addr - buf->data);
    payload_content = pattern;
    while (payload_len >= 4) {
        *cur_addr++ = payload_content >> 24;
        *cur_addr++ = payload_content >> 16;
        *cur_addr++ = payload_content >> 8;
        *cur_addr++ = payload_content & 0xff;
        payload_content += pattern_inc;
        payload_len -= 4;
    }
    while (payload_len != 0) { /* when payload_len = 3~1 */
        *cur_addr++ = payload_content >> (payload_len * 8);
        payload_len--;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_packet_fill(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          bool need_vlan, uint32_t vid, uint16_t ethertype,
                          uint32_t pattern, uint32_t pattern_inc)
{
#define ENET_TPID        (0x8100)

    uint8_t *cur_addr;
    uint32_t payload_len;
    uint32_t payload_content;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);
    SHR_NULL_CHECK(src_mac, SHR_E_PARAM);
    SHR_NULL_CHECK(dst_mac, SHR_E_PARAM);

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_len);

    cur_addr = buf->data;

    /* Ethernet header */
    sal_memcpy(cur_addr, dst_mac, sizeof(shr_mac_t));
    cur_addr += sizeof(shr_mac_t);
    sal_memcpy(cur_addr, src_mac, sizeof(shr_mac_t));
    cur_addr += sizeof(shr_mac_t);

    /* VLAN header */
    if (need_vlan == true) {
        *cur_addr++ = ENET_TPID >> 8;
        *cur_addr++ = ENET_TPID & 0xff;
        *cur_addr++ = vid >> 8;
        *cur_addr++ = vid & 0xff;
    }

    /* Ethertype */
    *cur_addr++ = ethertype >> 8;
    *cur_addr++ = ethertype & 0xff;

    /* Fill payload */
    if (pkt_len < (uint32_t)(cur_addr - buf->data)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    payload_len = pkt_len - (int)(cur_addr - buf->data);
    payload_content = pattern;
    while (payload_len >= 4) {
        *cur_addr++ = payload_content >> 24;
        *cur_addr++ = payload_content >> 16;
        *cur_addr++ = payload_content >> 8;
        *cur_addr++ = payload_content & 0xff;
        payload_content += pattern_inc;
        payload_len -= 4;
    }
    while (payload_len != 0) { /* when payload_len = 3~1 */
        *cur_addr++ = payload_content >> (payload_len * 8);
        payload_len--;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l3_packet_with_pri_fill(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          const shr_ip6_t src_ip_addr, const shr_ip6_t dst_ip_addr,
                          uint32_t vid, uint8_t ttl, uint8_t tos,
                          uint32_t pattern, uint32_t pattern_inc, bool random,
                          int seed, bool ipv6)
{
#define ENET_TPID        (0x8100)

    uint8_t *pkt_ptr;
    int offset = 0, i;
    uint32_t payload_len, ip_len;
    uint32_t payload_content, chk_sum, crc;
   uint8_t pri;
   uint8_t temp_pri;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);
    SHR_NULL_CHECK(src_mac, SHR_E_PARAM);
    SHR_NULL_CHECK(dst_mac, SHR_E_PARAM);

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_len);

    SHR_NULL_CHECK(buf->data, SHR_E_PARAM);
    pkt_ptr = buf->data;
   pri = sal_rand()&0x07; /*$random();*/
     pri = (pri << 1) + 1;

  if (seed != 0) {
        sal_srand(seed);
    }
    /* Ethernet header */
    /* Byte 0-5: MAC DA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = dst_mac[i];
    }

    /* Byte 6-11: MAC SA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = src_mac[i];
    }

    /* Byte 12-13: TPID */
    pkt_ptr[offset++] = ENET_TPID >> 8;
    pkt_ptr[offset++] = ENET_TPID & 0xff;

    /* Byte 14-15: VLAN */
    temp_pri =  ((pri & 0x0f) <<4);
    temp_pri = (temp_pri )| (vid >>8);
    pkt_ptr[offset++] = temp_pri;
    pkt_ptr[offset++] = vid & 0xff;
    if (ipv6) {
        /* Byte 16-17: IPv6 Ether II Type */
        pkt_ptr[offset++] = 0x86;
        pkt_ptr[offset++] = 0xdd;

        /* Byte 18-21: IPv6 Version/Traffic Class/Flow Label */
        pkt_ptr[offset++] = 0x60;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 22-23: IPv6 Payload Length */
        /*  Calculate Payload Length */
        if (pkt_len < (BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR + BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR +
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            payload_len = pkt_len - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR -
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        }
        pkt_ptr[offset++] = (payload_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = payload_len & 0x00ff;

        /* Byte 24: IPv6 Next Header (no next header) */
        pkt_ptr[offset++] = 0x3b;

        /* Byte 25: IPv6 Hop Limit */
        pkt_ptr[offset++] = ttl;

        /* Byte 26-41: IPv6 Link-Local SA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_ADDR; i++) {
            pkt_ptr[offset++] = src_ip_addr[i];
        }

        /* Byte 42-57: IPv6 Link-Local DA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_ADDR; i++) {
            pkt_ptr[offset++] = dst_ip_addr[i];
        }

    } else {
        /* Byte 16-17: IPv4 Ether II Type */
        pkt_ptr[offset++] = 0x08;
        pkt_ptr[offset++] = 0x00;

        /* Byte 18: IPv4 Version/Header Length */
        pkt_ptr[offset++] = 0x45;

        /* Byte 19: IPv4 TOS */
        pkt_ptr[offset++] = tos;

        /* Byte 20-21: IPv4 Total Length */
        ip_len = pkt_len - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        pkt_ptr[offset++] = (ip_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = ip_len & 0x00ff;

        /* Byte 22-23: IPv4 ID */
        pkt_ptr[offset++] = 0;
        pkt_ptr[offset++] = 0;

        /* Byte 24-25: IPv4 Fragment Flags/Offset */
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 26: IPv4 TTL */
        pkt_ptr[offset++] = ttl;

        /* Byte 27: IPv4 Protocol */
        pkt_ptr[offset++] = 0x00;

        /*  Byte 28-29: IPv4 Header Checksum */
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 30-33: IPv4 SA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR; i++) {
            pkt_ptr[offset++] = src_ip_addr[i];
        }

        /* Byte 34-37: IPv4 DA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR; i++) {
            pkt_ptr[offset++] = dst_ip_addr[i];
        }

        /*  Calculate IPv4 Header Checksum */
        calculate_hdr_chksum(pkt_ptr, BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR,
                             BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR, &chk_sum);
        /*  Byte 28-29: IPv4 Header Checksum */
        pkt_ptr[28] = (chk_sum >> 8) & 0xff;
        pkt_ptr[29] = chk_sum & 0xff;
        /*  Calculate Payload Length */
        if (pkt_len < (BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR + BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR +
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            payload_len = pkt_len - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR -
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        }




    }

    /* Fill payload */
    payload_content = pattern;
    if (random) {
        while (payload_len > 0) {
            /* coverity[dont_call : FALSE] */
            pkt_ptr[offset++] = sal_rand();
            payload_len--;
        }
    } else {
        while (payload_len >= 4) {
            pkt_ptr[offset++] = payload_content >> 24;
            pkt_ptr[offset++] = payload_content >> 16;
            pkt_ptr[offset++]= payload_content >> 8;
            pkt_ptr[offset++] = payload_content & 0xff;
            payload_content += pattern_inc;
            payload_len -= 4;
        }
        while (payload_len != 0) { /* when payload_len = 3~1 */
            pkt_ptr[offset++] = payload_content >> (payload_len * 8);
            payload_len--;
        }
    }
    generate_calculate_crc(pkt_ptr, pkt_len, &crc);
    for (i = BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l3_packet_fill(bcmpkt_data_buf_t *buf, uint32_t pkt_len,
                          const shr_mac_t src_mac, const shr_mac_t dst_mac,
                          const shr_ip6_t src_ip_addr, const shr_ip6_t dst_ip_addr,
                          uint32_t vid, uint8_t ttl, uint8_t tos,
                          uint32_t pattern, uint32_t pattern_inc, bool random,
                          int seed, bool ipv6)
{
#define ENET_TPID        (0x8100)

    uint8_t *pkt_ptr;
    int offset = 0, i;
    uint32_t payload_len, ip_len;
    uint32_t payload_content, chk_sum, crc;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);
    SHR_NULL_CHECK(src_mac, SHR_E_PARAM);
    SHR_NULL_CHECK(dst_mac, SHR_E_PARAM);

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_len);

    SHR_NULL_CHECK(buf->data, SHR_E_PARAM);
    pkt_ptr = buf->data;

    if (seed != 0) {
        sal_srand(seed);
    }
    /* Ethernet header */
    /* Byte 0-5: MAC DA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = dst_mac[i];
    }

    /* Byte 6-11: MAC SA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = src_mac[i];
    }

    /* Byte 12-13: TPID */
    pkt_ptr[offset++] = ENET_TPID >> 8;
    pkt_ptr[offset++] = ENET_TPID & 0xff;

    /* Byte 14-15: VLAN */
    pkt_ptr[offset++] = vid >> 8;
    pkt_ptr[offset++] = vid & 0xff;
    if (ipv6) {
        /* Byte 16-17: IPv6 Ether II Type */
        pkt_ptr[offset++] = 0x86;
        pkt_ptr[offset++] = 0xdd;

        /* Byte 18-21: IPv6 Version/Traffic Class/Flow Label */
        pkt_ptr[offset++] = 0x60;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 22-23: IPv6 Payload Length */
        /*  Calculate Payload Length */
        if (pkt_len < (BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR + BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR +
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            payload_len = pkt_len - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR -
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        }
        pkt_ptr[offset++] = (payload_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = payload_len & 0x00ff;

        /* Byte 24: IPv6 Next Header (no next header) */
        pkt_ptr[offset++] = 0x3b;

        /* Byte 25: IPv6 Hop Limit */
        pkt_ptr[offset++] = ttl;

        /* Byte 26-41: IPv6 Link-Local SA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_ADDR; i++) {
            pkt_ptr[offset++] = src_ip_addr[i];
        }

        /* Byte 42-57: IPv6 Link-Local DA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_ADDR; i++) {
            pkt_ptr[offset++] = dst_ip_addr[i];
        }

    } else {
        /* Byte 16-17: IPv4 Ether II Type */
        pkt_ptr[offset++] = 0x08;
        pkt_ptr[offset++] = 0x00;

        /* Byte 18: IPv4 Version/Header Length */
        pkt_ptr[offset++] = 0x45;

        /* Byte 19: IPv4 TOS */
        pkt_ptr[offset++] = tos;

        /* Byte 20-21: IPv4 Total Length */
        ip_len = pkt_len - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        pkt_ptr[offset++] = (ip_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = ip_len & 0x00ff;

        /* Byte 22-23: IPv4 ID */
        pkt_ptr[offset++] = 0;
        pkt_ptr[offset++] = 0;

        /* Byte 24-25: IPv4 Fragment Flags/Offset */
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 26: IPv4 TTL */
        pkt_ptr[offset++] = ttl;

        /* Byte 27: IPv4 Protocol */
        pkt_ptr[offset++] = 0x00;

        /*  Byte 28-29: IPv4 Header Checksum */
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 30-33: IPv4 SA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR; i++) {
            pkt_ptr[offset++] = src_ip_addr[i];
        }

        /* Byte 34-37: IPv4 DA */
        for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR; i++) {
            pkt_ptr[offset++] = dst_ip_addr[i];
        }

        /*  Calculate IPv4 Header Checksum */
        calculate_hdr_chksum(pkt_ptr, BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR,
                             BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR, &chk_sum);
        /*  Byte 28-29: IPv4 Header Checksum */
        pkt_ptr[28] = (chk_sum >> 8) & 0xff;
        pkt_ptr[29] = chk_sum & 0xff;
        /*  Calculate Payload Length */
        if (pkt_len < (BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR + BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR +
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            payload_len = pkt_len - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR -
                  BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        }

    }

    /* Fill payload */
    payload_content = pattern;
    if (random) {
        while (payload_len > 0) {
            /* coverity[dont_call : FALSE] */
            pkt_ptr[offset++] = sal_rand();
            payload_len--;
        }
    } else {
        while (payload_len >= 4) {
            pkt_ptr[offset++] = payload_content >> 24;
            pkt_ptr[offset++] = payload_content >> 16;
            pkt_ptr[offset++]= payload_content >> 8;
            pkt_ptr[offset++] = payload_content & 0xff;
            payload_content += pattern_inc;
            payload_len -= 4;
        }
        while (payload_len != 0) { /* when payload_len = 3~1 */
            pkt_ptr[offset++] = payload_content >> (payload_len * 8);
            payload_len--;
        }
    }
    generate_calculate_crc(pkt_ptr, pkt_len, &crc);
    for (i = BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
destroy_netif_cb(int unit, const bcmpkt_netif_t *netif, void *cb_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(netif, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_socket_destroy(unit, netif->id), SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT
        (bcmpkt_netif_destroy(unit, netif->id));

exit:
    SHR_FUNC_EXIT();
}

static int
destroy_filter_cb(int unit, const bcmpkt_filter_t *filter, void *cb_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(filter, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT
        (bcmpkt_filter_destroy(unit, filter->id));

exit:
    SHR_FUNC_EXIT();
}

static int
bcma_testutil_packet_dev_get(int unit, int* ret_pktdev_id,
                             bcmpkt_dev_drv_types_t* ret_pktdev_type)
{
    bcmpkt_dev_drv_types_t pktdev_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &pktdev_type));

    if (ret_pktdev_type != NULL) {
        *ret_pktdev_type = pktdev_type;
    }

    if (ret_pktdev_id != NULL) {
        *ret_pktdev_id = (pktdev_type == BCMPKT_DEV_DRV_T_KNET) ?
                           BCMPKT_BPOOL_SHARED_ID : unit;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_packet_dev_cleanup(int unit)
{
    int pktdev_id;
    bcmpkt_dev_drv_types_t pktdev_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_get(unit, &pktdev_id, &pktdev_type));

    if (pktdev_type == BCMPKT_DEV_DRV_T_KNET) {

        SHR_IF_ERR_EXIT
            (bcmpkt_filter_traverse(unit, destroy_filter_cb, NULL));

        SHR_IF_ERR_EXIT
            (bcmpkt_netif_traverse(unit, destroy_netif_cb, NULL));
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_cleanup(unit));

    if (pktdev_type == BCMPKT_DEV_DRV_T_UNET) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmpkt_bpool_destroy(pktdev_id), SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}


#define PKTDEV_RING_SIZE        (64)

static int
bcma_testutil_packet_dev_set_chan(int unit, int chan_id, int dir,
                                  int max_frame_size)
{
    bcmpkt_dma_chan_t chan;
    SHR_BITDCLNAME(queue_bmp, MAX_CPU_COS) = {0};
    int cmc_chan = 0, cmc_chan_num = 0;

    SHR_FUNC_ENTER(unit);

    chan.id = chan_id;
    chan.dir = dir;
    chan.ring_size = PKTDEV_RING_SIZE;
    chan.max_frame_size = max_frame_size;

    SHR_IF_ERR_EXIT
        (bcmpkt_dma_chan_set(unit, &chan));

    if (dir == BCMPKT_DMA_CH_DIR_RX) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_cmc_chan_get(unit, NULL, &cmc_chan_num));

        for (cmc_chan = 0; cmc_chan < cmc_chan_num; cmc_chan++) {
            if (cmc_chan != chan_id) {
                SHR_BITCLR_RANGE(queue_bmp, 0, MAX_CPU_COS);

                SHR_IF_ERR_EXIT
                    (bcma_bcmpkt_test_chan_qmap_set(unit, cmc_chan, queue_bmp,
                                                    MAX_CPU_COS));
            } else {
                SHR_BITSET_RANGE(queue_bmp, 0, MAX_CPU_COS);

                SHR_IF_ERR_EXIT
                    (bcma_bcmpkt_test_chan_qmap_set(unit, cmc_chan, queue_bmp,
                                                    MAX_CPU_COS));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/* The extra buffer count of bpool needed to be allocated:
 *   one more for KNET   : bcmlu_tpacket_create()
 *   ring count for UNET : bcmcnet_pdma_rx_queue_setup()
 */
#define PKTDEV_EXTRA_BPOOL_ALLOC_UNET  (PKTDEV_RING_SIZE)
#define PKTDEV_EXTRA_BPOOL_ALLOC_KNET  (1)

int
bcma_testutil_packet_dev_init(int unit, int tx_chan, int rx_chan,
                              int pkt_max_size, int pkt_count,
                              int *ret_netif_id, int* ret_pktdev_id)
{
    bcmpkt_dev_init_t dev_cfg = {
        .cgrp_bmp = 0x0,
        .cgrp_size = 0,
        .mac_addr = {0x00, 0xbc, 0x00, 0x00, 0x00, 0x00},
    };
    bcmpkt_dev_drv_types_t pktdev_type;
    int bpool_alloc_num;
    int netif_id, pktdev_id, cmc_num, cmc_chan_num, idx, buf_size;
    bcmpkt_netif_t *netif = NULL;
    bcmpkt_filter_t *filter = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_cmc_chan_get(unit, &cmc_num, &cmc_chan_num));

    dev_cfg.cgrp_size = cmc_chan_num;
    for (idx = 0; idx < cmc_num; idx++) {
        dev_cfg.cgrp_bmp |= (1 << idx);
    }

    /* Make sure packet device has been cleaned up before test */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_cleanup(unit));

    /* Create buffer pool */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_get(unit, &pktdev_id, &pktdev_type));

    if (pktdev_type == BCMPKT_DEV_DRV_T_UNET) {
        bpool_alloc_num = pkt_count + PKTDEV_EXTRA_BPOOL_ALLOC_UNET;

        buf_size = pkt_max_size;
        if (buf_size < BCMPKT_BPOOL_BSIZE_MIN) {
            buf_size = BCMPKT_BPOOL_BSIZE_MIN;
        }

        SHR_IF_ERR_EXIT
            (bcmpkt_bpool_create(pktdev_id, buf_size, bpool_alloc_num));
    }

    /* Create packet device */
    sal_sprintf(dev_cfg.name, "bcm%d", unit);
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_init(unit, &dev_cfg));

    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_set_chan
            (unit, tx_chan, BCMPKT_DMA_CH_DIR_TX, pkt_max_size));

    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_set_chan
            (unit, rx_chan, BCMPKT_DMA_CH_DIR_RX, pkt_max_size));

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_enable(unit));

    /* Extra init sequence for KNET */
    if (pktdev_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_ALLOC(netif, sizeof(bcmpkt_netif_t), "bcmaTestUtilNetif");
        SHR_NULL_CHECK(netif, SHR_E_MEMORY);
        sal_memset(netif, 0, sizeof(bcmpkt_netif_t));

        SHR_ALLOC(filter, sizeof(bcmpkt_filter_t), "bcmaTestUtilFilter");
        SHR_NULL_CHECK(filter, SHR_E_MEMORY);
        sal_memset(filter, 0, sizeof(bcmpkt_filter_t));

        netif->mac_addr[1] = 0x01;
        netif->max_frame_size = pkt_max_size;
        netif->flags = BCMPKT_NETIF_F_RCPU_ENCAP;

        filter->type = BCMPKT_FILTER_T_RX_PKT;
        filter->priority = 255;
        filter->dma_chan = 1;
        filter->dest_type = BCMPKT_DEST_T_NETIF;
        filter->dest_id = 1;
        filter->match_flags = 0;

        SHR_IF_ERR_EXIT
            (bcmpkt_netif_create(unit, netif));

        SHR_IF_ERR_EXIT
            (bcmpkt_socket_create(unit, netif->id, NULL));

        filter->dma_chan = rx_chan;
        filter->dest_id = netif->id;
        SHR_IF_ERR_EXIT
            (bcmpkt_filter_create(unit, filter));

        netif_id = netif->id;
    } else if (pktdev_type == BCMPKT_DEV_DRV_T_UNET) {
        /* For UNET, netif_id "n" should be the n-th Tx DMA channel.
         * During testing, we create only one Tx channel.
         * So we return netif_id 1 here whatever the channel id is
         */
        netif_id = 1;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (ret_netif_id != NULL) {
        *ret_netif_id = netif_id;
    }

    if (ret_pktdev_id != NULL) {
        *ret_pktdev_id = pktdev_id;
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcma_testutil_packet_dev_cleanup(unit);
    }
    SHR_FREE(netif);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

int
bcma_testutil_mpls_packet_fill(
    int unit,
    bcmpkt_data_buf_t *buf,
    bcma_testutil_pkt_attr_t pkt_attr)
{
    uint8_t *pkt_ptr;
    int b, i;
    uint32_t ip_len, pld_len, chk_sum, indx;
    uint32_t crc;
    int mpls_labels_bytes;
    int offset = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    mpls_labels_bytes = 0;

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_attr.size - 4);

    SHR_NULL_CHECK(buf->data, SHR_E_PARAM);

    if (pkt_attr.seed != 0) {
        sal_srand(pkt_attr.seed);
    }

    pkt_ptr = buf->data;

    /* Byte 0-5: MAC DA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = pkt_attr.mac_da[i];
    }

    /* Byte 6-11: MAC SA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = pkt_attr.mac_sa[i];
    }

    /* Byte 12-13: TPID */
    pkt_ptr[offset++] = 0x81;
    pkt_ptr[offset++] = 0x00;

    /* Byte 14-15: VLAN */
    pkt_ptr[offset++] = (pkt_attr.vlan_id >> 8) & 0x00ff;
    pkt_ptr[offset++] = pkt_attr.vlan_id & 0x00ff;

    if (pkt_attr.l3_mpls_en == 1) {
        /* Byte 16-17: MPLS IPv6 Ether II Type */
        pkt_ptr[offset++] = 0x88; /* MPLS unicast */
        pkt_ptr[offset++] = 0x47;

        /* MPLS 3 labels */
        for (i = 0; i < 3; i++ ) {
            for (b = 24; b >=0; b -= 8) {
                pkt_ptr[offset++] = (pkt_attr.mpls_label[i] >> b) & 0xff;
            }
        }
        mpls_labels_bytes = (3 * 4);
    } else if (pkt_attr.l3_en == 1) {
        if (pkt_attr.ipv6_en == 1) {
            /* Byte 16-17: IPv6 Ether II Type */
            pkt_ptr[offset++] = 0x86;
            pkt_ptr[offset++] = 0xdd;
        } else {
            /* Byte 16-17: IPv4 Ether II Type */
            pkt_ptr[offset++] = 0x08;
            pkt_ptr[offset++] = 0x00;
        }
        mpls_labels_bytes = 0;
    }

    if (pkt_attr.l3_en == 1) {
    if (pkt_attr.ipv6_en == 1) {
        /* Byte 18-21: IPv6 Version/Traffic Class/Flow Label */
        pkt_ptr[offset++] = 0x60;
        pkt_ptr[offset++] = 0x0; /* 0x30 | (sal_rand() & 0x000f);*/
        pkt_ptr[offset++] = 0x0; /* sal_rand(); */
        pkt_ptr[offset++] = 0x0; /* sal_rand(); */

        /* Byte 22-23: IPv6 Payload Length */
        pld_len = pkt_attr.size - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR - mpls_labels_bytes
                  - BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        pkt_ptr[offset++] = (pld_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = pld_len & 0x00ff;

        /* Byte 24: IPv6 Next Header (no next header) */
        pkt_ptr[offset++] = 0x3b;

        /* Byte 25: IPv6 Hop Limit */
        pkt_ptr[offset++] = pkt_attr.ttl;

        /* Byte 26-41: IPv6 Link-Local SA */
        for (i = 3; i >= 0; i-- ) {
            for (b = 24; b >=0; b -= 8) {
                pkt_ptr[offset++] = (pkt_attr.ip_sa[i] >> b) & 0xff;
            }
        }

        /* Byte 42-57: IPv6 Link-Local DA */
        for (i = 3; i >= 0; i-- ) {
            for (b = 24; b >=0; b -= 8) {
                pkt_ptr[offset++] = (pkt_attr.ip_da[i] >> b) & 0xff;
            }
        }
    } else {
        /* Byte 18: IPv4 Version/Header Length */
        pkt_ptr[offset++] = 0x45;

        /* Byte 19: IPv4 TOS */
        pkt_ptr[offset++] = pkt_attr.tos;

        /* Byte 20-21: IPv4 Total Length */
        ip_len = pkt_attr.size - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR
                 - mpls_labels_bytes - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
        pkt_ptr[offset++] = (ip_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = ip_len & 0x00ff;

        /* Byte 22-23: IPv4 ID */
        pkt_ptr[offset++] = sal_rand();
        pkt_ptr[offset++] = sal_rand();

        /* Byte 24-25: IPv4 Fragment Flags/Offset */
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 26: IPv4 TTL */
        pkt_ptr[offset++] = pkt_attr.ttl;

        /* Byte 27: IPv4 Protocol */
        pkt_ptr[offset++] = 0x00;

        /*  Byte 28-29: IPv4 Header Checksum */
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;

        /* Byte 30-33: IPv4 SA */
        for (i = BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (pkt_attr.ip_sa[0] >> (i * 8)) & 0x000000ff;
        }

        /* Byte 34-37: IPv4 DA */
        for (i = BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++]  = (pkt_attr.ip_da[0] >> (i * 8)) & 0x000000ff;
        }

        /*  Calculate IPv4 Header Checksum */
        calculate_hdr_chksum(buf->data,
                             BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR + mpls_labels_bytes,
                             BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR,
                             &chk_sum);
        /*pkt_ptr[28] = (chk_sum >> 8) & 0x00ff;
        pkt_ptr[29] = chk_sum & 0x00ff;*/
        pkt_ptr[offset - 10] = (chk_sum >> 8) & 0x00ff;
        pkt_ptr[offset -  9] = chk_sum & 0x00ff;

        /*  Calculate Payload Length */
        pld_len = pkt_attr.size - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR
                  - BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV4_HDR
                  - mpls_labels_bytes - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;
    }
    } else { /* L2 vlan tagged */
        pld_len = pkt_attr.size - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR;
    }


    /* Random Payload */
    for (indx = 0; indx < pld_len; indx++) {
        /* coverity[dont_call : FALSE] */
        pkt_ptr[offset++] = sal_rand();
    }

    /*  Calculate CRC */
    generate_calculate_crc(pkt_ptr, pkt_attr.size, &crc);
    for (i = BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }


exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_vxlan_packet_fill(
    int unit,
    bcmpkt_data_buf_t *buf,
    bcma_testutil_pkt_attr_t pkt_attr)
{
    uint8_t *pkt_ptr;
    int b, i;
    uint32_t pld_len, indx;
    uint32_t crc;
    int offset = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_attr.size - 4);

    SHR_NULL_CHECK(buf->data, SHR_E_PARAM);

    if (pkt_attr.seed != 0) {
        sal_srand(pkt_attr.seed);
    }

    pkt_ptr = buf->data;

    /* ----   Under Layer    ----*/
    /* Byte 0-5: MAC DA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = pkt_attr.ul_mac_da[i];
    }

    /* Byte 6-11: MAC SA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = pkt_attr.ul_mac_sa[i];
    }

    /* Byte 12-13: TPID */
    pkt_ptr[offset++] = 0x81;
    pkt_ptr[offset++] = 0x00;

    /* Byte 14-15: VLAN */
    pkt_ptr[offset++] = (pkt_attr.ul_vlan_id >> 8) & 0x00ff;
    pkt_ptr[offset++] = pkt_attr.ul_vlan_id & 0x00ff;

    /* Byte 16-17: Eth type */
    pkt_ptr[offset++] = 0x86;
    pkt_ptr[offset++] = 0xdd;

    /* Byte 18-21: IPv6 Version/Traffic Class/Flow Label */
    pkt_ptr[offset++] = 0x60;
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;

    pld_len = pkt_attr.size - BCMA_TESTUTIL_PACKET_NUM_BYTES_L2_HDR -
              BCMA_TESTUTIL_PACKET_NUM_BYTES_IPV6_HDR - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;

    /* Byte 22-23: PayloadLength */
    pkt_ptr[offset++] = (pld_len >> 8) & 0x00ff;
    pkt_ptr[offset++] = pld_len & 0x00ff;

    /* Byte 24: IPv6 Next Header : UDP */
    pkt_ptr[offset++] = 0x11;
    /* Byte 25: HopLimit */
    pkt_ptr[offset++] = pkt_attr.ttl;

    /* Byte 26-41: IPv6 Link-Local SA */
    for (i = 3; i >= 0; i-- ) {
        for (b = 24; b >=0; b -= 8) {
            pkt_ptr[offset++] = (pkt_attr.ul_ip_sa[i] >> b) & 0xff;
        }
    }
    /* Byte 42-57: IPv6 Link-Local DA */
    for (i = 3; i >= 0; i-- ) {
        for (b = 24; b >=0; b -= 8) {
            pkt_ptr[offset++] = (pkt_attr.ul_ip_da[i] >> b) & 0xff;
        }
    }

    /* ----   UDP Header    ----*/
    /* Byte 58-59: UDP src_port */
    pkt_ptr[offset++] = 0x15; 
    pkt_ptr[offset++] = 0xb3;
    /* Byte 60-61: UDP dst_port - fixed*/
    pkt_ptr[offset++] = 0x12;
    pkt_ptr[offset++] = 0xb5;
    /* Byte 62-63: UDP Length */
    pkt_ptr[offset++] = (pld_len >> 8) & 0x00ff;
    pkt_ptr[offset++] = pld_len & 0x00ff;
    /* Byte 64-65: UDP checksum = 0 */
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;

    /* ----   VXLAN Header    ----*/
    /* Byte 66-66: Flags */
    pkt_ptr[offset++] = 0xaa;
    /* Byte 67-69: Reserved = 0 */
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;

    /* Byte 70-72: VNID */
    pkt_ptr[offset++] = (pkt_attr.vxlan_vnid >> 16) & 0xff;
    pkt_ptr[offset++] = (pkt_attr.vxlan_vnid >> 8) & 0xff;
    pkt_ptr[offset++] = (pkt_attr.vxlan_vnid >> 0) & 0xff;

    /* Byte 73-73: Reserved = 0 */
    pkt_ptr[offset++] = 0x0;


    /* ----   Over Layer    ----*/
    /* Byte 74-79: MAC DA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = pkt_attr.mac_da[i];
    }
    /* Byte 80-85: MAC SA */
    for (i = 0; i < BCMA_TESTUTIL_PACKET_NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = pkt_attr.mac_sa[i];
    }

    /* Byte 86-87: TPID */
    pkt_ptr[offset++] = 0x81;
    pkt_ptr[offset++] = 0x00;

    /* Byte 88-89: VLAN */
    pkt_ptr[offset++] = (pkt_attr.vlan_id >> 8) & 0x00ff;
    pkt_ptr[offset++] = pkt_attr.vlan_id & 0x00ff;

    /* Byte 90-91: Eth type */
    pkt_ptr[offset++] = 0x86;
    pkt_ptr[offset++] = 0xdd;

    /* Byte 92-95: IPv6 Version/Traffic Class/Flow Label */
    pkt_ptr[offset++] = 0x60;
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;
    pkt_ptr[offset++] = 0x0;

    pld_len = pkt_attr.size - 132 - BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC;

    /* Byte 96-97: PayloadLength */
    pkt_ptr[offset++] = (pld_len >> 8) & 0x00ff;
    pkt_ptr[offset++] = pld_len & 0x00ff;

    /* Byte 98: IPv6 Next Header : No Next Header */
    pkt_ptr[offset++] = 0x3b;
    /* Byte 99: HopLimit */
    pkt_ptr[offset++] = pkt_attr.ttl;

    /* Byte 100-115: IPv6 Link-Local SA */
    for (i = 3; i >= 0; i-- ) {
        for (b = 24; b >=0; b -= 8) {
            pkt_ptr[offset++] = (pkt_attr.ip_sa[i] >> b) & 0xff;
        }
    }
    /* Byte 116-131: IPv6 Link-Local DA */
    for (i = 3; i >= 0; i-- ) {
        for (b = 24; b >=0; b -= 8) {
            pkt_ptr[offset++] = (pkt_attr.ip_da[i] >> b) & 0xff;
        }
    }

    /* Random Payload */
    for (indx = 0; indx < pld_len; indx++) {
        /* coverity[dont_call : FALSE] */
        pkt_ptr[offset++] = sal_rand();
    }

    /*  Calculate CRC */
    generate_calculate_crc(pkt_ptr, pkt_attr.size, &crc);
    for (i = BCMA_TESTUTIL_PACKET_NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }

exit:
    SHR_FUNC_EXIT();
}

