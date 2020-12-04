/*! \file sal_usleep.c
 *
 * Sleep API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <sched.h>
#include <time.h>

#include <sal/sal_time.h>
#include <sal/sal_sleep.h>

/* Default configuration */
#ifndef SAL_USLEEP_USE_SCHED_YIELD
#  if defined(_POSIX_PRIORITY_SCHEDULING)
#    if (_POSIX_PRIORITY_SCHEDULING >= 200112L)
#      define SAL_USLEEP_USE_SCHED_YIELD        1
#    endif
#  endif
#endif
#ifndef SAL_USLEEP_USE_SCHED_YIELD
#define SAL_USLEEP_USE_SCHED_YIELD              0
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_USLEEP_USE_SCHED_YIELD))
#endif

#if (SAL_USLEEP_USE_SCHED_YIELD)
#define SCHED_YIELD() sched_yield()
#else
#define SCHED_YIELD()
#endif

static long usec_res;

void
sal_usleep(sal_usecs_t usec)
{
    struct timespec tv;

    /* Get timer resolution the first time */
    if (usec_res == 0) {
        clock_getres(CLOCK_REALTIME, &tv);
        usec_res = tv.tv_nsec / 1000;
        if (usec_res == 0) {
            usec_res = 1;
        }
    }

    /* Poll if wait time is close to timer resolution */
    if (usec < (2 * usec_res)) {
        sal_usecs_t now, earlier;
        earlier = sal_time_usecs();
        do {
            SCHED_YIELD();
            now = sal_time_usecs();
        } while ((sal_usecs_t)(now - earlier) < usec);
        return;
    }

    /* Sleep for the specified time */
    tv.tv_sec = (time_t)(usec / SECOND_USEC);
    tv.tv_nsec = (long)(usec % SECOND_USEC) * 1000;
    nanosleep(&tv, NULL);
}
