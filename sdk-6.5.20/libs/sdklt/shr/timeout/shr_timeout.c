/*! \file shr_timeout.c
 *
 * Polling timer with timeout detection.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_sleep.h>
#include <shr/shr_timeout.h>

void
shr_timeout_init(shr_timeout_t *to, sal_usecs_t usec, int min_polls)
{
    to->min_polls = min_polls;
    to->usec = usec;
    to->polls = 0;
    to->exp_delay = 1; /* In case caller sets min_polls < 0 */
}

int
shr_timeout_check(shr_timeout_t *to)
{
    if (++to->polls >= to->min_polls) {
        if (to->min_polls >= 0) {
            /*
             * Just exceeded min_polls; calculate expiration time by
             * consulting O/S real time clock.
             */
            to->min_polls = -1;
            to->expire = SAL_USECS_ADD(sal_time_usecs(), to->usec);
            to->exp_delay = 1;
        } else {
            /*
             * Exceeded min_polls in a previous call.
             * Consult O/S real time clock to check for expiration.
             */
            if (SAL_USECS_SUB(sal_time_usecs(), to->expire) >= 0) {
                return 1;
            }

            sal_usleep(to->exp_delay);

            /* Exponential backoff with 10% maximum latency */
            if ((to->exp_delay *= 2) > to->usec / 10) {
                to->exp_delay = to->usec / 10;
            }
        }
    }

    return 0;
}

sal_usecs_t
shr_timeout_elapsed(shr_timeout_t *to)
{
    sal_usecs_t start_time;

    start_time = SAL_USECS_SUB(to->expire, to->usec);

    return SAL_USECS_SUB(sal_time_usecs(), start_time);
}
