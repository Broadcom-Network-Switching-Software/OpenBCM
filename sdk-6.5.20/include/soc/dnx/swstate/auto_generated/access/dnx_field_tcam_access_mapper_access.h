
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_MAPPER_ACCESS_H__
#define __DNX_FIELD_TCAM_ACCESS_MAPPER_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_mapper_types.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef int (*dnx_field_tcam_access_mapper_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_tcam_access_mapper_sw_init_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_alloc_cb)(
    int unit);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_create_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, sw_state_htbl_init_info_t *init_info);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_find_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key, uint32 *data_index, uint8 *found);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key, const uint32 *data_index, uint8 *success);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_next_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, SW_STATE_HASH_TABLE_ITER *iter, const dnx_field_tcam_access_mapper_hash_key *key, const uint32 *data_index);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_clear_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert_at_index_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, const dnx_field_tcam_access_mapper_hash_key *key, uint32 data_idx, uint8 *success);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete_by_index_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, uint32 data_index);


typedef int (*dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_by_index_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, uint32 key_2_entry_id_hash_idx_1, uint32 data_index, dnx_field_tcam_access_mapper_hash_key *key, uint8 *found);




typedef struct {
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_alloc_cb alloc;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_create_cb create;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_find_cb find;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert_cb insert;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_next_cb get_next;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_clear_cb clear;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete_cb delete;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_insert_at_index_cb insert_at_index;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_delete_by_index_cb delete_by_index;
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_get_by_index_cb get_by_index;
} dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_cbs;


typedef struct {
    dnx_field_tcam_access_mapper_sw_is_init_cb is_init;
    dnx_field_tcam_access_mapper_sw_init_cb init;
    
    dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_cbs key_2_entry_id_hash;
} dnx_field_tcam_access_mapper_sw_cbs;





extern dnx_field_tcam_access_mapper_sw_cbs dnx_field_tcam_access_mapper_sw;

#endif 
