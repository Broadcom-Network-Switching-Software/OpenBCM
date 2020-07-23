
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_ACCESS_H__
#define __DNX_FIELD_TCAM_ACCESS_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>



typedef int (*dnx_field_tcam_access_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_access_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_create_cb)(
    int unit, uint32 entry_location_hash_idx_0, sw_state_htbl_init_info_t *init_info);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_find_cb)(
    int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 *data_index, uint8 *found);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_insert_cb)(
    int unit, uint32 entry_location_hash_idx_0, const uint32 *key, const uint32 *data_index, uint8 *success);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_get_next_cb)(
    int unit, uint32 entry_location_hash_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const uint32 *key, const uint32 *data_index);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_clear_cb)(
    int unit, uint32 entry_location_hash_idx_0);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_delete_cb)(
    int unit, uint32 entry_location_hash_idx_0, const uint32 *key);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_insert_at_index_cb)(
    int unit, uint32 entry_location_hash_idx_0, const uint32 *key, uint32 data_idx, uint8 *success);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_delete_by_index_cb)(
    int unit, uint32 entry_location_hash_idx_0, uint32 data_index);


typedef int (*dnx_field_tcam_access_sw_entry_location_hash_get_by_index_cb)(
    int unit, uint32 entry_location_hash_idx_0, uint32 data_index, uint32 *key, uint8 *found);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc_bitmap_cb)(
    int unit, uint32 valid_bmp_idx_0);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_set_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _bit_num);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_clear_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _bit_num);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_get_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _bit_num, uint8* result);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_read_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_write_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_and_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_or_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_xor_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_remove_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_negate_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_clear_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_set_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_null_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_test_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_eq_cb)(
    int unit, uint32 valid_bmp_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_count_cb)(
    int unit, uint32 valid_bmp_idx_0, uint32 _first, uint32 _range, int *result);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc_bitmap_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_set_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_clear_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_get_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _bit_num, uint8* result);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_read_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_write_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_and_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_or_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_xor_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_remove_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_negate_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_clear_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_set_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_null_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_test_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_eq_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_count_cb)(
    int unit, uint32 entry_in_use_bmp_idx_0, uint32 _first, uint32 _range, int *result);


typedef int (*dnx_field_tcam_access_sw_fg_params_set_cb)(
    int unit, uint32 fg_params_idx_0, CONST dnx_field_tcam_access_fg_params_t *fg_params);


typedef int (*dnx_field_tcam_access_sw_fg_params_get_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_access_fg_params_t *fg_params);


typedef int (*dnx_field_tcam_access_sw_fg_params_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_sw_fg_params_key_size_set_cb)(
    int unit, uint32 fg_params_idx_0, uint32 key_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_key_size_get_cb)(
    int unit, uint32 fg_params_idx_0, uint32 *key_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_action_size_set_cb)(
    int unit, uint32 fg_params_idx_0, uint32 action_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_action_size_get_cb)(
    int unit, uint32 fg_params_idx_0, uint32 *action_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_actual_action_size_set_cb)(
    int unit, uint32 fg_params_idx_0, uint32 actual_action_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_actual_action_size_get_cb)(
    int unit, uint32 fg_params_idx_0, uint32 *actual_action_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_stage_set_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_stage_e stage);


typedef int (*dnx_field_tcam_access_sw_fg_params_stage_get_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_stage_e *stage);


typedef int (*dnx_field_tcam_access_sw_fg_params_prefix_size_set_cb)(
    int unit, uint32 fg_params_idx_0, uint32 prefix_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_prefix_size_get_cb)(
    int unit, uint32 fg_params_idx_0, uint32 *prefix_size);


typedef int (*dnx_field_tcam_access_sw_fg_params_prefix_value_set_cb)(
    int unit, uint32 fg_params_idx_0, uint32 prefix_value);


typedef int (*dnx_field_tcam_access_sw_fg_params_prefix_value_get_cb)(
    int unit, uint32 fg_params_idx_0, uint32 *prefix_value);


