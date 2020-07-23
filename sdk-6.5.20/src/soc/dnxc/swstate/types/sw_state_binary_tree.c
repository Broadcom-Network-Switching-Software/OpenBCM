
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <assert.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_binary_tree.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/drv.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#define SW_STATE_BT_CHECK_VALID_NODE(param) \
    do { \
        if ((param) == SW_STATE_BT_INVALID) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state bt ERROR: SW_STATE_BT_INVALID argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)

#define SW_STATE_BT_CHECK_NULL_PARAMETER(param) \
    do { \
        if ((param) == NULL) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state bt ERROR: NULL argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)

#define VALUE_AT_IDX(idx)                 ((uint8 *) bt->values + (bt->value_size * idx))

#define SW_STATE_BT_LOCAL_SET_FREE_LIST_HEAD(value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(bt->free_list_head), &value, sizeof(sw_state_bt_node_t), 0x0, "head")

#define SW_STATE_BT_LOCAL_SET_ROOT(value) \
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(bt->root), &value, sizeof(sw_state_bt_node_t), 0x0, "head")

#define SW_STATE_BT_LOCAL_SET_PARENT(index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(bt->parents[index]), &value, sizeof(sw_state_bt_node_t), 0x0, "parent")
    
#define SW_STATE_BT_LOCAL_SET_LEFT(index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(bt->lchilds[index]), &value, sizeof(sw_state_bt_node_t), 0x0, "left")

#define SW_STATE_BT_LOCAL_SET_RIGHT(index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(bt->rchilds[index]), &value, sizeof(sw_state_bt_node_t), 0x0, "right")

const uint32 sw_state_bt_invalid = SW_STATE_BT_INVALID;


int
sw_state_bt_create_empty_size_get(
    sw_state_bt_init_info_t *init_info,
    uint32 value_size)
{
    int size = 0;
    uint32 elements_to_alloc;

    elements_to_alloc = init_info->max_nof_elements + 1;

    
    size += (sizeof(sw_state_bt_t));

    
    size += (sizeof(uint8)*(value_size * elements_to_alloc));

    
    size += (sizeof(sw_state_bt_node_t)*(elements_to_alloc));
    
    
    size += (sizeof(sw_state_bt_node_t)*(elements_to_alloc));

    
    size += (sizeof(sw_state_bt_node_t)*(elements_to_alloc));

    return size;
}


