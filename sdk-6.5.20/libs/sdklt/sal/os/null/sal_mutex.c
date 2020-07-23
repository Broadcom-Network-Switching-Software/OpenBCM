/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>

sal_mutex_t
sal_mutex_create(char *desc)
{
    return NULL;
}

void
sal_mutex_destroy(sal_mutex_t mtx)
{
}

int
sal_mutex_take(sal_mutex_t mtx, int usec)
{
    return -1;
}

int
sal_mutex_give(sal_mutex_t mtx)
{
    return -1;
}
