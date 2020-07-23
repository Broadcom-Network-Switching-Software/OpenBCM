
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_FWD_TCAM_ACCESS_MAPPER_ACCESS_H__
#define __KBP_FWD_TCAM_ACCESS_MAPPER_ACCESS_H__

#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_fwd_tcam_access_mapper_types.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>



typedef int (*kbp_fwd_tcam_access_mapper_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*kbp_fwd_tcam_access_mapper_init_cb)(
    int unit);


typedef int (*kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_create_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, sw_state_htbl_init_info_t *init_info);


typedef int (*kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_find_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, const dnx_kbp_fwd_tcam_access_hash_key_t *key, uint32 *value, uint8 *found);


typedef int (*kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_insert_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, const dnx_kbp_fwd_tcam_access_hash_key_t *key, const uint32 *value, uint8 *success);


typedef int (*kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_get_next_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const dnx_kbp_fwd_tcam_access_hash_key_t *key, const uint32 *value);


typedef int (*kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_clear_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0);


typedef int (*kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_delete_cb)(
    int unit, uint32 key_2_entry_id_hash_idx_0, const dnx_kbp_fwd_tcam_access_hash_key_t *key);




typedef struct {
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_create_cb create;
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_find_cb find;
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_insert_cb insert;
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_get_next_cb get_next;
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_clear_cb clear;
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_delete_cb delete;
} kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_cbs;


typedef struct {
    kbp_fwd_tcam_access_mapper_is_init_cb is_init;
    kbp_fwd_tcam_access_mapper_init_cb init;
    
    kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_cbs key_2_entry_id_hash;
} kbp_fwd_tcam_access_mapper_cbs;






const char *
dnx_kbp_fwd_tcam_hash_table_index_e_get_name(dnx_kbp_fwd_tcam_hash_table_index_e value);




extern kbp_fwd_tcam_access_mapper_cbs kbp_fwd_tcam_access_mapper;
#endif  

#endif 