typedef int (*dnx_field_tcam_access_sw_fg_params_direct_table_set_cb)(
    int unit, uint32 fg_params_idx_0, uint8 direct_table);


typedef int (*dnx_field_tcam_access_sw_fg_params_direct_table_get_cb)(
    int unit, uint32 fg_params_idx_0, uint8 *direct_table);


typedef int (*dnx_field_tcam_access_sw_fg_params_dt_bank_id_set_cb)(
    int unit, uint32 fg_params_idx_0, int dt_bank_id);


typedef int (*dnx_field_tcam_access_sw_fg_params_dt_bank_id_get_cb)(
    int unit, uint32 fg_params_idx_0, int *dt_bank_id);


typedef int (*dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode);


typedef int (*dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_get_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_bank_allocation_mode_e *bank_allocation_mode);


typedef int (*dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb);


typedef int (*dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_get_cb)(
    int unit, uint32 fg_params_idx_0, dnx_field_tcam_context_sharing_handlers_get_p *context_sharing_handlers_cb);




typedef struct {
    dnx_field_tcam_access_sw_entry_location_hash_alloc_cb alloc;
    dnx_field_tcam_access_sw_entry_location_hash_create_cb create;
    dnx_field_tcam_access_sw_entry_location_hash_find_cb find;
    dnx_field_tcam_access_sw_entry_location_hash_insert_cb insert;
    dnx_field_tcam_access_sw_entry_location_hash_get_next_cb get_next;
    dnx_field_tcam_access_sw_entry_location_hash_clear_cb clear;
    dnx_field_tcam_access_sw_entry_location_hash_delete_cb delete;
    dnx_field_tcam_access_sw_entry_location_hash_insert_at_index_cb insert_at_index;
    dnx_field_tcam_access_sw_entry_location_hash_delete_by_index_cb delete_by_index;
    dnx_field_tcam_access_sw_entry_location_hash_get_by_index_cb get_by_index;
} dnx_field_tcam_access_sw_entry_location_hash_cbs;


typedef struct {
    dnx_field_tcam_access_sw_valid_bmp_v_bit_alloc_bitmap_cb alloc_bitmap;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_set_cb bit_set;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_clear_cb bit_clear;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_get_cb bit_get;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_read_cb bit_range_read;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_write_cb bit_range_write;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_and_cb bit_range_and;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_or_cb bit_range_or;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_xor_cb bit_range_xor;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_remove_cb bit_range_remove;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_negate_cb bit_range_negate;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_clear_cb bit_range_clear;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_set_cb bit_range_set;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_null_cb bit_range_null;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_test_cb bit_range_test;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_eq_cb bit_range_eq;
    dnx_field_tcam_access_sw_valid_bmp_v_bit_bit_range_count_cb bit_range_count;
} dnx_field_tcam_access_sw_valid_bmp_v_bit_cbs;


typedef struct {
    dnx_field_tcam_access_sw_valid_bmp_alloc_cb alloc;
    
    dnx_field_tcam_access_sw_valid_bmp_v_bit_cbs v_bit;
} dnx_field_tcam_access_sw_valid_bmp_cbs;


typedef struct {
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_alloc_bitmap_cb alloc_bitmap;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_set_cb bit_set;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_clear_cb bit_clear;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_get_cb bit_get;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_read_cb bit_range_read;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_write_cb bit_range_write;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_and_cb bit_range_and;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_or_cb bit_range_or;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_xor_cb bit_range_xor;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_remove_cb bit_range_remove;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_negate_cb bit_range_negate;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_clear_cb bit_range_clear;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_set_cb bit_range_set;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_null_cb bit_range_null;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_test_cb bit_range_test;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_eq_cb bit_range_eq;
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_bit_range_count_cb bit_range_count;
} dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_cbs;


