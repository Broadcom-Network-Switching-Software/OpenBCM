/** \file sw_state_binary_tree.h
 *  
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: binary tree header file
 */
#ifndef _DNX_SW_STATE_BINARY_TREE_H

#define _DNX_SW_STATE_BINARY_TREE_H

#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/error.h>
#include <shared/bsl.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>

#define SW_STATE_BT_INVALID               (0)

#define DNX_SW_STATE_BT_IS_NODE_VALID(node)\
    (node != SW_STATE_BT_INVALID)

typedef struct sw_state_bt_init_info_s
{
    uint32 max_nof_elements;
    uint32 expected_nof_elements;
} sw_state_bt_init_info_t;

typedef uint32 sw_state_bt_node_t;

typedef struct sw_state_bt_s
{
    sw_state_bt_node_t free_list_head;
    sw_state_bt_node_t root;
    uint32 value_size;
    uint8 *values;
    sw_state_bt_node_t *parents;
    sw_state_bt_node_t *lchilds;
    sw_state_bt_node_t *rchilds;
}  *sw_state_bt_t;

int sw_state_bt_create_empty(
    int unit,
    uint32 module_id,
    sw_state_bt_init_info_t * init_info,
    uint32 value_size,
    sw_state_bt_t * bt_ptr,
    uint32 max_nof_elements,
    uint32 alloc_flags);

int sw_state_bt_add_root(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    const uint8 *value);

int sw_state_bt_add_left_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node_parent,
    const uint8 *value);

int sw_state_bt_add_right_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node_parent,
    const uint8 *value);

int sw_state_bt_node_update(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    const uint8 *value);

int sw_state_bt_node_free(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node);

int sw_state_bt_set_parent(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    sw_state_bt_node_t node_parent);

int sw_state_bt_set_left_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    sw_state_bt_node_t node_left);

int sw_state_bt_set_right_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    sw_state_bt_node_t node_right);

int sw_state_bt_get_root(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t * output_node);

int sw_state_bt_get_parent(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t input_node,
    sw_state_bt_node_t * output_node);

int sw_state_bt_get_left_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t input_node,
    sw_state_bt_node_t * output_node);

int sw_state_bt_get_right_child(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t input_node,
    sw_state_bt_node_t * output_node);

int sw_state_bt_destroy(
    int unit,
    uint32 module_id,
    sw_state_bt_t * bt_ptr);

int sw_state_bt_node_value(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    sw_state_bt_node_t node,
    uint8 *value);

int sw_state_bt_nof_elements(
    int unit,
    uint32 module_id,
    sw_state_bt_t bt,
    uint32 *nof_elements);

int sw_state_bt_print(
    int unit,
    sw_state_bt_t bt);

int sw_state_bt_create_empty_size_get(
    sw_state_bt_init_info_t * init_info,
    uint32 value_size);

#define SW_STATE_BT_CREATE_EMPTY(module_id, bt, init_info, value_type, max_nof_elements, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_bt_create_empty(unit, module_id, init_info, sizeof(value_type), &bt, max_nof_elements, alloc_flags))

#define SW_STATE_BT_DESTROY(module_id, bt) \
    SHR_IF_ERR_EXIT(sw_state_bt_destroy(unit, module_id, &bt))

#define SW_STATE_BT_NOF_ELEMENTS(module_id, bt, nof_elements) \
    SHR_IF_ERR_EXIT(sw_state_bt_nof_elements(unit, module_id, bt, nof_elements))

#define SW_STATE_BT_NODE_VALUE(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_node_value(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_NODE_UPDATE(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_node_update(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_NODE_FREE(module_id, bt, node) \
    SHR_IF_ERR_EXIT(sw_state_bt_node_free(unit, module_id, bt, node))

#define SW_STATE_BT_GET_ROOT(module_id, bt, root) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_root(unit, module_id, bt, root))

#define SW_STATE_BT_GET_PARENT(module_id, bt, node, node_parent) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_parent(unit, module_id, bt, node, node_parent))

#define SW_STATE_BT_GET_LEFT_CHILD(module_id, bt, node, node_left) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_left_child(unit, module_id, bt, node, node_left))

#define SW_STATE_BT_GET_RIGHT_CHILD(module_id, bt, node, node_right) \
    SHR_IF_ERR_EXIT(sw_state_bt_get_right_child(unit, module_id, bt, node, node_right))

#define SW_STATE_BT_ADD_ROOT(module_id, bt, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_add_root(unit, module_id, bt, (uint8 *)value))

#define SW_STATE_BT_ADD_LEFT_CHILD(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_add_left_child(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_ADD_RIGHT_CHILD(module_id, bt, node, value) \
    SHR_IF_ERR_EXIT(sw_state_bt_add_right_child(unit, module_id, bt, node, (uint8 *)value))

#define SW_STATE_BT_SET_PARENT(module_id, bt, node, node_parent) \
    SHR_IF_ERR_EXIT(sw_state_bt_set_parent(unit, module_id, bt, node, node_parent))

#define SW_STATE_BT_SET_LEFT_CHILD(module_id, bt, node, node_left) \
    SHR_IF_ERR_EXIT(sw_state_bt_set_left_child(unit, module_id, bt, node, node_left))

#define SW_STATE_BT_SET_RIGHT_CHILD(module_id, bt, node, node_right) \
    SHR_IF_ERR_EXIT(sw_state_bt_set_right_child(unit, module_id, bt, node, node_right))

#define SW_STATE_BT_PRINT(module_id, bt)\
    SHR_IF_ERR_EXIT(sw_state_bt_print(unit, bt))

#define SW_STATE_BT_DEFAULT_PRINT(unit, bt)\
    SHR_IF_ERR_EXIT(sw_state_bt_print(unit, *(bt)))

#define SW_STATE_BT_CREATE_EMPTY_SIZE_GET(init_info, value_type) \
    sw_state_bt_create_empty_size_get(init_info, sizeof(value_type))

#define SW_STATE_BT_SIZE_GET(init_info, value_type) \
        SW_STATE_BT_CREATE_EMPTY_SIZE_GET(init_info, value_type)

#endif
