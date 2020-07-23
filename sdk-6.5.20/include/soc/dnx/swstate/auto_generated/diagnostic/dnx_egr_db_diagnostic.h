
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EGR_DB_DIAGNOSTIC_H__
#define __DNX_EGR_DB_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_egr_db_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_EGR_DB_INFO,
    DNX_EGR_DB_PS_INFO,
    DNX_EGR_DB_PS_ALLOCATION_BMAP_INFO,
    DNX_EGR_DB_PS_PRIO_MODE_INFO,
    DNX_EGR_DB_PS_IF_IDX_INFO,
    DNX_EGR_DB_INTERFACE_OCC_INFO,
    DNX_EGR_DB_TOTAL_EGR_IF_CREDITS_INFO,
    DNX_EGR_DB_INFO_NOF_ENTRIES
} sw_state_dnx_egr_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_egr_db_info[SOC_MAX_NUM_DEVICES][DNX_EGR_DB_INFO_NOF_ENTRIES];

extern const char* dnx_egr_db_layout_str[DNX_EGR_DB_INFO_NOF_ENTRIES];
int dnx_egr_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_egr_db_ps_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_egr_db_ps_allocation_bmap_dump(
    int unit, int allocation_bmap_idx_0, int allocation_bmap_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_egr_db_ps_prio_mode_dump(
    int unit, int prio_mode_idx_0, int prio_mode_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_egr_db_ps_if_idx_dump(
    int unit, int if_idx_idx_0, int if_idx_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_egr_db_interface_occ_dump(
    int unit, int interface_occ_idx_0, int interface_occ_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_egr_db_total_egr_if_credits_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
