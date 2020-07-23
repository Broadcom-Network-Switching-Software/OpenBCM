/*! \file bcmdrd_field_be.c
 *
 * Utility functions used for extracting field values from
 * registers and memories with big-endian word order.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <bcmdrd/bcmdrd_field.h>

/*
 * Function:
 *	bcmdrd_field_be_get
 * Purpose:
 *	Extract multi-word field value from multi-word register/memory.
 * Parameters:
 *	entbuf - current contents of register/memory (word array)
 *      wsize - size of entbuf counted in 32-bit words
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 *      fbuf - buffer where to store extracted field value
 * Returns:
 *      Pointer to extracted field value.
 */
uint32_t *
bcmdrd_field_be_get(const uint32_t *entbuf, int wsize,
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

/*
 * Function:
 *	bcmdrd_field_be_set
 * Purpose:
 *	Assign multi-word field value in multi-word register/memory.
 * Parameters:
 *	entbuf - current contents of register/memory (word array)
 *      wsize - size of entbuf counted in 32-bit words
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 *      fbuf - buffer with new field value
 * Returns:
 *      Nothing.
 */
void
bcmdrd_field_be_set(uint32_t *entbuf, int wsize,
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
            entbuf[wp--] |= (fbuf[i] & mask) << bp;
            if (len > (32 - bp)) {
                entbuf[wp] &= ~(mask >> (32 - bp));
                entbuf[wp] |= (fbuf[i] & mask) >> (32 - bp) & ((1 << bp) - 1);
            }
        } else {
            if (len < 32) {
                mask = (1 << len) - 1;
                entbuf[wp] &= ~mask;
                entbuf[wp--] |= (fbuf[i] & mask);
            } else {
                entbuf[wp--] = fbuf[i];
            }
        }
    }
}
