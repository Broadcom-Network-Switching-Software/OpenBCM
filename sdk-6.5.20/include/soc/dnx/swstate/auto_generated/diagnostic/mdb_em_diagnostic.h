
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_EM_DIAGNOSTIC_H__
#define __MDB_EM_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_em_types.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    MDB_EM_DB_INFO,
    MDB_EM_DB_SHADOW_EM_DB_INFO,
    MDB_EM_DB_VMV_INFO_INFO,
    MDB_EM_DB_VMV_INFO_VALUE_INFO,
    MDB_EM_DB_VMV_INFO_SIZE_INFO,
    MDB_EM_DB_VMV_INFO_ENCODING_MAP_INFO,
    MDB_EM_DB_VMV_INFO_ENCODING_MAP_ENCODING_INFO,
    MDB_EM_DB_VMV_INFO_ENCODING_MAP_SIZE_INFO,
    MDB_EM_DB_VMV_INFO_APP_ID_SIZE_INFO,
    MDB_EM_DB_VMV_INFO_MAX_PAYLOAD_SIZE_INFO,
    MDB_EM_DB_MACT_STAMP_INFO,
    MDB_EM_DB_INFO_NOF_ENTRIES
} sw_state_mdb_em_db_layout_e;


extern dnx_sw_state_diagnostic_info_t mdb_em_db_info[SOC_MAX_NUM_DEVICES][MDB_EM_DB_INFO_NOF_ENTRIES];

extern const char* mdb_em_db_layout_str[MDB_EM_DB_INFO_NOF_ENTRIES];
int mdb_em_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_shadow_em_db_dump(
    int unit, int shadow_em_db_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_dump(
    int unit, int vmv_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_value_dump(
    int unit, int vmv_info_idx_0, int value_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_size_dump(
    int unit, int vmv_info_idx_0, int size_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_encoding_map_dump(
    int unit, int vmv_info_idx_0, int encoding_map_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_encoding_map_encoding_dump(
    int unit, int vmv_info_idx_0, int encoding_map_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_encoding_map_size_dump(
    int unit, int vmv_info_idx_0, int encoding_map_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_app_id_size_dump(
    int unit, int vmv_info_idx_0, int app_id_size_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_vmv_info_max_payload_size_dump(
    int unit, int vmv_info_idx_0, int max_payload_size_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_em_db_mact_stamp_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
