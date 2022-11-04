
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __PP_TYPES_H__
#define __PP_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>


#define MAX_NOF_PP_KBR 3

#define MAX_NOF_KBR_SELCTORS 18



typedef struct {
    uint8 pp_stage_kbr_select[MAX_NOF_PP_KBR][MAX_NOF_KBR_SELCTORS];
} dnx_pp_sw_state_t;


#endif 
