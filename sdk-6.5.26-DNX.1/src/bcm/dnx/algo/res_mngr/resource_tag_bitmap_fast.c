/** \file resource_tag_bitmap_fast.c
 *
 * Indexed resource management
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
typedef int make_iso_compilers_happy_resource_rag_bitmap;

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/*undef if you want print the linkes list that hold the non full grains*/
/*#define EMPTY_LL_GRAINS_PRINT*/

/** { */
/*************
* INCLUDES  *
*************/
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap_fast.h>

#ifdef BCM_DNX_SUPPORT

/********************
 *
 *********************/

/*
 * max tag value + 1 represent empty group, relevant in case of resource with grains
 */
static int
_rtb_fast_get_empty_group(
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    int tag_level)
{
    return (res_tag_bitmap_fast->resource->tag_level_info[tag_level].max_tag_value + 1);
}

/*
 * Return number of optional tags in resource manager
 * include tags for empty groups
 */
static uint32
_rtb_get_total_nof_ll_heads(
    uint32 max_tags_value[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS],
    int nof_tags_levels)
{
    if (nof_tags_levels > 1)
    {
        /*
         * e.g. in case of max_tag_vals={1,2}, the optional tags are:
         * {empty, empty}, {0, empty}, {0,0}, {0,1}, {0,2}, {1,empty}, {1,0}, {1,1}, {1,2}
         */
        return (max_tags_value[1] + 2) * (max_tags_value[0] + 1) + 1;
    }
    else
    {
        /*
         * e.g. in case of max_tag_vals={2}, the optional tags are:
         * {empty}, {0}, {1}, {2}
         */
        return max_tags_value[0] + 2;
    }
}

/*
 *The function return lower tags level of the given tags. e.g. {1,3} -> {1,empty}
 *Return value: TRUE if found lower tag level
 FALSE if tags are already in the lower level
 (e.g. when tags = {empty, empty} - a lower level is not exist and return value = FALSE)
 */
static uint8
_rtb_fast_get_lower_tags_level(
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    for (int tag_level = resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource); tag_level >= 0;
         tag_level--)
    {
        if (tags[tag_level] != _rtb_fast_get_empty_group(res_tag_bitmap_fast, tag_level))
        {
            tags[tag_level] = _rtb_fast_get_empty_group(res_tag_bitmap_fast, tag_level);
            return TRUE;
        }
    }
    return FALSE;
}

/*
 *The function returns the head linked list index that represented by the tags
 */
static int
_rtb_get_ll_head_idx_by_tags(
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    return tags[1] * (res_tag_bitmap_fast->resource->tag_level_info[1].max_tag_value) + tags[0];
}

/*
 *The function returns the head linked list index that represented by tags in lower level
 * e.g. tags = {3,2} -> lower_tags = {3, empty}
 */
static int inline
_rtb_fast_get_lower_ll_head_idx(
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    uint32 lower_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };

    sal_memcpy(lower_tags, tags, sizeof(*tags) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
    _rtb_fast_get_lower_tags_level(res_tag_bitmap_fast, lower_tags);
    return _rtb_get_ll_head_idx_by_tags(res_tag_bitmap_fast, lower_tags);
}

/*
 *The function returns the head node(first node in the list) of the linked list that represented by the given tags
 */
static shr_error_e
_rtb_get_ll_head_node_by_tags(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS],
    sw_state_ll_node_t * grain_node)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, res_tag_bitmap_fast->free_grains,
                                          _rtb_get_ll_head_idx_by_tags(res_tag_bitmap_fast, tags), grain_node));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_create_verify(
    int unit,
    uint32 create_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (create_flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(create_flags, _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS"
                                     " is not supported");
    }

    if (create_flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(create_flags, _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS"
                                     " is not supported");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *The function initialize the free grains linked list.
 *All the grains in the linked list assigned to the the linked list head that is represented by the empty groups tags {max_tag0_val+1,max_tag1_val+1}
 */
static shr_error_e
_rtb_fast_initialize_free_fast(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    uint32 nof_grains)
{
    SHR_FUNC_INIT_VARS(unit);

    for (int grain_idx = 0; grain_idx < nof_grains; grain_idx++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, res_tag_bitmap_fast->free_grains,
                                        res_tag_bitmap_fast->empty_groups_ll_head_idx, &grain_idx, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static void
_rtb_fast_update_allocated_elements_per_grain(
    resource_tag_bitmap_utils_t resource,
    int element,
    int allocated_bits)
{
    int grain_idx, tag_level;
    for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
    {
        grain_idx = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, element);
        resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_idx] += allocated_bits;
    }
}

static shr_error_e
_rtb_fast_find_grain_by_allocated_element_and_tags(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS],
    int allocated_element,
    sw_state_ll_node_t * grain_node,
    uint8 *grain_found)
{
    int grain_idx = 0, ll_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    grain_idx =
        resource_tag_bitmap_utils_get_grain_index(res_tag_bitmap_fast->resource,
                                                  resource_tag_bitmap_utils_get_higher_tag_level
                                                  (res_tag_bitmap_fast->resource), allocated_element);
    ll_index = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap_fast, tags);
    SHR_IF_ERR_EXIT(sw_state_ll_find(unit, node_id, res_tag_bitmap_fast->free_grains, ll_index, grain_node,
                                     (void *) (&(grain_idx)), grain_found));

exit:
    SHR_FUNC_EXIT;
}

