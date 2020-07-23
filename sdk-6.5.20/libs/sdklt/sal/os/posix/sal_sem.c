/*! \file sal_sem.c
 *
 * Semaphore API.
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
#include <semaphore.h>
#include <pthread.h>

#include <sal/sal_time.h>
#include <sal/sal_sleep.h>
#include <sal/sal_sem.h>

/* Default configuration */
#ifndef SAL_SEM_USE_POSIX_TIMEDWAIT
#  if defined(_POSIX_TIMERS) && (_POSIX_TIMERS >= 200112L)
#    if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS >= 200112L)
#      if defined(CLOCK_REALTIME)
#        define SAL_SEM_USE_POSIX_TIMEDWAIT     1
#      endif
#    endif
#  endif
#endif
#ifndef SAL_SEM_USE_POSIX_TIMEDWAIT
#  define SAL_SEM_USE_POSIX_TIMEDWAIT           0
#endif

#ifndef SAL_SEM_USE_MONOTONIC_TIMEDWAIT
#ifdef CLOCK_MONOTONIC
#define SAL_SEM_USE_MONOTONIC_TIMEDWAIT         1
#else
#define SAL_SEM_USE_MONOTONIC_TIMEDWAIT         0
#endif
#endif

#if (SAL_SEM_USE_MONOTONIC_TIMEDWAIT)
#undef SAL_SEM_USE_POSIX_TIMEDWAIT
#define SAL_SEM_USE_POSIX_TIMEDWAIT             0
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_SEM_USE_MONOTONIC_TIMEDWAIT))
#pragma message(SAL_INT_VAR_VALUE(SAL_SEM_USE_POSIX_TIMEDWAIT))
#endif

#if (SAL_SEM_USE_MONOTONIC_TIMEDWAIT == 0)

/* Local abstract type built on the POSIX semaphore */
typedef struct {
    sem_t sem;
    char *desc;
    int binary;
} sem_ctrl_t;

