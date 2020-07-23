/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	time.c
 * Purpose:	Time management
 */


#include <sys/types.h>
#include <sys/time.h>
#if defined(linux) || (__STRICT_ANSI__)
#include <time.h>
#endif
#include <sal/core/time.h>

#ifdef COMPILER_HAS_DOUBLE

/*
 * Function:
 *	sal_time_double
 * Purpose:
 *	Returns the time since boot in seconds.
 * Returns:
 *	Double precision floating-point seconds.
 * Notes:
 *	Useful for benchmarking.
 *	Made to be as accurate as possible on the given platform.
 */

double
sal_time_double(void)
{
    struct timeval	tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec + tv.tv_usec * 0.000001);
}

#endif /* COMPILER_HAS_DOUBLE */


/*
 * Function:
 *	sal_time_usecs
 * Purpose:
 *	Returns the relative time in microseconds modulo 2^32.
 * Returns:
 *	Time in microseconds modulo 2^32
 * Notes:
 *	The precision is limited to the Unix clock period.
 *	Time is monotonic if supported by the O/S.
 */

sal_usecs_t
sal_time_usecs(void)
{
    struct timeval	ltv;
#ifdef CLOCK_MONOTONIC
    struct timespec	lts;
    if (clock_gettime(CLOCK_MONOTONIC, &lts) == 0) {
        return (lts.tv_sec * SECOND_USEC + lts.tv_nsec / 1000);
    }
#endif
    /* Fall back to RTC if monotonic clock unavailable */
    gettimeofday(&ltv, 0);
    return (ltv.tv_sec * SECOND_USEC + ltv.tv_usec);
}
    
/*
 * Function:
 *	sal_time
 * Purpose:
 *	Return the current time in seconds since 00:00, Jan 1, 1970.
 * Returns:
 *	Time in seconds
 * Notes:
 *	This routine must be implemented so it is safe to call from
 *	an interrupt routine.  It is used for timestamping and other
 *	purposes.
 */

sal_time_t
sal_time(void)
{
    /* "Interrupt safe" since intrs are fake for PLISIM */
    return (sal_time_t) time(0);
}
