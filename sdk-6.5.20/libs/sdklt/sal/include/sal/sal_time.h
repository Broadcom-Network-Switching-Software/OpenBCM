/*! \file sal_time.h
 *
 * Time API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_TIME_H
#define SAL_TIME_H

#include <sal/sal_types.h>

/*! Number of milliseconds per second. */
#define SECOND_MSEC     1000

/*! Number of milliseconds per minute. */
#define MINUTE_MSEC     (60 * SECOND_MSEC)

/*! Number of milliseconds per hour. */
#define HOUR_MSEC       (60 * MINUTE_MSEC)

/*! Number of microseconds per millisecond. */
#define MSEC_USEC       1000

/*! Number of microseconds per second. */
#define SECOND_USEC     (SECOND_MSEC * MSEC_USEC)

/*! Number of microseconds per minute. */
#define MINUTE_USEC     (60 * SECOND_USEC)

/*! Number of microseconds per hour. */
#define HOUR_USEC       (60 * MINUTE_USEC)

/*! Number of nanoseconds per microsecond. */
#define USEC_NSEC       1000

/*! Number of nanoseconds per second. */
#define SECOND_NSEC     (SECOND_USEC * USEC_NSEC)

/*! Timestamp in seconds. */
typedef unsigned long sal_time_t;

/*! Timestamp in microseconds. */
typedef uint32_t sal_usecs_t;

/*! Microsecond rollover check value. */
#define SAL_USECS_TIMESTAMP_ROLLOVER    ((sal_usecs_t)(-1))

/*! Signed result from subtraction of two sal_usecs_t values. */
#define SAL_USECS_SUB(_t2, _t1) \
    ((int)((_t2) - (_t1)))

/*! Result from addition of two sal_usecs_t values. */
#define SAL_USECS_ADD(_t, _us) \
    ((_t) + (_us))

/*!
 * \brief Get current timestamp in seconds.
 *
 * \return Timestamp in seconds.
 */
extern sal_time_t
sal_time(void);

/*!
 * \brief Get current timestamp in microseconds.
 *
 * \return Timestamp in microseconds.
 */
extern sal_usecs_t
sal_time_usecs(void);

#endif /* SAL_TIME_H */
