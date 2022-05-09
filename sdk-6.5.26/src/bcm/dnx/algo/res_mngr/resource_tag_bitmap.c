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

/**
 * \brief
 * Since we use sparse_alloc and sparse_free as internal implementation of all allocation and free
 *  operations, we use this as pattern and set the repeats = count.
 */
#define _RTB_PATTERN_NOT_SPARSE 0x1

/**
 * \brief
 *  If the max_tags_value is over _RTB_CRITICAL_MAX_TAG_VALUE, then an error will be returned.
 */
#define _RTB_CRITICAL_MAX_TAG_VALUE ((1 << 21) - 1)

/*
 * Pass this as force_tag if you don't wish to change the current force setting.
 * TO BE USED BY RESOURCE TAGGED BITMAP ONLY when the resource was created with RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag.
 */
#define _RTB_FORCE_TAG_NO_CHANGE    2

/*
 * If the ALLOC CHECK ONLY / ALLOC SILENTLY flag is passed, use this macros to return error without printing it.
 */
#define _RTB_ERROR_EXIT(flags, error, ...)                              \
    do                                                                                  \
    {                                                                                   \
        if(_SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY) ||             \
                _SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_SILENTLY))            \
        {                                                                               \
            SHR_SET_CURRENT_ERR(error);                                                 \
            SHR_EXIT();                                                                 \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SHR_ERR_EXIT(error, __VA_ARGS__)                                            \
        }                                                                               \
    }                                                                                   \
    while(0)

#define _RTB_IF_ERROR_EXIT(flags, rv)                                   \
    do                                                                                  \
    {                                                                                   \
        if(_SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY) ||             \
        _SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_SILENTLY))                    \
        {                                                                               \
            SHR_SET_CURRENT_ERR(rv);                                                    \
            if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)                                   \
            {                                                                           \
                SHR_EXIT();                                                             \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SHR_IF_ERR_EXIT(rv);                                                        \
        }                                                                               \
    }                                                                                   \
    while(0)

/*
 * This define will be used in the used sw state macros.
 */
#define _RTB_NO_FLAGS  0

/**
 * \brief
 * struct that used for alloc info grain node in the linked list free grains
 */
typedef struct
{
    /*
     * the tags of the grain node in the linked list
     */
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     *the allocated grain node in the linked list
     */
    sw_state_ll_node_t grain_node;
    /*
     * Starting ID of range of the grain
     */
    int grain_start;
    /*
     * ending ID of range of the grain
     * I.e., elements in the range {3,6}, set range_start to 3 and range_end to 7.
     */
    int grain_end;
    /*
     * Boolean indication of whether any of the grains involved were unused before this allocation.
     */
    uint8 first_references[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
} resource_tag_bitmap_grained_alloc_info_t;

static int inline
_rtb_get_allocated_block_length(
    resource_tag_bitmap_alloc_info_t alloc_info)
{
    return (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));
}

static int inline
_rtb_get_nof_free_elements(
    resource_tag_bitmap_t res_tag_bitmap,
    int tag_level,
    int grain_idx)
{
    return res_tag_bitmap->tag_level_info[tag_level].grain_size -
        res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_idx];
}

/*
 * max tag value + 1 represent empty group, relevant in case of resource with grains
 */
static int inline
_rtb_ll_get_empty_group(
    resource_tag_bitmap_t res_tag_bitmap,
    int tag_level)
{
    return (res_tag_bitmap->tag_level_info[tag_level].max_tag_value + 1);
}

/*
 * This function returns the higher tag level
 *  e.g. returns 1 in the following case:
 *  level 0 - |   0   |   1   |
 *  level 1 - | 0 | 1 | 2 | 3 |
 *  bitmap  - |0|1|2|3|4|5|6|7|
 */

static int inline
_rtb_get_higher_tag_level(
    resource_tag_bitmap_t res_tag_bitmap)
{
    return (res_tag_bitmap->nof_tags_levels - 1);
}

static int inline
_rtb_is_sparse(
    int pattern,
    int length)
{
    return !(pattern == _RTB_PATTERN_NOT_SPARSE && length == 1);
}

static uint8 inline
_rtb_is_ll_grained_resource(
    int node_id,                
    resource_tag_bitmap_t res_tag_bitmap)
{
    
    if (DNXC_SW_STATE_LAYOUT_GET_MODULE_ID(node_id) == EXAMPLE_RES_MNGR_MODULE_ID &&
        (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN))
        return TRUE;
    return FALSE;
}

/*
 *The function return lower tags level of the given tags. e.g. {1,3} -> {1,empty}
 *Return value: TRUE if found lower tag level
                FALSE if tags are already in the lower level
 (e.g. when tags = {empty, empty} - a lower level is not exist and return value = FALSE)
 */
static uint8
_rtb_ll_grained_get_lower_tags_level(
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    for (int tag_level = _rtb_get_higher_tag_level(res_tag_bitmap); tag_level >= 0; tag_level--)
    {
        if (tags[tag_level] != _rtb_ll_get_empty_group(res_tag_bitmap, tag_level))
        {
            tags[tag_level] = _rtb_ll_get_empty_group(res_tag_bitmap, tag_level);
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
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    return tags[1] * (res_tag_bitmap->tag_level_info[1].max_tag_value) + tags[0];
}

/*
 *The function returns the head linked list index that represented by tags in lower level
 * e.g. tags = {3,2} -> lower_tags = {3, empty}
 */
static int inline
_rtb_ll_grained_get_lower_ll_head_idx(
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    uint32 lower_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };

    sal_memcpy(lower_tags, tags, sizeof(*tags) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
    _rtb_ll_grained_get_lower_tags_level(res_tag_bitmap, lower_tags);
    return _rtb_get_ll_head_idx_by_tags(res_tag_bitmap, lower_tags);
}

static uint8 inline
_rtb_is_multilevel_tags_resource(
    resource_tag_bitmap_t res_tag_bitmap)
{
    return (res_tag_bitmap->nof_tags_levels > 1);
}

/*
 *The function returns the head node(first node in the list) of the linked list that represented by the given tags 
 */
static shr_error_e
_rtb_get_ll_head_node_by_tags(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS],
    sw_state_ll_node_t * grain_node)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, res_tag_bitmap->free_grains,
                                          _rtb_get_ll_head_idx_by_tags(res_tag_bitmap, tags), grain_node));

exit:
    SHR_FUNC_EXIT;
}

/*
 *The function initialize the free grains linked list.
 *All the grains in the linked list assigned to the the linked list head that is represented by the empty groups tags {max_tag0_val+1,max_tag1_val+1}
 */
static shr_error_e
_rtb_ll_grained_initialize_free_grains_ll(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_grains)
{
    SHR_FUNC_INIT_VARS(unit);

    for (int grain_idx = 0; grain_idx < nof_grains; grain_idx++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, res_tag_bitmap->free_grains,
                                        res_tag_bitmap->empty_groups_ll_head_idx, &grain_idx, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static int inline
_rtb_get_nof_small_grains_in_bigger_grains(
    resource_tag_bitmap_t res_tag_bitmap)
{
    if (_rtb_is_multilevel_tags_resource(res_tag_bitmap))
        return res_tag_bitmap->tag_level_info[0].grain_size / res_tag_bitmap->tag_level_info[1].grain_size;
    return 1;
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

static int
_rtb_get_grain_size(
    resource_tag_bitmap_t res_tag_bitmap,
    int tag_level,
    int element_index)
{
    int grain_size, first_grain_size, last_grain_size;
    grain_size = res_tag_bitmap->tag_level_info[tag_level].grain_size;
    /*
     * First and last grains of trimmed resource tag bitmap is trimmed
     */
    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        first_grain_size = res_tag_bitmap->tag_level_info[tag_level].grain_size - res_tag_bitmap->first_element;
        last_grain_size = (res_tag_bitmap->count - first_grain_size) % grain_size;
        if (last_grain_size == 0)
        {
            last_grain_size = grain_size;
        }
        if (element_index < (grain_size - res_tag_bitmap->first_element))
        {
            return first_grain_size;
        }
        else if (element_index >= (res_tag_bitmap->count - last_grain_size))
        {
            return last_grain_size;
        }
        else
        {
            return grain_size;
        }
    }
    else
    {
        return grain_size;
    }
}

/*
 * return first element of the grain that include the element
 */
static int
_rtb_get_grain_start(
    resource_tag_bitmap_t res_tag_bitmap,
    int tag_level,
    int element_index)
{
    /*
     * First grain of trimmed resource tag bitmap is trimmed, as a result all start of the grains are smaller by the
     * res_tag_bitmap->first_element. The first grain is different and start in 0 as all resource tag bitmaps.
     */
    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        /*
         * first grain starts from 0
         */
        if (element_index < res_tag_bitmap->tag_level_info[tag_level].grain_size - res_tag_bitmap->first_element)
        {
            return 0;
        }
        else
        {
            return (((element_index +
                      res_tag_bitmap->first_element) / res_tag_bitmap->tag_level_info[tag_level].grain_size) *
                    res_tag_bitmap->tag_level_info[tag_level].grain_size - res_tag_bitmap->first_element);
        }
    }
    else
    {
        return ((element_index / res_tag_bitmap->tag_level_info[tag_level].grain_size) *
                res_tag_bitmap->tag_level_info[tag_level].grain_size);
    }
}

/*
 * return grain/tag index of the element in the resource tag bitmap
 */
static int
_rtb_get_grain_index(
    resource_tag_bitmap_t res_tag_bitmap,
    int tag_level,
    int element_index)
{
    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        return ((element_index + res_tag_bitmap->first_element) / res_tag_bitmap->tag_level_info[tag_level].grain_size);
    }
    else
    {
        return (element_index / res_tag_bitmap->tag_level_info[tag_level].grain_size);
    }
}

/*
 * return number of grains in the resource tag bitmap
 */
static int
_rtb_get_nof_grains(
    int count,
    int grain_size,
    int first_element,
    int tag_level,
    int flags)
{

    if (flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        return UTILEX_DIV_ROUND_UP(count - (grain_size - first_element), grain_size) + 1;
    }
    else
    {
        return (count / grain_size);
    }
}

