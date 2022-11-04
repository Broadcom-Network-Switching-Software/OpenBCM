/** \file resource_tag_bitmap_utils.c
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

/** { */
/*************
* INCLUDES  *
*************/
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/alloc.h>
#include <shared/bitop.h>
#include <soc/error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap_utils.h>

#ifdef BCM_DNX_SUPPORT

/*
 * Macro used for data_per_entry print
 */
#define RES_MNGR_PRINT_STRING 100

inline static int
_rtb_is_sparse(
    int pattern,
    int length)
{
    return !(pattern == _RTB_PATTERN_NOT_SPARSE && length == 1);
}

uint8
resource_tag_bitmap_utils_is_multilevel_tags_resource(
    resource_tag_bitmap_utils_t resource)
{
    return (resource->nof_tags_levels > 1);
}

/* The function is returning index only for resource_alloc->data_per_entry_size*/
static inline uint8
_rtb_get_index_offset(
    resource_tag_bitmap_utils_t resource,
    int element)
{
    return resource->data_per_entry_size * element;
}

/* The function is address at given index only for resource_alloc->data_per_entry_size*/
static inline uint8 *
_rtb_get_data_at_idx(
    resource_tag_bitmap_utils_t resource,
    int element)
{
    return ((uint8 *) resource->data_per_entries + _rtb_get_index_offset(resource, element));
}

int
resource_tag_bitmap_utils_get_allocated_block_length(
    resource_tag_bitmap_utils_alloc_info_t alloc_info)
{
    return (utilex_msb_bit_on(alloc_info.pattern) + 1) + (alloc_info.length * (alloc_info.repeats - 1));
}

int
resource_tag_bitmap_utils_get_nof_small_grains_in_bigger_grains(
    resource_tag_bitmap_utils_t resource)
{
    if (resource_tag_bitmap_utils_is_multilevel_tags_resource(resource))
        return resource->tag_level_info[0].grain_size / resource->tag_level_info[1].grain_size;
    return 1;
}

int
resource_tag_bitmap_utils_get_nof_free_elements(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int grain_idx)
{
    return (resource->tag_level_info[tag_level].grain_size
            - resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_idx]);
}

/*
 * return grain/tag index of the element in the resource tag bitmap
 */
int
resource_tag_bitmap_utils_get_grain_index(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int element_index)
{
    if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        return ((element_index + resource->first_element) / resource->tag_level_info[tag_level].grain_size);
    }
    else
    {
        return (element_index / resource->tag_level_info[tag_level].grain_size);
    }
}

/*
 * This function returns the higher tag level
 *  e.g. returns 1 in the following case:
 *  level 0 - |   0   |   1   |
 *  level 1 - | 0 | 1 | 2 | 3 |
 *  bitmap  - |0|1|2|3|4|5|6|7|
 */
int
resource_tag_bitmap_utils_get_higher_tag_level(
    resource_tag_bitmap_utils_t resource)
{
    return (resource->nof_tags_levels - 1);
}

/*
 * return first element of the grain that include the element
 */
int
resource_tag_bitmap_utils_get_grain_start(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int element_index)
{
    /*
     * First grain of trimmed resource tag bitmap is trimmed, as a result all start of the grains are smaller by the
     * resource->first_element. The first grain is different and start in 0 as all resource tag bitmaps.
     */
    if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        /*
         * first grain starts from 0
         */
        if (element_index < resource->tag_level_info[tag_level].grain_size - resource->first_element)
        {
            return 0;
        }
        else
        {
            return (((element_index + resource->first_element)
                     / resource->tag_level_info[tag_level].grain_size)
                    * resource->tag_level_info[tag_level].grain_size - resource->first_element);
        }
    }
    else
    {
        return ((element_index / resource->tag_level_info[tag_level].grain_size)
                * resource->tag_level_info[tag_level].grain_size);
    }
}

int
resource_tag_bitmap_utils_get_grain_size(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int element_index)
{
    int grain_size, first_grain_size, last_grain_size;
    grain_size = resource->tag_level_info[tag_level].grain_size;
    /*
     * First and last grains of trimmed resource tag bitmap is trimmed
     */
    if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS)
    {
        first_grain_size = resource->tag_level_info[tag_level].grain_size - resource->first_element;
        last_grain_size = (resource->count - first_grain_size) % grain_size;
        if (last_grain_size == 0)
        {
            last_grain_size = grain_size;
        }
        if (element_index < (grain_size - resource->first_element))
        {
            return first_grain_size;
        }
        else if (element_index >= (resource->count - last_grain_size))
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
 * return number of grains in the resource tag bitmap
 */
int
resource_tag_bitmap_utils_get_nof_grains(
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

shr_error_e
resource_tag_bitmap_utils_tag_get_internal(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int elem_index,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS])
{
    SHR_BITDCL tmp;
    int tag_index, tag_level;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check arguments
     */
    if (elem_index < 0 || elem_index > resource->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not valid ID. Element(%d) must be between 0 and %d.", elem_index, resource->count);
    }

    for (tag_level = 0; tag_level < RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS; tag_level++)
    {
        tags[tag_level] = 0;
        if (resource->tag_level_info[tag_level].tag_size)
        {
            tag_index = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, elem_index);
            tmp = 0;
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, resource->tag_level_info[tag_level].tag_data,
                                        (tag_index * resource->tag_level_info[tag_level].tag_size), 0,
                                        resource->tag_level_info[tag_level].tag_size, &tmp);
            tags[tag_level] = tmp;
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_is_allocated(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int element,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_GET(unit, node_id, 0, resource->data, element - resource->first_element, is_allocated);

    SHR_EXIT();

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
    resource_tag_bitmap_utils_t resource,
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

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "Resource doesn't exist.");

    /*
     * Verify element.
     */
    SHR_NULL_CHECK(element, _SHR_E_PARAM, "element");

    if (*element < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The given element has negative value: \"%d.\"", *element);
    }

    current_element = UTILEX_MAX(resource->first_element, *element);

    for (; current_element < resource->first_element + resource->count; current_element++)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_is_allocated(unit, node_id, resource, current_element,
                                                               &is_allocated));
        if (is_allocated == 1)
        {
            break;
        }
    }

    if (current_element < resource->first_element + resource->count)
    {
        *element = current_element;
    }
    else
    {
        *element = DNX_ALGO_RES_ILLEGAL_ELEMENT;
    }

