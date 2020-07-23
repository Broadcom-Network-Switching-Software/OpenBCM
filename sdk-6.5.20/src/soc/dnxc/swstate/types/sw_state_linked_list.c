
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
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX




#define SW_STATE_LL_CHECK_VALID_NODE(param) \
    do { \
        if ((param) == SW_STATE_LL_INVALID) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: SW_STATE_LL_INVALID argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)

#define SW_STATE_LL_CHECK_NULL_PARAMETER(param) \
    do { \
        if ((param) == NULL) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: NULL argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(0)



#define SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index) \
    do { \
        if (ll_index > ll->nof_heads && ll_index != -1) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: accessed multihead linked list out of bound. index=%d nof_heads=%d\n%s", ll_index, ll->nof_heads, EMPTY); \
        } \
    } while(0)

#define TRANSLATE_LL_IDX(index)           (index+1) 
#define REVERSE_TRANSLATE_LL_IDX(index)   (index-1) 

#define KEY_AT_IDX(idx) (idx < ll->max_elements) ? ((uint8 *) ll->keys + (ll->key_size * idx)) : ((uint8 *) ll->keys + (ll->key_size * ll->service_pointers_keys[TRANSLATE_LL_SERVICE_IDX(idx)]))

#define VALUE_AT_IDX(idx)                 ((uint8 *) ll->values + (ll->value_size * idx))

#define TRANSLATE_LL_SERVICE_IDX(index)           (index - ll->max_elements) 

#define IDX_IS_SERVICE_NODE(idx) (idx < ll->max_elements) ? 0 : 1

#define SW_STATE_LL_SET_HEAD(ll_index, value) \
    if (IDX_IS_SERVICE_NODE(value)) { \
         DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->service_head[ll_index]), &value, sizeof(sw_state_ll_node_t), 0x0, "service head") \
    } else { \
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->head[ll_index]), &value, sizeof(sw_state_ll_node_t), 0x0, "head") \
    }

#define SW_STATE_LL_GET_HEAD(ll_index, is_service_node)     (is_service_node) ? (ll->service_head[ll_index]) : (ll->head[ll_index])

#define SW_STATE_LL_SET_TAIL(ll_index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->tail[ll_index]), &value, sizeof(sw_state_ll_node_t), 0x0, "tail")

#define SW_STATE_LL_GET_TAIL(ll_index, tail_node) \
    tail_node = ll->tail[ll_index]

#define SW_STATE_LL_SET_NEXT(index, value) \
    if (IDX_IS_SERVICE_NODE(index)) { \
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->service_nexts[TRANSLATE_LL_SERVICE_IDX(index)]), &value, sizeof(sw_state_ll_node_t), 0x0, "service nexts") \
    } else { \
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->nexts[index]), &value, sizeof(sw_state_ll_node_t), 0x0, "next") \
    }
#define SW_STATE_LL_GET_NEXT(node)    (IDX_IS_SERVICE_NODE(node)) ? (ll->service_nexts[TRANSLATE_LL_SERVICE_IDX(node)]) : (ll->nexts[node])

#define SW_STATE_LL_SET_PREV(index, value) \
    if (IDX_IS_SERVICE_NODE(index)) { \
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->service_prevs[TRANSLATE_LL_SERVICE_IDX(index)]), &value, sizeof(sw_state_ll_node_t), 0x0, "service prevs")\
    } else { \
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->prevs[index]), &value, sizeof(sw_state_ll_node_t), 0x0, "prev") \
    }

#define SW_STATE_LL_GET_PREV(node)    (IDX_IS_SERVICE_NODE(node)) ? (ll->service_prevs[TRANSLATE_LL_SERVICE_IDX(node)]) : (ll->prevs[node])

#define SW_STATE_LL_SET_SERVICE_DOWN(index, value) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->service_downs[TRANSLATE_LL_SERVICE_IDX(index)]), &value, sizeof(sw_state_ll_node_t), 0x0, "service down")

#define SW_STATE_LL_GET_SERVICE_DOWN(node)    (IDX_IS_SERVICE_NODE(node)) ? (ll->service_downs[TRANSLATE_LL_SERVICE_IDX(node)]) : (SW_STATE_LL_INVALID)

#define SW_STATE_SKIP_LIST_LEVEL_PROBABILITY 2
#define SERVICE_NODE                         1

const sw_state_ll_node_t sw_state_ll_invalid = SW_STATE_LL_INVALID;
const uint8 sw_state_ll_head_defult_val = 0;

