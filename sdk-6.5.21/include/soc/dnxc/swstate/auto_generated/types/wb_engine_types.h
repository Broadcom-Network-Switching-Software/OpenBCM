
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __WB_ENGINE_TYPES_H__
#define __WB_ENGINE_TYPES_H__

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/wb_engine.h>


typedef struct {
    
    DNX_SW_STATE_BUFF* instance;
} sw_state_wb_engine_buffer_t;

typedef struct {
    
    sw_state_wb_engine_buffer_t* buffer[SOC_WB_ENGINE_NOF];
} sw_state_wb_engine_t;

#endif  

#endif 
