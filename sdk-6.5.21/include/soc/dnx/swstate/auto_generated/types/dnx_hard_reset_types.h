
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_HARD_RESET_TYPES_H__
#define __DNX_HARD_RESET_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/switch.h>


typedef sw_state_user_def_t* dnx_hard_reset_callback_userdata_t;



typedef struct {
    
    bcm_switch_hard_reset_callback_t callback;
    
    dnx_hard_reset_callback_userdata_t userdata;
} dnx_hard_reset_t;


typedef struct {
    
    dnx_hard_reset_t hard_reset_callback;
} dnx_hard_reset_db_t;


#endif 
