
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL



#include <assert.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/dnx_sw_state_journal.h>
#endif

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX




#define DNX_SW_STATE_LL_MAX_NOF_NODES 0xFFFFFFFF

#define SW_STATE_SKIP_LIST_LEVEL_PROBABILITY 2
#define SERVICE_NODE                         1
#define FREE_NODES_LL_IDX                    (ll->nof_heads)
#define LL_NO_FLAGS                          0

#define SW_STATE_LL_ASSERT(x)

#define SW_STATE_LL_CHECK_VALID_NODE(param) \
    do { \
        if ((param) == SW_STATE_LL_INVALID) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: SW_STATE_LL_INVALID argument - " #param ".\n%s%s%s", EMPTY, EMPTY, EMPTY); \
        } \
    } while(FALSE)


#define SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index) \
    do { \
        if (ll_index >= ll->nof_heads) { \
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state ll ERROR: accessed ll head that does not exist (in multihead list). index=%d nof_heads=%d\n%s", ll_index, ll->nof_heads, EMPTY); \
        } \
    } while(FALSE)


const sw_state_ll_node_t sw_state_ll_invalid = SW_STATE_LL_INVALID;
const uint8 sw_state_ll_head_default_val = 0;



static inline uint32
translate_ll_service_idx(sw_state_ll_t ll, sw_state_ll_node_t idx)
{
    return (idx - ll->max_elements);
}

static inline uint32
reverse_translate_ll_service_idx(sw_state_ll_t ll, sw_state_ll_node_t idx)
{
    return (idx + ll->max_elements);
}

static inline uint32
is_service_node(sw_state_ll_t ll, sw_state_ll_node_t idx)
{
    if (idx < ll->max_elements)
        return FALSE;
    return TRUE;
}

static inline shr_error_e
key_at_idx(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t idx, uint8 **key)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    if (!is_service_node(ll, idx))
    {
        *key = ((uint8 *) (ll->keys + (ll->key_size * idx)));
        SHR_EXIT();
    }

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->service_pointers_keys, ll->ptr_size * translate_ll_service_idx(ll, idx), 0, ll->ptr_size, &node_ptr);

    *key = ((uint8 *) (ll->keys + (ll->key_size * node_ptr)));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline uint8*
value_at_idx(sw_state_ll_t ll, sw_state_ll_node_t idx)
{
    return ((uint8 *) ll->values + (ll->value_size * idx));
}

static inline shr_error_e
sw_state_ll_set_head(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t ll_index, sw_state_ll_node_t value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_service_node(ll, value))
    {
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->service_head, ll->ptr_size * ll_index, 0, ll->ptr_size, &value);
    }
    else
    {
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->head, ll->ptr_size * ll_index, 0, ll->ptr_size, &value);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_get_head(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, uint8 is_service_node, sw_state_ll_node_t *head_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    if (is_service_node)
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->service_head, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);
    else
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->head, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);

    *head_node = node_ptr;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_set_tail(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t ll_index, sw_state_ll_node_t value)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->tail, ll->ptr_size * ll_index, 0, ll->ptr_size, &value);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_set_next(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node_to_set, sw_state_ll_node_t next)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_service_node(ll, node_to_set))
    {
        
        SW_STATE_LL_ASSERT(is_service_node(ll, node_to_set) || node_to_set == SW_STATE_LL_INVALID);
        SW_STATE_LL_ASSERT(translate_ll_service_idx(ll, node_to_set) < ll->nof_service_nodes);
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->service_nexts, ll->ptr_size * translate_ll_service_idx(ll, node_to_set), 0, ll->ptr_size, &next);
    }
    else
    {
        
        SW_STATE_LL_ASSERT(!is_service_node(ll, node_to_set) || node_to_set == SW_STATE_LL_INVALID);
        SW_STATE_LL_ASSERT(node_to_set < ll->nof_regular_nodes);
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->nexts, ll->ptr_size * node_to_set, 0, ll->ptr_size, &next);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_get_next(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    if (is_service_node(ll, node))
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->service_nexts, ll->ptr_size * translate_ll_service_idx(ll, node), 0, ll->ptr_size, &node_ptr);
    else
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * node, 0, ll->ptr_size, &node_ptr);

    *next_node = node_ptr;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_set_prev(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node_to_set, sw_state_ll_node_t prev)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_service_node(ll, node_to_set))
    {
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->prevs, ll->ptr_size * node_to_set, 0, ll->ptr_size, &prev);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
isEqualKey(int unit, uint32 node_id, sw_state_ll_t ll, dnx_sw_state_sorted_list_cb sorted_list_cmp_cb, sw_state_ll_node_t node, const void* key, uint8 *cmp_result)
{
    uint8 *idx_key;

    SHR_FUNC_INIT_VARS(unit);

    if (node == SW_STATE_LL_INVALID)
    {
        *cmp_result = FALSE;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, node, &idx_key));
    *cmp_result = sorted_list_cmp_cb(idx_key, (uint8*)key, ll->key_size) == 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline int
isEqualValue(sw_state_ll_t ll, dnx_sw_state_sorted_list_cb sorted_list_cmp_cb, sw_state_ll_node_t node, const void* value)
{
    return (node != SW_STATE_LL_INVALID && value != NULL &&
            sal_memcmp(value_at_idx(ll, node), (uint8*)value, ll->value_size) == 0);
}

