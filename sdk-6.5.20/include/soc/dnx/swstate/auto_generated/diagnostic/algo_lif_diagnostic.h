
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ALGO_LIF_DIAGNOSTIC_H__
#define __ALGO_LIF_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    LOCAL_OUTLIF_INFO_INFO,
    LOCAL_OUTLIF_INFO_DISABLE_MDB_CLUSTERS_INFO,
    LOCAL_OUTLIF_INFO_DISABLE_EEDB_DATA_BANKS_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_ASSIGNED_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_IN_USE_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_USED_LOGICAL_PHASE_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_LL_IN_USE_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_LL_INDEX_INFO,
    LOCAL_OUTLIF_INFO_OUTLIF_BANK_INFO_EEDB_DATA_BANKS_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_PHYSICAL_PHASE_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_ADDRESS_GRANULARITY_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_FIRST_BANK_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_LAST_BANK_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_FIRST_LL_BANK_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_LAST_LL_BANK_INFO,
    LOCAL_OUTLIF_INFO_LOGICAL_PHASE_INFO_DBAL_VALID_COMBINATIONS_INFO,
    LOCAL_OUTLIF_INFO_PHYSICAL_PHASE_TO_LOGICAL_PHASE_MAP_INFO,
    LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_INFO,
    LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_BANK_OCCUPATION_BITMAP_INFO,
    LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_OUTLIF_PREFIX_OCCUPATION_BITMAP_INFO,
    LOCAL_OUTLIF_INFO_EEDB_BANKS_INFO_USED_DATA_BANK_PER_LOGICAL_PHASE_INFO,
    LOCAL_OUTLIF_INFO_INFO_NOF_ENTRIES
} sw_state_local_outlif_info_layout_e;


extern dnx_sw_state_diagnostic_info_t local_outlif_info_info[SOC_MAX_NUM_DEVICES][LOCAL_OUTLIF_INFO_INFO_NOF_ENTRIES];

extern const char* local_outlif_info_layout_str[LOCAL_OUTLIF_INFO_INFO_NOF_ENTRIES];
int local_outlif_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_disable_mdb_clusters_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_disable_eedb_data_banks_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_dump(
    int unit, int outlif_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_assigned_dump(
    int unit, int outlif_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_in_use_dump(
    int unit, int outlif_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_used_logical_phase_dump(
    int unit, int outlif_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_ll_in_use_dump(
    int unit, int outlif_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_ll_index_dump(
    int unit, int outlif_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_outlif_bank_info_eedb_data_banks_dump(
    int unit, int outlif_bank_info_idx_0, int eedb_data_banks_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_physical_phase_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_address_granularity_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_first_bank_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_last_bank_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_first_ll_bank_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_last_ll_bank_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_logical_phase_info_dbal_valid_combinations_dump(
    int unit, int logical_phase_info_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_physical_phase_to_logical_phase_map_dump(
    int unit, int physical_phase_to_logical_phase_map_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_eedb_banks_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_eedb_banks_info_bank_occupation_bitmap_dump(
    int unit, int bank_occupation_bitmap_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_dump(
    int unit, int outlif_prefix_occupation_bitmap_idx_0, dnx_sw_state_dump_filters_t filters);

int local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