static shr_error_e
_rtb_grained_ll_create_verify(
    int unit,
    uint32 create_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (create_flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        _RTB_ERROR_EXIT(create_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS"
                        " is not supported");
    }

    if (create_flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS)
    {
        _RTB_ERROR_EXIT(create_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS"
                        " is not supported");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify and init res tag bitmap create info.
 */
static shr_error_e
_rtb_verify_and_init_create_info(
    int unit,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    resource_tag_bitmap_create_info_t * rtb_create_info)
{
    resource_tag_bitmap_extra_arguments_create_info_t *extra_create_info;
    int idx, nof_tags_levels, nof_small_grains_in_big_grain, sum_max_tag = 1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(create_info, _SHR_E_PARAM, "create_data");

    sal_memset(rtb_create_info, 0, sizeof(*rtb_create_info));

    if (extra_arguments == NULL)
    {
        rtb_create_info->grains_size[0] = create_info->nof_elements;
        rtb_create_info->max_tags_value[0] = 0;
        rtb_create_info->flags = RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;
        rtb_create_info->nof_tags_levels = 1;
    }
    else
    {
        extra_create_info = (resource_tag_bitmap_extra_arguments_create_info_t *) extra_arguments;
        sal_memcpy(rtb_create_info->grains_size, extra_create_info->grains_size,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
        if (extra_create_info->nof_tags_levels <= 1)
            rtb_create_info->nof_tags_levels = 1;
        else
            rtb_create_info->nof_tags_levels = extra_create_info->nof_tags_levels;
        sal_memcpy(rtb_create_info->max_tags_value, extra_create_info->max_tags_value,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
    }

    nof_tags_levels = rtb_create_info->nof_tags_levels;
    rtb_create_info->flags = create_info->flags;
    rtb_create_info->first_element = create_info->first_element;
    rtb_create_info->count = create_info->nof_elements;
    sal_strncpy(rtb_create_info->name, create_info->name, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    if (0 >= rtb_create_info->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must have a positive number of elements\n");
    }

    if (RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS < nof_tags_levels)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_tags_levels=%d and it's forbidden. Max nof_tags_levels should be less then %d\n",
                     RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS, nof_tags_levels);
    }
    if (nof_tags_levels > 1 &&
        _SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS flag is"
                     "not supperted with multilevel tag resource (nof_tags = %d)\n", nof_tags_levels);
    }
    if (nof_tags_levels > 1)
    {
        if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS flag is"
                         "not supperted with multilevel tag resource (nof_tags = %d)\n", nof_tags_levels);
        }
        if (rtb_create_info->grains_size[1] > rtb_create_info->grains_size[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "In multilevel tag resource, grain size in level 0 should be bigger than grain size in level 1."
                         "Grain size level 1 (%d) is greater than grain size level 0 (%d)." "\n",
                         rtb_create_info->grains_size[1], rtb_create_info->grains_size[0]);
        }
        nof_small_grains_in_big_grain = rtb_create_info->grains_size[0] / rtb_create_info->grains_size[1];
        if (rtb_create_info->grains_size[0] != nof_small_grains_in_big_grain * rtb_create_info->grains_size[1])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "In multilevel tag resource, grain size in level 1 (%d) should be divided in "
                         "grain size in level 0 (%d) without remainder.\n",
                         rtb_create_info->grains_size[1], rtb_create_info->grains_size[0]);
        }
    }

    for (idx = 0; idx < nof_tags_levels; idx++)
    {
        if (0 >= rtb_create_info->grains_size[idx])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Must have at least one element per grain\n");
        }
        /*
         * If bit map is not RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS, count has to be multiplication of grain
         * size
         */
        if (!_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
            && (rtb_create_info->count % rtb_create_info->grains_size[idx]))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "Count %d is not a multiple of grain size %d%s\n",
                                     rtb_create_info->count, rtb_create_info->grains_size[idx], EMPTY);
        }
        if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
            && (rtb_create_info->first_element >= rtb_create_info->grains_size[idx]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS flag is on, "
                         "first element(=%d) have to be lower then grain size(=%d)\n",
                         rtb_create_info->first_element, rtb_create_info->grains_size[idx]);
        }

        if (rtb_create_info->max_tags_value[idx] > _RTB_CRITICAL_MAX_TAG_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " max_tag_value=%d and it's forbidden, max_tag_value should be less than %d\n",
                         rtb_create_info->max_tags_value[idx], _RTB_CRITICAL_MAX_TAG_VALUE);
        }

        if ((rtb_create_info->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN) &&
            (rtb_create_info->max_tags_value[idx] <= 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag is on, "
                         "max tag value have to be bigger than 0\n");
        }
        sum_max_tag *= rtb_create_info->max_tags_value[idx];
    }
    if (_RTB_CRITICAL_MAX_TAG_VALUE < sum_max_tag)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "sum of max_tags_value=%d and it's forbidden, sum of max_tags_value in all the levels"
                     " should be less then %d\n", sum_max_tag, _RTB_CRITICAL_MAX_TAG_VALUE);
    }

    if (0 >= sal_strnlen(rtb_create_info->name, sizeof(rtb_create_info->name)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource manager must have name\n");
    }
    if (DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH < sal_strnlen(rtb_create_info->name, sizeof(rtb_create_info->name)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name %s is too long. Must be at most %d characters",
                     rtb_create_info->name, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function restoring last free and next alloc to default value
 */
static shr_error_e
_rtb_restore_last_free_and_next_alloc_per_tags(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{

    SHR_FUNC_INIT_VARS(unit);

    /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &res_tag_bitmap->last_free[tags[0]][tags[1]],
                              &(res_tag_bitmap->count),
                              sizeof(res_tag_bitmap->count), _RTB_NO_FLAGS, "res_tag_bitmap->last_free");

    DNX_SW_STATE_MEMSET(unit,
                        node_id,
                        &res_tag_bitmap->next_alloc[tags[0]][tags[1]], 0, 0, sizeof(int), _RTB_NO_FLAGS,
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
    uint32 curr_pointer[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    for (curr_pointer[0] = 0; curr_pointer[0] < res_tag_bitmap->tag_level_info[0].max_tag_value + 1; curr_pointer[0]++)
    {
        tags[0] = curr_pointer[0];
        for (curr_pointer[1] = 0; curr_pointer[1] < res_tag_bitmap->tag_level_info[1].max_tag_value + 1;
             curr_pointer[1]++)
        {
            tags[1] = curr_pointer[1];
            SHR_IF_ERR_EXIT(_rtb_restore_last_free_and_next_alloc_per_tags(unit, node_id, res_tag_bitmap, tags));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_create(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags)
{
    int tags_size[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    uint32 nof_grains[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    uint32 nof_next_pointers[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    int use_tag, tag_level, idx;
    resource_tag_bitmap_create_info_t rtb_create_info;
    sw_state_ll_init_info_t free_grains_init_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments
     */
    if (sw_state_is_warm_boot(unit))
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED, "Can't create bitmap during warmboot.\n");
    }

    /*
     * Verify and Init res tag bitmap create info.
     */
    SHR_IF_ERR_EXIT(_rtb_verify_and_init_create_info(unit, create_info, extra_arguments, &rtb_create_info));

    DNX_SW_STATE_ALLOC(unit, node_id, *res_tag_bitmap, **res_tag_bitmap, /* nof elements */ 1, 0,
                       "sw_state res_tag_bitmap");

    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*res_tag_bitmap)->data, rtb_create_info.count, NULL, 0, 0,
                              "(*res_tag_bitmap)->data");

    DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->name, char,
                       1,
                       0,
                       "(*res_tag_bitmap)->name");

    for (tag_level = 0; tag_level < rtb_create_info.nof_tags_levels; tag_level++)
    {
        use_tag = rtb_create_info.max_tags_value[tag_level] > 0;
        if (use_tag)
        {
            nof_grains[tag_level] = _rtb_get_nof_grains(rtb_create_info.count,
                                                        rtb_create_info.grains_size[tag_level],
                                                        rtb_create_info.first_element,
                                                        tag_level, rtb_create_info.flags);
            tags_size[tag_level] = utilex_msb_bit_on(rtb_create_info.max_tags_value[tag_level]) + 1;

            DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*res_tag_bitmap)->tag_level_info[tag_level].tag_data,
                                      (tags_size[tag_level]) * (nof_grains[tag_level]),
                                      NULL, 0, 0, "(*res_tag_bitmap)->tag_data");

            if (rtb_create_info.flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
            {
                DNX_SW_STATE_ALLOC(unit, node_id,
                                   (*res_tag_bitmap)->tag_level_info[tag_level].nof_allocated_elements_per_grain, int,
                                   nof_grains[tag_level],
                                   0,
                                   "(*res_tag_bitmap)->nof_allocated_elements_per_grain");
            }

            if (_SHR_IS_FLAG_SET(rtb_create_info.flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
            {
                DNX_SW_STATE_ALLOC_BITMAP(unit, node_id,
                                          (*res_tag_bitmap)->tag_level_info[tag_level].tag_tracking_bitmap,
                                          rtb_create_info.count, NULL, 0, alloc_flags,
                                          "(*res_tag_bitmap)->tag_tracking_bitmap");
            }

            if (_SHR_IS_FLAG_SET(rtb_create_info.flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
            {
                DNX_SW_STATE_ALLOC_BITMAP(unit, node_id,
                                          (*res_tag_bitmap)->tag_level_info[tag_level].forced_tag_indication,
                                          nof_grains[tag_level], NULL, 0, RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE,
                                          "(*res_tag_bitmap)->forced_tag_indication");
            }
        }

        /*
         * Set the bitmap "static" data.
         */
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*res_tag_bitmap)->tag_level_info[tag_level].grain_size,
                                  &(rtb_create_info.grains_size[tag_level]),
                                  sizeof(rtb_create_info.grains_size[tag_level]), _RTB_NO_FLAGS,
                                  "(*res_tag_bitmap)->grain_size");

        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*res_tag_bitmap)->tag_level_info[tag_level].tag_size,
                                  &tags_size[tag_level],
                                  sizeof(tags_size[tag_level]), _RTB_NO_FLAGS, "(*res_tag_bitmap)->tag_size");

        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*res_tag_bitmap)->tag_level_info[tag_level].max_tag_value,
                                  &(rtb_create_info.max_tags_value[tag_level]),
                                  sizeof(rtb_create_info.max_tags_value[tag_level]), _RTB_NO_FLAGS,
                                  "(*res_tag_bitmap)->max_tag_value");
    }

    /*
     * Set the bitmap "static" data.
     */
    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->name,
                              &rtb_create_info.name,
                              sizeof(rtb_create_info.name), _RTB_NO_FLAGS, "(*res_tag_bitmap)->name");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->count,
                              &(rtb_create_info.count),
                              sizeof(rtb_create_info.count), _RTB_NO_FLAGS, "(*res_tag_bitmap)->count");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->nof_tags_levels,
                              &(rtb_create_info.nof_tags_levels),
                              sizeof(rtb_create_info.nof_tags_levels), _RTB_NO_FLAGS,
                              "(*res_tag_bitmap)->nof_tags_levels");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->first_element,
                              &rtb_create_info.first_element,
                              sizeof(rtb_create_info.first_element), _RTB_NO_FLAGS, "(*res_tag_bitmap)->first element");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->flags,
                              &rtb_create_info.flags,
                              sizeof(rtb_create_info.flags), _RTB_NO_FLAGS, "(*res_tag_bitmap)->flags");

    if (!_rtb_is_ll_grained_resource(node_id, *res_tag_bitmap))
    {
        /*
         * Allocate the next_alloc and last_free pointers.
         * Allocate one per tag value.
         */
        for (tag_level = 0; tag_level < RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS; tag_level++)
        {
            nof_next_pointers[tag_level] = rtb_create_info.max_tags_value[tag_level] + 1;
        }

        DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->next_alloc, int *,
                           nof_next_pointers[0],
                           _RTB_NO_FLAGS,
                           "(*res_tag_bitmap)->next_alloc");
        DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->last_free, int *,
                           nof_next_pointers[0],
                           0,
                           "(*res_tag_bitmap)->last_free");

        for (idx = 0; idx < nof_next_pointers[0]; idx++)
        {
            DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->next_alloc[idx], int,
                               nof_next_pointers[1],
                               _RTB_NO_FLAGS,
                               "(*res_tag_bitmap)->next_alloc");
            DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->last_free[idx], int,
                               nof_next_pointers[1],
                               _RTB_NO_FLAGS,
                               "(*res_tag_bitmap)->last_free");
        }
        /*
         *  Set the last_free value to count so it won't be used if nothing was actually freed.
         */
        SHR_IF_ERR_EXIT(_rtb_restore_last_frees_and_nexts_alloc(unit, node_id, *res_tag_bitmap));

    }
    else
    {
        _RTB_IF_ERROR_EXIT(rtb_create_info.flags, _rtb_grained_ll_create_verify(unit, rtb_create_info.flags));

        free_grains_init_info.max_nof_elements = nof_grains[_rtb_get_higher_tag_level(*res_tag_bitmap)];
        free_grains_init_info.nof_heads =
            _rtb_get_total_nof_ll_heads(rtb_create_info.max_tags_value, (*res_tag_bitmap)->nof_tags_levels);
        free_grains_init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp_uint32";
        SHR_IF_ERR_EXIT(sw_state_ll_create_empty(unit, node_id,
                                                 &free_grains_init_info,
                                                 SW_STATE_LL_SORTED | SW_STATE_LL_MULTIHEAD,
                                                 sizeof(int),
                                                 0,
                                                 &((*res_tag_bitmap)->free_grains),
                                                 nof_grains[_rtb_get_higher_tag_level(*res_tag_bitmap)],
                                                 _RTB_NO_FLAGS));
        (*res_tag_bitmap)->empty_groups_ll_head_idx =
            _rtb_ll_get_empty_group(*res_tag_bitmap,
                                    1) * ((*res_tag_bitmap)->tag_level_info[1].max_tag_value) +
            _rtb_ll_get_empty_group(*res_tag_bitmap, 0);
        SHR_IF_ERR_EXIT(_rtb_ll_grained_initialize_free_grains_ll
                        (unit, node_id, *res_tag_bitmap, nof_grains[_rtb_get_higher_tag_level(*res_tag_bitmap)]));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Compare tags by element.
 */
static shr_error_e
_rtb_compare_tags(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int tag_index,
    uint32 tag,
    int tag_level,
    uint8 *equal)
{
    SHR_BITDCL tmp = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start by assuming that the tags are equal. If one element is different, change it.
     */
    *equal = TRUE;

    /*
     * Compare the tag with the tag_data.
     */
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, res_tag_bitmap->tag_level_info[tag_level].tag_data,
                                (tag_index * (res_tag_bitmap->tag_level_info[tag_level].tag_size)), 0,
                                res_tag_bitmap->tag_level_info[tag_level].tag_size, &tmp);
    if (tag != tmp)
    {
        *equal = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_is_grain_in_use(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 alloc_flags,
    int tag_level,
    int grain_start,
    int grain_size,
    uint8 *grain_in_use)
{
    SHR_BITDCL *bmp;
    SHR_FUNC_INIT_VARS(unit);

    bmp = _SHR_IS_FLAG_SET(res_tag_bitmap->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG) ?
        (res_tag_bitmap->tag_level_info[tag_level].tag_tracking_bitmap) : (res_tag_bitmap->data);

    DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, bmp, grain_start, grain_size, grain_in_use);

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Check that all elements that are required for the current allocation either have the same tag
 *  as the grain they're in, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG is set, or are in a block that has no allocated elements.
 *  If all the elements meet these conditions, then result will be 0. Otherwise, it will be -1.
 */
static shr_error_e
_rtb_tag_check(
    int unit,
    uint32 alloc_flags,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int elem_index,
    int elem_count,
    uint32 tag,
    int tag_level,
    int *result)
{
    int tag_size, grain_size, current_elem, grain_start, tag_index, use_empty_grain, ignore_tag;
    uint8 grain_tag_valid, equal, always_check, cross_region_check, grain_in_use;
    uint32 create_flags;

    SHR_FUNC_INIT_VARS(unit);

    always_check = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG);
    cross_region_check = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK);
    create_flags = res_tag_bitmap->flags;
    ignore_tag = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG);
    use_empty_grain = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN);
    *result = 0;

    grain_size = res_tag_bitmap->tag_level_info[tag_level].grain_size;
    tag_size = res_tag_bitmap->tag_level_info[tag_level].tag_size;

    /*
     * If this bitmap is not using tags or the allocation done with ALLOC_IGNORE_TAG flag nothing to check
     */
    if (ignore_tag || tag_size <= 0)
    {
        SHR_EXIT();
    }

    if (cross_region_check)
    {
        int grain_end = 0;
        int last_element_to_allocate = 0;
        grain_start = _rtb_get_grain_start(res_tag_bitmap, tag_level, elem_index);
        grain_size = _rtb_get_grain_size(res_tag_bitmap, tag_level, elem_index);
        grain_end = grain_start + grain_size - 1;
        last_element_to_allocate = elem_index + elem_count - 1;

        /*
         * Check if the index of the last element to allocate is in the grain range.
         */
        if (last_element_to_allocate < grain_start || last_element_to_allocate > grain_end)
        {
            *result = -1;
            SHR_EXIT();
        }
    }

    /*
     * Iterate over all the tags that are attached to the element block, and verify that they either have
     *   the same tag, or don't have a tag.
     */
    current_elem = elem_index;
    while (current_elem < elem_index + elem_count)
    {
        grain_tag_valid = 0;
        grain_size = _rtb_get_grain_size(res_tag_bitmap, tag_level, current_elem);
        tag_index = _rtb_get_grain_index(res_tag_bitmap, tag_level, current_elem);

        if (!always_check || use_empty_grain)
        {
            /*
             * If we don't force check for the tag, then we check if there are any allocated elements in the
             * current grain. If there are, it means the tag is set for this range.
             * We would also check here for allocated elements if we want to use an empty grain.
             */
            if (_SHR_IS_FLAG_SET(create_flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
            {
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->tag_level_info[tag_level].forced_tag_indication,
                                     tag_index, &grain_tag_valid);
            }

            if (!grain_tag_valid || use_empty_grain)
            {
                grain_start = _rtb_get_grain_start(res_tag_bitmap, tag_level, current_elem);
                SHR_IF_ERR_EXIT(_rtb_is_grain_in_use(unit, node_id, res_tag_bitmap, alloc_flags, tag_level,
                                                     grain_start, grain_size, &grain_in_use));

                if (use_empty_grain && grain_in_use)
                {
                    /*
                     * If empty grain was required and the grain is in use, then we no longer care about the tag's validity,
                     * we can just return mismatch.
                     */
                    *result = -1;
                    SHR_EXIT();
                }

                grain_tag_valid = grain_in_use;
            }
        }

        if (grain_tag_valid || always_check)
        {
            /*
             * the grain is used by at least one other block
             */
            SHR_IF_ERR_EXIT(_rtb_compare_tags(unit, node_id, res_tag_bitmap, tag_index, tag, tag_level, &equal));

            /*
             * If the tags are not equal, mark it, and break.
             */
            if (!equal)
            {
                *result = -1;
                SHR_EXIT();
            }
        }
        current_elem += grain_size;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_tag_set_internal_verify(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int force_tag,
    int force_tag_allowed,
    int tag_level,
    int elem_index,
    int elem_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (tag > res_tag_bitmap->tag_level_info[tag_level].max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag,
                     res_tag_bitmap->tag_level_info[tag_level].max_tag_value);
    }

    if (elem_index + elem_count - 1 >= res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Trying to tag grain of element that its index %d greater than the number of elements in the resource %d",
                     elem_index + elem_count - 1, res_tag_bitmap->count);
    }

    if (force_tag && (force_tag != _RTB_FORCE_TAG_NO_CHANGE) && !force_tag_allowed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "force_tag can't be set if the ALLOW_FORCING_TAG flag wasn't set on init.");
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 *  Set the tag for the given number of elements. The elem_index here is
 *  offset from the start of the bitmap.
 *  first_references represent if the grain was tagged first
 */
static shr_error_e
_rtb_tag_set_internal(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int tag_level,
    int force_tag,
    int elem_index,
    int elem_count,
    uint8 *first_references)
{
    int index, count, offset, force_tag_allowed, grain_start, grain_size;
    uint8 grain_in_use;
    SHR_BITDCL new_tag[1];

    SHR_FUNC_INIT_VARS(unit);

    first_references[tag_level] = 0;
    new_tag[0] = tag;

    force_tag_allowed = _SHR_IS_FLAG_SET(res_tag_bitmap->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG)
        && (force_tag != _RTB_FORCE_TAG_NO_CHANGE);

    SHR_IF_ERR_EXIT(_rtb_tag_set_internal_verify
                    (unit, node_id, res_tag_bitmap, tag, force_tag, force_tag_allowed, tag_level, elem_index,
                     elem_count));
    /*
     * 'tag' is asymmetrical after free, which results in comparison journal issue in 'alloc -> free' scenarios.
     */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    if (res_tag_bitmap->tag_level_info[tag_level].tag_size)
    {
        index = _rtb_get_grain_index(res_tag_bitmap, tag_level, elem_index);
        /*
         * count represent number of grains that should be tagged
         */
        if (RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS && index == 0
            && elem_count > res_tag_bitmap->first_element)
        {
            count = _rtb_get_grain_index(res_tag_bitmap, tag_level, elem_index + elem_count - 1) - index + 1;
        }
        else
        {
            count = (elem_count + res_tag_bitmap->tag_level_info[tag_level].grain_size - 1)
                / res_tag_bitmap->tag_level_info[tag_level].grain_size;
        }

        for (offset = 0; offset < count; offset++)
        {
            grain_size = _rtb_get_grain_size(res_tag_bitmap, tag_level, elem_index);
            /*
             * check if first reference should set
             */
            if (!first_references[tag_level])
            {
                grain_start = _rtb_get_grain_start(res_tag_bitmap, tag_level, elem_index);
                DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->data,
                                            grain_start, grain_size, &grain_in_use);
                /*
                 * if the grain is empty, first reference indication should be returned
                 */
                if (!grain_in_use)
                {
                    first_references[tag_level] = 1;
                }
            }

            DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, res_tag_bitmap->tag_level_info[tag_level].tag_data,
                                         ((index + offset) * res_tag_bitmap->tag_level_info[tag_level].tag_size), 0,
                                         res_tag_bitmap->tag_level_info[tag_level].tag_size, new_tag);

            if (force_tag_allowed)
            {
                if (force_tag)
                {
                    DNX_SW_STATE_BIT_SET(unit, node_id, 0,
                                         res_tag_bitmap->tag_level_info[tag_level].forced_tag_indication,
                                         index + offset);
                }
                else
                {
                    DNX_SW_STATE_BIT_CLEAR(unit, node_id, 0,
                                           res_tag_bitmap->tag_level_info[tag_level].forced_tag_indication,
                                           index + offset);
                }
            }
            elem_index += grain_size;
        }
    }

