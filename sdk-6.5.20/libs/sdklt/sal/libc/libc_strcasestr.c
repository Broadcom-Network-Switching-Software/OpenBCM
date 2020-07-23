/*! \file libc_strcasestr.c
 *
 * SDK implementation of strcasestr.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strcasestr

char *
sal_strcasestr(const char *dest, const char *src)
{
    int dl, sl;

    if ((sl = sal_strlen(src)) == 0) {
        return (char *)dest;
    }
    dl = sal_strlen(dest);
    while (dl >= sl) {
        dl--;
        if (sal_strncasecmp(dest, src, sl) == 0) {
            return (char *)dest;
        }
        dest++;
    }
    return NULL;
}

#endif
