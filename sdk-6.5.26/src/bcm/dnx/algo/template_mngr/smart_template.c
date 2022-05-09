/** \file smart_template.c
 *
 * A template manager algorithm that is managed by a resource manager.
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

/** } **/

/**
 *  Smart template advanced algorithm.
 *
 *  The algorithm works as following:
 *
 *  We hold a res tag bitmap that has the same ID range as the template.
 *  When allocating an entry for the first time, we allocate it in the res tag bitmap, since it allows
 *   us to allocate according to specific user requirements, like ranged allocation, tagging, allocating
 *   several indexes per entry, etc., functionalities that don't exist in template manager.
 *   Then we use the ID allocated in the resource manager to save the entry in the template manager.
 *
 *  {
 */

shr_error_e
dnx_algo_smart_template_create(
    int unit,
    uint32 node_id,
    multi_set_t * multi_set_template,
    dnx_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags)
{
    dnx_algo_res_create_data_t bitmap_create_info;
    smart_template_create_info_t *extra_create_info = (smart_template_create_info_t *) extra_arguments;
    uint32 res_flags;
    resource_tag_bitmap_extra_arguments_create_info_t *resource_extra_create_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * 1. Create a regular multi_set_template.
     */
    SHR_IF_ERR_EXIT(multi_set_create(unit, node_id, multi_set_template, create_data, NULL, alloc_flags));

    /*
     * 2. Create a res_tag_bitmap to manage the entries.
     *    Use the res tag bitmap interface because it already holds BCM flags for the
     *    inner algorithm.
     */
    sal_memset(&bitmap_create_info, 0, sizeof(dnx_algo_res_create_data_t));
    sal_strncpy(bitmap_create_info.name, create_data->name, sizeof(bitmap_create_info.name));
    sal_strncpy(bitmap_create_info.advanced_algo_name, create_data->advanced_algo_name,
                sizeof(bitmap_create_info.advanced_algo_name));
    bitmap_create_info.first_element = create_data->first_profile;
    bitmap_create_info.nof_elements = create_data->nof_profiles;

    if (extra_create_info != NULL)
    {
        res_flags = extra_create_info->resource_flags;
        resource_extra_create_info = &extra_create_info->resource_create_info;
        bitmap_create_info.flags = res_flags;
    }
    else
    {
        res_flags = 0;
        resource_extra_create_info = NULL;
    }

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, node_id, &((*multi_set_template)->allocation_bitmap),
                                               &bitmap_create_info,
                                               (resource_tag_bitmap_extra_arguments_create_info_t *)
                                               resource_extra_create_info, res_flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_allocate(
    int unit,
    uint32 node_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    int tmp_profile, rv;
    uint8 allocation_needed = TRUE, with_id;
    smart_template_alloc_info_t *extra_alloc_info;
    uint32 resource_flags;
    resource_tag_bitmap_extra_arguments_alloc_info_t *resource_alloc_info, resource_alloc_info_struct;
    SHR_FUNC_INIT_VARS(unit);

    with_id = (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID));

    extra_alloc_info = (smart_template_alloc_info_t *) extra_arguments;
    if (extra_alloc_info != NULL)
    {
        resource_flags = extra_alloc_info->resource_flags;
        resource_alloc_info = &extra_alloc_info->resource_alloc_info;
    }
    else
    {
        resource_alloc_info = NULL;
        resource_flags = 0;
    }

    /*
     * 1. Check if the data already exists or if it needs to be allocated.
     */
    rv = multi_set_member_lookup(unit, node_id, multi_set_template, profile_data, &tmp_profile);

    /*
     * If E_NOT_FOUND was returned, it means the data wasn't allocated yet and we need to allocate it.
     * Otherwise, check for errors, and if none were returned, it means the data is already allocated.
     * If WITH_ID, make sure the existing profile match the given profile.
     * Otherwise, if some allocation criteria was given, verify that the existing profile matches these
     *  criteria.
     * If no errors were found, save the profile we found and use it with the multi set template.
     */
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
        if (with_id && (*profile != tmp_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "This data already exists in profile %d. Given profile %d", tmp_profile,
                         *profile);
        }

        if (resource_alloc_info != NULL)
        {
            /*
             * We want to verify that the allocation info given matches the actual allocated entry.
             * Run an allocation simulation WITH_ID.
             * This will trigger all the bitmap input verification, and will fail if the input info doesn't match
             * the given entry.
             * If all the verification succeeds, we'll get E_RESOURCE.
             */
            uint32 tmp_flags = resource_flags | DNX_ALGO_RES_ALLOCATE_WITH_ID | DNX_ALGO_RES_ALLOCATE_SIMULATION;
            rv = resource_tag_bitmap_allocate_single(unit, node_id, multi_set_template->allocation_bitmap,
                                                     tmp_flags, resource_alloc_info, &tmp_profile);

            if (rv != _SHR_E_RESOURCE)
            {
                SHR_ERR_EXIT(rv, "The given data is already allocated on profile %d, and the given"
                             "allocation information (in extra_arguments) doesn't match the existing entry.",
                             tmp_profile);
            }
        }

        allocation_needed = FALSE;
        *profile = tmp_profile;
    }

    /*
     * 2. If data doesn't already exist, allocate in the res tag bitmap.
     */
    if (allocation_needed)
    {
        /*
         * bitmap doesn't take into consideration the 'allocate with id' option but,
         *    takes into consideration 'allocate in range'
         * so this is a solution using range of 1 to implement the 'allocate with id' option
         */
        if (with_id)
        {
            resource_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
            if (resource_alloc_info == NULL)
            {
                sal_memset(&resource_alloc_info_struct, 0, sizeof(resource_tag_bitmap_extra_arguments_alloc_info_t));
                resource_alloc_info = &resource_alloc_info_struct;
            }
            resource_alloc_info->range_start = *profile;
            resource_alloc_info->range_end = *profile + 1;
        }
        SHR_IF_ERR_EXIT(resource_tag_bitmap_allocate_single(unit, node_id, multi_set_template->allocation_bitmap,
                                                            resource_flags, resource_alloc_info, &tmp_profile));
        *profile = tmp_profile;
    }

    /*
     * 3. Use the allocated profile for the multi set template WITH_ID.
     */
    SHR_IF_ERR_EXIT(multi_set_member_add
                    (unit, node_id, multi_set_template, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, nof_references,
                     profile_data, NULL, profile, first_reference));

    /*
     * Sanity check: allocation_needed and first_reference should be the same.
     */
    if (allocation_needed != *first_reference)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Algorithm inconsistency. allocation_needed: %d, first_reference: %d",
                     allocation_needed, *first_reference);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_free(
    int unit,
    uint32 node_id,
    multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free from the multi set template.
     */
    SHR_IF_ERR_EXIT(multi_set_member_remove_by_index_multiple
                    (unit, node_id, multi_set_template, profile, nof_references, last_reference));

    /*
     * If last reference, free from the res tag bitmap as well.
     */
    if (*last_reference)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_free(unit, node_id, multi_set_template->allocation_bitmap, profile, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_exchange(
    int unit,
    uint32 node_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{

    MULTI_SET_KEY *old_profile_data = NULL, *new_profile_old_data = NULL;
    int ref_count_old;
    int new_profile_old_ref_count;
    uint8 old_profile_freed = FALSE;
    uint8 add_success = FALSE;
    uint32 nof_refs = 1;
    smart_template_alloc_info_t *extra_alloc_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * First, we save the old data, to restore it in case the allocation fails, or in case of test.
     */
    SHR_ALLOC_ERR_EXIT(old_profile_data, multi_set_template->hash_table->key_size,
                       "Data buffer old.", "%s%s%s", "old_profile_data", EMPTY, EMPTY);

    extra_alloc_info = (smart_template_alloc_info_t *) extra_arguments;

    if ((extra_alloc_info != NULL) && (extra_alloc_info->nof_refs > 0))
    {
        nof_refs = extra_alloc_info->nof_refs;
    }

    SHR_IF_ERR_EXIT(multi_set_get_by_index
                    (unit, node_id, multi_set_template, old_profile, &ref_count_old, old_profile_data));

    if ((ref_count_old == 0) && !_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
    {
        /*
         * User gave old profile that was empty
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given old_profile %d doesn't exist.", old_profile);
    }

    if (ref_count_old != 0)
    {
        /*
         *  Remove one ref from old profile.
         */
        SHR_IF_ERR_EXIT(dnx_algo_smart_template_free
                        (unit, node_id, multi_set_template, old_profile, nof_refs, last_reference));
        /*
         *  Mark that one ref was freed in case we want to revert it later.
         */
        old_profile_freed = TRUE;
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
    {
        SHR_ALLOC_ERR_EXIT(new_profile_old_data, multi_set_template->hash_table->key_size,
                           "Old data of new profile", "%s%s%s", "new_profile_old_data", EMPTY, EMPTY);
        /*
         * WITH_ID means that the user would like to exchange the current profile for a specific new profile
         * (as opposed to changing the profile to any new profile containing the supplied data).
         * If IGNORE_DATA is not specified, then the user would also like to change the value of the new template.
         */
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID))
        {
            SHR_IF_ERR_EXIT(multi_set_get_by_index
                            (unit, node_id, multi_set_template, *new_profile, &new_profile_old_ref_count,
                             new_profile_old_data));
        }
        /*
         *  If IGNORE_DATA flag is set, take the existing data.
         */
        SHR_IF_ERR_EXIT(dnx_algo_smart_template_allocate
                        (unit, node_id, multi_set_template, flags, nof_refs,
                         new_profile_old_data, (void *) extra_arguments, new_profile, first_reference));
        add_success = TRUE;
    }
    else
    {
        /*
         *  In this block we perform the allocation of the ref.
         */
        SHR_IF_ERR_EXIT(dnx_algo_smart_template_allocate
                        (unit, node_id, multi_set_template, flags, nof_refs, (void *) profile_data,
                         (void *) extra_arguments, new_profile, first_reference));

        add_success = TRUE;
    }

exit:
    if (SHR_FUNC_ERR() || !add_success)
    {
        int rv;
        /*
         *  Add new failed, or test. Restore old data.
         */
        if (!add_success && !((SHR_GET_CURRENT_ERR() == _SHR_E_PARAM) || (SHR_GET_CURRENT_ERR() == _SHR_E_INTERNAL)))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_FULL);
        }

        if (old_profile_freed)
        {
            uint8 dummy_first_reference;

            rv = dnx_algo_smart_template_allocate(unit, node_id, multi_set_template,
                                                  DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, nof_refs, old_profile_data, NULL,
                                                  &old_profile, &dummy_first_reference);
            if (SHR_FAILURE(rv))
            {
                /*
                 *  Can't restore data. Internal error
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            }
        }
    }
    /*
     * Free pointers.
     */
    SHR_FREE(old_profile_data);
    SHR_FREE(new_profile_old_data);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_clear(
    int unit,
    uint32 node_id,
    multi_set_t multi_set_template)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Clear template and resource.
     */
    SHR_IF_ERR_EXIT(multi_set_clear(unit, node_id, multi_set_template));

    SHR_IF_ERR_EXIT(resource_tag_bitmap_clear(unit, node_id, multi_set_template->allocation_bitmap));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_tag_set(
    int unit,
    uint32 node_id,
    multi_set_t multi_set_template,
    resource_tag_bitmap_tag_info_t * algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(algorithm_info, _SHR_E_PARAM, "algorithm_info");

    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, node_id, multi_set_template->allocation_bitmap, algorithm_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_smart_template_tag_get(
    int unit,
    uint32 node_id,
    multi_set_t multi_set_template,
    resource_tag_bitmap_tag_info_t * algorithm_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(algorithm_info, _SHR_E_PARAM, "algorithm_info");

    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, node_id, multi_set_template->allocation_bitmap, algorithm_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
