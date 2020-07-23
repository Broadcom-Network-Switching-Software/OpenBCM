/*! \file sal_date.c
 *
 * Date API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <sal/sal_date.h>

char *
sal_date(sal_date_t *date)
{
    char *ptr, *nl;
    time_t now = time(NULL);

    if (!date || sizeof(date->buf) < 26) {
        return SAL_DATE_NULL;
    }
    ptr = ctime_r(&now, date->buf);
    if (!ptr) {
        return SAL_DATE_NULL;
    }
    /* Remove newline if present */
    if ((nl = strchr(ptr, '\n')) != NULL) {
        *nl = '\0';
    }
    return ptr;
}
