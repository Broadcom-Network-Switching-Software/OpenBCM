
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIFODMA_DIAGNOSTIC_H__
#define __DNX_FIFODMA_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_fifodma_types.h>
#include <include/bcm_int/dnx/l2/l2_learn.h>
#include <include/bcm_int/dnx/sat/sat.h>
#include <include/soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_FIFODMA_INFO_INFO,
    DNX_FIFODMA_INFO_ENTRY_INFO,
    DNX_FIFODMA_INFO_ENTRY_MAX_ENTRIES_INFO,
    DNX_FIFODMA_INFO_ENTRY_IS_MEM_INFO,
    DNX_FIFODMA_INFO_ENTRY_MEM_INFO,
    DNX_FIFODMA_INFO_ENTRY_REG_INFO,
    DNX_FIFODMA_INFO_ENTRY_ARRAY_INDEX_INFO,
    DNX_FIFODMA_INFO_ENTRY_COPYNO_INFO,
    DNX_FIFODMA_INFO_ENTRY_FORCE_ENTRY_SIZE_INFO,
    DNX_FIFODMA_INFO_ENTRY_THRESHOLD_INFO,
    DNX_FIFODMA_INFO_ENTRY_TIMEOUT_INFO,
    DNX_FIFODMA_INFO_ENTRY_IS_POLL_INFO,
    DNX_FIFODMA_INFO_ENTRY_ENTRY_SIZE_INFO,
    DNX_FIFODMA_INFO_ENTRY_IS_ALLOCED_INFO,
    DNX_FIFODMA_INFO_ENTRY_IS_STARTED_INFO,
    DNX_FIFODMA_INFO_ENTRY_IS_PAUSED_INFO,
    DNX_FIFODMA_INFO_ENTRY_HANDLER_INFO,
    DNX_FIFODMA_INFO_ENTRY_LAST_ENTRY_ID_INFO,
    DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_LO_INFO,
    DNX_FIFODMA_INFO_ENTRY_HOST_BUFF_HI_INFO,
    DNX_FIFODMA_INFO_INFO_NOF_ENTRIES
} sw_state_dnx_fifodma_info_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_fifodma_info_info[SOC_MAX_NUM_DEVICES][DNX_FIFODMA_INFO_INFO_NOF_ENTRIES];

extern const char* dnx_fifodma_info_layout_str[DNX_FIFODMA_INFO_INFO_NOF_ENTRIES];
int dnx_fifodma_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_max_entries_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_is_mem_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_mem_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_reg_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_array_index_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_copyno_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_force_entry_size_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_threshold_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_timeout_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_is_poll_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_entry_size_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_is_alloced_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_is_started_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_is_paused_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_handler_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_last_entry_id_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_host_buff_lo_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_fifodma_info_entry_host_buff_hi_dump(
    int unit, int entry_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
