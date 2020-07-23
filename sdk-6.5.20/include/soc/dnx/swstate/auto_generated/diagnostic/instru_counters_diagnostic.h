
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __INSTRU_COUNTERS_DIAGNOSTIC_H__
#define __INSTRU_COUNTERS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/instru_counters_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    INSTRU_SYNC_COUNTERS_INFO,
    INSTRU_SYNC_COUNTERS_ICGM_COUNTES_INFO,
    INSTRU_SYNC_COUNTERS_ICGM_COUNTES_SYNC_COUNTERS_STARTED_INFO,
    INSTRU_SYNC_COUNTERS_ICGM_COUNTES_TOD_1588_TIMESTAMP_START_INFO,
    INSTRU_SYNC_COUNTERS_ICGM_COUNTES_PER_CORE_INFO_INFO,
    INSTRU_SYNC_COUNTERS_ICGM_COUNTES_PER_CORE_INFO_RECORD_LAST_READ_NOF_INFO,
    INSTRU_SYNC_COUNTERS_NIF_COUNTES_INFO,
    INSTRU_SYNC_COUNTERS_NIF_COUNTES_SYNC_COUNTERS_STARTED_INFO,
    INSTRU_SYNC_COUNTERS_NIF_COUNTES_TOD_1588_TIMESTAMP_START_INFO,
    INSTRU_SYNC_COUNTERS_NIF_COUNTES_PER_ETHU_INFO_INFO,
    INSTRU_SYNC_COUNTERS_NIF_COUNTES_PER_ETHU_INFO_RECORD_LAST_READ_NOF_INFO,
    INSTRU_SYNC_COUNTERS_INFO_NOF_ENTRIES
} sw_state_instru_sync_counters_layout_e;


extern dnx_sw_state_diagnostic_info_t instru_sync_counters_info[SOC_MAX_NUM_DEVICES][INSTRU_SYNC_COUNTERS_INFO_NOF_ENTRIES];

extern const char* instru_sync_counters_layout_str[INSTRU_SYNC_COUNTERS_INFO_NOF_ENTRIES];
int instru_sync_counters_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_icgm_countes_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_icgm_countes_sync_counters_started_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_icgm_countes_tod_1588_timestamp_start_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_icgm_countes_per_core_info_dump(
    int unit, int per_core_info_idx_0, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_dump(
    int unit, int per_core_info_idx_0, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_nif_countes_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_nif_countes_sync_counters_started_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_nif_countes_tod_1588_timestamp_start_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_nif_countes_per_ethu_info_dump(
    int unit, int per_ethu_info_idx_0, int per_ethu_info_idx_1, dnx_sw_state_dump_filters_t filters);

int instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_dump(
    int unit, int per_ethu_info_idx_0, int per_ethu_info_idx_1, int record_last_read_nof_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
