/*! \file sal_thread.c
 *
 * Thread API.
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
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/param.h>
#include <sys/prctl.h>

#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>

#ifndef SAL_THREAD_USE_SCHED_YIELD
#  if defined(_POSIX_PRIORITY_SCHEDULING)
#    if (_POSIX_PRIORITY_SCHEDULING >= 200112L)
#      define SAL_THREAD_USE_SCHED_YIELD        1
#    endif
#  endif
#endif
#ifndef SAL_THREAD_USE_SCHED_YIELD
#define SAL_THREAD_USE_SCHED_YIELD              0
#endif

/* Optionally dump configuration */
#include <sal/sal_internal.h>
#if SAL_INT_DUMP_CONFIG
#pragma message(SAL_INT_VAR_VALUE(SAL_THREAD_USE_SCHED_YIELD))
#endif

#if (SAL_THREAD_USE_SCHED_YIELD)
#define SCHED_YIELD() sched_yield()
#else
#define SCHED_YIELD() sal_usleep(1)
#endif

static pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;

#define THREAD_LOCK() pthread_mutex_lock(&thread_lock)
#define THREAD_UNLOCK() pthread_mutex_unlock(&thread_lock)

/*
 * PTHREAD_STACK_MIN must be defined to avoid stack corruption.
 * Normally this is accomplished by including sys/param.h.
 */
#ifndef PTHREAD_STACK_MIN
#  ifdef PTHREAD_STACK_SIZE
#    define PTHREAD_STACK_MIN PTHREAD_STACK_SIZE
#  else
#    error PTHREAD_STACK_MIN not defined
#  endif
#endif

#ifndef SAL_THREAD_NAME_MAX
#define SAL_THREAD_NAME_MAX             32
#endif

typedef struct thread_info_s {
    char name[SAL_THREAD_NAME_MAX];
    void (*func)(void *);
    void *arg;
    pthread_t id;
    int stksz;
    int prio;
    sal_sem_t sem;
    struct thread_info_s *next;
} thread_info_t;

static thread_info_t *thread_head = NULL;

static void *
thread_boot(void *ti_void)
{
    thread_info_t *ti = ti_void;
    void (*func)(void *);
    void *arg;
    sigset_t new_mask, orig_mask;

    /* Make sure no child thread catches Control-C */
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &new_mask, &orig_mask);

    /* Ensure that we give up all resources upon exit */
    pthread_detach(pthread_self());

    sal_thread_prio_set(ti->prio);

#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, ti->name, 0, 0, 0);
#endif

    func = ti->func;
    arg = ti->arg;

    ti->id = pthread_self();

    /* Notify parent to continue */
    sal_sem_give(ti->sem);

    /* Call thread function */
    (*func)(arg);

    /* Thread function did not call sal_thread_exit() */
    sal_thread_exit(0);

    /* Will never get here */
    return NULL;
}

sal_thread_t
sal_thread_create(char *name, int stksz, int prio,
                  sal_thread_func_f func, void *arg)
{
    pthread_attr_t attribs;
    thread_info_t *ti;
    pthread_t id;
    sal_sem_t sem;

    if (pthread_attr_init(&attribs)) {
        return SAL_THREAD_ERROR;
    }

    stksz += PTHREAD_STACK_MIN;
    pthread_attr_setstacksize(&attribs, stksz);

    if ((ti = malloc(sizeof (*ti))) == NULL) {
        return SAL_THREAD_ERROR;
    }

    if ((sem = sal_sem_create("threadBoot", 1, 0)) == NULL) {
        free(ti);
        return SAL_THREAD_ERROR;
    }
    strncpy(ti->name, name, sizeof(ti->name));
    ti->name[sizeof(ti->name) - 1] = 0;
    ti->func = func;
    ti->arg = arg;
    ti->id = (pthread_t)0;
    ti->stksz = stksz;
    ti->prio = prio;
    ti->sem = sem;

    THREAD_LOCK();
    ti->next = thread_head;
    thread_head = ti;
    THREAD_UNLOCK();

    if (pthread_create(&id, &attribs, thread_boot, (void *)ti)) {
        THREAD_LOCK();
        thread_head = thread_head->next;
        THREAD_UNLOCK();
        free(ti);
        sal_sem_destroy(sem);
        return SAL_THREAD_ERROR;
    }

    /*
     * Note that at this point ti can no longer be safely
     * dereferenced, as the thread we just created may have exited
     * already. Instead we wait for the new thread to update
     * thread_info_t and tell us to continue.
     */
    sal_sem_take(sem, SAL_SEM_FOREVER);
    sal_sem_destroy(sem);

    return ((sal_thread_t)id);
}

sal_thread_t
sal_thread_self(void)
{
    return (sal_thread_t)pthread_self();
}

char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size)
{
    thread_info_t *ti;
    char *name;

    name = NULL;

    THREAD_LOCK();
    for (ti = thread_head; ti != NULL; ti = ti->next) {
        if (ti->id == (pthread_t)thread) {
            strncpy(thread_name, ti->name, thread_name_size);
            thread_name[thread_name_size - 1] = 0;
            name = thread_name;
            break;
        }
    }
    THREAD_UNLOCK();

    if (name == NULL) {
        thread_name[0] = 0;
    }

    return name;
}

void
sal_thread_exit(int rc)
{
    thread_info_t *ti, **tp;
    pthread_t id = pthread_self();

    ti = NULL;

    THREAD_LOCK();
    for (tp = &thread_head; (*tp) != NULL; tp = &(*tp)->next) {
        if ((*tp)->id == id) {
            ti = (*tp);
            (*tp) = (*tp)->next;
            break;
        }
    }
    THREAD_UNLOCK();

    if (ti) {
        free(ti);
    }

    pthread_exit((void *)(unsigned long)rc);
}

void
sal_thread_yield(void)
{
    SCHED_YIELD();
}
