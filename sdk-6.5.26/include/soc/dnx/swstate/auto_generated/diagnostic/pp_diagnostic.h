
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PP_DIAGNOSTIC_H__
#define __DNX_PP_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/pp_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    PP_DB_INFO,
    PP_DB_PP_STAGE_KBR_SELECT_INFO,
    PP_DB_INFO_NOF_ENTRIES
} sw_state_pp_db_layout_e;


extern dnx_sw_state_diagnostic_info_t pp_db_info[SOC_MAX_NUM_DEVICES][PP_DB_INFO_NOF_ENTRIES];

extern const char* pp_db_layout_str[PP_DB_INFO_NOF_ENTRIES];
int pp_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int pp_db_pp_stage_kbr_select_dump(
    int unit, int pp_stage_kbr_select_idx_0, int pp_stage_kbr_select_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
