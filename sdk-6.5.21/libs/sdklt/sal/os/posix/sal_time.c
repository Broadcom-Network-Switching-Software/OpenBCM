/*! \file sal_time.c
 *
 * Time API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <sal/sal_time.h>

/* Default configuration */
#ifndef SAL_TIME_USE_CLOCK_MONOTONIC
#  ifdef CLOCK_MONOTONIC
#    define SAL_TIME_USE_CLOCK_MONOTONIC        1
#  endif
#endif
#ifndef SAL_TIME_USE_CLOCK_MONOTONIC
#  define SAL_TIME_USE_CLOCK_MONOTONIC          0
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_TIME_USE_CLOCK_MONOTONIC))
#endif


sal_time_t
sal_time(void)
{
    return (sal_time_t)time(0);
}

sal_usecs_t
sal_time_usecs(void)
{
    struct timeval	ltv;
#if (SAL_TIME_USE_CLOCK_MONOTONIC)
    struct timespec	lts;

    if (clock_gettime(CLOCK_MONOTONIC, &lts) == 0) {
        return (lts.tv_sec * SECOND_USEC) + (lts.tv_nsec / 1000);
    }
#endif

    /* Fall back to RTC if monotonic clock unavailable */
    gettimeofday(&ltv, 0);
    return (ltv.tv_sec * SECOND_USEC) + ltv.tv_usec;
}