exit:if (LOG_CHECK(BSL_VERBOSE | BSL_LS_BCMDNX_RESMNGR))
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

/*
 * Init res tag bitmap create info.
 */
static shr_error_e
_rtb_init_create_info(
    int unit,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    resource_tag_bitmap_utils_create_info_t * rtb_create_info)
{
    resource_tag_bitmap_utils_extra_arg_create_info_t *extra_create_info;

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
        extra_create_info = (resource_tag_bitmap_utils_extra_arg_create_info_t *) extra_arguments;
        sal_memcpy(rtb_create_info->grains_size, extra_create_info->grains_size,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
        if (extra_create_info->nof_tags_levels <= 1)
        {
            rtb_create_info->nof_tags_levels = 1;
            if (extra_create_info->grains_size[0] < 1)
            {
                rtb_create_info->grains_size[0] = create_info->nof_elements;
            }
        }
        else
            rtb_create_info->nof_tags_levels = extra_create_info->nof_tags_levels;
        sal_memcpy(rtb_create_info->max_tags_value, extra_create_info->max_tags_value,
                   sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
        rtb_create_info->data_per_entry_size = extra_create_info->data_per_entry_size;
    }

    rtb_create_info->flags = create_info->flags;
    rtb_create_info->first_element = create_info->first_element;
    rtb_create_info->count = create_info->nof_elements;
    sal_strncpy(rtb_create_info->name, create_info->name, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify res tag bitmap create info.
 */
static shr_error_e
_rtb_create_info_verify(
    int unit,
    resource_tag_bitmap_utils_create_info_t * rtb_create_info)
{
    int idx, nof_tags_levels, nof_small_grains_in_big_grain, sum_max_tag = 1;
    SHR_FUNC_INIT_VARS(unit);

    nof_tags_levels = rtb_create_info->nof_tags_levels;

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
                     "not supported with multilevel tag resource (nof_tags = %d)\n", nof_tags_levels);
    }
    if (nof_tags_levels > 1)
    {
        if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS flag is"
                         "not supported with multilevel tag resource (nof_tags = %d)\n", nof_tags_levels);
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

    if (rtb_create_info->max_tags_value[0] <= 0)
    {
        if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
            SHR_ERR_EXIT(_SHR_E_PARAM, "RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag is "
                         "not supported when max tag value is not configured.");

        if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
            SHR_ERR_EXIT(_SHR_E_PARAM, "RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG flag is "
                         "not supported when max tag value is not configured");
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
    if (!_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY)
        && rtb_create_info->data_per_entry_size > 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "data_per_entry_size = %d  is given for the bitmap. \n"
                     "This API can be used only if RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY flag was given during creation of the resource\n",
                     rtb_create_info->data_per_entry_size);
    }
    if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY)
        && rtb_create_info->data_per_entry_size < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "'data_per_entry_size' should be bigger or equal to 1.\n Value of data_per_entry_size %d",
                     rtb_create_info->data_per_entry_size);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_tag_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    void *tag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get_internal(unit, node_id, resource,
                                                               ((resource_tag_bitmap_utils_tag_info_t *)
                                                                tag_info)->element - resource->first_element,
                                                               ((resource_tag_bitmap_utils_tag_info_t *)
                                                                tag_info)->tags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_create(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t * resource,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags,
    resource_tag_bitmap_utils_create_info_t * rtb_create_info)
{
    int tags_size[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    uint32 nof_grains[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    int use_tag, tag_level;
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
    SHR_IF_ERR_EXIT(_rtb_init_create_info(unit, create_info, extra_arguments, rtb_create_info));
    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(_rtb_create_info_verify(unit, rtb_create_info)));

    DNX_SW_STATE_ALLOC(unit, node_id, *resource, **resource, /* nof elements */ 1, 0,
                       "sw_state resource");

    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*resource)->data, rtb_create_info->count, NULL, 0, 0,
                              "(*resource)->data");

    DNX_SW_STATE_ALLOC(unit, node_id, (*resource)->name, char,
                       1,
                       0,
                       "(*resource)->name");

    for (tag_level = 0; tag_level < rtb_create_info->nof_tags_levels; tag_level++)
    {
        use_tag = rtb_create_info->max_tags_value[tag_level] > 0;
        if (use_tag)
        {
            nof_grains[tag_level] = resource_tag_bitmap_utils_get_nof_grains(rtb_create_info->count,
                                                                             rtb_create_info->grains_size[tag_level],
                                                                             rtb_create_info->first_element,
                                                                             tag_level, rtb_create_info->flags);
            tags_size[tag_level] = utilex_msb_bit_on(rtb_create_info->max_tags_value[tag_level]) + 1;

            DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*resource)->tag_level_info[tag_level].tag_data,
                                      (tags_size[tag_level]) * (nof_grains[tag_level]),
                                      NULL, 0, 0, "(*resource)->tag_data");

            if (rtb_create_info->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
            {
                DNX_SW_STATE_ALLOC(unit, node_id,
                                   (*resource)->tag_level_info[tag_level].nof_allocated_elements_per_grain, int,
                                   nof_grains[tag_level],
                                   0,
                                   "(*resource)->nof_allocated_elements_per_grain");
            }

            if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG))
            {
                DNX_SW_STATE_ALLOC_BITMAP(unit, node_id,
                                          (*resource)->tag_level_info[tag_level].tag_tracking_bitmap,
                                          rtb_create_info->count, NULL, 0, alloc_flags,
                                          "(*resource)->tag_tracking_bitmap");
            }

            if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
            {
                DNX_SW_STATE_ALLOC_BITMAP(unit, node_id,
                                          (*resource)->tag_level_info[tag_level].forced_tag_indication,
                                          nof_grains[tag_level], NULL, 0, RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE,
                                          "(*resource)->forced_tag_indication");
            }
        }

        /*
         * Set the bitmap "static" data.
         */
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*resource)->tag_level_info[tag_level].grain_size,
                                  &(rtb_create_info->grains_size[tag_level]),
                                  sizeof(rtb_create_info->grains_size[tag_level]), _RTB_NO_FLAGS,
                                  "(*resource)->grain_size");

        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*resource)->tag_level_info[tag_level].tag_size,
                                  &tags_size[tag_level],
                                  sizeof(tags_size[tag_level]), _RTB_NO_FLAGS, "(*resource)->tag_size");

        DNX_SW_STATE_MEMCPY_BASIC(unit,
                                  node_id,
                                  &(*resource)->tag_level_info[tag_level].max_tag_value,
                                  &(rtb_create_info->max_tags_value[tag_level]),
                                  sizeof(rtb_create_info->max_tags_value[tag_level]), _RTB_NO_FLAGS,
                                  "(*resource)->max_tag_value");
    }

    /*
     * Set the bitmap "static" data.
     */
    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*resource)->name,
                              &rtb_create_info->name,
                              sizeof(rtb_create_info->name), _RTB_NO_FLAGS, "(*resource)->name");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*resource)->count,
                              &(rtb_create_info->count),
                              sizeof(rtb_create_info->count), _RTB_NO_FLAGS, "(*resource)->count");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*resource)->nof_tags_levels,
                              &(rtb_create_info->nof_tags_levels),
                              sizeof(rtb_create_info->nof_tags_levels), _RTB_NO_FLAGS, "(*resource)->nof_tags_levels");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*resource)->first_element,
                              &rtb_create_info->first_element,
                              sizeof(rtb_create_info->first_element), _RTB_NO_FLAGS, "(*resource)->first element");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              node_id,
                              &(*resource)->flags,
                              &rtb_create_info->flags,
                              sizeof(rtb_create_info->flags), _RTB_NO_FLAGS, "(*resource)->flags");

    if (_SHR_IS_FLAG_SET(rtb_create_info->flags, RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY))
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &(*resource)->data_per_entry_size,
                                  &rtb_create_info->data_per_entry_size,
                                  sizeof(rtb_create_info->data_per_entry_size), _RTB_NO_FLAGS, "data_per_enty_size");

        DNX_SW_STATE_ALLOC(unit, node_id, (*resource)->data_per_entries, uint8,
                               (rtb_create_info->data_per_entry_size * (rtb_create_info->count)),
                           rtb_create_info->flags,
                           "data_per_entries");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_alloc_info_init(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_alloc_info_t * rtb_alloc_info,
    resource_tag_bitmap_utils_extra_arg_alloc_info_t ** alloc_info,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(rtb_alloc_info, 0, sizeof(resource_tag_bitmap_utils_alloc_info_t));

    if (extra_arguments != NULL)
    {
        *alloc_info = (resource_tag_bitmap_utils_extra_arg_alloc_info_t *) extra_arguments;
        rtb_alloc_info->align = (*alloc_info)->align;
        sal_memcpy(rtb_alloc_info->offs, (*alloc_info)->offs, sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS);
        rtb_alloc_info->nof_offsets = (*alloc_info)->nof_offsets;
        sal_memcpy(rtb_alloc_info->tags, (*alloc_info)->tags, sizeof(int) * RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS);
        rtb_alloc_info->pattern = (*alloc_info)->pattern;
        rtb_alloc_info->length = (*alloc_info)->length;
        rtb_alloc_info->repeats = (*alloc_info)->repeats;
        rtb_alloc_info->data = (*alloc_info)->data;
        /** Transfer the range_start and range_end parameters if ALLOC_IN_RANGE flag is defined. */
        if (_SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
        {
            rtb_alloc_info->range_start = (*alloc_info)->range_start;
            rtb_alloc_info->range_end = (*alloc_info)->range_end;
        }
    }

    rtb_alloc_info->flags = flags;
    rtb_alloc_info->count = nof_elements;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_alloc_pre_process(
    int unit,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info)
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
    else if (alloc_info->align > 1
             && (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_ALIGN)
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

shr_error_e
resource_tag_bitmap_utils_alloc_verify(
    int unit,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    int *elem)
{
    uint8 *data_buff;
    uint32 pattern_mask;
    uint8 wrong_offset, print_indx;
    int pattern_first_index, allocated_block_length, current_offset_index, tag_level;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If the bitmap is already full, then don't bother allocating, just return error.
     */
    if (resource->count < resource->used + alloc_info.count)
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_RESOURCE,
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
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                     "The pattern length=%d must be at most 32. length=length*repeats\n",
                                     alloc_info.length);
    }

    for (int i = 0; i < alloc_info.nof_offsets; i++)
    {
        if (alloc_info.align <= alloc_info.offs[i])
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "The offset[%d] = %d is bigger or equal than the alignment = %d.", i,
                         alloc_info.offs[i], alloc_info.align);
        }
    }

    if (!_SHR_IS_FLAG_SET(resource->flags,
                          RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
        && _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                     "Error: Can't allocate element with \"RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG\" flag when \"RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG\" flag isn't set.\n");
    }

    if (!_SHR_IS_FLAG_SET(resource->flags,
                          RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY)
        && _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY))
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                     "Error: Can't allocate element with \"RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY\" flag when \"RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY\" flag isn't set.\n");
    }

    if (!_SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY) && alloc_info.data != NULL)
    {
        data_buff = sal_alloc((sizeof(uint8) * resource->data_per_entry_size), "data_per_entry_size buffer");
        sal_memset(data_buff, 0, (sizeof(uint8) * resource->data_per_entry_size));
        sal_memcpy(data_buff, alloc_info.data, (resource->data_per_entry_size));
        DNX_SW_STATE_PRINT(unit, "\nData value: 0x");
        for (print_indx = 0; print_indx < resource->data_per_entry_size; ++print_indx)
        {
            DNX_SW_STATE_PRINT(unit, "%02x", data_buff[print_indx]);
        }
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                     "Can't set data without setting \"RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY\" \n");
    }

    if (resource_tag_bitmap_utils_is_multilevel_tags_resource(resource) &&
        _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN))
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                     "Error: Can't use RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN in multilevel resource manager.\n");
    }

    for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
    {
        if (alloc_info.tags[tag_level] > resource->tag_level_info[tag_level].max_tag_value)
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                         "The tag = %d is greater than max_tags_value[0] = %d. \n",
                                         alloc_info.tags[tag_level], resource->tag_level_info[tag_level].max_tag_value);
        }
    }

    if (_SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
    {
        if (_SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
            && (*elem < alloc_info.range_start || *elem >= alloc_info.range_end))
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags,
                                         _SHR_E_PARAM,
                                         "Given element WITH_ID that is not in the given range. Element is %d,"
                                         "range start is %d and range end is %d.\n", *elem, alloc_info.range_start,
                                         alloc_info.range_end);
        }
        if (alloc_info.range_start < resource->first_element)
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags,
                                         _SHR_E_PARAM,
                                         "Start ID for allocation in range %d cannot be smaller than the first element of the resource %d.\n",
                                         alloc_info.range_start, resource->first_element);
        }

        if (alloc_info.range_end > (resource->first_element + resource->count))
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags,
                                         _SHR_E_PARAM,
                                         "End ID for allocation in range %d cannot be larger than the first invalid element of the resource %d.\n",
                                         alloc_info.range_end, resource->first_element + resource->count);
        }
    }

    /*
     * Calculate the length of the allocated block. We can ignore trailing 0s in the pattern.
     */
    allocated_block_length = resource_tag_bitmap_utils_get_allocated_block_length(alloc_info);

    if (alloc_info.count > 1 && _SHR_IS_FLAG_SET(alloc_info.flags, RESOURCE_TAG_BITMAP_ALLOC_SPARSE))
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.count,
                                     _SHR_E_PARAM,
                                     "The number of elements should be equal to 1 when \"RESOURCE_TAG_BITMAP_ALLOC_SPARSE\". Please use repeats instead.\nThe number of elements is %d\n",
                                     alloc_info.count);
    }

    if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
    {
        int element_offset;
        /*
         * WITH_ID, so only try the specifically requested block.
         */
        if (*elem < resource->first_element)
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                         "Given element %d is too low. Must be at least %d. \n", *elem,
                                         resource->first_element);
        }
        pattern_first_index = *elem - resource->first_element - utilex_lsb_bit_on(alloc_info.pattern);
        if (pattern_first_index + allocated_block_length > resource->count)
        {
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                         "Allocating %d elements starting from element %d will exceed the maximum element %d. \n",
                                         allocated_block_length, *elem, resource->first_element + resource->count - 1);
        }

        if (alloc_info.flags & RESOURCE_TAG_BITMAP_ALLOC_SPARSE)
        {
            int bit_offset = 0;

            if (pattern_first_index < 0)
            {
                RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
                                             "The pattern first index %d is lower than 0. \n", pattern_first_index);
            }

            bit_offset = *elem - pattern_first_index;

            if (!(alloc_info.pattern & (1 << bit_offset)))
            {
                RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags, _SHR_E_PARAM,
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
            element_offset = (pattern_first_index + resource->first_element) % alloc_info.align;
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
            RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info.flags,
                                         _SHR_E_PARAM,
                                         "provided first element %d does not conform"
                                         " to provided align %d + offset values" " (actual offset = %d)\n", *elem,
                                         alloc_info.align, element_offset);
        }
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
    resource_tag_bitmap_utils_t resource,
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
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, resource->tag_level_info[tag_level].tag_data,
                                (tag_index * (resource->tag_level_info[tag_level].tag_size)), 0,
                                resource->tag_level_info[tag_level].tag_size, &tmp);
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
shr_error_e
resource_tag_bitmap_utils_tag_check(
    int unit,
    uint32 alloc_flags,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
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
    create_flags = resource->flags;
    ignore_tag = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG);
    use_empty_grain = _SHR_IS_FLAG_SET(alloc_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN);
    *result = 0;

    grain_size = resource->tag_level_info[tag_level].grain_size;
    tag_size = resource->tag_level_info[tag_level].tag_size;

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
        grain_start = resource_tag_bitmap_utils_get_grain_start(resource, tag_level, elem_index);
        grain_size = resource_tag_bitmap_utils_get_grain_size(resource, tag_level, elem_index);
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
        grain_size = resource_tag_bitmap_utils_get_grain_size(resource, tag_level, current_elem);
        tag_index = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, current_elem);

        if (!always_check || use_empty_grain)
        {
            /*
             * If we don't force check for the tag, then we check if there are any allocated elements in the
             * current grain. If there are, it means the tag is set for this range.
             * We would also check here for allocated elements if we want to use an empty grain.
             */
            if (_SHR_IS_FLAG_SET(create_flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG))
            {
                DNX_SW_STATE_BIT_GET(unit, node_id, 0,
                                     resource->tag_level_info[tag_level].forced_tag_indication,
                                     tag_index, &grain_tag_valid);
            }

            if (!grain_tag_valid || use_empty_grain)
            {
                grain_start = resource_tag_bitmap_utils_get_grain_start(resource, tag_level, current_elem);
                SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_is_grain_in_use
                                (unit, node_id, resource, alloc_flags, tag_level, grain_start, grain_size,
                                 &grain_in_use));

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
            SHR_IF_ERR_EXIT(_rtb_compare_tags(unit, node_id, resource, tag_index, tag, tag_level, &equal));

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

shr_error_e
resource_tag_bitmap_utils_check_element_for_allocation(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int element,
    int allocated_block_length,
    int *is_free_block,
    int *tag_result)
{
    int tag_level;
    SHR_FUNC_INIT_VARS(unit);

    *tag_result = 0;

    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                    resource_tag_bitmap_utils_is_block_free(unit, node_id, resource,
                                                                            alloc_info->pattern, alloc_info->length,
                                                                            alloc_info->repeats, element,
                                                                            is_free_block));

    if (*is_free_block)
    {
        /*
         * Check if all the tags are matching, in case one of the tags mismatching, no reason to continue the check
         * */
        for (tag_level = 0; tag_level < resource->nof_tags_levels && !(*tag_result); tag_level++)
        {
            RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                            resource_tag_bitmap_utils_tag_check(unit, alloc_info->flags, node_id,
                                                                                resource, element,
                                                                                allocated_block_length,
                                                                                alloc_info->tags[tag_level], tag_level,
                                                                                tag_result));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * In case allocation element/s with ID (RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
 * this function verify that tag matches and that all elements in the block are free.
 */
shr_error_e
resource_tag_bitmap_utils_check_alloc_with_id(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
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
    *pattern_first_index = *elem - resource->first_element - utilex_lsb_bit_on(alloc_info->pattern);

    /*
     * check whether the block is free
     */
    RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                    resource_tag_bitmap_utils_check_element_for_allocation(unit, node_id, resource,
                                                                                           alloc_info,
                                                                                           *pattern_first_index,
                                                                                           allocated_block_length,
                                                                                           &can_allocate,
                                                                                           &tag_compare_result));

    if (!can_allocate)
    {
        /*
         * In use; can't do WITH_ID alloc of this block
         */
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info->flags,
                                     _SHR_E_RESOURCE,
                                     "Can't allocate resource WITH_ID. Given element, or some of its block, "
                                     "already exists. Asked for %d elements starting from index %d\n",
                                     allocated_block_length, *elem);
    }
    if (tag_compare_result)
    {
        /*
         * One of the elements has a mismatching tag.
         */
        for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
        {
            /*
             * get the current tag of the requested element.
             */
            tag_index = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, *pattern_first_index);
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, resource->tag_level_info[tag_level].tag_data,
                                        (tag_index * (resource->tag_level_info[tag_level].tag_size)), 0,
                                        resource->tag_level_info[tag_level].tag_size, &(exist_tags[tag_level]));
        }
        RESOURCE_TAG_BITMAP_ERR_EXIT(alloc_info->flags,
                                     _SHR_E_PARAM,
                                     "Can't allocate resource WITH_ID. The requested ID is in a range that is assigned a different "
                                     "property. (represented by tags {%d, %d}). \nAsked for %d elements starting from index %d with tags {%d, %d} .",
                                     exist_tags[0], exist_tags[1], allocated_block_length, *elem, alloc_info->tags[0],
                                     alloc_info->tags[1]);
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
 * Update the number of allocated elements in the relevant grains.
 * The operation should set to FALSE when freeing elements and TRUE while allocating elements.
 */
shr_error_e
resource_tag_bitmap_utils_allocated_elements_per_bank_update(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int repeats,
    int element,
    int tag_level,
    uint8 operation)
{
    uint32 grain_index = 0, tmp_elements_to_update = 0;
    int grain_size, first_element, tmp_repeats = 0, grain_element_index = 0;
    uint8 is_trimmed_bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);

    grain_index = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, element);
    grain_size = resource->tag_level_info[tag_level].grain_size;
    first_element = resource->first_element;

    tmp_repeats = repeats;
    grain_element_index = element;

    if ((resource->flags & RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS))
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
                    resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index]
                    + (grain_end - grain_element_index);
            }
            else
            {
                tmp_elements_to_update =
                    resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index]
                    - (grain_end - grain_element_index);
            }
            DNX_SW_STATE_MEMCPY_BASIC(unit, node_id,
                                      &resource->
                                      tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index],
                                      &tmp_elements_to_update, sizeof(tmp_elements_to_update), _RTB_NO_FLAGS,
                                      "resource->nof_allocated_elements_per_grain");

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
                    resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] + tmp_repeats;
            }
            else
            {
                tmp_elements_to_update =
                    resource->tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index] - tmp_repeats;
            }
            /*
             * Since the repeats are only in the current grain set tmp_repeats to 0, to break the loop.
             */
            DNX_SW_STATE_MEMCPY_BASIC(unit, node_id,
                                      &resource->
                                      tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_index],
                                      &tmp_elements_to_update, sizeof(tmp_elements_to_update), _RTB_NO_FLAGS,
                                      "resource->nof_allocated_elements_per_grain");

            tmp_repeats = 0;
        }
        grain_index++;
        grain_element_index = grain_end;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_update_resource_after_allocation(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int pattern_first_index,
    int *elem,
    int *allocated_bits)
{
    int new_used_count = 0, tag_level = 0, grain_index = 0, index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * set the tag for all grains involved
     * don't set tag if IGNORE_TAG flag is set.
     */
    if (!(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
    {
        for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
        {
            RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags, resource_tag_bitmap_utils_tag_set
                                            (unit, node_id, resource, alloc_info->tags[tag_level], tag_level,
                                             _RTB_FORCE_TAG_NO_CHANGE, pattern_first_index, alloc_info->length,
                                             alloc_info->first_references));
        }
    }
    /*
     * mark the block as in-use
     */
    if (_rtb_is_sparse(alloc_info->pattern, alloc_info->length))
    {
        int repeat = 0, bit_offset = 0, current = 0;
        for (*allocated_bits = 0, current = pattern_first_index; repeat < alloc_info->repeats; repeat++)
        {
            for (bit_offset = 0; bit_offset < alloc_info->length; bit_offset++, current++)
            {
                if (alloc_info->pattern & (1 << bit_offset))
                {
                    DNX_SW_STATE_BIT_SET(unit, node_id, 0, resource->data, current);
                    if (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY)
                    {
                        DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, _rtb_get_data_at_idx(resource, current),
                                                  alloc_info->data, resource->data_per_entry_size, 0x0,
                                                  "Add data to the data_per_entries array");
                    }
                    if ((resource->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
                        && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
                    {
                        for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
                        {
                            DNX_SW_STATE_BIT_SET(unit, node_id, 0,
                                                 resource->tag_level_info[tag_level].tag_tracking_bitmap, current);
                        }
                    }
                    (*allocated_bits)++;

                    /*
                     * Increment the number of allocated elements per grain.
                     */
                    if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
                    {
                        for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
                        {
                            grain_index = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, current);

                            DNX_SW_STATE_COUNTER_INC(unit,
                                                     node_id,
                                                     resource->
                                                     tag_level_info[tag_level].nof_allocated_elements_per_grain
                                                     [grain_index], 1,
                                                     resource->
                                                     tag_level_info[tag_level].nof_allocated_elements_per_grain
                                                     [grain_index], _RTB_NO_FLAGS,
                                                     "resource->nof_allocated_elements_per_grain[current / resource->grain_size]");
                        }
                    }
                }
            }
        }
    }
    else
    {
        /*
         * not using sparse 
         */
        DNX_SW_STATE_BIT_RANGE_SET(unit, node_id, 0, resource->data, pattern_first_index, alloc_info->repeats);
        if (alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY)
        {
            for (index = pattern_first_index; index < pattern_first_index + alloc_info->repeats; index++)
            {
                DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, _rtb_get_data_at_idx(resource, index),
                                          alloc_info->data, resource->data_per_entry_size, 0x0,
                                          "Add data to the data_per_entries array");
            }
        }

        if ((resource->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
            && !(alloc_info->flags & RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG))
        {
            for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
            {
                DNX_SW_STATE_BIT_RANGE_SET(unit, node_id, 0,
                                           resource->tag_level_info[tag_level].tag_tracking_bitmap,
                                           pattern_first_index, alloc_info->repeats);
            }
        }
        *allocated_bits = alloc_info->repeats;

        /*
         * Update the number of allocated elements in the relevant grain. The operation is set to TRUE because we are allocating elements.
         */
        if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
        {
            for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
            {
                RESOURCE_TAG_BITMAP_IF_ERR_EXIT(alloc_info->flags,
                                                resource_tag_bitmap_utils_allocated_elements_per_bank_update(unit,
                                                                                                             node_id,
                                                                                                             resource,
                                                                                                             alloc_info->repeats,
                                                                                                             pattern_first_index,
                                                                                                             tag_level,
                                                                                                             TRUE));
            }
        }
    }

    /*
     * In case it's the first allocation in the resource tag bitmap, this is the first allocation in the grain
     */
    if (resource->used == 0)
    {
        alloc_info->first_references[tag_level] = 1;
    }
    new_used_count = resource->used + *allocated_bits;

    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &resource->used, &new_used_count,
                              sizeof(new_used_count), _RTB_NO_FLAGS, "resource->used");
    /*
     * return the first allocated element in the pattern.
     */
    *elem = pattern_first_index + resource->first_element + utilex_lsb_bit_on(alloc_info->pattern);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
rtb_free_verify(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
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

    last_freeing_element_idx = elem - resource->first_element + nof_elements;
    if (last_freeing_element_idx > resource->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Given input will exceed the maximum element in the resource. "
                     "Element was %d, nof_elements was %d", elem, nof_elements);
    }

    /*
     * Check if all of the bits in the range that should be unset are not free
     */
    for (elem_idx = elem - resource->first_element; elem_idx < last_freeing_element_idx; elem_idx++)
    {
        DNX_SW_STATE_BIT_GET(unit, node_id, 0, resource->data, elem_idx, &temp);
        if (!temp)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Attempting to free an already free resource (id=%d)",
                         elem_idx + resource->first_element);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_free_several(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    uint32 nof_elements,
    int elem,
    void *extra_argument)
{
    int current_element, tag_level;
    uint32 current_tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];

    SHR_FUNC_INIT_VARS(unit);

    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(rtb_free_verify(unit, node_id, resource, nof_elements, elem)));

    current_element = elem - resource->first_element;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get_internal(unit, node_id, resource, current_element, current_tags));

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, resource->data, current_element, nof_elements);

    for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
        if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
        {
            DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0,
                                         resource->tag_level_info[tag_level].tag_tracking_bitmap,
                                         current_element, nof_elements);
        }
    DNX_SW_STATE_COUNTER_DEC(unit, node_id, resource->used, nof_elements,
                             resource->used, _RTB_NO_FLAGS, "resource->used");

    if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY)
    {
        DNX_SW_STATE_MEMSET(unit, node_id, resource->data_per_entries, _rtb_get_index_offset(resource, current_element),
                            0, resource->data_per_entry_size * nof_elements, 0x0,
                            "free data from data_per_entries array");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_clear(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource)
{
    int grain_idx = 0, tag_level = 0, zero = 0;
    int nof_grains[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0, resource->data, 0, resource->count);

    DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, &resource->used, &zero, sizeof(zero), _RTB_NO_FLAGS, "resource->used");

    for (tag_level = 0; tag_level < resource->nof_tags_levels; tag_level++)
    {
        nof_grains[tag_level] = resource_tag_bitmap_utils_get_nof_grains(resource->count,
                                                                         resource->tag_level_info[tag_level].grain_size,
                                                                         resource->first_element, tag_level,
                                                                         resource->flags);
        if (resource->tag_level_info[tag_level].tag_size > 0)
        {
            DNX_SW_STATE_BIT_RANGE_CLEAR(unit,
                                         node_id,
                                         0,
                                         resource->tag_level_info[tag_level].tag_data,
                                         0,
                                         resource->tag_level_info[tag_level].tag_size
                                         * (resource->count / resource->tag_level_info[tag_level].grain_size));
        }
        if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN)
        {
            for (grain_idx = 0; grain_idx < nof_grains[tag_level]; grain_idx++)
            {
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                                          node_id,
                                          &resource->
                                          tag_level_info[tag_level].nof_allocated_elements_per_grain[grain_idx], &zero,
                                          sizeof(zero), _RTB_NO_FLAGS, "resource->nof_allocated_elements_per_grain");
            }
        }
        if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG)
        {
            DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0,
                                         resource->tag_level_info[tag_level].tag_tracking_bitmap, 0, resource->count);
        }
        if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG)
        {

            DNX_SW_STATE_BIT_RANGE_CLEAR(unit, node_id, 0,
                                         resource->tag_level_info[tag_level].forced_tag_indication,
                                         0, nof_grains[resource_tag_bitmap_utils_get_higher_tag_level(resource)]);
        }
    }

    if (resource->flags & RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY)
    {
        /*
         * Clear the data_per_entries array
         */
        DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, resource->data_per_entries, &zero, sizeof(zero),
                                  _RTB_NO_FLAGS, "resource->data_per_entries");
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_nof_free_elements_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int *nof_free_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_free_elements, _SHR_E_PARAM, "nof_free_elements");

    /*
     * The number of free elements is the number of existing elements (count) -
     *    the number of used elements.
     */
    *nof_free_elements = resource->count - resource->used;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_allocated_elements, _SHR_E_PARAM, "nof_elements");

    if (range_start < resource->first_element)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range start(%d) must be bigger than resource tag bitmap first element(%d).\n",
                     range_start, resource->first_element);
    }

    if ((range_start + nof_elements_in_range) > (resource->first_element + resource->count))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range end(%d) must be smaller than resource tag bitmap last element(%d).\n",
                     (range_start + nof_elements_in_range), (resource->first_element + resource->count));
    }

    if (nof_elements_in_range < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range size(%d) must be at least 1.\n", nof_elements_in_range);
    }

    DNX_SW_STATE_BIT_RANGE_COUNT(unit, node_id, 0, resource->data,
                                 range_start - resource->first_element, nof_elements_in_range, nof_allocated_elements);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_data_per_entry_verify(
    int unit,
    resource_tag_bitmap_utils_t resource,
    const void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!(resource->flags & RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY))
    {
        RESOURCE_TAG_BITMAP_ERR_EXIT(resource->flags, _SHR_E_PARAM,
                                     "data for data_per_entry is given for the bitmap. \n"
                                     "This API can be used only if RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY flag was given during creation of the resource\n");
    }
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_data_per_entry_set(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int element,
    const void *data)
{
    int element_index;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    DNXC_VERIFY_INVOKE(RESOURCE_TAG_BITMAP_IF_ERR_EXIT(resource->flags,
                                                       _rtb_data_per_entry_verify(unit, resource, data)));

    element_index = element - resource->first_element;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_is_allocated(unit, node_id, resource, element, &is_allocated));
    if (is_allocated != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Bit = %d is not set/allocated. Data per entry can't be set for bit which is not allocated.",
                     element);
    }
    else
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit, node_id, _rtb_get_data_at_idx(resource, element_index), data,
                                  resource->data_per_entry_size, 0x0, "Add data to the data_per_entries array");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_data_per_entry_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int element,
    void *data)
{
    int element_index;
    SHR_FUNC_INIT_VARS(unit);

    DNXC_VERIFY_INVOKE(RESOURCE_TAG_BITMAP_IF_ERR_EXIT(resource->flags,
                                                       _rtb_data_per_entry_verify(unit, resource, data)));

    element_index = element - resource->first_element;

    DNX_SW_STATE_MEMREAD(unit, data, resource->data_per_entries,
                         _rtb_get_index_offset(resource, element_index),
                         resource->data_per_entry_size, 0, "get data at index");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_data_per_entries_print(
    int unit,
    uint32 node_id,
    uint32 start_index,
    uint32 end_index,
    resource_tag_bitmap_utils_t resource,
    uint8 *data_per_entry_buff,
    char *print_string)
{
    int print_idx, element_iterator;

    SHR_FUNC_INIT_VARS(unit);

    if (resource->data_per_entry_size != 0 && resource->data_per_entries != NULL)
    {
        DNX_SW_STATE_PRINT(unit, "\n");
        DNX_SW_STATE_PRINT(unit, "%s", print_string);
        for (element_iterator = start_index; element_iterator < end_index; element_iterator++)
        {
            sal_memset(data_per_entry_buff, 0, (sizeof(uint8) * resource->data_per_entry_size));
            resource_tag_bitmap_utils_data_per_entry_get(unit, node_id, resource, element_iterator,
                                                         data_per_entry_buff);
            DNX_SW_STATE_PRINT(unit, " 0x");
            for (print_idx = 0; print_idx < resource->data_per_entry_size; ++print_idx)
            {
                DNX_SW_STATE_PRINT(unit, "%02x", data_per_entry_buff[print_idx]);
            }
            DNX_SW_STATE_PRINT(unit, " | ");
        }
    }
    DNX_SW_STATE_PRINT(unit, "\n");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_print(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    dnx_algo_res_dump_data_t * data)
{
    int resource_end, entry_id, first_entry, tag_level, grain_size, grain_start, grain_end, nof_tags_levels,
        grain_ind = 0, nof_elements_in_grain = 0;
    uint8 *data_per_entry_buff = NULL;
    char print_str[RES_MNGR_PRINT_STRING] = { 0 };
    resource_tag_bitmap_utils_tag_info_t local_tag_info;
    int nof_grains[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Verify that the resource exist.
     */
    if (resource == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource doesn't exist.");
    }

    nof_tags_levels = resource->nof_tags_levels;
    for (tag_level = 0; tag_level < nof_tags_levels; tag_level++)
    {
        nof_grains[tag_level] = resource_tag_bitmap_utils_get_nof_grains(resource->count,
                                                                         resource->tag_level_info[tag_level].grain_size,
                                                                         resource->first_element, tag_level,
                                                                         resource->flags);
    }

    DNX_SW_STATE_PRINT(unit, "\nResource :%s", data->create_data.name);
    DNX_SW_STATE_PRINT(unit,
                       " (Advanced Algorithm - %s)",
                       (data->create_data.advanced_algorithm == DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC ?
                        "No" : data->create_data.advanced_algo_name));

    DNX_SW_STATE_PRINT(unit, "\n|");

    DNX_SW_STATE_PRINT(unit, " Number of entries : %d |", data->create_data.nof_elements);
    DNX_SW_STATE_PRINT(unit, " Number of used entries : %d |", data->nof_used_elements);
    DNX_SW_STATE_PRINT(unit, " First entry ID : %d |\n", data->create_data.first_element);

    entry_id = data->create_data.first_element;
    resource_end = data->create_data.nof_elements + data->create_data.first_element;
    if (resource->data_per_entry_size > 0)
    {
        SHR_ALLOC_SET_ZERO(data_per_entry_buff, resource->data_per_entry_size, "data_per_entry_buff", "%s%s%s",
                           EMPTY, EMPTY, EMPTY);
    }

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
                SHR_IF_ERR_EXIT(_rtb_get_next(unit, node_id, resource, &entry_id, data->create_data.name));

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
            /*
             * Print for the data per entries
             */
            sal_strncpy(print_str, "Data per entries :", RES_MNGR_PRINT_STRING);
            _rtb_data_per_entries_print(unit, node_id, data->create_data.first_element,
                                        data->create_data.nof_elements + data->create_data.first_element,
                                        resource, data_per_entry_buff, print_str);
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
            DNX_SW_STATE_PRINT(unit, "Grain size: %d | ", resource->tag_level_info[tag_level].grain_size);
            DNX_SW_STATE_PRINT(unit, "Number of grains: %d | ", nof_grains[tag_level]);
            DNX_SW_STATE_PRINT(unit, "Tag size: %d | ", resource->tag_level_info[tag_level].tag_size);
            DNX_SW_STATE_PRINT(unit, "Max tag value: %d", resource->tag_level_info[tag_level].max_tag_value);
        }
        tag_level--;

        if (data->nof_used_elements)
        {
            DNX_SW_STATE_PRINT(unit, "\n\tGrain info (dumps only grains with allocated entries)\n");
            while (grain_ind < nof_grains[tag_level] && entry_id < resource_end)
            {
                nof_elements_in_grain = 0;
                grain_ind =
                    resource_tag_bitmap_utils_get_grain_index(resource, tag_level,
                                                              entry_id - data->create_data.first_element);
                grain_start =
                    resource_tag_bitmap_utils_get_grain_start(resource, tag_level,
                                                              entry_id - data->create_data.first_element) +
                    data->create_data.first_element;
                grain_size =
                    resource_tag_bitmap_utils_get_grain_size(resource, tag_level,
                                                             entry_id - data->create_data.first_element);
                grain_end = grain_start + grain_size;
                first_entry = entry_id;

                local_tag_info.element = grain_start;

                SHR_IF_ERR_EXIT(resource_tag_bitmap_utils_tag_get(unit, node_id, resource, &local_tag_info));

                /*
                 * calculate number of elements in the grain
                 */
                for (; entry_id < grain_end; entry_id++)
                {
                    SHR_IF_ERR_EXIT(_rtb_get_next(unit, node_id, resource, &entry_id, data->create_data.name));
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
                        SHR_IF_ERR_EXIT(_rtb_get_next(unit, node_id, resource, &entry_id, data->create_data.name));
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
                            grain_ind = resource_tag_bitmap_utils_get_grain_index(resource, tag_level,
                                                                                  entry_id -
                                                                                  data->create_data.first_element) - 1;
                            break;
                        }
                        DNX_SW_STATE_PRINT(unit, " %d |", entry_id);
                    }
                    /*
                     * Print for the data per entries
                     */
                    sal_snprintf(print_str, RES_MNGR_PRINT_STRING,
                                 "Data per entries for Grain %d bit range{%d; %d}:", grain_ind, grain_start, grain_end);
                    _rtb_data_per_entries_print(unit, node_id, grain_start, grain_end, resource,
                                                data_per_entry_buff, print_str);
                }

            }
        }
    }
