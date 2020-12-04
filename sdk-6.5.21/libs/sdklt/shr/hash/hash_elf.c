/*! \file hash_elf.c
 *
 * Calculate Unix ELF hash.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_hash.h>

uint32_t
shr_hash_elf(const uint8_t *data, size_t data_len)
{
    uint32_t hash = 0;
    uint32_t high = 0;
    size_t j = 0;
    const uint8_t *ptr;

    for (j = 0, ptr = data; j < data_len; ptr++, j++) {
        hash = (hash << 4) + (*ptr);
        if ((high = hash & 0xf0000000L) != 0) {
            hash ^= (high >> 24);
        }
        hash &= ~high;
    }
    return hash;
}
