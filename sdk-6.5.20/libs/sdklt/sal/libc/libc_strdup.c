/*! \file libc_strdup.c
 *
 * SDK implementation of strdup.
 *
 * Note that thihs version relies on sal_alloc, so allocated string
 * must be freed using sal_free.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>

#include <sal/sal_libc.h>

#ifndef sal_strdup

char *
sal_strdup(const char *str)
{
    int len = sal_strlen(str) + 1;
    char *rc = sal_alloc(len, "salStrdup");

    if (rc != NULL) {
        sal_memcpy(rc, str, len);
    }
    return rc;
}

#endif
