
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __RX_TRAP_ACCESS_H__
#define __RX_TRAP_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/rx_trap_types.h>


typedef int (*rx_trap_info_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*rx_trap_info_init_cb)(
    int unit);

typedef int (*rx_trap_info_user_trap_info_set_cb)(
    int unit, uint32 user_trap_info_idx_0, CONST rx_trap_user_info_t *user_trap_info);

typedef int (*rx_trap_info_user_trap_info_get_cb)(
    int unit, uint32 user_trap_info_idx_0, rx_trap_user_info_t *user_trap_info);

typedef int (*rx_trap_info_user_trap_info_alloc_cb)(
    int unit);



typedef struct {
    rx_trap_info_user_trap_info_set_cb set;
    rx_trap_info_user_trap_info_get_cb get;
    rx_trap_info_user_trap_info_alloc_cb alloc;
} rx_trap_info_user_trap_info_cbs;

typedef struct {
    rx_trap_info_is_init_cb is_init;
    rx_trap_info_init_cb init;
    rx_trap_info_user_trap_info_cbs user_trap_info;
} rx_trap_info_cbs;





extern rx_trap_info_cbs rx_trap_info;

#endif 
