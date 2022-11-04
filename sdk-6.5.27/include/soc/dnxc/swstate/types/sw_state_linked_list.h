/** \file sw_state_linked_list.h
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * INFO: linked list header file
 *
 */
#ifndef _DNX_SW_STATE_LINKED_LIST_H

#define _DNX_SW_STATE_LINKED_LIST_H

#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/error.h>
#include <shared/bsl.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>

#define SW_STATE_LL_INVALID               (0)

#define DNX_SW_STATE_LL_IS_NODE_VALID(node)\
    (node != SW_STATE_LL_INVALID)

#define SW_STATE_LL_NO_FLAGS  0x0
#define SW_STATE_LL_SORTED    0x1
#define SW_STATE_LL_MULTIHEAD 0x2

typedef int non_existing_dummy_type_t;

typedef int32 (
    *sw_state_sorted_ll_key_cmp_cb) (
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

typedef struct sw_state_ll_init_info_s
{
    uint32 max_nof_elements;
    char *key_cmp_cb_name;
    uint32 nof_heads;
} sw_state_ll_init_info_t;

typedef uint32 sw_state_ll_node_t;

typedef struct sw_state_ll_s
{
    SHR_BITDCL *head;
    SHR_BITDCL *tail;
    uint8 is_sorted;
    uint32 max_level;
    uint32 curr_level;
    uint32 nof_heads;
    uint32 nof_regular_nodes;
    uint32 nof_service_nodes;
    uint32 value_size;
    uint32 key_size;
    sw_state_cb_t key_cmp_cb;
    uint8 *keys;
    uint8 *values;
    SHR_BITDCL *nexts;
    SHR_BITDCL *prevs;
    SHR_BITDCL *service_downs;
    SHR_BITDCL *service_nexts;
    SHR_BITDCL *service_pointers_keys;
    SHR_BITDCL *service_head;
    uint32 max_elements;
    uint32 ptr_size;
    sw_state_ll_node_t *sandbox;
}  *sw_state_ll_t;

typedef sw_state_ll_t sw_state_sorted_ll_t;
typedef sw_state_ll_t sw_state_multihead_ll_t;
typedef sw_state_ll_t sw_state_sorted_multihead_ll_t;

shr_error_e sw_state_ll_create_empty(
    int unit,
    uint32 node_id,
    sw_state_ll_init_info_t * init_info,
    uint32 flags,
    uint32 key_size,
    uint32 value_size,
    sw_state_ll_t * ll_ptr,
    uint32 max_nof_elements,
    uint32 alloc_flags);

shr_error_e sw_state_ll_add(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    const void *key,
    const void *value);

shr_error_e sw_state_ll_add_first(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    const void *key,
    const void *value);

shr_error_e sw_state_ll_add_last(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    const void *key,
    const void *value);

shr_error_e sw_state_ll_add_before(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t node,
    const void *key,
    const void *value);

shr_error_e sw_state_ll_add_after(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t node,
    const void *key,
    const void *value);

shr_error_e sw_state_ll_remove_node(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t node);

shr_error_e sw_state_ll_node_update(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t node,
    const void *value);

shr_error_e sw_state_ll_next_node(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t input_node,
    sw_state_ll_node_t * output_node);

shr_error_e sw_state_ll_previous_node(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t input_node,
    sw_state_ll_node_t * output_node);

shr_error_e sw_state_ll_get_first(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t * output_node);

shr_error_e sw_state_ll_get_last(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t * output_node);

shr_error_e sw_state_ll_node_key(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t node,
    void *key);

shr_error_e sw_state_ll_node_value(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    sw_state_ll_node_t node,
    void *value);

shr_error_e sw_state_ll_nof_elements(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    uint32 *nof_elements);

shr_error_e sw_state_ll_print(
    int unit,
    sw_state_ll_t ll);

shr_error_e sw_state_ll_find(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index,
    sw_state_ll_node_t * node,
    const void *key,
    uint8 *found);

shr_error_e sw_state_ll_clear(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll);

shr_error_e sw_state_ll_clear_all_at_index(
    int unit,
    uint32 node_id,
    sw_state_ll_t ll,
    uint32 ll_index);

#define SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(node_id, ll, init_info, flags, key_type, value_type, max_nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_ll_create_empty(unit, node_id, init_info, flags, sizeof(key_type), sizeof(value_type), &ll, max_nof_elements, alloc_flags))

#define SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(node_id, ll, ll_index,nof_elements) \
    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, node_id, ll, ll_index, nof_elements))

#define SW_STATE_MULTIHEAD_LL_NODE_KEY(node_id, ll, node, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_node_key(unit, node_id, ll, node, value))

#define SW_STATE_MULTIHEAD_LL_NODE_VALUE(node_id, ll, node, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_node_value(unit, node_id, ll, node, value))

#define SW_STATE_MULTIHEAD_LL_NODE_UPDATE(node_id, ll, node, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_node_update(unit, node_id, ll, node, value))

#define SW_STATE_MULTIHEAD_LL_NEXT_NODE(node_id, ll, node, next_node) \
    SHR_IF_ERR_EXIT(sw_state_ll_next_node(unit, node_id, ll, node, next_node))

#define SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(node_id, ll, node, prev_node) \
    SHR_IF_ERR_EXIT(sw_state_ll_previous_node(unit, node_id, ll, node, prev_node))

