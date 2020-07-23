/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_SAL_H
#define BFCMAP_SAL_H
#include <buser_sal.h>

typedef buser_sal_lock_t bfcmap_sal_lock_t;

/*
 * BFCMAP_SAL_SEM_CREATE creates a bfcmap_sal_lock_t and return 
 * bfcmap_sal_lock_t back.
 */
#define BFCMAP_SAL_LOCK_CREATE(n)       BMF_SAL_LOCK_CREATE(n)

/*
 * Destroy lock.
 */
#define BFCMAP_SAL_LOCK_DESTROY(s)      BMF_SAL_LOCK_DESTROY(s)

/*
 * Take lock.
 */
#define BFCMAP_SAL_LOCK(s)              BMF_SAL_LOCK(s)

/*
 * unlock.
 */
#define BFCMAP_SAL_UNLOCK(s)            BMF_SAL_UNLOCK(s)

/*
 * memory allocation.
 */
#define BFCMAP_SAL_MALLOC(s,n)          BMF_SAL_MALLOC(s,n)

/*
 * free memory.
 */
#define BFCMAP_SAL_FREE(p)              BMF_SAL_FREE(p)

/*
 * Sleep for t microseconds
 */
#define BFCMAP_SAL_USLEEP(t)            BMF_SAL_USLEEP(t)



#endif /* BFCMAP_SAL_H */

