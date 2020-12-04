/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packet generator for diagnostic (TR) tests.
 * This packet generator can be used to generate random L2 and L3 Ethernet
 * packets complete with CRC. Created in addition to existing packet generator
 8 for added flexibility.
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <shared/bsl.h>
#include "gen_pkt.h"

#define NUM_ENTRIES_CRC_TABLE 256

uint32 tgp_polynomial = 0x04c11db7;
uint32 tgp_initial_remainder = 0xffffffff;
uint8  tgp_refin = 0x1;
uint8  tgp_refout = 0x1;
uint32 tgp_final_xor_value = 0xffffffff;
uint32 tgp_global_seed_ctr = 0;
uint32 tgp_crc_table[NUM_ENTRIES_CRC_TABLE];

/*
 * Function:
 *      tgp_reflect
 * Purpose:
 *      Reflect n-bit data
 * Parameters:
 *      data - Input data (max 32 bits)
 *      nbits - Number of bits reflected
 * Returns:
 *     Reflected data
 */

uint32
tgp_reflect(uint32 data, uint32 nbits)
{
    uint32 reflection, i;

    reflection = 0;
    for (i = 0; i < nbits; i++) {
        if (data & 0x00000001) {
            reflection |= (1 << ((nbits - 1) - i));
        }
        data = data >> 1;
    }
    return (reflection);
}

/*
 * Function:
 *      tgp_populate_crc_table
 * Purpose:
 *      Create CRC table for Ethernet packet CRC calculation. Populates
 *      tgp_crc_table.
 * Parameters:
 *      None
 * Returns:
 *     Nothing
 */

void
tgp_populate_crc_table(void)
{
    uint32 remainder, topbit;
    uint32 dividend;
    uint32 i;

    topbit = (1 << (32 - 1));

    for (dividend = 0; dividend < NUM_ENTRIES_CRC_TABLE; dividend++) {
        remainder = dividend << (32 - 8);

        for (i = 8; i > 0; i--) {
            if (remainder & topbit) {
                remainder = (remainder << 1) ^ tgp_polynomial;
            } else {
                remainder = (remainder << 1);
            }
        }
        tgp_crc_table[dividend] = remainder;
    }

}

/*
 * Function:
 *      tgp_generate_calculate_crc
 * Purpose:
 *      Calculate 4 byte CRC for Ethernet packet
 * Parameters:
 *      pkt_ptr - Pointer to packet
 *      pkt_size - Packet Size in bytes
 * Returns:
 *     4 byte CRC.
 */

uint32
tgp_generate_calculate_crc(uint8 *pkt_ptr, int32 pkt_size)
{
    uint32 i, remainder, reflected_remainder, crc;
    uint8 data;

    remainder = tgp_initial_remainder;


    for (i = 0; i < (pkt_size - NUM_BYTES_CRC); i++) {
        data =
            (tgp_refin ==
             0x1) ? (tgp_reflect(pkt_ptr[i],
                             8) ^ (remainder >> (32 -
                                                 8))) : (pkt_ptr[i] ^ (remainder
                                                                       >> (32 -
                                                                           8)));
        remainder = tgp_crc_table[data] ^ (remainder << 8);
    }
    reflected_remainder = tgp_reflect(remainder, 32);


    crc =
        (tgp_refout ==
         0x1) ? (reflected_remainder ^ tgp_final_xor_value) : (remainder ^
                                                           tgp_final_xor_value);

    return (crc);
}

/*
 * Function:
 *      tgp_gen_random_l2_pkt
 * Purpose:
 *      Generate random L2 Ethernet packet
 * Parameters:
 *      pkt_ptr - Pointer to packet
 *      pkt_size - Packet size in bytes
 *      mac_da - MAC DA in packet header
 *      mac_sa - MAC SA in packet header
 *      tpid - TPID
 *      vlan_id - {priority, cfi, VLAN}
 * Returns:
 *     Reflected data
 */

