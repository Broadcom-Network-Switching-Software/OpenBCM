/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_sem.h>

sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count)
{
    return NULL;
}

void
sal_sem_destroy(sal_sem_t sem)
{
}

int
sal_sem_take(sal_sem_t sem, int usec)
{
    return -1;
}

int
sal_sem_give(sal_sem_t sem)
{
    return -1;
}
