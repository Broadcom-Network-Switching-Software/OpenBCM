/*! \file sal_thread.c
 *
 * Thread API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_thread.h>

sal_thread_t
sal_thread_create(char *name, int stksz, int prio, void (func)(void *),
                  void *arg)
{
    return SAL_THREAD_ERROR;
}

sal_thread_t
sal_thread_self(void)
{
    return SAL_THREAD_ERROR;
}

char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size)
{
    return NULL;
}

void
sal_thread_exit(int rc)
{
}

void
sal_thread_yield(void)
{
}
