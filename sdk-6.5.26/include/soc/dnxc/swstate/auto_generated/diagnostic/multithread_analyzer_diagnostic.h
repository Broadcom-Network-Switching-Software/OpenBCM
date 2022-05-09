
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXC_MULTITHREAD_ANALYZER_DIAGNOSTIC_H__
#define __DNXC_MULTITHREAD_ANALYZER_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_index_hash_table.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    MULTITHREAD_ANALYZER_INFO,
    MULTITHREAD_ANALYZER_HTB_INFO,
    MULTITHREAD_ANALYZER_INFO_NOF_ENTRIES
} sw_state_multithread_analyzer_layout_e;


extern dnx_sw_state_diagnostic_info_t multithread_analyzer_info[SOC_MAX_NUM_DEVICES][MULTITHREAD_ANALYZER_INFO_NOF_ENTRIES];

extern const char* multithread_analyzer_layout_str[MULTITHREAD_ANALYZER_INFO_NOF_ENTRIES];
int multithread_analyzer_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int multithread_analyzer_htb_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
