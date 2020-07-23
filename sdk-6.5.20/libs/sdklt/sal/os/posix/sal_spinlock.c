/*! \file sal_spinlock.c
 *
 * Spinlock API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sys/select.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include <sal/sal_time.h>
#include <sal/sal_mutex.h>
#include <sal/sal_spinlock.h>

/* Default configuration */
#ifndef SAL_SPINLOCK_USE_POSIX_NATIVE
#  if defined(__linux__) && defined(__USE_XOPEN2K) && defined(__GLIBC__)
#    define SAL_SPINLOCK_USE_POSIX_NATIVE      1
#  endif
#endif
#ifndef SAL_SPINLOCK_USE_POSIX_NATIVE
#define SAL_SPINLOCK_USE_POSIX_NATIVE          0
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_SPINLOCK_USE_POSIX_NATIVE))
#endif


/* Local abstract type built on the POSIX spinlock */
#if (SAL_SPINLOCK_USE_POSIX_NATIVE)
typedef struct spinlock_ctrl_s {
    pthread_spinlock_t spinlock;
    char *desc;
} *spinlock_ctrl_t;
#endif


sal_spinlock_t
sal_spinlock_create(char *desc)
{
#if SAL_SPINLOCK_USE_POSIX_NATIVE
    spinlock_ctrl_t sl = malloc(sizeof(*sl));
    int rv;

    if (sl != NULL) {
        rv = pthread_spin_init(&(sl->spinlock), PTHREAD_PROCESS_SHARED);
        if (rv < 0) {
            free(sl);
            return (sal_spinlock_t)NULL;
        }
        sl->desc = desc;
    }
    return (sal_spinlock_t)sl;
#else
    return (sal_spinlock_t)sal_mutex_create(desc);
#endif
}

int
sal_spinlock_destroy(sal_spinlock_t lock)
{
#if SAL_SPINLOCK_USE_POSIX_NATIVE
    spinlock_ctrl_t sl = (spinlock_ctrl_t)lock;
    int rv;

    assert(sl);

    rv = pthread_spin_destroy(&sl->spinlock);
    free(sl);

    return rv;
#else
    sal_mutex_destroy((sal_mutex_t)lock);

    return 0;
#endif
}

int
sal_spinlock_lock(sal_spinlock_t lock)
{
#if (SAL_SPINLOCK_USE_POSIX_NATIVE)
    spinlock_ctrl_t sl = (spinlock_ctrl_t)lock;
    struct timeval tv;

    assert(sl);

    /*
     * On most systems pthread_spin_lock() will not block preemption,
     * which could lead to deadlock. To avoid this, we use
     * pthread_spin_trylock() instead.
     */
    while (pthread_spin_trylock(&sl->spinlock)) {
        tv.tv_sec = 0;
        tv.tv_usec = SECOND_USEC / sysconf(_SC_CLK_TCK);
        select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &tv);
    }
    return 0;
#else
    return sal_mutex_take((sal_mutex_t)lock, SAL_MUTEX_FOREVER);
#endif
}

int
sal_spinlock_unlock(sal_spinlock_t lock)
{
#if (SAL_SPINLOCK_USE_POSIX_NATIVE)
    spinlock_ctrl_t sl = (spinlock_ctrl_t)lock;

    assert(sl);

    return pthread_spin_unlock(&sl->spinlock);
#else
    return sal_mutex_give((sal_mutex_t)lock);
#endif
}
