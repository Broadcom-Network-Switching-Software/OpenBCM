
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_ACCESS_H__
#define __MDB_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb_global.h>



typedef int (*mdb_db_infos_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*mdb_db_infos_init_cb)(
    int unit);


typedef int (*mdb_db_infos_em_key_format_set_cb)(
    int unit, uint32 em_key_format_idx_0, CONST em_key_format_info *em_key_format);


typedef int (*mdb_db_infos_em_key_format_get_cb)(
    int unit, uint32 em_key_format_idx_0, em_key_format_info *em_key_format);


typedef int (*mdb_db_infos_em_key_format_key_size_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 key_size_idx_0, uint8 key_size);


typedef int (*mdb_db_infos_em_key_format_key_size_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 key_size_idx_0, uint8 *key_size);


typedef int (*mdb_db_infos_em_key_format_format_count_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint8 format_count);


typedef int (*mdb_db_infos_em_key_format_format_count_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint8 *format_count);


typedef int (*mdb_db_infos_em_key_format_key_to_format_map_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 key_to_format_map_idx_0, uint32 key_to_format_map);


typedef int (*mdb_db_infos_em_key_format_key_to_format_map_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 key_to_format_map_idx_0, uint32 *key_to_format_map);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_is_flex_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 is_flex);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_is_flex_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 *is_flex);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 bank_size_used_macro_b);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 *bank_size_used_macro_b);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_macro_index_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 macro_index);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_macro_index_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 *macro_index);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_macro_type_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, mdb_macro_types_e macro_type);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_macro_type_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, mdb_macro_types_e *macro_type);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 cluster_mapping);


typedef int (*mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 *cluster_mapping);


typedef int (*mdb_db_infos_em_key_format_nof_address_bits_set_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 nof_address_bits);


typedef int (*mdb_db_infos_em_key_format_nof_address_bits_get_cb)(
    int unit, uint32 em_key_format_idx_0, uint32 *nof_address_bits);


typedef int (*mdb_db_infos_db_number_of_alloc_clusters_set_cb)(
    int unit, uint32 db_idx_0, int number_of_alloc_clusters);


typedef int (*mdb_db_infos_db_number_of_alloc_clusters_get_cb)(
    int unit, uint32 db_idx_0, int *number_of_alloc_clusters);


typedef int (*mdb_db_infos_db_number_of_clusters_set_cb)(
    int unit, uint32 db_idx_0, int number_of_clusters);


typedef int (*mdb_db_infos_db_number_of_clusters_get_cb)(
    int unit, uint32 db_idx_0, int *number_of_clusters);


typedef int (*mdb_db_infos_db_nof_cluster_per_entry_set_cb)(
    int unit, uint32 db_idx_0, int nof_cluster_per_entry);


typedef int (*mdb_db_infos_db_nof_cluster_per_entry_get_cb)(
    int unit, uint32 db_idx_0, int *nof_cluster_per_entry);


typedef int (*mdb_db_infos_db_clusters_info_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, CONST mdb_cluster_info_t *clusters_info);


typedef int (*mdb_db_infos_db_clusters_info_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, mdb_cluster_info_t *clusters_info);


typedef int (*mdb_db_infos_db_clusters_info_alloc_cb)(
    int unit, uint32 db_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*mdb_db_infos_db_clusters_info_macro_type_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, mdb_macro_types_e macro_type);


typedef int (*mdb_db_infos_db_clusters_info_macro_type_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, mdb_macro_types_e *macro_type);


typedef int (*mdb_db_infos_db_clusters_info_macro_index_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 macro_index);


typedef int (*mdb_db_infos_db_clusters_info_macro_index_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 *macro_index);


typedef int (*mdb_db_infos_db_clusters_info_cluster_index_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 cluster_index);


typedef int (*mdb_db_infos_db_clusters_info_cluster_index_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 *cluster_index);


typedef int (*mdb_db_infos_db_clusters_info_cluster_position_in_entry_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 cluster_position_in_entry);