exit:
    SHR_FREE(data_per_entry_buff);
    SHR_FUNC_EXIT;
}

static shr_error_e
_rtb_tag_set_utils_verify(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    uint32 tag,
    int force_tag,
    int force_tag_allowed,
    int tag_level,
    int elem_index,
    int elem_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (tag > resource->tag_level_info[tag_level].max_tag_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The tag = %d is greater than max_tag_value = %d.", tag,
                     resource->tag_level_info[tag_level].max_tag_value);
    }

    if (elem_index + elem_count - 1 >= resource->count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Trying to tag grain of element that its index %d greater than the number of elements in the resource %d",
                     elem_index + elem_count - 1, resource->count);
    }

    if (force_tag && (force_tag != _RTB_FORCE_TAG_NO_CHANGE) && !force_tag_allowed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "force_tag can't be set if the ALLOW_FORCING_TAG flag wasn't set on init.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_tag_set(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
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
    SHR_BITDCL tmp;

    SHR_FUNC_INIT_VARS(unit);

    first_references[tag_level] = 0;
    new_tag[0] = tag;

    force_tag_allowed = _SHR_IS_FLAG_SET(resource->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG)
        && (force_tag != _RTB_FORCE_TAG_NO_CHANGE);

    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT
                       (_rtb_tag_set_utils_verify
                        (unit, node_id, resource, tag, force_tag, force_tag_allowed, tag_level, elem_index,
                         elem_count)));
    /*
     * 'tag' is asymmetrical after free, which results in comparison journal issue in 'alloc -> free' scenarios.
     */
    SHR_IF_ERR_EXIT(dnx_state_comparison_suppress(unit, TRUE));

    if (resource->tag_level_info[tag_level].tag_size)
    {
        index = resource_tag_bitmap_utils_get_grain_index(resource, tag_level, elem_index);
        /*
         * count represent number of grains that should be tagged
         */
        if (RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS && index == 0 && elem_count > resource->first_element)
        {
            count =
                resource_tag_bitmap_utils_get_grain_index(resource, tag_level, elem_index + elem_count - 1) - index + 1;
        }
        else
        {
            count = (elem_count + resource->tag_level_info[tag_level].grain_size - 1)
                / resource->tag_level_info[tag_level].grain_size;
        }

        for (offset = 0; offset < count; offset++)
        {
            grain_size = resource_tag_bitmap_utils_get_grain_size(resource, tag_level, elem_index);
            /*
             * check if first reference should set
             */
            if (!first_references[tag_level])
            {
                grain_start = resource_tag_bitmap_utils_get_grain_start(resource, tag_level, elem_index);
                DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, resource->data, grain_start, grain_size, &grain_in_use);
                /*
                 * if the grain is empty, first reference indication should be returned
                 */
                if (!grain_in_use)
                {
                    first_references[tag_level] = 1;
                }
            }

            tmp = 0;
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, resource->tag_level_info[tag_level].tag_data,
                                        ((index + offset) * resource->tag_level_info[tag_level].tag_size), 0,
                                        resource->tag_level_info[tag_level].tag_size, &tmp);

            /*
             * Update the swstate only if the tag is different, than the one we already have.
             */
            if (tmp != tag)
            {
                DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, resource->tag_level_info[tag_level].tag_data,
                                             ((index + offset) * resource->tag_level_info[tag_level].tag_size), 0,
                                             resource->tag_level_info[tag_level].tag_size, new_tag);
            }

            if (force_tag_allowed)
            {
                if (force_tag)
                {
                    DNX_SW_STATE_BIT_SET(unit, node_id, 0,
                                         resource->tag_level_info[tag_level].forced_tag_indication, index + offset);
                }
                else
                {
                    DNX_SW_STATE_BIT_CLEAR(unit, node_id, 0,
                                           resource->tag_level_info[tag_level].forced_tag_indication, index + offset);
                }
            }
            elem_index += grain_size;
        }
    }

exit:
    dnx_state_comparison_suppress(unit, FALSE);
    SHR_FUNC_EXIT;
}

shr_error_e
resource_tag_bitmap_utils_is_block_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
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
                    DNX_SW_STATE_BIT_GET(unit, node_id, 0, resource->data, current_index, &temp);

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
        DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, resource->data, element, repeats, &temp);
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

shr_error_e
resource_tag_bitmap_utils_is_grain_in_use(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    uint32 alloc_flags,
    int tag_level,
    int grain_start,
    int grain_size,
    uint8 *grain_in_use)
{
    SHR_BITDCL *bmp;
    SHR_FUNC_INIT_VARS(unit);

    bmp = _SHR_IS_FLAG_SET(resource->flags, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG) ?
        (resource->tag_level_info[tag_level].tag_tracking_bitmap) : (resource->data);

    DNX_SW_STATE_BIT_RANGE_TEST(unit, node_id, 0, bmp, grain_start, grain_size, grain_in_use);

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
