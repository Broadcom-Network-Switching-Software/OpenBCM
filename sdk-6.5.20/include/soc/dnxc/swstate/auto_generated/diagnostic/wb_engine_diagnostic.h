
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __WB_ENGINE_DIAGNOSTIC_H__
#define __WB_ENGINE_DIAGNOSTIC_H__

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnxc/swstate/auto_generated/types/wb_engine_types.h>
#include <soc/wb_engine.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SW_STATE_WB_ENGINE_INFO,
    SW_STATE_WB_ENGINE_BUFFER_INFO,
    SW_STATE_WB_ENGINE_BUFFER_INSTANCE_INFO,
    SW_STATE_WB_ENGINE_INFO_NOF_ENTRIES
} sw_state_sw_state_wb_engine_layout_e;


extern dnx_sw_state_diagnostic_info_t sw_state_wb_engine_info[SOC_MAX_NUM_DEVICES][SW_STATE_WB_ENGINE_INFO_NOF_ENTRIES];

extern const char* sw_state_wb_engine_layout_str[SW_STATE_WB_ENGINE_INFO_NOF_ENTRIES];
int sw_state_wb_engine_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int sw_state_wb_engine_buffer_dump(
    int unit, int engine_id, int buffer_id, dnx_sw_state_dump_filters_t filters);

int sw_state_wb_engine_buffer_instance_dump(
    int unit, int engine_id, int buffer_id, int instance_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  

#endif 
