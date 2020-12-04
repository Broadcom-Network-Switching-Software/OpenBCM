/*! \file sal_sleep.c
 *
 * Sleep API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <time.h>

#include <sal/sal_sleep.h>

void
sal_sleep(unsigned int sec)
{
    struct timespec tv;

    tv.tv_sec = (time_t)sec;
    tv.tv_nsec = 0;
    nanosleep(&tv, NULL);
}
