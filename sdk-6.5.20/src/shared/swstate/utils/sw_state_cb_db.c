/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * this module is used to store and retrieve all callback functions. 
 * callback functions are not stored in the global sw_state structure as they 
 * cannot be automatically recovered during warm reboot
 *
 * This module seems to only be relevant for DPP
 */
#if defined(BCM_PETRA_SUPPORT)
/* { */
#include <shared/swstate/sw_state_cb_db.h>

sw_state_cb_db_t sw_state_cb_db[BCM_MAX_NUM_UNITS];
/* } */
#else
int sw_state_cb_db_cfile;
#endif

