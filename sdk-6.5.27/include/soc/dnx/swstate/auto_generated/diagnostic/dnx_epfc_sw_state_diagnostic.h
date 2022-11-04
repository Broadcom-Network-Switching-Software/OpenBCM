
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EPFC_SW_STATE_DIAGNOSTIC_H__
#define __DNX_EPFC_SW_STATE_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_epfc_sw_state_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_EPFC_INTERFACE_DB_INFO,
    DNX_EPFC_INTERFACE_DB_IS_INITIALIZED_INFO,
    DNX_EPFC_INTERFACE_DB_UC_NUM_INFO,
    DNX_EPFC_INTERFACE_DB_INFO_NOF_ENTRIES
} sw_state_dnx_epfc_interface_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_epfc_interface_db_info[SOC_MAX_NUM_DEVICES][DNX_EPFC_INTERFACE_DB_INFO_NOF_ENTRIES];

extern const char* dnx_epfc_interface_db_layout_str[DNX_EPFC_INTERFACE_DB_INFO_NOF_ENTRIES];
int dnx_epfc_interface_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_epfc_interface_db_is_initialized_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_epfc_interface_db_uc_num_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