#ifdef DNX_SW_STATE_VERIFICATIONS
STATIC int sw_state_ll_verify_order(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *created_node){
    uint8 is_suppressed = FALSE;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;

    sw_state_ll_node_t next_tmp;
    sw_state_ll_node_t prev_tmp;
    sw_state_ll_node_t prev_prev_tmp;
    sw_state_ll_node_t down_tmp;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_CB_DB_GET_CB(
               module_id,
               ll->key_cmp_cb,
               &sorted_list_cmp_cb,
               dnx_sw_state_sorted_list_cb_get_cb
           );

     
    next_tmp = SW_STATE_LL_GET_NEXT((*created_node));
    if (next_tmp != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(*created_node), KEY_AT_IDX(next_tmp), ll->key_size) >= 0) {
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), *created_node));
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation. key must be smaller than next->key \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    prev_tmp = SW_STATE_LL_GET_PREV((*created_node));
    prev_prev_tmp = SW_STATE_LL_GET_PREV(prev_tmp);
    if ((prev_tmp != SW_STATE_LL_INVALID) && prev_prev_tmp != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(*created_node), KEY_AT_IDX(prev_tmp), ll->key_size) <= 0){
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), *created_node));
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation. key must be bigger than prev->key \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    down_tmp = SW_STATE_LL_GET_SERVICE_DOWN((*created_node));
    if (down_tmp != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(*created_node), KEY_AT_IDX(down_tmp), ll->key_size) != 0){
                    SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, module_id, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), *created_node));
                    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation. key must be equal to down->key \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}
#endif 



STATIC int sw_state_ll_create_new_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *created_node, 
                                const void *key, const void *value, sw_state_ll_node_t prev, sw_state_ll_node_t next, sw_state_ll_node_t down,
                                sw_state_ll_node_t key_pointer, uint8 is_service_node) {

    sw_state_ll_node_t next_tmp;
    sw_state_ll_node_t prev_tmp;

    SHR_FUNC_INIT_VARS(unit);

    assert(ll != NULL);
    assert(created_node != NULL);

    
    *created_node = SW_STATE_LL_GET_HEAD(0, is_service_node);
    if (*created_node == SW_STATE_LL_INVALID) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state ll ERROR: ll is full.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    next_tmp = SW_STATE_LL_GET_NEXT(*created_node);

    if(is_service_node && next_tmp == SW_STATE_LL_INVALID){
        next_tmp = next_tmp + ll->max_elements;
    }
    
    SW_STATE_LL_SET_HEAD(0, next_tmp);

    
    if ((*created_node) != SW_STATE_LL_INVALID) {
        SW_STATE_LL_SET_PREV((*created_node), sw_state_ll_invalid);
    }
    SW_STATE_LL_SET_PREV((*created_node), sw_state_ll_invalid);
    SW_STATE_LL_SET_NEXT((*created_node), sw_state_ll_invalid);
    if (is_service_node) {
        SW_STATE_LL_SET_SERVICE_DOWN((*created_node), sw_state_ll_invalid);
    }

    
    if(!is_service_node) {
        if(value == &sw_state_ll_head_defult_val) {
            DNX_SW_STATE_MEMSET(unit, module_id, VALUE_AT_IDX(*created_node), 0, 0, ll->value_size, 0x0, "sw state ll node's value")
        }
        else {
            DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, VALUE_AT_IDX(*created_node), value, ll->value_size, 0x0, "sw state ll node's value");
        }
        
        if (key != NULL) {
            if (key == &sw_state_ll_head_defult_val) {
                DNX_SW_STATE_MEMSET(unit, module_id, KEY_AT_IDX(*created_node), 0, 0, ll->key_size, 0x0, "sw state ll node's key")
            }
            else {
                DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, KEY_AT_IDX(*created_node), key, ll->key_size, 0x0, "sw state ll node's key");
            }
        }
    }
    
    else {
            DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(ll->service_pointers_keys[TRANSLATE_LL_SERVICE_IDX(*created_node)]), &key_pointer, sizeof(sw_state_ll_node_t), 0x0, "key_pointer")
    }

    SW_STATE_LL_SET_PREV((*created_node), prev);
    SW_STATE_LL_SET_NEXT((*created_node), next);
    if (is_service_node) {
        SW_STATE_LL_SET_SERVICE_DOWN((*created_node), down);
    }
    next_tmp = SW_STATE_LL_GET_NEXT((*created_node));
    if ( next_tmp != SW_STATE_LL_INVALID) {
        SW_STATE_LL_SET_PREV(next_tmp, *created_node);
    } else {
        if(!ll->is_sorted || (ll->is_sorted && !is_service_node))
        SW_STATE_LL_SET_TAIL(ll_index, *created_node);
    }
    
    prev_tmp = SW_STATE_LL_GET_PREV((*created_node));
    if (prev_tmp != SW_STATE_LL_INVALID) {
        SW_STATE_LL_SET_NEXT(prev_tmp, *created_node);
    } else {
        if (!ll->is_sorted || (ll->is_sorted && !is_service_node)){
            SW_STATE_LL_SET_HEAD(ll_index, *created_node);
        }
    }

    if (ll->is_sorted) {
        DNXC_SW_STATE_VERIFICATIONS_ONLY(sw_state_ll_verify_order(unit, module_id, ll, ll_index, created_node));
    }

