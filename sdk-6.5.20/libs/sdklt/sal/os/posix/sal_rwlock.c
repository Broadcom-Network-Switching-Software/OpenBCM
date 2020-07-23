/*! \file sal_rwlock.c
 *
 * Read/write lock API.
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
#include <sal/sal_rwlock.h>

#define SAL_RWLOCK_SIGNATURE   0x8482a9d0

/* Local abstract type built on the POSIX rwlock */
typedef struct rwlock_ctrl_s {
    uint32_t signature;
    pthread_rwlock_t rwlock;
    const char *desc;
} rwlock_ctrl_t;

static int
timed_calc(uint32_t usec, struct timespec *ts)
{
#if defined(CLOCK_REALTIME)
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        return -1;
    }
#else
    /* we use time if realtime clock is not available */
    ts->tv_sec = time(NULL)+1; /* round up fractions of sec */
#endif

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;
    ts->tv_nsec += (usec % SECOND_USEC) * 1000;
    /* currect overflow - if occured */
    if (ts->tv_nsec > SECOND_NSEC) {
        /* can only have overflow of 1 sec */
        ts->tv_sec++;
        ts->tv_nsec -= SECOND_NSEC;
    }
    return 0;
}

sal_rwlock_t
sal_rwlock_create(const char *desc)
{
    rwlock_ctrl_t *mc;

    if ((mc = malloc(sizeof(rwlock_ctrl_t))) == NULL) {
        return NULL;
    }

    mc->desc = desc;   /* save the name for debug usage */
    mc->signature = SAL_RWLOCK_SIGNATURE;
    /* initialize with the default attributes */
    if (pthread_rwlock_init(&mc->rwlock, NULL) != 0) {
        free(mc);
        return NULL;
    }

    return (sal_rwlock_t)mc;
}

void
sal_rwlock_destroy(sal_rwlock_t rwlock)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return;
    }
    pthread_rwlock_destroy(&mc->rwlock);
    free(mc);
}

int
sal_rwlock_rlock(sal_rwlock_t rwlock, uint32_t usec)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    int rv = -1;
    struct timespec ts;

    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return -1;
    }
    if (usec == SAL_RWLOCK_FOREVER) {
        rv = pthread_rwlock_rdlock(&mc->rwlock);
    } else if (usec == SAL_RWLOCK_NOWAIT) {
        rv = pthread_rwlock_tryrdlock(&mc->rwlock);
    } else {
        if (timed_calc(usec, &ts) == 0) {
            rv = pthread_rwlock_timedrdlock(&mc->rwlock, &ts);
        }
    }
    return (rv != 0 ? -1 : 0);
}

int
sal_rwlock_wlock(sal_rwlock_t rwlock, uint32_t usec)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    int rv = -1;
    struct timespec ts;

    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return -1;
    }
    if (usec == SAL_RWLOCK_FOREVER) {
        rv = pthread_rwlock_wrlock(&mc->rwlock);
    } else if (usec == SAL_RWLOCK_NOWAIT) {
        rv = pthread_rwlock_trywrlock(&mc->rwlock);
    } else {
        if (timed_calc(usec, &ts) == 0) {
            rv = pthread_rwlock_timedwrlock(&mc->rwlock, &ts);
        }
    }
    return (rv != 0 ? -1 : 0);
}

int
sal_rwlock_give(sal_rwlock_t rwlock)
{
    rwlock_ctrl_t *mc = (rwlock_ctrl_t *)rwlock;
    int rv;

    if (!mc || (mc->signature != SAL_RWLOCK_SIGNATURE)) {
        return -1;
    }
    rv = pthread_rwlock_unlock(&mc->rwlock);
    return (rv != 0 ? -1 : 0);
}

