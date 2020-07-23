/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	thread.h
 * Purpose: 	SAL thread definitions
 */

#ifndef _SAL_THREAD_H
#define _SAL_THREAD_H

#include <sdk_config.h>

typedef struct sal_thread_s{
    char thread_opaque_type;
} *sal_thread_t;

#define SAL_THREAD_ERROR	((sal_thread_t) -1)
#ifndef SAL_THREAD_STKSZ
#define	SAL_THREAD_STKSZ	16384	/* Default Stack Size */
#endif

#define SAL_THREAD_NAME_MAX_LEN     80 
#define SAL_THREAD_PRIO_NO_PREEMPT  -1

extern int sal_thread_join(sal_thread_t , void **);
extern sal_thread_t sal_thread_create(char *, int, int, 
				  void (f)(void *), void *);
extern int sal_thread_destroy(sal_thread_t);
extern sal_thread_t sal_thread_self(void);
extern char* sal_thread_name(sal_thread_t thread,
				 char *thread_name, int thread_name_size);
extern void sal_thread_exit(int);
extern void sal_thread_yield(void);
extern void sal_thread_main_set(sal_thread_t thread);
extern sal_thread_t sal_thread_main_get(void);
extern int sal_thread_id_get(void);

typedef int (*thread_traverse_cb_f)(void *, int, char *);
extern void sal_thread_traverse(thread_traverse_cb_f cb_func, void *cb_date);



#define sal_msleep(x)    sal_usleep((x) * 1000)
extern void sal_sleep(int sec);
extern void sal_usleep(unsigned int usec);
extern void sal_udelay(unsigned int usec);

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
extern void
sal_thread_resource_usage_get(
                unsigned int *sal_thread_count_curr,
                unsigned int *sal_stack_size_curr,
                unsigned int *sal_thread_count_max,
                unsigned int *sal_stack_size_max);
#endif
#endif


typedef void sal_tls_key_t;

extern sal_tls_key_t * sal_tls_key_create(void (*destructor)(void *));
extern int sal_tls_key_set(sal_tls_key_t *, void *);
extern void * sal_tls_key_get(sal_tls_key_t *);
extern int sal_tls_key_delete(sal_tls_key_t *);

#endif	/* !_SAL_THREAD_H */
