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
 *  If the max_tag_value is over _RTB_CRITICAL_MAX_TAG_VALUE, then an error will be returned.
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


static uint32
_rtb_get_calc_tag(
    int nof_tags_levels,
    int first_tag_size,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    if (nof_tags_levels <= 1)
        return tags[0];
    else
        return tags[0] | (tags[1] << first_tag_size);
}


static void
_rtb_update_splitted_tag(
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS],
    int nof_tags_levels,
    int first_tag_size)
{
    if (nof_tags_levels > 1)
    {
        tags[1] = tags[0] >> first_tag_size;
        tags[0] = tags[0] ^ (tags[1] << first_tag_size);
    }
}


static int
_rtb_get_calc_max_tag(
    resource_tag_bitmap_create_info_t tags_info)
{
    if (tags_info.nof_tags_levels <= 1)
        return tags_info.max_tag_value[0];
    else
        return tags_info.max_tag_value[0] +
            (tags_info.max_tag_value[1] << (utilex_log2_round_down(tags_info.max_tag_value[0]) + 1));
}


static int
_rtb_get_max_grain_size(
    resource_tag_bitmap_create_info_t grains_info)
{
    int max_grain = grains_info.grain_size[0];
    if (grains_info.nof_tags_levels > 1 && grains_info.grain_size[1] > grains_info.grain_size[0])
        max_grain = grains_info.grain_size[1];
    return max_grain;
}

/*
 * return grain size of the resource tag bitmap
 */
static int
_rtb_get_grain_size(
    resource_tag_bitmap_t res_tag_bitmap,
    int element_index)
{
    int grain_size, first_grain_size, last_grain_size;
    grain_size = res_tag_bitmap->grainSize;
    /*
     * First and last grains of trimmed resource tag bitmap is trimmed
     */
    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        first_grain_size = res_tag_bitmap->grainSize - res_tag_bitmap->first_element;
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
        if (element_index < res_tag_bitmap->grainSize - res_tag_bitmap->first_element)
        {
            return 0;
        }
        else
        {
            return (((element_index +
                      res_tag_bitmap->first_element) / res_tag_bitmap->grainSize) * res_tag_bitmap->grainSize -
                    res_tag_bitmap->first_element);
        }
    }
    else
    {
        return ((element_index / res_tag_bitmap->grainSize) * res_tag_bitmap->grainSize);
    }
}

/*
 * return grain/tag index of the element in the resource tag bitmap
 */
static int
_rtb_get_grain_index(
    resource_tag_bitmap_t res_tag_bitmap,
    int element_index)
{
    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        return ((element_index + res_tag_bitmap->first_element) / res_tag_bitmap->grainSize);
    }
    else
    {
        return (element_index / res_tag_bitmap->grainSize);
    }
}

/*
 * return number of grains in the resource tag bitmap
 */
int
resource_tag_bitmap_get_nof_grains(
    int count,
    int grain_size,
    int first_element,
    int flags)
{
    if (flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        return (UTILEX_DIV_ROUND_UP(count - (grain_size - first_element), grain_size) + 1);
    }
    else
    {
        return count / grain_size;
    }
}

/*
 * Verify and Init res tag bitmap create info.
 */