void
tgp_gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size,
                  uint8 mac_da[NUM_BYTES_MAC_ADDR],
                  uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 tpid, uint16 vlan_id)
{
    int i;
    uint32 crc;

    for (i = 0; i < NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[i] = mac_da[i];
    }

    for (i = NUM_BYTES_MAC_ADDR; i < (2 * NUM_BYTES_MAC_ADDR); i++) {
        pkt_ptr[i] = mac_sa[i - NUM_BYTES_MAC_ADDR];
    }

    pkt_ptr[2 * NUM_BYTES_MAC_ADDR] = (tpid >> 8) & 0x00ff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 1] = tpid & 0x00ff;

    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 2] = (vlan_id >> 8) & 0x00ff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 3] = vlan_id & 0x00ff;

    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 4] = 0xff;
    pkt_ptr[(2 * NUM_BYTES_MAC_ADDR) + 5] = 0xff;

    for (i = ((2 * NUM_BYTES_MAC_ADDR) + 6); i < (pkt_size - NUM_BYTES_CRC);
         i++) {
        /* coverity[dont_call : FALSE] */
        pkt_ptr[i] = sal_rand();

    }

    for (i = 0; i < NUM_BYTES_CRC; i++) {
        pkt_ptr[pkt_size - NUM_BYTES_CRC + i] = 0x00;
    }

    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    pkt_ptr[pkt_size - NUM_BYTES_CRC + 3] = (crc >> 24) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 2] = (crc >> 16) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC + 1] = (crc >> 8) & 0xff;
    pkt_ptr[pkt_size - NUM_BYTES_CRC] = (crc) & 0xff;
    tgp_global_seed_ctr++;

}

/*
 * Function:
 *      tgp_generate_calculate_hdr_chksum
 * Purpose:
 *      Calculate 2 byte checksum for IP header
 * Parameters:
 *      pkt_ptr - Pointer to packet
 *      hdr_offset - Header Offset
 *      hdr_len - Header Length in bytes
 * Returns:
 *      2 byte header checksum
 */

uint16
tgp_generate_calculate_hdr_chksum(uint8 *pkt_ptr, int32 hdr_offset,
                                  int32 hdr_len)
{
    uint32 i, sum = 0;
    uint16 chksum;

    for (i = hdr_offset; i < hdr_offset + hdr_len; i+=2) {
        sum += (pkt_ptr[i] << 8) + pkt_ptr[i+1];
    }

    chksum = ~((sum >> 16 & 0x0000ffff) + (sum & 0x0000ffff));

    return (chksum);
}

/*
 * Function:
 *      tgp_gen_random_ip_pkt
 * Purpose:
 *      Generate random IPv4/6 Ethernet packet
 * Parameters:
 *      pkt_ptr  - Pointer to packet
 *      pkt_size - Packet size in bytes
 *      mac_da   - MAC DA in packet header
 *      mac_sa   - MAC SA in packet header
 *      vlan_id  - {priority, cfi, VLAN}
 *      ip_da    - IP DA in IPv4/6 header
 *      ip_sa    - IP SA in IPv4/6 header
 *      ttl      - TTL in IPv4 header/hop limit in IPv6 header
 *      tos      - TOS in IPv4 header
 * Returns:
 *     Reflected data
 */

