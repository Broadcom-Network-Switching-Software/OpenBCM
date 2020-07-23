/*! \file shr_thread.c
 *
 * Common thread framework built on top of SAL API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_internal.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_thread.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_assert.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

/*! Signature for sanity checks. */
#define TC_SIGNATURE   SAL_SIG_DEF('S', 'H', 'T', 'C')

/*! Check that this is a valid thread control object. */
#define BAD_TC(_tc)   ((_tc)->tc_sig != TC_SIGNATURE)

/*!
 * Thread control object.
 */
struct shr_thread_ctrl_s {

    /*! Signature which indicates that object is initialized. */
    uint32_t tc_sig;

    /*! Next thread in global thread list. */
    struct shr_thread_ctrl_s *next;

    /*! Application-provided function to call from thread context. */
    shr_thread_func_f *thread_func;

    /*! Application-provided context for the thread function. */
    void *thread_arg;

    /*! Semaphore to synchronize thread stop/exit flow. */
    sal_sem_t stop_sem;

    /*! Semaphore to allow thread to wait for an event. */
    sal_sem_t sleep_sem;

    /*! Thread name for debug purposes. */
    char name[32];

    /*! Non-zero if thread is active (running or done). */
    int active;

    /*! Non-zero if thread has been requested to stop. */
    int stopping;

    /*! Non-zero if thread is waiting for an event. */
    int waiting;

    /*! Thread function has exited and returned to thread control. */
    int done;

    /*! Non-zero if thread encountered an internal state error. */
    int error;

};

static sal_spinlock_t thread_lock;
static struct shr_thread_ctrl_s *thread_root;

static void
thread_ctrl_destroy(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;
    struct shr_thread_ctrl_s **list;

    if (tc == NULL) {
        return;
    }
    if (BAD_TC(tc)) {
        assert(0);
        return;
    }
    if (thread_lock == NULL) {
        return;
    }

    sal_spinlock_lock(thread_lock);
    list = &thread_root;
    while (*list) {
        if (*list == tc) {
            *list = tc->next;
            break;
        }
        list = &((*list)->next);
    }
    sal_spinlock_unlock(thread_lock);

    if (thread_root == NULL) {
        sal_spinlock_destroy(thread_lock);
        thread_lock = NULL;
    }

    if (tc->stop_sem) {
        sal_sem_destroy(tc->stop_sem);
    }
    if (tc->sleep_sem) {
        sal_sem_destroy(tc->sleep_sem);
    }
    tc->tc_sig = 0;
    sal_free(tc);
}

static shr_thread_t
thread_ctrl_create(void)
{
    struct shr_thread_ctrl_s *tc;

    if (thread_root == NULL) {
        thread_lock = sal_spinlock_create("shrThread");
    }
    if (thread_lock == NULL) {
        return NULL;
    }

    tc =  sal_alloc(sizeof(*tc), "shrThread");
    if (tc == NULL) {
        return NULL;
    }
    sal_memset(tc, 0, sizeof(*tc));

    sal_spinlock_lock(thread_lock);
    tc->next = thread_root;
    thread_root = tc;
    sal_spinlock_unlock(thread_lock);

    tc->stop_sem = sal_sem_create("shrThreadStop", SAL_SEM_BINARY, 0);
    if (tc->stop_sem == NULL) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    tc->sleep_sem = sal_sem_create("shrThreadSleep", SAL_SEM_BINARY, 0);
    if (tc->sleep_sem == NULL) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    tc->tc_sig = TC_SIGNATURE;

    return tc;
}

static void
shr_thread(void *context)
{
    struct shr_thread_ctrl_s *tc = (shr_thread_t)context;

    if (tc->thread_func) {
        tc->active = TRUE;
        tc->thread_func(tc, tc->thread_arg);
        tc->done = TRUE;
        /*
         * Wait for controlling thread if we terminated without a stop request.
         */
        while (!tc->stopping) {
            shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        }
        tc->active = FALSE;
        /* Signal controlling thread */
        sal_sem_give(tc->stop_sem);
    }
}

shr_thread_t
shr_thread_start(char *name, int prio, shr_thread_func_f func, void *arg)
{
    sal_thread_t tid;
    struct shr_thread_ctrl_s *tc;

    if (prio < 0) {
        prio = SAL_THREAD_PRIO_DEFAULT;
    }

    tc = thread_ctrl_create();
    if (tc == NULL) {
        return NULL;
    }
    tc->thread_func = func;
    tc->thread_arg = arg;
    sal_strlcpy(tc->name, name, sizeof(tc->name));

    tid = sal_thread_create(name, SHR_THREAD_STKSZ, prio, shr_thread, tc);
    if (tid == SAL_THREAD_ERROR) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    return tc;
}

int
shr_thread_stop(shr_thread_t th, sal_usecs_t usecs)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc) {
        if (BAD_TC(tc)) {
            assert(0);
            return SHR_E_INTERNAL;
        }
        tc->stopping = TRUE;
        if (usecs == 0) {
            /* Do not wait */
            return SHR_E_NONE;
        }
        /* Wake up thread */
        sal_sem_give(tc->sleep_sem);
        /* Wait for thread to stop */
        sal_sem_take(tc->stop_sem, usecs);
        if (tc->active) {
            /* Thread did not stop in time */
            tc->error = TRUE;
            return SHR_E_TIMEOUT;
        }
        thread_ctrl_destroy(tc);
    }
    return SHR_E_NONE;
}

int
shr_thread_stopping(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && tc->stopping) {
        return TRUE;
    }
    return FALSE;
}

int
shr_thread_done(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && tc->done) {
        return TRUE;
    }
    return FALSE;
}

int
shr_thread_sleep(shr_thread_t th, sal_usecs_t usecs)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && tc->sleep_sem) {
        tc->waiting = TRUE;
        if (sal_sem_take(tc->sleep_sem, usecs) < 0) {
            return SHR_E_FAIL;
        }
        tc->waiting = FALSE;
    }
    return SHR_E_NONE;
}

int
shr_thread_wake(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && tc->sleep_sem) {
        sal_sem_give(tc->sleep_sem);
    }
    return SHR_E_NONE;
}

int
shr_thread_dump(shr_pb_t *pb)
{
    struct shr_thread_ctrl_s tmp_tc;
    struct shr_thread_ctrl_s *tc;
    int idx, next_idx;

    next_idx = 0;
    while (thread_lock) {
        tc = thread_root;
        idx = 0;
        sal_spinlock_lock(thread_lock);
        while (tc) {
            if (idx++ == next_idx) {
                next_idx++;
                sal_memcpy(&tmp_tc, tc, sizeof(tmp_tc));
                tc = &tmp_tc;
                break;
            }
            tc = tc->next;
        }
        sal_spinlock_unlock(thread_lock);
        if (tc) {
            shr_pb_printf(pb, "%c", tc->active ? 'A' : '-');
            shr_pb_printf(pb, "%c", tc->stopping ? 'S' : '-');
            shr_pb_printf(pb, "%c", tc->waiting ? 'W' : '-');
            shr_pb_printf(pb, "%c", tc->done ? 'D' : '-');
            shr_pb_printf(pb, "%c", tc->error ? 'E' : '-');
            shr_pb_printf(pb, " %s\n", tc->name);
        } else {
            break;
        }
    }
    return next_idx;
}
