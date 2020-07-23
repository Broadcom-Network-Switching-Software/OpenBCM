/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SW_STATE_HASHTABLE_H_INCLUDED

#define SW_STATE_HASHTABLE_H_INCLUDED

#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>

#define SW_STATE_HASH_TABLE_NULL  UTILEX_U32_MAX

#define SW_STATE_HTB_CREATE(module_id, htb, init_info, key_type, value_type, is_index, nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_create(unit, module_id, &htb, init_info, sizeof(key_type), sizeof(value_type), is_index, nof_elements, alloc_flags))

#define SW_STATE_HTB_CREATE_WITH_SIZE(module_id, htb, init_info, key_size, value_size, is_index, nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_create(unit, module_id, &htb, init_info, key_size, value_size, is_index, nof_elements, alloc_flags))

#define SW_STATE_HTB_DESTROY(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_destroy(unit, module_id, &htb))

#define SW_STATE_HTB_INSERT(module_id, htb, key, data, success) \
    do { \
        if (htb->is_index) { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, NULL, (uint32*)data, success)); \
        } else { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, (SW_STATE_HASH_TABLE_DATA*)data, NULL, success)); \
        } \
    } while (0)

#define SW_STATE_HTB_FIND(module_id, htb, key, data, found) \
    do { \
        if (htb->is_index) { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, NULL, (uint32*)data, found)); \
        } else { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, (SW_STATE_HASH_TABLE_DATA*)data, NULL, found)); \
        } \
    } while(0)

#define SW_STATE_HTB_GET_NEXT(module_id, htb, iter, key, data) \
    do { \
        if (htb->is_index) { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_get_next(unit, module_id, htb, iter, (SW_STATE_HASH_TABLE_KEY*)key, NULL, (uint32*)data)); \
        } else { \
            SHR_IF_ERR_EXIT(sw_state_hash_table_get_next(unit, module_id, htb, iter, (SW_STATE_HASH_TABLE_KEY*)key, (SW_STATE_HASH_TABLE_DATA*)data, NULL)); \
        } \
    } while(0)

#define SW_STATE_HTB_CLEAR(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_clear(unit, module_id, htb))

#define SW_STATE_HTB_DELETE(module_id, htb, key) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key))

#define SW_STATE_HTB_DELETE_ALL(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove_all(unit, module_id, htb))

#define SW_STATE_HTB_PRINT(module_id, htb) \
    SHR_IF_ERR_EXIT(sw_state_htb_print(unit, module_id, htb))

#define SW_STATE_HTB_DEFAULT_PRINT(unit, module_id, htb) \
    sw_state_htb_print(unit, module_id, *(htb))

#define SW_STATE_HTB_INSERT_AT_INDEX(module_id, htb, key, data_indx, success) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add_at_index(unit, module_id, htb, (SW_STATE_HASH_TABLE_KEY*)key, data_indx, success))

#define SW_STATE_HTB_DELETE_BY_INDEX(module_id, htb, data_indx) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove_by_index(unit, module_id, htb, data_indx))

#define SW_STATE_HTB_GET_BY_INDEX(module_id, htb, data_indx, key, found) \
    SHR_IF_ERR_EXIT(sw_state_hash_table_get_by_index(unit, module_id, htb, data_indx, (SW_STATE_HASH_TABLE_KEY*)key, found))

#define SW_STATE_HTB_SIZE_GET(init_info, key_type, value_type, is_index) \
    sw_state_hash_table_size_get(init_info, sizeof(key_type), sizeof(value_type), is_index)

#define SW_STATE_HASH_TABLE_ITER_SET_BEGIN(iter) ((*iter) = 0)

#define SW_STATE_HASH_TABLE_ITER_IS_END(iter)    ((*iter) == UTILEX_U32_MAX)

#define SW_STATE_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table) \
  { \
    if (hash_table == NULL) { \
      SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL) ; \
      SHR_EXIT() ; \
    } \
  }