static inline shr_error_e
sw_state_ll_get_prev(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    if (!is_service_node(ll, node))
    {
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->prevs, ll->ptr_size * node, 0, ll->ptr_size, &node_ptr);
    }

    *prev_node = node_ptr;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_set_service_down(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node_to_set, sw_state_ll_node_t down)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->service_downs, ll->ptr_size * translate_ll_service_idx(ll, node_to_set), 0, ll->ptr_size, &down);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static inline shr_error_e
sw_state_ll_get_service_down(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node, sw_state_ll_node_t *service_down_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    if(!is_service_node(ll, node))
    {
        *service_down_node = SW_STATE_LL_INVALID;
        SHR_EXIT();
    }

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->service_downs, ll->ptr_size * translate_ll_service_idx(ll, node), 0, ll->ptr_size, &node_ptr);

    *service_down_node = node_ptr;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_SW_STATE_VERIFICATIONS
static shr_error_e
sw_state_ll_verify_order(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *created_node)
{
    uint8 is_suppressed = FALSE;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;

    sw_state_ll_node_t next_tmp;
    sw_state_ll_node_t prev_tmp;
    sw_state_ll_node_t prev_prev_tmp;
    sw_state_ll_node_t down_tmp;

    uint8 *idx_key_created, *idx_key_tmp;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    SW_STATE_CB_DB_GET_CB(
               node_id,
               ll->key_cmp_cb,
               &sorted_list_cmp_cb,
               dnx_sw_state_sorted_list_cb_get_cb
           );

     
    SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, (*created_node), &next_tmp));
    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, *created_node, &idx_key_created));
    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, next_tmp, &idx_key_tmp));
    if (next_tmp != SW_STATE_LL_INVALID && sorted_list_cmp_cb(idx_key_created, idx_key_tmp, ll->key_size) > 0)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, ll, ll_index, *created_node));
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation. key must be smaller than next->key \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(sw_state_ll_get_prev(unit, node_id, ll, (*created_node), &prev_tmp));
    SHR_IF_ERR_EXIT(sw_state_ll_get_prev(unit, node_id, ll, prev_tmp, &prev_prev_tmp));
    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, prev_tmp, &idx_key_tmp));
    if ((prev_tmp != SW_STATE_LL_INVALID) && prev_prev_tmp != SW_STATE_LL_INVALID && sorted_list_cmp_cb(idx_key_created, idx_key_tmp, ll->key_size) < 0)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, ll, ll_index, *created_node));
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation. key must be bigger than prev->key \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(sw_state_ll_get_service_down(unit, node_id, ll, (*created_node), &down_tmp));
    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, down_tmp, &idx_key_tmp));
    if (down_tmp != SW_STATE_LL_INVALID && sorted_list_cmp_cb(idx_key_created, idx_key_tmp, ll->key_size) != 0)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, ll, ll_index, *created_node));
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ll ERROR: sorted list order violation. key must be equal to down->key \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}
#endif 


