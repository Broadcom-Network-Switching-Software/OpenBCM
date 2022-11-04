/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: 	sync.h
 * Purpose: 	SAL thread definitions
 *
 * Note: the SAL mutex abstraction is required to allow the same mutex
 * to be taken recursively by the same thread without deadlock.
 */

#ifndef _SAL_SYNC_H
#define _SAL_SYNC_H

#include <sal/compiler.h>

typedef struct sal_mutex_s{
    char mutex_opaque_type;
} *sal_mutex_t;

typedef struct sal_sem_s{
    char sal_opaque_type;
} *sal_sem_t;

typedef struct sal_spinlock_s {
    char spinlock_opaque_type;
} *sal_spinlock_t;

#define sal_mutex_FOREVER	(-1)
#define sal_mutex_NOWAIT	0

#define sal_sem_FOREVER		(-1)
#define sal_sem_BINARY		1
#define sal_sem_COUNTING	0

extern sal_mutex_t sal_mutex_create(char *desc) SAL_ATTR_WEAK;
extern void sal_mutex_destroy(sal_mutex_t m) SAL_ATTR_WEAK;

/*#define BROADCOM_DEBUG_MUTEX*/

#ifndef BROADCOM_DEBUG_MUTEX
extern int sal_mutex_take(sal_mutex_t m, int usec) SAL_ATTR_WEAK;
extern int sal_mutex_give(sal_mutex_t m) SAL_ATTR_WEAK;
#endif

extern sal_sem_t sal_sem_create(char *desc, int binary, int initial_count) SAL_ATTR_WEAK;
extern void sal_sem_destroy(sal_sem_t b) SAL_ATTR_WEAK;
extern int sal_sem_take(sal_sem_t b, int usec) SAL_ATTR_WEAK;
extern int sal_sem_give(sal_sem_t b) SAL_ATTR_WEAK;
#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
extern void
sal_sem_resource_usage_get(unsigned int *sem_curr, unsigned int *sem_max) SAL_ATTR_WEAK;
extern void
sal_mutex_resource_usage_get(unsigned int *mutex_curr, unsigned int *mutex_max) SAL_ATTR_WEAK;
#endif
#endif



#ifdef BROADCOM_DEBUG_MUTEX
extern void sal_mutex_dbg_dump(void) SAL_ATTR_WEAK;

extern int sal_mutex_take_bcm_debug(sal_mutex_t m, int usec, const char *take_loc, int line) SAL_ATTR_WEAK;
extern int sal_mutex_give_bcm_debug(sal_mutex_t m, const char *give_loc, int line) SAL_ATTR_WEAK;


#define sal_mutex_take(MUTEX, WAIT_TIME) sal_mutex_take_bcm_debug(MUTEX, WAIT_TIME, __FILE__, __LINE__)
#define sal_mutex_give(MUTEX) sal_mutex_give_bcm_debug(MUTEX, __FILE__ , __LINE__)


/*
extern int sal_mutex_take_bcm_debug(sal_mutex_t m, int usec, const char *take_loc) SAL_ATTR_WEAK;
extern int sal_mutex_give_bcm_debug(sal_mutex_t m, const char *) SAL_ATTR_WEAK;


#define sal_mutex_take(MUTEX, WAIT_TIME) sal_mtx_take_bcm_debug(MUTEX, WAIT_TIME, __FILE__)

#define sal_mutex_give(MUTEX) sal_mutex_give_bcm_debug(MUTEX, __LINE__ )
*/

#endif

/* SAL Global Lock - SDK internal use only */
extern sal_mutex_t         sal_global_lock;
int sal_global_lock_init(void) SAL_ATTR_WEAK;
#define SAL_GLOBAL_LOCK \
    do { \
        if (sal_global_lock) { \
            sal_mutex_take(sal_global_lock, sal_mutex_FOREVER);\
        } \
    } while (0)
#define SAL_GLOBAL_UNLOCK \
    do { \
        if (sal_global_lock) { \
            sal_mutex_give(sal_global_lock);\
        } \
    } while (0)

extern sal_mutex_t         appl_global_lock;
int sal_appl_global_lock_init(void) SAL_ATTR_WEAK;
#define APPL_GLOBAL_LOCK \
    do { \
        if (appl_global_lock) { \
            sal_mutex_take(appl_global_lock, sal_mutex_FOREVER);\
        } \
    } while (0)
#define APPL_GLOBAL_UNLOCK \
    do { \
        if (appl_global_lock) { \
            sal_mutex_give(appl_global_lock);\
        } \
    } while (0)

extern sal_spinlock_t sal_spinlock_create(char *desc) SAL_ATTR_WEAK;
extern int sal_spinlock_destroy(sal_spinlock_t lock) SAL_ATTR_WEAK;
extern int sal_spinlock_lock(sal_spinlock_t lock) SAL_ATTR_WEAK;
extern int sal_spinlock_unlock(sal_spinlock_t lock) SAL_ATTR_WEAK;

#endif	/* !_SAL_SYNC_H */