exit:
    dnx_state_comparison_suppress(unit, FALSE);
    SHR_FUNC_EXIT;
}


static shr_error_e
_rtb_tag_get_internal(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int elem_index,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    SHR_BITDCL tmp;
    int tag_index, tag_level;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments
     */
    if (elem_index < 0 || elem_index > res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not valid ID. Element(%d) must be between 0 and %d.", elem_index,
                     res_tag_bitmap->count);
    }

    for (tag_level = 0; tag_level < RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS; tag_level++)
    {
        tags[tag_level] = 0;
        if (res_tag_bitmap->tag_level_info[tag_level].tag_size)
        {
            tag_index = _rtb_get_grain_index(res_tag_bitmap, tag_level, elem_index);
            tmp = 0;
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, res_tag_bitmap->tag_level_info[tag_level].tag_data,
                                        (tag_index * res_tag_bitmap->tag_level_info[tag_level].tag_size), 0,
                                        res_tag_bitmap->tag_level_info[tag_level].tag_size, &tmp);
            tags[tag_level] = tmp;
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_tag_set(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    void *tag_info)
{
    int tag_level;
    SHR_FUNC_INIT_VARS(unit);

    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        SHR_IF_ERR_EXIT(_rtb_tag_set_internal
                        (unit, node_id, res_tag_bitmap,
                         ((resource_tag_bitmap_tag_info_t *) tag_info)->tags[tag_level],
                         tag_level,
                         ((resource_tag_bitmap_tag_info_t *) tag_info)->force_tag,
                         ((resource_tag_bitmap_tag_info_t *) tag_info)->element - res_tag_bitmap->first_element,
                         ((resource_tag_bitmap_tag_info_t *) tag_info)->nof_elements,
                         ((resource_tag_bitmap_tag_info_t *) tag_info)->first_references));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_tag_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    void *tag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_rtb_tag_get_internal(unit, node_id, res_tag_bitmap,
                                          ((resource_tag_bitmap_tag_info_t *) tag_info)->element -
                                          res_tag_bitmap->first_element,
                                          ((resource_tag_bitmap_tag_info_t *) tag_info)->tags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_nof_free_elements_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments
     */

    SHR_NULL_CHECK(nof_free_elements, _SHR_E_PARAM, "nof_free_elements");

    /*
     * The number of free elements is the number of existing elements (count) -
     *    the number of used elements.
     */
    *nof_free_elements = res_tag_bitmap->count - res_tag_bitmap->used;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Update the number of allocated elements in the relevant grains.
 * The operation should set to FALSE when freeing elements and TRUE while allocating elements.
 */
static shr_error_e
_rtb_allocated_elements_per_bank_update(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int repeats,
    int element,
    int tag_level,
    uint8 operation)
{
    uint32 grain_index = 0, tmp_elements_to_update = 0;
    int grain_size, first_element, tmp_repeats = 0, grain_element_index = 0;
    uint8 is_trimmed_bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);

    grain_index = _rtb_get_grain_index(res_tag_bitmap, tag_level, element);
    grain_size = res_tag_bitmap->tag_level_info[tag_level].grain_size;
    first_element = res_tag_bitmap->first_element;

    tmp_repeats = repeats;
    grain_element_index = element;

    if ((res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS))
    {
        is_trimmed_bitmap = 1;
    }

    while (tmp_repeats > 0)
    {
        int grain_end = (grain_size * (grain_index + 1) - (is_trimmed_bitmap * first_element));
        /*
         * Check if the temporary repeats are from multiple grains.
         */
        if ((tmp_repeats + grain_element_index) > grain_end)
        {
            /*
             * Increase/Decrease the number of allocated elements according to operation.
             * operation is TRUE if we are adding elements and FALSE if we are subtracting.
             */
            if (operation)
            {
                tmp_elements_to_update =
                    res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] +
                    (grain_end - grain_element_index);
            }
            else
            {
                tmp_elements_to_update =
                    res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] -
                    (grain_end - grain_element_index);
            }
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                                      node_id,
                                      &res_tag_bitmap->
                                      tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index],
                                      &tmp_elements_to_update, sizeof(tmp_elements_to_update), _RTB_NO_FLAGS,
                                      "res_tag_bitmap->nof_allocated_elements_per_grain");

            tmp_repeats -= (grain_end - grain_element_index);
        }
        else
        {
            /*
             * Increase/Decrease the number of allocated elements according to operation.
             * operation is TRUE if we are adding elements and FALSE if we are subtracting.
             */
            if (operation)
            {
                tmp_elements_to_update =
                    res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] +
                    tmp_repeats;
            }
            else
            {
                tmp_elements_to_update =
                    res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] -
                    tmp_repeats;
            }
            /*
             * Since the repeats are only in the current grain set tmp_repeats to 0, to break the loop.
             */
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                                      node_id,
                                      &res_tag_bitmap->
                                      tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index],
                                      &tmp_elements_to_update, sizeof(tmp_elements_to_update), _RTB_NO_FLAGS,
                                      "res_tag_bitmap->nof_allocated_elements_per_grain");

            tmp_repeats = 0;
        }
        grain_index++;
        grain_element_index = grain_end;
    }

    SHR_EXIT();

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
    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        element = current_element;
        last_references[tag_level] = 0;
        do
        {
            grain_start = _rtb_get_grain_start(res_tag_bitmap, tag_level, element);
            grain_size = _rtb_get_grain_size(res_tag_bitmap, tag_level, element);
            grain_in_use = 0;
            /*
             * In case grain includes current_element not in use last_references = TRUE
             */
            DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->data, grain_start, grain_size, &grain_in_use);
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
_rtb_free_verify(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int elem)
{
    int elem_idx;
    uint8 temp;
    int last_freeing_element_idx;
    SHR_FUNC_INIT_VARS(unit);

    if (0 >= nof_elements)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must free a positive number of elements. nof_elements was %d", nof_elements);
    }

    last_freeing_element_idx = elem - res_tag_bitmap->first_element + nof_elements;
    if (last_freeing_element_idx > res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given input will exceed the maximum element in the resource. "
                     "Element was %d, nof_elements was %d", elem, nof_elements);
    }

    /*
     * Check if all of the bits in the range that should be unset are not free
     */
    for (elem_idx = elem - res_tag_bitmap->first_element; elem_idx < last_freeing_element_idx; elem_idx++)
    {
        DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data, elem_idx, &temp);
        if (!temp)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Attempting to free an already free resource (id=%d)",
                         elem_idx + res_tag_bitmap->first_element);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int elem,
    void *extra_argument)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_free_several
                    (unit, node_id, res_tag_bitmap, 1 /* nof_elements */ , elem, (uint8 *) extra_argument));

