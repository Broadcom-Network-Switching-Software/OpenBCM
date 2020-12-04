/*! \file sal_mutex.c
 *
 * Mutex API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <sal/sal_time.h>
#include <sal/sal_sleep.h>
#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>

/* Default configuration */
#ifndef SAL_MUTEX_USE_POSIX_TIMEDLOCK
#  if defined(_POSIX_TIMERS) && (_POSIX_TIMERS >= 200112L)
#    if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS >= 200112L)
#      if defined(CLOCK_REALTIME)
#        define SAL_MUTEX_USE_POSIX_TIMEDLOCK   1
#      endif
#    endif
#  endif
#endif
#ifndef SAL_MUTEX_USE_POSIX_TIMEDLOCK
#  define SAL_MUTEX_USE_POSIX_TIMEDLOCK         0
#endif

#ifndef SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT
#ifdef CLOCK_MONOTONIC
#define SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT       1
#else
#define SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT       0
#endif
#endif

/*
 * Require timed semaphores for monotonic mutexes because the pthread mutex API
 * only supports real-time timers.
 */
#if SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT
#if defined(SAL_SEM_USE_MONOTONIC_TIMEDWAIT) && !SAL_SEM_USE_MONOTONIC_TIMEDWAIT
#error Monotonic mutexes require monotonic semaphores.
#else
#undef SAL_MUTEX_USE_POSIX_TIMEDLOCK
#define SAL_MUTEX_USE_POSIX_TIMEDLOCK           0
#endif
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT))
#pragma message(SAL_INT_VAR_VALUE(SAL_MUTEX_USE_POSIX_TIMEDLOCK))
#endif

#if (SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT == 0)

/* Local abstract type built on the POSIX mutex */
typedef struct mutex_ctrl_s {
    pthread_mutex_t mutex;
    char *desc;
} mutex_ctrl_t;

#if (SAL_MUTEX_USE_POSIX_TIMEDLOCK)
static int
_timed_mutex_take(mutex_ctrl_t *mc, int usec)
{
    struct timespec timespec, *ts = &timespec;
    time_t sec;
    long nsec;

    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        return -1;
    }

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;

    /* Compute new nsec */
    nsec = ts->tv_nsec + (usec % SECOND_USEC) * 1000;

    /* Detect and handle rollover */
    if (nsec < 0) {
        ts->tv_sec += 1;
        nsec -= SECOND_NSEC;
    }
    ts->tv_nsec = nsec;

    /* Normalize if needed */
    sec = ts->tv_nsec / SECOND_NSEC;
    if (sec) {
        ts->tv_sec += sec;
        ts->tv_nsec = ts->tv_nsec % SECOND_NSEC;
    }

    /* Treat EAGAIN as a fatal error */
    return pthread_mutex_timedlock(&mc->mutex, ts);
}
#else
static int
_timed_mutex_take(mutex_ctrl_t *mc, int usec)
{
    int time_wait = 1;
    int rv = 0;

    /* Retry algorithm with exponential backoff */
    for (;;) {
        rv = pthread_mutex_trylock(&mc->mutex);

        if (rv != EBUSY) {
            /* Done (or error other than EBUSY) */
            break;
        }

        if (time_wait > usec) {
            time_wait = usec;
        }

        sal_usleep(time_wait);

        usec -= time_wait;

        if (usec == 0) {
            rv = ETIMEDOUT;
            break;
        }

        if ((time_wait *= 2) > 100000) {
            time_wait = 100000;
        }
    }

    return (rv != 0) ? -1 : 0;
}
#endif /* SAL_MUTEX_USE_POSIX_TIMEDLOCK */
#else /* SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT == 1 */
static sal_mutex_t
sal_mutex_monotonic_create(char *desc)
{
    sal_sem_t sem;

    /* Use binary semaphore as the mutex */
    sem = sal_sem_create(desc, 1, 1);

    return (sal_mutex_t)sem;
}

static void
sal_mutex_monotonic_destroy(sal_mutex_t mtx)
{
    sal_sem_t sem = (sal_sem_t)mtx;

    assert(sem);

    return sal_sem_destroy(sem);
}

static int
sal_mutex_monotonic_take(sal_mutex_t mtx, int usec)
{
    sal_sem_t sem = (sal_sem_t)mtx;

    assert(sem);

    return sal_sem_take(sem, usec);
}

static int
sal_mutex_monotonic_give(sal_mutex_t mtx)
{
    sal_sem_t sem = (sal_sem_t)mtx;

    assert(sem);

    return sal_sem_give(sem);
}
#endif /* SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT */

sal_mutex_t
sal_mutex_create(char *desc)
{
#if (SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT)
    return sal_mutex_monotonic_create(desc);
#else
    mutex_ctrl_t *mc;
    pthread_mutexattr_t attr;

    if ((mc = malloc(sizeof (mutex_ctrl_t))) == NULL) {
	return NULL;
    }

    mc->desc = desc;
    pthread_mutexattr_init(&attr);

    pthread_mutex_init(&mc->mutex, &attr);

    return (sal_mutex_t)mc;
#endif
}

void
sal_mutex_destroy(sal_mutex_t mtx)
{
#if (SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT)
    return sal_mutex_monotonic_destroy(mtx);
#else
    mutex_ctrl_t *mc = (mutex_ctrl_t *)mtx;

    assert(mc);

    pthread_mutex_destroy(&mc->mutex);

    free(mc);
#endif
}

int
sal_mutex_take(sal_mutex_t mtx, int usec)
{
#if (SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT)
    return sal_mutex_monotonic_take(mtx, usec);
#else
    mutex_ctrl_t *mc = (mutex_ctrl_t *)mtx;
    int rv = 0;

    assert(mc);

    if (usec == SAL_MUTEX_FOREVER) {
        do {
            rv = pthread_mutex_lock(&mc->mutex);
        } while (rv != 0 && errno == EINTR);
    } else {
        rv = _timed_mutex_take(mc, usec);
    }

    return (rv != 0) ? -1 : 0;
#endif
}

int
sal_mutex_give(sal_mutex_t mtx)
{
#if (SAL_MUTEX_USE_MONOTONIC_TIMEDWAIT)
    return sal_mutex_monotonic_give(mtx);
#else
    mutex_ctrl_t *mc = (mutex_ctrl_t *)mtx;
    int	rv;

    assert(mc);

    rv = pthread_mutex_unlock(&mc->mutex);

    return (rv != 0) ? -1 : 0;
#endif
}
