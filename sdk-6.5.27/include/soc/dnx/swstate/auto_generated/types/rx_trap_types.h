
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __RX_TRAP_TYPES_H__
#define __RX_TRAP_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/rx.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>


typedef struct {
    char name[BCM_RX_TRAP_MAX_NAME_LEN];
} rx_trap_user_info_t;

typedef struct {
    rx_trap_user_info_t* user_trap_info;
} rx_trap_info_t;


#endif 