exit:
    SHR_FUNC_EXIT;
}

static void
_rtb_ll_grained_update_allocated_elements_per_grain(
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    int allocated_bits)
{
    
    int grain_idx, tag_level;
    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        grain_idx = _rtb_get_grain_index(res_tag_bitmap, tag_level, element);
        res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_idx] += allocated_bits;
    }
}

static shr_error_e
_rtb_ll_grained_find_grain_by_allocated_element_and_tags(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS],
    int allocated_element,
    sw_state_ll_node_t * grain_node,
    uint8 *grain_found)
{
    int grain_idx = 0, ll_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    grain_idx = _rtb_get_grain_index(res_tag_bitmap, _rtb_get_higher_tag_level(res_tag_bitmap), allocated_element);
    ll_index = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap, tags);
    SHR_IF_ERR_EXIT(sw_state_ll_find(unit, node_id, res_tag_bitmap->free_grains, ll_index, grain_node,
                                     (void *) (&(grain_idx)), grain_found));

exit:
    SHR_FUNC_EXIT;
}

/*This function moving grain in the multi head linked list from ll_head_idx to_ll_head_idx*/
static shr_error_e
_rtb_ll_grained_move_grain(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    sw_state_ll_node_t grain_node,
    int from_ll_head_idx,
    int to_ll_head_idx)
{
    int grain_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_node_key(unit, node_id, res_tag_bitmap->free_grains, grain_node, (void *) &grain_idx));
    SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, res_tag_bitmap->free_grains, from_ll_head_idx, grain_node));
    SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, res_tag_bitmap->free_grains, to_ll_head_idx, &grain_idx, NULL));

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
_rtb_ll_grained_move_neighbors(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int from_ll_head_idx,
    int to_ll_head_idx,
    int range_start)
{
    int grain_idx = 0, range_end = 0;
    uint8 grain_found = FALSE;
    sw_state_ll_node_t grain_node = SW_STATE_LL_INVALID, next_grain_node = SW_STATE_LL_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    range_end = range_start + _rtb_get_nof_small_grains_in_bigger_grains(res_tag_bitmap);

    /*
     * Searching for the first grain in the linked list indicated by from_ll_head_idx
     */
    for (grain_idx = range_start; grain_idx < range_end && !grain_found; grain_idx++)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_find(unit, node_id, res_tag_bitmap->free_grains, from_ll_head_idx, &grain_node,
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
            SHR_IF_ERR_EXIT(sw_state_ll_next_node
                            (unit, node_id, res_tag_bitmap->free_grains, from_ll_head_idx, grain_node,
                             &next_grain_node));
            SHR_IF_ERR_EXIT(sw_state_ll_node_key
                            (unit, node_id, res_tag_bitmap->free_grains, grain_node, (void *) &grain_idx));
            if (grain_idx < range_end)
            {
                SHR_IF_ERR_EXIT(_rtb_ll_grained_move_grain
                                (unit, node_id, res_tag_bitmap, grain_node, from_ll_head_idx, to_ll_head_idx));
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
 * The function update the non full grains in the multihead linked list according to the elements that were free.
 */

static shr_error_e
_rtb_ll_grained_free_update_free_grains_ll(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int nof_elements_to_free,
    int element,
    uint8 *last_reference)
{
    int grains_idx[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    uint32 freeing_grain_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    int grain_size = 0, tag_level = 0, lower_free_ll_head_idx = 0, free_ll_head_idx = 0,
        nof_free_elements = 0, range_start = 0;
    sw_state_ll_node_t grain_node = SW_STATE_LL_INVALID;
    uint8 grain_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    grain_size = res_tag_bitmap->tag_level_info[_rtb_get_higher_tag_level(res_tag_bitmap)].grain_size;
    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        grains_idx[tag_level] = _rtb_get_grain_index(res_tag_bitmap, tag_level, element);
    }
    SHR_IF_ERR_EXIT(_rtb_tag_get_internal(unit, node_id, res_tag_bitmap, element, freeing_grain_tags));
    free_ll_head_idx = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap, freeing_grain_tags);
    nof_free_elements = _rtb_get_nof_free_elements(res_tag_bitmap, _rtb_get_higher_tag_level(res_tag_bitmap),
                                                   grains_idx[_rtb_get_higher_tag_level(res_tag_bitmap)]);
    _rtb_ll_grained_update_allocated_elements_per_grain(res_tag_bitmap, element, -nof_elements_to_free);
    if (nof_free_elements > 0)
    {
        /*
         * Searching only in case of nof_free_elements bigger then zero,
         * otherwise the grain was full before and was not part of the ll->free_grains.
         */
        SHR_IF_ERR_EXIT(_rtb_ll_grained_find_grain_by_allocated_element_and_tags
                        (unit, node_id, res_tag_bitmap, freeing_grain_tags, element, &grain_node, &grain_found));

        if (nof_free_elements + nof_elements_to_free == grain_size)
        {
            if (res_tag_bitmap->tag_level_info[0].nof_allocated_elements_per_grain[grains_idx[0]] == 0)
            {
                SHR_IF_ERR_EXIT(_rtb_ll_grained_move_grain(unit, node_id, res_tag_bitmap, grain_node,
                                                           free_ll_head_idx, res_tag_bitmap->empty_groups_ll_head_idx));
            }
            else
            {   /* valid only to multilevel_tags_resource */
                lower_free_ll_head_idx = _rtb_ll_grained_get_lower_ll_head_idx(res_tag_bitmap, freeing_grain_tags);
                SHR_IF_ERR_EXIT(_rtb_ll_grained_move_grain(unit, node_id, res_tag_bitmap, grain_node,
                                                           free_ll_head_idx, lower_free_ll_head_idx));
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
            SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, res_tag_bitmap->free_grains, free_ll_head_idx,
                                            &grains_idx[_rtb_get_higher_tag_level(res_tag_bitmap)], NULL));
        }
        else if (res_tag_bitmap->tag_level_info[0].nof_allocated_elements_per_grain[grains_idx[0]] == 0)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_add
                            (unit, node_id, res_tag_bitmap->free_grains, res_tag_bitmap->empty_groups_ll_head_idx,
                             &grains_idx[_rtb_get_higher_tag_level(res_tag_bitmap)], NULL));
        }
        else
        {       /* valid only to multilevel_tags_resource ,
                 * res_tag_bitmap->tag_level_info[1].nof_allocated_elements_per_grain[grains_idx[1]] == 0 */
            lower_free_ll_head_idx = _rtb_ll_grained_get_lower_ll_head_idx(res_tag_bitmap, freeing_grain_tags);
            SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, res_tag_bitmap->free_grains, lower_free_ll_head_idx,
                                            &grains_idx[_rtb_get_higher_tag_level(res_tag_bitmap)], NULL));
        }
    }

    /*
     * check if need to update the neighbors grains
     */
    if (_rtb_is_multilevel_tags_resource(res_tag_bitmap) &&
        res_tag_bitmap->tag_level_info[0].nof_allocated_elements_per_grain[grains_idx[0]] == 0)
    {
        /*
         * No more allocated grains in the grain that include the small grains, move all the neighbors grains from
         * {tag, empty} to {empty, empty}
         */
        range_start = _rtb_get_nof_small_grains_in_bigger_grains(res_tag_bitmap) * grains_idx[0];
        lower_free_ll_head_idx = _rtb_ll_grained_get_lower_ll_head_idx(res_tag_bitmap, freeing_grain_tags);
        SHR_IF_ERR_EXIT(_rtb_ll_grained_move_neighbors(unit, node_id, res_tag_bitmap, lower_free_ll_head_idx,
                                                       res_tag_bitmap->empty_groups_ll_head_idx, range_start));
    }

    /*
     * update last reference
     */
    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        last_reference[tag_level] =
            (res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grains_idx[tag_level]] ==
             0) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * The function searching for free grain that fit the grained_alloc_info and update the struct accordingly
 */
