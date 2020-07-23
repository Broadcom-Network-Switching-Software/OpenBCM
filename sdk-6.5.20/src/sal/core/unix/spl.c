/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	spl.c
 * Purpose:	Interrupt Blocking
 */
#if defined(LINUX) || defined(linux) || defined(__linux__)
#define _XOPEN_SOURCE 600
#endif
#include <sys/types.h>
#include <signal.h>
#include <assert.h>
#include <sal/core/sync.h>
#include <pthread.h>

/*
 *  To improve spl lock performance, pthread_mutex_lock/pthread_mutex_unlock
 *  is used by default.
 *  Define SAL_SPL_USE_SAL_MUTEX to use sal_mutex on the spl lock.
 */
#ifdef SAL_SPL_USE_SAL_MUTEX
static sal_mutex_t spl_mutex;
#define SAL_SPL_LOCK_INIT(m) m = sal_mutex_create("spl mutex")
#define SAL_SPL_LOCK(m)      sal_mutex_take(m, sal_mutex_FOREVER)
#define SAL_SPL_UNLOCK(m)    sal_mutex_give(m)
#else
static pthread_mutex_t spl_mutex;
#define SAL_SPL_LOCK_INIT(m) \
    do {\
        pthread_mutexattr_t attr;\
        pthread_mutexattr_init(&attr);\
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);\
        pthread_mutex_init(&m, &attr);\
    } while(0)
#define SAL_SPL_LOCK(m)      pthread_mutex_lock(&m)
#define SAL_SPL_UNLOCK(m)    pthread_mutex_unlock(&m)
#endif /* SAL_SPL_USE_SAL_MUTEX */

static int spl_level;

/*
 * Function:
 *	sal_spl_init
 * Purpose:
 *	Initialize the synchronization portion of the SAL
 * Returns:
 *	0 on success, -1 on failure
 */

int
sal_spl_init(void)
{
    /* Initialize spl */
    SAL_SPL_LOCK_INIT(spl_mutex);
    spl_level = 0;

    return 0;
}

/*
 * Function:
 *	sal_spl
 * Purpose:
 *	Set interrupt level.
 * Parameters:
 *	level - interrupt level to set.
 * Returns:
 *	Value of interrupt level in effect prior to the call.
 * Notes:
 *	Used most often to restore interrupts blocked by sal_splhi.
 */

int
sal_spl(int level)
{
#ifdef SAL_SPL_NO_PREEMPT
    struct sched_param param;
    int policy;

    if (pthread_getschedparam(pthread_self(), &policy, &param) == 0) {
        /* Interrupt thread uses SCHED_RR and should be left alone */
        if (policy != SCHED_RR) {
            /* setting sched_priority to 0 will only change the real time priority
               of the thread. For a non real time thread it should be 0. This is
               very Linux specific.
             */  
            param.sched_priority = 0;
            pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
        }
    }
#endif
    assert(level == spl_level);
    spl_level--;
    SAL_SPL_UNLOCK(spl_mutex);
    return 0;
}

/*
 * Function:
 *	sal_splhi
 * Purpose:
 *	Set interrupt mask to highest level.
 * Parameters:
 *	None
 * Returns:
 *	Value of interrupt level in effect prior to the call.
 */

int
sal_splhi(void)
{
#ifdef SAL_SPL_NO_PREEMPT
    struct sched_param param;
    int policy;

    if (pthread_getschedparam(pthread_self(), &policy, &param) == 0) {
        /* Interrupt thread uses SCHED_RR and should be left alone */
        if (policy != SCHED_RR) {
            param.sched_priority = 90;
            pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        }
    }
#endif
    SAL_SPL_LOCK(spl_mutex);
    return ++spl_level;
}

/*
 * Function:
 *	sal_int_context
 * Purpose:
 *	Return TRUE if running in interrupt context.
 * Parameters:
 *	None
 * Returns:
 *	Boolean
 */

int
sal_int_context(void)
{
    /* Must be provided by the virtual interrupt controller */
    extern int intr_int_context(void) __attribute__((weak)); 
    
    if (intr_int_context) {
        return intr_int_context();
    } else {
        return 0;
    }
}

/*
 * Function:
 *	sal_no_sleep
 * Purpose:
 *	Return TRUE if current context cannot sleep.
 * Parameters:
 *	None
 * Returns:
 *	Boolean
 */

int
sal_no_sleep(void)
{
    return 0;
}
