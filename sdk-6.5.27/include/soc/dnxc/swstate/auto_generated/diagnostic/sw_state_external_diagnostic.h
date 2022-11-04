
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXC_SW_STATE_EXTERNAL_DIAGNOSTIC_H__
#define __DNXC_SW_STATE_EXTERNAL_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/auto_generated/types/sw_state_external_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    SW_STATE_EXTERNAL_INFO,
    SW_STATE_EXTERNAL_OFFSETS_INFO,
    SW_STATE_EXTERNAL_INFO_NOF_ENTRIES
} sw_state_sw_state_external_layout_e;


extern dnx_sw_state_diagnostic_info_t sw_state_external_info[SOC_MAX_NUM_DEVICES][SW_STATE_EXTERNAL_INFO_NOF_ENTRIES];

extern const char* sw_state_external_layout_str[SW_STATE_EXTERNAL_INFO_NOF_ENTRIES];
int sw_state_external_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int sw_state_external_offsets_dump(
    int unit, int offsets_idx_0, int offsets_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
