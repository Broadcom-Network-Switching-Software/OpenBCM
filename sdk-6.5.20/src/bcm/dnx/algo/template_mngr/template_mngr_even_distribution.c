/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * algo_port_pp_esem_cmd_allocation.c
 *
 *  Created on: Feb 25, 2020
 *      Author: eb892187
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_even_distribution.h>
/** } **/

/**
 * \brief -
 *   The function uses the smart template 'tag' functionality as a counter for elements in the grain.
 *   The function increments/decrements (by one) the value of the tag associated with the given profile
 *    depends on the value of the 'increment' boolean flag.
 *
 * \param [in] unit - relevant unit
 * \param [in] module_id - relevant module_id
 * \param [in,out] multi_set_template -
 *      The multi_set_template that the tag change will be performed on
 *      The field that changes is multi_set_template->allocation_bitmap->tagData
 *      See 'dnx_algo_smart_template_tag_set' for more information.
 * \param [in] profile - A profile that has the tag that we would like to change
 * \param [in] increment - Boolean that specify increment or decrement the counter
 *      TRUE for increment and FALSE for decrement
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 *
 * \see
 *   dnx_algo_smart_template_tag_set
 */
shr_error_e
dnx_algo_template_mngr_even_distribution_set_cnt_tag(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    uint8 increment)
{
    resource_tag_bitmap_tag_info_t tag_set_info, tag_get_info;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tag_set_info, 0, sizeof(resource_tag_bitmap_tag_info_t));
    sal_memset(&tag_get_info, 0, sizeof(resource_tag_bitmap_tag_info_t));

    /** Get the the current counter value associated with 'profile' */
    tag_get_info.element = profile;
    rv = dnx_algo_smart_template_tag_get(unit, module_id, multi_set_template, &tag_get_info);

    /** Increment/decrement tag value */
    /*
     * getting '_SHR_E_NOT_FOUND' means that the tag isn't set (there is no allocation associated with this grain) we
     * still need to perform changes in the grain tag having received that error (e.g. freeing the last allocated
     * profile of a grain)
     */
    if (rv != _SHR_E_NOT_FOUND)
    {
        /** Exit function if error occurred that is different than 'entry not found' */
        SHR_IF_ERR_EXIT(rv);

        /** Set the 'set tag' information */
        /*
         * Force tag is set to TRUE as indicated in the resource_tag_bitmap_tag_info_t structure
         *  for template managers that allows forcing tag in creation
         */
        tag_set_info.force_tag = TRUE;
        /** Set the new tag value to be the value before incremented/decremented by 1 */
        tag_set_info.tag = (increment) ? tag_get_info.tag + 1 : tag_get_info.tag - 1;
        /*
         * We call this function with one element so the change will be only for one tag
         *  (the one associated with the profile)
         */
        tag_set_info.nof_elements = 1;
        tag_set_info.element = profile;
        /** Set tag value using smart template */
        SHR_IF_ERR_EXIT(dnx_algo_smart_template_tag_set(unit, module_id, multi_set_template, &tag_set_info));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   This function verifies the input for dnx_algo_template_mngr_even_distribution_set_range
 *
 * \param [in] unit - Relevant unit
 * \param [in] multi_set_template - Relevant template manager
 * \param [in] flags - Allocation flags (template manager flags)
 * \param [in] extra_alloc_info - Extra allocation information
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   The given range should be grain size compatible (the start and end range should be a multiplication of the grain)
 *
 * \see
 *   dnx_algo_template_mngr_even_distribution_set_range documentation for farther information
 */
shr_error_e
dnx_algo_template_mngr_even_distribution_set_range_verify(
    int unit,
    multi_set_t multi_set_template,
    uint32 flags,
    smart_template_alloc_info_t * extra_alloc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(extra_alloc_info->resource_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
    {
        int grain_size = multi_set_template->allocation_bitmap->grainSize;
        /** Range should be a multiplication of the grain size */
        if ((extra_alloc_info->resource_alloc_info.range_start % grain_size != 0)
            || (extra_alloc_info->resource_alloc_info.range_end % grain_size != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Smart template for even distribution, invalid arguments\n"
                         "range (%d - %d) should be multiplication of the grain (%d).\n",
                         extra_alloc_info->resource_alloc_info.range_start,
                         extra_alloc_info->resource_alloc_info.range_end, grain_size);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   This function changes the range of the allocation.
 *   The range is set to be the grain that has the smallest counter within range limitations
 *      if those were given.
 *   The counter represents current capacity of the grain.
 *   The rage change is done in order to achieve even distribution in a deterministic way.
 *
 * \param [in] unit - Relevant unit
 * \param [in] module_id - Relevant module_id
 * \param [in] multi_set_template - Relevant template manager
 * \param [in] flags - Allocation flags (template manager flags)
 * \param [in,out] extra_alloc_info - Extra allocation information
 *      This structure contains range information that will be changed as explained above (in brief)
 *      The fields changed are 'extra_alloc_info->resource_alloc_info.range_start'
 *          and 'extra_alloc_info->resource_alloc_info.range_end'
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 *
 * \see
 *   dnx_algo_port_pp_esem_cmd_allocate documentation for farther explanation
 */
shr_error_e
dnx_algo_template_mngr_even_distribution_set_range(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    smart_template_alloc_info_t * extra_alloc_info)
{
    resource_tag_bitmap_tag_info_t tag_get_info;
    int iter_range_start, iter_range_end, min_allocations, chosen_grain, grain_iter, increment;
    /** This variable is part of a preparation for making reverse order allocation an option using flags */
    uint8 allocate_in_reverse_order = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_template_mngr_even_distribution_set_range_verify
                    (unit, multi_set_template, flags, extra_alloc_info));

    /** Temporary value in case all the range given is already allocated */
    chosen_grain = 0;
    sal_memset(&tag_get_info, 0, sizeof(resource_tag_bitmap_tag_info_t));

    /**
     * Iterate over all the grains in order to fined the grain with the minimal number of allocations
     */
    /** Set the range for the iterator*/
    if (_SHR_IS_FLAG_SET(extra_alloc_info->resource_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
    {
        /** If 'allocate in range' flag is set define the iterator ranges as defined in the input information */
        iter_range_start = extra_alloc_info->resource_alloc_info.range_start;
        iter_range_end = extra_alloc_info->resource_alloc_info.range_end;
    }
    else
    {
        /*
         * If the 'ALLOC_IN_RANGE' flag is not set, than we define the iteration range to be from 'low' to 'count'
         *  - 'low' is the lowest allocation id possible
         *  - 'count' is the number of elements that can be allocated
         */
        extra_alloc_info->resource_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
        iter_range_start = multi_set_template->allocation_bitmap->low;
        iter_range_end = multi_set_template->allocation_bitmap->count;
    }

    /** Set increment value for the iterator */
    increment = multi_set_template->allocation_bitmap->grainSize;

    /*
     * If 'allocate in reverse order' is set, switch the ranges and the increment sign
     * to fit reverse order iterations (currently the variable is hard coded to reverse allocation)
     */
    if (allocate_in_reverse_order)
    {
        grain_iter = iter_range_start;
        iter_range_start = iter_range_end - 1;
        iter_range_end = grain_iter - 1;
        increment *= -1;
    }
    /** Iterate over the counter in the given range and find the grain that has the smallest tag value */
    grain_iter = iter_range_start;
    /*
     * Set minimum value to the maximum value possible,
     *  which is the same value as the grain size
     * This is because we use tag to count the number of element allocated in each grain
     */
    min_allocations = multi_set_template->allocation_bitmap->grainSize;
    while (grain_iter - iter_range_end != 0)
    {
        int rv;

        /** Set the element in get tag to the current iteration grain to be checked*/
        tag_get_info.element = grain_iter;
        rv = dnx_algo_smart_template_tag_get(unit, module_id, multi_set_template, &tag_get_info);
        if (rv == _SHR_E_NOT_FOUND)
        {
            /*
             * If 'not found' error occurred than it means that no allocations were made from this grain.
             * This means that we can just allocate this grain and no farther examination is needed
             */
            chosen_grain = grain_iter / multi_set_template->allocation_bitmap->grainSize;
            break;
        }
        else if (rv != _SHR_E_NONE)
        {
            /** If different error has occurred we would like to exit the function */
            SHR_IF_ERR_EXIT(rv);
        }
        else if (tag_get_info.tag < min_allocations)
        {
            /** Update the minimum value and the grain that matches the value */
            min_allocations = tag_get_info.tag;
            chosen_grain = grain_iter / multi_set_template->allocation_bitmap->grainSize;
        }
        /** Increment iterator */
        grain_iter += increment;
    }
    /** Set the resource allocation range values inside extra_alloc_info to the chosen grain */
    extra_alloc_info->resource_alloc_info.range_start = chosen_grain * multi_set_template->allocation_bitmap->grainSize;
    extra_alloc_info->resource_alloc_info.range_end =
        extra_alloc_info->resource_alloc_info.range_start + multi_set_template->allocation_bitmap->grainSize;
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_algo_template_mngr_even_distribution_allocate(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    smart_template_alloc_info_t extra_alloc_info;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Create new smart_template_alloc_info_t variable
     *  so we can fill it with values necessary for even distribution algorithm
     */
    if (extra_arguments == NULL)
    {
        sal_memset(&extra_alloc_info, 0, sizeof(smart_template_alloc_info_t));
    }
    else
    {
        extra_alloc_info = *((smart_template_alloc_info_t *) extra_arguments);
    }
    /*
     * set allocation flag to ignore tag because we only use tag as a counter
     * we are not looking for a grain with tag match
     */
    extra_alloc_info.resource_flags |= RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG;

    if (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
    {
        /*
         * If no specific ID was given than change the range to fit the grain
         * we would like to allocate from
         */
        SHR_IF_ERR_EXIT(dnx_algo_template_mngr_even_distribution_set_range(unit,
                                                                           module_id, multi_set_template, flags,
                                                                           &extra_alloc_info));
    }

    /** Allocate using smart template */
    SHR_IF_ERR_EXIT(dnx_algo_smart_template_allocate(unit,
                                                     module_id,
                                                     multi_set_template,
                                                     flags,
                                                     nof_references,
                                                     profile_data,
                                                     (void *) &extra_alloc_info, profile, first_reference));
    
    if (extra_arguments != NULL)
    {
        smart_template_alloc_info_t *alloc_info = (smart_template_alloc_info_t *) extra_arguments;
        if (_SHR_IS_FLAG_SET(alloc_info->resource_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
        {
            SHR_RANGE_VERIFY(*profile, alloc_info->resource_alloc_info.range_start,
                             alloc_info->resource_alloc_info.range_end - 1, _SHR_E_INTERNAL,
                             "The profile allocated: %d, is out of range(%d - %d)", *profile,
                             alloc_info->resource_alloc_info.range_start, alloc_info->resource_alloc_info.range_end);
        }
    }

    /**
     * If first reference, increment the tag of the grain containing this profile
     * This marks that the number of allocated profiles in this grain has increased by one
     */
    if (*first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_algo_template_mngr_even_distribution_set_cnt_tag
                        (unit, module_id, multi_set_template, *profile, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_algo_template_mngr_even_distribution_free(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Free allocation using smart template */
    SHR_IF_ERR_EXIT(dnx_algo_smart_template_free
                    (unit, module_id, multi_set_template, profile, nof_references, last_reference));

    /** If last reference, decrement the tag of the grain containing this profile */
    if (*last_reference)
    {
        SHR_IF_ERR_EXIT(dnx_algo_template_mngr_even_distribution_set_cnt_tag
                        (unit, module_id, multi_set_template, profile, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}
