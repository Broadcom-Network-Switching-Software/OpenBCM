/*! \file bcmpmac_field_be.c
 *
 * BCM PortMAC register field operations for big endian word order.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <bcmpmac/bcmpmac_util_internal.h>

uint32_t *
bcmpmac_field_be_get(const uint32_t *entbuf, int wsize,
                     int sbit, int ebit, uint32_t *fbuf)
{
    int i, wp, bp, len;

    assert(fbuf);

    bp = sbit;
    len = ebit - sbit + 1;
    assert(len > 0);

    wp = wsize - (bp / 32) - 1;
    bp = bp & (32 - 1);
    i = 0;

    for (; len > 0; len -= 32, i++) {
        assert(wp >= 0);
        if (bp) {
            fbuf[i] = (entbuf[wp--] >> bp) & ((1 << (32 - bp)) - 1);
            if (len > (32 - bp)) {
                fbuf[i] |= entbuf[wp] << (32 - bp);
            }
        } else {
            fbuf[i] = entbuf[wp--];
        }
        if (len < 32) {
            fbuf[i] &= ((1 << len) - 1);
        }
    }

    return fbuf;
}

void
bcmpmac_field_be_set(uint32_t *entbuf, int wsize,
                     int sbit, int ebit, uint32_t *fbuf)
{
    uint32_t mask;
    int i, wp, bp, len;

    assert(fbuf);

    bp = sbit;
    len = ebit - sbit + 1;
    assert(len > 0);

    wp = wsize - (bp / 32) - 1;
    bp = bp & (32 - 1);
    i = 0;

    for (; len > 0; len -= 32, i++) {
        assert(wp >= 0);
        if (bp) {
            if (len < 32) {
                mask = (1 << len) - 1;
            } else {
                mask = ~0;
            }
            entbuf[wp] &= ~(mask << bp);
            entbuf[wp--] |= fbuf[i] << bp;
            if (len > (32 - bp)) {
                entbuf[wp] &= ~(mask >> (32 - bp));
                entbuf[wp] |= fbuf[i] >> (32 - bp) & ((1 << bp) - 1);
            }
        } else {
            if (len < 32) {
                mask = (1 << len) - 1;
                entbuf[wp] &= ~mask;
                entbuf[wp--] |= (fbuf[i] & mask) << bp;
            } else {
                entbuf[wp--] = fbuf[i];
            }
        }
    }
}

uint32_t
bcmpmac_field32_be_get(const uint32_t *entbuf, int wsize,
                       int sbit, int ebit)
{
    uint32_t fval;

    /* coverity[address_of] */
    bcmpmac_field_be_get(entbuf, wsize, sbit, ebit, &fval);

    return fval;
}

void
bcmpmac_field32_be_set(uint32_t *entbuf, int wsize,
                       int sbit, int ebit, uint32_t fval)
{
    /* coverity[address_of] */
    bcmpmac_field_be_set(entbuf, wsize, sbit, ebit, &fval);
}