void
tgp_gen_random_ip_pkt(uint8 ipv6, uint8 *pkt_ptr, uint32 pkt_size,
                      uint8 mac_da[NUM_BYTES_MAC_ADDR],
                      uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 vlan_id,
                      uint32 ip_da, uint32 ip_sa, uint8 ttl, uint8 tos,
                      uint8 sv_tag_en, uint32 sv_tag)
{
    int i, offset = 0;
    uint16 ip_len, pld_len, chk_sum;
    uint32 crc;

    /* Byte 0-5: MAC DA */
    for (i = 0; i < NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = mac_da[i];
    }

    /* Byte 6-11: MAC SA */
    for (i = 0; i < NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = mac_sa[i];
    }

    if (sv_tag_en) {
       for (i = NUM_BYTES_SV_TAG - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (sv_tag >> (i * 8)) & 0x000000ff;
        }
    }

    /* Byte 12-13: TPID */
    pkt_ptr[offset++] = 0x81;
    pkt_ptr[offset++] = 0x00;

    /* Byte 14-15: VLAN */
    pkt_ptr[offset++] = (vlan_id >> 8) & 0x00ff;
    pkt_ptr[offset++] = vlan_id & 0x00ff;

    if (ipv6) {
        /* Byte 16-17: IPv6 Ether II Type */
        pkt_ptr[offset++] = 0x86;
        pkt_ptr[offset++] = 0xdd;

        /* Byte 18-21: IPv6 Version/Traffic Class/Flow Label */
        pkt_ptr[offset++] = 0x60;
        pkt_ptr[offset++] = 0x30 | (sal_rand() & 0x000f);
        pkt_ptr[offset++] = sal_rand();
        pkt_ptr[offset++] = sal_rand();

        /* Byte 22-23: IPv6 Payload Length */
        pld_len = pkt_size - NUM_BYTES_L2_HDR - NUM_BYTES_IPV6_HDR -
                  NUM_BYTES_CRC;
        pkt_ptr[offset++] = (pld_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = pld_len & 0x00ff;

        /* Byte 24: IPv6 Next Header (no next header) */
        pkt_ptr[offset++] = 0x3b;

        /* Byte 25: IPv6 Hop Limit */
        pkt_ptr[offset++] = ttl;

        /* Byte 26-41: IPv6 Link-Local SA */
        pkt_ptr[offset++] = 0xfe;
        pkt_ptr[offset++] = 0x80;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        for (i = NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (ip_sa >> (i * 8)) & 0x000000ff;
        }

        /* Byte 42-57: IPv6 Link-Local DA */
        pkt_ptr[offset++] = 0xfe;
        pkt_ptr[offset++] = 0x80;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        pkt_ptr[offset++] = 0x00;
        for (i = NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (ip_da >> (i * 8)) & 0x000000ff;
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
        ip_len = pkt_size - NUM_BYTES_L2_HDR - NUM_BYTES_CRC;
        if (sv_tag_en) ip_len -= NUM_BYTES_SV_TAG;

        pkt_ptr[offset++] = (ip_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = ip_len & 0x00ff;

        /* Byte 22-23: IPv4 ID */
        pkt_ptr[offset++] = sal_rand();
        pkt_ptr[offset++] = sal_rand();

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
        for (i = NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (ip_sa >> (i * 8)) & 0x000000ff;
        }

        /* Byte 34-37: IPv4 DA */
        for (i = NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (ip_da >> (i * 8)) & 0x000000ff;
        }

        if (sv_tag_en) {
            /*  Calculate IPv4 Header Checksum */
            chk_sum = tgp_generate_calculate_hdr_chksum(pkt_ptr, NUM_BYTES_L2_HDR + NUM_BYTES_SV_TAG,
                                                    NUM_BYTES_IPV4_HDR);

            pkt_ptr[28+4] = (chk_sum >> 8) & 0x00ff;
            pkt_ptr[29+4] = chk_sum & 0x00ff;
            /*  Calculate Payload Length */
            pld_len = pkt_size - NUM_BYTES_L2_HDR - NUM_BYTES_IPV4_HDR -
                  NUM_BYTES_CRC - NUM_BYTES_SV_TAG;
        } else {
            /*  Calculate IPv4 Header Checksum */
            chk_sum = tgp_generate_calculate_hdr_chksum(pkt_ptr, NUM_BYTES_L2_HDR,
                                                    NUM_BYTES_IPV4_HDR);

            pkt_ptr[28] = (chk_sum >> 8) & 0x00ff;
            pkt_ptr[29] = chk_sum & 0x00ff;
            /*  Calculate Payload Length */
            pld_len = pkt_size - NUM_BYTES_L2_HDR - NUM_BYTES_IPV4_HDR -
                  NUM_BYTES_CRC;
        }
    }

    /* Random Payload */
    for (i = 0; i < pld_len; i++) {
        /* coverity[dont_call : FALSE] */
        pkt_ptr[offset++] = sal_rand();
    }

    /*  Calculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);
    for (i = NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }

    tgp_global_seed_ctr++;
}


/*
 * Function:
 *      tgp_gen_random_mpls_ip_pkt
 * Purpose:
 *      Generate random MPLS, IPv4/6, Ethernet packet
 * Parameters:
 *      pkt_ptr  - Pointer to packet
 *      pkt_size - Packet size in bytes
 *      mac_da   - MAC DA in packet header
 *      mac_sa   - MAC SA in packet header
 *      vlan_id  - {priority, cfi, VLAN}
 *      l3_en    - 1 if L3 enabled
 *      l3_mpls_en - 1 if MPLS enabled
 *      mpls_labels - an uint32 array of MPLs labels
 *      ipv6_en  - 1 if ipv6 enabled
 *      ip_da    - IP DA in IPv4/6 header
 *      ip_sa    - IP SA in IPv4/6 header
 *      ttl      - TTL in IPv4 header/hop limit in IPv6 header
 *      tos      - TOS in IPv4 header
 * Returns:
 *     Reflected data
 */
void
tgp_gen_random_mpls_ip_pkt(
    uint8 *pkt_ptr,
    uint32 pkt_size,
    uint8 mac_da[NUM_BYTES_MAC_ADDR],
    uint8 mac_sa[NUM_BYTES_MAC_ADDR],
    uint16 vlan_id,
    int l3_en,
    int l3_mpls_en,
    uint32 *mpls_labels,
    int ipv6_en,
    uint32 *ipv6_da,
    uint32 *ipv6_sa,
    uint8 ttl,
    uint8 tos)
{
    int i, b, offset = 0;
    uint16 ip_len, pld_len, chk_sum;
    uint32 crc;
    int mpls_labels_bytes;

    mpls_labels_bytes = 0;

    /* Byte 0-5: MAC DA */
    for (i = 0; i < NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = mac_da[i];
    }

    /* Byte 6-11: MAC SA */
    for (i = 0; i < NUM_BYTES_MAC_ADDR; i++) {
        pkt_ptr[offset++] = mac_sa[i];
    }

    /* Byte 12-13: TPID */
    pkt_ptr[offset++] = 0x81;
    pkt_ptr[offset++] = 0x00;

    /* Byte 14-15: VLAN */
    pkt_ptr[offset++] = (vlan_id >> 8) & 0x00ff;
    pkt_ptr[offset++] = vlan_id & 0x00ff;

    if (l3_mpls_en == 1) {
        /* Byte 16-17: MPLS IPv6 Ether II Type */
        pkt_ptr[offset++] = 0x88; /* MPLS unicast */
        pkt_ptr[offset++] = 0x47;

        /* MPLS 3 labels */
        for (i = 0; i < 3; i++ ) {
            for (b = 24; b >=0; b -= 8) {
                pkt_ptr[offset++] = (mpls_labels[i] >> b) & 0xff;
            }
        }
        mpls_labels_bytes = (3 * NUM_BYTES_MPLS_HDR);
    } else if (l3_en == 1) {
        if (ipv6_en == 1) {
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

    if (l3_en == 1) {
    if (ipv6_en == 1) {
        /* Byte 18-21: IPv6 Version/Traffic Class/Flow Label */
        pkt_ptr[offset++] = 0x60;
        pkt_ptr[offset++] = 0x0; /* 0x30 | (sal_rand() & 0x000f);*/
        pkt_ptr[offset++] = 0x0; /* sal_rand(); */
        pkt_ptr[offset++] = 0x0; /* sal_rand(); */

        /* Byte 22-23: IPv6 Payload Length */
        pld_len = pkt_size - NUM_BYTES_L2_HDR - mpls_labels_bytes
                  - NUM_BYTES_IPV6_HDR - NUM_BYTES_CRC ;
        pkt_ptr[offset++] = (pld_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = pld_len & 0x00ff;

        /* Byte 24: IPv6 Next Header (no next header) */
        pkt_ptr[offset++] = 0x3b;

        /* Byte 25: IPv6 Hop Limit */
        pkt_ptr[offset++] = ttl;

        /* Byte 26-41: IPv6 Link-Local SA */
        for (i = 3; i >= 0; i-- ) {
            for (b = 24; b >=0; b -= 8) {
                pkt_ptr[offset++] = (ipv6_sa[i] >> b) & 0xff;
            }
        }

        /* Byte 42-57: IPv6 Link-Local DA */
        for (i = 3; i >= 0; i-- ) {
            for (b = 24; b >=0; b -= 8) {
                pkt_ptr[offset++] = (ipv6_da[i] >> b) & 0xff;
            }
        }
    } else {
        /* Byte 18: IPv4 Version/Header Length */
        pkt_ptr[offset++] = 0x45;

        /* Byte 19: IPv4 TOS */
        pkt_ptr[offset++] = tos;

        /* Byte 20-21: IPv4 Total Length */
        ip_len = pkt_size - NUM_BYTES_L2_HDR - mpls_labels_bytes - NUM_BYTES_CRC;
        pkt_ptr[offset++] = (ip_len >> 8) & 0x00ff;
        pkt_ptr[offset++] = ip_len & 0x00ff;

        /* Byte 22-23: IPv4 ID */
        pkt_ptr[offset++] = sal_rand();
        pkt_ptr[offset++] = sal_rand();

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
        for (i = NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (ipv6_sa[0] >> (i * 8)) & 0x000000ff;
        }

        /* Byte 34-37: IPv4 DA */
        for (i = NUM_BYTES_IPV4_ADDR - 1; i >= 0; i--) {
            pkt_ptr[offset++] = (ipv6_da[0] >> (i * 8)) & 0x000000ff;
        }

        /*  Calculate IPv4 Header Checksum */
        chk_sum = tgp_generate_calculate_hdr_chksum(pkt_ptr,
                                                    NUM_BYTES_L2_HDR + mpls_labels_bytes,
                                                    NUM_BYTES_IPV4_HDR);
        /*pkt_ptr[28] = (chk_sum >> 8) & 0x00ff;
        pkt_ptr[29] = chk_sum & 0x00ff;*/
        pkt_ptr[offset - 10] = (chk_sum >> 8) & 0x00ff;
        pkt_ptr[offset -  9] = chk_sum & 0x00ff;

        /*  Calculate Payload Length */
        pld_len = pkt_size - NUM_BYTES_L2_HDR - NUM_BYTES_IPV4_HDR -
                  mpls_labels_bytes - NUM_BYTES_CRC;
    }
    } else { /* L2 vlan tagged */
        pld_len = pkt_size - NUM_BYTES_L2_HDR;
    }


    /* Random Payload */
    for (i = 0; i < pld_len; i++) {
        /* coverity[dont_call : FALSE] */
        pkt_ptr[offset++] = sal_rand();
    }

    /*  Calculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);
    for (i = NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }

    tgp_global_seed_ctr++;
}



/*
 * Function:
 *      tgp_print_pkt
 * Purpose:
 *      Print packet
 * Parameters:
 *      pkt_ptr  - Pointer to packet
 *      pkt_size - Packet size in bytes
 *
 * Returns:
 *     Nothing
 */

void
tgp_print_pkt(uint8 *pkt_ptr, uint32 pkt_size)
{
    int i;

    for (i = 0; i < pkt_size; i++) {
        if (i % 16 == 0) {
           cli_out("\n");
        }
        cli_out("%02x ", pkt_ptr[i]);
    }
    cli_out("\n");
}
