/*! \file shr_timeout.h
 *
 * Polling timer with timeout detection.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_TIMEOUT_H
#define SHR_TIMEOUT_H

#include <sal/sal_time.h>

/*!
 * \brief Object for timeout tracking.
 *
 * Caller should not modify this structure directly.
 */
typedef struct shr_timeout_s {

    /*! Timeout will occur at this time. */
    sal_usecs_t expire;

    /*! Caller-provided timeout value. */
    sal_usecs_t usec;

    /*! Number of polls before we start sleeping between polls. */
    int min_polls;

    /*! Current number of polls. */
    int polls;

    /*! Number of usecs to sleep (increases with number of polls). */
    sal_usecs_t exp_delay;

} shr_timeout_t;

/*!
 * \brief Initialize timeout object.
 *
 * These routines implement a polling timer that, in the normal case,
 * has low overhead, but provides reasonably accurate timeouts for
 * intervals longer than a millisecond.
 *
 * If applicable to the situation, \c min_polls should be chosen such
 * that the operation is expected to complete within \c min_polls.
 *
 * If the operation does indeed complete within \c min_polls, then
 * there is very little overhead. Otherwise, the checker function
 * starts making O/S calls to check the real time clock and uses an
 * exponential timeout to avoid hogging the CPU.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     shr_timeout_t to;
 *     sal_usecs_t timeout_usec = 100000;
 *     int min_polls = 100;
 *
 *     shr_timeout_init(&to, timeout_usec, min_polls);
 *     while (TRUE) {
 *         if (check_XX_status(unit) == DONE) {
 *             break;
 *         }
 *         if (shr_timeout_check(&to)) {
 *             printf("Operation timed out\n");
 *             return ERROR;
 *         }
 *     }
 *     ...
 * }
 * \endcode
 *
 * \param [in] to Timeout object.
 * \param [in] usec Timeout value in usecs.
 * \param [in] min_polls Minimum number of polls before sleeping.
 *
 * \return Nothing.
 */
extern void
shr_timeout_init(shr_timeout_t *to, sal_usecs_t usec, int min_polls);

/*!
 * \brief Check if a timer timed out.
 *
 * See \ref shr_timeout_init for details.
 *
 * \param [in] to Timeout object.
 *
 * \retval 0 No timeout.
 * \retval 1 Timer timed out.
 */
extern int
shr_timeout_check(shr_timeout_t *to);

/*!
 * \brief Time elapsed since timeout object was initialized.
 *
 * See \ref shr_timeout_init for details.
 *
 * \param [in] to Timeout object.
 *
 * \return Number of usecs elapsed.
 */
sal_usecs_t
shr_timeout_elapsed(shr_timeout_t *to);

#endif /* SHR_TIMEOUT_H */
