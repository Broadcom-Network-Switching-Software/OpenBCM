
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __EDK_ACCESS_H__
#define __EDK_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/edk_types.h>


typedef int (*edk_state_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*edk_state_init_cb)(
    int unit);

typedef int (*edk_state_edk_seq_set_cb)(
    int unit, uint32 edk_seq_idx_0, uint32 edk_seq_idx_1, int edk_seq);

typedef int (*edk_state_edk_seq_get_cb)(
    int unit, uint32 edk_seq_idx_0, uint32 edk_seq_idx_1, int *edk_seq);

typedef int (*edk_state_edk_seq_alloc_cb)(
    int unit, uint32 edk_seq_idx_0, uint32 nof_instances_to_alloc_1);



typedef struct {
    edk_state_edk_seq_set_cb set;
    edk_state_edk_seq_get_cb get;
    edk_state_edk_seq_alloc_cb alloc;
} edk_state_edk_seq_cbs;

typedef struct {
    edk_state_is_init_cb is_init;
    edk_state_init_cb init;
    edk_state_edk_seq_cbs edk_seq;
} edk_state_cbs;





extern edk_state_cbs edk_state;

#endif 
