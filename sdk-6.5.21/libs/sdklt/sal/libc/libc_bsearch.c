/*! \file libc_bsearch.c
 *
 * Basic support of binary search.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_bsearch

void *
sal_bsearch(const void *el, const void *arr, size_t numel,
            size_t elsz, int (*cmpfn)(const void *, const void *))
{
    const uint8_t *a = (const uint8_t *)arr;
    const uint8_t *pivot;
    int rv;

    if (cmpfn == NULL) {
        return NULL;
    }

    while (numel > 0) {
        pivot = a + (numel >> 1) * elsz;
        rv = cmpfn(el, pivot);
        if (rv == 0) {
            return (void *)pivot;
        } else if (rv > 0) {
            a = pivot + elsz;
            numel--;
        }
        numel >>= 1;
    }
    return NULL;
}

#endif