#if (SAL_SEM_USE_POSIX_TIMEDWAIT)
static int
_timed_sem_take(sem_ctrl_t *sc, int usec)
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

    while (1) {
        if (sem_timedwait(&sc->sem, ts) == 0) {
            break;
        }
        if (errno != EAGAIN && errno != EINTR) {
            return -1;
        }
    }

    return 0;
}
#else
static int
_timed_sem_take(sem_ctrl_t *sc, int usec)
{
    int time_wait = 1;
    int rv = 0;

    /* Retry algorithm with exponential backoff */
    for (;;) {
        if (sem_trywait(&sc->sem) == 0) {
            rv = 0;
            break;
        }

        if (errno != EAGAIN && errno != EINTR) {
            rv = errno;
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
#endif /* SAL_SEM_USE_POSIX_TIMEDWAIT */
#else /* SAL_SEM_USE_MONOTONIC_TIMEDWAIT == 1 */

/* Local abstract type built on the POSIX mutex and condition variable. */
typedef struct sem_monotonic_ctrl_s {
    char *desc;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int val;
    int binary;
} sem_monotonic_ctrl_t;

/* Caculate the absolute timeout value. */
static int
sem_monotonic_timeout_get(int usec, struct timespec *ts)
{
    time_t sec;
    long nsec;

    if (clock_gettime(CLOCK_MONOTONIC, ts) != 0) {
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

    return 0;
}

static int
sem_monotonic_take(sem_monotonic_ctrl_t *msc, int usec)
{
    int rv;
    struct timespec timespec, *ts = NULL;

    if (usec != SAL_SEM_FOREVER) {
        rv = sem_monotonic_timeout_get(usec, &timespec);
        if (rv != 0) {
            return rv;
        }
        ts = &timespec;
    }

    rv = pthread_mutex_lock(&msc->mutex);
    while ((rv == 0) && (msc->val == 0)) {
        if (ts) {
            rv = pthread_cond_timedwait(&msc->cond, &msc->mutex, ts);
        } else {
            rv = pthread_cond_wait(&msc->cond, &msc->mutex);
        }
    }

    if (rv == 0) {
        msc->val--;
    }

    pthread_mutex_unlock(&msc->mutex);

    return rv;
}

static int
sem_monotonic_give(sem_monotonic_ctrl_t *msc)
{
    int rv;

    pthread_mutex_lock(&msc->mutex);
    msc->val++;
    rv = pthread_cond_broadcast(&msc->cond);
    pthread_mutex_unlock(&msc->mutex);

    return rv;
}

static sal_sem_t
sal_sem_monotonic_create(char *desc, int binary, int initial_count)
{
    int err = 0;
    sem_monotonic_ctrl_t *msc;
    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;

    if ((msc = malloc(sizeof (sem_monotonic_ctrl_t))) == NULL) {
        return NULL;
    }

    msc->desc = desc;
    msc->val = initial_count;
    msc->binary = binary;

    err += pthread_mutexattr_init(&mattr);
    err += pthread_mutex_init(&msc->mutex, &mattr);

    err += pthread_condattr_init(&cattr);
    err += pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
    err += pthread_cond_init(&msc->cond, &cattr);

    if (err) {
        free(msc);
        return NULL;
    }

    return (sal_sem_t)msc;
}

static void
sal_sem_monotonic_destroy(sal_sem_t sem)
{
    sem_monotonic_ctrl_t *msc = (sem_monotonic_ctrl_t *)sem;

    assert(msc);

    pthread_mutex_destroy(&msc->mutex);
    /*
     * We intentionally skip calling pthread_cond_destroy here, since
     * this function may hang if our CLI Ctrl-C handler (longjmp) was
     * called while we were waiting on the condition variable.
     */

    free(msc);
}

int
sal_sem_monotonic_take(sal_sem_t sem, int usec)
{
    int rv;
    sem_monotonic_ctrl_t *msc = (sem_monotonic_ctrl_t *)sem;

    assert(msc);

    rv = sem_monotonic_take(msc, usec);

    return (rv != 0) ? -1 : 0;
}

int
sal_sem_monotonic_give(sal_sem_t sem)
{
    int rv = 0;
    sem_monotonic_ctrl_t *msc = (sem_monotonic_ctrl_t *)sem;

    assert(msc);

    if (msc->binary) {
        /* Binary sem only post if sem_val == 0 */
        if (msc->val == 0) {
            rv = sem_monotonic_give(msc);
        }
    } else {
        rv = sem_monotonic_give(msc);
    }

    return (rv != 0) ? -1 : 0;
}
#endif /* SAL_SEM_USE_MONOTONIC_TIMEDWAIT */

sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count)
{
#if (SAL_SEM_USE_MONOTONIC_TIMEDWAIT)
    return sal_sem_monotonic_create(desc, binary, initial_count);
#else
    sem_ctrl_t *sc = NULL;

    if ((sc = malloc(sizeof (sem_ctrl_t))) == NULL) {
        return NULL;
    }
    if (sem_init(&sc->sem, 0, initial_count) < 0) {
        free(sc);
        return NULL;
    }
    sc->desc = desc;
    sc->binary = binary;

    return (sal_sem_t)sc;
#endif
}

void
sal_sem_destroy(sal_sem_t sem)
{
#if (SAL_SEM_USE_MONOTONIC_TIMEDWAIT)
    return sal_sem_monotonic_destroy(sem);
#else
    sem_ctrl_t *sc = (sem_ctrl_t *)sem;

    assert(sc);

    sem_destroy(&sc->sem);

    free(sc);
#endif
}

int
sal_sem_take(sal_sem_t sem, int usec)
{
#if (SAL_SEM_USE_MONOTONIC_TIMEDWAIT)
    return sal_sem_monotonic_take(sem, usec);
#else
    sem_ctrl_t *sc = (sem_ctrl_t *)sem;
    int rv = 0;

    if (usec == SAL_SEM_FOREVER) {
        do {
            rv = sem_wait(&sc->sem);
        } while (rv != 0 && errno == EINTR);
    } else {
        rv = _timed_sem_take(sc, usec);
    }

    return (rv != 0) ? -1 : 0;
#endif
}

int
sal_sem_give(sal_sem_t sem)
{
#if (SAL_SEM_USE_MONOTONIC_TIMEDWAIT)
    return sal_sem_monotonic_give(sem);
#else
    sem_ctrl_t *sc = (sem_ctrl_t *)sem;
    int rv = 0;
    int sem_val = 0;

    /* Binary sem only post if sem_val == 0 */
    if (sc->binary) {
        /* Post sem on getvalue failure */
        sem_getvalue(&sc->sem, &sem_val);
        if (sem_val == 0) {
            rv = sem_post(&sc->sem);
        }
    } else {
        rv = sem_post(&sc->sem);
    }

    return (rv != 0) ? -1 : 0;
#endif
}
