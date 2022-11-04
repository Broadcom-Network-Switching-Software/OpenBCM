
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SW_STATE_EXTERNAL_TYPES_H__
#define __SW_STATE_EXTERNAL_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnxc_sw_state_external.h>


#define DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE 100



typedef struct {
    uint32 offsets[DNXC_SW_STATE_EXTERNAL_COUNT][DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE];
} sw_state_external_t;


#endif 