exit:
    SHR_FUNC_EXIT;
}


static void sw_state_ll_find_nodes_before_node(int unit, uint32 module_id, sw_state_ll_t ll, const void *key, sw_state_ll_node_t *update){
    sw_state_ll_node_t curr_node;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    uint8 curr_level = ll->max_level-1;
    sw_state_ll_node_t prev;
    sw_state_ll_node_t next;

    SW_STATE_CB_DB_GET_CB(
        module_id,
        ll->key_cmp_cb,
        &sorted_list_cmp_cb,
        dnx_sw_state_sorted_list_cb_get_cb
    );
    curr_node = SW_STATE_LL_GET_HEAD(1, 1);
    do
    {
        prev = SW_STATE_LL_GET_PREV(curr_node);
        next = SW_STATE_LL_GET_NEXT(curr_node);

        while((prev != SW_STATE_LL_INVALID || next != SW_STATE_LL_INVALID)  &&
                (next != SW_STATE_LL_INVALID && sorted_list_cmp_cb(KEY_AT_IDX(next), (uint8*)key, ll->key_size) < 0) ) {
            curr_node = next;
            prev = SW_STATE_LL_GET_PREV(curr_node);
            next = SW_STATE_LL_GET_NEXT(curr_node);
        }
        update[curr_level] = curr_node;
        curr_level --;
        curr_node = SW_STATE_LL_GET_SERVICE_DOWN(curr_node);

    } while(curr_node != SW_STATE_LL_INVALID);

}


static void sw_state_ll_rand_level(sw_state_ll_t ll, int* level) {
    sw_state_ll_node_t head;
    int counter=1;
    head = SW_STATE_LL_GET_HEAD(0, 1);
    head = SW_STATE_LL_GET_NEXT(head);

    while (sal_rand() < SAL_RAND_MAX / SW_STATE_SKIP_LIST_LEVEL_PROBABILITY && counter <= ll->curr_level && counter < ll->max_level &&
            head != SW_STATE_LL_INVALID){
        counter++;
        head = SW_STATE_LL_GET_NEXT(head);
    }
    *level=counter;
}



STATIC int sw_state_ll_add_internal(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node_to_add_after, uint8 is_first, const void* key, const void* value) {

    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_ll_node_t created_node;
    sw_state_ll_node_t created_temp_node = SW_STATE_LL_INVALID;
    uint8 is_suppressed = FALSE;
    sw_state_ll_node_t* nodes_to_add_after = sal_alloc(ll->max_level * sizeof(sw_state_ll_node_t), "nodes to add after");
    int level=1;
    int i;
    
    sw_state_ll_node_t next;
    sw_state_ll_node_t prev;
    sw_state_ll_node_t key_pointer;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    
    if (ll->is_sorted) {
        SW_STATE_CB_DB_GET_CB(
            module_id,
            ll->key_cmp_cb,
            &sorted_list_cmp_cb,
            dnx_sw_state_sorted_list_cb_get_cb
        );
    }

    if (ll->is_sorted) {
        sw_state_ll_find_nodes_before_node(unit, module_id, ll, key, nodes_to_add_after);

        prev = SW_STATE_LL_GET_PREV(nodes_to_add_after[0]);
        next = SW_STATE_LL_GET_NEXT(nodes_to_add_after[0]);
        if ( prev != SW_STATE_LL_INVALID &&
             sorted_list_cmp_cb(KEY_AT_IDX(next), (uint8*)key, ll->key_size) == 0 )
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: duplicate entries .\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        
        sw_state_ll_rand_level(ll, &level);
        
        if (level > ll->curr_level) {
            ll -> curr_level = level;
        }

        
        sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_temp_node, key, value, nodes_to_add_after[0], next, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE);
        next = created_temp_node;
        key_pointer = created_temp_node;

        for (i=1; i<level; i++){
            next = SW_STATE_LL_GET_NEXT(nodes_to_add_after[i]);
            sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, nodes_to_add_after[i], next, created_temp_node, key_pointer, SERVICE_NODE);
            created_temp_node = created_node;
        }
        SOC_EXIT;
    }

    
    assert(ll != NULL);
    if (ll->is_sorted) {
        assert(key != NULL);
    }
    assert(value != NULL);

    
    
    if (ll->tail[ll_index] == SW_STATE_LL_INVALID) {

        
        assert(ll->head[ll_index] == SW_STATE_LL_INVALID);
        assert(node_to_add_after == SW_STATE_LL_INVALID);

        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));

        SW_STATE_LL_SET_HEAD(ll_index, created_node);
        SW_STATE_LL_SET_TAIL(ll_index, created_node);

        SOC_EXIT;
    }

    
    if (node_to_add_after != SW_STATE_LL_INVALID) {
        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, node_to_add_after,
                                                    ll->nexts[node_to_add_after], SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));
        SOC_EXIT;
    }

    
    SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, module_id, ll, ll_index, &created_node, key, value, SW_STATE_LL_INVALID, ll->head[ll_index], SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));
    SW_STATE_LL_SET_HEAD(ll_index, created_node);

