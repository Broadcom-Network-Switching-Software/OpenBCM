/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Extern declarations for test functions in gen_pkt.c
 */

#define NUM_BYTES_MAC_ADDR 6
#define NUM_BYTES_IPV4_ADDR 4
#define NUM_BYTES_L2_HDR 18
#define NUM_BYTES_SV_TAG 4
#define NUM_BYTES_IPV4_HDR 20
#define NUM_BYTES_IPV6_HDR 40
#define NUM_BYTES_SEQ_ID 6
#define NUM_BYTES_CRC 4
#define MIN_IPV6_PKT_SIZE NUM_BYTES_L2_HDR + NUM_BYTES_IPV6_HDR + \
                          NUM_BYTES_SEQ_ID + NUM_BYTES_CRC
#define NUM_BYTES_MPLS_HDR 4

extern uint32 tgp_reflect(uint32, uint32);
extern void tgp_populate_crc_table(void);
extern unsigned int tgp_generate_calculate_crc(uint8*, int32);
extern void tgp_gen_random_l2_pkt(uint8*, uint32, uint8[], uint8[],
                                  uint16, uint16);
extern void tgp_gen_random_ip_pkt(uint8, uint8*, uint32, uint8[], uint8[],
                                  uint16, uint32, uint32, uint8, uint8,
                                  uint8, uint32);
extern void tgp_print_pkt(uint8 *pkt_ptr, uint32 pkt_size);

extern void
tgp_gen_random_mpls_ip_pkt(
    uint8*,
    uint32,
    uint8[],
    uint8[],
    uint16,
    int,
    int,
    uint32*,
    int,
    uint32*,
    uint32*,
    uint8,
    uint8);
