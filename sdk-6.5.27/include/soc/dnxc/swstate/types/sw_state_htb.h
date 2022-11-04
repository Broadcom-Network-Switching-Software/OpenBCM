/* \file sw_state_htb.h
 *
 * DNX hash table create APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_HTB_RB_H_
#define INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_HTB_RB_H_

#include <soc/dnxc/swstate/sw_state_features.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <shared/bitop.h>
#include <include/soc/dnxc/swstate/types/sw_state_htb_create.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>

#define SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb) \
  { \
    if (htb == NULL) { \
      SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL) ; \
      SHR_EXIT() ; \
    } \
  }

#define SW_STATE_HTB_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
     \
    SHR_SET_CURRENT_ERR(_SHR_E_PARAM) ; \
    SHR_EXIT() ; \
  }

#define SW_STATE_HTB_RH_HASH_ALGORITHM_MAX_NAME_LENGTH 100
#define SW_STATE_HTB_RH_KEY_NOT_PROVIDED_IN_XML        0
#define SW_STATE_HTB_RH_DATA_NOT_PROVIDED_IN_XML       0

#define SW_STATE_HTB_RH_ITER_SET_BEGIN(iter) ((*iter) = 0)

#define SW_STATE_HTB_RH_ITER_IS_END(iter)    ((*iter) == UTILEX_U32_MAX)

#define SW_STATE_MULTIHEAD_HTB_RH_CREATE(node_id, htb, init_info, key_size, data_size, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_htb_rh_create(unit, node_id, &htb, init_info, key_size, data_size, alloc_flags))

#define SW_STATE_MULTIHEAD_HTB_RH_INSERT(htb, table_idx, key, data) \
    SHR_IF_ERR_EXIT(sw_state_htb_rh_insert(unit, htb, table_idx, (uint8 *)key, (uint8 *)data))

#define SW_STATE_MULTIHEAD_HTB_RH_REPLACE(htb, table_idx, key, data) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_htb_rh_replace(unit, htb, table_idx, (uint8 *)key, (uint8 *)data), _SHR_E_NOT_FOUND)

#define SW_STATE_MULTIHEAD_HTB_RH_FIND(htb, table_idx, key, data) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_htb_rh_find(unit, htb, table_idx, (uint8 *)key, (uint8 *)data, NULL), _SHR_E_NOT_FOUND)

#define SW_STATE_MULTIHEAD_HTB_RH_GET_NEXT(htb, table_idx, iter, key, data) \
    SHR_IF_ERR_EXIT(sw_state_htb_rh_get_next(unit, htb, table_idx, iter,(uint8 *)key, (uint8 *)data))

#define SW_STATE_MULTIHEAD_HTB_RH_DELETE(htb, table_idx, key) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_htb_rh_delete(unit, htb, table_idx, (uint8 *)key), _SHR_E_NOT_FOUND)

#define SW_STATE_MULTIHEAD_HTB_RH_DELETE_ALL(htb, table_idx) \
    SHR_IF_ERR_EXIT(sw_state_htb_rh_delete_all(unit, htb, table_idx))

#define SW_STATE_MULTIHEAD_HTB_RH_DELETE_ALL_TABLES(htb) \
    SHR_IF_ERR_EXIT(sw_state_htb_rh_delete_all_tables(unit, htb))

#define SW_STATE_MULTIHEAD_HTB_RH_PRINT(htb) \
    SHR_IF_ERR_EXIT(sw_state_htb_rh_print(unit, htb))

#define SW_STATE_MULTIHEAD_HTB_RH_TRAVERSE(htb, table_idx, traverse_func, user_data) \
     SHR_IF_ERR_EXIT(sw_state_htb_rh_traverse(unit, htb, table_idx, traverse_func, user_data))

#define SW_STATE_HTB_RH_CREATE(node_id, htb, init_info, key_size, data_size, alloc_flags) \
    SW_STATE_MULTIHEAD_HTB_RH_CREATE(node_id, htb, init_info, key_size, data_size, alloc_flags)

#define SW_STATE_HTB_RH_INSERT(htb, key, data) \
    SW_STATE_MULTIHEAD_HTB_RH_INSERT(htb, 0, key, data)

#define SW_STATE_HTB_RH_REPLACE(htb, key, data) \
    SW_STATE_MULTIHEAD_HTB_RH_REPLACE(htb, 0, key, data)

#define SW_STATE_HTB_RH_FIND(htb, key, data) \
    SW_STATE_MULTIHEAD_HTB_RH_FIND(htb, 0, key, data)

#define SW_STATE_HTB_RH_GET_NEXT(htb, iter, key, data) \
    SW_STATE_MULTIHEAD_HTB_RH_GET_NEXT(htb, 0, iter, key, data)

#define SW_STATE_HTB_RH_DELETE(htb, key) \
    SW_STATE_MULTIHEAD_HTB_RH_DELETE(htb, 0, key)

#define SW_STATE_HTB_RH_DELETE_ALL(htb) \
    SW_STATE_MULTIHEAD_HTB_RH_DELETE_ALL(htb, 0)

#define SW_STATE_HTB_RH_PRINT(htb) \
    SW_STATE_MULTIHEAD_HTB_RH_PRINT(htb)

#define SW_STATE_HTB_RH_TRAVERSE(htb, traverse_func, user_data) \
    SW_STATE_MULTIHEAD_HTB_RH_TRAVERSE(htb, 0, traverse_func, user_data)

#define SW_STATE_HTB_RH_DEFAULT_PRINT(unit, htb) \
    sw_state_htb_rh_print(unit, *(htb))

typedef shr_error_e(
    *sw_state_htb_traverse_cb) (
    int unit,
    void *key,
    void *data,
    void *user_data);

typedef struct sw_state_htb_sandbox_s
{

    DNX_SW_STATE_BUFF *ll_node;

    DNX_SW_STATE_BUFF *ll_node_swap;
    DNX_SW_STATE_BUFF *key;
    DNX_SW_STATE_BUFF *key_swap;
    DNX_SW_STATE_BUFF *get_next_key;
    DNX_SW_STATE_BUFF *data;
    DNX_SW_STATE_BUFF *data_swap;
    DNX_SW_STATE_BUFF psl;
    DNX_SW_STATE_BUFF curr_psl;
    DNX_SW_STATE_BUFF table_idx;
    DNX_SW_STATE_BUFF table_idx_swap;
} sw_state_htb_sandbox_t;

typedef struct
{

    uint32 node_id;

    DNX_SW_STATE_BUFF *metadata;

    uint8 node_size;

    uint8 node_offset;

    uint8 table_idx_size;

    uint8 table_idx_offset;

    uint32 key_size;

    uint32 key_offset;

    uint32 data_size;

    uint32 data_offset;

    uint8 psl_active_size;

    uint32 psl_active_offset;

    uint32 metadata_size;

    uint32 max_nof_elements;

    uint32 max_allowed_nof_elements;

    uint32 nof_used_elements;

    uint8 load_factor;

    sw_state_htb_hash_func_e hash_func;

    uint32 flags;

    uint8 nof_htbs;

    char print_cb_name[SW_STATE_CB_DB_NAME_STR_SIZE];
    sw_state_cb_t print_cb_db;

    struct sw_state_htb_s *reverse_map;

    sw_state_htb_sandbox_t sandbox;

    sw_state_ll_t traverse_ll;
}  *sw_state_htb_t;

typedef sw_state_htb_t sw_state_multihead_htb_t;

typedef uint32 (
    *sw_state_htb_hashing) (
    int unit,
    sw_state_htb_t htb,
    uint8 *const key);

typedef struct
{
    char hash_name[SW_STATE_HTB_RH_HASH_ALGORITHM_MAX_NAME_LENGTH];
    sw_state_htb_hashing hash_cb;
} sw_state_htb_hash_algorithm_cb_t;

shr_error_e sw_state_htb_rh_create(
    int unit,
    uint32 node_id,
    sw_state_htb_t * htb,
    sw_state_htb_create_info_t * create_info,
    uint32 key_size,
    uint32 data_size,
    uint32 alloc_flags);

shr_error_e sw_state_htb_rh_insert(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *const data);

shr_error_e sw_state_htb_rh_find(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *data,
    uint32 *offset);

shr_error_e sw_state_htb_rh_delete(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key);

shr_error_e sw_state_htb_rh_traverse(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    sw_state_htb_traverse_cb traverse_func,
    void *user_data);

shr_error_e sw_state_htb_rh_replace(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *const data);

shr_error_e sw_state_htb_find_reverse(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const data,
    void *key);

shr_error_e sw_state_htb_rh_print(
    int unit,
    sw_state_htb_t htb);

shr_error_e sw_state_htb_rh_delete_all(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx);

shr_error_e sw_state_htb_rh_delete_all_tables(
    int unit,
    sw_state_htb_t htb);

shr_error_e sw_state_htb_rh_get_next(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    uint32 *iter,
    void *key,
    void *data);

#endif