exit:
    if(is_suppressed)
    {
        sal_free(nodes_to_add_after);
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_ll_create_empty(int unit, uint32 module_id, sw_state_ll_init_info_t *init_info, uint32 flags, uint32 key_size,
    uint32 value_size, sw_state_ll_t * ll_ptr, uint32 max_nof_elements, uint32 alloc_flags) {

    uint32 elements_to_alloc;
    uint32 heads_to_alloc;
    uint32 index;
    sw_state_ll_t ll;
    sw_state_ll_node_t node;
    sw_state_ll_node_t tmp_node;
    uint8 is_sorted_ll;
    uint8 is_suppressed = FALSE;
    uint32 el_alloc_flags;
    sw_state_ll_node_t created_node;
    sw_state_ll_node_t created_temp_node;
    sw_state_ll_node_t key_pointer;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(ll_ptr != NULL);
    SW_STATE_LL_CHECK_NULL_PARAMETER(init_info);

    if (*ll_ptr != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state ll create ERROR: ll already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_ALLOC(unit, module_id, *ll_ptr, **ll_ptr,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state linked list");
    ll = *ll_ptr;

    
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

    
    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED))
    {
        ll->max_level = utilex_log2_round_down(elements_to_alloc) + 1;
        elements_to_alloc = elements_to_alloc +1 ;
    }

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->max_elements, &elements_to_alloc, sizeof(elements_to_alloc), 0x0, "ll max elements");
    ll->max_elements = elements_to_alloc;

    
    if (!sw_state_is_flag_on(flags, SW_STATE_LL_MULTIHEAD)) {
        init_info->nof_heads = 1;
    }

    
    heads_to_alloc = init_info->nof_heads + 1;

    
    DNX_SW_STATE_ALLOC(unit, module_id, ll->head, sw_state_ll_node_t, heads_to_alloc, DNXC_SW_STATE_NO_FLAGS, "ll heads");
    DNX_SW_STATE_ALLOC(unit, module_id, ll->tail, sw_state_ll_node_t, heads_to_alloc, DNXC_SW_STATE_NO_FLAGS, "ll tails");
    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED))
    {
        DNX_SW_STATE_ALLOC(unit, module_id, ll->keys, uint8, key_size * elements_to_alloc, el_alloc_flags, "ll keys");
        DNX_SW_STATE_ALLOC(unit, module_id, ll->service_head, sw_state_ll_node_t, heads_to_alloc, DNXC_SW_STATE_NO_FLAGS, "ll service heads");
        DNX_SW_STATE_ALLOC(unit, module_id, ll->service_downs, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll service downs");
        DNX_SW_STATE_ALLOC(unit, module_id, ll->service_prevs, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll service prevs");
        DNX_SW_STATE_ALLOC(unit, module_id, ll->service_nexts, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll service nexts");
        DNX_SW_STATE_ALLOC(unit, module_id, ll->service_pointers_keys, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll service pointers keys");

    }
    DNX_SW_STATE_ALLOC(unit, module_id, ll->values, uint8, value_size * elements_to_alloc, el_alloc_flags, "ll values");
    DNX_SW_STATE_ALLOC(unit, module_id, ll->prevs, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll prevs");
    DNX_SW_STATE_ALLOC(unit, module_id, ll->nexts, sw_state_ll_node_t, elements_to_alloc, el_alloc_flags, "ll nexts");


    
    node = 1;
    SW_STATE_LL_SET_HEAD(0, node);
    tmp_node = elements_to_alloc-1;
    SW_STATE_LL_SET_TAIL(0, tmp_node);
    
    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED)){
        node = node + elements_to_alloc;
        SW_STATE_LL_SET_HEAD(0, node);
    }

    
    if(SOC_IS_DONE_INIT(unit)) {
        
        for (index = 2; index < (elements_to_alloc-1); index++) {
            node = index - 1;
            SW_STATE_LL_SET_PREV(index, node);
            node = index + 1;
            SW_STATE_LL_SET_NEXT(index, node);
        }
        
        if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED)){
            for (index = elements_to_alloc + 2; index < (2*elements_to_alloc-1); index++) {
                node = index - 1;
                SW_STATE_LL_SET_PREV(index, node);
                node = index + 1;
                SW_STATE_LL_SET_NEXT(index, node);
            }
        }

    } else {
        
        for (index = 2; index < (elements_to_alloc-1); index++) {
            node = index - 1;
            ll->prevs[index] = node;

            node = index + 1;
            ll->nexts[index] = node;
        }
        
        if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED)){
            for (index = 2; index < (elements_to_alloc-1); index++) {
                node = index - 1 + elements_to_alloc;
                ll->service_prevs[index] = node;
                node = index + 1 + elements_to_alloc;
                ll->service_nexts[index] = node;
            }
        }
    }

    SW_STATE_LL_SET_PREV(1, sw_state_ll_invalid);
    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED)){
        SW_STATE_LL_SET_PREV(1+elements_to_alloc, sw_state_ll_invalid);
    }

    node = 2;
    SW_STATE_LL_SET_NEXT(1, node);
    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED)){
        node = node + elements_to_alloc;
        SW_STATE_LL_SET_NEXT(elements_to_alloc+1, node);
    }

    node = elements_to_alloc-1;
    SW_STATE_LL_SET_PREV(elements_to_alloc-1, node);
    SW_STATE_LL_SET_NEXT(elements_to_alloc-1, sw_state_ll_invalid);

    if (sw_state_is_flag_on(flags, SW_STATE_LL_SORTED)){
        node = node + elements_to_alloc;
        SW_STATE_LL_SET_PREV(2*elements_to_alloc-1, node);
        SW_STATE_LL_SET_NEXT(2*elements_to_alloc-1, sw_state_ll_invalid);
    }


    
    is_sorted_ll = sw_state_is_flag_on(flags, SW_STATE_LL_SORTED);
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->is_sorted, &is_sorted_ll, sizeof(is_sorted_ll), 0x0, "is_sorted_ll");
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->nof_heads, &init_info->nof_heads, sizeof(init_info->nof_heads), 0x0, "nof_heads");
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->key_size, &key_size, sizeof(key_size), 0x0, "key_size");
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &ll->value_size, &value_size, sizeof(value_size), 0x0, "value_size");


    if (ll->is_sorted) {
        sal_srand(700);
        ll->curr_level = 1;
        
        key_pointer = SW_STATE_LL_GET_HEAD(0, 0);
        sw_state_ll_create_new_node(unit, module_id, ll, 1, &created_temp_node, &sw_state_ll_head_defult_val, &sw_state_ll_head_defult_val, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid, key_pointer, SERVICE_NODE);
        SW_STATE_LL_SET_HEAD(1, created_temp_node);
        for (i = 0; i < ll->max_level - 2 ; i++) {
            sw_state_ll_create_new_node(unit, module_id, ll, 1, &created_node, &sw_state_ll_head_defult_val, &sw_state_ll_head_defult_val, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid, key_pointer, SERVICE_NODE);
            SW_STATE_LL_SET_SERVICE_DOWN(created_temp_node, created_node);
            created_temp_node = created_node;
        }
        sw_state_ll_create_new_node(unit, module_id, ll, 1, &created_node, &sw_state_ll_head_defult_val, &sw_state_ll_head_defult_val, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid, !SERVICE_NODE);
        SW_STATE_LL_SET_SERVICE_DOWN(created_temp_node, created_node);
        SW_STATE_LL_SET_TAIL(1, created_node);

        if (init_info->key_cmp_cb_name != NULL) {
            SW_STATE_CB_DB_REGISTER_CB(module_id, ll->key_cmp_cb, init_info->key_cmp_cb_name, dnx_sw_state_sorted_list_cb_get_cb);
        }
        else{
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state sorted ll ERROR: ll must be given a compare cb.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_ll_add(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);

    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), SW_STATE_LL_INVALID, 0, key, value);
    
    DNXC_SW_STATE_FUNC_RETURN;
}