typedef int (*mdb_db_infos_db_clusters_info_cluster_position_in_entry_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 *cluster_position_in_entry);


typedef int (*mdb_db_infos_db_clusters_info_start_address_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, int start_address);


typedef int (*mdb_db_infos_db_clusters_info_start_address_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, int *start_address);


typedef int (*mdb_db_infos_db_clusters_info_end_address_set_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, int end_address);


typedef int (*mdb_db_infos_db_clusters_info_end_address_get_cb)(
    int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, int *end_address);


typedef int (*mdb_db_infos_capacity_set_cb)(
    int unit, uint32 capacity_idx_0, int capacity);


typedef int (*mdb_db_infos_capacity_get_cb)(
    int unit, uint32 capacity_idx_0, int *capacity);


typedef int (*mdb_db_infos_em_entry_count_set_cb)(
    int unit, uint32 em_entry_count_idx_0, uint32 em_entry_count_idx_1, int em_entry_count);


typedef int (*mdb_db_infos_em_entry_count_get_cb)(
    int unit, uint32 em_entry_count_idx_0, uint32 em_entry_count_idx_1, int *em_entry_count);


typedef int (*mdb_db_infos_em_entry_capacity_estimate_set_cb)(
    int unit, uint32 em_entry_capacity_estimate_idx_0, uint32 em_entry_capacity_estimate_idx_1, int em_entry_capacity_estimate);


typedef int (*mdb_db_infos_em_entry_capacity_estimate_get_cb)(
    int unit, uint32 em_entry_capacity_estimate_idx_0, uint32 em_entry_capacity_estimate_idx_1, int *em_entry_capacity_estimate);


typedef int (*mdb_db_infos_em_entry_capacity_estimate_average_set_cb)(
    int unit, uint32 em_entry_capacity_estimate_average_idx_0, uint32 em_entry_capacity_estimate_average_idx_1, int em_entry_capacity_estimate_average);


typedef int (*mdb_db_infos_em_entry_capacity_estimate_average_get_cb)(
    int unit, uint32 em_entry_capacity_estimate_average_idx_0, uint32 em_entry_capacity_estimate_average_idx_1, int *em_entry_capacity_estimate_average);


typedef int (*mdb_db_infos_eedb_payload_type_set_cb)(
    int unit, uint32 eedb_payload_type_idx_0, uint32 eedb_payload_type);


typedef int (*mdb_db_infos_eedb_payload_type_get_cb)(
    int unit, uint32 eedb_payload_type_idx_0, uint32 *eedb_payload_type);


typedef int (*mdb_db_infos_mdb_cluster_infos_cache_enabled_set_cb)(
    int unit, uint8 cache_enabled);


typedef int (*mdb_db_infos_mdb_cluster_infos_cache_enabled_get_cb)(
    int unit, uint8 *cache_enabled);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_alloc_cb)(
    int unit, uint32 macro_cluster_assoc_idx_0, uint32 nof_instances_to_alloc_1);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_set_cb)(
    int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, dbal_enum_value_field_mdb_physical_table_e pdb);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_get_cb)(
    int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, dbal_enum_value_field_mdb_physical_table_e *pdb);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_set_cb)(
    int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, uint8 flags);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_get_cb)(
    int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, uint8 *flags);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_alloc_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 nof_instances_to_alloc_1);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_set_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 data_idx_0, uint32 data_idx_1, uint32 data);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_get_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 data_idx_0, uint32 data_idx_1, uint32 *data);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_read_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 data_idx_0, uint32 nof_elements, uint32 *dest);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_write_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 data_idx_0, uint32 nof_elements, const uint32 *source);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_fill_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 data_idx_0, uint32 from_idx, uint32 nof_elements, uint32 value);


typedef int (*mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_alloc_cb)(
    int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 nof_instances_to_alloc_0);


