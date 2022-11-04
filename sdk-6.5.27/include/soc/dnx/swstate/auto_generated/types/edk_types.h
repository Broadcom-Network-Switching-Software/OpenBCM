
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __EDK_TYPES_H__
#define __EDK_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>


#define EDK_SEQ_MAX 50



typedef struct {
    int* edk_seq[EDK_SEQ_MAX];
} edk_sw_state;


#endif 
