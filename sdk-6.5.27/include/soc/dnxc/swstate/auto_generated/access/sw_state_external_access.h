
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SW_STATE_EXTERNAL_ACCESS_H__
#define __SW_STATE_EXTERNAL_ACCESS_H__

#include <soc/dnxc/swstate/auto_generated/types/sw_state_external_types.h>


typedef int (*sw_state_external_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*sw_state_external_init_cb)(
    int unit);

typedef int (*sw_state_external_offsets_set_cb)(
    int unit, uint32 offsets_idx_0, uint32 offsets_idx_1, uint32 offsets);

typedef int (*sw_state_external_offsets_get_cb)(
    int unit, uint32 offsets_idx_0, uint32 offsets_idx_1, uint32 *offsets);



typedef struct {
    sw_state_external_offsets_set_cb set;
    sw_state_external_offsets_get_cb get;
} sw_state_external_offsets_cbs;

typedef struct {
    sw_state_external_is_init_cb is_init;
    sw_state_external_init_cb init;
    sw_state_external_offsets_cbs offsets;
} sw_state_external_cbs;





extern sw_state_external_cbs sw_state_external;

#endif 