static shr_error_e
_rtb_ll_grained_find_free_grain(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_grained_alloc_info_t * grained_alloc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case grain node is not invalid we need to search for the next grain in the list
     */
    if (grained_alloc_info->grain_node != SW_STATE_LL_INVALID)
    {
        SHR_IF_ERR_EXIT(sw_state_ll_next_node
                        (unit, node_id, res_tag_bitmap->free_grains, 0, grained_alloc_info->grain_node,
                         &(grained_alloc_info->grain_node)));
        /*
         * In case grained_alloc_info->grain_node is valid -> a candidate grain was found
         * In case grained_alloc_info->grain_node is invalid -> it's mean there is no grain with free elements in the same list.
         * We will check if in case of lower tag level match, free grain exist.
         */
        if (grained_alloc_info->grain_node != SW_STATE_LL_INVALID ||
            (grained_alloc_info->grain_node == SW_STATE_LL_INVALID &&
             _rtb_ll_grained_get_lower_tags_level(res_tag_bitmap, grained_alloc_info->tags) == FALSE))
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
            _rtb_get_ll_head_node_by_tags(unit, node_id, res_tag_bitmap, grained_alloc_info->tags,
                                          &(grained_alloc_info->grain_node));
        }
        while (grained_alloc_info->grain_node == SW_STATE_LL_INVALID &&
               _rtb_ll_grained_get_lower_tags_level(res_tag_bitmap, grained_alloc_info->tags) == TRUE);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_ll_grained_get_elements_range_in_grain_node(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_grained_alloc_info_t * grained_alloc_info)
{
    int grain_idx = 0, grain_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_node_key
                    (unit, node_id, res_tag_bitmap->free_grains, grained_alloc_info->grain_node,
                     (void *) (&grain_idx)));
    grain_size = res_tag_bitmap->tag_level_info[_rtb_get_higher_tag_level(res_tag_bitmap)].grain_size;
    grained_alloc_info->grain_start = grain_idx * grain_size;
    grained_alloc_info->grain_end = grained_alloc_info->grain_start + grain_size;

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function checks whether all bits in the requested block are free.
 * If a single bit that's supposed to be allocated is not free, can_allocate will be FALSE.
 * Otherwise, it will be TRUE.
 */
static shr_error_e
_rtb_is_block_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int pattern,
    int length,
    int repeats,
    int element,
    int *can_allocate)
{
    int repeat, current_index, bit_offset;
    uint8 temp;
    SHR_FUNC_INIT_VARS(unit);

    *can_allocate = TRUE;

    if (_rtb_is_sparse(pattern, length))
    {
        for (repeat = 0, current_index = element; repeat < repeats; repeat++)
        {
            for (bit_offset = 0; bit_offset < length; bit_offset++, current_index++)
            {
                if (pattern & (1 << bit_offset))
                {
                    DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data, current_index, &temp);

                    if (temp)
                    {
                        /*
                         * One of the required bits is already taken. Can't allocate here.
                         */
                        *can_allocate = FALSE;
                        SHR_EXIT();
                    }   /* if (temp) */
                }       /* if(pattern & (1 << offset)) */
            }   /* for (bit_offset = 0; bit_offset < length; bit_offset++, current++) */
        }       /* for (repeat = 0, current=index; repeat < repeats; repeat++) */
    }
    else
    {
        DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->data, element, repeats, &temp);
        if (temp)
        {
            /*
             * One of the required bits is already taken. Can't allocate here.
             */
            *can_allocate = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_ll_grained_alloc_next_free_element_in_grain(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
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
            SHR_IF_ERR_EXIT(_rtb_is_grain_in_use
                            (unit, node_id, res_tag_bitmap, alloc_info.flags, _rtb_get_higher_tag_level(res_tag_bitmap),
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
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data,
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
            _rtb_is_block_free(unit, node_id, res_tag_bitmap, alloc_info.pattern,
                               alloc_info.length, alloc_info.repeats, *element, allocation_success);
        }
        if (*allocation_success == FALSE)
        {
            index += alloc_info.align;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * This function checks whether requested block and the tag fits
 * If a single bit that's supposed to be allocated is not free, is_free_block will be FALSE.
 * Otherwise, it will be TRUE.
 * tag_result will be 0 if all elements that are required for the current allocation either have the same tag
 * as the grain they're in, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG is set, or are in a block that has no allocated elements.
   Otherwise, it will be -1.
 */
static shr_error_e
_rtb_check_element_for_allocation(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t * alloc_info,
    int element,
    int allocated_block_length,
    int *is_free_block,
    int *tag_result)
{
    int tag_level;
    SHR_FUNC_INIT_VARS(unit);

    *tag_result = 0;

    _RTB_IF_ERROR_EXIT(alloc_info->flags,
                       _rtb_is_block_free(unit, node_id, res_tag_bitmap, alloc_info->pattern,
                                          alloc_info->length, alloc_info->repeats, element, is_free_block));

    if (*is_free_block)
    {
        /*
         * Check if all the tags are matching, in case one of the tags mismatching, no reason to continue the check
         * */
        for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels && !(*tag_result); tag_level++)
        {
            _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_tag_check
                               (unit, alloc_info->flags, node_id, res_tag_bitmap, element, allocated_block_length,
                                alloc_info->tags[tag_level], tag_level, tag_result));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * In case allocation element/s with ID (RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
 * this function verify that tag matches and that all elements in the block are free.
 */
static shr_error_e
_rtb_check_alloc_with_id(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t * alloc_info,
    int allocated_block_length,
    int *pattern_first_index,
    int *elem,
    int *allocation_success)
{
    int tag_index, tag_compare_result, can_allocate, tag_level;
    SHR_BITDCL exist_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Pattern first index is the element from which the pattern starts.
     * In sparse allocation, the element that we return is the first ALLOCATED element, but if the first bit of the
     * pattern is 0 then it won't be the same as the first bit in the pattern.
     */
    *pattern_first_index = *elem - res_tag_bitmap->first_element - utilex_lsb_bit_on(alloc_info->pattern);

    /*
     * check whether the block is free
     */
    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_check_element_for_allocation
                       (unit, node_id, res_tag_bitmap, alloc_info, *pattern_first_index, allocated_block_length,
                        &can_allocate, &tag_compare_result));

    if (!can_allocate)
    {
        /*
         * In use; can't do WITH_ID alloc of this block
         */
        _RTB_ERROR_EXIT(alloc_info->flags, _SHR_E_RESOURCE,
                        "Can't allocate resource WITH_ID. Given element, or some of its block, "
                        "already exists. Asked for %d elements starting from index %d\n",
                        allocated_block_length, *elem);
    }
    if (tag_compare_result)
    {
        /*
         * One of the elements has a mismatching tag.
         */
        for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
        {
            /*
             * get the current tag of the requested element.
             */
            tag_index = _rtb_get_grain_index(res_tag_bitmap, tag_level, *pattern_first_index);
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, res_tag_bitmap->tag_level_info[tag_level].tag_data,
                                        (tag_index * (res_tag_bitmap->tag_level_info[tag_level].tag_size)), 0,
                                        res_tag_bitmap->tag_level_info[tag_level].tag_size, &(exist_tags[tag_level]));
        }
        _RTB_ERROR_EXIT(alloc_info->flags, _SHR_E_PARAM,
                        "Can't allocate resource WITH_ID. The requested ID is in a range that is assigned a different "
                        "property. (represented by tags {%d, %d}). \nAsked for %d elements starting from index %d with tags {%d, %d} .",
                        exist_tags[0], exist_tags[1], allocated_block_length, *elem,
                        alloc_info->tags[0], alloc_info->tags[1]);
    }
    /*
     * If we got here then we successfully allocated.
     * Don't adjust last free or next alloc for WITH_ID.
     */
    *allocation_success = TRUE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_ll_grained_check_alloc_with_id(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
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
    *pattern_first_index = *element - res_tag_bitmap->first_element - utilex_lsb_bit_on(alloc_info.pattern);

    _RTB_IF_ERROR_EXIT(alloc_info.flags,
                       _rtb_check_alloc_with_id(unit, node_id, res_tag_bitmap, &alloc_info,
                                                allocated_block_length, pattern_first_index, element,
                                                allocation_success));

    /*
     * Update grained_alloc_info information for the grain_node_tags and the grain_node
     * */
    if (allocation_success)
    {
        do
        {
            _RTB_IF_ERROR_EXIT(alloc_info.flags,
                               _rtb_ll_grained_find_grain_by_allocated_element_and_tags(unit, node_id, res_tag_bitmap,
                                                                                        grained_alloc_info->tags,
                                                                                        *element,
                                                                                        &
                                                                                        (grained_alloc_info->grain_node),
                                                                                        &grain_found));
        }
        while (grain_found == FALSE && _rtb_ll_grained_get_lower_tags_level(res_tag_bitmap, grained_alloc_info->tags));

        if (grain_found == FALSE)
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_RESOURCE,
                            "Internal error, cannot find the grain in the non full grains linked list\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_grained_ll_alloc_verify(
    int unit,
    int node_id,
    uint32 alloc_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO"
                        " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_ALIGN"
                        " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG"
                        " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG"
                        " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_SPARSE"
                        " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE"
                        " is not supported");
    }

    if (alloc_flags & RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK)
    {
        _RTB_ERROR_EXIT(alloc_flags, _SHR_E_PARAM,
                        "Combination of RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN and RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK"
                        " is not supported");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_ll_grained_check_allocation(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
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
        _rtb_ll_grained_check_alloc_with_id(unit, node_id, res_tag_bitmap, alloc_info, allocated_block_length,
                                            pattern_first_index, grained_alloc_info, element, allocation_success);
    }
    else
    {
        do
        {
            SHR_IF_ERR_EXIT(_rtb_ll_grained_find_free_grain(unit, node_id, res_tag_bitmap, grained_alloc_info));
            if (grained_alloc_info->grain_node != SW_STATE_LL_INVALID)
            {
                SHR_IF_ERR_EXIT(_rtb_ll_grained_get_elements_range_in_grain_node
                                (unit, node_id, res_tag_bitmap, grained_alloc_info));

                SHR_IF_ERR_EXIT(_rtb_ll_grained_alloc_next_free_element_in_grain
                                (unit, node_id, res_tag_bitmap, alloc_info, *grained_alloc_info, allocated_block_length,
                                 pattern_first_index, allocation_success));
            }
        }
        while (grained_alloc_info->grain_node != SW_STATE_LL_INVALID && *allocation_success == FALSE);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_ll_grained_alloc_update_free_grains_ll(
    int unit,
    int node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
    resource_tag_bitmap_grained_alloc_info_t grained_alloc_info,
    int allocated_element,
    int allocated_bits)
{
    int grain_ll_head_idx = 0, alloc_ll_head_idx = 0, lower_alloc_ll_head_idx = 0,
        tag_level = 0, nof_free_elements = 0, range_start = 0, grain_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    grain_idx = _rtb_get_grain_index(res_tag_bitmap, _rtb_get_higher_tag_level(res_tag_bitmap), allocated_element);
    nof_free_elements =
        _rtb_get_nof_free_elements(res_tag_bitmap, _rtb_get_higher_tag_level(res_tag_bitmap), grain_idx);
    grain_ll_head_idx = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap, grained_alloc_info.tags);
    alloc_ll_head_idx = _rtb_get_ll_head_idx_by_tags(res_tag_bitmap, alloc_info.tags);

    if (nof_free_elements == 0)
    {
        /*
         * No more free elements in the grain, the grain will be removed from the free grains linked list
         */
        SHR_IF_ERR_EXIT(sw_state_ll_remove_node
                        (unit, node_id, res_tag_bitmap->free_grains, grain_ll_head_idx, grained_alloc_info.grain_node));
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
            SHR_IF_ERR_EXIT(_rtb_ll_grained_move_grain(unit, node_id, res_tag_bitmap, grained_alloc_info.grain_node,
                                                       grain_ll_head_idx, alloc_ll_head_idx));
        }
        /*
         * The grain is in the {empty, empty} ll or in the {tag, empty}
         */
        if (_rtb_is_multilevel_tags_resource(res_tag_bitmap) &&
            grain_ll_head_idx == res_tag_bitmap->empty_groups_ll_head_idx)
        {
            /*
             * Reorganize multihead linked lists, In case of multilevel linked list and the free grain was taken from
             * the empty group, we should remove from empty group and move to lower level all the neighbors.
             * e.g. in first allocation of resource with nof_tags_level = 2, grain_size = {10,5}, nof_elements=20, tags={2,1}
             * Mulithead ll free grains BEFORE allocation : (empty, empty) -> 0->1->2->3
             *                          AFTER  allocation : (empty, empty) -> 2->3 (2,empty) -> 1 (2,1) -> 0
             */
            range_start =
                _rtb_get_nof_small_grains_in_bigger_grains(res_tag_bitmap) * _rtb_get_grain_index(res_tag_bitmap, 0,
                                                                                                  allocated_element);
            lower_alloc_ll_head_idx = _rtb_ll_grained_get_lower_ll_head_idx(res_tag_bitmap, alloc_info.tags);
            SHR_IF_ERR_EXIT(_rtb_ll_grained_move_neighbors(unit, node_id, res_tag_bitmap, grain_ll_head_idx,
                                                           lower_alloc_ll_head_idx, range_start));
            for (tag_level = 0; tag_level < _rtb_get_higher_tag_level(res_tag_bitmap); tag_level++)
            {
                alloc_info.first_references[tag_level] = 1;
            }
        }
        alloc_info.first_references[_rtb_get_higher_tag_level(res_tag_bitmap)] = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_free_several(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int elem,
    void *extra_argument)
{
    uint8 empty_grains = FALSE;
    int current_element, grain_index, tag_level = 0;
    uint32 current_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_rtb_free_verify(unit, node_id, res_tag_bitmap, nof_elements, elem));

    current_element = elem - res_tag_bitmap->first_element;

    SHR_IF_ERR_EXIT(_rtb_tag_get_internal(unit, node_id, res_tag_bitmap, current_element, current_tags));

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, res_tag_bitmap->data, current_element, nof_elements);

    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
    {
        DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0,
                                     res_tag_bitmap->tag_level_info[tag_level].tag_tracking_bitmap,
                                     current_element, nof_elements);
    }
    DNX_SW_STATE_COUNTER_DEC(unit,
                             node_id,
                             res_tag_bitmap->used,
                             nof_elements, res_tag_bitmap->used, _RTB_NO_FLAGS, "res_tag_bitmap->used");

    if (_rtb_is_ll_grained_resource(node_id, res_tag_bitmap))
    {
        SHR_IF_ERR_EXIT(_rtb_ll_grained_free_update_free_grains_ll(unit, node_id, res_tag_bitmap, nof_elements, elem,
                                                                   (uint8 *) extra_argument));
    }
    else
    {
        SHR_IF_ERR_EXIT(_rtb_is_last_references
                        (unit, node_id, res_tag_bitmap, current_element, (uint8 *) extra_argument, nof_elements));

        /*
         * Update the number of allocated elements in the relevant grain. The operation is set to FALSE because we are freeing elements.
         */
        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
        {
            for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
            {
                grain_index = _rtb_get_grain_index(res_tag_bitmap, tag_level, current_element);

                SHR_IF_ERR_EXIT(_rtb_allocated_elements_per_bank_update
                                (unit, node_id, res_tag_bitmap, nof_elements, current_element, tag_level, FALSE));
                if ((tag_level == 0 || empty_grains)
                    && res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] == 0)
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
                SHR_IF_ERR_EXIT(_rtb_restore_last_free_and_next_alloc_per_tags
                                (unit, node_id, res_tag_bitmap, current_tags));
            }
        }

            /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

        /*
         * Update the last free indication with the first bit that was freed in this pattern.
         */
        if ((res_tag_bitmap->last_free[current_tags[0]][current_tags[1]] >= current_element) && !empty_grains)
        {
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                                      node_id,
                                      &res_tag_bitmap->last_free[current_tags[0]][current_tags[1]],
                                      &current_element,
                                      sizeof(current_element), _RTB_NO_FLAGS, "res_tag_bitmap->lastFree[current_tag]");
        }

        /*
         * Reset nextAlloc and lastFree if the resource is empty.
         * In case RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN is set,
         * the restoring happen already and should not happen again
         */
        if (res_tag_bitmap->used == 0
            && !(res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN))
        {
            SHR_IF_ERR_EXIT(_rtb_restore_last_frees_and_nexts_alloc(unit, node_id, res_tag_bitmap));
        }

        /** Continue journaling comparison. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_alloc_pre_process(
    int unit,
    resource_tag_bitmap_alloc_info_t * alloc_info)
{
    int trailing_spaces, idx;
    uint32 original_pattern;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * The bitmap is always behaving as if all input for sparse allocation/freeing was used.
     * In case it's not sparse allocation/freeing, we'd want to fix the values of the
     * sparse allocation/free fields (pattern, length, repeats) to match regular allocation/free.
     */
    if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        alloc_info->pattern = _RTB_PATTERN_NOT_SPARSE;
        alloc_info->length = 1;
        alloc_info->repeats = alloc_info->count;
    }
    /*
     * In case of sparse allocation/free, we processing the pattern to includes the repeats,
     * length without trailing spaces.
     * e.g. pattern = 1, length = 2, repeat = 2
     * The processed pattern will be 0101 -> 101
     */
    else
    {
        if (0 >= alloc_info->repeats)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern repeats must be at least 1. The given repeats are: %d \n",
                         alloc_info->repeats);
        }
        trailing_spaces = alloc_info->length - (utilex_msb_bit_on(alloc_info->pattern) + 1);
        original_pattern = alloc_info->pattern;
        for (idx = 1; idx < alloc_info->repeats; idx++)
        {
            alloc_info->pattern |= original_pattern << (idx * alloc_info->length);
        }
        alloc_info->length = alloc_info->length * alloc_info->repeats - trailing_spaces;
        alloc_info->repeats = 1;
    }

    /*
     * If the ALLOC_ALIGN or ALLOC_ALIGN_ZERO flag is set and the align field is set
     * to lower than one then ERROR will be return because it would be illegal.
     */
    if (alloc_info->align <= 0
        && ((alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
            || (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The alignment can't be lower than 1 when RESOURCE_TAG_BITMAP_ALLOC_ALIGN/RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO flag is set. The given align is: %d \n",
                     alloc_info->align);
    }
    /*
     * If the ALLOC_ALIGN or ALLOC_ALIGN_ZERO flag is not set but the value of the alignment is bigger than 1 that
     * means the align field was set The alignment can be used when ALLOC_ALIGN or ALLOC_ALIGN_ZERO flag is set 
     */
    else if (alloc_info->align > 1 &&
             (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
              && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The alignment can't be use without setting flag \"RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO\" or \"RESOURCE_TAG_BITMAP_ALLOC_ALIGN\".\n");
    }

    /*
     * If the ALLOC_ALIGN flag wasn't set then the align field wasn't set either.
     * Since we use the align field anyway, set it to 1 since 0 would be illegal.
     */
    else if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
             && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO))
    {
        alloc_info->align = 1;
    }

    /*
     * Minimum nof_offsets is 1 because the offset field is always checked.
     * Even if offset is not in use, the value there should still be 0.
     */
    if (alloc_info->nof_offsets < 1)
    {
        alloc_info->nof_offsets = 1;
        alloc_info->offs[0] = 0;
    }
    else if (alloc_info->nof_offsets > RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The given nof_offsets is: %d but the maximum nof_offset value is: \"%d.\"",
                     alloc_info->nof_offsets, RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *      Given a resource and element, returns the next
 *      allocated element for this resource.
 *      The returned element will be equal to or greater than the given element. It is the user's
 *      responsibility to update it between iterations.
 *      When there are no more allocated elements, DNX_ALGO_RES_ILLEGAL_ELEMENT will be returned.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] node_id - Node id.
 *  \param [in] algo_res - Resource.
 *  \param [in,out] element -
 *       \b As \b input - \n
 *       The element to start searching for the next allocated element from.
 *       \b As \b output - \n
 *       he next allocated element. It will be >= from the element given as input.
 *
 *   \return
 *       \retval Zero if no error was detected
 *       \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *      It is the user's responsibility to update the element between iterations. This usually means
 *       giving it a ++.
 * \see
 *      None
 */
static shr_error_e
_rtb_get_next(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int *element,
    char *name)
{
    int current_element;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR,
                    (BSL_META("\n dnx_algo_res_get_next(unit:%d, name:%s, element:%d)\n"), unit, name, *element));
    }

    SHR_NULL_CHECK(res_tag_bitmap, _SHR_E_PARAM, "Resource doesn't exist.");

    /*
     * Verify element.
     */
    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");

    if (*element < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The given element has negative value: \"%d.\"", *element);
    }

    current_element = UTILEX_MAX(res_tag_bitmap->first_element, *element);

    for (; current_element < res_tag_bitmap->first_element + res_tag_bitmap->count; current_element++)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_is_allocated
                        (unit, node_id, res_tag_bitmap, current_element, &is_allocated));
        if (is_allocated == 1)
        {
            break;
        }
    }

    if (current_element < res_tag_bitmap->first_element + res_tag_bitmap->count)
    {
        *element = current_element;
    }
    else
    {
        *element = DNX_ALGO_RES_ILLEGAL_ELEMENT;
    }

