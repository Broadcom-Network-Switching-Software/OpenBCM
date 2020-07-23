/*! \file sal_rwlock.h
 *
 * Read/write lock API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_RWLOCK_H
#define SAL_RWLOCK_H

#include <sal_config.h>

/*!
 * Use this value to disable timeout for \ref sal_rwlock_rlock. and
 * \ref sal_rwlock_wlock.
 */
#define SAL_RWLOCK_FOREVER      (uint32_t)(-1)

/*!
 * Use this timeout value to return immediately from \ref
 * sal_rwlock_rlock or \ref sal_rwlock_wlock even if the lock could
 * not be acquired.
 */
#define SAL_RWLOCK_NOWAIT       0

/*! Opaque read/write lock handle. */
typedef struct sal_rwlock_s *sal_rwlock_t;

/*!
 * \brief Create read/write lock
 *
 * \param [in] desc Text string that identifies the object during debug.
 *
 * \return Lock handle on success or NULL on failure.
 */
extern sal_rwlock_t
sal_rwlock_create(const char *desc);

/*!
 * \brief Destroy read/write lock
 *
 * \param [in] rwlock Lock handle returned by \ref sal_rwlock_create.
 *
 * \return Nothing.
 */
extern void
sal_rwlock_destroy(sal_rwlock_t rwlock);

/*!
 * \brief Acquire read lock.
 *
 * Upon acquiring this lock other threads will not be able to acquire
 * the corresponding write lock until this lock is released (using
 * \ref sal_rwlock_give).  Note that the same thread may acquire the
 * read lock multiple times. However, it is assumed that \ref
 * sal_rwlock_give will be called for every lock acquisition.
 *
 * \param [in] rwlock Lock handle returned by \ref sal_rwlock_create.
 * \param [in] usec Maximum time to wait for the lock.
 *
 * \return 0 on success and non-zero otherwise.
 */
extern int
sal_rwlock_rlock(sal_rwlock_t rwlock, uint32_t usec);

/*!
 * \brief Acquire write lock.
 *
 * Upon acquiring this lock other threads will not be able to acquire
 * the corresponding read or write locks until this lock is released
 * (using \ref sal_rwlock_give).  Note that the same thread may obtain
 * the write lock multiple times. However, it is assumed that \ref
 * sal_rwlock_give will be called for every lock acquisition.
 *
 * \param [in] rwlock Lock handle returned by \ref sal_rwlock_create.
 * \param [in] usec Maximum time to wait for the lock.
 *
 * \return 0 on success and non-zero otherwise.
 */
extern int
sal_rwlock_wlock(sal_rwlock_t rwlock, uint32_t usec);

/*!
 * \brief Release a lock (read or write).
 *
 * \param [in] rwlock Lock handle returned by \ref sal_rwlock_create.
 *
 * \return 0 on success and non-zero otherwise.
 */
extern int
sal_rwlock_give(sal_rwlock_t rwlock);

#endif /* SAL_RWLOCK_H */
