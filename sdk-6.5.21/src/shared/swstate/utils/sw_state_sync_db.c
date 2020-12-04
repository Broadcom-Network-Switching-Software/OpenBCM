/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * this module is used to store and acquire all of the device lock's, semaphores, mutexes etc. 
 */


#include <shared/swstate/sw_state_sync_db.h>

sw_state_sync_db_t sw_state_sync_db[BCM_MAX_NUM_UNITS];