static shr_error_e
_rtb_verify_and_init_create_info(
    int unit,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    resource_tag_bitmap_create_info_t * res_tag_bitmap_create_info)
{
    resource_tag_bitmap_extra_arguments_create_info_t *extra_create_info;
    int idx, nof_tags_levels;
    int sum_max_tag = 1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(create_info, _SHR_E_PARAM, "create_data");

    sal_memset(res_tag_bitmap_create_info, 0, sizeof(*res_tag_bitmap_create_info));

    if (extra_arguments == NULL)
    {
        res_tag_bitmap_create_info->grain_size[0] = create_info->nof_elements;
        res_tag_bitmap_create_info->max_tag_value[0] = 0;
        res_tag_bitmap_create_info->flags = RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;
        res_tag_bitmap_create_info->nof_tags_levels = 1;
    }
    else
    {
        extra_create_info = (resource_tag_bitmap_extra_arguments_create_info_t *) extra_arguments;
        sal_memcpy(res_tag_bitmap_create_info->grain_size, extra_create_info->grain_size,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
        if (extra_create_info->nof_tags_levels <= 1)
            res_tag_bitmap_create_info->nof_tags_levels = 1;
        else
            res_tag_bitmap_create_info->nof_tags_levels = extra_create_info->nof_tags_levels;
        sal_memcpy(res_tag_bitmap_create_info->max_tag_value, extra_create_info->max_tag_value,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
    }

    nof_tags_levels = res_tag_bitmap_create_info->nof_tags_levels;
    res_tag_bitmap_create_info->flags = create_info->flags;
    res_tag_bitmap_create_info->first_element = create_info->first_element;
    res_tag_bitmap_create_info->count = create_info->nof_elements;
    sal_strncpy(res_tag_bitmap_create_info->name, create_info->name, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    if (0 >= res_tag_bitmap_create_info->count)
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
        _SHR_IS_FLAG_SET(res_tag_bitmap_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS flag is"
                     "not supperted with multilevel tag resource (nof_tags = %d)\n", nof_tags_levels);
    }

    for (idx = 0; idx < nof_tags_levels; idx++)
    {
        if (0 >= res_tag_bitmap_create_info->grain_size[idx])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Must have at least one element per grain\n");
        }
        /*
         * If bit map is not RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS, count has to be multiplication of grain
         * size
         */
        if (!_SHR_IS_FLAG_SET(res_tag_bitmap_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
            && (res_tag_bitmap_create_info->count % res_tag_bitmap_create_info->grain_size[idx]))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "Count %d is not a multiple of grain size %d%s\n",
                                     res_tag_bitmap_create_info->count, res_tag_bitmap_create_info->grain_size[idx],
                                     EMPTY);
        }
        if (_SHR_IS_FLAG_SET(res_tag_bitmap_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
            && (res_tag_bitmap_create_info->first_element >= res_tag_bitmap_create_info->grain_size[idx]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS flag is on, "
                         "first element(=%d) have to be lower then grain size(=%d)\n",
                         res_tag_bitmap_create_info->first_element, res_tag_bitmap_create_info->grain_size[idx]);
        }

        if (res_tag_bitmap_create_info->max_tag_value[idx] > _RTB_CRITICAL_MAX_TAG_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " max_tag_value=%d and it's forbidden, max_tag_value should be less than %d\n",
                         res_tag_bitmap_create_info->max_tag_value[idx], _RTB_CRITICAL_MAX_TAG_VALUE);
        }
        sum_max_tag *= res_tag_bitmap_create_info->max_tag_value[idx];

        if ((res_tag_bitmap_create_info->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN) &&
            (res_tag_bitmap_create_info->max_tag_value[idx] <= 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag is on, "
                         "max tag value have to be bigger than 0\n");
        }
    }
    if (_RTB_CRITICAL_MAX_TAG_VALUE < sum_max_tag)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "sum of max_tag_value=%d and it's forbidden, sum of max_tag_value in all the levels"
                     " should be less then %d\n", sum_max_tag, _RTB_CRITICAL_MAX_TAG_VALUE);
    }
    
    if (nof_tags_levels > 1)
    {
        if (_RTB_CRITICAL_MAX_TAG_VALUE < _rtb_get_calc_max_tag(*res_tag_bitmap_create_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " max_tag_value=%d and it's forbidden, max_tag_value should be less then %d\n",
                         _rtb_get_calc_max_tag(*res_tag_bitmap_create_info), _RTB_CRITICAL_MAX_TAG_VALUE);
        }
    }
    if (0 >= sal_strnlen(res_tag_bitmap_create_info->name, sizeof(res_tag_bitmap_create_info->name)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource manager must have name\n");
    }
    if (DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH <
        sal_strnlen(res_tag_bitmap_create_info->name, sizeof(res_tag_bitmap_create_info->name)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name %s is too long. Must be at most %d characters",
                     res_tag_bitmap_create_info->name, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH);
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
    uint32 tag_size;
    uint32 nof_grains;
    int use_tag, tag_idx, max_grain_size, first_tag_size;
    uint32 nof_next_pointers, current_next_pointer, max_tag;
    resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;
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
    SHR_IF_ERR_EXIT(_rtb_verify_and_init_create_info(unit, create_info, extra_arguments, &res_tag_bitmap_create_info));

    DNX_SW_STATE_ALLOC(unit, node_id, *res_tag_bitmap, **res_tag_bitmap, /* nof elements */ 1, 0,
                       "sw_state res_tag_bitmap");

    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*res_tag_bitmap)->data, res_tag_bitmap_create_info.count, 0,
                              "(*res_tag_bitmap)->data");

    DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->name, char,
                       1,
                       0,
                       "(*res_tag_bitmap)->name");

    
    max_tag = _rtb_get_calc_max_tag(res_tag_bitmap_create_info);
    first_tag_size = utilex_msb_bit_on(res_tag_bitmap_create_info.max_tag_value[0]) + 1;
    max_grain_size = _rtb_get_max_grain_size(res_tag_bitmap_create_info);
    use_tag = res_tag_bitmap_create_info.max_tag_value[0] > 0;
    nof_grains = 0;
    tag_size = 0;

    if (use_tag)
    {
        nof_grains =
            resource_tag_bitmap_get_nof_grains(res_tag_bitmap_create_info.count, max_grain_size,
                                               res_tag_bitmap_create_info.first_element,
                                               res_tag_bitmap_create_info.flags);

        
        for (tag_idx = 0; tag_idx < res_tag_bitmap_create_info.nof_tags_levels; tag_idx++)
        {
            tag_size += utilex_msb_bit_on(res_tag_bitmap_create_info.max_tag_value[tag_idx]) + 1;
        }

        DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*res_tag_bitmap)->tagData,
                                  (tag_size) * (nof_grains), 0, "(*res_tag_bitmap)->data");

        if (res_tag_bitmap_create_info.flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
        {
            DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->nof_allocated_elements_per_grain, int,
                               nof_grains,
                               0,
                               "(*res_tag_bitmap)->nof_allocated_elements_per_grain");
        }

        if (_SHR_IS_FLAG_SET(res_tag_bitmap_create_info.flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
        {
            DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*res_tag_bitmap)->tag_tracking_bitmap,
                                      res_tag_bitmap_create_info.count, alloc_flags,
                                      "(*res_tag_bitmap)->tag_tracking_bitmap");
        }

        if (_SHR_IS_FLAG_SET(res_tag_bitmap_create_info.flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
        {
            DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*res_tag_bitmap)->forced_tag_indication, nof_grains, 0,
                                      "(*res_tag_bitmap)->forced_tag_indication");
        }
    }

    /*
     * Allocate the nextAlloc and lastFree pointers.
     * Allocate one per tag value.
     */
    nof_next_pointers = max_tag + 1;

    DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->nextAlloc, int,
                       nof_next_pointers,
                       0,
                       "(*res_tag_bitmap)->nextAlloc");
    DNX_SW_STATE_ALLOC(unit, node_id, (*res_tag_bitmap)->lastFree, int,
                       nof_next_pointers,
                       0,
                       "(*res_tag_bitmap)->lastFree");

    /*
     * Set the lastFree value to count so it won't be used if nothing was actually freed.
     */
    for (current_next_pointer = 0; current_next_pointer < nof_next_pointers; current_next_pointer++)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*res_tag_bitmap)->lastFree[current_next_pointer],
                                  &(res_tag_bitmap_create_info.count),
                                  sizeof(res_tag_bitmap_create_info.count),
                                  _RTB_NO_FLAGS, "(*res_tag_bitmap)->lastFree");
    }

    /*
     * Set the bitmap "static" data.
     */
    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->name,
                              &res_tag_bitmap_create_info.name,
                              sizeof(res_tag_bitmap_create_info.name), _RTB_NO_FLAGS, "(*res_tag_bitmap)->name");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->count,
                              &(res_tag_bitmap_create_info.count),
                              sizeof(res_tag_bitmap_create_info.count), _RTB_NO_FLAGS, "(*res_tag_bitmap)->count");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->nof_tags_levels,
                              &(res_tag_bitmap_create_info.nof_tags_levels),
                              sizeof(res_tag_bitmap_create_info.nof_tags_levels), _RTB_NO_FLAGS,
                              "(*res_tag_bitmap)->nof_tags_levels");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->firstTagSize,
                              &(first_tag_size),
                              sizeof(first_tag_size), _RTB_NO_FLAGS, "(*res_tag_bitmap)->nof_tags_levels");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->first_element,
                              &res_tag_bitmap_create_info.first_element,
                              sizeof(res_tag_bitmap_create_info.first_element), _RTB_NO_FLAGS,
                              "(*hash_table)->first element");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->grainSize,
                              &(max_grain_size), sizeof(max_grain_size), _RTB_NO_FLAGS, "(*hash_table)->grainSize");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->tagSize,
                              &(tag_size), sizeof(tag_size), _RTB_NO_FLAGS, "(*hash_table)->tagSize");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->max_tag_value,
                              &(max_tag), sizeof(max_tag), _RTB_NO_FLAGS, "(*hash_table)->max_tag_value");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*res_tag_bitmap)->flags,
                              &res_tag_bitmap_create_info.flags,
                              sizeof(res_tag_bitmap_create_info.flags), _RTB_NO_FLAGS, "(*hash_table)->flags");

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
    uint8 *equal)
{
    SHR_BITDCL tmp;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start by assuming that the tags are equal. If one element is different, change it.
     */
    *equal = TRUE;
    tmp = 0;

    /*
     * Compare the tag with the tagData.
     */
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, res_tag_bitmap->tagData, (tag_index * (res_tag_bitmap->tagSize)), 0,
                                res_tag_bitmap->tagSize, &tmp);
    if (tag != tmp)
    {
        *equal = FALSE;
    }

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
    uint32 *tags,
    int *result)
{
    int tag_size;
    int grain_size, current_elem, grain_start, tag_index, use_empty_grain, ignore_tag;
    uint8 grain_tag_valid, equal, grain_in_use, always_check, cross_region_check;
    uint32 create_flags, tag;

    SHR_FUNC_INIT_VARS(unit);

    always_check = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG);
    cross_region_check = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK);
    create_flags = res_tag_bitmap->flags;
    use_empty_grain = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN);
    ignore_tag = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG);
    *result = 0;
    tag = _rtb_get_calc_tag(res_tag_bitmap->nof_tags_levels, res_tag_bitmap->firstTagSize, tags);
    tag_size = res_tag_bitmap->tagSize;

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
        grain_start = _rtb_get_grain_start(res_tag_bitmap, elem_index);
        grain_size = _rtb_get_grain_size(res_tag_bitmap, elem_index);
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
        grain_size = _rtb_get_grain_size(res_tag_bitmap, current_elem);
        tag_index = _rtb_get_grain_index(res_tag_bitmap, current_elem);

        if (!always_check || use_empty_grain)
        {
            /*
             * If we don't force check for the tag, then we check if there are any allocated elements in the
             * current grain. If there are, it means the tag is set for this range.
             * We would also check here for allocated elements if we want to use an empty grain.
             */
            grain_start = _rtb_get_grain_start(res_tag_bitmap, current_elem);
            if (_SHR_IS_FLAG_SET(create_flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
            {
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->forced_tag_indication, tag_index,
                                     &grain_tag_valid);
            }

            if (!grain_tag_valid || use_empty_grain)
            {

                /*
                 * If tag is not forced, then check if would be valid if it's already allocated.
                 * If we need to use an empty grain, then we check for allocation anyway.
                 */
                if (_SHR_IS_FLAG_SET(create_flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
                {
                    DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->tag_tracking_bitmap,
                                                grain_start, grain_size, &grain_in_use);
                }
                else
                {
                    DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->data, grain_start, grain_size,
                                                &grain_in_use);
                }

                grain_tag_valid = grain_in_use;

                if (use_empty_grain && grain_in_use)
                {
                    /*
                     * If empty grain was required and the grain is in use, then we no longer care about the tag's validity,
                     * we can just return mismatch.
                     */
                    *result = -1;
                    SHR_EXIT();
                }
            }
        }

        if (grain_tag_valid || always_check)
        {
            /*
             * the grain is used by at least one other block
             */
            SHR_IF_ERR_EXIT(_rtb_compare_tags(unit, node_id, res_tag_bitmap, tag_index, tag, &equal));

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
    int elem_index,
    int elem_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (tag > res_tag_bitmap->max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag,
                     res_tag_bitmap->max_tag_value);
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
 *  first_reference represent if the grain was tagged first
 */
static shr_error_e
_rtb_tag_set_internal(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int force_tag,
    int elem_index,
    int elem_count,
    uint8 *first_reference)
{
    int index;
    int count;
    int offset;
    int force_tag_allowed;
    int grain_start, grain_size;
    uint8 grain_in_use;
    SHR_BITDCL new_tag[1];

    SHR_FUNC_INIT_VARS(unit);

    *first_reference = 0;
    new_tag[0] = tag;
    force_tag_allowed = _SHR_IS_FLAG_SET(res_tag_bitmap->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG)
        && (force_tag != _RTB_FORCE_TAG_NO_CHANGE);

    SHR_IF_ERR_EXIT(_rtb_tag_set_internal_verify
                    (unit, node_id, res_tag_bitmap, tag, force_tag, force_tag_allowed, elem_index, elem_count));
    /*
     * 'tag' is asymmetrical after free, which results in comparison journal issue in 'alloc -> free' scenarios.
     */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    if (res_tag_bitmap->tagSize)
    {
        index = _rtb_get_grain_index(res_tag_bitmap, elem_index);
        /*
         * count represent number of grains that should be tagged
         */
        if (RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS && index == 0
            && elem_count > res_tag_bitmap->first_element)
        {
            count = _rtb_get_grain_index(res_tag_bitmap, elem_index + elem_count - 1) - index + 1;
        }
        else
        {
            count = (elem_count + res_tag_bitmap->grainSize - 1) / res_tag_bitmap->grainSize;
        }

        for (offset = 0; offset < count; offset++)
        {
            grain_size = _rtb_get_grain_size(res_tag_bitmap, elem_index);
            /*
             * check if first reference should set
             */
            if (!*first_reference)
            {
                grain_start = _rtb_get_grain_start(res_tag_bitmap, elem_index);
                DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->data,
                                            grain_start, grain_size, &grain_in_use);
                /*
                 * if the grain is empty, first reference indication should be returned
                 */
                if (!grain_in_use)
                {
                    *first_reference = 1;
                }
            }

            DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, res_tag_bitmap->tagData,
                                         ((index + offset) * res_tag_bitmap->tagSize), 0, res_tag_bitmap->tagSize,
                                         new_tag);

            if (force_tag_allowed)
            {
                if (force_tag)
                {
                    DNX_SW_STATE_BIT_SET(unit, node_id, 0, res_tag_bitmap->forced_tag_indication, index + offset);
                }
                else
                {
                    DNX_SW_STATE_BIT_CLEAR(unit, node_id, 0, res_tag_bitmap->forced_tag_indication, index + offset);
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
    uint32 *tags)
{
    SHR_BITDCL tmp;
    SHR_FUNC_INIT_VARS(unit);

    tmp = 0;
    tags[0] = 0;

    /*
     * check arguments
     */
    if (elem_index < 0 || elem_index > res_tag_bitmap->count)
    {
        /*
         * not valid ID
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Element(%d) must be between 0 and %d.", elem_index, res_tag_bitmap->count);
    }

    if (res_tag_bitmap->tagSize)
    {
        int tag_index = _rtb_get_grain_index(res_tag_bitmap, elem_index);
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, res_tag_bitmap->tagData,
                                    (tag_index * res_tag_bitmap->tagSize), 0, res_tag_bitmap->tagSize, &tmp);
        tags[0] = tmp;
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
    uint32 tag;
    SHR_FUNC_INIT_VARS(unit);

    tag = _rtb_get_calc_tag(res_tag_bitmap->nof_tags_levels,
                            res_tag_bitmap->firstTagSize, ((resource_tag_bitmap_tag_info_t *) tag_info)->tags);

    SHR_IF_ERR_EXIT(_rtb_tag_set_internal
                    (unit, node_id, res_tag_bitmap, tag,
                     ((resource_tag_bitmap_tag_info_t *) tag_info)->force_tag,
                     ((resource_tag_bitmap_tag_info_t *) tag_info)->element - res_tag_bitmap->first_element,
                     ((resource_tag_bitmap_tag_info_t *) tag_info)->nof_elements,
                     &((resource_tag_bitmap_tag_info_t *) tag_info)->first_reference));
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
                                          (((resource_tag_bitmap_tag_info_t *) tag_info)->tags)));
    _rtb_update_splitted_tag(((resource_tag_bitmap_tag_info_t *) tag_info)->tags, res_tag_bitmap->nof_tags_levels,
                             res_tag_bitmap->firstTagSize);

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
    uint8 operation)
{
    uint32 grain_index = 0, tmp_elements_to_update = 0;
    int grain_size, first_element, tmp_repeats = 0, grain_element_index = 0;
    uint8 is_trimmed_bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);

    grain_index = _rtb_get_grain_index(res_tag_bitmap, element);
    grain_size = res_tag_bitmap->grainSize;
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
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] +
                    (grain_end - grain_element_index);
            }
            else
            {
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] -
                    (grain_end - grain_element_index);
            }
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                                      node_id,
                                      &res_tag_bitmap->nof_allocated_elements_per_grain[grain_index],
                                      &tmp_elements_to_update,
                                      sizeof(tmp_elements_to_update),
                                      _RTB_NO_FLAGS, "hash_table->nof_allocated_elements_per_grain");

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
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] + tmp_repeats;
            }
            else
            {
                tmp_elements_to_update = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] - tmp_repeats;
            }
            /*
             * Since the repeats are only in the current grain set tmp_repeats to 0, to break the loop.
             */
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                                      node_id,
                                      &res_tag_bitmap->nof_allocated_elements_per_grain[grain_index],
                                      &tmp_elements_to_update,
                                      sizeof(tmp_elements_to_update),
                                      _RTB_NO_FLAGS, "hash_table->nof_allocated_elements_per_grain");

            tmp_repeats = 0;
        }
        grain_index++;
        grain_element_index = grain_end;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_restore_last_free_and_next_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &res_tag_bitmap->lastFree[tag],
                              &(res_tag_bitmap->count),
                              sizeof(res_tag_bitmap->count), _RTB_NO_FLAGS, "res_tag_bitmap->lastFree");

    DNX_SW_STATE_MEMSET(unit,
                        node_id,
                        &res_tag_bitmap->nextAlloc[tag], 0, 0, sizeof(int), _RTB_NO_FLAGS, "res_tag_bitmap->nextAlloc");

    /** Continue journaling comparison. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

    SHR_EXIT();
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
                    (unit, node_id, res_tag_bitmap, 1 /* nof_elements */ , elem, extra_argument));

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
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(res_tag_bitmap_free_info));
    res_tag_bitmap_free_info.count = nof_elements;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free(unit, node_id, res_tag_bitmap, &res_tag_bitmap_free_info, elem));
    if (extra_argument != NULL)
    {
        *(uint8 *) extra_argument = res_tag_bitmap_free_info.last_reference;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * check if last reference should be set
 */
static shr_error_e
_rtb_is_last_reference(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int current_element,
    uint8 *last_reference,
    int count)
{
    int grain_start, grain_size;
    uint8 grain_in_use;

    SHR_FUNC_INIT_VARS(unit);

    *last_reference = 0;

    do
    {
        grain_start = _rtb_get_grain_start(res_tag_bitmap, current_element);
        grain_size = _rtb_get_grain_size(res_tag_bitmap, current_element);
        grain_in_use = 0;
        /*
         * In case grain includes current_element not in use last_reference = TRUE
         */
        DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, res_tag_bitmap->data, grain_start, grain_size, &grain_in_use);
        if (!grain_in_use)
        {
            *last_reference = 1;
        }
        /*
         * moving to the first element in the next grain
         */
        current_element = grain_start + grain_size;
    }
    while (current_element < count);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e static
_rtb_alloc_free_verify(
    int unit,
    resource_tag_bitmap_alloc_info_t alloc_free_info)
{
    uint32 pattern_mask;
    SHR_FUNC_INIT_VARS(unit);

    if (0 >= alloc_free_info.length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Must free/alloc a positive number of elements. Length was %d",
                     alloc_free_info.length);
    }

    if (BYTES2BITS(sizeof(alloc_free_info.pattern)) < alloc_free_info.length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The pattern length=%d must be at most 32. length=length*repeats\n",
                     alloc_free_info.length);
    }

    if (0 >= alloc_free_info.repeats)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern repeats must be at least 1. Repeats was %d\n", alloc_free_info.repeats);
    }

    pattern_mask = (((uint32) 1 << (alloc_free_info.length - 1)) - 1) | ((uint32) 1 << (alloc_free_info.length - 1));

    if (0 == (alloc_free_info.pattern & pattern_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must contain at least one element. Pattern was 0x%x and length was %d",
                     alloc_free_info.pattern, alloc_free_info.length);
    }
    if (alloc_free_info.pattern & ~pattern_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Pattern must not contain unused bits. Pattern was 0x%x and length was %d",
                     alloc_free_info.pattern, alloc_free_info.length);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_advanced_free_verify(
    int unit,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t free_info,
    int elem)
{
    int final_bit_in_pattern, first_index_in_pattern;
    SHR_FUNC_INIT_VARS(unit);

    _RTB_IF_ERROR_EXIT(free_info.flags, _rtb_alloc_free_verify(unit, free_info));

    if (elem < res_tag_bitmap->first_element)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Starting element is too small: %d. Must be at least %d.", elem,
                     res_tag_bitmap->first_element);
    }

    final_bit_in_pattern = (utilex_msb_bit_on(free_info.pattern) + 1) + (free_info.length * (free_info.repeats - 1));
    first_index_in_pattern = elem - res_tag_bitmap->first_element - utilex_lsb_bit_on(free_info.pattern);

    if (first_index_in_pattern + final_bit_in_pattern > res_tag_bitmap->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given input will exceed the maximum element in the resource. "
                     "Element was %d, pattern was 0x%x and repeats was %d", elem, free_info.pattern, free_info.repeats);
    }

