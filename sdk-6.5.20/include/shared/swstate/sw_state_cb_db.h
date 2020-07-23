/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * this module is used to store and acquire all of the device lock's, semaphores, mutexes etc. 
 */
#ifndef _SW_STATE_CB_DB_H
#define _SW_STATE_CB_DB_H

#include <sal/core/sync.h>

#include <bcm_int/dpp/l2.h>

typedef struct sw_dpp_state_cb_db_s{

    /* 
     * bcm cbs
     */
    _bcm_petra_l2_cb_t  l2_cb;


    /* 
     * soc cbs
     */
    /* add dpp's soc mutexes and semaphores here */
     
} sw_dpp_state_cb_db_t;

typedef struct sw_state_cb_db_s {
    sw_dpp_state_cb_db_t  dpp;
} sw_state_cb_db_t;

extern sw_state_cb_db_t sw_state_cb_db[BCM_MAX_NUM_UNITS];

#endif /*_SW_STATE_CB_DB_H*/