typedef struct {
    dnx_field_tcam_access_sw_entry_in_use_bmp_alloc_cb alloc;
    
    dnx_field_tcam_access_sw_entry_in_use_bmp_v_bit_cbs v_bit;
} dnx_field_tcam_access_sw_entry_in_use_bmp_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_key_size_set_cb set;
    dnx_field_tcam_access_sw_fg_params_key_size_get_cb get;
} dnx_field_tcam_access_sw_fg_params_key_size_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_action_size_set_cb set;
    dnx_field_tcam_access_sw_fg_params_action_size_get_cb get;
} dnx_field_tcam_access_sw_fg_params_action_size_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_actual_action_size_set_cb set;
    dnx_field_tcam_access_sw_fg_params_actual_action_size_get_cb get;
} dnx_field_tcam_access_sw_fg_params_actual_action_size_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_stage_set_cb set;
    dnx_field_tcam_access_sw_fg_params_stage_get_cb get;
} dnx_field_tcam_access_sw_fg_params_stage_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_prefix_size_set_cb set;
    dnx_field_tcam_access_sw_fg_params_prefix_size_get_cb get;
} dnx_field_tcam_access_sw_fg_params_prefix_size_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_prefix_value_set_cb set;
    dnx_field_tcam_access_sw_fg_params_prefix_value_get_cb get;
} dnx_field_tcam_access_sw_fg_params_prefix_value_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_direct_table_set_cb set;
    dnx_field_tcam_access_sw_fg_params_direct_table_get_cb get;
} dnx_field_tcam_access_sw_fg_params_direct_table_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_dt_bank_id_set_cb set;
    dnx_field_tcam_access_sw_fg_params_dt_bank_id_get_cb get;
} dnx_field_tcam_access_sw_fg_params_dt_bank_id_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_set_cb set;
    dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_get_cb get;
} dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_set_cb set;
    dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_get_cb get;
} dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_cbs;


typedef struct {
    dnx_field_tcam_access_sw_fg_params_set_cb set;
    dnx_field_tcam_access_sw_fg_params_get_cb get;
    dnx_field_tcam_access_sw_fg_params_alloc_cb alloc;
    
    dnx_field_tcam_access_sw_fg_params_key_size_cbs key_size;
    
    dnx_field_tcam_access_sw_fg_params_action_size_cbs action_size;
    
    dnx_field_tcam_access_sw_fg_params_actual_action_size_cbs actual_action_size;
    
    dnx_field_tcam_access_sw_fg_params_stage_cbs stage;
    
    dnx_field_tcam_access_sw_fg_params_prefix_size_cbs prefix_size;
    
    dnx_field_tcam_access_sw_fg_params_prefix_value_cbs prefix_value;
    
    dnx_field_tcam_access_sw_fg_params_direct_table_cbs direct_table;
    
    dnx_field_tcam_access_sw_fg_params_dt_bank_id_cbs dt_bank_id;
    
    dnx_field_tcam_access_sw_fg_params_bank_allocation_mode_cbs bank_allocation_mode;
    
    dnx_field_tcam_access_sw_fg_params_context_sharing_handlers_cb_cbs context_sharing_handlers_cb;
} dnx_field_tcam_access_sw_fg_params_cbs;


typedef struct {
    dnx_field_tcam_access_sw_is_init_cb is_init;
    dnx_field_tcam_access_sw_init_cb init;
    
    dnx_field_tcam_access_sw_entry_location_hash_cbs entry_location_hash;
    
    dnx_field_tcam_access_sw_valid_bmp_cbs valid_bmp;
    
    dnx_field_tcam_access_sw_entry_in_use_bmp_cbs entry_in_use_bmp;
    
    dnx_field_tcam_access_sw_fg_params_cbs fg_params;
} dnx_field_tcam_access_sw_cbs;






const char *
dnx_field_tcam_stage_e_get_name(dnx_field_tcam_stage_e value);



const char *
dnx_field_tcam_bank_allocation_mode_e_get_name(dnx_field_tcam_bank_allocation_mode_e value);



const char *
dnx_field_tcam_entry_size_e_get_name(dnx_field_tcam_entry_size_e value);



const char *
dnx_field_tcam_core_e_get_name(dnx_field_tcam_core_e value);




extern dnx_field_tcam_access_sw_cbs dnx_field_tcam_access_sw;

#endif 