#define SW_STATE_MULTIHEAD_LL_ADD(node_id, ll, ll_index,key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, ll, ll_index, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_FIRST(node_id, ll, ll_index,key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_first(unit, node_id, ll, ll_index, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_LAST(node_id, ll, ll_index,key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_last(unit, node_id, ll, ll_index, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_BEFORE(node_id, ll, ll_index,node, key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_before(unit, node_id, ll, ll_index, node, key, value))

#define SW_STATE_MULTIHEAD_LL_ADD_AFTER(node_id, ll, ll_index, node, key, value) \
    SHR_IF_ERR_EXIT(sw_state_ll_add_after(unit, node_id, ll, ll_index, node, key, value))

#define SW_STATE_MULTIHEAD_LL_REMOVE_NODE(node_id, ll, ll_index, node) \
    SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, ll, ll_index, node))

#define SW_STATE_MULTIHEAD_LL_GET_LAST(node_id, ll, ll_index, node) \
    SHR_IF_ERR_EXIT(sw_state_ll_get_last(unit, node_id, ll, ll_index, node))

#define SW_STATE_MULTIHEAD_LL_GET_FIRST(node_id, ll, ll_index, node) \
    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, ll, ll_index, node))

#define SW_STATE_MULTIHEAD_LL_FIND(node_id, ll, ll_index, node, key, found) \
    SHR_IF_ERR_EXIT(sw_state_ll_find(unit, node_id, ll, ll_index, node, key, found))

#define SW_STATE_MULTIHEAD_LL_PRINT(node_id, ll)\
    SHR_IF_ERR_EXIT(sw_state_ll_print(unit, ll))

#define SW_STATE_MULTIHEAD_LL_DEFAULT_PRINT(unit, ll)\
    sw_state_ll_print(unit, *(ll))

#define SW_STATE_MULTIHEAD_LL_CLEAR_ALL(node_id, ll)\
    SHR_IF_ERR_EXIT(sw_state_ll_clear(unit, node_id, ll))

#define SW_STATE_LL_CREATE_EMPTY(node_id, ll, init_info, flags, key_type, value_type, max_nof_elements, alloc_flags) \
    SW_STATE_MULTIHEAD_LL_CREATE_EMPTY(node_id, ll, init_info, flags, key_type, value_type, max_nof_elements, alloc_flags)

#define SW_STATE_LL_NOF_ELEMENTS(node_id, ll, nof_elements) \
    SW_STATE_MULTIHEAD_LL_NOF_ELEMENTS(node_id, ll, 0, nof_elements)

#define SW_STATE_LL_NODE_KEY(node_id, ll, node, value) \
    SW_STATE_MULTIHEAD_LL_NODE_KEY(node_id, ll, node, value)

#define SW_STATE_LL_NODE_VALUE(node_id, ll, node, value) \
    SW_STATE_MULTIHEAD_LL_NODE_VALUE(node_id, ll, node, value)

#define SW_STATE_LL_NODE_UPDATE(node_id, ll, node, value) \
    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(node_id, ll, node, value)

#define SW_STATE_LL_NEXT_NODE(node_id, ll, node, next_node) \
    SW_STATE_MULTIHEAD_LL_NEXT_NODE(node_id, ll, node, next_node)

#define SW_STATE_LL_PREVIOUS_NODE(node_id, ll, node, prev_node) \
    SW_STATE_MULTIHEAD_LL_PREVIOUS_NODE(node_id, ll, node, prev_node)

#define SW_STATE_LL_ADD(node_id, ll, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD(node_id, ll, 0, key, value)

#define SW_STATE_LL_ADD_FIRST(node_id, ll, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_FIRST(node_id, ll, 0, key, value)

#define SW_STATE_LL_ADD_LAST(node_id, ll, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_LAST(node_id, ll, 0, key, value)

#define SW_STATE_LL_ADD_BEFORE(node_id, ll, node, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_BEFORE(node_id, ll, 0, node, key, value)

#define SW_STATE_LL_ADD_AFTER(node_id, ll, node, key, value) \
    SW_STATE_MULTIHEAD_LL_ADD_AFTER(node_id, ll, 0, node, key, value)

#define SW_STATE_LL_REMOVE_NODE(node_id, ll, node) \
    SW_STATE_MULTIHEAD_LL_REMOVE_NODE(node_id, ll, 0, node)

#define SW_STATE_LL_GET_LAST(node_id, ll, node) \
    SW_STATE_MULTIHEAD_LL_GET_LAST(node_id, ll, 0, node)

#define SW_STATE_LL_GET_FIRST(node_id, ll, node) \
    SW_STATE_MULTIHEAD_LL_GET_FIRST(node_id, ll, 0, node)

#define SW_STATE_LL_FIND(node_id, ll, node, key, found) \
    SW_STATE_MULTIHEAD_LL_FIND(node_id, ll, 0, node, key, found)

#define SW_STATE_LL_PRINT(node_id, ll)\
    SW_STATE_MULTIHEAD_LL_PRINT(node_id, ll)

#define SW_STATE_LL_DEFAULT_PRINT(unit, ll)\
    SW_STATE_MULTIHEAD_LL_DEFAULT_PRINT(unit, ll)

#define SW_STATE_LL_CLEAR_ALL(node_id, ll)\
    SW_STATE_MULTIHEAD_LL_CLEAR_ALL(node_id, ll)

#endif