typedef int (*mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_set_cb)(
    int unit, uint32 eedb_banks_hitbit_idx_0, uint32 eedb_banks_hitbit_idx_1, uint32 eedb_banks_hitbit);


typedef int (*mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_get_cb)(
    int unit, uint32 eedb_banks_hitbit_idx_0, uint32 eedb_banks_hitbit_idx_1, uint32 *eedb_banks_hitbit);


typedef int (*mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_alloc_cb)(
    int unit, uint32 eedb_banks_hitbit_idx_0);


typedef int (*mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_set_cb)(
    int unit, uint32 small_kaps_bb_hitbit_idx_0, uint32 small_kaps_bb_hitbit_idx_1, uint32 small_kaps_bb_hitbit);


typedef int (*mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_get_cb)(
    int unit, uint32 small_kaps_bb_hitbit_idx_0, uint32 small_kaps_bb_hitbit_idx_1, uint32 *small_kaps_bb_hitbit);


typedef int (*mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_alloc_cb)(
    int unit);


typedef int (*mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_set_cb)(
    int unit, uint32 small_kaps_rpb_hitbit_idx_0, uint32 small_kaps_rpb_hitbit_idx_1, uint32 small_kaps_rpb_hitbit);


typedef int (*mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_get_cb)(
    int unit, uint32 small_kaps_rpb_hitbit_idx_0, uint32 small_kaps_rpb_hitbit_idx_1, uint32 *small_kaps_rpb_hitbit);


typedef int (*mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_alloc_cb)(
    int unit);


typedef int (*mdb_db_infos_mdb_cluster_infos_direct_address_mapping_set_cb)(
    int unit, uint32 direct_address_mapping_idx_0, uint32 direct_address_mapping_idx_1, uint32 direct_address_mapping);


typedef int (*mdb_db_infos_mdb_cluster_infos_direct_address_mapping_get_cb)(
    int unit, uint32 direct_address_mapping_idx_0, uint32 direct_address_mapping_idx_1, uint32 *direct_address_mapping);


typedef int (*mdb_db_infos_mdb_cluster_infos_direct_address_mapping_alloc_cb)(
    int unit, uint32 direct_address_mapping_idx_0, uint32 nof_instances_to_alloc_1);


typedef int (*mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_set_cb)(
    int unit, uint32 eedb_address_mapping_idx_0, uint32 eedb_address_mapping);


typedef int (*mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_get_cb)(
    int unit, uint32 eedb_address_mapping_idx_0, uint32 *eedb_address_mapping);


typedef int (*mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*mdb_db_infos_max_fec_id_value_set_cb)(
    int unit, uint32 max_fec_id_value);


typedef int (*mdb_db_infos_max_fec_id_value_get_cb)(
    int unit, uint32 *max_fec_id_value);


typedef int (*mdb_db_infos_eedb_bank_traffic_lock_local_lifs_set_cb)(
    int unit, uint32 local_lifs_idx_0, uint32 local_lifs_idx_1, int local_lifs);


typedef int (*mdb_db_infos_eedb_bank_traffic_lock_local_lifs_get_cb)(
    int unit, uint32 local_lifs_idx_0, uint32 local_lifs_idx_1, int *local_lifs);


typedef int (*mdb_db_infos_eedb_bank_traffic_lock_local_lifs_alloc_cb)(
    int unit, uint32 local_lifs_idx_0, uint32 nof_instances_to_alloc_1);


typedef int (*mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_set_cb)(
    int unit, uint32 global_lif_destination_idx_0, int global_lif_destination);


