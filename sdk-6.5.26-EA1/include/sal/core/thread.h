/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: 	thread.h
 * Purpose: 	SAL thread definitions
 */

#ifndef _SAL_THREAD_H
#define _SAL_THREAD_H

#include <sal/compiler.h>
#include <sdk_config.h>

typedef struct sal_thread_s{
    char thread_opaque_type;
} *sal_thread_t;

#define SAL_THREAD_ERROR	((sal_thread_t) -1)
/*!
 * Default stack size for threads created via \ref sal_thread_create.
 *
 * Some SDK threads allow invocation of CINT functions via callbacks,
 * which in turn may print variables. Execution of "print <variable>;"
 * is handled by cint_variable_print() which may allocate upto 3
 * buffers of size CINT_CONFIG_MAX_STACK_PRINT_BUFFER on the stack,
 * and may be called recursively when structures have other structures
 * or arrays as members.
 *
 * With CINT_CONFIG_MAX_STACK_PRINT_BUFFER currently set to ~5KB,
 * assuming up to 10 recursive calls, adding the on-stack space thus
 * required to the current default stack size of 16KB and rounding up
 * to nearest multiple of 16KB we arrive at 176KB as the default
 * stack size for SDK threads.
 */
#ifndef SAL_THREAD_STKSZ
#define	SAL_THREAD_STKSZ	180224 /* Default Stack Size */
#endif

#define SAL_THREAD_NAME_MAX_LEN     80 

/*!
 * \brief Run thread at highest acceptable priority.
 *
 * The primary use of this priority level is for user threads
 * executing interrupt handlers.
 */
#define SAL_THREAD_PRIO_NO_PREEMPT      -1

/*!
 * \brief Run thread at default priority.
 *
 * To be used for all threads with no special scheduling needs.
 * Most SDK threads will use this value.
 */
#ifndef SAL_THREAD_PRIO_DEFAULT
#define SAL_THREAD_PRIO_DEFAULT         50
#endif

/*!
 * \brief Run thread at background priority.
 *
 * Used for threads, which only have to run occasionally, e.g. to
 * check overall system health.
 *
 * Support for this priority is optional, i.e. it may fall back to the
 * system default priority, if system support is unavailable.
 */
#ifndef SAL_THREAD_PRIO_LOW
#define SAL_THREAD_PRIO_LOW             40
#endif

/*!
 * \brief Run thread at elevated priority.
 *
 * Used for threads, which cannot be stalled for an extended amount of
 * time, e.g. to prevent hardware counter overruns.
 *
 * Support for this priority is optional, i.e. it may fall back to the
 * system default priority, if system support is unavailable.
 */
#ifndef SAL_THREAD_PRIO_HIGH
#define SAL_THREAD_PRIO_HIGH            60
#endif

/*!
 * \brief Run thread at high priority.
 *
 * This priority should make a thread run before all normal threads,
 * and it will typically be used for time-sensitive application
 * callbacks.
 */
#ifndef SAL_THREAD_PRIO_CRITICAL
#define SAL_THREAD_PRIO_CRITICAL        80
#endif

/*!
 * \brief Run thread at interrupt priority.
 *
 * This priority should make a thread run before all other threads,
 * with the exception of threads with the same priority.
 */
#ifndef SAL_THREAD_PRIO_INTERRUPT
#define SAL_THREAD_PRIO_INTERRUPT       SAL_THREAD_PRIO_NO_PREEMPT
#endif

extern int sal_thread_join(sal_thread_t , void **) SAL_ATTR_WEAK;
extern sal_thread_t sal_thread_create(char *, int, int, 
				  void (f)(void *), void *) SAL_ATTR_WEAK;
extern int sal_thread_destroy(sal_thread_t) SAL_ATTR_WEAK;
extern sal_thread_t sal_thread_self(void) SAL_ATTR_WEAK;
extern char* sal_thread_name(sal_thread_t thread,
				 char *thread_name, int thread_name_size) SAL_ATTR_WEAK;
extern void sal_thread_exit(int) SAL_ATTR_WEAK;
extern void sal_thread_yield(void) SAL_ATTR_WEAK;
extern void sal_thread_main_set(sal_thread_t thread) SAL_ATTR_WEAK;
extern sal_thread_t sal_thread_main_get(void) SAL_ATTR_WEAK;
extern int sal_thread_id_get(void) SAL_ATTR_WEAK;
extern int sal_thread_prio_set(int prio) SAL_ATTR_WEAK;
extern int sal_thread_prio_get(int *prio) SAL_ATTR_WEAK;

typedef int (*thread_traverse_cb_f)(void *, int, char *);
extern void sal_thread_traverse(thread_traverse_cb_f cb_func, void *cb_date) SAL_ATTR_WEAK;


#define sal_msleep(x)    sal_usleep((x) * 1000)
extern void sal_sleep(int sec) SAL_ATTR_WEAK;
extern void sal_usleep(unsigned int usec) SAL_ATTR_WEAK;
extern void sal_udelay(unsigned int usec) SAL_ATTR_WEAK;

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
extern void
sal_thread_resource_usage_get(
                unsigned int *sal_thread_count_curr,
                unsigned int *sal_stack_size_curr,
                unsigned int *sal_thread_count_max,
                unsigned int *sal_stack_size_max) SAL_ATTR_WEAK;
#endif
#endif


typedef void sal_tls_key_t;

extern sal_tls_key_t * sal_tls_key_create(void (*destructor)(void *)) SAL_ATTR_WEAK;
extern int sal_tls_key_set(sal_tls_key_t *, void *) SAL_ATTR_WEAK;
extern void * sal_tls_key_get(sal_tls_key_t *) SAL_ATTR_WEAK;
extern int sal_tls_key_delete(sal_tls_key_t *) SAL_ATTR_WEAK;

extern void print_process_info(int pid) SAL_ATTR_WEAK;

#endif	/* !_SAL_THREAD_H */
