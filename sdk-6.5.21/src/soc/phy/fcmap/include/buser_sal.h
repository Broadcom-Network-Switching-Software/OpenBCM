/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BMF_SAL_H
#define BMF_SAL_H

/*
 * include system specific header file for SAL.
 */
#if defined(BROADCOM_SAL)
#include <include/sal/core/alloc.h>
#include <include/sal/core/sync.h>
#include <include/sal/core/thread.h>

typedef sal_mutex_t buser_sal_lock_t;

/*
 * BMF_SAL_SEM_CREATE creates a buser_sal_lock_t and return 
 * buser_sal_lock_t back.
 */
#define BMF_SAL_LOCK_CREATE(n)       sal_mutex_create((n))

/*
 * Destroy lock.
 */
#define BMF_SAL_LOCK_DESTROY(s)      sal_mutex_destroy((s))

/*
 * Take lock.
 */
#define BMF_SAL_LOCK(s)              sal_mutex_take((s), sal_mutex_FOREVER)

/*
 * unlock.
 */
#define BMF_SAL_UNLOCK(s)            sal_mutex_give((s))

/*
 * memory allocation.
 */
#define BMF_SAL_MALLOC(s,n)          sal_alloc((s),(n))

/*
 * free memory.
 */
#define BMF_SAL_FREE(p)              sal_free((p))

/*
 * Sleep for t microseconds
 */
#define BMF_SAL_USLEEP(t)            sal_usleep((t))

#else

#error "User should provide SAL implementation"

#endif /* BROADCOM_SAL */


#endif /* BMF_SAL_H */

