
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_STK_SYS_DIAGNOSTIC_H__
#define __DNX_STK_SYS_DIAGNOSTIC_H__

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_stk_sys_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_STK_SYS_DB_INFO,
    DNX_STK_SYS_DB_NOF_FAP_IDS_INFO,
    DNX_STK_SYS_DB_MODULE_ENABLE_DONE_INFO,
    DNX_STK_SYS_DB_INFO_NOF_ENTRIES
} sw_state_dnx_stk_sys_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_stk_sys_db_info[SOC_MAX_NUM_DEVICES][DNX_STK_SYS_DB_INFO_NOF_ENTRIES];

extern const char* dnx_stk_sys_db_layout_str[DNX_STK_SYS_DB_INFO_NOF_ENTRIES];
int dnx_stk_sys_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_stk_sys_db_nof_fap_ids_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_stk_sys_db_module_enable_done_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 
#endif  

#endif 
