
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_SEM_PLAIN_H_

#define _DNXC_SW_STATE_SEM_PLAIN_H_

#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/sync.h>

int dnxc_sw_state_sem_create(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int is_binary,
    int initial_count,
    char *desc,
    uint32 flags);

int dnxc_sw_state_sem_destroy(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 flags);

int dnxc_sw_state_sem_take(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    int usec,
    uint32 flags);

int dnxc_sw_state_sem_give(
    int unit,
    uint32 module_id,
    sw_state_sem_t * ptr_sem,
    uint32 flags);

#endif
