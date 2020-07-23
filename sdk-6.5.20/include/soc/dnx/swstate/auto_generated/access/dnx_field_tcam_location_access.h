
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_LOCATION_ACCESS_H__
#define __DNX_FIELD_TCAM_LOCATION_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>



typedef int (*dnx_field_tcam_location_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_location_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc_cb)(
    int unit, uint32 tcam_handlers_info_idx_0);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_create_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, sw_state_occ_bitmap_init_info_t * init_info);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 *place, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next_in_range_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_status_set_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 place, uint8 occupied);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_is_occupied_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 place, uint8 *occupied);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc_next_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 *place, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_clear_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_alloc_cb)(
    int unit, uint32 tcam_handlers_info_idx_0);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_create_empty_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_init_info_t *init_info);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_nof_elements_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, uint32 *nof_elements);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_value_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_update_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, const dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_next_node_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *next_node);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_previous_node_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_first_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, const uint32 *key, const dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_last_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, const uint32 *key, const dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_before_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, const uint32 *key, const dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_after_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, const uint32 *key, const dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_remove_node_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_last_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t *node);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_first_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t *node);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_print_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, const uint32 *key, const dnx_field_tcam_location_range_t *value);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_key_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, uint32 *key);


typedef int (*dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_find_cb)(
    int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t *node, const uint32 *key, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_location_priority_arr_set_cb)(
    int unit, uint32 location_priority_arr_idx_0, uint32 location_priority_arr_idx_1, uint32 location_priority_arr);


typedef int (*dnx_field_tcam_location_sw_location_priority_arr_get_cb)(
    int unit, uint32 location_priority_arr_idx_0, uint32 location_priority_arr_idx_1, uint32 *location_priority_arr);


typedef int (*dnx_field_tcam_location_sw_location_priority_arr_alloc_cb)(
    int unit, uint32 location_priority_arr_idx_0);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_create_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0, sw_state_occ_bitmap_init_info_t * init_info);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 *place, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next_in_range_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_status_set_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 place, uint8 occupied);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_is_occupied_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 place, uint8 *occupied);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc_next_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 *place, uint8 *found);


typedef int (*dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_clear_cb)(
    int unit, uint32 tcam_banks_occupation_bitmap_idx_0);




typedef struct {
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc_cb alloc;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_create_cb create;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next_cb get_next;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next_in_range_cb get_next_in_range;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_status_set_cb status_set;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_is_occupied_cb is_occupied;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc_next_cb alloc_next;
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_clear_cb clear;
} dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_cbs;


typedef struct {
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_alloc_cb alloc;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_create_empty_cb create_empty;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_nof_elements_cb nof_elements;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_value_cb node_value;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_update_cb node_update;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_next_node_cb next_node;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_previous_node_cb previous_node;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_first_cb add_first;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_last_cb add_last;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_before_cb add_before;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_after_cb add_after;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_remove_node_cb remove_node;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_last_cb get_last;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_first_cb get_first;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_print_cb print;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_cb add;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_key_cb node_key;
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_find_cb find;
} dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_cbs;


typedef struct {
    dnx_field_tcam_location_sw_tcam_handlers_info_alloc_cb alloc;
    
    dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_cbs entries_occupation_bitmap;
    
    dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_cbs priorities_range;
} dnx_field_tcam_location_sw_tcam_handlers_info_cbs;


typedef struct {
    dnx_field_tcam_location_sw_location_priority_arr_set_cb set;
    dnx_field_tcam_location_sw_location_priority_arr_get_cb get;
    dnx_field_tcam_location_sw_location_priority_arr_alloc_cb alloc;
} dnx_field_tcam_location_sw_location_priority_arr_cbs;


typedef struct {
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc_cb alloc;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_create_cb create;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next_cb get_next;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next_in_range_cb get_next_in_range;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_status_set_cb status_set;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_is_occupied_cb is_occupied;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc_next_cb alloc_next;
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_clear_cb clear;
} dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_cbs;


typedef struct {
    dnx_field_tcam_location_sw_is_init_cb is_init;
    dnx_field_tcam_location_sw_init_cb init;
    
    dnx_field_tcam_location_sw_tcam_handlers_info_cbs tcam_handlers_info;
    
    dnx_field_tcam_location_sw_location_priority_arr_cbs location_priority_arr;
    
    dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_cbs tcam_banks_occupation_bitmap;
} dnx_field_tcam_location_sw_cbs;





extern dnx_field_tcam_location_sw_cbs dnx_field_tcam_location_sw;

#endif 
