/*! \file sal_mutex.h
 *
 * Mutex API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_MUTEX_H
#define SAL_MUTEX_H

#include <sal/sal_types.h>

/*!
 * Use this value to disable timeout for \ref sal_mutex_take.
 */
#define SAL_MUTEX_FOREVER       -1

/*!
 * Use this timeout value to return immediately from \ref
 * sal_mutex_take even if mutex was not acquired.
 */
#define SAL_MUTEX_NOWAIT        0

/*! Opaque mutex handle. */
typedef struct sal_mutex_s *sal_mutex_t;

/*!
 * \brief Create a mutex.
 *
 * Create mutually exclusive lock to protect shared data from
 * concurrent access. Only one thread at a time can hold the mutex.
 *
 * \param [in] desc Text string for tracking purposes.
 *
 * \return Mutex handle or NULL on error.
 */
extern sal_mutex_t
sal_mutex_create(char *desc);

/*!
 * \brief Destroy a mutex.
 *
 * Destroy a mutex created by \ref sal_mutex_create and free all
 * associated resources.
 *
 * \param [in] mtx Mutex handle from \ref sal_mutex_create.
 *
 * \return Nothing.
 */
extern void
sal_mutex_destroy(sal_mutex_t mtx);

/*!
 * \brief Acquire mutex.
 *
 * This function will attempt to acquire a mutex. If the mutex is
 * already taken, then the function will wait for up to \c usec
 * microseconds and return either when the mutex has been acquired or
 * the specified wait time has elapsed.
 *
 * The special timeout value \ref SAL_MUTEX_FOREVER will cause the
 * function to wait indefinitely for the mutex.
 *
 * \param [in] mtx Mutex handle from \ref sal_mutex_create.
 * \param [in] usec Maximum number of microseconds to wait.
 *
 * \retval 0 Mutex successfully acquired.
 * \retval -1 Failed to acquire mutex.
 */
extern int
sal_mutex_take(sal_mutex_t mtx, int usec);

/*!
 * \brief Release mutex.
 *
 * Release lock acquired by calling \ref sal_mutex_take.
 *
 * \param [in] mtx Mutex handle from \ref sal_mutex_create.
 *
 * \retval 0 Mutex successfully released.
 * \retval -1 Failed to release mutex.
 */
extern int
sal_mutex_give(sal_mutex_t mtx);

#endif /* SAL_MUTEX_H */
