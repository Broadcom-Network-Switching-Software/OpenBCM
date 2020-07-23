
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ALGO_LIF_ACCESS_H__
#define __ALGO_LIF_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>



typedef int (*local_outlif_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*local_outlif_info_init_cb)(
    int unit);


typedef int (*local_outlif_info_disable_mdb_clusters_set_cb)(
    int unit, uint8 disable_mdb_clusters);


typedef int (*local_outlif_info_disable_mdb_clusters_get_cb)(
    int unit, uint8 *disable_mdb_clusters);


typedef int (*local_outlif_info_disable_eedb_data_banks_set_cb)(
    int unit, uint8 disable_eedb_data_banks);


typedef int (*local_outlif_info_disable_eedb_data_banks_get_cb)(
    int unit, uint8 *disable_eedb_data_banks);


typedef int (*local_outlif_info_outlif_bank_info_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*local_outlif_info_outlif_bank_info_assigned_set_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 assigned);


typedef int (*local_outlif_info_outlif_bank_info_assigned_get_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 *assigned);


typedef int (*local_outlif_info_outlif_bank_info_in_use_set_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 in_use);


typedef int (*local_outlif_info_outlif_bank_info_in_use_get_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 *in_use);


typedef int (*local_outlif_info_outlif_bank_info_used_logical_phase_set_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 used_logical_phase);


typedef int (*local_outlif_info_outlif_bank_info_used_logical_phase_get_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 *used_logical_phase);


typedef int (*local_outlif_info_outlif_bank_info_ll_in_use_set_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 ll_in_use);


typedef int (*local_outlif_info_outlif_bank_info_ll_in_use_get_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 *ll_in_use);


typedef int (*local_outlif_info_outlif_bank_info_ll_index_set_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 ll_index);


typedef int (*local_outlif_info_outlif_bank_info_ll_index_get_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint8 *ll_index);


typedef int (*local_outlif_info_outlif_bank_info_eedb_data_banks_set_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint32 eedb_data_banks_idx_0, uint8 eedb_data_banks);


typedef int (*local_outlif_info_outlif_bank_info_eedb_data_banks_get_cb)(
    int unit, uint32 outlif_bank_info_idx_0, uint32 eedb_data_banks_idx_0, uint8 *eedb_data_banks);


typedef int (*local_outlif_info_logical_phase_info_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*local_outlif_info_logical_phase_info_physical_phase_set_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 physical_phase);


typedef int (*local_outlif_info_logical_phase_info_physical_phase_get_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 *physical_phase);


typedef int (*local_outlif_info_logical_phase_info_address_granularity_set_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 address_granularity);


typedef int (*local_outlif_info_logical_phase_info_address_granularity_get_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 *address_granularity);


typedef int (*local_outlif_info_logical_phase_info_first_bank_set_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint32 first_bank);


typedef int (*local_outlif_info_logical_phase_info_first_bank_get_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint32 *first_bank);


typedef int (*local_outlif_info_logical_phase_info_last_bank_set_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint32 last_bank);


typedef int (*local_outlif_info_logical_phase_info_last_bank_get_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint32 *last_bank);


typedef int (*local_outlif_info_logical_phase_info_first_ll_bank_set_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 first_ll_bank);


typedef int (*local_outlif_info_logical_phase_info_first_ll_bank_get_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 *first_ll_bank);


typedef int (*local_outlif_info_logical_phase_info_last_ll_bank_set_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 last_ll_bank);


typedef int (*local_outlif_info_logical_phase_info_last_ll_bank_get_cb)(
    int unit, uint32 logical_phase_info_idx_0, uint8 *last_ll_bank);


typedef int (*local_outlif_info_logical_phase_info_dbal_valid_combinations_create_cb)(
    int unit, uint32 logical_phase_info_idx_0, sw_state_htbl_init_info_t *init_info);


typedef int (*local_outlif_info_logical_phase_info_dbal_valid_combinations_find_cb)(
    int unit, uint32 logical_phase_info_idx_0, const dbal_to_phase_info_t *key, dnx_local_outlif_ll_indication_e *value, uint8 *found);


typedef int (*local_outlif_info_logical_phase_info_dbal_valid_combinations_insert_cb)(
    int unit, uint32 logical_phase_info_idx_0, const dbal_to_phase_info_t *key, const dnx_local_outlif_ll_indication_e *value, uint8 *success);


typedef int (*local_outlif_info_logical_phase_info_dbal_valid_combinations_get_next_cb)(
    int unit, uint32 logical_phase_info_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const dbal_to_phase_info_t *key, const dnx_local_outlif_ll_indication_e *value);