exit:
    SHR_FUNC_EXIT;
}

static void
_rtb_alloc_free_pre_process(
    int unit,
    resource_tag_bitmap_alloc_info_t * alloc_free_info)
{
    int trailing_spaces, idx;
    uint32 original_pattern;

    /*
     * The bitmap is always behaving as if all input for sparse allocation/freeing was used.
     * In case it's not sparse allocation/freeing, we'd want to fix the values of the
     * sparse allocation/free fields (pattern, length, repeats) to match regular allocation/free.
     */
    if (!(alloc_free_info->flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        alloc_free_info->pattern = _RTB_PATTERN_NOT_SPARSE;
        alloc_free_info->length = 1;
        alloc_free_info->repeats = alloc_free_info->count;
    }
    /*
     * In case of sparse allocation/free, we processing the pattern to includes the repeats,
     * length without trailing spaces.
     * e.g. pattern = 1, length = 2, repeat = 2
     * The processed pattern will be 0101 -> 101
     */
    else
    {
        trailing_spaces = alloc_free_info->length - (utilex_msb_bit_on(alloc_free_info->pattern) + 1);
        original_pattern = alloc_free_info->pattern;
        for (idx = 1; idx < alloc_free_info->repeats; idx++)
        {
            alloc_free_info->pattern |= original_pattern << (idx * alloc_free_info->length);
        }
        alloc_free_info->length = alloc_free_info->length * alloc_free_info->repeats - trailing_spaces;
        alloc_free_info->repeats = 1;
    }

}

shr_error_e
resource_tag_bitmap_advanced_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t * free_info,
    int elem)
{
    uint8 temp, empty_grain = FALSE;
    int current_element, first_index_in_pattern, offset, repeat_index, use_sparse, grain_index;
    uint32 current_tag;
    uint32 current_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    uint32 nof_next_last_pointers = 0, current_next_last_pointer = 0;
    SHR_FUNC_INIT_VARS(unit);

    _rtb_alloc_free_pre_process(unit, free_info);

    SHR_IF_ERR_EXIT(_rtb_advanced_free_verify(unit, res_tag_bitmap, *free_info, elem));

    first_index_in_pattern = elem - res_tag_bitmap->first_element - utilex_lsb_bit_on(free_info->pattern);

    SHR_IF_ERR_EXIT(_rtb_tag_get_internal(unit, node_id, res_tag_bitmap, first_index_in_pattern, current_tags));
    
    current_tag = current_tags[0];
    /*
     * If the pattern is simple one, we can save run time but not going bit by bit, but by going en mass.
     */
    use_sparse = free_info->pattern != _RTB_PATTERN_NOT_SPARSE;

    current_element = first_index_in_pattern;
    for (repeat_index = 0; repeat_index < free_info->repeats; repeat_index++)
    {
        for (offset = 0; offset < free_info->length; offset++, current_element++)
        {
            if (free_info->pattern & (1 << offset))
            {
                DNX_SW_STATE_BIT_GET(unit, node_id, 0, res_tag_bitmap->data, current_element, &temp);
                if (temp)
                {
                    if (use_sparse)
                    {
                        DNX_SW_STATE_BIT_CLEAR(unit, node_id, 0, res_tag_bitmap->data, current_element);
                        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                        {
                            DNX_SW_STATE_BIT_CLEAR(unit, node_id, 0, res_tag_bitmap->tag_tracking_bitmap,
                                                   current_element);
                        }
                        DNX_SW_STATE_COUNTER_DEC(unit,
                                                 node_id,
                                                 res_tag_bitmap->used,
                                                 1, res_tag_bitmap->used, _RTB_NO_FLAGS, "res_tag_bitmap->used");
                        SHR_IF_ERR_EXIT(_rtb_is_last_reference
                                        (unit, node_id, res_tag_bitmap, current_element,
                                         &(free_info->last_reference), 1));
                        /*
                         * Decrement the number of allocated elements per grain.
                         */
                        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                        {
                            grain_index = _rtb_get_grain_index(res_tag_bitmap, current_element);

                            DNX_SW_STATE_COUNTER_DEC(unit,
                                                     node_id,
                                                     res_tag_bitmap->nof_allocated_elements_per_grain
                                                     [grain_index], 1,
                                                     res_tag_bitmap->nof_allocated_elements_per_grain
                                                     [grain_index], _RTB_NO_FLAGS,
                                                     "res_tag_bitmap->nof_allocated_elements_per_grain");

                            if (res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] == 0)
                            {
                                empty_grain = TRUE;
                                SHR_IF_ERR_EXIT(_rtb_restore_last_free_and_next_alloc(unit,
                                                                                      node_id,
                                                                                      res_tag_bitmap, current_tag));
                            }

                        }
                    }
                    else
                    {
                        /*
                         * We can just free the entire range when we get to the first bit, then break the loop.
                         * In this case, repeats indicates how many elements to deallocate.
                         */
                        DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, res_tag_bitmap->data, current_element,
                                                     free_info->repeats);

                        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                        {
                            DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0,
                                                         res_tag_bitmap->tag_tracking_bitmap, current_element,
                                                         free_info->repeats);
                        }
                        DNX_SW_STATE_COUNTER_DEC(unit,
                                                 node_id,
                                                 res_tag_bitmap->used,
                                                 free_info->repeats,
                                                 res_tag_bitmap->used, _RTB_NO_FLAGS, "res_tag_bitmap->used");
                        SHR_IF_ERR_EXIT(_rtb_is_last_reference
                                        (unit, node_id, res_tag_bitmap, current_element,
                                         &(free_info->last_reference), free_info->repeats));

                        /*
                         * Update the number of allocated elements in the relevant grain. The operation is set to FALSE because we are freeing elements.
                         */
                        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                        {
                            int grain_index = _rtb_get_grain_index(res_tag_bitmap, current_element);

                            SHR_IF_ERR_EXIT(_rtb_allocated_elements_per_bank_update
                                            (unit, node_id, res_tag_bitmap, free_info->repeats,
                                             current_element, FALSE));
                            if (res_tag_bitmap->nof_allocated_elements_per_grain[grain_index] == 0)
                            {
                                empty_grain = TRUE;
                                SHR_IF_ERR_EXIT(_rtb_restore_last_free_and_next_alloc(unit, node_id,
                                                                                      res_tag_bitmap, current_tag));
                            }
                        }

                        /*
                         * Set loop variables to maximum to break the loop.
                         */
                        offset = free_info->length;
                        repeat_index = free_info->repeats;
                    }
                }       /* if(_SHR_E_FULL == result) */
                else
                {
                    /*
                     * This bit was given as part of the pattern, but is not allocated. Break all loops and
                     * return error.
                     */

                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given element %d does not exist.", current_element);
                }
            }   /* if (this element is in the pattern) */
        }       /* for (pattern length) */
    }   /* for (repeat_index = 0; repeat_index < repeats; repeat_index++) */

    /** Suppress journaling comparison - this data is not supposed to be restored after add-delete. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    /*
     * Update the last free indication with the first bit that was freed in this pattern.
     */
    if ((res_tag_bitmap->lastFree[current_tag] >= first_index_in_pattern) && !empty_grain)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &res_tag_bitmap->lastFree[current_tag],
                                  &first_index_in_pattern,
                                  sizeof(first_index_in_pattern),
                                  _RTB_NO_FLAGS, "res_tag_bitmap->lastFree[current_tag]");
    }

    /*
     * Reset nextAlloc and lastFree if the resource is empty.
     */
    if (res_tag_bitmap->used == 0)
    {
        nof_next_last_pointers = res_tag_bitmap->max_tag_value + 1;

        DNX_SW_STATE_MEMSET(unit,
                            node_id,
                            res_tag_bitmap->nextAlloc,
                            0, 0, nof_next_last_pointers * sizeof(int), _RTB_NO_FLAGS, "(*res_tag_bitmap)->nextAlloc");

        /*
         * Set the lastFree value to count so it won't be used if nothing was actually freed.
         */
        for (current_next_last_pointer = 0; current_next_last_pointer < nof_next_last_pointers;
             current_next_last_pointer++)
        {
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                                      node_id,
                                      &res_tag_bitmap->lastFree[current_next_last_pointer],
                                      &res_tag_bitmap->count,
                                      sizeof(res_tag_bitmap->count), _RTB_NO_FLAGS, "(*res_tag_bitmap)->lastFree");
        }
    }

    /** Continue journaling comparison. */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/*
 *      Given a resource name and element, returns the next
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
    int resource_end, entry_id, first_entry, nof_grains, grain_size, grain_start, grain_end, grain_ind =
        0, nof_elements_in_grain = 0;
    resource_tag_bitmap_tag_info_t local_tag_info;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Verify that the resource exist.
     */
    if (*res_tag_bitmap == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource doesn't exist.");
    }

    nof_grains = resource_tag_bitmap_get_nof_grains((*res_tag_bitmap)->count, (*res_tag_bitmap)->grainSize,
                                                    (*res_tag_bitmap)->first_element, (*res_tag_bitmap)->flags);

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
    if (nof_grains == 1)
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
        grain_size = (*res_tag_bitmap)->grainSize;
        if ((*res_tag_bitmap)->nof_tags_levels > 1)
        {
            DNX_SW_STATE_PRINT(unit, " Multilevel tag resource with %d levels |", (*res_tag_bitmap)->nof_tags_levels);
        }
        DNX_SW_STATE_PRINT(unit, " Grain size: %d |", grain_size);
        DNX_SW_STATE_PRINT(unit, " Number of grains: %d |", nof_grains);
        if ((*res_tag_bitmap)->nof_tags_levels > 1)
        {
            DNX_SW_STATE_PRINT(unit, " Tag size: level 0 - %d, level 1 - %d |",
                               (*res_tag_bitmap)->firstTagSize,
                               (*res_tag_bitmap)->tagSize - (*res_tag_bitmap)->firstTagSize);
        }
        else
        {
            DNX_SW_STATE_PRINT(unit, " Tag size: %d |", (*res_tag_bitmap)->tagSize);
        }

        if (data->nof_used_elements)
        {
            DNX_SW_STATE_PRINT(unit, " Grain info (dumps only grains with allocated entries)\n");
            while (grain_ind < nof_grains && entry_id < resource_end)
            {
                nof_elements_in_grain = 0;
                grain_ind = _rtb_get_grain_index(*res_tag_bitmap, entry_id - data->create_data.first_element);
                grain_start = _rtb_get_grain_start(*res_tag_bitmap, entry_id - data->create_data.first_element) +
                    data->create_data.first_element;
                grain_size = _rtb_get_grain_size(*res_tag_bitmap, entry_id - data->create_data.first_element);
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
                    if ((entry_id == DNX_ALGO_RES_ILLEGAL_ELEMENT) || (entry_id >= grain_end))
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
                    
                    if ((*res_tag_bitmap)->nof_tags_levels > 1)
                    {
                        DNX_SW_STATE_PRINT(unit,
                                           "\t\t     Grain %d | Tags: level 0 - %d, level 1 - %d | Number of elements %d | Used Entries: ",
                                           grain_ind, local_tag_info.tags[0], local_tag_info.tags[1],
                                           nof_elements_in_grain);
                    }
                    else
                    {
                        DNX_SW_STATE_PRINT(unit, "\t\t     Grain %d | Tag %d | Number of elements %d | Used Entries: ",
                                           grain_ind, local_tag_info.tags[0], nof_elements_in_grain);
                    }

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
                            grain_ind = nof_grains;
                            break;
                        }
                        if (entry_id >= grain_end)
                        {
                            grain_ind =
                                _rtb_get_grain_index(*res_tag_bitmap, entry_id - data->create_data.first_element) - 1;
                            break;
                        }
                        DNX_SW_STATE_PRINT(unit, " %d |", entry_id);
                    }
                    DNX_SW_STATE_PRINT(unit, "\n");
                }

            }
        }
    }

    DNXC_SW_STATE_FUNC_RETURN;
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
    int repeat, current_index, bit_offset, use_sparse;
    uint8 temp;
    SHR_FUNC_INIT_VARS(unit);

    *can_allocate = TRUE;
    use_sparse = !(pattern == _RTB_PATTERN_NOT_SPARSE && length == 1);

    if (use_sparse)
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
    SHR_FUNC_INIT_VARS(unit);

    _RTB_IF_ERROR_EXIT(alloc_info->flags,
                       _rtb_is_block_free(unit, node_id, res_tag_bitmap, alloc_info->pattern,
                                          alloc_info->length, alloc_info->repeats, element, is_free_block));

    if (*is_free_block)
    {
        _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_tag_check
                           (unit, alloc_info->flags, node_id, res_tag_bitmap, element, allocated_block_length,
                            alloc_info->tags, tag_result));
    }

