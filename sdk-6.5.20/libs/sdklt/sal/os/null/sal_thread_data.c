/*! \file sal_thread_data.c
 *
 * Thread data API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_thread.h>

sal_thread_data_t *
sal_thread_data_create(void (*destructor)(void *))
{
    return NULL;
}

int
sal_thread_data_set(sal_thread_data_t *tdata, void *val)
{
    return 0;
}

void *
sal_thread_data_get(sal_thread_data_t *tdata)
{
    return NULL;
}

int
sal_thread_data_delete(sal_thread_data_t *tdata)
{
    return 0;
}
