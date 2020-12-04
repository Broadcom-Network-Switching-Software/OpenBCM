/*! \file shr_util.c
 *
 * Common utility routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_util.h>

int
shr_util_popcount(uint32_t n)
{
    n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0f0f0f0f;
    n = n + (n >> 8);

    return (n + (n >> 16)) & 0xff;
}

int
shr_util_ffs(uint32_t n)
{
    int i16, i8, i4, i2, i1, i0;

    i16 = !(n & 0xffff) << 4;
    n >>= i16;
    i8 = !(n & 0xff) << 3;
    n >>= i8;
    i4 = !(n & 0xf) << 2;
    n >>= i4;
    i2 = !(n & 0x3) << 1;
    n >>= i2;
    i1 = !(n & 0x1);
    i0 = (n >> i1) & 1 ? 0 : -32;

    return i16 + i8 + i4 + i2 + i1 + i0;
}

int
shr_util_log2(uint32_t n)
{
    int rv = -1;

    do {
        rv++;
        n >>= 1;
    } while (n);

    return rv;
}

int
shr_util_xch2i(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return (ch - '0'     );
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 10);
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 10);
    }
    return -1;
}

unsigned short
shr_ip_chksum(unsigned int length, unsigned char *data)
{
    unsigned int chksum = 0;
    unsigned short w16;
    int i=0;

    while (length > 1) {
        w16 = (((unsigned int)data[i]) << 8) + data[i+1];
        chksum += w16;
        i+=2;
        length -= 2;
    }
    if (length) {
        w16 = (((unsigned int)data[i]) << 8) + 0;
        chksum += w16;
    }

    while (chksum >> 16) {
        chksum = (chksum & 0xFFFF) + (chksum >> 16);
    }

    return (~chksum);
}

