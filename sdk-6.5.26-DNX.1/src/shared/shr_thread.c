/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Common thread framework built on top of SAL API.
 *
 */

#include <assert.h>
#include <sal/sp.h>
#include <sal/generic.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <shared/shr_thread.h>
#include <shared/error.h>

/*******************************************************************************
 * Local definitions
 ******************************************************************************/

/*! Signature for sanity checks. */
#define TA_SIGNATURE   SAL_SIG_DEF('S', 'H', 'T', 'A')

/*! Check that thread attributes were properly initialized. */
#define BAD_TA(_ta)   ((_ta)->ta_sig != TA_SIGNATURE)

/*! Signature for sanity checks. */
#define TC_SIGNATURE   SAL_SIG_DEF('S', 'H', 'T', 'C')

/*! Check that this is a valid thread control object. */
#define BAD_TC(_tc)   ((_tc)->tc_sig != TC_SIGNATURE)

/* Short-hand for single-producer variables */
#define SP_GET(_sp) sal_sp_load_int(&_sp)
#define SP_PUT(_sp, _val) sal_sp_store_int(&_sp, _val)

/*!
 * Thread control object.
 */
struct shr_thread_ctrl_s {

    /*! Signature which indicates that object is initialized. */
    uint32 tc_sig;

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

    /*! Semaphore to support global suspension state. */
    sal_sem_t suspend_sem;

    /*! Thread attributes. */
    shr_thread_attr_t attr;

    /*! Thread name for debug purposes. */
    char name[32];

    /*! Non-zero if thread is active (running or done). */
    sal_sp_int active;

    /*! Non-zero if thread has been requested to stop. */
    sal_sp_int stopping;

    /*! Non-zero if thread is waiting for an event. */
    sal_sp_int waiting;

    /*! Non-zero if thread is suspended. */
    sal_sp_int suspended;

    /*! Thread function has exited and returned to thread control. */
    sal_sp_int done;

    /*! Non-zero if thread encountered an internal state error. */
    sal_sp_int error;

    /*! The OS thread ID returned from sal_thread_create(). */
    sal_thread_t tid;
};

/*******************************************************************************
 * Local data
 ******************************************************************************/

static sal_spinlock_t thread_lock;
static struct shr_thread_ctrl_s *thread_root;
static sal_sp_int bg_suspended;

/*******************************************************************************
 * Private functions
 ******************************************************************************/

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
    if (tc->suspend_sem) {
        sal_sem_destroy(tc->suspend_sem);
    }
    tc->attr.ta_sig = 0;
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

    tc->stop_sem = sal_sem_create("shrThreadStop", sal_sem_BINARY, 0);
    if (tc->stop_sem == NULL) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    tc->sleep_sem = sal_sem_create("shrThreadSleep", sal_sem_BINARY, 0);
    if (tc->sleep_sem == NULL) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    tc->suspend_sem = sal_sem_create("shrThreadSuspend", sal_sem_BINARY, 0);
    if (tc->suspend_sem == NULL) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    tc->tc_sig = TC_SIGNATURE;

    return tc;
}

static void
bg_check(struct shr_thread_ctrl_s *tc)
{
    if (tc->attr.bg) {
        while (SP_GET(bg_suspended) && !SP_GET(tc->stopping)) {
            SP_PUT(tc->suspended, TRUE);
            sal_sem_take(tc->suspend_sem, sal_sem_FOREVER);
        }
        SP_PUT(tc->suspended, FALSE);
    }
}

static void
tc_thread(void *context)
{
    struct shr_thread_ctrl_s *tc = (shr_thread_t)context;

    bg_check(tc);

    if (tc->thread_func) {
        SP_PUT(tc->active, TRUE);
        if (!SP_GET(tc->stopping)) {
            tc->thread_func(tc, tc->thread_arg);
        }
        SP_PUT(tc->done, TRUE);
        /*
         * Wait for controlling thread if we terminated without a stop
         * request.
         */
        while (!SP_GET(tc->stopping)) {
            shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        }
        SP_PUT(tc->active, FALSE);
        /* Signal controlling thread */
        sal_sem_give(tc->stop_sem);
    }
}

/*******************************************************************************
 * Public functions
 ******************************************************************************/

void
shr_thread_attr_init(shr_thread_attr_t *attr)
{
    if (attr) {
        sal_memset(attr, 0, sizeof(*attr));
        attr->ta_sig = TA_SIGNATURE;
        attr->prio = -1;
        attr->inst = -1;
        attr->stksz = -1;
    }
}

shr_thread_t
shr_thread_attr_start(char *name, shr_thread_attr_t *attr,
                      shr_thread_func_f func, void *arg)
{
    sal_thread_t tid;
    shr_thread_attr_t thread_attr;
    struct shr_thread_ctrl_s *tc;

    if (attr == NULL) {
        attr = &thread_attr;
        shr_thread_attr_init(attr);
    }
    if (BAD_TA(attr)) {
        return NULL;
    }

    tc = thread_ctrl_create();
    if (tc == NULL) {
        return NULL;
    }

    /* Copy attributes to thread control object */
    sal_memcpy(&tc->attr, attr, sizeof(tc->attr));
    attr = &tc->attr;

    /* Initialize uninitialized attributes */
    if (attr->prio < 0) {
        attr->prio = SAL_THREAD_PRIO_DEFAULT;
    }
    if (attr->stksz < 0) {
        attr->stksz = SHR_THREAD_STKSZ;
    }

    /* Set thread name (truncate as needed) */
    sal_strlcpy(tc->name, name, sizeof(tc->name));

    /* Optionally append instance number to thread name */
    if (attr->inst >= 0) {
        int len, max, offs;
        char buf[32];

        /* Adding instance string may truncate thread name */
        len = sal_snprintf(buf, sizeof(buf), ".%d", attr->inst) + 1;
        max = sizeof(tc->name) - len;
        if (max > 0) {
            offs = sal_strlen(tc->name);
            if (offs > max) {
                offs = max;
            }
            sal_strlcpy(&tc->name[offs], buf, len);
        }
    }

    tc->thread_func = func;
    tc->thread_arg = arg;

    tid = sal_thread_create(tc->name, attr->stksz, attr->prio, tc_thread, tc);
    if (tid == SAL_THREAD_ERROR) {
        thread_ctrl_destroy(tc);
        return NULL;
    }
    tc->tid = tid;

    return tc;
}

