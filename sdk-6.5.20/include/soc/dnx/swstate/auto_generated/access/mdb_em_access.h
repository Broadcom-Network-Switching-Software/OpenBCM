
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_EM_ACCESS_H__
#define __MDB_EM_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_em_types.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>



typedef int (*mdb_em_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*mdb_em_db_init_cb)(
    int unit);


typedef int (*mdb_em_db_shadow_em_db_create_cb)(
    int unit, uint32 shadow_em_db_idx_0, sw_state_htbl_init_info_t *init_info, int key_size, int value_size);


typedef int (*mdb_em_db_shadow_em_db_find_cb)(
    int unit, uint32 shadow_em_db_idx_0, const void *key, void *value, uint8 *found);


typedef int (*mdb_em_db_shadow_em_db_insert_cb)(
    int unit, uint32 shadow_em_db_idx_0, const void *key, const void *value, uint8 *success);


typedef int (*mdb_em_db_shadow_em_db_get_next_cb)(
    int unit, uint32 shadow_em_db_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const void *key, const void *value);


typedef int (*mdb_em_db_shadow_em_db_clear_cb)(
    int unit, uint32 shadow_em_db_idx_0);


typedef int (*mdb_em_db_shadow_em_db_delete_cb)(
    int unit, uint32 shadow_em_db_idx_0, const void *key);


typedef int (*mdb_em_db_shadow_em_db_delete_all_cb)(
    int unit, uint32 shadow_em_db_idx_0);


typedef int (*mdb_em_db_vmv_info_value_set_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 value_idx_0, uint8 value);


typedef int (*mdb_em_db_vmv_info_value_get_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 value_idx_0, uint8 *value);


typedef int (*mdb_em_db_vmv_info_size_set_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 size_idx_0, uint8 size);


typedef int (*mdb_em_db_vmv_info_size_get_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 size_idx_0, uint8 *size);


typedef int (*mdb_em_db_vmv_info_encoding_map_encoding_set_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 encoding);


typedef int (*mdb_em_db_vmv_info_encoding_map_encoding_get_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 *encoding);


typedef int (*mdb_em_db_vmv_info_encoding_map_size_set_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 size);


typedef int (*mdb_em_db_vmv_info_encoding_map_size_get_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 encoding_map_idx_0, uint8 *size);


typedef int (*mdb_em_db_vmv_info_app_id_size_set_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 app_id_size_idx_0, uint8 app_id_size);


typedef int (*mdb_em_db_vmv_info_app_id_size_get_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 app_id_size_idx_0, uint8 *app_id_size);


typedef int (*mdb_em_db_vmv_info_max_payload_size_set_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 max_payload_size_idx_0, int max_payload_size);


typedef int (*mdb_em_db_vmv_info_max_payload_size_get_cb)(
    int unit, uint32 vmv_info_idx_0, uint32 max_payload_size_idx_0, int *max_payload_size);


typedef int (*mdb_em_db_mact_stamp_set_cb)(
    int unit, uint32 mact_stamp);


typedef int (*mdb_em_db_mact_stamp_get_cb)(
    int unit, uint32 *mact_stamp);




typedef struct {
    mdb_em_db_shadow_em_db_create_cb create;
    mdb_em_db_shadow_em_db_find_cb find;
    mdb_em_db_shadow_em_db_insert_cb insert;
    mdb_em_db_shadow_em_db_get_next_cb get_next;
    mdb_em_db_shadow_em_db_clear_cb clear;
    mdb_em_db_shadow_em_db_delete_cb delete;
    mdb_em_db_shadow_em_db_delete_all_cb delete_all;
} mdb_em_db_shadow_em_db_cbs;


typedef struct {
    mdb_em_db_vmv_info_value_set_cb set;
    mdb_em_db_vmv_info_value_get_cb get;
} mdb_em_db_vmv_info_value_cbs;


typedef struct {
    mdb_em_db_vmv_info_size_set_cb set;
    mdb_em_db_vmv_info_size_get_cb get;
} mdb_em_db_vmv_info_size_cbs;


typedef struct {
    mdb_em_db_vmv_info_encoding_map_encoding_set_cb set;
    mdb_em_db_vmv_info_encoding_map_encoding_get_cb get;
} mdb_em_db_vmv_info_encoding_map_encoding_cbs;


typedef struct {
    mdb_em_db_vmv_info_encoding_map_size_set_cb set;
    mdb_em_db_vmv_info_encoding_map_size_get_cb get;
} mdb_em_db_vmv_info_encoding_map_size_cbs;


typedef struct {
    
    mdb_em_db_vmv_info_encoding_map_encoding_cbs encoding;
    
    mdb_em_db_vmv_info_encoding_map_size_cbs size;
} mdb_em_db_vmv_info_encoding_map_cbs;


typedef struct {
    mdb_em_db_vmv_info_app_id_size_set_cb set;
    mdb_em_db_vmv_info_app_id_size_get_cb get;
} mdb_em_db_vmv_info_app_id_size_cbs;


typedef struct {
    mdb_em_db_vmv_info_max_payload_size_set_cb set;
    mdb_em_db_vmv_info_max_payload_size_get_cb get;
} mdb_em_db_vmv_info_max_payload_size_cbs;


typedef struct {
    
    mdb_em_db_vmv_info_value_cbs value;
    
    mdb_em_db_vmv_info_size_cbs size;
    
    mdb_em_db_vmv_info_encoding_map_cbs encoding_map;
    
    mdb_em_db_vmv_info_app_id_size_cbs app_id_size;
    
    mdb_em_db_vmv_info_max_payload_size_cbs max_payload_size;
} mdb_em_db_vmv_info_cbs;


typedef struct {
    mdb_em_db_mact_stamp_set_cb set;
    mdb_em_db_mact_stamp_get_cb get;
} mdb_em_db_mact_stamp_cbs;


typedef struct {
    mdb_em_db_is_init_cb is_init;
    mdb_em_db_init_cb init;
    
    mdb_em_db_shadow_em_db_cbs shadow_em_db;
    
    mdb_em_db_vmv_info_cbs vmv_info;
    
    mdb_em_db_mact_stamp_cbs mact_stamp;
} mdb_em_db_cbs;





extern mdb_em_db_cbs mdb_em_db;

#endif 
