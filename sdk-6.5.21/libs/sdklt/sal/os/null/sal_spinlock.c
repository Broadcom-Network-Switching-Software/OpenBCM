/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_spinlock.h>

sal_spinlock_t
sal_spinlock_create(char *desc)
{
    return NULL;
}

int
sal_spinlock_destroy(sal_spinlock_t lock)
{
    return 0;
}

int
sal_spinlock_lock(sal_spinlock_t lock)
{
    return -1;
}

int
sal_spinlock_unlock(sal_spinlock_t lock)
{
    return -1;
}