/*This function moving grain in the multi head linked list from ll_head_idx to_ll_head_idx*/
static shr_error_e
_rtb_fast_move_grain(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    sw_state_ll_node_t grain_node,
    int from_ll_head_idx,
    int to_ll_head_idx)
{
    int grain_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_node_key(unit, node_id, res_tag_bitmap_fast->free_grains, grain_node,
                                         (void *) &grain_idx));
    SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, res_tag_bitmap_fast->free_grains, from_ll_head_idx,
                                            grain_node));
    SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, res_tag_bitmap_fast->free_grains, to_ll_head_idx, &grain_idx, NULL));

exit:
    SHR_FUNC_EXIT;
}


/*
 * This function moving 'neighbors' grains from_ll_head_idx to_ll_head_idx
 * 'neighbors' means small grains in big grain
 * Relevant only for resource with more then 1 tags levels (multilevel)
 * e.g. nof_tags_levels = 2, grains_size = {6,2}, nof_elements = 12.
 * level 0 - |           0           |               1         |
 * level 1 - |   0   |   1   |   2   |   3   |   4   |    5    |
 * bitmap  - | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
 * The neighbors of grain {1} in {level 1} are {0,2}
 * The neighbors of grain {3} in {level 1} are {4,5}
 * */
static shr_error_e
_rtb_fast_move_neighbors(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    int from_ll_head_idx,
    int to_ll_head_idx,
    int range_start)
{
    int grain_idx = 0, range_end = 0;
    uint8 grain_found = FALSE;
    sw_state_ll_node_t grain_node = SW_STATE_LL_INVALID, next_grain_node = SW_STATE_LL_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    range_end =
        range_start + resource_tag_bitmap_utils_get_nof_small_grains_in_bigger_grains(res_tag_bitmap_fast->resource);

    /*
     * Searching for the first grain in the linked list indicated by from_ll_head_idx
     */
    for (grain_idx = range_start; grain_idx < range_end && !grain_found; grain_idx++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_find(unit, node_id, res_tag_bitmap_fast->free_grains, from_ll_head_idx, &grain_node,
                                         (void *) (&(grain_idx)), &grain_found));
    }

    grain_idx--;
    /*
     * In case a grain was found in from_ll_head_idx search for neighbors in the range and moving
     * all the grains in the range to to_ll_head_idx
     */
    if (grain_found && grain_idx < range_end)
    {
        while (grain_node != SW_STATE_LL_INVALID)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_next_node(unit, node_id, res_tag_bitmap_fast->free_grains, from_ll_head_idx,
                                                  grain_node, &next_grain_node));
            SHR_IF_ERR_EXIT(sw_state_ll_node_key(unit, node_id, res_tag_bitmap_fast->free_grains, grain_node,
                                                 (void *) &grain_idx));
            if (grain_idx < range_end)
            {
                SHR_IF_ERR_EXIT(_rtb_fast_move_grain(unit, node_id, res_tag_bitmap_fast, grain_node, from_ll_head_idx,
                                                     to_ll_head_idx));
            }
            else
            {
                SHR_EXIT();
            }
            grain_node = next_grain_node;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * The function searching for free grain that fit the grained_alloc_info and update the struct accordingly
 */