STATIC int sw_state_bt_create_new_node(int unit, uint32 module_id, sw_state_bt_t bt, 
                                sw_state_bt_node_t parent, uint32 is_left_child, const uint8 *value,
                                sw_state_bt_node_t *created_node)
{
    uint8 is_suppressed = FALSE;
    sw_state_bt_node_t curr_node;
    
    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(bt != NULL);
    assert(created_node != NULL);

    
    if (bt->free_list_head == SW_STATE_BT_INVALID) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state bt ERROR: bt is full.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    curr_node = bt->free_list_head;
    (*created_node) = curr_node;

    SW_STATE_BT_LOCAL_SET_FREE_LIST_HEAD(bt->rchilds[bt->free_list_head]);

    assert(bt->lchilds[curr_node] == SW_STATE_BT_INVALID);
    assert(bt->parents[curr_node] == SW_STATE_BT_INVALID);

    if(bt->free_list_head != SW_STATE_BT_INVALID) {
        assert(bt->lchilds[bt->free_list_head] == curr_node);
        assert(bt->parents[bt->free_list_head] == SW_STATE_BT_INVALID);
        SW_STATE_BT_LOCAL_SET_LEFT(bt->free_list_head, sw_state_bt_invalid);
    }

    SW_STATE_BT_LOCAL_SET_PARENT(curr_node, parent);
    SW_STATE_BT_LOCAL_SET_LEFT(curr_node, sw_state_bt_invalid);
    SW_STATE_BT_LOCAL_SET_RIGHT(curr_node, sw_state_bt_invalid);

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, VALUE_AT_IDX(curr_node), value, bt->value_size, 0x0, "sw state bt node's value");

    if (parent != SW_STATE_BT_INVALID) {
        if (is_left_child) {
            SW_STATE_BT_LOCAL_SET_LEFT(parent, curr_node);
        } else {
            SW_STATE_BT_LOCAL_SET_RIGHT(parent, curr_node);
        }
    } else {    
        SW_STATE_BT_LOCAL_SET_ROOT(curr_node);
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_bt_create_empty(int unit, uint32 module_id, sw_state_bt_init_info_t *init_info, uint32 value_size, sw_state_bt_t * bt_ptr, uint32 max_nof_elements, uint32 alloc_flags)
{

    uint32 elements_to_alloc;
    sw_state_bt_t bt;
    uint32 node, next_node;
    uint8 is_suppressed = FALSE;
    uint32 el_alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(bt_ptr != NULL);
    SW_STATE_BT_CHECK_NULL_PARAMETER(init_info);

    if (*bt_ptr != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state bt create ERROR: bt already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    if (max_nof_elements != 0)
    {
        
        elements_to_alloc = max_nof_elements + 1;
        el_alloc_flags = alloc_flags;
    }
    else
    {
        
        elements_to_alloc = init_info->max_nof_elements + 1;
        el_alloc_flags = DNXC_SW_STATE_NO_FLAGS;
    }

    DNX_SW_STATE_ALLOC(unit, module_id, *bt_ptr, **bt_ptr,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state binary tree");
    bt = *bt_ptr;

    
    DNX_SW_STATE_ALLOC(unit, module_id, bt->values, uint8, value_size * elements_to_alloc, el_alloc_flags, "bt values");
    DNX_SW_STATE_ALLOC(unit, module_id, bt->parents, sw_state_bt_node_t, elements_to_alloc, el_alloc_flags, "bt parents");
    DNX_SW_STATE_ALLOC(unit, module_id, bt->lchilds, sw_state_bt_node_t, elements_to_alloc, el_alloc_flags, "bt lchilds");
    DNX_SW_STATE_ALLOC(unit, module_id, bt->rchilds, sw_state_bt_node_t, elements_to_alloc, el_alloc_flags, "bt rchilds");

    
    node = 1;
    SW_STATE_BT_LOCAL_SET_FREE_LIST_HEAD(node);
    
    SW_STATE_BT_LOCAL_SET_LEFT(node, sw_state_bt_invalid);

    
    if(SOC_IS_DONE_INIT(unit)) {
        for (node = 1; node < (init_info->max_nof_elements); node++) {
            next_node = node + 1;
            SW_STATE_BT_LOCAL_SET_PARENT(node, sw_state_bt_invalid);
            SW_STATE_BT_LOCAL_SET_LEFT(next_node, node);
            SW_STATE_BT_LOCAL_SET_RIGHT(node, next_node);
        }
    } else {
        for (node = 2; node < (init_info->max_nof_elements); node++) {
            next_node = node + 1;
            bt->parents[node] = SW_STATE_BT_INVALID;
            bt->lchilds[next_node] = node;
            bt->rchilds[node] = next_node;
        }
    }

    node = init_info->max_nof_elements;
    SW_STATE_BT_LOCAL_SET_PARENT(node, sw_state_bt_invalid);
    SW_STATE_BT_LOCAL_SET_RIGHT(node, sw_state_bt_invalid);

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &bt->value_size, &value_size, sizeof(value_size), 0x0, "value_size");

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


STATIC int sw_state_bt_add_internal(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node_parent, uint8 is_left_child, const uint8* value)
{

    sw_state_bt_node_t created_node;
    uint8 is_suppressed = FALSE;
    
    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "binary tree bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    assert(bt != NULL);
    assert(value != NULL);

    
    if (bt->root == SW_STATE_BT_INVALID) {
        
        assert(node_parent == SW_STATE_BT_INVALID);

        SHR_IF_ERR_EXIT(sw_state_bt_create_new_node(unit, module_id, bt, SW_STATE_BT_INVALID, 0, value, &created_node));

        SW_STATE_BT_LOCAL_SET_ROOT(created_node);

        SOC_EXIT;
    } else {
        
        
        assert(node_parent != SW_STATE_BT_INVALID);
        SHR_IF_ERR_EXIT(sw_state_bt_create_new_node(unit, module_id, bt, node_parent, is_left_child, value, &created_node));
        SOC_EXIT;
    }
    
exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_bt_add_root(int unit, uint32 module_id, sw_state_bt_t bt, const uint8 *value)
{

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_NULL_PARAMETER(value);

    return sw_state_bt_add_internal(unit, module_id, bt, SW_STATE_BT_INVALID, 0, value);

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_add_left_child(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node_parent, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    assert(bt != NULL);
    SW_STATE_BT_CHECK_NULL_PARAMETER(value);
    SW_STATE_BT_CHECK_VALID_NODE(node_parent);

    return sw_state_bt_add_internal(unit, module_id, bt, node_parent, 1, value);
    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_add_right_child(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node_parent, const uint8 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    assert(bt != NULL);
    SW_STATE_BT_CHECK_NULL_PARAMETER(value);
    SW_STATE_BT_CHECK_VALID_NODE(node_parent);

    return sw_state_bt_add_internal(unit, module_id, bt, node_parent, 0, value);
    DNXC_SW_STATE_FUNC_RETURN; 
}


int sw_state_bt_node_update(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node, const uint8 *value)
{
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_BT_CHECK_VALID_NODE(node);
    SW_STATE_BT_CHECK_NULL_PARAMETER(value);
    
    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, VALUE_AT_IDX(node), value, bt->value_size, 0x0, "sw state bt node's value");

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_bt_node_free(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    if (bt->free_list_head == SW_STATE_BT_INVALID) {
        
        SW_STATE_BT_LOCAL_SET_FREE_LIST_HEAD(node);

        SW_STATE_BT_LOCAL_SET_LEFT(node, sw_state_bt_invalid);
        SW_STATE_BT_LOCAL_SET_RIGHT(node, sw_state_bt_invalid);
        SW_STATE_BT_LOCAL_SET_PARENT(node, sw_state_bt_invalid);
    }
    else {
        SW_STATE_BT_LOCAL_SET_LEFT(bt->free_list_head, node);

        SW_STATE_BT_LOCAL_SET_LEFT(node, sw_state_bt_invalid);
        SW_STATE_BT_LOCAL_SET_RIGHT(node, bt->free_list_head);
        SW_STATE_BT_LOCAL_SET_PARENT(node, sw_state_bt_invalid);

        SW_STATE_BT_LOCAL_SET_FREE_LIST_HEAD(node);
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_bt_get_root(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t *output_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_NULL_PARAMETER(output_node);

    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = bt->root;

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_get_parent(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t input_node, sw_state_bt_node_t *output_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_VALID_NODE(input_node);
    SW_STATE_BT_CHECK_NULL_PARAMETER(output_node);

    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = bt->parents[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_get_left_child(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t input_node, sw_state_bt_node_t *output_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_VALID_NODE(input_node);
    SW_STATE_BT_CHECK_NULL_PARAMETER(output_node);

    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = bt->lchilds[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_get_right_child(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t input_node, sw_state_bt_node_t *output_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_VALID_NODE(input_node);
    SW_STATE_BT_CHECK_NULL_PARAMETER(output_node);
    
    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    *output_node = bt->rchilds[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_set_parent(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node, sw_state_bt_node_t node_parent)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_VALID_NODE(node);
    
    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_BT_LOCAL_SET_PARENT(node, node_parent);

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_set_left_child(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node, sw_state_bt_node_t node_left)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_VALID_NODE(node);
    
    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_BT_LOCAL_SET_LEFT(node, node_left);

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_set_right_child(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node, sw_state_bt_node_t node_right)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_BT_CHECK_VALID_NODE(node);
    
    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to add to inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_BT_LOCAL_SET_RIGHT(node, node_right);

    DNXC_SW_STATE_FUNC_RETURN;
}

int sw_state_bt_nof_elements_internal(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node, uint32 *nof_elements)
{
    uint32 count_left = 0, count_right = 0;    
    sw_state_bt_node_t lchild = SW_STATE_BT_INVALID;
    sw_state_bt_node_t rchild = SW_STATE_BT_INVALID;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (node == SW_STATE_BT_INVALID) {
        *nof_elements = 0;
    } else {
        SHR_IF_ERR_EXIT(sw_state_bt_get_left_child(unit, module_id, bt, node, &lchild));
        SHR_IF_ERR_EXIT(sw_state_bt_get_right_child(unit, module_id, bt, node, &rchild));
        
        SHR_IF_ERR_EXIT(sw_state_bt_nof_elements_internal(unit, module_id, bt, lchild, &count_left));
        SHR_IF_ERR_EXIT(sw_state_bt_nof_elements_internal(unit, module_id, bt, rchild, &count_right));
        
        *nof_elements = count_left + count_right + 1;
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_nof_elements(int unit, uint32 module_id, sw_state_bt_t bt, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to access inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_BT_CHECK_NULL_PARAMETER(nof_elements);
    
    SHR_IF_ERR_EXIT(sw_state_bt_nof_elements_internal(unit, module_id, bt, bt->root, nof_elements));

    DNXC_SW_STATE_FUNC_RETURN;
}


STATIC int sw_state_bt_nof_unused_elements(int unit, uint32 module_id, sw_state_bt_t bt, uint32 *nof_elements)
{
    sw_state_bt_node_t node;
    int count = 0;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if (bt == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state bt ERROR: trying to access inactive bt.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_BT_CHECK_NULL_PARAMETER(nof_elements);
    
    node = bt->free_list_head;

    while (node != SW_STATE_BT_INVALID) {
        count++;
        node = bt->rchilds[node];
    }

    *nof_elements = count;

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_node_value(int unit, uint32 module_id, sw_state_bt_t bt, sw_state_bt_node_t node, uint8 *value)
{
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(bt != NULL);

    SW_STATE_BT_CHECK_VALID_NODE(node);
    SW_STATE_BT_CHECK_NULL_PARAMETER(value);

    sal_memcpy(value, VALUE_AT_IDX(node), bt->value_size);

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_bt_destroy(int unit, uint32 module_id, sw_state_bt_t *bt_ptr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert((*bt_ptr) != NULL);

    
    

    DNXC_SW_STATE_FUNC_RETURN;
}

STATIC DNXC_SW_STATE_INLINE void
sw_state_bt_data_hexdump(int unit, uint8 *data, uint32 size)
{
    int i;
    assert(data != NULL);
    
    for (i = 0; i < size; i++)
    {
        DNX_SW_STATE_PRINT(unit, "%02X", data[i]);
    }
}


int sw_state_bt_print_internal(int unit, sw_state_bt_t bt, sw_state_bt_node_t node)
{
    sw_state_bt_node_t lchild = SW_STATE_BT_INVALID;
    sw_state_bt_node_t rchild = SW_STATE_BT_INVALID;
    
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if (node != SW_STATE_BT_INVALID) {
        DNX_SW_STATE_PRINT(unit, " node -> ");
        sw_state_bt_data_hexdump(unit, VALUE_AT_IDX(node), bt->value_size);
        DNX_SW_STATE_PRINT(unit, "\n");
        
        SHR_IF_ERR_EXIT(sw_state_bt_get_left_child(unit, -1, bt, node, &lchild));
        SHR_IF_ERR_EXIT(sw_state_bt_get_right_child(unit, -1, bt, node, &rchild));

        SHR_IF_ERR_EXIT(sw_state_bt_print_internal(unit, bt, lchild));
        SHR_IF_ERR_EXIT(sw_state_bt_print_internal(unit, bt, rchild));
    } else {
        DNX_SW_STATE_PRINT(unit, " node -> NULL\n");
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_bt_print(int unit, sw_state_bt_t bt)
{
    uint32 nof_elements;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(bt != NULL);

    
    SHR_IF_ERR_EXIT(sw_state_bt_nof_unused_elements(unit, -1, bt, &nof_elements));
    DNX_SW_STATE_PRINT(unit, "Printing Binary Tree:\n");

    DNX_SW_STATE_PRINT(unit, "Number of unused nodes: %d\n", nof_elements);

    SHR_IF_ERR_EXIT(sw_state_bt_print_internal(unit, bt, bt->root));

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


