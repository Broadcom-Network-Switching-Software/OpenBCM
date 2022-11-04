/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _SW_STATE_LL_CALLBACKS_H
#define _SW_STATE_LL_CALLBACKS_H

#include <sal/types.h>

typedef struct
{

    uint32 entry_id_first;

    uint32 entry_id_last;

} DNX_TCAM_PRIO_LOCATION;

int32 tcam_sorted_list_cmp_priority(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

int32 sw_state_sorted_list_cmp_uint8(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

int32 sw_state_sorted_list_cmp_uint32(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

int32 sw_state_sorted_list_cmp_uint64(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

#endif
