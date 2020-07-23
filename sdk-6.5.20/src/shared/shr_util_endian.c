/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility functions for endianness-dependent code.
 *
 * Mainly intended for DMA descriptors and packet headers.
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/shr_util_endian.h>

static int initialized;
static int host_is_be;

static void
host_endian_init(void)
{
    uint32 data;
    uint8 *ptr;

    data = 0x11223344;
    ptr = (uint8 *)(void *)&data;
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
shr_util_host_to_be32(uint32 *dst, uint32 *src, size_t wlen)
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
shr_util_host_to_le32(uint32 *dst, uint32 *src, size_t wlen)
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
