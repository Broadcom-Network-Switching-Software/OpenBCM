/*! \file libc_strtok_r.c
 *
 * SDK implementation of strtok_r.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strtok_r

char *
sal_strtok_r(char *s1, const char *delim, char **s2)
{
    char *ret_str;
    char *p;
    int len = 0;

    if (s1 == NULL) {
        s1 = *s2;
        if (s1 == NULL) {
            return NULL;
        }
    }

    /* Skip initial delimiters */
    p = s1;
    while ((*p) && (sal_strchr(delim, *p++) != NULL)) {
       len++;
    }
    s1 += len;

    if (*s1 == '\0') {
        return NULL;
    }

    /* Parse token */
    ret_str = s1;
    while (*s1) {
       if (sal_strchr(delim, *s1) != NULL) {
           break;
       }
       s1++;
    }

    if (*s1) {
        *s1++ = '\0';
    }

    if (s2 != NULL) {
        *s2 = s1;
    }

    return ret_str;
}

#endif