typedef int (*mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_get_cb)(
    int unit, uint32 global_lif_destination_idx_0, int *global_lif_destination);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_set_cb)(
    int unit, uint32 fec_hierarchy_map_idx_0, uint32 fec_hierarchy_map);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_get_cb)(
    int unit, uint32 fec_hierarchy_map_idx_0, uint32 *fec_hierarchy_map);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_alloc_cb)(
    int unit);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_id_range_start_set_cb)(
    int unit, uint32 fec_id_range_start_idx_0, uint32 fec_id_range_start);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_id_range_start_get_cb)(
    int unit, uint32 fec_id_range_start_idx_0, uint32 *fec_id_range_start);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_id_range_start_alloc_cb)(
    int unit);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_id_range_size_set_cb)(
    int unit, uint32 fec_id_range_size_idx_0, uint32 fec_id_range_size);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_id_range_size_get_cb)(
    int unit, uint32 fec_id_range_size_idx_0, uint32 *fec_id_range_size);


typedef int (*mdb_db_infos_fec_hierarchy_info_fec_id_range_size_alloc_cb)(
    int unit);


typedef int (*mdb_db_infos_mdb_profile_set_cb)(
    int unit, uint8 mdb_profile);


typedef int (*mdb_db_infos_mdb_profile_get_cb)(
    int unit, uint8 *mdb_profile);


typedef int (*mdb_db_infos_mdb_em_init_done_set_cb)(
    int unit, uint8 mdb_em_init_done);


typedef int (*mdb_db_infos_mdb_em_init_done_get_cb)(
    int unit, uint8 *mdb_em_init_done);


typedef int (*mdb_db_infos_kaps_scan_cfg_on_set_cb)(
    int unit, uint32 kaps_scan_cfg_on_idx_0, uint32 kaps_scan_cfg_on);


typedef int (*mdb_db_infos_kaps_scan_cfg_on_get_cb)(
    int unit, uint32 kaps_scan_cfg_on_idx_0, uint32 *kaps_scan_cfg_on);


typedef int (*mdb_db_infos_kaps_scan_cfg_paused_set_cb)(
    int unit, uint32 kaps_scan_cfg_paused_idx_0, uint32 kaps_scan_cfg_paused);


typedef int (*mdb_db_infos_kaps_scan_cfg_paused_get_cb)(
    int unit, uint32 kaps_scan_cfg_paused_idx_0, uint32 *kaps_scan_cfg_paused);




typedef struct {
    mdb_db_infos_em_key_format_key_size_set_cb set;
    mdb_db_infos_em_key_format_key_size_get_cb get;
} mdb_db_infos_em_key_format_key_size_cbs;


typedef struct {
    mdb_db_infos_em_key_format_format_count_set_cb set;
    mdb_db_infos_em_key_format_format_count_get_cb get;
} mdb_db_infos_em_key_format_format_count_cbs;


typedef struct {
    mdb_db_infos_em_key_format_key_to_format_map_set_cb set;
    mdb_db_infos_em_key_format_key_to_format_map_get_cb get;
} mdb_db_infos_em_key_format_key_to_format_map_cbs;


typedef struct {
    mdb_db_infos_em_key_format_em_bank_info_is_flex_set_cb set;
    mdb_db_infos_em_key_format_em_bank_info_is_flex_get_cb get;
} mdb_db_infos_em_key_format_em_bank_info_is_flex_cbs;


typedef struct {
    mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_set_cb set;
    mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_get_cb get;
} mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_cbs;


typedef struct {
    mdb_db_infos_em_key_format_em_bank_info_macro_index_set_cb set;
    mdb_db_infos_em_key_format_em_bank_info_macro_index_get_cb get;
} mdb_db_infos_em_key_format_em_bank_info_macro_index_cbs;


typedef struct {
    mdb_db_infos_em_key_format_em_bank_info_macro_type_set_cb set;
    mdb_db_infos_em_key_format_em_bank_info_macro_type_get_cb get;
} mdb_db_infos_em_key_format_em_bank_info_macro_type_cbs;


typedef struct {
    mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_set_cb set;
    mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_get_cb get;
} mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_cbs;


typedef struct {
    
    mdb_db_infos_em_key_format_em_bank_info_is_flex_cbs is_flex;
    
    mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_cbs bank_size_used_macro_b;
    
    mdb_db_infos_em_key_format_em_bank_info_macro_index_cbs macro_index;
    
    mdb_db_infos_em_key_format_em_bank_info_macro_type_cbs macro_type;
    
    mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_cbs cluster_mapping;
} mdb_db_infos_em_key_format_em_bank_info_cbs;


