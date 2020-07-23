/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains locking definitions internal to the BCM library.
 */

#ifndef _BCM_INT_LOCK_H
#define _BCM_INT_LOCK_H

#include <bcm/types.h>

#include <sal/core/sync.h>

/*
 * BCM Locking
 *
 * (1) BCM calls that need to perform multiple operations on a resource
 *     atomically, such as read/modify/write a table entry, should lock
 *     the resource.
 *
 * (2) Module routines that call outside their module must be very
 *     careful to avoid deadlocks.  Deadlocks result from two or more
 *     threads trying to take the same locks but in a different order.
 *
 * (3) BCM_LOCK should be taken during all major configuration changes
 *     to prevent corruption that could result if two tasks (like a CLI
 *     and a SNMP server) try to alter configuration simultaneously.
 *
 * (4) BCM_LOCK may also be useful in avoiding deadlocks if multiple
 *     resources are needed.
 */

extern sal_mutex_t _bcm_lock[BCM_MAX_NUM_UNITS];

#define BCM_LOCK(unit) \
        sal_mutex_take(_bcm_lock[unit], sal_mutex_FOREVER)

#define BCM_UNLOCK(unit) \
        sal_mutex_give(_bcm_lock[unit])

#endif	/* !_BCM_INT_LOCK_H */