static shr_error_e
sw_state_ll_create_new_node(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *created_node, const void *key, const void *value, sw_state_ll_node_t prev, sw_state_ll_node_t next, sw_state_ll_node_t down, sw_state_ll_node_t key_pointer, uint8 is_service_node)
{
    sw_state_ll_node_t next_tmp;
    uint8 *idx_key;

    SHR_FUNC_INIT_VARS(unit);

    assert(ll != NULL);
    assert(created_node != NULL);

    
    SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, FREE_NODES_LL_IDX, is_service_node, created_node));
    if (*created_node == SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state ll ERROR: ll is full.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, *created_node, &next_tmp));

    if (is_service_node && next_tmp == SW_STATE_LL_INVALID)
    {
        next_tmp = ll->max_elements;
    }
    
    SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, FREE_NODES_LL_IDX, next_tmp));

    
    if (!is_service_node)
    {
        if (ll->value_size > 0)
        {
            if (value == &sw_state_ll_head_default_val)
            {
                DNX_SW_STATE_MEMSET(unit, node_id, value_at_idx(ll, *created_node), 0, 0, ll->value_size, LL_NO_FLAGS, "sw state ll node's value")
            }
            else
            {
                DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, value_at_idx(ll, *created_node), value, ll->value_size, LL_NO_FLAGS, "sw state ll node's value");
            }
        }
        
        if (key != NULL)
        {
            SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, *created_node, &idx_key));
            if (key == &sw_state_ll_head_default_val)
            {
                DNX_SW_STATE_MEMSET(unit, node_id, idx_key, 0, 0, ll->key_size, LL_NO_FLAGS, "sw state ll node's key");
            }
            else
            {
                DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, idx_key, key, ll->key_size, LL_NO_FLAGS, "sw state ll node's key");
            }
        }
    }
    
    else
    {
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->service_pointers_keys, ll->ptr_size * translate_ll_service_idx(ll, *created_node), 0, ll->ptr_size, &key_pointer);
        SHR_IF_ERR_EXIT(sw_state_ll_set_service_down(unit, node_id, ll, (*created_node), down));
    }

    SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, (*created_node), prev));
    SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, (*created_node), next));

    if (next != SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, next, (*created_node)));
    }
    else
    {
        if (!ll->is_sorted || (ll->is_sorted && !is_service_node))
        {
            SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, ll_index, (*created_node)));
        }
    }

    if (prev != SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, prev, (*created_node)));
    }
    else
    {
        if (!ll->is_sorted || (ll->is_sorted && !is_service_node))
        {
            SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, ll_index, (*created_node)));
        }
    }

    if (ll->is_sorted)
    {
        DNXC_SW_STATE_VERIFICATIONS_ONLY(sw_state_ll_verify_order(unit, node_id, ll, ll_index, created_node));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_ll_find_nodes_around_node(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value, uint8 find_before, sw_state_ll_node_t *update)
{
    sw_state_ll_node_t curr_node, next;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    uint8 curr_level = ll->max_level-1;

    uint8 *idx_key_next;
    int cmp_res = 0;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_CB_DB_GET_CB(
        node_id,
        ll->key_cmp_cb,
        &sorted_list_cmp_cb,
        dnx_sw_state_sorted_list_cb_get_cb
    );

    SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, ll_index, TRUE, &curr_node));

    do
    {
        
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, curr_node, &next));
        if (next != SW_STATE_LL_INVALID)
        {
            SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, next, &idx_key_next));
            cmp_res = sorted_list_cmp_cb(idx_key_next, (uint8*)key, ll->key_size);
        }

        
        while((next != SW_STATE_LL_INVALID) && ((cmp_res < 0) || ((cmp_res == 0) && !find_before)))
        {
            curr_node = next;
            SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, curr_node, &next));
            if (next != SW_STATE_LL_INVALID)
            {
                SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, next, &idx_key_next));
                cmp_res = sorted_list_cmp_cb(idx_key_next, (uint8*)key, ll->key_size);
            }
        }
        
        if(!find_before)
        {
            
            update[curr_level] = next;
        }
        
        else
        {
            update[curr_level] = curr_node;
        }
        
        curr_level--;
        SHR_IF_ERR_EXIT(sw_state_ll_get_service_down(unit, node_id, ll, curr_node, &curr_node));
    } while(curr_node != SW_STATE_LL_INVALID);

    
    if (value != NULL)
    {
        if (next != SW_STATE_LL_INVALID)
        {
            SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, next, &idx_key_next));
        }
        while (next != SW_STATE_LL_INVALID && sorted_list_cmp_cb(idx_key_next, (uint8*)key, ll->key_size) <= 0 && !isEqualValue(ll, sorted_list_cmp_cb, next, value))
        {
            curr_node = next;
            SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, curr_node, &next));
            if (next != SW_STATE_LL_INVALID)
            {
                SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, next, &idx_key_next));
            }
        }
        if (curr_node != SW_STATE_LL_INVALID)
        {
            update[0] = curr_node;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_ll_rand_level(int unit, uint32 node_id, sw_state_ll_t ll, int* level)
{
    sw_state_ll_node_t head;
    int counter=1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, FREE_NODES_LL_IDX, TRUE, &head));
    SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, head, &head));

    while (sal_rand() < SAL_RAND_MAX / SW_STATE_SKIP_LIST_LEVEL_PROBABILITY && counter <= ll->curr_level && counter < ll->max_level &&
            head != SW_STATE_LL_INVALID)
    {
        counter++;
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, head, &head));
    }
    *level=counter;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_ll_add_internal(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node_to_add_after, const void* key, const void* value)
{
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_ll_node_t created_node, created_temp_node = SW_STATE_LL_INVALID, key_pointer, next, prev = SW_STATE_LL_INVALID;
    uint8 is_suppressed = FALSE, is_equal_key = FALSE;
    int idx, level=1;

    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    
    if (ll->is_sorted)
    {
        SW_STATE_CB_DB_GET_CB(
            node_id,
            ll->key_cmp_cb,
            &sorted_list_cmp_cb,
            dnx_sw_state_sorted_list_cb_get_cb
        );
    }

    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_find_nodes_around_node(unit, node_id, ll, ll_index, key, value, TRUE, ll->sandbox));

        SHR_IF_ERR_EXIT(sw_state_ll_get_prev(unit, node_id, ll, ll->sandbox[0], &prev));
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, ll->sandbox[0], &next));

        
        SHR_IF_ERR_EXIT(sw_state_ll_rand_level(unit, node_id, ll, &level));
        
        if (level > ll->curr_level)
        {
            ll->curr_level = level;
        }

        SHR_IF_ERR_EXIT(isEqualKey(unit, node_id, ll, sorted_list_cmp_cb, next, key, &is_equal_key));
        if (!is_equal_key)
        {
            SHR_IF_ERR_EXIT(isEqualKey(unit, node_id, ll, sorted_list_cmp_cb, prev, key, &is_equal_key));
        }
        if(is_equal_key)
        {
            if(value == NULL)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EXISTS);
            }
            if(isEqualValue(ll, sorted_list_cmp_cb, next, value) || isEqualValue(ll, sorted_list_cmp_cb, prev, value))
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EXISTS);
            }
            
            SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_temp_node, key, value, ll->sandbox[0], next, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));
        }
        else
        {
            
            SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_temp_node, key, value, ll->sandbox[0], next, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));
            
            next = created_temp_node;
            key_pointer = created_temp_node;

            for (idx=1; idx<level; idx++)
            {
                SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, ll->sandbox[idx], &next));
                SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_node, key, value, ll->sandbox[idx], next, created_temp_node, key_pointer, SERVICE_NODE));
                created_temp_node = created_node;
            }
        }
        SOC_EXIT;
    }

    
    assert(ll != NULL);
    if (ll->is_sorted)
    {
        assert(key != NULL);
        if(ll->value_size > 0)
        {
            assert(value != NULL);
        }
    }
    else
    {
        assert(value != NULL);
    }

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->tail, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);
    
    
    if (node_ptr == SW_STATE_LL_INVALID)
    {

        
        node_ptr = 0;
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->head, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);
        assert(node_ptr == SW_STATE_LL_INVALID);
        assert(node_to_add_after == SW_STATE_LL_INVALID);

        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_node, key, value, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));

        SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, ll_index, created_node));
        SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, ll_index, created_node));

        SOC_EXIT;
    }

    
    if (node_to_add_after != SW_STATE_LL_INVALID)
    {
        node_ptr = 0;
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * node_to_add_after, 0, ll->ptr_size, &node_ptr);
        SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_node, key, value, node_to_add_after,
                                                    node_ptr, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));
        SOC_EXIT;
    }

    
    node_ptr = 0;
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->head, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);
    SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_node, key, value, SW_STATE_LL_INVALID, node_ptr, SW_STATE_LL_INVALID, SW_STATE_LL_INVALID, !SERVICE_NODE));
    SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, ll_index, created_node));

exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_create_empty(int unit, uint32 node_id, sw_state_ll_init_info_t *init_info, uint32 flags, uint32 key_size, uint32 value_size, sw_state_ll_t * ll_ptr, uint32 max_nof_elements, uint32 alloc_flags)
{
    uint32 elements_to_alloc, heads_to_alloc, service_elements_to_alloc=0, index;
    sw_state_ll_t ll;
    sw_state_ll_node_t created_node, created_temp_node, key_pointer;
    uint8 is_sorted_ll, is_suppressed = FALSE;
    uint32 el_alloc_flags, nof_elements_requested;
    
    uint32 original_nof_heads = init_info->nof_heads || 1;
    int idx;
    sw_state_ll_node_t ll_index;

    SHR_BITDCL val_to_write = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    SHR_NULL_CHECK(ll_ptr, SOC_E_PARAM, "ll_ptr");
    SHR_NULL_CHECK(init_info, SOC_E_PARAM, "init_info");
    if (*ll_ptr != NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state ll create ERROR: ll already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_ALLOC(unit, node_id, *ll_ptr, **ll_ptr,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state linked list");
    ll = *ll_ptr;

    
    if (max_nof_elements != 0)
    {
        nof_elements_requested = max_nof_elements;
        el_alloc_flags = alloc_flags;
    }
    else
    {
        nof_elements_requested = init_info->max_nof_elements;
        el_alloc_flags = DNXC_SW_STATE_NO_FLAGS;
    }
    
    elements_to_alloc = nof_elements_requested + 1;

    
    if (!sw_state_is_flag_on(flags, SW_STATE_LL_MULTIHEAD))
    {
        init_info->nof_heads = 1;
    }

    is_sorted_ll = sw_state_is_flag_on(flags, SW_STATE_LL_SORTED);
    
    if (is_sorted_ll)
    {
        ll->max_level = utilex_log2_round_down(elements_to_alloc) + 1;
        service_elements_to_alloc =  elements_to_alloc + init_info->nof_heads * ll->max_level;
        elements_to_alloc = elements_to_alloc + init_info->nof_heads ;
    }

    if ((elements_to_alloc + service_elements_to_alloc) > DNX_SW_STATE_LL_MAX_NOF_NODES)
    {
        uint32 max_nof_allowed_nodes;
        if (is_sorted_ll)
        {
            
            max_nof_allowed_nodes = DNX_SW_STATE_LL_MAX_NOF_NODES - service_elements_to_alloc - 1;
            SHR_ERR_EXIT(SOC_E_MEMORY, "sw state ll create ERROR: number of elements in sorted LL with %d heads cannot exceed %d, but %d elements and %d heads were requested.\n", original_nof_heads, max_nof_allowed_nodes, nof_elements_requested, original_nof_heads);
        }
        else
        {
            
            max_nof_allowed_nodes = DNX_SW_STATE_LL_MAX_NOF_NODES - 1;
            SHR_ERR_EXIT(SOC_E_MEMORY, "sw state ll create ERROR: number of elements in unsorted LL cannot exceed %d, but %d elements and %d heads were requested.\n", max_nof_allowed_nodes, nof_elements_requested, original_nof_heads);
        }
    }

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &ll->is_sorted, &is_sorted_ll, sizeof(is_sorted_ll), LL_NO_FLAGS, "is_sorted_ll");
    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &ll->nof_heads, &init_info->nof_heads, sizeof(init_info->nof_heads), LL_NO_FLAGS, "nof_heads");
    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &ll->key_size, &key_size, sizeof(key_size), LL_NO_FLAGS, "key_size");
    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &ll->value_size, &value_size, sizeof(value_size), LL_NO_FLAGS, "value_size");
    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &ll->max_elements, &elements_to_alloc, sizeof(elements_to_alloc), LL_NO_FLAGS, "ll max elements");

    
    heads_to_alloc = init_info->nof_heads + 1;

    
    ll->nof_regular_nodes = elements_to_alloc;
    ll->nof_service_nodes = service_elements_to_alloc;

    ll->ptr_size = utilex_log2_round_up(elements_to_alloc + service_elements_to_alloc + 1);

    
    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->head, heads_to_alloc * ll->ptr_size, NULL, 0, DNXC_SW_STATE_NO_FLAGS, "ll heads");
    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->tail, heads_to_alloc * ll->ptr_size, NULL, 0, DNXC_SW_STATE_NO_FLAGS, "ll tails");
    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->prevs, elements_to_alloc * ll->ptr_size, NULL, 0, el_alloc_flags, "ll prevs");
    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->nexts, elements_to_alloc * ll->ptr_size, NULL, 0, el_alloc_flags, "ll nexts");

    if(!ll->is_sorted || (ll->is_sorted && ll->value_size > 0))
    {
        DNX_SW_STATE_ALLOC(unit, node_id, ll->values, uint8, value_size * (elements_to_alloc), el_alloc_flags, "ll values");
    }
    if (ll->is_sorted)
    {
        DNX_SW_STATE_ALLOC(unit, node_id, ll->keys, uint8, key_size * (elements_to_alloc), el_alloc_flags, "ll keys");
        DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->service_head, heads_to_alloc * ll->ptr_size, NULL, 0, DNXC_SW_STATE_NO_FLAGS, "ll service heads");
        DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->service_downs, service_elements_to_alloc * ll->ptr_size, NULL, 0, el_alloc_flags, "ll service downs");
        DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->service_nexts, service_elements_to_alloc * ll->ptr_size, NULL, 0, el_alloc_flags, "ll service nexts");
        DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, ll->service_pointers_keys, service_elements_to_alloc * ll->ptr_size, NULL, 0, el_alloc_flags, "ll service pointers keys");
    }

    
    DNX_SW_STATE_ALLOC(unit, node_id, ll->sandbox, sw_state_ll_node_t, sizeof(sw_state_ll_node_t) * ll->max_level * 3, LL_NO_FLAGS, "ll sandbox");

    
    SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, FREE_NODES_LL_IDX, 1));
    SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, FREE_NODES_LL_IDX, elements_to_alloc-1));
    
    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, FREE_NODES_LL_IDX, reverse_translate_ll_service_idx(ll, 1)));
    }

    
    for (index = 1; index < (elements_to_alloc-1); index++)
    {
        val_to_write = index - 1;
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->prevs, ll->ptr_size * index, 0, ll->ptr_size, &val_to_write);
        val_to_write = index + 1;
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->nexts, ll->ptr_size * index, 0, ll->ptr_size, &val_to_write);
    }

    
    if (ll->is_sorted)
    {
        for (index = 1; index < service_elements_to_alloc - 1 ; index++)
        {
            val_to_write = reverse_translate_ll_service_idx(ll, index + 1);
            DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, ll->service_nexts, ll->ptr_size * index, 0, ll->ptr_size, &val_to_write);
        }
    }

    SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, elements_to_alloc-1, sw_state_ll_invalid));

    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, reverse_translate_ll_service_idx(ll, service_elements_to_alloc-1), sw_state_ll_invalid));
    }

    if (ll->is_sorted)
    {
        sal_srand(700);
        ll->curr_level = 1;
        
        SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, FREE_NODES_LL_IDX, FALSE, &key_pointer));
        for(ll_index = 0; ll_index < init_info->nof_heads; ll_index++)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_temp_node, &sw_state_ll_head_default_val,
                                                        &sw_state_ll_head_default_val, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid,
                                                        key_pointer, SERVICE_NODE));
            SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, ll_index, created_temp_node));
            for (idx = 0; idx < ll->max_level - 2 ; idx++)
            {
                SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_node, &sw_state_ll_head_default_val,
                                                            &sw_state_ll_head_default_val, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid,
                                                            key_pointer, SERVICE_NODE));
                SHR_IF_ERR_EXIT(sw_state_ll_set_service_down(unit, node_id, ll, created_temp_node, created_node));
                created_temp_node = created_node;
            }
            SHR_IF_ERR_EXIT(sw_state_ll_create_new_node(unit, node_id, ll, ll_index, &created_node, &sw_state_ll_head_default_val, &sw_state_ll_head_default_val,
                                                        sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid, sw_state_ll_invalid, !SERVICE_NODE));
            SHR_IF_ERR_EXIT(sw_state_ll_set_service_down(unit, node_id, ll, created_temp_node, created_node));
            SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, 1, created_node));
        }

        if (init_info->key_cmp_cb_name != NULL)
        {
            SW_STATE_CB_DB_REGISTER_CB(node_id, ll->key_cmp_cb, init_info->key_cmp_cb_name, dnx_sw_state_sorted_list_cb_get_cb);
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_RESOURCE, "sw state sorted ll ERROR: ll must be given a compare cb.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }

    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_add(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    assert(ll != NULL);
    if (ll->is_sorted)
    {
        SHR_NULL_CHECK(key, SOC_E_PARAM, "key");
        if (ll->value_size > 0)
        {
            SHR_NULL_CHECK(value, SOC_E_PARAM, "value");
        }
    }
    else
    {
        SHR_NULL_CHECK(value, SOC_E_PARAM, "value");
    }
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    SHR_IF_ERR_EXIT(sw_state_ll_add_internal(unit, node_id, ll, ll_index, SW_STATE_LL_INVALID, key, value));

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_add_first(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add last in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SHR_NULL_CHECK(value, SOC_E_PARAM, "value");
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    SHR_IF_ERR_EXIT(sw_state_ll_add_internal(unit, node_id, ll, ll_index, SW_STATE_LL_INVALID, key, value));

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_add_last(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, const void *key, const void *value)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add last in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SHR_NULL_CHECK(value, SOC_E_PARAM, "value");
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->tail, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);
    SHR_IF_ERR_EXIT(sw_state_ll_add_internal(unit, node_id, ll, ll_index, node_ptr, key, value));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_add_before(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID, node_ptr2 = SW_STATE_LL_INVALID;
    sw_state_ll_node_t node_to_add_after;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add before in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->prevs, ll->ptr_size * node, 0, ll->ptr_size, &node_ptr);
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * node_ptr, 0, ll->ptr_size, &node_ptr2);

    
    if (node_ptr == SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_add_internal(unit, node_id, ll, ll_index, SW_STATE_LL_INVALID, key, value));
    }

    
    assert (node_ptr2 != SW_STATE_LL_INVALID);

    node_to_add_after = node_ptr;

    
    SHR_IF_ERR_EXIT(sw_state_ll_add_internal(unit, node_id, ll, ll_index, node_to_add_after, key, value));

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_add_after(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node, const void *key, const void *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_FAIL, "sw state ll ERROR: cannot use function of add after in sorted linked list.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    SHR_IF_ERR_EXIT(sw_state_ll_add_internal(unit, node_id, ll, ll_index, node, key, value));

    DNXC_SW_STATE_FUNC_RETURN;
}