typedef int (*local_outlif_info_logical_phase_info_dbal_valid_combinations_clear_cb)(
    int unit, uint32 logical_phase_info_idx_0);


typedef int (*local_outlif_info_logical_phase_info_dbal_valid_combinations_delete_cb)(
    int unit, uint32 logical_phase_info_idx_0, const dbal_to_phase_info_t *key);


typedef int (*local_outlif_info_physical_phase_to_logical_phase_map_set_cb)(
    int unit, uint32 physical_phase_to_logical_phase_map_idx_0, uint8 physical_phase_to_logical_phase_map);


typedef int (*local_outlif_info_physical_phase_to_logical_phase_map_get_cb)(
    int unit, uint32 physical_phase_to_logical_phase_map_idx_0, uint8 *physical_phase_to_logical_phase_map);


typedef int (*local_outlif_info_physical_phase_to_logical_phase_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*local_outlif_info_eedb_banks_info_bank_occupation_bitmap_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);


typedef int (*local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);


typedef int (*local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);


typedef int (*local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_create_empty_cb)(
    int unit, sw_state_ll_init_info_t *init_info);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_nof_elements_cb)(
    int unit, uint32 ll_head_index, uint32 *nof_elements);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_value_cb)(
    int unit, sw_state_ll_node_t node, uint8 *value);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_update_cb)(
    int unit, sw_state_ll_node_t node, const uint8 *value);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_next_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_previous_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_first_cb)(
    int unit, uint32 ll_head_index, const uint8 *value);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_last_cb)(
    int unit, uint32 ll_head_index, const uint8 *value);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_remove_node_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t node);


typedef int (*local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_get_first_cb)(
    int unit, uint32 ll_head_index, sw_state_ll_node_t *node);




typedef struct {
    local_outlif_info_disable_mdb_clusters_set_cb set;
    local_outlif_info_disable_mdb_clusters_get_cb get;
} local_outlif_info_disable_mdb_clusters_cbs;


typedef struct {
    local_outlif_info_disable_eedb_data_banks_set_cb set;
    local_outlif_info_disable_eedb_data_banks_get_cb get;
} local_outlif_info_disable_eedb_data_banks_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_assigned_set_cb set;
    local_outlif_info_outlif_bank_info_assigned_get_cb get;
} local_outlif_info_outlif_bank_info_assigned_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_in_use_set_cb set;
    local_outlif_info_outlif_bank_info_in_use_get_cb get;
} local_outlif_info_outlif_bank_info_in_use_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_used_logical_phase_set_cb set;
    local_outlif_info_outlif_bank_info_used_logical_phase_get_cb get;
} local_outlif_info_outlif_bank_info_used_logical_phase_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_ll_in_use_set_cb set;
    local_outlif_info_outlif_bank_info_ll_in_use_get_cb get;
} local_outlif_info_outlif_bank_info_ll_in_use_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_ll_index_set_cb set;
    local_outlif_info_outlif_bank_info_ll_index_get_cb get;
} local_outlif_info_outlif_bank_info_ll_index_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_eedb_data_banks_set_cb set;
    local_outlif_info_outlif_bank_info_eedb_data_banks_get_cb get;
} local_outlif_info_outlif_bank_info_eedb_data_banks_cbs;


typedef struct {
    local_outlif_info_outlif_bank_info_alloc_cb alloc;
    
    local_outlif_info_outlif_bank_info_assigned_cbs assigned;
    
    local_outlif_info_outlif_bank_info_in_use_cbs in_use;
    
    local_outlif_info_outlif_bank_info_used_logical_phase_cbs used_logical_phase;
    
    local_outlif_info_outlif_bank_info_ll_in_use_cbs ll_in_use;
    
    local_outlif_info_outlif_bank_info_ll_index_cbs ll_index;
    
    local_outlif_info_outlif_bank_info_eedb_data_banks_cbs eedb_data_banks;
} local_outlif_info_outlif_bank_info_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_physical_phase_set_cb set;
    local_outlif_info_logical_phase_info_physical_phase_get_cb get;
} local_outlif_info_logical_phase_info_physical_phase_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_address_granularity_set_cb set;
    local_outlif_info_logical_phase_info_address_granularity_get_cb get;
} local_outlif_info_logical_phase_info_address_granularity_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_first_bank_set_cb set;
    local_outlif_info_logical_phase_info_first_bank_get_cb get;
} local_outlif_info_logical_phase_info_first_bank_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_last_bank_set_cb set;
    local_outlif_info_logical_phase_info_last_bank_get_cb get;
} local_outlif_info_logical_phase_info_last_bank_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_first_ll_bank_set_cb set;
    local_outlif_info_logical_phase_info_first_ll_bank_get_cb get;
} local_outlif_info_logical_phase_info_first_ll_bank_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_last_ll_bank_set_cb set;
    local_outlif_info_logical_phase_info_last_ll_bank_get_cb get;
} local_outlif_info_logical_phase_info_last_ll_bank_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_dbal_valid_combinations_create_cb create;
    local_outlif_info_logical_phase_info_dbal_valid_combinations_find_cb find;
    local_outlif_info_logical_phase_info_dbal_valid_combinations_insert_cb insert;
    local_outlif_info_logical_phase_info_dbal_valid_combinations_get_next_cb get_next;
    local_outlif_info_logical_phase_info_dbal_valid_combinations_clear_cb clear;
    local_outlif_info_logical_phase_info_dbal_valid_combinations_delete_cb delete;
} local_outlif_info_logical_phase_info_dbal_valid_combinations_cbs;


