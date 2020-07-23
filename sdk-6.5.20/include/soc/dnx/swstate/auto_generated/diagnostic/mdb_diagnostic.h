
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_DIAGNOSTIC_H__
#define __MDB_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb_global.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    MDB_DB_INFOS_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_KEY_SIZE_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_FORMAT_COUNT_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_KEY_TO_FORMAT_MAP_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_IS_FLEX_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_BANK_SIZE_USED_MACRO_B_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_MACRO_INDEX_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_MACRO_TYPE_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_CLUSTER_MAPPING_INFO,
    MDB_DB_INFOS_EM_KEY_FORMAT_NOF_ADDRESS_BITS_INFO,
    MDB_DB_INFOS_DB_INFO,
    MDB_DB_INFOS_DB_NUMBER_OF_ALLOC_CLUSTERS_INFO,
    MDB_DB_INFOS_DB_NUMBER_OF_CLUSTERS_INFO,
    MDB_DB_INFOS_DB_NOF_CLUSTER_PER_ENTRY_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_MACRO_TYPE_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_MACRO_INDEX_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_CLUSTER_INDEX_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_CLUSTER_POSITION_IN_ENTRY_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_START_ADDRESS_INFO,
    MDB_DB_INFOS_DB_CLUSTERS_INFO_END_ADDRESS_INFO,
    MDB_DB_INFOS_CAPACITY_INFO,
    MDB_DB_INFOS_EM_ENTRY_COUNT_INFO,
    MDB_DB_INFOS_EM_ENTRY_CAPACITY_ESTIMATE_INFO,
    MDB_DB_INFOS_EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE_INFO,
    MDB_DB_INFOS_EEDB_PAYLOAD_TYPE_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_CACHE_ENABLED_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_PDB_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_FLAGS_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_BANKS_HITBIT_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_BB_HITBIT_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_RPB_HITBIT_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_DIRECT_ADDRESS_MAPPING_INFO,
    MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_ADDRESS_MAPPING_INFO,
    MDB_DB_INFOS_MAX_FEC_ID_VALUE_INFO,
    MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_INFO,
    MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_LOCAL_LIFS_INFO,
    MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_GLOBAL_LIF_DESTINATION_INFO,
    MDB_DB_INFOS_FEC_HIERARCHY_INFO_INFO,
    MDB_DB_INFOS_FEC_HIERARCHY_INFO_FEC_HIERARCHY_MAP_INFO,
    MDB_DB_INFOS_FEC_HIERARCHY_INFO_FEC_ID_RANGE_START_INFO,
    MDB_DB_INFOS_FEC_HIERARCHY_INFO_FEC_ID_RANGE_SIZE_INFO,
    MDB_DB_INFOS_MDB_PROFILE_INFO,
    MDB_DB_INFOS_MDB_EM_INIT_DONE_INFO,
    MDB_DB_INFOS_KAPS_SCAN_CFG_ON_INFO,
    MDB_DB_INFOS_KAPS_SCAN_CFG_PAUSED_INFO,
    MDB_DB_INFOS_INFO_NOF_ENTRIES
} sw_state_mdb_db_infos_layout_e;


extern dnx_sw_state_diagnostic_info_t mdb_db_infos_info[SOC_MAX_NUM_DEVICES][MDB_DB_INFOS_INFO_NOF_ENTRIES];

extern const char* mdb_db_infos_layout_str[MDB_DB_INFOS_INFO_NOF_ENTRIES];
int mdb_db_infos_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_dump(
    int unit, int em_key_format_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_key_size_dump(
    int unit, int em_key_format_idx_0, int key_size_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_format_count_dump(
    int unit, int em_key_format_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_key_to_format_map_dump(
    int unit, int em_key_format_idx_0, int key_to_format_map_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_em_bank_info_dump(
    int unit, int em_key_format_idx_0, int em_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_em_bank_info_is_flex_dump(
    int unit, int em_key_format_idx_0, int em_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_dump(
    int unit, int em_key_format_idx_0, int em_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_em_bank_info_macro_index_dump(
    int unit, int em_key_format_idx_0, int em_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_em_bank_info_macro_type_dump(
    int unit, int em_key_format_idx_0, int em_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_dump(
    int unit, int em_key_format_idx_0, int em_bank_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_key_format_nof_address_bits_dump(
    int unit, int em_key_format_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_dump(
    int unit, int db_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_number_of_alloc_clusters_dump(
    int unit, int db_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_number_of_clusters_dump(
    int unit, int db_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_nof_cluster_per_entry_dump(
    int unit, int db_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_macro_type_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_macro_index_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_cluster_index_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_cluster_position_in_entry_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_start_address_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_db_clusters_info_end_address_dump(
    int unit, int db_idx_0, int clusters_info_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_capacity_dump(
    int unit, int capacity_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_entry_count_dump(
    int unit, int em_entry_count_idx_0, int em_entry_count_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_entry_capacity_estimate_dump(
    int unit, int em_entry_capacity_estimate_idx_0, int em_entry_capacity_estimate_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_em_entry_capacity_estimate_average_dump(
    int unit, int em_entry_capacity_estimate_average_idx_0, int em_entry_capacity_estimate_average_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_eedb_payload_type_dump(
    int unit, int eedb_payload_type_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_cache_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_dump(
    int unit, int macro_cluster_assoc_idx_0, int macro_cluster_assoc_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_dump(
    int unit, int macro_cluster_assoc_idx_0, int macro_cluster_assoc_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_dump(
    int unit, int macro_cluster_assoc_idx_0, int macro_cluster_assoc_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_dump(
    int unit, int macro_cluster_cache_idx_0, int macro_cluster_cache_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_dump(
    int unit, int macro_cluster_cache_idx_0, int macro_cluster_cache_idx_1, int data_idx_0, int data_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_dump(
    int unit, int eedb_banks_hitbit_idx_0, int eedb_banks_hitbit_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_dump(
    int unit, int small_kaps_bb_hitbit_idx_0, int small_kaps_bb_hitbit_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_dump(
    int unit, int small_kaps_rpb_hitbit_idx_0, int small_kaps_rpb_hitbit_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_direct_address_mapping_dump(
    int unit, int direct_address_mapping_idx_0, int direct_address_mapping_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_dump(
    int unit, int eedb_address_mapping_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_max_fec_id_value_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_eedb_bank_traffic_lock_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_eedb_bank_traffic_lock_local_lifs_dump(
    int unit, int local_lifs_idx_0, int local_lifs_idx_1, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_dump(
    int unit, int global_lif_destination_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_fec_hierarchy_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_dump(
    int unit, int fec_hierarchy_map_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_fec_hierarchy_info_fec_id_range_start_dump(
    int unit, int fec_id_range_start_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_fec_hierarchy_info_fec_id_range_size_dump(
    int unit, int fec_id_range_size_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_profile_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_mdb_em_init_done_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_kaps_scan_cfg_on_dump(
    int unit, int kaps_scan_cfg_on_idx_0, dnx_sw_state_dump_filters_t filters);

int mdb_db_infos_kaps_scan_cfg_paused_dump(
    int unit, int kaps_scan_cfg_paused_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
