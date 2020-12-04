/*! \file sal_thread_prio.c
 *
 * SAL thread priority APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <pthread.h>

#include <sal/sal_thread.h>

/*
 * Linux scheduler policy and real-time priority used for threads
 * configured for SAL_THREAD_PRIO_INTERRUPT. Please refer to the Linux
 * documentation if you want to override these values.
 */
#ifndef SAL_SCHED_POLICY_INTERRUPT
#define SAL_SCHED_POLICY_INTERRUPT      SCHED_FIFO
#endif
#ifndef SAL_SCHED_PRIO_INTERRUPT
#define SAL_SCHED_PRIO_INTERRUPT        90
#endif

/*
 * Linux scheduler policy and real-time priority used for threads
 * configured for SAL_THREAD_PRIO_CRITICAL. Please refer to the Linux
 * documentation if you want to override these values.
 */
#ifndef SAL_SCHED_POLICY_CRITICAL
#define SAL_SCHED_POLICY_CRITICAL       SCHED_FIFO
#endif
#ifndef SAL_SCHED_PRIO_CRITICAL
#define SAL_SCHED_PRIO_CRITICAL         10
#endif

int
sal_thread_prio_set(int prio)
{
    pthread_t thread;
    struct sched_param param;

    thread = pthread_self();
    switch (prio) {
    case SAL_THREAD_PRIO_INTERRUPT:
        param.sched_priority = SAL_SCHED_PRIO_INTERRUPT;
        pthread_setschedparam(thread, SAL_SCHED_POLICY_INTERRUPT, &param);
        return 0;
    case SAL_THREAD_PRIO_CRITICAL:
        param.sched_priority = SAL_SCHED_PRIO_CRITICAL;
        pthread_setschedparam(thread, SAL_SCHED_POLICY_CRITICAL, &param);
        return 0;
    case SAL_THREAD_PRIO_DEFAULT:
    case SAL_THREAD_PRIO_LOW:
    case SAL_THREAD_PRIO_HIGH:
        param.sched_priority = 0;
        pthread_setschedparam(thread, SCHED_OTHER, &param);
        return 0;
    default:
        break;
    }
    return -1;
}

int
sal_thread_prio_get(int *prio)
{
    pthread_t thread;
    struct sched_param param;
    int policy;

    thread = pthread_self();
    if (thread && pthread_getschedparam(thread, &policy, &param) == 0) {
        if (policy == SAL_SCHED_POLICY_INTERRUPT &&
            param.sched_priority == SAL_SCHED_PRIO_INTERRUPT) {
            *prio = SAL_THREAD_PRIO_INTERRUPT;
            return 0;
        }
        if (policy == SAL_SCHED_POLICY_CRITICAL &&
            param.sched_priority == SAL_SCHED_PRIO_CRITICAL) {
            *prio = SAL_THREAD_PRIO_CRITICAL;
            return 0;
        }
        if (policy == SCHED_OTHER) {
            *prio = SAL_THREAD_PRIO_DEFAULT;
            return 0;
        }
    }
    return -1;
}