typedef struct {
    mdb_db_infos_em_key_format_nof_address_bits_set_cb set;
    mdb_db_infos_em_key_format_nof_address_bits_get_cb get;
} mdb_db_infos_em_key_format_nof_address_bits_cbs;


typedef struct {
    mdb_db_infos_em_key_format_set_cb set;
    mdb_db_infos_em_key_format_get_cb get;
    
    mdb_db_infos_em_key_format_key_size_cbs key_size;
    
    mdb_db_infos_em_key_format_format_count_cbs format_count;
    
    mdb_db_infos_em_key_format_key_to_format_map_cbs key_to_format_map;
    
    mdb_db_infos_em_key_format_em_bank_info_cbs em_bank_info;
    
    mdb_db_infos_em_key_format_nof_address_bits_cbs nof_address_bits;
} mdb_db_infos_em_key_format_cbs;


typedef struct {
    mdb_db_infos_db_number_of_alloc_clusters_set_cb set;
    mdb_db_infos_db_number_of_alloc_clusters_get_cb get;
} mdb_db_infos_db_number_of_alloc_clusters_cbs;


typedef struct {
    mdb_db_infos_db_number_of_clusters_set_cb set;
    mdb_db_infos_db_number_of_clusters_get_cb get;
} mdb_db_infos_db_number_of_clusters_cbs;


typedef struct {
    mdb_db_infos_db_nof_cluster_per_entry_set_cb set;
    mdb_db_infos_db_nof_cluster_per_entry_get_cb get;
} mdb_db_infos_db_nof_cluster_per_entry_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_macro_type_set_cb set;
    mdb_db_infos_db_clusters_info_macro_type_get_cb get;
} mdb_db_infos_db_clusters_info_macro_type_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_macro_index_set_cb set;
    mdb_db_infos_db_clusters_info_macro_index_get_cb get;
} mdb_db_infos_db_clusters_info_macro_index_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_cluster_index_set_cb set;
    mdb_db_infos_db_clusters_info_cluster_index_get_cb get;
} mdb_db_infos_db_clusters_info_cluster_index_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_cluster_position_in_entry_set_cb set;
    mdb_db_infos_db_clusters_info_cluster_position_in_entry_get_cb get;
} mdb_db_infos_db_clusters_info_cluster_position_in_entry_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_start_address_set_cb set;
    mdb_db_infos_db_clusters_info_start_address_get_cb get;
} mdb_db_infos_db_clusters_info_start_address_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_end_address_set_cb set;
    mdb_db_infos_db_clusters_info_end_address_get_cb get;
} mdb_db_infos_db_clusters_info_end_address_cbs;


typedef struct {
    mdb_db_infos_db_clusters_info_set_cb set;
    mdb_db_infos_db_clusters_info_get_cb get;
    mdb_db_infos_db_clusters_info_alloc_cb alloc;
    
    mdb_db_infos_db_clusters_info_macro_type_cbs macro_type;
    
    mdb_db_infos_db_clusters_info_macro_index_cbs macro_index;
    
    mdb_db_infos_db_clusters_info_cluster_index_cbs cluster_index;
    
    mdb_db_infos_db_clusters_info_cluster_position_in_entry_cbs cluster_position_in_entry;
    
    mdb_db_infos_db_clusters_info_start_address_cbs start_address;
    
    mdb_db_infos_db_clusters_info_end_address_cbs end_address;
} mdb_db_infos_db_clusters_info_cbs;


typedef struct {
    
    mdb_db_infos_db_number_of_alloc_clusters_cbs number_of_alloc_clusters;
    
    mdb_db_infos_db_number_of_clusters_cbs number_of_clusters;
    
    mdb_db_infos_db_nof_cluster_per_entry_cbs nof_cluster_per_entry;
    
    mdb_db_infos_db_clusters_info_cbs clusters_info;
} mdb_db_infos_db_cbs;