static shr_error_e
sw_state_ll_return_node_to_pool(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node)
{
    sw_state_ll_node_t head;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, FREE_NODES_LL_IDX, is_service_node(ll, node), &head));

    if (head == SW_STATE_LL_INVALID)
    {
        
        SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, FREE_NODES_LL_IDX, node));
        if (!is_service_node(ll, node))
        {
            SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, FREE_NODES_LL_IDX, node));
        }
        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, node, sw_state_ll_invalid));
        SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, node, sw_state_ll_invalid));
    }
    else
    {
        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, node, head));
        SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, head, node));
        SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, FREE_NODES_LL_IDX, node));
    }

exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_ll_sorted_ll_remove_node(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node)
{
    int PREVS_ADJUSTER = 0;
    int NEXTS_ADJUSTER = ll->max_level;
    int REMOVES_ADJUSTER = ll->max_level * 2;
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;
    sw_state_ll_node_t node_to_remove;

    int nodes_to_remove_num = 0;
    int i = 0;

    uint8 *idx_key_node;
    uint8 is_equal_key = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_CB_DB_GET_CB(
            node_id,
            ll->key_cmp_cb,
            &sorted_list_cmp_cb,
            dnx_sw_state_sorted_list_cb_get_cb
    );

    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, node, &idx_key_node));
    SHR_IF_ERR_EXIT(sw_state_ll_find_nodes_around_node(unit, node_id, ll, ll_index, idx_key_node, value_at_idx(ll, node), TRUE, ll->sandbox));

    
    SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, ll->sandbox[PREVS_ADJUSTER], &node_to_remove));
    SHR_IF_ERR_EXIT(isEqualKey(unit, node_id, ll, sorted_list_cmp_cb, node_to_remove, idx_key_node, &is_equal_key));
    if(!is_equal_key)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: Node %d does not exist, cannot remove.\n%s%s", node, EMPTY, EMPTY);
    }

    for(i=0; i<ll->max_level; i++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, ll->sandbox[i + PREVS_ADJUSTER], &node_to_remove));
        SHR_IF_ERR_EXIT(isEqualKey(unit, node_id, ll, sorted_list_cmp_cb, node_to_remove, idx_key_node, &is_equal_key));
        if(is_equal_key)
        {
            ll->sandbox[i + REMOVES_ADJUSTER] = node_to_remove;
            SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, node_to_remove, &(ll->sandbox[i + NEXTS_ADJUSTER])));
            nodes_to_remove_num++;
        }
        else
            break;
    }

    
    if(ll->sandbox[NEXTS_ADJUSTER] == SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, ll_index, ll->sandbox[PREVS_ADJUSTER]));
    }

    
    SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, ll->sandbox[NEXTS_ADJUSTER], ll->sandbox[PREVS_ADJUSTER]));
    for(i=0; i<nodes_to_remove_num; i++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, node_id, ll, ll->sandbox[i + REMOVES_ADJUSTER]));
        
        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, ll->sandbox[i + PREVS_ADJUSTER], ll->sandbox[i + NEXTS_ADJUSTER]));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_remove_node(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t node)
{
    
    sw_state_ll_node_t prev = SW_STATE_LL_INVALID;
    sw_state_ll_node_t next = SW_STATE_LL_INVALID;
    uint8 is_suppressed = FALSE;

    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID, node_ptr2 = SW_STATE_LL_INVALID;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    if (ll->is_sorted)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_sorted_ll_remove_node(unit, node_id, ll, ll_index, node));
        SOC_EXIT;
    }

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->prevs, ll->ptr_size * node, 0, ll->ptr_size, &node_ptr);
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * node, 0, ll->ptr_size, &node_ptr2);

    
    if ((node_ptr2 != SW_STATE_LL_INVALID) && (node_ptr != SW_STATE_LL_INVALID))
    {
        prev = node_ptr;
        next = node_ptr2;

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, node_id, ll, node));

        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, prev, next));
        SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, next, prev));

        SOC_EXIT;
    }

    
    if ((node_ptr2 == SW_STATE_LL_INVALID) && (node_ptr == SW_STATE_LL_INVALID))
    {
        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, node_id, ll, node));

        SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, ll_index, sw_state_ll_invalid));
        SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, ll_index, sw_state_ll_invalid));

        SOC_EXIT;
    }

    
    if (node_ptr2 == SW_STATE_LL_INVALID)
    {
        prev = node_ptr;

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, node_id, ll, node));

        SHR_IF_ERR_EXIT(sw_state_ll_set_next(unit, node_id, ll, prev, sw_state_ll_invalid));
        SHR_IF_ERR_EXIT(sw_state_ll_set_tail(unit, node_id, ll, ll_index, prev));

        SOC_EXIT;
    }

    
    if (node_ptr == SW_STATE_LL_INVALID)
    {
        next = node_ptr2;

        SHR_IF_ERR_EXIT(sw_state_ll_return_node_to_pool(unit, node_id, ll, node));

        SHR_IF_ERR_EXIT(sw_state_ll_set_prev(unit, node_id, ll, next, sw_state_ll_invalid));
        SHR_IF_ERR_EXIT(sw_state_ll_set_head(unit, node_id, ll, ll_index, next));
    }

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }

    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_node_update(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node, const void *value)
{
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SHR_NULL_CHECK(value, SOC_E_PARAM, "value");

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, value_at_idx(ll, node), value, ll->value_size, LL_NO_FLAGS, "sw state ll node's value");

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }

    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_next_node(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_VALID_NODE(input_node);
    SHR_NULL_CHECK(output_node, SOC_E_PARAM, "output_node");

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * input_node, 0, ll->ptr_size, &node_ptr);
    *output_node = node_ptr;

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_previous_node(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t input_node, sw_state_ll_node_t *output_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SW_STATE_LL_CHECK_VALID_NODE(input_node);
    SHR_NULL_CHECK(output_node, SOC_E_PARAM, "output_node");

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->prevs, ll->ptr_size * input_node, 0, ll->ptr_size, &node_ptr);
    *output_node = node_ptr;

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_get_first(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node)
{
    sw_state_ll_node_t curr_node, next_down_node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(output_node, SOC_E_PARAM, "output_node");

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    if(ll->is_sorted )
    {
        SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, ll_index, TRUE, &curr_node));
        if(ll_index == FREE_NODES_LL_IDX)
        {
            *output_node = curr_node;
        }
        else
        {
            SHR_IF_ERR_EXIT(sw_state_ll_get_service_down(unit, node_id, ll, curr_node, &next_down_node));
            while(next_down_node != SW_STATE_LL_INVALID)
            {
                curr_node = next_down_node;
                SHR_IF_ERR_EXIT(sw_state_ll_get_service_down(unit, node_id, ll, curr_node, &next_down_node));
            }
            SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, curr_node, output_node));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, ll_index, FALSE, output_node));
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_get_last(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t *output_node)
{
    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(output_node, SOC_E_PARAM, "output_node");

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to add to inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->tail, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);
    *output_node = node_ptr;

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_nof_elements(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, uint32 *nof_elements)
{
    sw_state_ll_node_t node;
    int count = 0;

    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to access inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    SHR_NULL_CHECK(nof_elements, SOC_E_PARAM, "nof_elements");

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->head, ll->ptr_size * ll_index, 0, ll->ptr_size, &node_ptr);

    node = node_ptr;

    if(ll->is_sorted && (ll_index != FREE_NODES_LL_IDX))
    {
        SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, ll, ll_index, &node));
    }

    while (node != SW_STATE_LL_INVALID)
    {
        count++;
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * node, 0, ll->ptr_size, &node_ptr);
        node = node_ptr;
    }

    *nof_elements = count;

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_node_key(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *key)
{
    uint8 is_suppressed = FALSE;

    uint8 *idx_key_node;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    
    if (ll == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_NOT_FOUND, "sw state ll ERROR: trying to access inactive ll.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SHR_NULL_CHECK(key, SOC_E_PARAM, "key");

    SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, node, &idx_key_node));
    sal_memcpy(key, idx_key_node, ll->key_size);

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }

    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_node_value(int unit, uint32 node_id, sw_state_ll_t ll, sw_state_ll_node_t node, void *value)
{
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    assert(ll != NULL);

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SHR_NULL_CHECK(value, SOC_E_PARAM, "value");

    sal_memcpy(value, value_at_idx(ll, node), ll->value_size);

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }

    SHR_FUNC_EXIT;
}

