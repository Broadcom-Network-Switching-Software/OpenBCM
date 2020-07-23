/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        lock.c
 * Purpose:     SAL Global lock
 */

#include <sal/core/libc.h>
#include <sal/core/sync.h>

/* SAL Global Lock */
sal_mutex_t                     sal_global_lock = NULL;
/* APPL Global Lock */
sal_mutex_t                     appl_global_lock = NULL;

int
sal_global_lock_init(void)
{
    /* Initialize CM System lock */
    if (sal_global_lock == NULL) {
        sal_global_lock = sal_mutex_create("SAL Global Lock");
        if (sal_global_lock == NULL) {
            return -1;
        }
    }

    return 0;
}

int
sal_appl_global_lock_init(void)
{
    /* Initialize APPL System lock */
    if (appl_global_lock == NULL) {
        appl_global_lock = sal_mutex_create("APPL Global Lock");
        if (appl_global_lock == NULL) {
            return -1;
        }
    }

    return 0;
}
