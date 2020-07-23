/*! \file shr_util_endian.c
 *
 * Utility functions for endianness-dependent code.
 *
 * Mainly intended for DMA descriptors and packet headers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <sal/sal_libc.h>

#include <shr/shr_util.h>

static int initialized;
static int host_is_be;

static void
host_endian_init(void)
{
    uint32_t data;
    uint8_t *ptr;

    data = 0x11223344;
    ptr = (uint8_t *)(void *)&data;
    host_is_be = (*ptr == 0x11) ? 1 : 0;
    initialized = 1;
}

int
shr_util_host_is_be(void) {

    if (!initialized) {
        host_endian_init();
    }

    return host_is_be;
}

int
shr_util_host_to_be32(uint32_t *dst, uint32_t *src, size_t wlen)
{
    unsigned int idx;

    assert(src);
    assert(dst);

    if (!initialized) {
        host_endian_init();
    }
    if (host_is_be) {
        sal_memcpy(dst, src, 4 * wlen);
    } else {
        for (idx = 0; idx < wlen; idx++) {
            SHR_UTIL_HOST_WORD_TO_BE32(&dst[idx], &src[idx]);
        }
    }
    return 0;
}

int
shr_util_host_to_le32(uint32_t *dst, uint32_t *src, size_t wlen)
{
    unsigned int idx;

    assert(src);
    assert(dst);

    if (!initialized) {
        host_endian_init();
    }
    if (host_is_be) {
        for (idx = 0; idx < wlen; idx++) {
            SHR_UTIL_HOST_WORD_TO_LE32(&dst[idx], &src[idx]);
        }
    } else {
        sal_memcpy(dst, src, 4 * wlen);
    }
    return 0;
}
