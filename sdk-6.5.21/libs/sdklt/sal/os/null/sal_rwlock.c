/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_rwlock.h>

sal_rwlock_t
sal_rwlock_create(const char *desc)
{
    return NULL;
}

void
sal_rwlock_destroy(sal_rwlock_t rwlock)
{
}

int
sal_rwlock_rlock(sal_rwlock_t rwlock, uint32_t usec)
{
    return -1;
}

int
sal_rwlock_wlock(sal_rwlock_t rwlock, uint32_t usec)
{
    return -1;
}

int
sal_rwlock_give(sal_rwlock_t rwlock)
{
    return -1;
}