#define SW_STATE_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
     \
    SHR_SET_CURRENT_ERR(_SHR_E_PARAM) ; \
    SHR_EXIT() ; \
  }

typedef uint32 SW_STATE_HASH_TABLE_PTR;

typedef uint8 SW_STATE_HASH_TABLE_KEY;

typedef uint8 SW_STATE_HASH_TABLE_DATA;

typedef uint32 SW_STATE_HASH_TABLE_ITER;

typedef uint32 (
    *SW_STATE_HASH_MAP_HASH_FUN_CALL_BACK) (
    SW_STATE_HASH_TABLE_KEY * const key,
    uint32 seed,
    uint32 *hash_val);

typedef struct
{

    uint32 max_nof_elements;

    uint32 expected_nof_elements;

    SW_STATE_HASH_MAP_HASH_FUN_CALL_BACK hash_function;

    char *print_cb_name;

} sw_state_htbl_init_info_t;

typedef struct
{

    DNX_SW_STATE_BUFF *lists_head;

    DNX_SW_STATE_BUFF *keys;

    DNX_SW_STATE_BUFF *data;

    DNX_SW_STATE_BUFF *next;

    uint32 ptr_size;

    uint32 key_size;

    uint32 data_size;

    sw_state_occ_bm_t memory_use;

    uint32 null_ptr;

    uint8 is_index;

    char print_cb_name[SW_STATE_CB_DB_NAME_STR_SIZE];

    sw_state_cb_t print_cb_db;

    sw_state_htbl_init_info_t init_info;
}  *sw_state_htbl_t;

shr_error_e sw_state_hash_table_create(
    int unit,
    uint32 module_id,
    sw_state_htbl_t * hash_table,
    sw_state_htbl_init_info_t * init_info,
    uint32 key_size,
    uint32 data_size,
    uint8 is_index,
    uint32 nof_elements,
    uint32 alloc_flags);

shr_error_e sw_state_hash_table_destroy(
    int unit,
    uint32 module_id,
    sw_state_htbl_t * hash_table_ptr);

shr_error_e sw_state_hash_table_entry_add(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key,
    SW_STATE_HASH_TABLE_DATA * const data,
    uint32 *data_indx,
    uint8 *success);

shr_error_e sw_state_hash_table_entry_add_at_index(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key,
    uint32 data_indx,
    uint8 *success);

shr_error_e sw_state_hash_table_entry_remove(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key);

shr_error_e sw_state_hash_table_entry_remove_all(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

shr_error_e sw_state_hash_table_entry_remove_by_index(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    uint32 data_indx);

shr_error_e sw_state_hash_table_entry_lookup(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_KEY * const key,
    SW_STATE_HASH_TABLE_DATA * data,
    uint32 *data_indx,
    uint8 *found);

shr_error_e sw_state_hash_table_get_by_index(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    uint32 data_indx,
    SW_STATE_HASH_TABLE_KEY * key,
    uint8 *found);

shr_error_e sw_state_hash_table_clear(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

shr_error_e sw_state_hash_table_get_next(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    SW_STATE_HASH_TABLE_ITER * iter,
    SW_STATE_HASH_TABLE_KEY * const key,
    SW_STATE_HASH_TABLE_DATA * const data,
    uint32 *data_indx);

shr_error_e sw_state_hash_table_info_clear(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

shr_error_e sw_state_htb_print(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

int sw_state_hash_table_size_get(
    sw_state_htbl_init_info_t * init_info,
    uint32 key_size,
    uint32 data_size,
    uint8 is_index);

shr_error_e sw_state_hash_table_get_num_active(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table,
    uint32 *num_active_ptr);

#ifdef SW_STATE_DEBUG

shr_error_e sw_state_hash_table_info_print(
    int unit,
    uint32 module_id,
    sw_state_htbl_t hash_table);

shr_error_e sw_state_htbl_test_1(
    int unit);

#endif

#endif