static DNXC_SW_STATE_INLINE void
sw_state_ll_data_hexdump(int unit, uint8 *data, uint32 size)
{
    int i;
    assert(data != NULL);
    
    for (i = 0; i < size; i++)
    {
        DNX_SW_STATE_PRINT(unit, "%02X", data[i]);
    }
}


#ifdef SORTED_LL_EXTENDED_PRINT

static shr_error_e
sw_state_ll_print_skipped_ll(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index)
{
    sw_state_ll_node_t node;
    sw_state_ll_node_t head_node;

    uint32 ll_level = ll->max_level;
    uint32 nof_elements;

    uint8 *idx_key_node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    
    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, -1, ll, FREE_NODES_LL_IDX, &nof_elements));
    DNX_SW_STATE_PRINT(unit, "Number of unused nodes: %d\n", nof_elements);

    
    SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, ll_index, TRUE, &head_node));
    while (*head_node != SW_STATE_LL_INVALID)
    {
        DNX_SW_STATE_PRINT(unit, "Printing Linked List Level #%d:\n", ll_level);
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, head_node, &node));
        
        while (node != SW_STATE_LL_INVALID)
        {
            DNX_SW_STATE_PRINT(unit, " %d -> ", node);
            DNX_SW_STATE_PRINT(unit, "key: ");
            SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, node, idx_key_node));
            sw_state_ll_data_hexdump(unit, idx_key_node, ll->key_size);
            DNX_SW_STATE_PRINT(unit, "\n");
            SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, node, &node));
        }
        SHR_IF_ERR_EXIT(sw_state_ll_get_service_down(unit, node_id, ll, head_node, &head_node));
        ll_level--;
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


