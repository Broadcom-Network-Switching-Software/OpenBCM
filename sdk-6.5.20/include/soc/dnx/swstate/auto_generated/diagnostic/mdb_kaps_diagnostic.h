
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_KAPS_DIAGNOSTIC_H__
#define __MDB_KAPS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_hb.h>
#include <libs/kaps/include/kaps_instruction.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    MDB_KAPS_DB_INFO,
    MDB_KAPS_DB_DB_INFO_INFO,
    MDB_KAPS_DB_DB_INFO_DB_P_INFO,
    MDB_KAPS_DB_DB_INFO_AD_DB_P_INFO,
    MDB_KAPS_DB_DB_INFO_HB_DB_P_INFO,
    MDB_KAPS_DB_DB_INFO_IS_VALID_INFO,
    MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INFO,
    MDB_KAPS_DB_SEARCH_INSTRUCTION_INFO_INST_P_INFO,
    MDB_KAPS_DB_BIG_KAPS_LARGE_BB_MODE_INFO,
    MDB_KAPS_DB_INFO_NOF_ENTRIES
} sw_state_mdb_kaps_db_layout_e;


extern dnx_sw_state_diagnostic_info_t mdb_kaps_db_info[SOC_MAX_NUM_DEVICES][MDB_KAPS_DB_INFO_NOF_ENTRIES];

extern const char* mdb_kaps_db_layout_str[MDB_KAPS_DB_INFO_NOF_ENTRIES];
int mdb_kaps_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_db_info_dump(
    int unit, int db_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_db_info_db_p_dump(
    int unit, int db_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_db_info_ad_db_p_dump(
    int unit, int db_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_db_info_hb_db_p_dump(
    int unit, int db_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_db_info_is_valid_dump(
    int unit, int db_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_search_instruction_info_dump(
    int unit, int search_instruction_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_search_instruction_info_inst_p_dump(
    int unit, int search_instruction_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_kaps_db_big_kaps_large_bb_mode_dump(
    int unit, int big_kaps_large_bb_mode_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