typedef struct {
    local_outlif_info_logical_phase_info_alloc_cb alloc;
    
    local_outlif_info_logical_phase_info_physical_phase_cbs physical_phase;
    
    local_outlif_info_logical_phase_info_address_granularity_cbs address_granularity;
    
    local_outlif_info_logical_phase_info_first_bank_cbs first_bank;
    
    local_outlif_info_logical_phase_info_last_bank_cbs last_bank;
    
    local_outlif_info_logical_phase_info_first_ll_bank_cbs first_ll_bank;
    
    local_outlif_info_logical_phase_info_last_ll_bank_cbs last_ll_bank;
    
    local_outlif_info_logical_phase_info_dbal_valid_combinations_cbs dbal_valid_combinations;
} local_outlif_info_logical_phase_info_cbs;


typedef struct {
    local_outlif_info_physical_phase_to_logical_phase_map_set_cb set;
    local_outlif_info_physical_phase_to_logical_phase_map_get_cb get;
    local_outlif_info_physical_phase_to_logical_phase_map_alloc_cb alloc;
} local_outlif_info_physical_phase_to_logical_phase_map_cbs;


typedef struct {
    local_outlif_info_eedb_banks_info_bank_occupation_bitmap_alloc_bitmap_cb alloc_bitmap;
    local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_set_cb bit_set;
    local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_clear_cb bit_clear;
    local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_get_cb bit_get;
    local_outlif_info_eedb_banks_info_bank_occupation_bitmap_bit_range_count_cb bit_range_count;
} local_outlif_info_eedb_banks_info_bank_occupation_bitmap_cbs;


typedef struct {
    local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_alloc_bitmap_cb alloc_bitmap;
    local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_set_cb bit_set;
    local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_clear_cb bit_clear;
    local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_bit_get_cb bit_get;
} local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_cbs;


typedef struct {
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_create_empty_cb create_empty;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_nof_elements_cb nof_elements;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_value_cb node_value;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_node_update_cb node_update;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_next_node_cb next_node;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_previous_node_cb previous_node;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_first_cb add_first;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_add_last_cb add_last;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_remove_node_cb remove_node;
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_get_first_cb get_first;
} local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_cbs;


typedef struct {
    
    local_outlif_info_eedb_banks_info_bank_occupation_bitmap_cbs bank_occupation_bitmap;
    
    local_outlif_info_eedb_banks_info_outlif_prefix_occupation_bitmap_cbs outlif_prefix_occupation_bitmap;
    
    local_outlif_info_eedb_banks_info_used_data_bank_per_logical_phase_cbs used_data_bank_per_logical_phase;
} local_outlif_info_eedb_banks_info_cbs;


typedef struct {
    local_outlif_info_is_init_cb is_init;
    local_outlif_info_init_cb init;
    
    local_outlif_info_disable_mdb_clusters_cbs disable_mdb_clusters;
    
    local_outlif_info_disable_eedb_data_banks_cbs disable_eedb_data_banks;
    
    local_outlif_info_outlif_bank_info_cbs outlif_bank_info;
    
    local_outlif_info_logical_phase_info_cbs logical_phase_info;
    
    local_outlif_info_physical_phase_to_logical_phase_map_cbs physical_phase_to_logical_phase_map;
    
    local_outlif_info_eedb_banks_info_cbs eedb_banks_info;
} local_outlif_info_cbs;






const char *
dnx_local_outlif_ll_indication_e_get_name(dnx_local_outlif_ll_indication_e value);




extern local_outlif_info_cbs local_outlif_info;

#endif 