typedef struct {
    mdb_db_infos_capacity_set_cb set;
    mdb_db_infos_capacity_get_cb get;
} mdb_db_infos_capacity_cbs;


typedef struct {
    mdb_db_infos_em_entry_count_set_cb set;
    mdb_db_infos_em_entry_count_get_cb get;
} mdb_db_infos_em_entry_count_cbs;


typedef struct {
    mdb_db_infos_em_entry_capacity_estimate_set_cb set;
    mdb_db_infos_em_entry_capacity_estimate_get_cb get;
} mdb_db_infos_em_entry_capacity_estimate_cbs;


typedef struct {
    mdb_db_infos_em_entry_capacity_estimate_average_set_cb set;
    mdb_db_infos_em_entry_capacity_estimate_average_get_cb get;
} mdb_db_infos_em_entry_capacity_estimate_average_cbs;


typedef struct {
    mdb_db_infos_eedb_payload_type_set_cb set;
    mdb_db_infos_eedb_payload_type_get_cb get;
} mdb_db_infos_eedb_payload_type_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_cache_enabled_set_cb set;
    mdb_db_infos_mdb_cluster_infos_cache_enabled_get_cb get;
} mdb_db_infos_mdb_cluster_infos_cache_enabled_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_set_cb set;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_get_cb get;
} mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_set_cb set;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_get_cb get;
} mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_alloc_cb alloc;
    
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_cbs pdb;
    
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_cbs flags;
} mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_set_cb set;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_get_cb get;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_read_cb range_read;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_write_cb range_write;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_fill_cb range_fill;
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_alloc_cb alloc;
} mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_alloc_cb alloc;
    
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_cbs data;
} mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_set_cb set;
    mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_get_cb get;
    mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_alloc_cb alloc;
} mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_set_cb set;
    mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_get_cb get;
    mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_alloc_cb alloc;
} mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_set_cb set;
    mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_get_cb get;
    mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_alloc_cb alloc;
} mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_direct_address_mapping_set_cb set;
    mdb_db_infos_mdb_cluster_infos_direct_address_mapping_get_cb get;
    mdb_db_infos_mdb_cluster_infos_direct_address_mapping_alloc_cb alloc;
} mdb_db_infos_mdb_cluster_infos_direct_address_mapping_cbs;


typedef struct {
    mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_set_cb set;
    mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_get_cb get;
    mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_alloc_cb alloc;
} mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_cbs;


typedef struct {
    
    mdb_db_infos_mdb_cluster_infos_cache_enabled_cbs cache_enabled;
    
    mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_cbs macro_cluster_assoc;
    
    mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_cbs macro_cluster_cache;
    
    mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_cbs eedb_banks_hitbit;
    
    mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_cbs small_kaps_bb_hitbit;
    
    mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_cbs small_kaps_rpb_hitbit;
    
    mdb_db_infos_mdb_cluster_infos_direct_address_mapping_cbs direct_address_mapping;
    
    mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_cbs eedb_address_mapping;
} mdb_db_infos_mdb_cluster_infos_cbs;


typedef struct {
    mdb_db_infos_max_fec_id_value_set_cb set;
    mdb_db_infos_max_fec_id_value_get_cb get;
} mdb_db_infos_max_fec_id_value_cbs;


typedef struct {
    mdb_db_infos_eedb_bank_traffic_lock_local_lifs_set_cb set;
    mdb_db_infos_eedb_bank_traffic_lock_local_lifs_get_cb get;
    mdb_db_infos_eedb_bank_traffic_lock_local_lifs_alloc_cb alloc;
} mdb_db_infos_eedb_bank_traffic_lock_local_lifs_cbs;


typedef struct {
    mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_set_cb set;
    mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_get_cb get;
} mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_cbs;