exit:
    if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
    {
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META("\n dnx_algo_res_get_next(name:%s"), name));
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
        {
            LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META(", element:%d"), *element));
        }
        LOG_VERBOSE(BSL_LS_BCMDNX_RESMNGR, (BSL_META(") Result:%d\n"), SHR_GET_CURRENT_ERR()));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_print(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_dump_data_t * data)
{
    int resource_end, entry_id, first_entry, tag_level, grain_size, grain_start, grain_end, nof_tags_levels,
        grain_ind = 0, nof_elements_in_grain = 0;
    resource_tag_bitmap_tag_info_t local_tag_info;
    int nof_grains[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Verify that the resource exist.
     */
    if (*res_tag_bitmap == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource doesn't exist.");
    }

    nof_tags_levels = (*res_tag_bitmap)->nof_tags_levels;
    for (tag_level = 0; tag_level < nof_tags_levels; tag_level++)
    {
        nof_grains[tag_level] = _rtb_get_nof_grains((*res_tag_bitmap)->count,
                                                    (*res_tag_bitmap)->tag_level_info[tag_level].grain_size,
                                                    (*res_tag_bitmap)->first_element, tag_level,
                                                    (*res_tag_bitmap)->flags);
    }

    DNX_SW_STATE_PRINT(unit, "\nResource :%s", data->create_data.name);
    DNX_SW_STATE_PRINT(unit, " (Advanced Algorithm - %s)",
                       (data->create_data.advanced_algorithm ==
                        DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC ? "No" : data->create_data.advanced_algo_name));

    DNX_SW_STATE_PRINT(unit, "\n|");

    DNX_SW_STATE_PRINT(unit, " Number of entries : %d |", data->create_data.nof_elements);
    DNX_SW_STATE_PRINT(unit, " Number of used entries : %d |", data->nof_used_elements);
    DNX_SW_STATE_PRINT(unit, " First entry ID : %d |\n", data->create_data.first_element);

    entry_id = data->create_data.first_element;
    resource_end = data->create_data.nof_elements + data->create_data.first_element;
    /*
     * Print resource tag bitmap without grains and tags
     */
    tag_level--;
    if (nof_grains[tag_level] == 1)
    {
        if (data->nof_used_elements)
        {
            DNX_SW_STATE_PRINT(unit, "\tUsed Entries:");
            while (1)
            {
                SHR_IF_ERR_EXIT(_rtb_get_next(unit, node_id, (*res_tag_bitmap), &entry_id, data->create_data.name));

                if ((entry_id == DNX_ALGO_RES_ILLEGAL_ELEMENT) || (entry_id > resource_end))
                {
                    break;
                }
                DNX_SW_STATE_PRINT(unit, " %d |", entry_id);
                nof_elements_in_grain++;

                if (nof_elements_in_grain % 25 == 0)
                {
                    DNX_SW_STATE_PRINT(unit, "\n\t\t     ");
                }

                entry_id++;
            }
            DNX_SW_STATE_PRINT(unit, "\n");
        }
    }
    /*
     * Print resource tag bitmap with grains and tags and grains info
     */
    else
    {
        DNX_SW_STATE_PRINT(unit, " Resource manager with %d levels:", nof_tags_levels);
        for (tag_level = 0; tag_level < nof_tags_levels; tag_level++)
        {
            DNX_SW_STATE_PRINT(unit, "\n\tLEVEL %d | ", tag_level + 1);
            DNX_SW_STATE_PRINT(unit, "Grain size: %d | ", (*res_tag_bitmap)->tag_level_info[tag_level].grain_size);
            DNX_SW_STATE_PRINT(unit, "Number of grains: %d | ", nof_grains[tag_level]);
            DNX_SW_STATE_PRINT(unit, "Tag size: %d | ", (*res_tag_bitmap)->tag_level_info[tag_level].tag_size);
            DNX_SW_STATE_PRINT(unit, "Max tag value: %d", (*res_tag_bitmap)->tag_level_info[tag_level].max_tag_value);
        }
        tag_level--;

        if (data->nof_used_elements)
        {
            DNX_SW_STATE_PRINT(unit, "\n\tGrain info (dumps only grains with allocated entries)\n");
            while (grain_ind < nof_grains[tag_level] && entry_id < resource_end)
            {
                nof_elements_in_grain = 0;
                grain_ind =
                    _rtb_get_grain_index(*res_tag_bitmap, tag_level, entry_id - data->create_data.first_element);
                grain_start =
                    _rtb_get_grain_start(*res_tag_bitmap, tag_level,
                                         entry_id - data->create_data.first_element) + data->create_data.first_element;
                grain_size =
                    _rtb_get_grain_size(*res_tag_bitmap, tag_level, entry_id - data->create_data.first_element);
                grain_end = grain_start + grain_size;
                first_entry = entry_id;

                local_tag_info.element = grain_start;

                SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, node_id, (*res_tag_bitmap), &local_tag_info));

                /*
                 * calculate number of elements in the grain
                 */
                for (; entry_id < grain_end; entry_id++)
                {
                    SHR_IF_ERR_EXIT(_rtb_get_next(unit, node_id, (*res_tag_bitmap), &entry_id, data->create_data.name));
                    if ((entry_id == DNX_ALGO_RES_ILLEGAL_ELEMENT) || (entry_id >= resource_end))
                    {
                        grain_ind = nof_grains[tag_level];
                        break;
                    }
                    else if (entry_id >= grain_end)
                    {
                        break;
                    }
                    nof_elements_in_grain++;
                }

                /*
                 * print grain info when grain is not empty
                 */
                if (nof_elements_in_grain > 0)
                {
                    DNX_SW_STATE_PRINT(unit, "\t\t     Grain %d | Tags:", grain_ind);
                    for (tag_level = 0; tag_level < nof_tags_levels; tag_level++)
                    {
                        DNX_SW_STATE_PRINT(unit, " level %d - %d ", tag_level, local_tag_info.tags[tag_level]);
                    }
                    DNX_SW_STATE_PRINT(unit, "| Number of elements %d | Used Entries: ", nof_elements_in_grain);
                    tag_level--;

                    /*
                     * print the entries in grain
                     */
                    for (entry_id = first_entry; entry_id < grain_end; entry_id++)
                    {
                        SHR_IF_ERR_EXIT(_rtb_get_next
                                        (unit, node_id, (*res_tag_bitmap), &entry_id, data->create_data.name));
                        /*
                         * In case the entry is not in the current grain, the grain_ind should be updated.
                         */
                        if ((entry_id == DNX_ALGO_RES_ILLEGAL_ELEMENT) || (entry_id >= resource_end))
                        {
                            grain_ind = nof_grains[tag_level];
                            break;
                        }
                        if (entry_id >= grain_end)
                        {
                            grain_ind =
                                _rtb_get_grain_index(*res_tag_bitmap, tag_level,
                                                     entry_id - data->create_data.first_element) - 1;
                            break;
                        }
                        DNX_SW_STATE_PRINT(unit, " %d |", entry_id);
                    }
                    DNX_SW_STATE_PRINT(unit, "\n");

                }

            }
        }
    }
    if (_rtb_is_ll_grained_resource(node_id, *res_tag_bitmap))
    {
        sw_state_ll_print(0, (*res_tag_bitmap)->free_grains);
    }
    DNXC_SW_STATE_FUNC_RETURN;
}

