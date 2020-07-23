
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * this module is used to store and acquire all of the device lock's, semaphores, mutexes etc. 
 */
#ifndef _SW_STATE_SYNC_DB_H
#define _SW_STATE_SYNC_DB_H

#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <bcm/types.h>

typedef struct sw_state_dpp_sync_db_counter_s {
    sal_thread_t    background;    /* unit background thread */
    sal_sem_t       bgSem;         /* unit background waiting sem */
    sal_mutex_t     cacheLock;     /* cache access locking */
} sw_state_dpp_sync_db_counter_t;

typedef struct sw_state_dpp_sync_db_s{

    /* 
     * bcm locks
     */
    sal_mutex_t stg_lock;
    sal_mutex_t policer_lock;
    sal_mutex_t mirror_lock;
    sal_mutex_t l3_lock;
    sal_mutex_t l2_lock;
    sal_mutex_t trunk_lock;
    sw_state_dpp_sync_db_counter_t counter;
    sal_mutex_t unitLock;  /* unit lock */ /* Moved from bcm_dpp_field_info_OLD_t */
    /* 
     * soc locks
     */
    /* add dpp's soc mutexes and semaphores here */

    /* 
     * shared locks
     */
    /* add shared mutexes and semaphores here */
    sal_mutex_t *htb_locks;

} sw_state_dpp_sync_db_t;

typedef struct sw_state_sync_db_s {
    sw_state_dpp_sync_db_t  dpp;
} sw_state_sync_db_t;

extern sw_state_sync_db_t sw_state_sync_db[BCM_MAX_NUM_UNITS];

#endif /*_SW_STATE_SYNC_DB_H*/

