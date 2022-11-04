/* \file sw_state_index_htb.h
 *
 * Definitions and prototypes for all common utilities related to hash table.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_INDEX_HTB_H_
#define INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_INDEX_HTB_H_

#include <soc/dnxc/swstate/types/sw_state_htb.h>

#define SW_STATE_INDEX_HTB_RH_ITER_SET_BEGIN(iter) \
    SW_STATE_HTB_RH_ITER_SET_BEGIN(iter)

#define SW_STATE_INDEX_HTB_RH_ITER_IS_END(iter)\
    SW_STATE_HTB_RH_ITER_IS_END(iter)

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_CREATE(node_id, htb, init_info, key_size, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_index_htb_rh_create(unit, node_id, &htb, init_info, key_size, alloc_flags))

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_INSERT(htb, table_idx, key, data_idx) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_index_htb_rh_insert(unit, htb, table_idx, (uint8 *)key, data_idx), _SHR_E_FULL)

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_FIND(htb, table_idx, key, data_idx) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_index_htb_rh_find(unit, htb, table_idx, (uint8 *)key, data_idx), _SHR_E_NOT_FOUND)\

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_GET_NEXT(htb, table_idx, iter, key, data) \
    SW_STATE_MULTIHEAD_HTB_RH_GET_NEXT(htb->htb_rh, table_idx, iter, key, data)

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE(htb, table_idx, key) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_index_htb_rh_delete(unit, htb, table_idx, (uint8 *)key), _SHR_E_NOT_FOUND)

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE_ALL(htb, table_idx) \
    SHR_IF_ERR_EXIT(sw_state_index_htb_rh_delete_all(unit, htb, table_idx))

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE_ALL_TABLES(htb) \
    SHR_IF_ERR_EXIT(sw_state_index_htb_rh_delete_all_tables(unit, htb))

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_TRAVERSE(htb, table_idx, traverse_func, user_data) \
    SW_STATE_MULTIHEAD_HTB_RH_TRAVERSE(htb->htb_rh, table_idx, traverse_func, user_data)

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_INSERT_AT_INDEX(htb, table_idx, key, data_idx) \
    SHR_IF_ERR_EXIT(sw_state_index_htb_rh_insert_at_index(unit, htb, table_idx, (uint8 *)key, data_idx))

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE_BY_INDEX(htb, table_idx, data_idx) \
    SHR_IF_ERR_EXIT(sw_state_index_htb_rh_delete_by_index(unit, htb, table_idx, data_idx))

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_GET_BY_INDEX(htb, table_idx, data_idx, key) \
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_index_htb_rh_get_by_index(unit, htb, table_idx, data_idx, (uint8 *)key), _SHR_E_NOT_FOUND)

#define SW_STATE_MULTIHEAD_INDEX_HTB_RH_PRINT(htb) \
    SW_STATE_MULTIHEAD_HTB_RH_PRINT(htb->htb_rh)

#define SW_STATE_INDEX_HTB_RH_CREATE(node_id, htb, init_info, key_size, alloc_flags) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_CREATE(node_id, htb, init_info, key_size, alloc_flags)

#define SW_STATE_INDEX_HTB_RH_INSERT(htb, key, data_idx) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_INSERT(htb, 0, key, data_idx)

#define SW_STATE_INDEX_HTB_RH_FIND(htb, key, data_idx) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_FIND(htb, 0, key, data_idx)

#define SW_STATE_INDEX_HTB_RH_GET_NEXT(htb, iter, key, data) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_GET_NEXT(htb, 0, iter, key, data)

#define SW_STATE_INDEX_HTB_RH_DELETE(htb, key) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE(htb, 0, key)

#define SW_STATE_INDEX_HTB_RH_DELETE_ALL(htb) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE_ALL(htb, 0)

#define SW_STATE_INDEX_HTB_RH_TRAVERSE(htb, traverse_func, user_data) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_TRAVERSE(htb, 0, traverse_func, user_data)

#define SW_STATE_INDEX_HTB_RH_INSERT_AT_INDEX(htb, key, data_idx) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_INSERT_AT_INDEX(htb, 0, key, data_idx)

#define SW_STATE_INDEX_HTB_RH_DELETE_BY_INDEX(htb, data_idx) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_DELETE_BY_INDEX(htb, 0, data_idx)

#define SW_STATE_INDEX_HTB_RH_GET_BY_INDEX(htb, data_idx, key) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_GET_BY_INDEX(htb, 0, data_idx, key)

#define SW_STATE_INDEX_HTB_RH_PRINT(htb) \
    SW_STATE_MULTIHEAD_INDEX_HTB_RH_PRINT(htb)

#define SW_STATE_INDEX_HTB_RH_DEFAULT_PRINT(unit, htb) \
    sw_state_htb_rh_print(unit, *(htb->htb_rh))

typedef struct sw_state_index_htb_sandbox_s
{

    DNX_SW_STATE_BUFF *key;

    uint32 data_idx;

} sw_state_index_htb_sandbox_t;

typedef struct
{

    sw_state_htb_t htb_rh;

    DNX_SW_STATE_BUFF *key_map;

    sw_state_index_htb_sandbox_t sandbox;

    sw_state_occ_bm_t data_indices_in_use;

}  *sw_state_index_htb_t;

typedef sw_state_index_htb_t sw_state_multihead_index_htb_t;

shr_error_e sw_state_index_htb_rh_create(
    int unit,
    uint32 node_id,
    sw_state_index_htb_t * index_htb,
    sw_state_htb_create_info_t * create_info,
    uint32 key_size,
    uint32 alloc_flags);

shr_error_e sw_state_index_htb_rh_insert(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key,
    uint32 *data_idx);

shr_error_e sw_state_index_htb_rh_find(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key,
    uint32 *data_idx);

shr_error_e sw_state_index_htb_rh_delete(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key);

shr_error_e sw_state_index_htb_rh_delete_all(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx);

shr_error_e sw_state_index_htb_rh_delete_all_tables(
    int unit,
    sw_state_index_htb_t index_htb);

shr_error_e sw_state_index_htb_rh_insert_at_index(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key,
    uint32 data_idx);

shr_error_e sw_state_index_htb_rh_delete_by_index(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    uint32 data_idx);

shr_error_e sw_state_index_htb_rh_get_by_index(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    uint32 data_idx,
    void *key);

#endif
