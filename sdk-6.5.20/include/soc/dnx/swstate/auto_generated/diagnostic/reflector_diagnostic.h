
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __REFLECTOR_DIAGNOSTIC_H__
#define __REFLECTOR_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/reflector_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    REFLECTOR_SW_DB_INFO_INFO,
    REFLECTOR_SW_DB_INFO_UC_INFO,
    REFLECTOR_SW_DB_INFO_UC_ENCAP_ID_INFO,
    REFLECTOR_SW_DB_INFO_UC_IS_ALLOCATED_INFO,
    REFLECTOR_SW_DB_INFO_SBFD_REFLECTOR_CNT_INFO,
    REFLECTOR_SW_DB_INFO_INFO_NOF_ENTRIES
} sw_state_reflector_sw_db_info_layout_e;


extern dnx_sw_state_diagnostic_info_t reflector_sw_db_info_info[SOC_MAX_NUM_DEVICES][REFLECTOR_SW_DB_INFO_INFO_NOF_ENTRIES];

extern const char* reflector_sw_db_info_layout_str[REFLECTOR_SW_DB_INFO_INFO_NOF_ENTRIES];
int reflector_sw_db_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int reflector_sw_db_info_uc_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int reflector_sw_db_info_uc_encap_id_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int reflector_sw_db_info_uc_is_allocated_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int reflector_sw_db_info_sbfd_reflector_cnt_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
