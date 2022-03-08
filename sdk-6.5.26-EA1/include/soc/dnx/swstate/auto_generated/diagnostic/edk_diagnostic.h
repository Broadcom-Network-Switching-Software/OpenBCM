
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EDK_DIAGNOSTIC_H__
#define __DNX_EDK_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/edk_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    EDK_STATE_INFO,
    EDK_STATE_EDK_SEQ_INFO,
    EDK_STATE_INFO_NOF_ENTRIES
} sw_state_edk_state_layout_e;


extern dnx_sw_state_diagnostic_info_t edk_state_info[SOC_MAX_NUM_DEVICES][EDK_STATE_INFO_NOF_ENTRIES];

extern const char* edk_state_layout_str[EDK_STATE_INFO_NOF_ENTRIES];
int edk_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int edk_state_edk_seq_dump(
    int unit, int edk_seq_idx_0, int edk_seq_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
