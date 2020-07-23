
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_MUTEX_WB_H_

#define _DNXC_SW_STATE_MUTEX_WB_H_

#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/sync.h>

int dnxc_sw_state_mutex_create_wb(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * mtx,
    char *desc,
    uint32 flags);

int dnxc_sw_state_mutex_destroy_wb(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 flags);

int dnxc_sw_state_mutex_take_wb(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    int usec,
    uint32 flags);

int dnxc_sw_state_mutex_give_wb(
    int unit,
    uint32 module_id,
    sw_state_mutex_t * ptr_mtx,
    uint32 flags);

#endif