exit:
    SHR_FUNC_EXIT;
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
    int found_match, allocated_block_length;
    int tag_compare_result, skip_block;
    int grain_size, grain_count;
    uint8 temp;

    SHR_FUNC_INIT_VARS(unit);

    current_offset_index = 0;

    /*
     * Iterate between range_start and range_end, and check for suitable elements in the middle.
     */
    found_match = FALSE;
    index = range_start;
    allocated_block_length =
        (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));
    last_legal_index = range_end - allocated_block_length;
    pattern_first_set_bit = utilex_lsb_bit_on(alloc_info.pattern);

    while (index <= last_legal_index)
    {
        /*
         * First, skip grains until a grain with an appropriate tag is found.
         */
        do
        {
            grain_size = _rtb_get_grain_size(res_tag_bitmap, index);
            skip_block = FALSE;
            SHR_IF_ERR_EXIT(_rtb_tag_check
                            (unit, alloc_info.flags, node_id, res_tag_bitmap, index, allocated_block_length,
                             alloc_info.tags, &tag_compare_result));

            if (!tag_compare_result)
            {
                /*
                 * Tags are matching, but skip grain if it's full.
                 */
                int first_element_in_grain = _rtb_get_grain_start(res_tag_bitmap, index);
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
                temp_index = _rtb_get_grain_start(res_tag_bitmap, index + grain_size);
                /*
                 * Realign after this grain.
                 */
                temp_index =
                    (((temp_index + alloc_info.align - 1) / alloc_info.align) * alloc_info.align) + alloc_info.offs[0];
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
         * Found a matching block with matching tag. We might have exceeded grain boundry,
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
    uint32 pattern_mask, tag;
    uint8 wrong_offset;
    int pattern_first_index, allocated_block_length, current_offset_index;
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
    _RTB_IF_ERROR_EXIT(alloc_info.flags, _rtb_alloc_free_verify(unit, alloc_info));

    SHR_NULL_CHECK(elem, _SHR_E_PARAM, "elem \n");

    if (0 >= alloc_info.length)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM, "Pattern length must be at least 1. \n");
    }

    if (BYTES2BITS(sizeof(alloc_info.pattern)) < alloc_info.length)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                        "The pattern length=%d must be at most 32. length=length*repeats\n", alloc_info.length);
    }

    if (0 >= alloc_info.repeats)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM, "Pattern repeats must be at least 1. \n");
    }

    pattern_mask = (((uint32) 1 << (alloc_info.length - 1)) - 1) | ((uint32) 1 << (alloc_info.length - 1));
    if (0 == (alloc_info.pattern & pattern_mask))
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM, "pattern must contain at least one element. \n");
    }

    if (alloc_info.pattern & ~pattern_mask)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM, "pattern must not contain unused bits. \n");
    }

    tag = _rtb_get_calc_tag(res_tag_bitmap->nof_tags_levels, res_tag_bitmap->firstTagSize, alloc_info.tags);
    if (tag > res_tag_bitmap->max_tag_value)
    {
        _RTB_ERROR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                        "The tag = %d is greater than max_tag_value = %d. \n", tag, res_tag_bitmap->max_tag_value);
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
    allocated_block_length =
        (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));

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
    resource_tag_bitmap_extra_arguments_alloc_info_t *alloc_info;

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
        (((resource_tag_bitmap_extra_arguments_alloc_info_t *) extra_arguments)->first_reference) =
            (res_tag_bitmap_alloc_info.first_reference);
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
    int tag_index, tag_compare_result, can_allocate;
    SHR_BITDCL exist_tag = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If WITH_ID, then
     */

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
         * get the current tag of the requested element.
         */
        tag_index = _rtb_get_grain_index(res_tag_bitmap, *pattern_first_index);
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, res_tag_bitmap->tagData,
                                    (tag_index * (res_tag_bitmap->tagSize)), 0, res_tag_bitmap->tagSize, &exist_tag);
        /*
         * One of the elements has a mismatching tag.
         */
        
        if (res_tag_bitmap->nof_tags_levels > 1)
        {
            uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
            tags[0] = exist_tag;
            _rtb_update_splitted_tag(tags, res_tag_bitmap->nof_tags_levels, res_tag_bitmap->firstTagSize);
            _RTB_ERROR_EXIT(alloc_info->flags, _SHR_E_PARAM,
                            "Can't allocate resource WITH_ID. The requested ID is in a range that is assigned a different "
                            "property (represented by tags, level 0: %d, level 1: %d). \nA"
                            "sked for %d elements starting from index %d with tags, level 0: %d, level 1: %d.",
                            tags[0], tags[1], allocated_block_length, *elem, alloc_info->tags[0], alloc_info->tags[1]);
        }
        else
        {
            _RTB_ERROR_EXIT(alloc_info->flags, _SHR_E_PARAM,
                            "Can't allocate resource WITH_ID. The requested ID is in a range that is assigned a different "
                            "property (represented by tag %d). \nAsked for %d elements starting from index %d with tag %d.",
                            exist_tag, allocated_block_length, *elem, alloc_info->tags[0]);
        }
    }
    /*
     * If we got here then we successfully allocated.
     * Don't adjust last free or next alloc for WITH_ID.
     */
    *allocation_success = TRUE;

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
    int pattern_first_index, allocated_block_length, tag_for_pointers, nextAlloc, tag;
    int allocation_success = FALSE, update_last_free = FALSE, update_next_alloc = FALSE;
    int tmp_update_value = 0, current_offset_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    _rtb_alloc_free_pre_process(unit, alloc_info);

    /*
     * If the ALLOC_ALIGN flag wasn't set then probably the align field wasn't set either.
     * Since we use the align field anyway, set it to 1 since 0 would be illegal.
     */
    if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
        && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
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
    /*
     * After all fields are adjusted, verify the input.
     */
    _RTB_IF_ERROR_EXIT(alloc_info->flags, _rtb_alloc_verify(unit, res_tag_bitmap, *alloc_info, elem));

    /*
     * If we allocate ignoring tags, we need to read the next alloc and last free pointers from index 0,
     * and not from the given tag.
     */
    tag = _rtb_get_calc_tag(res_tag_bitmap->nof_tags_levels, res_tag_bitmap->firstTagSize, alloc_info->tags);
    tag_for_pointers = (_SHR_IS_FLAG_SET(alloc_info->flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG)) ? 0 : tag;

    /*
     * Calculate the length of the allocated block.
     * This value will be used to determine if the last element in the block that we're currently inspecting
     * is greater than the end of the bitmap. If it is, then we don't need to check it.
     * If the allocation is not sparse allocation, then the block length will just be the number of allocated elements.
     * If the allocation is sparse, then it will be the length of the pattern times repeats, minus the trailing 0s
     * that may be at the end of the block.
     */
    allocated_block_length =
        (utilex_msb_bit_on(alloc_info->pattern) + 1) + (alloc_info->length * (alloc_info->repeats - 1));

    if (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
    {
        _RTB_IF_ERROR_EXIT(alloc_info->flags,
                           _rtb_check_alloc_with_id(unit, node_id, res_tag_bitmap, alloc_info,
                                                    allocated_block_length, &pattern_first_index, elem,
                                                    &allocation_success));
    }
    else
    {   /* !(flags & SHR_RES_ALLOC_WITH_ID) */

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
                                                          res_tag_bitmap->lastFree[tag_for_pointers],
                                                          &pattern_first_index, &update_last_free,
                                                          &allocation_success));
            /*
             * If Last Free was not a good fit, try nextAlloc. Try it if nextAlloc is smaller than lastFree or lastFree is 0(to be sure we will not leave empty blocks)
             */
            if (!allocation_success &&
                res_tag_bitmap->nextAlloc[tag_for_pointers] < res_tag_bitmap->lastFree[tag_for_pointers])
            {
                _RTB_IF_ERROR_EXIT(alloc_info->flags,
                                   _rtb_check_alloc_heuristic(unit, node_id, res_tag_bitmap, alloc_info,
                                                              allocated_block_length,
                                                              res_tag_bitmap->nextAlloc[tag_for_pointers],
                                                              &pattern_first_index, &update_next_alloc,
                                                              &allocation_success));
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
                SHR_IF_ERR_EXIT(_rtb_allocate_next_free_element_in_range
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
                nextAlloc =
                    UTILEX_MIN(res_tag_bitmap->nextAlloc[tag_for_pointers], res_tag_bitmap->lastFree[tag_for_pointers]);

                pattern_first_index =
                    (((nextAlloc + alloc_info->align - 1) / alloc_info->align) * alloc_info->align) +
                    alloc_info->offs[0];
                SHR_IF_ERR_EXIT(_rtb_allocate_next_free_element_in_range
                                (unit, node_id, res_tag_bitmap, *alloc_info, pattern_first_index,
                                 res_tag_bitmap->count, &pattern_first_index, &allocation_success));

                if (!allocation_success)
                {
                    /*
                     * Couldn't place element after the last successfully allocated element.
                     * Try searching from the start of the pool.
                     */
                    pattern_first_index = alloc_info->offs[0];
                    SHR_IF_ERR_EXIT(_rtb_allocate_next_free_element_in_range
                                    (unit, node_id, res_tag_bitmap, *alloc_info, pattern_first_index, nextAlloc,
                                     &pattern_first_index, &allocation_success));
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
        {       /* (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS) */

            /*
             * Search from the start of the pool to allocate the element at the lowest available index.
             */
            pattern_first_index = UTILEX_ALIGN_UP(0, alloc_info->align) + alloc_info->offs[0];
            SHR_IF_ERR_EXIT(_rtb_allocate_next_free_element_in_range
                            (unit, node_id, res_tag_bitmap, *alloc_info, pattern_first_index, res_tag_bitmap->count,
                             &pattern_first_index, &allocation_success));

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
                SHR_IF_ERR_EXIT(_rtb_allocate_next_free_element_in_range
                                (unit, node_id, res_tag_bitmap, *alloc_info,
                                 alloc_info->range_start - res_tag_bitmap->first_element,
                                 alloc_info->range_end - res_tag_bitmap->first_element, &pattern_first_index,
                                 &allocation_success));
            }
        }
    }   /* else -> !(flags & SHR_RES_ALLOC_WITH_ID) */
    if ((allocation_success) && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY))
    {
        int repeat, bit_offset, current, allocated_bits, use_sparse;
        int new_used_count = 0;
        /*
         * set the tag for all grains involved
         * don't set tag if IGNORE_TAG flag is set.
         */
        if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            SHR_IF_ERR_EXIT(_rtb_tag_set_internal(unit, node_id, res_tag_bitmap, tag,
                                                  _RTB_FORCE_TAG_NO_CHANGE,
                                                  pattern_first_index, alloc_info->length,
                                                  &alloc_info->first_reference));
        }
        /*
         * mark the block as in-use
         */
        use_sparse = !(alloc_info->pattern == _RTB_PATTERN_NOT_SPARSE && alloc_info->length == 1);
        if (use_sparse)
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
                            DNX_SW_STATE_BIT_SET(unit, node_id, 0, res_tag_bitmap->tag_tracking_bitmap, current);
                        }
                        allocated_bits++;

                        /*
                         * Increment the number of allocated elements per grain.
                         */
                        if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                        {
                            int grain_index = _rtb_get_grain_index(res_tag_bitmap, current);

                            DNX_SW_STATE_COUNTER_INC(unit,
                                                     node_id,
                                                     res_tag_bitmap->nof_allocated_elements_per_grain[grain_index], 1,
                                                     res_tag_bitmap->nof_allocated_elements_per_grain[grain_index],
                                                     _RTB_NO_FLAGS,
                                                     "res_tag_bitmap->nof_allocated_elements_per_grain[current / res_tag_bitmap->grainSize]");
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
                DNX_SW_STATE_BIT_RANGE_SET(unit, node_id, 0, res_tag_bitmap->tag_tracking_bitmap, pattern_first_index,
                                           alloc_info->repeats);
            }
            allocated_bits = alloc_info->repeats;

            /*
             * Update the number of allocated elements in the relevant grain. The operation is set to TRUE because we are allocating elements.
             */
            if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
            {
                SHR_IF_ERR_EXIT(_rtb_allocated_elements_per_bank_update
                                (unit, node_id, res_tag_bitmap, alloc_info->repeats, pattern_first_index, TRUE));
            }
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
                                          &res_tag_bitmap->lastFree[tag_for_pointers],
                                          &tmp_update_value, sizeof(tmp_update_value),
                                          _RTB_NO_FLAGS, "res_tag_bitmap->lastFree[alloc_info->tag]");
            }

            if (update_next_alloc == TRUE)
            {
                tmp_update_value = pattern_first_index + allocated_block_length;
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &res_tag_bitmap->nextAlloc[tag_for_pointers],
                                          &tmp_update_value,
                                          sizeof(tmp_update_value),
                                          _RTB_NO_FLAGS, "res_tag_bitmap->nextAlloc[alloc_info->tag]");
            }
        }
        /** Continue journaling comparison. */
        SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, FALSE));

        /*
         * In case it's the first allocation in the resource tag bitmap, this is the first allocation in the grain
         */
        if (res_tag_bitmap->used == 0)
        {
            alloc_info->first_reference = 1;
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
                        "No free element matching required conditions. Asked for %d elements aligned to %d with offs %d and tag %d\n",
                        allocated_block_length, alloc_info->align, alloc_info->offs[0], alloc_info->tags[0]);
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
    int i;
    int zero = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, res_tag_bitmap->data, 0, res_tag_bitmap->count);

    if (res_tag_bitmap->tagSize > 0)
    {
        DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, res_tag_bitmap->tagData, 0,
                                     res_tag_bitmap->tagSize * (res_tag_bitmap->count / res_tag_bitmap->grainSize));
    }

    for (i = 0; i <= res_tag_bitmap->max_tag_value; i++)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &res_tag_bitmap->lastFree[i],
                                  &res_tag_bitmap->count, sizeof(res_tag_bitmap->count), _RTB_NO_FLAGS,
                                  "res_tag_bitmap->lastFree");

        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &res_tag_bitmap->nextAlloc[i],
                                  &zero, sizeof(zero), _RTB_NO_FLAGS, "res_tag_bitmap->nextAlloc");

    }

    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &res_tag_bitmap->used, &zero, sizeof(zero), _RTB_NO_FLAGS,
                              "res_tag_bitmap->used");

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
    uint32 *nof_allocated_elements_per_grain)
{
    SHR_FUNC_INIT_VARS(unit);

    if (res_tag_bitmap->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
    {
        *nof_allocated_elements_per_grain = res_tag_bitmap->nof_allocated_elements_per_grain[grain_index];
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
    SHR_FUNC_INIT_VARS(unit);

    create_info->first_element = res_tag_bitmap->first_element;
    create_info->count = res_tag_bitmap->count;
    create_info->grain_size[0] = res_tag_bitmap->grainSize;
    create_info->flags = res_tag_bitmap->flags;
    create_info->max_tag_value[0] = res_tag_bitmap->max_tag_value;

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