shr_thread_t
shr_thread_start(char *name, int prio, shr_thread_func_f func, void *arg)
{
    shr_thread_attr_t thread_attr;

    shr_thread_attr_init(&thread_attr);
    thread_attr.prio = prio;

    return shr_thread_attr_start(name, &thread_attr, func, arg);
}

int
shr_thread_stop(shr_thread_t th, sal_usecs_t usec)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc) {
        if (BAD_TC(tc)) {
            assert(0);
            return _SHR_E_INTERNAL;
        }
        SP_PUT(tc->stopping, TRUE);
        if (SP_GET(tc->suspended)) {
            sal_sem_give(tc->suspend_sem);
        }
        if (usec == 0) {
            /* Do not wait */
            return _SHR_E_NONE;
        }
        /* Wake up thread */
        sal_sem_give(tc->sleep_sem);
        /* Wait for thread to stop */
        sal_sem_take(tc->stop_sem, usec);
        if (SP_GET(tc->active)) {
            /* Thread did not stop in time */
            SP_PUT(tc->error, TRUE);
            return _SHR_E_TIMEOUT;
        }
        thread_ctrl_destroy(tc);
    }
    return _SHR_E_NONE;
}

int
shr_thread_stopping(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && SP_GET(tc->stopping)) {
        return TRUE;
    }
    return FALSE;
}

int
shr_thread_is_self(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (!tc ||  SP_GET(tc->done) || tc->tc_sig != TC_SIGNATURE) {
        return FALSE;
    }
    return (tc->tid == sal_thread_self());
}

int
shr_thread_done(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && SP_GET(tc->done)) {
        return TRUE;
    }
    return FALSE;
}

int
shr_thread_sleep(shr_thread_t th, sal_usecs_t usec)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && tc->sleep_sem) {
        shr_thread_waiting_set(tc, TRUE);
        sal_sem_take(tc->sleep_sem, usec);
        shr_thread_waiting_set(tc, FALSE);
    }
    return _SHR_E_NONE;
}

int
shr_thread_wake(shr_thread_t th)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc && tc->sleep_sem) {
        sal_sem_give(tc->sleep_sem);
    }
    return _SHR_E_NONE;
}

int
shr_thread_waiting_set(shr_thread_t th, int waiting)
{
    struct shr_thread_ctrl_s *tc = (struct shr_thread_ctrl_s *)th;

    if (tc) {
        SP_PUT(tc->waiting, waiting);
        bg_check(tc);
    }
    return _SHR_E_NONE;
}

void
shr_thread_bg_suspend(void)
{
    SP_PUT(bg_suspended, TRUE);
}

void
shr_thread_bg_resume(void)
{
    struct shr_thread_ctrl_s *tc;

    SP_PUT(bg_suspended, FALSE);

    /* Kick all suspended background threads */
    if (thread_lock) {
        tc = thread_root;
        sal_spinlock_lock(thread_lock);
        while (tc) {
            if (SP_GET(tc->suspended)) {
                sal_sem_give(tc->suspend_sem);
            }
            tc = tc->next;
        }
        sal_spinlock_unlock(thread_lock);
    }
}

int
shr_thread_bg_suspended(void)
{
    struct shr_thread_ctrl_s *tc;
    int all_bg_suspended = SP_GET(bg_suspended);

    /* Look for background threads that are not suspended */
    if (thread_lock) {
        tc = thread_root;
        sal_spinlock_lock(thread_lock);
        while (tc) {
            if (tc->attr.bg && !SP_GET(tc->suspended)) {
                all_bg_suspended = FALSE;
                break;
            }
            tc = tc->next;
        }
        sal_spinlock_unlock(thread_lock);
    }
    return all_bg_suspended;
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
            shr_pb_printf(pb, "%c", SP_GET(tc->active) ? 'A' : '-');
            shr_pb_printf(pb, "%c", SP_GET(tc->stopping) ? 'S' : '-');
            shr_pb_printf(pb, "%c", SP_GET(tc->waiting) ? 'W' : '-');
            shr_pb_printf(pb, "%c", SP_GET(tc->suspended) ? 's' : '-');
            shr_pb_printf(pb, "%c", SP_GET(tc->done) ? 'D' : '-');
            shr_pb_printf(pb, "%c", SP_GET(tc->error) ? 'E' : '-');
            shr_pb_printf(pb, "%c", tc->attr.bg ? 'b' : '-');
            shr_pb_printf(pb, " %s\n", tc->name);
        } else {
            break;
        }
    }
    return next_idx;
}
