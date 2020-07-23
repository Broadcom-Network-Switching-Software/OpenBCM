/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:     time.h
 * Purpose:     SAL time definitions
 *
 * Microsecond Time Routines
 *
 *  The main clock abstraction is sal_usecs_t, an unsigned 32-bit
 *  counter that increments every microsecond and wraps around every
 *  4294.967296 seconds (~71.5 minutes)
 */

#ifndef _SAL_TIME_H
#define _SAL_TIME_H

#include <sal/types.h>

/*
 * Constants
 */

#define MILLISECOND_USEC        (1000)
#define SECOND_USEC            (1000000)
#define MINUTE_USEC            (60 * SECOND_USEC)
#define HOUR_USEC            (60 * MINUTE_USEC)

#define SECOND_MSEC            (1000)
#define MINUTE_MSEC            (60 * SECOND_MSEC)

#define SAL_USECS_TIMESTAMP_ROLLOVER    ((uint32)(0xFFFFFFFF))    

typedef unsigned long sal_time_t;
typedef uint32 sal_usecs_t;

void sal_time_usecs_register(sal_usecs_t (*f)(void));
void sal_time_register(sal_time_t (*f)(void));

/* Relative time in microseconds modulo 2^32 */
extern sal_usecs_t sal_time_usecs(void);

/* Absolute time in seconds ala Unix time, interrupt-safe */
extern sal_time_t sal_time(void);

#ifdef COMPILER_HAS_DOUBLE

void sal_time_double_register(double (*f)(void));

/*
 * Time since boot in seconds as a double precision value, useful for
 * benchmarking.  Precision is platform-dependent and may be limited to
 * scheduler clock period.  On a Mousse it uses the timebase and has a
 * precision of 80 ns (0.00000008).
 */
extern double sal_time_double(void);
#define SAL_TIME_DOUBLE() sal_time_double()

#else /* COMPILER_HAS_DOUBLE */
#define SAL_TIME_DOUBLE() sal_time()
#endif


/*
 * SAL_USECS_SUB
 *
 *  Subtracts two sal_usecs_t and returns signed integer microseconds.
 *  Can be used as a comparison because it returns:
 *
 *    integer < 0 if A < B
 *    integer > 0 if A > B
 *    integer = 0 if A == B
 *
 * SAL_USECS_ADD
 *
 *  Adds usec to time.  Wrap-around is OK as long as the total
 *  amount added is less than 0x7fffffff.
 */

#define SAL_USECS_SUB(_t2, _t1)        (((int) ((_t2) - (_t1))))
#define SAL_USECS_ADD(_t, _us)        ((_t) + (_us))

#endif    /* !_SAL_TIME_H */