int sw_state_ll_add_first(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add last in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), SW_STATE_LL_INVALID, 1, key, value);

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_add_last(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add last in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), ll->tail[TRANSLATE_LL_IDX(ll_index)], 0, key, value);
    
    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_add_before(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add before in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    SW_STATE_LL_CHECK_VALID_NODE(node);

    
    if (ll->prevs[node] == SW_STATE_LL_INVALID) {
        return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), SW_STATE_LL_INVALID, 0, key, value);
    }

    
    assert (ll->nexts[ll->prevs[node]] != SW_STATE_LL_INVALID);

    
    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), ll->prevs[node], 0, key, value);

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_add_after(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    assert(ll != NULL);
    if (ll->is_sorted) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add after in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    SW_STATE_LL_CHECK_VALID_NODE(node);

    return sw_state_ll_add_internal(unit, module_id, ll, TRANSLATE_LL_IDX(ll_index), node, 0, key, value);
    
    DNXC_SW_STATE_FUNC_RETURN; 
}

STATIC int sw_state_ll_return_node_to_pool(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node) {

    sw_state_ll_node_t head;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    head = SW_STATE_LL_GET_HEAD(0, IDX_IS_SERVICE_NODE(node));

    if (head == SW_STATE_LL_INVALID) {
        
        SW_STATE_LL_SET_HEAD(0, node);
        if (!IDX_IS_SERVICE_NODE(node)) {
            SW_STATE_LL_SET_TAIL(0, node);
        }
        SW_STATE_LL_SET_NEXT(node, sw_state_ll_invalid);
        SW_STATE_LL_SET_PREV(node, sw_state_ll_invalid);
    }
    else {
        SW_STATE_LL_SET_NEXT(node, head);
        SW_STATE_LL_SET_PREV(head, node);
        SW_STATE_LL_SET_HEAD(0, node);
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}

static int sw_state_ll_sorted_ll_remove_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node) {

    sw_state_ll_node_t* prevs = sal_alloc(ll->max_level * sizeof(sw_state_ll_node_t), "previous nodes");
    sw_state_ll_node_t* nexts = sal_alloc(ll->max_level * sizeof(sw_state_ll_node_t), "nexts nodes");
    sw_state_ll_node_t* nodes_to_remove = sal_alloc(ll->max_level * sizeof(sw_state_ll_node_t), "nodes to remove");
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_ll_node_t next;

    int nodes_to_remove_num = 0;
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_CB_DB_GET_CB(
            module_id,
            ll->key_cmp_cb,
            &sorted_list_cmp_cb,
            dnx_sw_state_sorted_list_cb_get_cb
    );

    sw_state_ll_find_nodes_before_node(unit, module_id, ll, KEY_AT_IDX(node), prevs);

    
    next = SW_STATE_LL_GET_NEXT(prevs[0]);
    if(sorted_list_cmp_cb(KEY_AT_IDX(next), KEY_AT_IDX(node), ll->key_size) != 0){
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: Node %d is not exist, cannot remove.\n%s%s", node, EMPTY, EMPTY);
    }

    for(i=0; i<ll->max_level; i++){
        next = SW_STATE_LL_GET_NEXT(prevs[i]);
        if(sorted_list_cmp_cb(KEY_AT_IDX(next), KEY_AT_IDX(node), ll->key_size) == 0){
            nodes_to_remove[i] = SW_STATE_LL_GET_NEXT(prevs[i]);
            nexts[i] = SW_STATE_LL_GET_NEXT(nodes_to_remove[i]);
            nodes_to_remove_num++;
        }
        else
            break;
    }

    
    if(nexts[0] == SW_STATE_LL_INVALID){
        SW_STATE_LL_SET_TAIL(1, prevs[0]);
    }

    for(i=0; i<nodes_to_remove_num; i++){
        
        if (i==0) {
            DNX_SW_STATE_MEMSET(unit, module_id, KEY_AT_IDX(nodes_to_remove[i]), 0, -1, ll->key_size, 0x0, "sw state ll node's key");
            DNX_SW_STATE_MEMSET(unit, module_id, VALUE_AT_IDX(nodes_to_remove[i]), 0, -1, ll->value_size, 0x0, "sw state ll node's value");
        }

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, nodes_to_remove[i]));
        
        SW_STATE_LL_SET_NEXT(prevs[i], nexts[i]);
        SW_STATE_LL_SET_PREV(nexts[i], prevs[i]);
    }

