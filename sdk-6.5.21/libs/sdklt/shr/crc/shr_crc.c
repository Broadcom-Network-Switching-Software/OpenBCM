/*! \file shr_crc.c
 *
 * Ethernet CRC Algorithm. Initially for hash vector computing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_crc.h>

static uint32_t
shr_swap32(uint32_t i)
{
    i = (i << 16) | (i >> 16);

    return (i & 0xff00ffff) >> 8 | (i & 0xffff00ff) << 8;
}

uint16_t
shr_bit_rev16(uint16_t n)
{
    n = (((n & 0xaaaa) >> 1) | ((n & 0x5555) << 1));
    n = (((n & 0xcccc) >> 2) | ((n & 0x3333) << 2));
    n = (((n & 0xf0f0) >> 4) | ((n & 0x0f0f) << 4));
    n = (((n & 0xff00) >> 8) | ((n & 0x00ff) << 8));
    return n;
}

uint32_t
shr_bit_rev_by_byte_word32(uint32_t n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    return n;
}


static int shr_crc_table_inited;
static uint32_t shr_crc_table[256];

uint32_t
shr_crc32(uint32_t crc, uint8_t *data, int len)
{
    int i;

    if (!shr_crc_table_inited) {
        int j;
        uint32_t accum;

        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                if (accum & 1) {
                    accum = accum >> 1 ^ 0xedb88320UL;
                } else {
                    accum = accum >> 1;
                }
            }

            shr_crc_table[i] = shr_swap32(accum);
        }

        shr_crc_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
        crc = crc << 8 ^ shr_crc_table[crc >> 24 ^ data[i]];
    }

    return crc;
}

uint32_t
shr_crc32b(uint32_t crc, uint8_t *data, int nbits)
{
    int i;
    int j;
    uint32_t accum;
    uint32_t poly = 0xedb88320UL;
    int last_nbits;

    if (!shr_crc_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            shr_crc_table[i] = shr_swap32(accum);
        }
        shr_crc_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
        crc = (crc << 8) ^ shr_crc_table[data[i] ^ ((crc >> 24) & 0x000FF)];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = ((crc >> (32 - last_nbits)) & ((1 << last_nbits) - 1)) ^
                (data[i]  & ((1 << last_nbits) - 1));
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc << last_nbits) ^ shr_swap32(accum);
    }

    return crc;
}

static uint16_t shr_crc_16_table[16] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
};

uint16_t
shr_crc16(int start, uint8_t *p, int n)
{
    uint16_t crc = start;
    int r;

    /* while there is more data to process */
    while (n-- > 0) {

        /* compute checksum of lower four bits of *p */
        r = shr_crc_16_table[crc & 0xF];
        crc = (crc >> 4) & 0x0FFF;
        crc = crc ^ r ^ shr_crc_16_table[*p & 0xF];

        /* now compute checksum of upper four bits of *p */
        r = shr_crc_16_table[crc & 0xF];
        crc = (crc >> 4) & 0x0FFF;
        crc = crc ^ r ^ shr_crc_16_table[(*p >> 4) & 0xF];

        /* next... */
        p++;
    }

    return(crc);
}

static int shr_crc16b_table_inited;
static uint32_t shr_crc_16btable[256];

uint16_t
shr_crc16b(int crc, uint8_t *data, int nbits)
{
    int i;
    int j;
    uint32_t accum;
    uint32_t poly = 0xa001;
    int last_nbits;

    if (!shr_crc16b_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            shr_crc_16btable[i] = accum;
        }
        shr_crc16b_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
        crc = (crc >> 8) ^ shr_crc_16btable[((data[i] & 0x000000FF) << 0) ^
                                             (crc & 0x000000FF)];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = (crc & ((1 << last_nbits) - 1)) ^
                (data[i]  & ((1 << last_nbits) - 1));
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc >> last_nbits) ^ accum;
    }

    return crc;
}
