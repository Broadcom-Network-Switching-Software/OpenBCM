/** \file resource_tag_bitmap.c
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

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/alloc.h>
#include <shared/bitop.h>
#include <soc/error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#ifdef BCM_DNX_SUPPORT

/*
 * Calculate the tag index that created from the tags.
 * It's needed for last_free & next_alloc that in case of multilevel resource to represent 2d array in 1d.
 */
static uint32
_get_calculated_tag_idx(
    resource_tag_bitmap_utils_t resource,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    if (resource_tag_bitmap_utils_is_multilevel_tags_resource(resource))
    {
        return (tags[1] + tags[0] * resource->tag_level_info[1].max_tag_value);
    }
    else
    {
        return tags[0];
    }
}

/*
 * Calculate the optional number of tags that exist according to the max_tags_val that supplied by the user.
 * In case of multilevel, it's includes all the combinations between max_tag_level_0 and max_tag_level_1
 */
static uint32
_get_nof_optional_tags(
    resource_tag_bitmap_utils_t resource)
{
    uint32 nof_tags = 1;

    for (int tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
    {
        nof_tags *= (resource->tag_level_info[tag_level].max_tag_value + 1);
    }
    return nof_tags;
}

/*
 * This function restoring last free and next alloc to default value
 */
static shr_error_e
_rtb_restore_last_free_and_next_alloc_per_tags(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &res_tag_bitmap->lowest_free[tag],
                              &(res_tag_bitmap->resource->count), sizeof(res_tag_bitmap->resource->count),
                              _RTB_NO_FLAGS, "res_tag_bitmap->last_free");

    DNX_SW_STATE_MEMSET(unit, node_id, &res_tag_bitmap->next_alloc[tag], 0, 0, sizeof(int), _RTB_NO_FLAGS,
                        "res_tag_bitmap->next_alloc");

    /** Continue journaling comparison. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function restoring all last frees and nexts alloc in the resource to default value
 */
static shr_error_e
_rtb_restore_last_frees_and_nexts_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap)
{
    SHR_FUNC_INIT_VARS(unit);

    for (int tag = 0; tag < _get_nof_optional_tags(res_tag_bitmap->resource); tag++)
    {
        SHR_IF_ERR_EXIT(_rtb_restore_last_free_and_next_alloc_per_tags(unit, node_id, res_tag_bitmap, tag));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function check whether last free or next alloc can be allocated (id is free and the tag fits)
 */
static shr_error_e
_rtb_check_alloc_heuristic(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int allocated_block_length,
    int element_to_check,
    int *pattern_first_index,
    int *update_element_to_check,
    int *allocation_success)
{
    int tag_compare_result, can_allocate;

    SHR_FUNC_INIT_VARS(unit);

    *pattern_first_index = UTILEX_ALIGN_UP(element_to_check, alloc_info->align) + alloc_info->offs[0];
    if (*pattern_first_index + allocated_block_length < res_tag_bitmap->resource->count)
    {
        /*
         * it might fit
         */
        RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                        resource_tag_bitmap_utils_check_element_for_allocation(unit, node_id,
                                                                                               res_tag_bitmap->resource,
                                                                                               alloc_info,
                                                                                               *pattern_first_index,
                                                                                               allocated_block_length,
                                                                                               &can_allocate,
                                                                                               &tag_compare_result));

        if (can_allocate && !tag_compare_result)
        {
            /*
             * Looks good; mark allocation success, and set element_to_check to
             */
            *allocation_success = TRUE;
            *update_element_to_check = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Check if last reference should be set in case of valid last_references pointer
 */
static shr_error_e
_rtb_is_last_references(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int current_element,
    uint8 *last_references,
    int count)
{
    int grain_start, grain_size, tag_level, element;
    uint8 grain_in_use;

    SHR_FUNC_INIT_VARS(unit);

    if (last_references == NULL)
    {
        SHR_EXIT();
    }
    for (tag_level = 0; tag_level < res_tag_bitmap->resource->nof_tags_levels; tag_level++)
    {
        element = current_element;
        last_references[tag_level] = 0;
        do
        {
            grain_start = resource_tag_bitmap_utils_get_grain_start(res_tag_bitmap->resource, tag_level, element);
            grain_size = resource_tag_bitmap_utils_get_grain_size(res_tag_bitmap->resource, tag_level, element);
            grain_in_use = 0;
            /*
             * In case grain includes current_element not in use last_references = TRUE
             */
            DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->resource->data, grain_start, grain_size,
                                        &grain_in_use);
            if (!grain_in_use)
            {
                last_references[tag_level] = 1;
            }
            /*
             * moving to the first element in the next grain
             */
            element = grain_start + grain_size;
        }
        while (element < count);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_allocate_next_free_element_in_range(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    int range_start,
    int range_end,
    int *elem,
    int *found)
{
    int index, last_legal_index, pattern_first_set_bit, temp_index, current_offset_index;
    int found_match, allocated_block_length, tag_level = 0;
    int tag_compare_result, skip_block = FALSE;
    int grain_size, grain_count;
    uint8 temp;

    SHR_FUNC_INIT_VARS(unit);

    current_offset_index = 0;

    /*
     * Iterate between range_start and range_end, and check for suitable elements in the middle.
     */
    found_match = FALSE;
    index = range_start;
    allocated_block_length = resource_tag_bitmap_utils_get_allocated_block_length(alloc_info);
    last_legal_index = range_end - allocated_block_length;
    pattern_first_set_bit = utilex_lsb_bit_on(alloc_info.pattern);

    while (index <= last_legal_index)
    {
        /*
         * First, skip grains until a grain with an appropriate tags is found.
         */
        do
        {
            for (tag_level = 0; tag_level < res_tag_bitmap->resource->nof_tags_levels; tag_level++)
            {
                skip_block = FALSE;
                grain_size = resource_tag_bitmap_utils_get_grain_size(res_tag_bitmap->resource, tag_level, index);
                SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_check
                                (unit, alloc_info.flags, node_id, res_tag_bitmap->resource, index,
                                 allocated_block_length, alloc_info.tags[tag_level], tag_level, &tag_compare_result));

                if (!tag_compare_result)
                {
                    /*
                     * Tags are matching, but skip grain if it's full.
                     */
                    int first_element_in_grain =
                        resource_tag_bitmap_utils_get_grain_start(res_tag_bitmap->resource, tag_level, index);
                    DNX_SW_STATE_BIT_RANGE_COUNT(unit, node_id, 0, res_tag_bitmap->resource->data,
                                                 first_element_in_grain, grain_size, &grain_count);

                    if (grain_count == grain_size)
                    {
                        /*
                         * Grain is full, skip it.
                         */
                        skip_block = TRUE;
                    }
                }
                else
                {
                    /*
                     * Tag mismatch, skip block.
                     */
                    skip_block = TRUE;
                }

                if (skip_block)
                {
                    /*
                     * At least one element has a different tag.
                     */
                    found_match = FALSE;
                    /*
                     * Skip to beginning of next grain
                     */
                    temp_index =
                        resource_tag_bitmap_utils_get_grain_start(res_tag_bitmap->resource, tag_level,
                                                                  index + grain_size);
                    /*
                     * Realign after this grain.
                     */
                    temp_index = (((temp_index + alloc_info.align - 1) / alloc_info.align) * alloc_info.align)
                        + alloc_info.offs[0];
                    /*
                     * If temp_index is equal to index, will get stuck in infinite loop. Set index to last_legal_index + 1 to prevent this.
                     */
                    if (temp_index != index)
                    {
                        index = temp_index;
                    }
                    else
                    {
                        index = last_legal_index + 1;
                    }
                }
                else
                {
                    /*
                     * Found a matching grain, use it.
                     */
                    break;
                }
            }
        }
        while (skip_block && index <= last_legal_index);

        if (index > last_legal_index)
        {
            /*
             * We didn't find a matching tag in the required range.
             */
            break;
        }

        DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->resource->data, index + pattern_first_set_bit, &temp);

        /*
         * temp indicates whether the first required element in the pattern starting from index is taken. Keep searching until
         *  it's false, or until we finished scanning the range.
         * Remove the offset before the first iteration because we already added it while searching for the tag.
         */
        index -= alloc_info.offs[0];
        while (temp && (index <= last_legal_index))
        {
            for (current_offset_index = 0; current_offset_index < alloc_info.nof_offsets; current_offset_index++)
            {
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->resource->data,
                                     index + pattern_first_set_bit + alloc_info.offs[current_offset_index], &temp);
                if (!temp)
                {
                    break;
                }
            }
            if (temp)
            {
                index += alloc_info.align;
            }
        }

        /*
         * Add the offset that we used in the last iteration to find the correct element.
         */
        index += alloc_info.offs[current_offset_index];

        if (index > last_legal_index)
        {
            /*
             * We didn't find a matching element in range.
             */
            break;
        }

        /*
         * We found a candidate; see if block matches the required pattern.
         * Found a matching block with matching tag. We might have exceeded grain boundary,
         * so check the tag again.
         */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_check_element_for_allocation
                        (unit, node_id, res_tag_bitmap->resource, &alloc_info, index, allocated_block_length,
                         &found_match, &tag_compare_result));

        if (found_match)
        {
            if (tag_compare_result)
            {
                /*
                 * Element tag doesn't match. Continue to next grain.
                 */
                found_match = FALSE;
            }
            else
                /*
                 * Otherwise, we found a match. Break.
                 */
                break;
        }

        if (!found_match)
        {
            /*
             * Either the entries are not free, or the tag doesn't match. Advance the index and continue to loop.
             */
            index -= alloc_info.offs[current_offset_index];
            index += alloc_info.align;
            index += alloc_info.offs[0];
            continue;
        }
    }

    *elem = index;
    *found = found_match;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_create(
    int unit,
    uint32 node_id,
    void *resource_tag_bitmap,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags)
{
    uint32 nof_optional_tags;
    resource_tag_bitmap_utils_create_info_t rtb_create_info;
    resource_tag_bitmap_t *res_tag_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_ALLOC(unit, node_id, *((resource_tag_bitmap_t *) resource_tag_bitmap), **((resource_tag_bitmap_t *) resource_tag_bitmap),      /* nof 
                                                                                                                                                 * elements 
                                                                                                                                                 */ 1, 0,
                       "sw_state res_tag_bitmap");

    res_tag_bitmap = (resource_tag_bitmap_t *) resource_tag_bitmap;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_create
                    (unit, node_id, &((*res_tag_bitmap)->resource), create_info, extra_arguments, alloc_flags,
                     &rtb_create_info));

    /*
     * Allocate the next_alloc and last_free pointers.
     * Allocate one per tag value.
     */
    nof_optional_tags = _get_nof_optional_tags((*res_tag_bitmap)->resource);

    DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->next_alloc, int,
                       nof_optional_tags,
                       _RTB_NO_FLAGS,
                       "(*res_tag_bitmap)->next_alloc");
    DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->lowest_free, int *,
                       nof_optional_tags,
                       0,
                       "(*res_tag_bitmap)->last_free");

    /*
     *  Set the last_free value to count so it won't be used if nothing was actually freed.
     */
    SHR_IF_ERR_EXIT(_rtb_restore_last_frees_and_nexts_alloc(unit, node_id, (*res_tag_bitmap)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_tag_set(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    void *tag_info)
{
    int tag_level;
    resource_tag_bitmap_t rtb = (resource_tag_bitmap_t) res_tag_bitmap;
    resource_tag_bitmap_utils_tag_info_t tag_set_info = *(resource_tag_bitmap_utils_tag_info_t *) tag_info;

    SHR_FUNC_INIT_VARS(unit);

    for (tag_level = 0; tag_level < rtb->resource->nof_tags_levels; tag_level++)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_set(unit,
                                                          node_id,
                                                          rtb->resource,
                                                          tag_set_info.tags[tag_level],
                                                          tag_level,
                                                          tag_set_info.force_tag,
                                                          tag_set_info.element - rtb->resource->first_element,
                                                          tag_set_info.nof_elements, tag_set_info.first_references));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_tag_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    void *tag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get
                    (unit, node_id, ((resource_tag_bitmap_t) res_tag_bitmap)->resource, tag_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_nof_free_elements_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_nof_free_elements_get
                    (unit, node_id, ((resource_tag_bitmap_t) res_tag_bitmap)->resource, nof_free_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_free(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int elem,
    void *extra_argument)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_free_several(unit, node_id, res_tag_bitmap, 1 /* nof_elements */ , elem,
                                                     (uint8 *) extra_argument));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_free_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 nof_elements,
    int elem,
    void *extra_argument)
{
    uint8 empty_grains = FALSE;
    int grain_index, tag_level, tag_idx;
    uint32 current_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    resource_tag_bitmap_t rtb = (resource_tag_bitmap_t) res_tag_bitmap;
    int current_element = elem - rtb->resource->first_element;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_free_several(unit, node_id, rtb->resource, nof_elements,
                                                           elem, extra_argument));
    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get_internal
                    (unit, node_id, rtb->resource, current_element, current_tags));
    tag_idx = _get_calculated_tag_idx(rtb->resource, current_tags);

    SHR_IF_ERR_EXIT(_rtb_is_last_references
                    (unit, node_id, res_tag_bitmap, current_element, (uint8 *) extra_argument, nof_elements));
    /*
     * Update the number of allocated elements in the relevant grain. The operation is set to FALSE because we are freeing elements.
     */
    if (rtb->resource->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
    {
        for (tag_level = 0; tag_level < rtb->resource->nof_tags_levels; tag_level++)
        {
            grain_index = resource_tag_bitmap_utils_get_grain_index(rtb->resource, tag_level, current_element);

            SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_allocated_elements_per_bank_update(unit, node_id, rtb->resource,
                                                                                         nof_elements, current_element,
                                                                                         tag_level, FALSE));
            if ((tag_level == 0 || empty_grains)
                && rtb->resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] == 0)
            {
                empty_grains = TRUE;
            }
            else
            {
                empty_grains = FALSE;
            }
        }
        if (empty_grains)
        {
            SHR_IF_ERR_EXIT(_rtb_restore_last_free_and_next_alloc_per_tags(unit, node_id, res_tag_bitmap, tag_idx));
        }
    }

    /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    /*
     * Update the last free indication with the first bit that was freed in this pattern.
     */
    if ((rtb->lowest_free[tag_idx] >= current_element) && !empty_grains)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &rtb->lowest_free[tag_idx],
                                  &current_element, sizeof(current_element), _RTB_NO_FLAGS,
                                  "res_tag_bitmap->resource->lastFree[current_tag]");
    }

    /*
     * Reset nextAlloc and lastFree if the resource is empty.
     * In case RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN is set,
     * the restoring happen already and should not happen again
     */
    if (rtb->resource->used == 0 && !(rtb->resource->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN))
    {
        SHR_IF_ERR_EXIT(_rtb_restore_last_frees_and_nexts_alloc(unit, node_id, res_tag_bitmap));
    }

    /** Continue journaling comparison. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_print(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_dump_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_print
                    (unit, node_id, (*(resource_tag_bitmap_t *) res_tag_bitmap)->resource, data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_allocate_several(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
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

    rv = resource_tag_bitmap_alloc(unit, node_id, res_tag_bitmap, &res_tag_bitmap_alloc_info, element);

    /*
     * Return indication for the user whether it's the first element in the grain 
     */
    if (extra_arguments != NULL)
    {
        sal_memcpy(alloc_info->first_references, res_tag_bitmap_alloc_info.first_references,
                   sizeof(alloc_info->first_references) *
                   ((resource_tag_bitmap_t) res_tag_bitmap)->resource->nof_tags_levels);
    }
    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(flags, rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_allocate_single(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    SHR_FUNC_INIT_VARS(unit);

    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(flags, resource_tag_bitmap_allocate_several(unit, node_id, res_tag_bitmap, flags, 1 /* nof 
                                                                                                                         * elements 
                                                                                                                         */ ,
                                                                                extra_arguments, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int *elem)
{
    int pattern_first_index, allocated_block_length, next_alloc, tag_level, allocated_bits, tag_idx;
    uint32 tag_for_pointers[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    int allocation_success = FALSE, update_last_free = FALSE, update_next_alloc = FALSE;
    int tmp_update_value = 0, current_offset_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags, resource_tag_bitmap_utils_alloc_pre_process(unit, alloc_info));

    /*
     * After all fields are adjusted, verify the input.
     */
    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                    resource_tag_bitmap_utils_alloc_verify(unit, res_tag_bitmap->resource, *alloc_info,
                                                                           elem));

    /*
     * If we allocate ignoring tags, we need to read the next alloc and last free pointers from index 0,
     * and not from the given tag.
     */
    for (tag_level = 0; tag_level < res_tag_bitmap->resource->nof_tags_levels; tag_level++)
    {
        tag_for_pointers[tag_level] =
            (_SHR_IS_FLAG_SET(alloc_info->flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)) ?
            0 : alloc_info->tags[tag_level];
    }
    tag_idx = _get_calculated_tag_idx(res_tag_bitmap->resource, tag_for_pointers);

    /*
     * Calculate the length of the allocated block.
     * This value will be used to determine if the last element in the block that we're currently inspecting
     * is greater than the end of the bitmap. If it is, then we don't need to check it.
     * If the allocation is not sparse allocation, then the block length will just be the number of allocated elements.
     * If the allocation is sparse, then it will be the length of the pattern times repeats, minus the trailing 0s
     * that may be at the end of the block.
     */
    allocated_block_length = resource_tag_bitmap_utils_get_allocated_block_length(*alloc_info);

    if (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
    {
        RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                        resource_tag_bitmap_utils_check_alloc_with_id(unit, node_id,
                                                                                      res_tag_bitmap->resource,
                                                                                      alloc_info,
                                                                                      allocated_block_length,
                                                                                      &pattern_first_index, elem,
                                                                                      &allocation_success));
    }
    else
    {
        if (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)
        {
            /*
             * Adjust offset to represent alignment against zero, not start of pool.
             */
            for (current_offset_index = 0; current_offset_index < alloc_info->nof_offsets; current_offset_index++)
            {
                alloc_info->offs[current_offset_index] = (alloc_info->offs[current_offset_index] + alloc_info->align
                                                          -
                                                          (res_tag_bitmap->resource->first_element %
                                                           alloc_info->align)) % alloc_info->align;
            }
        }

        if (!(res_tag_bitmap->resource->flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS))
        {
            /*
             * Try to allocate immediately after the last freed element.
             */
            RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                            _rtb_check_alloc_heuristic(unit, node_id, res_tag_bitmap, alloc_info,
                                                                       allocated_block_length,
                                                                       res_tag_bitmap->lowest_free[tag_idx],
                                                                       &pattern_first_index, &update_last_free,
                                                                       &allocation_success));
            /*
             * If Last Free was not a good fit, try next_alloc. Try it if next_alloc is smaller than last_free or last_free is 0(to be sure we will not leave empty blocks)
             */
            if (!allocation_success && res_tag_bitmap->next_alloc[tag_idx] < res_tag_bitmap->lowest_free[tag_idx])
            {
                RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                                _rtb_check_alloc_heuristic(unit, node_id, res_tag_bitmap, alloc_info,
                                                                           allocated_block_length,
                                                                           res_tag_bitmap->next_alloc[tag_idx],
                                                                           &pattern_first_index, &update_next_alloc,
                                                                           &allocation_success));
            }

            /*
             * Allocate an ID in the given range if flag is provided and either allocation was not successful or
             * if previous allocation is out of range.
             */
            if ((alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE)
                && ((allocation_success == FALSE)
                    || ((allocation_success == TRUE)
                        && (pattern_first_index
                            < (alloc_info->range_start - res_tag_bitmap->resource->first_element)
                            || pattern_first_index
                            >= (alloc_info->range_end - res_tag_bitmap->resource->first_element)))))
            {
                update_last_free = FALSE;
                update_next_alloc = FALSE;
                RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                                _rtb_allocate_next_free_element_in_range(unit, node_id, res_tag_bitmap,
                                                                                         *alloc_info,
                                                                                         alloc_info->range_start -
                                                                                         res_tag_bitmap->
                                                                                         resource->first_element,
                                                                                         alloc_info->range_end -
                                                                                         res_tag_bitmap->
                                                                                         resource->first_element,
                                                                                         &pattern_first_index,
                                                                                         &allocation_success));
            }

            if (!allocation_success && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
            {
                /*
                 * We couldn't reuse the last freed element.
                 * Start searching after last successful allocation.
                 */
                next_alloc = UTILEX_MIN(res_tag_bitmap->next_alloc[tag_idx], res_tag_bitmap->lowest_free[tag_idx]);

                pattern_first_index = (((next_alloc + alloc_info->align - 1) / alloc_info->align)
                                       * alloc_info->align) + alloc_info->offs[0];
                RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                                _rtb_allocate_next_free_element_in_range(unit, node_id, res_tag_bitmap,
                                                                                         *alloc_info,
                                                                                         pattern_first_index,
                                                                                         res_tag_bitmap->
                                                                                         resource->count,
                                                                                         &pattern_first_index,
                                                                                         &allocation_success));

                if (!allocation_success)
                {
                    /*
                     * Couldn't place element after the last successfully allocated element.
                     * Try searching from the start of the pool.
                     */
                    pattern_first_index = alloc_info->offs[0];
                    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                                    _rtb_allocate_next_free_element_in_range(unit, node_id,
                                                                                             res_tag_bitmap,
                                                                                             *alloc_info,
                                                                                             pattern_first_index,
                                                                                             next_alloc,
                                                                                             &pattern_first_index,
                                                                                             &allocation_success));
                }

                if (allocation_success)
                {
                    /*
                     * got some space; update next alloc.
                     */
                    update_next_alloc = TRUE;
                }
            }   /* if (_SHR_E_NONE == result) */
        }
        else
        {       /* (res_tag_bitmap->resource->flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS) */

            /*
             * Search from the start of the pool to allocate the element at the lowest available index.
             */
            pattern_first_index = UTILEX_ALIGN_UP(0, alloc_info->align) + alloc_info->offs[0];
            RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                            _rtb_allocate_next_free_element_in_range(unit, node_id, res_tag_bitmap,
                                                                                     *alloc_info, pattern_first_index,
                                                                                     res_tag_bitmap->resource->count,
                                                                                     &pattern_first_index,
                                                                                     &allocation_success));

            /*
             * Allocate an ID in the given range if flag is provided and either allocation was not successful or
             * if previous allocation is out of range.
             */
            if ((alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE)
                && ((allocation_success == FALSE)
                    || ((allocation_success == TRUE)
                        && (pattern_first_index
                            < (alloc_info->range_start - res_tag_bitmap->resource->first_element)
                            || ((pattern_first_index
                                 >= (alloc_info->range_end
                                     - res_tag_bitmap->resource->first_element)) && alloc_info->range_end > 0)))))
            {
                RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                                _rtb_allocate_next_free_element_in_range(unit, node_id, res_tag_bitmap,
                                                                                         *alloc_info,
                                                                                         alloc_info->range_start -
                                                                                         res_tag_bitmap->
                                                                                         resource->first_element,
                                                                                         alloc_info->range_end -
                                                                                         res_tag_bitmap->
                                                                                         resource->first_element,
                                                                                         &pattern_first_index,
                                                                                         &allocation_success));
            }
        }
    }

    if ((allocation_success) && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                        resource_tag_bitmap_utils_update_resource_after_allocation(unit, node_id,
                                                                                                   res_tag_bitmap->resource,
                                                                                                   alloc_info,
                                                                                                   pattern_first_index,
                                                                                                   elem,
                                                                                                   &allocated_bits));

        /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

        if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            if (update_last_free == TRUE)
            {
                tmp_update_value = res_tag_bitmap->resource->count;
                DNX_SW_STATE_MEMCPY_BASIC(unit, node_id,
                                          &res_tag_bitmap->lowest_free[tag_idx],
                                          &tmp_update_value, sizeof(tmp_update_value), _RTB_NO_FLAGS,
                                          "res_tag_bitmap->last_free[alloc_info->tag]");
            }

            if (update_next_alloc == TRUE)
            {
                tmp_update_value = pattern_first_index + allocated_block_length;
                DNX_SW_STATE_MEMCPY_BASIC(unit, node_id,
                                          &res_tag_bitmap->next_alloc[tag_idx],
                                          &tmp_update_value, sizeof(tmp_update_value), _RTB_NO_FLAGS,
                                          "res_tag_bitmap->next_alloc[alloc_info->tag]");
            }
        }
        /** Continue journaling comparison. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

    }
    else if (allocation_success)
    {
        /*
         * if here, allocation can be successful but was run in simulation or silent mode (RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY flag is on)
         * thus, return the first element in the pattern that can be allocated.
         */
        *elem = pattern_first_index + res_tag_bitmap->resource->first_element + utilex_lsb_bit_on(alloc_info->pattern);
    }
    else
    {
        /*
         * allocation was failed (allocation_success = FALSE)
         */
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info->flags,
                                     _SHR_E_RESOURCE,
                                     "No free element matching required conditions. Asked for %d elements aligned to %d, with offs %d, and tags %d, %d\n",
                                     allocated_block_length, alloc_info->align, alloc_info->offs[0],
                                     alloc_info->tags[0], alloc_info->tags[1]);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_clear(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap)
{
    resource_tag_bitmap_t rtb = (resource_tag_bitmap_t) res_tag_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_clear(unit, node_id, rtb->resource));

    SHR_IF_ERR_EXIT(_rtb_restore_last_frees_and_nexts_alloc(unit, node_id, res_tag_bitmap));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_nof_used_elements_in_grain_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 grain_index,
    int tag_level,
    uint32 *nof_allocated_elements_per_grain)
{
    SHR_FUNC_INIT_VARS(unit);

    if (res_tag_bitmap->resource->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
    {
        *nof_allocated_elements_per_grain =
            res_tag_bitmap->resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "resource_tag_bitmap_nof_used_elements_in_grain_get is not available for the bitmap. \n"
                     "This API can be used only if RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag was given during creation of the resource\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_create_info_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_utils_create_info_t * create_info)
{
    int tag_level;
    SHR_FUNC_INIT_VARS(unit);

    create_info->first_element = res_tag_bitmap->resource->first_element;
    create_info->count = res_tag_bitmap->resource->count;
    create_info->flags = res_tag_bitmap->resource->flags;
    for (tag_level = 0; tag_level < res_tag_bitmap->resource->nof_tags_levels; tag_level++)
    {
        create_info->grains_size[tag_level] = res_tag_bitmap->resource->tag_level_info[tag_level].grain_size;
        create_info->max_tags_value[tag_level] = res_tag_bitmap->resource->tag_level_info[tag_level].max_tag_value;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_allocated_elements, _SHR_E_PARAM, "nof_elements");

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_nof_allocated_elements_in_range_get
                    (unit, node_id, ((resource_tag_bitmap_t) res_tag_bitmap)->resource, range_start,
                     nof_elements_in_range, nof_allocated_elements));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_is_allocated(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_is_allocated
                    (unit, node_id, ((resource_tag_bitmap_t) res_tag_bitmap)->resource, element, is_allocated));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_data_per_entry_set(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    const void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_data_per_entry_set
                    (unit, node_id, ((resource_tag_bitmap_t) res_tag_bitmap)->resource, element, data));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_data_per_entry_get(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_data_per_entry_get
                    (unit, node_id, ((resource_tag_bitmap_t) res_tag_bitmap)->resource, element, data));

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