static shr_error_e
_rtb_allocate_next_free_element_in_range(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
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
    allocated_block_length = _rtb_get_allocated_block_length(alloc_info);
    last_legal_index = range_end - allocated_block_length;
    pattern_first_set_bit = utilex_lsb_bit_on(alloc_info.pattern);

    while (index <= last_legal_index)
    {
        /*
         * First, skip grains until a grain with an appropriate tags is found.
         */
        do
        {
            for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
            {
                skip_block = FALSE;
                grain_size = _rtb_get_grain_size(res_tag_bitmap, tag_level, index);
                SHR_IF_ERR_EXIT(_rtb_tag_check
                                (unit, alloc_info.flags, node_id, res_tag_bitmap, index, allocated_block_length,
                                 alloc_info.tags[tag_level], tag_level, &tag_compare_result));

                if (!tag_compare_result)
                {
                    /*
                     * Tags are matching, but skip grain if it's full.
                     */
                    int first_element_in_grain = _rtb_get_grain_start(res_tag_bitmap, tag_level, index);
                    DNX_SW_STATE_BIT_RANGE_COUNT(unit, node_id, 0, res_tag_bitmap->data, first_element_in_grain,
                                                 grain_size, &grain_count);

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
                    temp_index = _rtb_get_grain_start(res_tag_bitmap, tag_level, index + grain_size);
                    /*
                     * Realign after this grain.
                     */
                    temp_index =
                        (((temp_index + alloc_info.align - 1) / alloc_info.align) * alloc_info.align) +
                        alloc_info.offs[0];
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

        DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data, index + pattern_first_set_bit, &temp);

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
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data,
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
        SHR_IF_ERR_EXIT(_rtb_check_element_for_allocation
                        (unit, node_id, res_tag_bitmap, &alloc_info, index, allocated_block_length, &found_match,
                         &tag_compare_result));

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

static shr_error_e
_rtb_alloc_verify(
    int unit,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
    int *elem)
{
    uint32 pattern_mask;
    uint8 wrong_offset;
    int pattern_first_index, allocated_block_length, current_offset_index, tag_level;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If the bitmap is already full, then don't bother allocating, just return error.
     */
    if (res_tag_bitmap->count < res_tag_bitmap->used + alloc_info.count)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_RESOURCE,
                        "Can't allocate element because the resource is already full. \n");
    }

    /*
     * check arguments
     */
    if (0 >= alloc_info.length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern length must be at least 1. Length was %d", alloc_info.length);
    }

    if (BYTES2BITS(sizeof(alloc_info.pattern)) < alloc_info.length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The pattern length=%d must be at most 32. length=length*repeats\n",
                     alloc_info.length);
    }

    if (0 >= alloc_info.repeats)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern repeats must be at least 1. Repeats was %d\n", alloc_info.repeats);
    }

    pattern_mask = (((uint32) 1 << (alloc_info.length - 1)) - 1) | ((uint32) 1 << (alloc_info.length - 1));

    if (0 == (alloc_info.pattern & pattern_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must contain at least one element. Pattern was 0x%x and length was %d",
                     alloc_info.pattern, alloc_info.length);
    }
    if (alloc_info.pattern & ~pattern_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must not contain unused bits. Pattern was 0x%x and length was %d",
                     alloc_info.pattern, alloc_info.length);
    }

    SHR_NULL_CHECK(elem, _SHR_E_PARAM, "elem \n");

    if (BYTES2BITS(sizeof(alloc_info.pattern)) < alloc_info.length)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                        "The pattern length=%d must be at most 32. length=length*repeats\n", alloc_info.length);
    }

    for (int i = 0; i < alloc_info.nof_offsets; i++)
    {
        if (alloc_info.align <= alloc_info.offs[i])
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "The offset[%d] = %d is bigger or equal than the alignment = %d.", i,
                         alloc_info.offs[i], alloc_info.align);
        }
    }

    if (!_SHR_IS_FLAG_SET(res_tag_bitmap->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
        && _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                        "Error: Can't allocate element with \"RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG\" flag when \"RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG\" flag isn't set.\n");
    }

    if (_rtb_is_multilevel_tags_resource(res_tag_bitmap)
        && _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN))
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                        "Error: Can't use RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN in multilevel resource manager.\n");
    }

    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        if (alloc_info.tags[tag_level] > res_tag_bitmap->tag_level_info[tag_level].max_tag_value)
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                            "The tag = %d is greater than max_tags_value[0] = %d. \n", alloc_info.tags[tag_level],
                            res_tag_bitmap->tag_level_info[tag_level].max_tag_value);
        }
    }

    if (_SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
    {
        if (_SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
            && (*elem < alloc_info.range_start || *elem >= alloc_info.range_end))
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                            "Given element WITH_ID that is not in the given range. Element is %d,"
                            "range start is %d and range end is %d.\n", *elem, alloc_info.range_start,
                            alloc_info.range_end);
        }
        if (alloc_info.range_start < res_tag_bitmap->first_element)
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                            "Start ID for allocation in range %d cannot be smaller than the first element of the resource %d.\n",
                            alloc_info.range_start, res_tag_bitmap->first_element);
        }

        if (alloc_info.range_end > (res_tag_bitmap->first_element + res_tag_bitmap->count))
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                            "End ID for allocation in range %d cannot be larger than the first invalid element of the resource %d.\n",
                            alloc_info.range_end, res_tag_bitmap->first_element + res_tag_bitmap->count);
        }
    }

    /*
     * Calculate the length of the allocated block. We can ignore trailing 0s in the pattern.
     */
    allocated_block_length = _rtb_get_allocated_block_length(alloc_info);

    if (alloc_info.count > 1 && _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        _RTB_ERROR_EXIT(alloc_info.count, _SHR_E_PARAM,
                        "The number of elements should be equal to 1 when \"RESOURCE_TAG_BITMAP_ALLOC_SPARSE\". Please use repeats instead.\nThe number of elements is %d\n",
                        alloc_info.count);
    }

    if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
    {
        int element_offset;
        /*
         * WITH_ID, so only try the specifically requested block.
         */
        if (*elem < res_tag_bitmap->first_element)
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                            "Given element %d is too low. Must be at least %d. \n", *elem,
                            res_tag_bitmap->first_element);
        }
        pattern_first_index = *elem - res_tag_bitmap->first_element - utilex_lsb_bit_on(alloc_info.pattern);
        if (pattern_first_index + allocated_block_length > res_tag_bitmap->count)
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                            "Allocating %d elements starting from element %d will exceed the maximum element %d. \n",
                            allocated_block_length, *elem, res_tag_bitmap->first_element + res_tag_bitmap->count - 1);
        }

        if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE)
        {
            int bit_offset = 0;

            if (pattern_first_index < 0)
            {
                _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                "The pattern first index %d is lower than 0. \n", pattern_first_index);
            }

            bit_offset = *elem - pattern_first_index;

            if (!(alloc_info.pattern & (1 << bit_offset)))
            {
                _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                "The pattern first index %d is not aligned with %d. \n",
                                pattern_first_index, alloc_info.align);
            }
        }

        /*
         * Check that the element given conforms with the given align and offset.
         */
        wrong_offset = FALSE;
        if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO)
        {
            /*
             * Alignment is against zero. We should calculate the alignment of the input pattern first index, not the internal one.
             */
            element_offset = (pattern_first_index + res_tag_bitmap->first_element) % alloc_info.align;
        }
        else
        {
            /*
             * alignment is against low
             */
            element_offset = (pattern_first_index) % alloc_info.align;
        }

        if (alloc_info.nof_offsets == 0)
        {
            if (element_offset != 0)
            {
                wrong_offset = TRUE;
            }
        }
        else
        {
            for (current_offset_index = 0; current_offset_index < alloc_info.nof_offsets; current_offset_index++)
            {
                if (element_offset == alloc_info.offs[current_offset_index])
                    break;
            }
            if (current_offset_index == alloc_info.nof_offsets)
            {
                wrong_offset = TRUE;
            }
        }

        if (wrong_offset)
        {
            _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM, "provided first element %d does not conform"
                            " to provided align %d + offset values" " (actual offset = %d)\n", *elem,
                            alloc_info.align, element_offset);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_allocate_several(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element)
{
    int rv;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    resource_tag_bitmap_extra_arguments_alloc_info_t *alloc_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));

    if (extra_arguments != NULL)
    {
        alloc_info = (resource_tag_bitmap_extra_arguments_alloc_info_t *) extra_arguments;
        res_tag_bitmap_alloc_info.align = alloc_info->align;
        sal_memcpy(res_tag_bitmap_alloc_info.offs, alloc_info->offs, sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS);
        res_tag_bitmap_alloc_info.nof_offsets = alloc_info->nof_offsets;
        sal_memcpy(res_tag_bitmap_alloc_info.tags, alloc_info->tags,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
        res_tag_bitmap_alloc_info.pattern = alloc_info->pattern;
        res_tag_bitmap_alloc_info.length = alloc_info->length;
        res_tag_bitmap_alloc_info.repeats = alloc_info->repeats;
        /** Transfer the range_start and range_end parameters if ALLOC_IN_RANGE flag is defined. */
        if (_SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
        {
            res_tag_bitmap_alloc_info.range_start = alloc_info->range_start;
            res_tag_bitmap_alloc_info.range_end = alloc_info->range_end;
        }
    }

    res_tag_bitmap_alloc_info.flags = flags;
    res_tag_bitmap_alloc_info.count = nof_elements;

    rv = resource_tag_bitmap_alloc(unit, node_id, res_tag_bitmap, &res_tag_bitmap_alloc_info, element);
    /*
     * Return indication for the user whether it's the first element in the grain 
     */
    if (extra_arguments != NULL)
    {
        sal_memcpy(alloc_info->first_references, res_tag_bitmap_alloc_info.first_references,
                   sizeof(alloc_info->first_references) * res_tag_bitmap->nof_tags_levels);
    }
    _RTB_IF_ERROR_EXIT(flags, rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_allocate_single(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = resource_tag_bitmap_allocate_several(unit, node_id, res_tag_bitmap, flags, 1 /* nof elements */ ,
                                              extra_arguments, element);

    _RTB_IF_ERROR_EXIT(flags, rv);

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
    resource_tag_bitmap_alloc_info_t * alloc_info,
    int allocated_block_length,
    int element_to_check,
    int *pattern_first_index,
    int *update_element_to_check,
    int *allocation_success)
{
    int tag_compare_result, can_allocate;

    SHR_FUNC_INIT_VARS(unit);

    *pattern_first_index = UTILEX_ALIGN_UP(element_to_check, alloc_info->align) + alloc_info->offs[0];
    if (*pattern_first_index + allocated_block_length < res_tag_bitmap->count)
    {
        /*
         * it might fit
         */
        _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_check_element_for_allocation
                           (unit, node_id, res_tag_bitmap, alloc_info, *pattern_first_index, allocated_block_length,
                            &can_allocate, &tag_compare_result));

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

shr_error_e
resource_tag_bitmap_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t * alloc_info,
    int *elem)
{
    int pattern_first_index, allocated_block_length, next_alloc, tag_level, grain_index;
    int tag_for_pointers[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    int allocation_success = FALSE, update_last_free = FALSE, update_next_alloc = FALSE;
    int tmp_update_value = 0, current_offset_index = 0;
    resource_tag_bitmap_grained_alloc_info_t grained_alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_alloc_pre_process(unit, alloc_info));

    /*
     * After all fields are adjusted, verify the input.
     */
    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_alloc_verify(unit, res_tag_bitmap, *alloc_info, elem));

    /*
     * If we allocate ignoring tags, we need to read the next alloc and last free pointers from index 0,
     * and not from the given tag.
     */
    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        tag_for_pointers[tag_level] =
            (_SHR_IS_FLAG_SET(alloc_info->flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)) ? 0 :
            alloc_info->tags[tag_level];
    }

    /*
     * Calculate the length of the allocated block.
     * This value will be used to determine if the last element in the block that we're currently inspecting
     * is greater than the end of the bitmap. If it is, then we don't need to check it.
     * If the allocation is not sparse allocation, then the block length will just be the number of allocated elements.
     * If the allocation is sparse, then it will be the length of the pattern times repeats, minus the trailing 0s
     * that may be at the end of the block.
     */
    allocated_block_length = _rtb_get_allocated_block_length(*alloc_info);

    if (_rtb_is_ll_grained_resource(node_id, res_tag_bitmap))
    {
        _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_grained_ll_alloc_verify(unit, node_id, alloc_info->flags));
        sal_memset(&grained_alloc_info, 0, sizeof(grained_alloc_info));
        _RTB_IF_ERROR_EXIT(alloc_info->flags,
                           _rtb_ll_grained_check_allocation(unit, node_id, res_tag_bitmap, *alloc_info,
                                                            allocated_block_length, &pattern_first_index,
                                                            &grained_alloc_info, elem, &allocation_success));
    }
    else
    {   /* !(flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID) */
        if (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
        {
            _RTB_IF_ERROR_EXIT(alloc_info->flags,
                               _rtb_check_alloc_with_id(unit, node_id, res_tag_bitmap, alloc_info,
                                                        allocated_block_length, &pattern_first_index, elem,
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
                    alloc_info->offs[current_offset_index] =
                        (alloc_info->offs[current_offset_index] + alloc_info->align -
                         (res_tag_bitmap->first_element % alloc_info->align)) % alloc_info->align;
                }
            }

            if (!(res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS))
            {
                /*
                 * Try to allocate immediately after the last freed element.
                 */
                _RTB_IF_ERROR_EXIT(alloc_info->flags,
                                   _rtb_check_alloc_heuristic(unit, node_id, res_tag_bitmap, alloc_info,
                                                              allocated_block_length,
                                                              res_tag_bitmap->last_free[tag_for_pointers[0]]
                                                              [tag_for_pointers[1]], &pattern_first_index,
                                                              &update_last_free, &allocation_success));
                /*
                 * If Last Free was not a good fit, try next_alloc. Try it if next_alloc is smaller than last_free or last_free is 0(to be sure we will not leave empty blocks)
                 */
                if (!allocation_success &&
                    res_tag_bitmap->next_alloc[tag_for_pointers[0]][tag_for_pointers[1]] <
                    res_tag_bitmap->last_free[tag_for_pointers[0]][tag_for_pointers[1]])
                {
                    _RTB_IF_ERROR_EXIT(alloc_info->flags,
                                       _rtb_check_alloc_heuristic(unit, node_id, res_tag_bitmap, alloc_info,
                                                                  allocated_block_length,
                                                                  res_tag_bitmap->next_alloc[tag_for_pointers[0]]
                                                                  [tag_for_pointers[1]], &pattern_first_index,
                                                                  &update_next_alloc, &allocation_success));
                }

                /*
                 * Allocate an ID in the given range if flag is provided and either allocation was not successful or
                 * if previous allocation is out of range.
                 */
                if ((alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE) &&
                    ((allocation_success == FALSE) ||
                     ((allocation_success == TRUE)
                      && (pattern_first_index < (alloc_info->range_start - res_tag_bitmap->first_element)
                          || pattern_first_index >= (alloc_info->range_end - res_tag_bitmap->first_element)))))
                {
                    update_last_free = FALSE;
                    update_next_alloc = FALSE;
                    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_allocate_next_free_element_in_range
                                       (unit, node_id, res_tag_bitmap, *alloc_info,
                                        alloc_info->range_start - res_tag_bitmap->first_element,
                                        alloc_info->range_end - res_tag_bitmap->first_element, &pattern_first_index,
                                        &allocation_success));
                }

                if (!allocation_success && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
                {
                    /*
                     * We couldn't reuse the last freed element.
                     * Start searching after last successful allocation.
                     */
                    next_alloc =
                        UTILEX_MIN(res_tag_bitmap->next_alloc[tag_for_pointers[0]][tag_for_pointers[1]],
                                   res_tag_bitmap->last_free[tag_for_pointers[0]][tag_for_pointers[1]]);

                    pattern_first_index =
                        (((next_alloc + alloc_info->align - 1) / alloc_info->align) * alloc_info->align) +
                        alloc_info->offs[0];
                    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_allocate_next_free_element_in_range
                                       (unit, node_id, res_tag_bitmap, *alloc_info, pattern_first_index,
                                        res_tag_bitmap->count, &pattern_first_index, &allocation_success));

                    if (!allocation_success)
                    {
                        /*
                         * Couldn't place element after the last successfully allocated element.
                         * Try searching from the start of the pool.
                         */
                        pattern_first_index = alloc_info->offs[0];
                        _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_allocate_next_free_element_in_range
                                           (unit, node_id, res_tag_bitmap, *alloc_info, pattern_first_index, next_alloc,
                                            &pattern_first_index, &allocation_success));
                    }

                    if (allocation_success)
                    {
                        /*
                         * got some space; update next alloc.
                         */
                        update_next_alloc = TRUE;
                    }
                }       /* if (_SHR_E_NONE == result) */
            }
            else
            {   /* (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS) */

                /*
                 * Search from the start of the pool to allocate the element at the lowest available index.
                 */
                pattern_first_index = UTILEX_ALIGN_UP(0, alloc_info->align) + alloc_info->offs[0];
                _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_allocate_next_free_element_in_range
                                   (unit, node_id, res_tag_bitmap, *alloc_info, pattern_first_index,
                                    res_tag_bitmap->count, &pattern_first_index, &allocation_success));

                /*
                 * Allocate an ID in the given range if flag is provided and either allocation was not successful or
                 * if previous allocation is out of range.
                 */
                if ((alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE) &&
                    ((allocation_success == FALSE) ||
                     ((allocation_success == TRUE) &&
                      (pattern_first_index < (alloc_info->range_start - res_tag_bitmap->first_element) ||
                       ((pattern_first_index >= (alloc_info->range_end - res_tag_bitmap->first_element)) &&
                        alloc_info->range_end > 0)))))
                {
                    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_allocate_next_free_element_in_range
                                       (unit, node_id, res_tag_bitmap, *alloc_info,
                                        alloc_info->range_start - res_tag_bitmap->first_element,
                                        alloc_info->range_end - res_tag_bitmap->first_element, &pattern_first_index,
                                        &allocation_success));
                }
            }
        }
    }   /* else -> !(flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID) */
    if ((allocation_success) && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        int repeat = 0, bit_offset = 0, current = 0, allocated_bits = 0, new_used_count = 0;

        /*
         * set the tag for all grains involved
         * don't set tag if IGNORE_TAG flag is set.
         */
        if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
            {
                _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_tag_set_internal
                                   (unit, node_id, res_tag_bitmap, alloc_info->tags[tag_level], tag_level,
                                    _RTB_FORCE_TAG_NO_CHANGE, pattern_first_index, alloc_info->length,
                                    alloc_info->first_references));
            }
        }
        /*
         * mark the block as in-use
         */
        if (_rtb_is_sparse(alloc_info->pattern, alloc_info->length))
        {
            for (repeat = 0, allocated_bits = 0, current = pattern_first_index; repeat < alloc_info->repeats; repeat++)
            {
                for (bit_offset = 0; bit_offset < alloc_info->length; bit_offset++, current++)
                {
                    if (alloc_info->pattern & (1 << bit_offset))
                    {
                        DNX_SW_STATE_BIT_SET(unit, node_id, 0, res_tag_bitmap->data, current);
                        if ((res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                            && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
                        {
                            for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
                            {
                                DNX_SW_STATE_BIT_SET(unit, node_id, 0,
                                                     res_tag_bitmap->tag_level_info[tag_level].tag_tracking_bitmap,
                                                     current);
                            }
                        }
                        allocated_bits++;

                        /*
                         * Increment the number of allocated elements per grain.
                         */
                        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                        {
                            for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
                            {
                                grain_index = _rtb_get_grain_index(res_tag_bitmap, tag_level, current);

                                DNX_SW_STATE_COUNTER_INC(unit,
                                                         node_id,
                                                         res_tag_bitmap->
                                                         tag_level_info[tag_level].nof_allocated_elements_per_grain
                                                         [grain_index], 1,
                                                         res_tag_bitmap->
                                                         tag_level_info[tag_level].nof_allocated_elements_per_grain
                                                         [grain_index], _RTB_NO_FLAGS,
                                                         "res_tag_bitmap->nof_allocated_elements_per_grain[current / res_tag_bitmap->grain_size]");
                            }
                        }
                    }
                }
            }
        }
        else
        {       /* not using sparse */
            DNX_SW_STATE_BIT_RANGE_SET(unit, node_id, 0, res_tag_bitmap->data, pattern_first_index,
                                       alloc_info->repeats);
            if ((res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
            {
                for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
                {
                    DNX_SW_STATE_BIT_RANGE_SET(unit, node_id, 0,
                                               res_tag_bitmap->tag_level_info[tag_level].tag_tracking_bitmap,
                                               pattern_first_index, alloc_info->repeats);
                }
            }
            allocated_bits = alloc_info->repeats;

            /*
             * Update the number of allocated elements in the relevant grain. The operation is set to TRUE because we are allocating elements.
             */
            if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
            {
                for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
                {
                    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_allocated_elements_per_bank_update
                                       (unit, node_id, res_tag_bitmap, alloc_info->repeats, pattern_first_index,
                                        tag_level, TRUE));
                }
            }
        }

        if (_rtb_is_ll_grained_resource(node_id, res_tag_bitmap))
        {
            _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_ll_grained_alloc_update_free_grains_ll
                               (unit, node_id, res_tag_bitmap, *alloc_info, grained_alloc_info, pattern_first_index,
                                allocated_bits));
        }

        /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

        if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            if (update_last_free == TRUE)
            {
                tmp_update_value = res_tag_bitmap->count;
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &res_tag_bitmap->last_free[tag_for_pointers[0]][tag_for_pointers[1]],
                                          &tmp_update_value, sizeof(tmp_update_value),
                                          _RTB_NO_FLAGS, "res_tag_bitmap->last_free[alloc_info->tag]");
            }

            if (update_next_alloc == TRUE)
            {
                tmp_update_value = pattern_first_index + allocated_block_length;
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &res_tag_bitmap->next_alloc[tag_for_pointers[0]][tag_for_pointers[1]],
                                          &tmp_update_value,
                                          sizeof(tmp_update_value),
                                          _RTB_NO_FLAGS, "res_tag_bitmap->next_alloc[alloc_info->tag]");
            }
        }
            /** Continue journaling comparison. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

        /*
         * In case it's the first allocation in the resource tag bitmap, this is the first allocation in the grain
         */
        if (res_tag_bitmap->used == 0)
        {
            alloc_info->first_references[tag_level] = 1;
        }
        new_used_count = res_tag_bitmap->used + allocated_bits;

        DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &res_tag_bitmap->used, &new_used_count, sizeof(new_used_count),
                                  _RTB_NO_FLAGS, "res_tag_bitmap->used");
        /*
         * return the first allocated element in the pattern.
         */
        *elem = pattern_first_index + res_tag_bitmap->first_element + utilex_lsb_bit_on(alloc_info->pattern);
    }
    else if (allocation_success)
    {
        /*
         * if here, allocation can be successful but was run in simulation or silent mode (RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY/SILENTLY flag is on)
         * thus, return the first element in the pattern that can be allocated.
         */
        *elem = pattern_first_index + res_tag_bitmap->first_element + utilex_lsb_bit_on(alloc_info->pattern);
    }
    else
    {
        /*
         * allocation was failed (allocation_success = FALSE)
         */
        _RTB_ERROR_EXIT(alloc_info->flags, _SHR_E_RESOURCE,
                        "No free element matching required conditions. Asked for %d elements aligned to %d, with offs %d, and tags %d, %d\n",
                        allocated_block_length, alloc_info->align, alloc_info->offs[0], alloc_info->tags[0],
                        alloc_info->tags[1]);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_clear(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap)
{
    int level_0_idx = 0, level_1_idx = 0, grain_idx = 0, tag_level = 0, zero = 0;
    int nof_grains[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, res_tag_bitmap->data, 0, res_tag_bitmap->count);

    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &res_tag_bitmap->used, &zero, sizeof(zero), _RTB_NO_FLAGS,
                              "res_tag_bitmap->used");

    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        nof_grains[tag_level] = _rtb_get_nof_grains(res_tag_bitmap->count,
                                                    res_tag_bitmap->tag_level_info[tag_level].grain_size,
                                                    res_tag_bitmap->first_element, tag_level, res_tag_bitmap->flags);
        if (res_tag_bitmap->tag_level_info[tag_level].tag_size > 0)
        {
            DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, res_tag_bitmap->tag_level_info[tag_level].tag_data, 0,
                                         res_tag_bitmap->tag_level_info[tag_level].tag_size * (res_tag_bitmap->count /
                                                                                               res_tag_bitmap->tag_level_info
                                                                                               [tag_level].grain_size));
        }
        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
        {
            for (grain_idx = 0; grain_idx < nof_grains[tag_level]; grain_idx++)
            {
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &res_tag_bitmap->
                                          tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_idx], &zero,
                                          sizeof(zero), _RTB_NO_FLAGS,
                                          "res_tag_bitmap->nof_allocated_elements_per_grain");
            }
        }
    }

    if (_rtb_is_ll_grained_resource(node_id, res_tag_bitmap))
    {
        SHR_IF_ERR_EXIT(sw_state_ll_clear(unit, node_id, res_tag_bitmap->free_grains));
        SHR_IF_ERR_EXIT(_rtb_ll_grained_initialize_free_grains_ll
                        (unit, node_id, res_tag_bitmap, nof_grains[_rtb_get_higher_tag_level(res_tag_bitmap)]));
    }
    else
    {
        for (level_0_idx = 0; level_0_idx < res_tag_bitmap->tag_level_info[0].max_tag_value + 1; level_0_idx++)
        {
            for (level_1_idx = 0; level_1_idx < res_tag_bitmap->tag_level_info[1].max_tag_value + 1; level_1_idx++)
            {
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &res_tag_bitmap->last_free[level_0_idx][level_1_idx],
                                          &res_tag_bitmap->count, sizeof(res_tag_bitmap->count), _RTB_NO_FLAGS,
                                          "res_tag_bitmap->last_free");

                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &res_tag_bitmap->next_alloc[level_0_idx][level_1_idx],
                                          &zero, sizeof(zero), _RTB_NO_FLAGS, "res_tag_bitmap->next_alloc");
            }
        }
    }

    SHR_EXIT();

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

    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
    {
        *nof_allocated_elements_per_grain =
            res_tag_bitmap->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index];
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
    resource_tag_bitmap_create_info_t * create_info)
{
    int tag_level;
    SHR_FUNC_INIT_VARS(unit);

    create_info->first_element = res_tag_bitmap->first_element;
    create_info->count = res_tag_bitmap->count;
    create_info->flags = res_tag_bitmap->flags;
    for (tag_level = 0; tag_level < res_tag_bitmap->nof_tags_levels; tag_level++)
    {
        create_info->grains_size[tag_level] = res_tag_bitmap->tag_level_info[tag_level].grain_size;
        create_info->max_tags_value[tag_level] = res_tag_bitmap->tag_level_info[tag_level].max_tag_value;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_allocated_elements, _SHR_E_PARAM, "nof_elements");

    if (range_start < res_tag_bitmap->first_element)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range start(%d) must be bigger than resource tag bitmap first element(%d).\n",
                     range_start, res_tag_bitmap->first_element);
    }

    if ((range_start + nof_elements_in_range) > (res_tag_bitmap->first_element + res_tag_bitmap->count))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range end(%d) must be smaller than resource tag bitmap last element(%d).\n",
                     (range_start + nof_elements_in_range), (res_tag_bitmap->first_element + res_tag_bitmap->count));
    }

    if (nof_elements_in_range < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range size(%d) must be at least 1.\n", nof_elements_in_range);
    }

    DNX_SW_STATE_BIT_RANGE_COUNT
        (unit, node_id, 0, res_tag_bitmap->data, range_start - res_tag_bitmap->first_element, nof_elements_in_range,
         nof_allocated_elements);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_is_allocated(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data, element - res_tag_bitmap->first_element, is_allocated);

    SHR_EXIT();

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