static shr_error_e
_rtb_fast_find_free_grain(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_grained_alloc_info_t * grained_alloc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case grain node is not invalid we need to search for the next grain in the list
     */
    if (grained_alloc_info->grain_node != SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_next_node(unit, node_id, res_tag_bitmap_fast->free_grains, 0,
                                              grained_alloc_info->grain_node, &(grained_alloc_info->grain_node)));
        /*
         * In case grained_alloc_info->grain_node is valid -> a candidate grain was found
         * In case grained_alloc_info->grain_node is invalid -> it's mean there is no grain with free elements in the same list.
         * We will check if in case of lower tag level match, free grain exist.
         */
        if (grained_alloc_info->grain_node != SW_STATE_LL_INVALID
            || (grained_alloc_info->grain_node == SW_STATE_LL_INVALID
                && _rtb_fast_get_lower_tags_level(res_tag_bitmap_fast, grained_alloc_info->tags) == FALSE))
        {
            SHR_EXIT();
        }
    }
    /*
     * In case grain node is invalid it's mean that we need to search for free grain in the list according to the tags
     * The list with the head that was calculated by the tags does not have an empty grains. Searching in
     * another list with lower level of tag than removing. e.g. 1) first try with tags = {1,2} here it will
     * check for {1,0} 2) first try with tags = {1,0} here it will check for {0,0} when 0 represent empty group
     */
    if (grained_alloc_info->grain_node == SW_STATE_LL_INVALID)
    {
        do
        {
            _rtb_get_ll_head_node_by_tags(unit, node_id, res_tag_bitmap_fast, grained_alloc_info->tags,
                                          &(grained_alloc_info->grain_node));
        }
        while (grained_alloc_info->grain_node == SW_STATE_LL_INVALID
               && _rtb_fast_get_lower_tags_level(res_tag_bitmap_fast, grained_alloc_info->tags) == TRUE);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_get_elements_range_in_grain_node(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_grained_alloc_info_t * grained_alloc_info)
{
    int grain_idx = 0, grain_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_node_key
                    (unit, node_id, res_tag_bitmap_fast->free_grains, grained_alloc_info->grain_node,
                     (void *) (&grain_idx)));
    grain_size =
        res_tag_bitmap_fast->
        resource->tag_level_info[resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource)].
        grain_size;
    grained_alloc_info->grain_start = grain_idx * grain_size;
    grained_alloc_info->grain_end = grained_alloc_info->grain_start + grain_size;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_alloc_update_ll(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    resource_tag_bitmap_grained_alloc_info_t grained_alloc_info,
    int allocated_element,
    int allocated_bits)
{
    int grain_ll_head_idx = 0, alloc_ll_head_idx = 0, lower_alloc_ll_head_idx = 0, tag_level = 0, nof_free_elements = 0,
        range_start = 0, grain_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    grain_idx =
        resource_tag_bitmap_utils_get_grain_index(res_tag_bitmap_fast->resource,
                                                  resource_tag_bitmap_utils_get_higher_tag_level
                                                  (res_tag_bitmap_fast->resource), allocated_element);
    nof_free_elements =
        resource_tag_bitmap_utils_get_nof_free_elements(res_tag_bitmap_fast->resource,
                                                        resource_tag_bitmap_utils_get_higher_tag_level
                                                        (res_tag_bitmap_fast->resource), grain_idx);
    grain_ll_head_idx = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap_fast, grained_alloc_info.tags);
    alloc_ll_head_idx = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap_fast, alloc_info.tags);

    if (nof_free_elements == 0)
    {
        /*
         * No more free elements in the grain, the grain will be removed from the free grains linked list
         */
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, res_tag_bitmap_fast->free_grains, grain_ll_head_idx,
                                                grained_alloc_info.grain_node));
    }
    if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)
    {
        /*
         * In case of allocation with this flag, we should not do anything beside removing the grain node in case there
         * are no more free elements in the grain
         */
        SHR_EXIT();
    }
    if (alloc_ll_head_idx != grain_ll_head_idx)
    {
        /*
         * The grain is not in the right linked list head, the grain/neighbors should night be moved
         */
        if (nof_free_elements > 0)
        {
            /*
             * Moving from {empty, empty}/{tag,empty} linked list head index to the list with the tags of the allocation
             */
            SHR_IF_ERR_EXIT(_rtb_fast_move_grain(unit, node_id, res_tag_bitmap_fast, grained_alloc_info.grain_node,
                                                 grain_ll_head_idx, alloc_ll_head_idx));
        }
        /*
         * The grain is in the {empty, empty} ll or in the {tag, empty}
         */
        if (resource_tag_bitmap_utils_is_multilevel_tags_resource(res_tag_bitmap_fast->resource)
            && grain_ll_head_idx == res_tag_bitmap_fast->empty_groups_ll_head_idx)
        {
            /*
             * Reorganize multihead linked lists, In case of multilevel linked list and the free grain was taken from
             * the empty group, we should remove from empty group and move to lower level all the neighbors.
             * e.g. in first allocation of resource with nof_tags_level = 2, grain_size = {10,5}, nof_elements=20, tags={2,1}
             * Mulithead ll free grains BEFORE allocation : (empty, empty) -> 0->1->2->3
             *                          AFTER  allocation : (empty, empty) -> 2->3 (2,empty) -> 1 (2,1) -> 0
             */
            range_start = resource_tag_bitmap_utils_get_nof_small_grains_in_bigger_grains(res_tag_bitmap_fast->resource)
                * resource_tag_bitmap_utils_get_grain_index(res_tag_bitmap_fast->resource, 0, allocated_element);
            lower_alloc_ll_head_idx = _rtb_fast_get_lower_ll_head_idx(res_tag_bitmap_fast, alloc_info.tags);
            SHR_IF_ERR_EXIT(_rtb_fast_move_neighbors(unit, node_id, res_tag_bitmap_fast, grain_ll_head_idx,
                                                     lower_alloc_ll_head_idx, range_start));
            for (tag_level = 0;
                 tag_level < resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource); tag_level++)
            {
                alloc_info.first_references[tag_level] = 1;
            }
        }
        alloc_info.first_references[resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource)] = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_check_alloc_with_id(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    int allocated_block_length,
    int *pattern_first_index,
    resource_tag_bitmap_grained_alloc_info_t * grained_alloc_info,
    int *element,
    int *allocation_success)
{
    uint8 grain_found;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Pattern first index is the element from which the pattern starts.
     * In sparse allocation, the element that we return is the first ALLOCATED element, but if the first bit of the
     * pattern is 0 then it won't be the same as the first bit in the pattern.
     */
    *pattern_first_index =
        *element - res_tag_bitmap_fast->resource->first_element - utilex_lsb_bit_on(alloc_info.pattern);

    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info.flags,
                                    resource_tag_bitmap_utils_check_alloc_with_id(unit, node_id,
                                                                                  res_tag_bitmap_fast->resource,
                                                                                  &alloc_info, allocated_block_length,
                                                                                  pattern_first_index, element,
                                                                                  allocation_success));

    /*
     * Update grained_alloc_info information for the grain_node_tags and the grain_node
     * */
    if (allocation_success)
    {
        do
        {
            RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info.flags,
                                            _rtb_fast_find_grain_by_allocated_element_and_tags(unit, node_id,
                                                                                               res_tag_bitmap_fast,
                                                                                               grained_alloc_info->tags,
                                                                                               *element,
                                                                                               &
                                                                                               (grained_alloc_info->grain_node),
                                                                                               &grain_found));
        }
        while (grain_found == FALSE && _rtb_fast_get_lower_tags_level(res_tag_bitmap_fast, grained_alloc_info->tags));

        if (grain_found == FALSE)
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_RESOURCE,
                                         "Internal error, cannot find the grain in the non full grains linked list\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_alloc_verify(
    int unit,
    int node_id,
    uint32 alloc_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO"
                                     " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_ALIGN"
                                     " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG"
                                     " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG"
                                     " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_SPARSE"
                                     " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE"
                                     " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_flags,
                                     _SHR_E_PARAM,
                                     "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK"
                                     " is not supported");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_alloc_next_free_element_in_grain(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    resource_tag_bitmap_grained_alloc_info_t grained_alloc_info,
    int allocated_block_length,
    int *element,
    int *allocation_success)
{
    int index = 0, last_legal_index = 0, pattern_first_set_bit = 0, current_offset_index = 0;
    uint8 temp = TRUE, grain_in_use = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    index = (((grained_alloc_info.grain_start + alloc_info.align - 1) / alloc_info.align) * alloc_info.align);
    last_legal_index = grained_alloc_info.grain_end - allocated_block_length;
    pattern_first_set_bit = utilex_lsb_bit_on(alloc_info.pattern);

    if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN)
    {
        {
            SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_is_grain_in_use
                            (unit, node_id, res_tag_bitmap_fast->resource, alloc_info.flags,
                             resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource),
                             grained_alloc_info.grain_start,
                             grained_alloc_info.grain_end - grained_alloc_info.grain_start, &grain_in_use));
        }
        if (grain_in_use)
        {
            *allocation_success = FALSE;
            SHR_EXIT();
        }
    }

    /*
     * Iterate between range_start and range_end, and check for suitable elements in the middle.
     */
    while (index <= last_legal_index && *allocation_success == FALSE)
    {
        while (temp && (index <= last_legal_index))
        {
            for (current_offset_index = 0; current_offset_index < alloc_info.nof_offsets && temp;
                 current_offset_index++)
            {
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap_fast->resource->data,
                                     index + pattern_first_set_bit + alloc_info.offs[current_offset_index], &temp);
            }
            if (temp)
            {
                index += alloc_info.align;
            }
        }
        /*
         * if founded empty bit and still in valid range
         */
        if (!temp && index <= last_legal_index)
        {
            *element = index + alloc_info.offs[current_offset_index];
            resource_tag_bitmap_utils_is_block_free(unit, node_id, res_tag_bitmap_fast->resource, alloc_info.pattern,
                                                    alloc_info.length, alloc_info.repeats, *element,
                                                    allocation_success);
        }
        if (*allocation_success == FALSE)
        {
            index += alloc_info.align;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_fast_check_allocation(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    int allocated_block_length,
    int *pattern_first_index,
    resource_tag_bitmap_grained_alloc_info_t * grained_alloc_info,
    int *element,
    int *allocation_success)
{
    SHR_FUNC_INIT_VARS(unit);
    *allocation_success = FALSE;

    sal_memcpy(grained_alloc_info->tags, alloc_info.tags,
               sizeof(*(alloc_info.tags)) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
    if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
    {
        _rtb_fast_check_alloc_with_id(unit, node_id, res_tag_bitmap_fast, alloc_info, allocated_block_length,
                                      pattern_first_index, grained_alloc_info, element, allocation_success);
    }
    else
    {
        do
        {
            SHR_IF_ERR_EXIT(_rtb_fast_find_free_grain(unit, node_id, res_tag_bitmap_fast, grained_alloc_info));
            if (grained_alloc_info->grain_node != SW_STATE_LL_INVALID)
            {
                SHR_IF_ERR_EXIT(_rtb_fast_get_elements_range_in_grain_node(unit, node_id, res_tag_bitmap_fast,
                                                                           grained_alloc_info));

                SHR_IF_ERR_EXIT(_rtb_fast_alloc_next_free_element_in_grain
                                (unit, node_id, res_tag_bitmap_fast, alloc_info, *grained_alloc_info,
                                 allocated_block_length, pattern_first_index, allocation_success));
            }
        }
        while (grained_alloc_info->grain_node != SW_STATE_LL_INVALID && *allocation_success == FALSE);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * The function update the non full grains in the multihead linked list according to the elements that were free.
 */
static shr_error_e
_rtb_fast_free_update_free_fast(
    int unit,
    int node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    int nof_elements_to_free,
    int element,
    uint8 *last_reference)
{
    int grains_idx[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    uint32 freeing_grain_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    int grain_size = 0, tag_level = 0, lower_free_ll_head_idx = 0, free_ll_head_idx = 0, nof_free_elements = 0,
        range_start = 0;
    sw_state_ll_node_t grain_node = SW_STATE_LL_INVALID;
    uint8 grain_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    grain_size =
        res_tag_bitmap_fast->
        resource->tag_level_info[resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource)].
        grain_size;
    for (tag_level = 0; tag_level < res_tag_bitmap_fast->resource->nof_tags_levels; tag_level++)
    {
        grains_idx[tag_level] =
            resource_tag_bitmap_utils_get_grain_index(res_tag_bitmap_fast->resource, tag_level, element);
    }
    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get_internal
                    (unit, node_id, res_tag_bitmap_fast->resource, element, freeing_grain_tags));
    free_ll_head_idx = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap_fast, freeing_grain_tags);
    nof_free_elements =
        resource_tag_bitmap_utils_get_nof_free_elements(res_tag_bitmap_fast->resource,
                                                        resource_tag_bitmap_utils_get_higher_tag_level
                                                        (res_tag_bitmap_fast->resource),
                                                        grains_idx[resource_tag_bitmap_utils_get_higher_tag_level
                                                                   (res_tag_bitmap_fast->resource)]);
    _rtb_fast_update_allocated_elements_per_grain(res_tag_bitmap_fast->resource, element, -nof_elements_to_free);
    if (nof_free_elements > 0)
    {
        /*
         * Searching only in case of nof_free_elements bigger then zero,
         * otherwise the grain was full before and was not part of the ll->free_grains.
         */
        SHR_IF_ERR_EXIT(_rtb_fast_find_grain_by_allocated_element_and_tags(unit, node_id, res_tag_bitmap_fast,
                                                                           freeing_grain_tags, element, &grain_node,
                                                                           &grain_found));

        if (nof_free_elements + nof_elements_to_free == grain_size)
        {
            if (res_tag_bitmap_fast->resource->tag_level_info[0].nof_allocated_elements_per_grain[grains_idx[0]] == 0)
            {
                SHR_IF_ERR_EXIT(_rtb_fast_move_grain(unit, node_id, res_tag_bitmap_fast, grain_node, free_ll_head_idx,
                                                     res_tag_bitmap_fast->empty_groups_ll_head_idx));
            }
            else
            {   /* valid only to multilevel_tags_resource */
                lower_free_ll_head_idx = _rtb_fast_get_lower_ll_head_idx(res_tag_bitmap_fast, freeing_grain_tags);
                SHR_IF_ERR_EXIT(_rtb_fast_move_grain(unit, node_id, res_tag_bitmap_fast, grain_node, free_ll_head_idx,
                                                     lower_free_ll_head_idx));
            }
        }
    }
    else
    {
        /*
         * The grain was full before and was not part of the ll->free_grains.
         * The grain node should be added to {tag, tag}, {tag, empty} or {empty, empty}
         */
        if (nof_free_elements + nof_elements_to_free < grain_size)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_add
                            (unit, node_id, res_tag_bitmap_fast->free_grains, free_ll_head_idx,
                             &grains_idx[resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource)],
                             NULL));
        }
        else if (res_tag_bitmap_fast->resource->tag_level_info[0].nof_allocated_elements_per_grain[grains_idx[0]] == 0)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_add
                            (unit, node_id, res_tag_bitmap_fast->free_grains,
                             res_tag_bitmap_fast->empty_groups_ll_head_idx,
                             &grains_idx[resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource)],
                             NULL));
        }
        else
        {       /* valid only to multilevel_tags_resource , *
                 * res_tag_bitmap_fast->resource->tag_level_info[1].nof_allocated_elements_per_grain[grains_idx[1]] ==
                 * 0 */
            lower_free_ll_head_idx = _rtb_fast_get_lower_ll_head_idx(res_tag_bitmap_fast, freeing_grain_tags);
            SHR_IF_ERR_EXIT(sw_state_ll_add
                            (unit, node_id, res_tag_bitmap_fast->free_grains, lower_free_ll_head_idx,
                             &grains_idx[resource_tag_bitmap_utils_get_higher_tag_level(res_tag_bitmap_fast->resource)],
                             NULL));
        }
    }

    /*
     * check if need to update the neighbors grains
     */
    if (resource_tag_bitmap_utils_is_multilevel_tags_resource(res_tag_bitmap_fast->resource)
        && res_tag_bitmap_fast->resource->tag_level_info[0].nof_allocated_elements_per_grain[grains_idx[0]] == 0)
    {
        /*
         * No more allocated grains in the grain that include the small grains, move all the neighbors grains from
         * {tag, empty} to {empty, empty}
         */
        range_start =
            resource_tag_bitmap_utils_get_nof_small_grains_in_bigger_grains(res_tag_bitmap_fast->resource) *
            grains_idx[0];
        lower_free_ll_head_idx = _rtb_fast_get_lower_ll_head_idx(res_tag_bitmap_fast, freeing_grain_tags);
        SHR_IF_ERR_EXIT(_rtb_fast_move_neighbors(unit, node_id, res_tag_bitmap_fast, lower_free_ll_head_idx,
                                                 res_tag_bitmap_fast->empty_groups_ll_head_idx, range_start));
    }

    /*
     * update last reference
     */
    for (tag_level = 0; tag_level < res_tag_bitmap_fast->resource->nof_tags_levels; tag_level++)
    {
        last_reference[tag_level] =
            (res_tag_bitmap_fast->resource->
             tag_level_info[tag_level].nof_allocated_elements_per_grain[grains_idx[tag_level]] == 0) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_create(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags)
{
    uint32 nof_grains;
    resource_tag_bitmap_utils_create_info_t rtb_create_info;
    sw_state_ll_init_info_t free_grains_init_info;
    resource_tag_bitmap_fast_t *rtb_fast;
    int higher_tag_level;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&free_grains_init_info, 0, sizeof(sw_state_ll_init_info_t));
    DNX_SW_STATE_ALLOC(unit, node_id, *((resource_tag_bitmap_fast_t *) res_tag_bitmap_fast), **((resource_tag_bitmap_fast_t *) res_tag_bitmap_fast),    /* nof 
                                                                                                                                                         * elements 
                                                                                                                                                         */ 1, 0,
                       "sw_state res_tag_bitmap");

    rtb_fast = (resource_tag_bitmap_fast_t *) res_tag_bitmap_fast;

    create_info->flags |= RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_create(unit, node_id, &((*rtb_fast)->resource), create_info,
                                                     extra_arguments, alloc_flags, &rtb_create_info));

    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(_rtb_fast_create_verify(unit, rtb_create_info.flags)));

    higher_tag_level = resource_tag_bitmap_utils_get_higher_tag_level((*rtb_fast)->resource);
    nof_grains = resource_tag_bitmap_utils_get_nof_grains(rtb_create_info.count,
                                                          rtb_create_info.grains_size[higher_tag_level],
                                                          rtb_create_info.first_element,
                                                          higher_tag_level, rtb_create_info.flags);
    free_grains_init_info.nof_heads =
        _rtb_get_total_nof_ll_heads(rtb_create_info.max_tags_value, (*rtb_fast)->resource->nof_tags_levels);
    free_grains_init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp_uint32";
    SHR_IF_ERR_EXIT(sw_state_ll_create_empty(unit, node_id,
                                             &free_grains_init_info,
                                             SW_STATE_LL_SORTED | SW_STATE_LL_MULTIHEAD,
                                             sizeof(int),
                                             0,
                                             &((*rtb_fast)->free_grains),
                                             nof_grains, RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE));
    (*rtb_fast)->empty_groups_ll_head_idx =
        _rtb_fast_get_empty_group(*rtb_fast, 1) *
        ((*rtb_fast)->resource->tag_level_info[1].max_tag_value) + _rtb_fast_get_empty_group(*rtb_fast, 0);
    SHR_IF_ERR_EXIT(_rtb_fast_initialize_free_fast(unit, node_id, *rtb_fast, nof_grains));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_fast_t res_tag_bitmap_fast,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int *elem)
{
    int pattern_first_index, allocated_block_length, allocated_bits;
    int allocation_success = FALSE;
    resource_tag_bitmap_grained_alloc_info_t grained_alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags, resource_tag_bitmap_utils_alloc_pre_process(unit, alloc_info));

    /*
     * After all fields are adjusted, verify the input.
     */
    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                    resource_tag_bitmap_utils_alloc_verify(unit, res_tag_bitmap_fast->resource,
                                                                           *alloc_info, elem));

    DNXC_VERIFY_INVOKE(RESOURCE_TAG_BITMAP_IF_ERR_EXIT
                       (alloc_info->flags, _rtb_fast_alloc_verify(unit, node_id, alloc_info->flags)));

    /*
     * Calculate the length of the allocated block.
     * This value will be used to determine if the last element in the block that we're currently inspecting
     * is greater than the end of the bitmap. If it is, then we don't need to check it.
     * If the allocation is not sparse allocation, then the block length will just be the number of allocated elements.
     * If the allocation is sparse, then it will be the length of the pattern times repeats, minus the trailing 0s
     * that may be at the end of the block.
     */
    allocated_block_length = resource_tag_bitmap_utils_get_allocated_block_length(*alloc_info);

    sal_memset(&grained_alloc_info, 0, sizeof(grained_alloc_info));
    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                    _rtb_fast_check_allocation(unit, node_id, res_tag_bitmap_fast, *alloc_info,
                                                               allocated_block_length, &pattern_first_index,
                                                               &grained_alloc_info, elem, &allocation_success));

    if ((allocation_success) && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                        resource_tag_bitmap_utils_update_resource_after_allocation(unit, node_id,
                                                                                                   res_tag_bitmap_fast->resource,
                                                                                                   alloc_info,
                                                                                                   pattern_first_index,
                                                                                                   elem,
                                                                                                   &allocated_bits));
        RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                        _rtb_fast_alloc_update_ll(unit, node_id, res_tag_bitmap_fast, *alloc_info,
                                                                  grained_alloc_info, pattern_first_index,
                                                                  allocated_bits));
    }
    else if (allocation_success)
    {
        /*
         * if here, allocation can be successful but was run in simulation(RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY flag is on)
         * thus, return the first element in the pattern that can be allocated.
         */
        *elem =
            pattern_first_index + res_tag_bitmap_fast->resource->first_element + utilex_lsb_bit_on(alloc_info->pattern);
    }
    else
    {
        /*
         * allocation was failed (allocation_success = FALSE)
         */
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info->flags, _SHR_E_RESOURCE,
                                     "No free element matching required conditions. Asked for %d elements aligned to %d, with offs %d, and tags %d, %d\n",
                                     allocated_block_length, alloc_info->align, alloc_info->offs[0],
                                     alloc_info->tags[0], alloc_info->tags[1]);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_allocate_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element)
{
    int rv;
    resource_tag_bitmap_utils_alloc_info_t res_tag_bitmap_alloc_info;
    resource_tag_bitmap_utils_extra_arg_alloc_info_t *alloc_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    resource_tag_bitmap_utils_alloc_info_init(unit, node_id, &res_tag_bitmap_alloc_info, &alloc_info,
                                              flags, nof_elements, extra_arguments);

    rv = resource_tag_bitmap_fast_alloc(unit, node_id, res_tag_bitmap_fast, &res_tag_bitmap_alloc_info, element);

    /*
     * Return indication for the user whether it's the first element in the grain
     */
    if (extra_arguments != NULL)
    {
        sal_memcpy(alloc_info->first_references, res_tag_bitmap_alloc_info.first_references,
                   sizeof(alloc_info->first_references) *
                   ((resource_tag_bitmap_fast_t) res_tag_bitmap_fast)->resource->nof_tags_levels);
    }
    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(flags, rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_allocate_single(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    SHR_FUNC_INIT_VARS(unit);

    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(flags, resource_tag_bitmap_fast_allocate_several(unit, node_id, res_tag_bitmap_fast, flags, 1       /* nof 
                                                                                                                                         * elements 
                                                                                                                                         */ ,
                                                                                     extra_arguments, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_tag_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    void *tag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get
                    (unit, node_id, ((resource_tag_bitmap_fast_t) res_tag_bitmap_fast)->resource, tag_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_print(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    dnx_algo_res_dump_data_t * data)
{
    resource_tag_bitmap_fast_t res_mngr = *(resource_tag_bitmap_fast_t *) res_tag_bitmap_fast;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_print(unit, node_id, res_mngr->resource, data));

#ifdef EMPTY_LL_GRAINS_PRINT
    SHR_IF_ERR_EXIT(sw_state_ll_print(0, res_mngr->free_grains));
#endif

    DNXC_SW_STATE_FUNC_RETURN;
}

shr_error_e
resource_tag_bitmap_fast_is_allocated(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int element,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_is_allocated
                    (unit, node_id, ((resource_tag_bitmap_fast_t) res_tag_bitmap_fast)->resource, element,
                     is_allocated));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_free(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int elem,
    void *extra_argument)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_fast_free_several
                    (unit, node_id, res_tag_bitmap_fast, 1 /* nof_elements */ , elem,
                     (uint8 *) extra_argument));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_free_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    uint32 nof_elements,
    int elem,
    void *extra_argument)
{
    resource_tag_bitmap_fast_t rtb_fast = (resource_tag_bitmap_fast_t) res_tag_bitmap_fast;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_free_several
                    (unit, node_id, rtb_fast->resource, nof_elements, elem, extra_argument));

    SHR_IF_ERR_EXIT(_rtb_fast_free_update_free_fast(unit, node_id, res_tag_bitmap_fast, nof_elements,
                                                    elem, (uint8 *) extra_argument));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_nof_free_elements_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_nof_free_elements_get
                    (unit, node_id, ((resource_tag_bitmap_fast_t) res_tag_bitmap_fast)->resource, nof_free_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_allocated_elements, _SHR_E_PARAM, "nof_elements");

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_nof_allocated_elements_in_range_get
                    (unit, node_id, ((resource_tag_bitmap_fast_t) res_tag_bitmap_fast)->resource, range_start,
                     nof_elements_in_range, nof_allocated_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_data_per_entry_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast,
    int element,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_data_per_entry_get
                    (unit, node_id, ((resource_tag_bitmap_fast_t) res_tag_bitmap_fast)->resource, element, data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_fast_clear(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap_fast)
{
    int nof_grains, tag_level;
    resource_tag_bitmap_fast_t rtb_fast = (resource_tag_bitmap_fast_t) res_tag_bitmap_fast;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_clear(unit, node_id, rtb_fast->resource));

    SHR_IF_ERR_EXIT(sw_state_ll_clear(unit, node_id, rtb_fast->free_grains));

    tag_level = resource_tag_bitmap_utils_get_higher_tag_level(rtb_fast->resource);
    nof_grains = resource_tag_bitmap_utils_get_nof_grains(rtb_fast->resource->count,
                                                          rtb_fast->resource->tag_level_info[tag_level].grain_size,
                                                          rtb_fast->resource->first_element, tag_level,
                                                          rtb_fast->resource->flags);

    SHR_IF_ERR_EXIT(_rtb_fast_initialize_free_fast(unit, node_id, res_tag_bitmap_fast, nof_grains));

exit:
    SHR_FUNC_EXIT;
}

#else
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
resource_tag_bitmap_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif /* BCM_DNX_SUPPORT */
