/*! \file shr_util_realloc.c
 *
 * Convenience function for doing realloc on top of sal_alloc API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_util.h>

void *
shr_util_realloc(void *old_ptr, size_t old_size, size_t size, char *str)
{
    void *ptr;

    if (old_ptr != NULL && size <= old_size) {
        /* In the unlikely case that we shrink the memory */
        return old_ptr;
    }
    /* Allocate new block of memory */
    ptr = sal_alloc(size, str);
    if (ptr == NULL) {
        return NULL;
    }
    /* Copy old buffer if provided */
    if (old_ptr != NULL && old_size > 0) {
        sal_memcpy(ptr, old_ptr, old_size);
        sal_free(old_ptr);
    }
    return ptr;
}
