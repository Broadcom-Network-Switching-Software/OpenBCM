/*! \file cci_util.c
 *
 * Common utility  functions for CCI
 *
 * This file contains the implementation of  common utility  func for CCI
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmptm/bcmptm_cci_internal.h>

/*!
 * \brief Find max of two parameters without branch.
 * if x < y, then -(x < y) will be all ones,
 * so r = x ^ (x ^ y) & ~0 = x ^ x ^ y = y
 */
#define MAX_OF(x, y) (x ^ ((x ^ y) & -(x < y)))

/*!
 * \brief Copy bit fields between two word arrays.
 */
void
bcmptm_cci_buff32_field_copy(uint32_t *d, int offd, const uint32_t *s,
                             int offs, int n)
{
    uint32_t mask;
    const uint32_t *ps;
    uint32_t *pd;
    int uoffd, uoffs, t;
    int m;

    while (n) {
        /* Get offset with in 32 bit boundary */
        uoffs = offs & 31;
        uoffd = offd & 31;
        /* # of bits to be copied from src to dst */
        t = MAX_OF(uoffd, uoffs);
        m = (n + t > 31) ? 32 - t : n;
        /* Get to uint32_t pointer associated with the offset */
        ps = s + (offs >> 5);
        pd = d + (offd >> 5);
        /* Copy the multiple bits */
        if (m == 32) {
            *pd = *ps;
        } else {
            mask = (0x01 << m) - 1;
            /* Zero out the destination then populate bits */
            *pd &= ~(mask << uoffd);
            *pd |= ((*ps >> uoffs) & mask) << uoffd;
        }
        /* increment the offsets */
        offs += m;
        offd += m;
        n -= m;
    }
}