exit:
    sal_free(nodes_to_remove);
    sal_free(prevs);
    sal_free(nexts);

    SHR_FUNC_EXIT;
}



int sw_state_ll_remove_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node) {

    
    sw_state_ll_node_t prev = SW_STATE_LL_INVALID;
    sw_state_ll_node_t next = SW_STATE_LL_INVALID;
    uint32 translated_ll_index = TRANSLATE_LL_IDX(ll_index);
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_VALID_NODE(node);
    
    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if (ll->is_sorted) {
        SHR_IF_ERR_EXIT(sw_state_ll_sorted_ll_remove_node(unit, module_id, ll, ll_index, node));
        SOC_EXIT;
    }

    
    else{
        
        DNX_SW_STATE_MEMSET(unit, module_id, VALUE_AT_IDX(node), 0, -1, ll->value_size, 0x0, "sw state ll node's value");
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    
    if ((ll->nexts[node] != SW_STATE_LL_INVALID) && (ll->prevs[node] != SW_STATE_LL_INVALID)) {

        prev = ll->prevs[node];
        next = ll->nexts[node];

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_NEXT(prev, next);
        SW_STATE_LL_SET_PREV(next, prev);

        SOC_EXIT;
    }

    
    if ((ll->nexts[node] == SW_STATE_LL_INVALID) && (ll->prevs[node] == SW_STATE_LL_INVALID)) {

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_HEAD(translated_ll_index, sw_state_ll_invalid);
        SW_STATE_LL_SET_TAIL(translated_ll_index, sw_state_ll_invalid);

        SOC_EXIT;
    }

    
    if (ll->nexts[node] == SW_STATE_LL_INVALID) {

        prev = ll->prevs[node];

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_NEXT(prev, sw_state_ll_invalid);
        SW_STATE_LL_SET_TAIL(translated_ll_index, prev);

        SOC_EXIT;
    }

    
    if (ll->prevs[node] == SW_STATE_LL_INVALID) {

        next = ll->nexts[node];

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, module_id, ll, node));

        SW_STATE_LL_SET_PREV(next, sw_state_ll_invalid);
        SW_STATE_LL_SET_HEAD(translated_ll_index, next);

        SOC_EXIT;
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_ll_node_update(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, const void *value) {

    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);
    
    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, VALUE_AT_IDX(node), value, ll->value_size, 0x0, "sw state ll node's value");

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_ll_next_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_VALID_NODE(input_node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->nexts[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_previous_node(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_VALID_NODE(input_node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);
    
    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->prevs[input_node];

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_get_first(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node) {
    sw_state_ll_node_t curr_node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    if(ll->is_sorted ){
        curr_node = SW_STATE_LL_GET_HEAD(TRANSLATE_LL_IDX(ll_index), 1);
        if(TRANSLATE_LL_IDX(ll_index) == 0) {
            *output_node = curr_node;
        }
        else{
            while(SW_STATE_LL_GET_SERVICE_DOWN(curr_node) != SW_STATE_LL_INVALID) {
                curr_node = SW_STATE_LL_GET_SERVICE_DOWN(curr_node);
            }
            *output_node = SW_STATE_LL_GET_NEXT(curr_node);
        }
    }
    else {
        *output_node = SW_STATE_LL_GET_HEAD(TRANSLATE_LL_IDX(ll_index), 0);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_get_last(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_NULL_PARAMETER(output_node);

    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    *output_node = ll->tail[TRANSLATE_LL_IDX(ll_index)];

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_nof_elements(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, uint32 *nof_elements) {

    sw_state_ll_node_t node;
    int count = 0;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to access inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);
  
    SW_STATE_LL_CHECK_NULL_PARAMETER(nof_elements);
    
    node = ll->head[TRANSLATE_LL_IDX(ll_index)];

    if(ll->is_sorted && (TRANSLATE_LL_IDX(ll_index) != 0)){
        SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, module_id, ll, ll_index, &node));
    }
    while (node != SW_STATE_LL_INVALID) {
        count++;
        node = ll->nexts[node];
    }

    *nof_elements = count;

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_node_key(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *key) {
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    
    if (ll == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to access inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(key);

    sal_memcpy(key, KEY_AT_IDX(node), ll->key_size);

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_ll_node_value(int unit, uint32 module_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *value) {

    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    assert(ll != NULL);

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(value);

    sal_memcpy(value, VALUE_AT_IDX(node), ll->value_size);

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }

    SHR_FUNC_EXIT;
}


int sw_state_ll_destroy(int unit, uint32 module_id, sw_state_ll_t *ll_ptr) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert((*ll_ptr) != NULL);

    

    DNXC_SW_STATE_FUNC_RETURN;
}

STATIC DNXC_SW_STATE_INLINE void
sw_state_ll_data_hexdump(int unit, uint8 *data, uint32 size) {
    int i;
    assert(data != NULL);
    
    for (i = 0; i < size; i++)
    {
        DNX_SW_STATE_PRINT(unit, "%02X", data[i]);
    }
}

#ifdef SORTED_LL_EXTENDED_PRINT

STATIC int sw_state_ll_print_skipped_ll(int unit, sw_state_ll_t ll) {

    sw_state_ll_node_t node;
    sw_state_ll_node_t head_node;

    uint32 ll_level = ll->max_level;
    uint32 nof_elements;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    
    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, -1, ll, REVERSE_TRANSLATE_LL_IDX(0), &nof_elements));
    DNX_SW_STATE_PRINT(unit, "Number of unused nodes: %d\n", nof_elements);

    
    head_node = SW_STATE_LL_GET_HEAD(1, 1);
    while (head_node!=SW_STATE_LL_INVALID) {
        DNX_SW_STATE_PRINT(unit, "Printing Linked List Level #%d:\n", ll_level);
        node = SW_STATE_LL_GET_NEXT(head_node);
        
        while (node != SW_STATE_LL_INVALID) {
            DNX_SW_STATE_PRINT(unit, " %d -> ", node);
            DNX_SW_STATE_PRINT(unit, "key: ");
            sw_state_ll_data_hexdump(unit, KEY_AT_IDX(node), ll->key_size);
            DNX_SW_STATE_PRINT(unit, "\n");
            node = SW_STATE_LL_GET_NEXT(node);
        }
        head_node = SW_STATE_LL_GET_SERVICE_DOWN(head_node);
        ll_level--;
    }
    DNXC_SW_STATE_FUNC_RETURN;
}


STATIC int sw_state_ll_print_free_service_nodes_skipped_ll(int unit, sw_state_ll_t ll) {

    sw_state_ll_node_t node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    
    node = SW_STATE_LL_GET_HEAD(0, 1);

    DNX_SW_STATE_PRINT(unit, "Printing service nodes :\n");
    while (node != SW_STATE_LL_INVALID) {
        DNX_SW_STATE_PRINT(unit, " %d -> ", node);
        node = SW_STATE_LL_GET_NEXT(node);
    }
    DNXC_SW_STATE_FUNC_RETURN;
}

#endif



int sw_state_ll_print(int unit, sw_state_ll_t ll) {

    sw_state_ll_node_t node;
    int count = 0;
    uint32 ll_index;
    uint32 nof_elements;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);

#ifdef SORTED_LL_EXTENDED_PRINT
    if (ll->is_sorted)
        sw_state_ll_print_skipped_ll(unit, ll);
        sw_state_ll_print_free_service_nodes_skipped_ll(unit, ll);
#endif

    
    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, -1, ll, REVERSE_TRANSLATE_LL_IDX(0), &nof_elements));
    DNX_SW_STATE_PRINT(unit, "Number of unused nodes: %d\n", nof_elements);

    
    for (ll_index = 1; ll_index < ll->nof_heads+1; ll_index++) {
        node = SW_STATE_LL_GET_HEAD(ll_index, 0);

        if (ll->nof_heads == 1) {
            DNX_SW_STATE_PRINT(unit, "Printing Linked List:\n");
        }
        else {
            DNX_SW_STATE_PRINT(unit, "Printing Linked List #%d:\n", ll_index);
        }

        
        if (ll->is_sorted) {
            SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, -1, ll, REVERSE_TRANSLATE_LL_IDX(ll_index), &node));
        }

        
        while (node != SW_STATE_LL_INVALID) {
            count++;
            DNX_SW_STATE_PRINT(unit, " %d -> ", count);
            if (ll->is_sorted) {
                DNX_SW_STATE_PRINT(unit, "key: ");
                sw_state_ll_data_hexdump(unit, KEY_AT_IDX(node), ll->key_size);
                DNX_SW_STATE_PRINT(unit, ", value: ");
            }
            sw_state_ll_data_hexdump(unit, VALUE_AT_IDX(node), ll->value_size);
            DNX_SW_STATE_PRINT(unit, "\n");
            node = SW_STATE_LL_GET_NEXT(node);
        }
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int sw_state_ll_find(int unit, uint32 module_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t * node, const void *key, uint8* found) {

    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_ll_node_t* prevs = sal_alloc(ll->max_level * sizeof(sw_state_ll_node_t), "previous nodes");
    
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_NULL_PARAMETER(key);
    SW_STATE_LL_CHECK_NULL_PARAMETER(found);

    SW_STATE_CB_DB_GET_CB(
        module_id,
        ll->key_cmp_cb,
        &sorted_list_cmp_cb,
        dnx_sw_state_sorted_list_cb_get_cb
    );

    sw_state_ll_find_nodes_before_node(unit, module_id, ll, key, prevs);

    if (sorted_list_cmp_cb(KEY_AT_IDX(ll->nexts[prevs[0]]), (uint8*)key, ll->key_size)==0)
    {
        *node = SW_STATE_LL_GET_NEXT(prevs[0]);
        *found = 1;
    }
    else
    {
        *node = prevs[0];
        *found=0;
    }

exit:
    sal_free(prevs);
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME


