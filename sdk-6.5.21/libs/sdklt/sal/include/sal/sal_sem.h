/*! \file sal_sem.h
 *
 * Semaphore API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_SEM_H
#define SAL_SEM_H

#include <sal/sal_types.h>

/*! Create counting semaphore when calling \ref sal_sem_create. */
#define SAL_SEM_COUNTING        0

/*! Create binary semaphore when calling \ref sal_sem_create. */
#define SAL_SEM_BINARY          1

/*!
 * Use this value to disable timeout for \ref sal_sem_take.
 */
#define SAL_SEM_FOREVER         -1

/*!
 * Use this timeout value to return immediately from \ref sal_sem_take
 * even if semaphore was not acquired.
 */
#define SAL_SEM_NOWAIT          0

/*! Opaque semaphore handle. */
typedef struct sal_sem_s *sal_sem_t;

/*!
 * \brief Create a semaphore.
 *
 * Create a binary or counting semaphore.
 *
 * A binary semaphore can be taken only once even if it has been given
 * multiple times. A binary semaphore is typically used to signal a
 * thread to run, either from another thread or from an interrupt
 * handler.
 *
 * A counting semaphore is normally used to protect a shared
 * resource. The initial count should correspond to the number of
 * resources which are protected by the semaphore.
 *
 * \param [in] desc Text string for tracking purposes.
 * \param [in] binary Set to \ref SAL_SEM_BINARY or \ref SAL_SEM_COUNTING.
 * \param [in] initial_count Initial count for \ref SAL_SEM_COUNTING type.
 *
 * \return Semaphore handle or NULL on error.
 */
extern sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count);

/*!
 * \brief Destroy a semaphore.
 *
 * Destroy a semaphore created by \ref sal_sem_create and free all
 * associated resources.
 *
 * \param [in] sem Semaphore handle returned by \ref sal_sem_create.
 *
 * \return Nothing.
 */
extern void
sal_sem_destroy(sal_sem_t sem);

/*!
 * \brief Acquire semaphore.
 *
 * This function will attempt to acquire a semaphore by decrementing
 * the semaphore count. If the semaphore is locked (count is zero),
 * then the function will wait for up to \c usec microseconds and
 * return either when the semaphore has been acquired or the specified
 * wait time has elapsed.
 *
 * The special timeout value \ref SAL_SEM_FOREVER will cause the
 * function to wait indefinitely for the semaphore.
 *
 * \param [in] sem Semaphore handle returned by \ref sal_sem_create.
 * \param [in] usec Maximum number of microseconds to wait.
 *
 * \retval 0 Semaphore successfully acquired.
 * \retval -1 Failed to acquire semaphore.
 */
extern int
sal_sem_take(sal_sem_t sem, int usec);

/*!
 * \brief Unlock semaphore.
 *
 * Increment the count value of a semaphore. For semaphores of type
 * \ref SAL_SEM_BINARY, the count is not incremented beyond 1.
 *
 * \param [in] sem Semaphore handle returned by \ref sal_sem_create.
 *
 * \retval 0 Semaphore successfully unlocked.
 * \retval -1 Something went wrong.
 */
extern int
sal_sem_give(sal_sem_t sem);

#endif /* SAL_SEM_H */