static shr_error_e
sw_state_ll_print_free_service_nodes_skipped_ll(int unit, uint32 node_id, sw_state_ll_t ll)
{
    sw_state_ll_node_t node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    
    SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, FREE_NODES_LL_IDX, TRUE, &node));

    DNX_SW_STATE_PRINT(unit, "Printing free service nodes :\n");
    while (*node != SW_STATE_LL_INVALID)
    {
        DNX_SW_STATE_PRINT(unit, " %d -> ", node);
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, node, &node));
    }
    DNXC_SW_STATE_FUNC_RETURN;
}

#endif


shr_error_e
sw_state_ll_print(int unit, sw_state_ll_t ll)
{
    sw_state_ll_node_t node;
    int count = 0;
    uint32 ll_index;
    uint32 nof_elements;
    int nof_usable_elements;
    uint32 node_id = 0;

    uint8 *idx_key_node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);


    
    for (ll_index = 0; ll_index < ll->nof_heads; ll_index++)
    {
        #ifdef SORTED_LL_EXTENDED_PRINT
            DNX_SW_STATE_PRINT(unit, "Printing ll_index: %d\n", ll_index);
            if (ll->is_sorted)
            {
                sw_state_ll_print_skipped_ll(unit, node_id, ll, ll_index);
                sw_state_ll_print_free_service_nodes_skipped_ll(unit, node_id, ll);
            }
        #endif
        SHR_IF_ERR_EXIT(sw_state_ll_get_head(unit, node_id, ll, ll_index, FALSE, &node));

        
        SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, node_id, ll, ll_index, &nof_elements));

        
        nof_usable_elements = ll->max_elements - 1;
        if(ll->is_sorted)
        {
            
            nof_usable_elements-=ll->nof_heads;
        }

        DNX_SW_STATE_PRINT(unit, "Number of unused nodes: %d\n", (nof_usable_elements - nof_elements));

        if (ll->nof_heads == 1)
        {
            DNX_SW_STATE_PRINT(unit, "Printing Linked List:\n");
        }
        else {
            DNX_SW_STATE_PRINT(unit, "Printing Linked List #%d:\n", ll_index);
        }

        
        if (ll->is_sorted)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, -1, ll, ll_index, &node));
        }

        
        while (node != SW_STATE_LL_INVALID)
        {
            count++;
            DNX_SW_STATE_PRINT(unit, " %d -> ", count);
            if (ll->is_sorted)
            {
                DNX_SW_STATE_PRINT(unit, "key: ");
                SHR_IF_ERR_EXIT(key_at_idx(unit, node_id, ll, node, &idx_key_node));
                sw_state_ll_data_hexdump(unit, idx_key_node, ll->key_size);
                if(ll->value_size > 0)
                {
                    DNX_SW_STATE_PRINT(unit, ", value: ");
                    sw_state_ll_data_hexdump(unit, value_at_idx(ll, node), ll->value_size);
                }
            }
            else
            {
                sw_state_ll_data_hexdump(unit, value_at_idx(ll, node), ll->value_size);
            }
            DNX_SW_STATE_PRINT(unit, "\n");
            SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, node, &node));
        }
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_find(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index, sw_state_ll_node_t * node, const void *key, uint8* found)
{
    dnx_sw_state_sorted_list_cb sorted_list_cmp_cb;

    SHR_BITDCL node_ptr = SW_STATE_LL_INVALID;
    uint8 is_equal_key = FALSE;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    assert(ll->is_sorted);

    SW_STATE_LL_CHECK_VALID_NODE(node);
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);
    SHR_NULL_CHECK(key, SOC_E_PARAM, "key");
    SHR_NULL_CHECK(found, SOC_E_PARAM, "found");

    SW_STATE_CB_DB_GET_CB(
        node_id,
        ll->key_cmp_cb,
        &sorted_list_cmp_cb,
        dnx_sw_state_sorted_list_cb_get_cb
    );

    SHR_IF_ERR_EXIT(sw_state_ll_find_nodes_around_node(unit, node_id, ll, ll_index, key, NULL, TRUE, ll->sandbox));

    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, ll->nexts, ll->ptr_size * ll->sandbox[0], 0, ll->ptr_size, &node_ptr);

    SHR_IF_ERR_EXIT(isEqualKey(unit, node_id, ll, sorted_list_cmp_cb, node_ptr, key, &is_equal_key));
    if (is_equal_key)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, ll->sandbox[0], node));
        *found = 1;
    }
    else
    {
        *node = ll->sandbox[0];
        *found=0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_ll_clear(int unit, uint32 node_id, sw_state_ll_t ll)
{
    uint32 ll_index;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);

    
    for (ll_index = 0; ll_index < ll->nof_heads; ll_index++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_clear_all_at_index(unit, node_id, ll, ll_index));
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


shr_error_e
sw_state_ll_clear_all_at_index(int unit, uint32 node_id, sw_state_ll_t ll, uint32 ll_index)
{
    sw_state_ll_node_t node, next_node;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ll != NULL);
    SW_STATE_LL_CHECK_LL_INDEX_OOB(ll, ll_index);

    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, ll, ll_index, &node));

    while (node != SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_get_next(unit, node_id, ll, node, &next_node));
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, ll, ll_index, node));
        node = next_node;
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


