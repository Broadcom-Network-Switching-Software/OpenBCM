
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_HARD_RESET_ACCESS_H__
#define __DNX_HARD_RESET_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_hard_reset_types.h>
#include <include/bcm/switch.h>



typedef int (*dnx_hard_reset_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_hard_reset_db_init_cb)(
    int unit);


typedef int (*dnx_hard_reset_db_hard_reset_callback_callback_set_cb)(
    int unit, bcm_switch_hard_reset_callback_t callback);


typedef int (*dnx_hard_reset_db_hard_reset_callback_callback_get_cb)(
    int unit, bcm_switch_hard_reset_callback_t *callback);


typedef int (*dnx_hard_reset_db_hard_reset_callback_userdata_set_cb)(
    int unit, dnx_hard_reset_callback_userdata_t userdata);


typedef int (*dnx_hard_reset_db_hard_reset_callback_userdata_get_cb)(
    int unit, dnx_hard_reset_callback_userdata_t *userdata);




typedef struct {
    dnx_hard_reset_db_hard_reset_callback_callback_set_cb set;
    dnx_hard_reset_db_hard_reset_callback_callback_get_cb get;
} dnx_hard_reset_db_hard_reset_callback_callback_cbs;


typedef struct {
    dnx_hard_reset_db_hard_reset_callback_userdata_set_cb set;
    dnx_hard_reset_db_hard_reset_callback_userdata_get_cb get;
} dnx_hard_reset_db_hard_reset_callback_userdata_cbs;


typedef struct {
    
    dnx_hard_reset_db_hard_reset_callback_callback_cbs callback;
    
    dnx_hard_reset_db_hard_reset_callback_userdata_cbs userdata;
} dnx_hard_reset_db_hard_reset_callback_cbs;


typedef struct {
    dnx_hard_reset_db_is_init_cb is_init;
    dnx_hard_reset_db_init_cb init;
    
    dnx_hard_reset_db_hard_reset_callback_cbs hard_reset_callback;
} dnx_hard_reset_db_cbs;





extern dnx_hard_reset_db_cbs dnx_hard_reset_db;

#endif 
