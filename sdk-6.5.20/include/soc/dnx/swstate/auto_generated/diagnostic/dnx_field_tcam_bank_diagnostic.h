
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_BANK_DIAGNOSTIC_H__
#define __DNX_FIELD_TCAM_BANK_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIELD_TCAM_BANK_SW_INFO,
    DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_INFO,
    DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_OWNER_STAGE_INFO,
    DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_ACTIVE_HANDLERS_ID_INFO,
    DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_NOF_FREE_ENTRIES_INFO,
    DNX_FIELD_TCAM_BANK_SW_TCAM_BANKS_BANK_MODE_INFO,
    DNX_FIELD_TCAM_BANK_SW_INFO_NOF_ENTRIES
} sw_state_dnx_field_tcam_bank_sw_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_field_tcam_bank_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_BANK_SW_INFO_NOF_ENTRIES];

extern const char* dnx_field_tcam_bank_sw_layout_str[DNX_FIELD_TCAM_BANK_SW_INFO_NOF_ENTRIES];
int dnx_field_tcam_bank_sw_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_bank_sw_tcam_banks_dump(
    int unit, int tcam_banks_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_bank_sw_tcam_banks_owner_stage_dump(
    int unit, int tcam_banks_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_bank_sw_tcam_banks_active_handlers_id_dump(
    int unit, int tcam_banks_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_bank_sw_tcam_banks_nof_free_entries_dump(
    int unit, int tcam_banks_idx_0, int nof_free_entries_idx_0, int nof_free_entries_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_field_tcam_bank_sw_tcam_banks_bank_mode_dump(
    int unit, int tcam_banks_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