typedef struct {
    
    mdb_db_infos_eedb_bank_traffic_lock_local_lifs_cbs local_lifs;
    
    mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_cbs global_lif_destination;
} mdb_db_infos_eedb_bank_traffic_lock_cbs;


typedef struct {
    mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_set_cb set;
    mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_get_cb get;
    mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_alloc_cb alloc;
} mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_cbs;


typedef struct {
    mdb_db_infos_fec_hierarchy_info_fec_id_range_start_set_cb set;
    mdb_db_infos_fec_hierarchy_info_fec_id_range_start_get_cb get;
    mdb_db_infos_fec_hierarchy_info_fec_id_range_start_alloc_cb alloc;
} mdb_db_infos_fec_hierarchy_info_fec_id_range_start_cbs;


typedef struct {
    mdb_db_infos_fec_hierarchy_info_fec_id_range_size_set_cb set;
    mdb_db_infos_fec_hierarchy_info_fec_id_range_size_get_cb get;
    mdb_db_infos_fec_hierarchy_info_fec_id_range_size_alloc_cb alloc;
} mdb_db_infos_fec_hierarchy_info_fec_id_range_size_cbs;


typedef struct {
    
    mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_cbs fec_hierarchy_map;
    
    mdb_db_infos_fec_hierarchy_info_fec_id_range_start_cbs fec_id_range_start;
    
    mdb_db_infos_fec_hierarchy_info_fec_id_range_size_cbs fec_id_range_size;
} mdb_db_infos_fec_hierarchy_info_cbs;


typedef struct {
    mdb_db_infos_mdb_profile_set_cb set;
    mdb_db_infos_mdb_profile_get_cb get;
} mdb_db_infos_mdb_profile_cbs;


typedef struct {
    mdb_db_infos_mdb_em_init_done_set_cb set;
    mdb_db_infos_mdb_em_init_done_get_cb get;
} mdb_db_infos_mdb_em_init_done_cbs;


typedef struct {
    mdb_db_infos_kaps_scan_cfg_on_set_cb set;
    mdb_db_infos_kaps_scan_cfg_on_get_cb get;
} mdb_db_infos_kaps_scan_cfg_on_cbs;


typedef struct {
    mdb_db_infos_kaps_scan_cfg_paused_set_cb set;
    mdb_db_infos_kaps_scan_cfg_paused_get_cb get;
} mdb_db_infos_kaps_scan_cfg_paused_cbs;


typedef struct {
    mdb_db_infos_is_init_cb is_init;
    mdb_db_infos_init_cb init;
    
    mdb_db_infos_em_key_format_cbs em_key_format;
    
    mdb_db_infos_db_cbs db;
    
    mdb_db_infos_capacity_cbs capacity;
    
    mdb_db_infos_em_entry_count_cbs em_entry_count;
    
    mdb_db_infos_em_entry_capacity_estimate_cbs em_entry_capacity_estimate;
    
    mdb_db_infos_em_entry_capacity_estimate_average_cbs em_entry_capacity_estimate_average;
    
    mdb_db_infos_eedb_payload_type_cbs eedb_payload_type;
    
    mdb_db_infos_mdb_cluster_infos_cbs mdb_cluster_infos;
    
    mdb_db_infos_max_fec_id_value_cbs max_fec_id_value;
    
    mdb_db_infos_eedb_bank_traffic_lock_cbs eedb_bank_traffic_lock;
    
    mdb_db_infos_fec_hierarchy_info_cbs fec_hierarchy_info;
    
    mdb_db_infos_mdb_profile_cbs mdb_profile;
    
    mdb_db_infos_mdb_em_init_done_cbs mdb_em_init_done;
    
    mdb_db_infos_kaps_scan_cfg_on_cbs kaps_scan_cfg_on;
    
    mdb_db_infos_kaps_scan_cfg_paused_cbs kaps_scan_cfg_paused;
} mdb_db_infos_cbs;





extern mdb_db_infos_cbs mdb_db_infos;

#endif 
