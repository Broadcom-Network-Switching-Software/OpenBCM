/** 
 * \file multi_set_same_data.c
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set_same_data.h>

#ifdef BCM_DNX_SUPPORT

/*
 * Search for a eligible profile in the given range. allocation_needed is set to true is such was not found.
 */
static shr_error_e
multi_set_same_data_search_in_ll(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 ll_head_id,
    uint32 range_start,
    uint32 range_end,
    uint32 *profile,
    uint8 *allocation_needed)
{
    sw_state_ll_node_t node;
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    *allocation_needed = TRUE;

    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, multi_set->ll_list, ll_head_id, &node));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(sw_state_ll_node_value(unit, node_id, multi_set->ll_list, node, &value));
        if (value >= range_start && value < range_end && *profile >= value)
        {
            *profile = value;
            *allocation_needed = FALSE;
        }
        SHR_IF_ERR_EXIT(sw_state_ll_next_node(unit, node_id, multi_set->ll_list, node, &node));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Set ll head id by a node value(aka profile id)
 */
static shr_error_e
multi_set_same_data_set_head_id(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 profile,
    uint32 ll_head_id)
{
    uint8 value[sizeof(uint32)];
    uint32 _ll_head_id[1];

    SHR_FUNC_INIT_VARS(unit);
    _ll_head_id[0] = ll_head_id;

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(_ll_head_id, sizeof(uint32), value));

    DNX_SW_STATE_MEMWRITE(unit, node_id, value, multi_set->head_ids, profile * sizeof(uint32),
                          sizeof(uint32), 0, "multi_set head_ids.");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Returns ll head id by a node value(aka profile id)
 */
static shr_error_e
multi_set_same_data_get_head_id(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 profile,
    uint32 *ll_head_id)
{
    uint8 value[sizeof(uint32)];
    uint32 _ll_head_id[1];

    SHR_FUNC_INIT_VARS(unit);
    _ll_head_id[0] = -1;

    DNX_SW_STATE_MEMREAD(unit, value, multi_set->head_ids, profile * sizeof(uint32), sizeof(uint32), 0, "ref counter");

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(value, sizeof(uint32), _ll_head_id));
    *ll_head_id = _ll_head_id[0];

exit:
    SHR_FUNC_EXIT;
}

/*
 * Removing a ll node by its value (or a profile)
 */
static shr_error_e
multi_set_same_data_remove_node_by_id(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 ll_head_id,
    uint32 profile,
    uint8 *is_last)
{
    sw_state_ll_node_t node;
    uint32 value;
    uint32 nof_nodes = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_ll_nof_elements(unit, node_id, multi_set->ll_list, ll_head_id, &nof_nodes));
    if (nof_nodes == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trying to free a non existing ll node.\n");
    }
    else if (nof_nodes == 1)
    {
        /*
         * Signal that this is the last node in the ll. This means that the hash table entry for this data can be removed.
         */
        *is_last = TRUE;
    }
    else
    {
        /*
         * Do nothing with the hash table entry.
         */
        *is_last = FALSE;
    }

    /*
     * Search for the node and remove it.
     */
    SHR_IF_ERR_EXIT(sw_state_ll_get_first(unit, node_id, multi_set->ll_list, ll_head_id, &node));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(sw_state_ll_node_value(unit, node_id, multi_set->ll_list, node, &value));
        if (value == profile)
        {
            SHR_IF_ERR_EXIT(sw_state_ll_remove_node(unit, node_id, multi_set->ll_list, ll_head_id, node));
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(sw_state_ll_next_node(unit, node_id, multi_set->ll_list, node, &node));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Update the number of references for the allocated profile
 */
static shr_error_e
multi_set_same_data_update_nof_references(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 flags,
    uint32 profile_index,
    int nof_references,
    uint8 *first_appear,
    uint8 *success)
{

    uint8 tmp_cnt[sizeof(uint32)];
    uint32 ref_count[1], adjusted_additions, tmp_ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init local variables
     */
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));
    ref_count[0] = 0;
    adjusted_additions = 0;

    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * 'profile_index * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, profile_index * multi_set->counter_size,
                         multi_set->counter_size, 0, "multi_set ref_counter.");

    tmp_ref_count = 0;
    SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &tmp_ref_count));

    *ref_count = tmp_ref_count;
    /*
     * Set if maximum entries is required. 
     */
    if (nof_references == UTILEX_U32_MAX)
    {
        adjusted_additions = (multi_set->init_info.max_duplications - (*ref_count));
    }
    else
    {
        adjusted_additions = nof_references;
    }
    /*
     * Same value referenced more than initialized value times, return
     * operation fail.
     */
    if (((*ref_count) + adjusted_additions) >
        UTILEX_BITS_MASK((UTILEX_NOF_BITS_IN_BYTE * multi_set->counter_size - 1), 0))
    {
        /*
         * Overflowing the counter. 
         */
        *success = FALSE;
        SHR_EXIT();
    }
    if (((*ref_count) + adjusted_additions) > multi_set->init_info.max_duplications)
    {   /* Adding more entries than there are available */
        *success = FALSE;
        SHR_EXIT();
    }

    if (*ref_count == 0)
    {
        *first_appear = TRUE;
    }
    else
    {
        *first_appear = FALSE;
    }
    *ref_count += adjusted_additions;

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(ref_count, multi_set->counter_size, tmp_cnt));

    /*
     * Copy 'counter_size' bytes from 'tmp_cnt' buffer into 'multi_set->ref_counter'
     * buffer at offset 'profile_index * counter_size'.
     */
    DNX_SW_STATE_MEMWRITE(unit, node_id, tmp_cnt, multi_set->ref_counter, profile_index * multi_set->counter_size,
                          multi_set->counter_size, 0, "multi_set ref_counter.");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_same_data_create(
    int unit,
    uint32 node_id,
    multi_set_t * multi_set,
    dnx_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags)
{
    dnx_algo_res_create_data_t bitmap_create_info;
    sw_state_ll_init_info_t init_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create a multi_set.
     */
    SHR_IF_ERR_EXIT(multi_set_create(unit, node_id, multi_set, create_data, NULL, alloc_flags));

    /*
     * Create a res_tag_bitmap to manage the entries.
     */
    sal_memset(&bitmap_create_info, 0, sizeof(dnx_algo_res_create_data_t));
    sal_strncpy(bitmap_create_info.name, create_data->name, sizeof(bitmap_create_info.name));
    sal_strncpy(bitmap_create_info.advanced_algo_name, create_data->advanced_algo_name,
                sizeof(bitmap_create_info.advanced_algo_name));
    bitmap_create_info.first_element = create_data->first_profile;
    bitmap_create_info.nof_elements = create_data->nof_profiles;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, node_id, &((*multi_set)->allocation_bitmap),
                                               &bitmap_create_info, NULL, alloc_flags));

    /*
     * Create a linked list
     */
    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = create_data->nof_profiles;
    init_info.nof_heads = create_data->nof_profiles;

    SHR_IF_ERR_EXIT(sw_state_ll_create_empty(unit, node_id, &init_info, SW_STATE_LL_MULTIHEAD, 0,
                                             sizeof(uint32), &((*multi_set)->ll_list), 0, alloc_flags));

    /*
     * Allocate a buffer to store profile to ll head id mapping
     */
    DNX_SW_STATE_ALLOC(unit, node_id, (*multi_set)->head_ids, DNX_SW_STATE_BUFF,
                       create_data->nof_profiles * (sizeof(uint32)), alloc_flags, "sw_state multi_set head id map");
    DNX_SW_STATE_MEMSET(unit, node_id, (*multi_set)->head_ids, 0, 0x00, create_data->nof_profiles * (sizeof(uint32)), 0,
                        "sw_state multi_set head id map");

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_same_data_member_alloc(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 flags,
    int nof_references,
    void *data,
    void *extra_arguments,
    int *profile,
    uint8 *first_appear)
{
    uint32 ll_head_id;
    uint8 with_id = 0;
    uint8 success = TRUE;
    uint8 allocation_needed = FALSE;
    multi_set_same_data_alloc_info_t extra_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data, _SHR_E_INTERNAL, "key");
    SHR_NULL_CHECK(profile, _SHR_E_INTERNAL, "profile");
    SHR_NULL_CHECK(first_appear, _SHR_E_INTERNAL, "first_appear");

    /*
     * 'll_head_id' is loaded as indirect output below, so it does not need to be
     * initialized. However, Coverity requires initialization...
     */
    ll_head_id = 0;
    with_id = (_SHR_IS_FLAG_SET(flags, MULTI_SET_ADD_WITH_ID));
    sal_memset(&extra_alloc_info, 0, sizeof(extra_alloc_info));
    if (extra_arguments != NULL)
    {
        sal_memcpy(&extra_alloc_info, extra_arguments, sizeof(extra_alloc_info));
    }

    /*
     * Calibrate the ID because the multiset starts from 0 but the IDs the caller gives can start from "first_profile"
     * given in the init_info.
     */
    if (with_id)
    {
        *profile = *profile - multi_set->init_info.first_profile;
        extra_alloc_info.range_start = *profile;
        extra_alloc_info.range_end = *profile + 1;
    }
    else
    {
        /*
         * Set the profile to invalid number, so it will be updated only if we find eligible profile.
         */
        *profile = SAL_UINT32_MAX;
    }

    if (nof_references == 0)
    {
        success = FALSE;
        goto exit;
    }

    /*
     * Add an entry for the linked list head(if it does not exists)
     */
    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_index_htb_rh_find
                                    (unit, multi_set->hash_table, 0, (uint8 *) data, &ll_head_id), _SHR_E_NOT_FOUND);
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        /*
         * Add an entry to the hash table. The returned index will be used for linked list head id.
         */
        SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_index_htb_rh_insert
                                        (unit, multi_set->hash_table, 0, (uint8 *) data, &ll_head_id), _SHR_E_FULL);
        if (SHR_GET_CURRENT_ERR() == _SHR_E_FULL)
        {
            success = FALSE;
            ll_head_id = UTILEX_U32_MAX;
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        }
        allocation_needed = TRUE;
    }
    else
    {
        /*
         * If the data exists in the hash table. Search for a eligible profile in the given range.
         */
        SHR_IF_ERR_EXIT(multi_set_same_data_search_in_ll
                        (unit, node_id, multi_set, ll_head_id, extra_alloc_info.range_start, extra_alloc_info.range_end,
                         (uint32 *) profile, &allocation_needed));
    }

    if (allocation_needed == TRUE)
    {
        resource_tag_bitmap_utils_extra_arg_alloc_info_t alloc_info;
        uint32 resource_flags;
        sal_memset(&alloc_info, 0, sizeof(alloc_info));

        /*
         * We should have with_id or in_range allocations, not both.
         */
        if (with_id)
        {
            resource_flags = RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
        }
        else
        {
            resource_flags = RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
            alloc_info.range_start = extra_alloc_info.range_start;
            alloc_info.range_end = extra_alloc_info.range_end;
        }

        /*
         * Allocate a profile in the resource bitmap. Add the profile to the coresponding ll and update the profile to ll head id map.
         */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_allocate_single(unit, node_id, multi_set->allocation_bitmap,
                                                            resource_flags, &alloc_info, profile));

        SHR_IF_ERR_EXIT(sw_state_ll_add(unit, node_id, multi_set->ll_list, ll_head_id, NULL, profile));

        SHR_IF_ERR_EXIT(multi_set_same_data_set_head_id(unit, node_id, multi_set, *profile, ll_head_id));

    }

    /*
     * Apparently, if *success is FALSE then 'found_index' must be sw_state_index_hash_table_NULL
     * and the other way round.
     */
    if (!success)
    {
        /*
         * Enter if *success is FALSE
         *
         * If we were given the index, we assume the user allocated the 
         * entry before the call. If we weren't given the index, we
         * assume the operation failed because the table is full.
         */
        *first_appear = (with_id) ? FALSE : TRUE;
        goto exit;
    }
    /*
     * At this point, member is NOT a singleton and was successfully
     * added to the hashtable of the multiset. So we should update the ref_counter
     */
    SHR_IF_ERR_EXIT(multi_set_same_data_update_nof_references
                    (unit, node_id, multi_set, flags, *profile, nof_references, first_appear, &success));

exit:
    *profile = *profile + multi_set->init_info.first_profile;

    if (!success || SHR_GET_CURRENT_ERR() == _SHR_E_INTERNAL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FULL);
    }

    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_same_data_member_free(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    int profile,
    int nof_references,
    uint8 *last_appear)
{
    uint32 adjusted_nof_references = 0, ref_counter = 0;
    uint8 tmp_cnt[sizeof(uint32)];
    int _profile = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(last_appear, _SHR_E_INTERNAL, "last_appear");

    /*
     * calibrate the Data ID because the multiset starts from
     * ID 0 but the IDs the caller gives can start
     * from "first_profile" given in the init_info
     */
    _profile = profile - multi_set->init_info.first_profile;

    /*
     * Init local variables
     */
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));

    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * 'profile * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, _profile * multi_set->counter_size,
                         multi_set->counter_size, 0, "ref counter");

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &ref_counter));

    if (ref_counter == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trying to free a non existing profile.\n");
    }

    /*
     * Remove all entries
     */
    if ((nof_references > ref_counter))
    {
        adjusted_nof_references = ref_counter;
    }
    else
    {
        adjusted_nof_references = nof_references;
    }

    ref_counter -= adjusted_nof_references;
    *last_appear = (ref_counter == 0) ? TRUE : FALSE;
    SHR_IF_ERR_EXIT(utilex_U32_to_U8(&ref_counter, multi_set->counter_size, tmp_cnt));

    /*
     * Copy 'counter_size' bytes from 'tmp_cnt' buffer into 'multi_set->ref_counter'
     * buffer at offset 'profile * counter_size'.
     */

    DNX_SW_STATE_MEMWRITE(unit, node_id, tmp_cnt, multi_set->ref_counter, (_profile) * multi_set->counter_size,
                          multi_set->counter_size, 0, "multi_set ref_counter.");

    if (*last_appear)
    {
        uint8 is_last_node = FALSE;
        uint32 ll_head_id = -1;

        /*
         * Free the profile from the resource bitmap and the linked list.
         */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_free(unit, node_id, multi_set->allocation_bitmap, _profile, NULL));

        SHR_IF_ERR_EXIT(multi_set_same_data_get_head_id(unit, node_id, multi_set, _profile, &ll_head_id));
        SHR_IF_ERR_EXIT(multi_set_same_data_remove_node_by_id
                        (unit, node_id, multi_set, ll_head_id, _profile, &is_last_node));
        if (is_last_node == TRUE)
        {
            /*
             * If this was the last node in the linked list, then we need to remove the entry from the hash table.
             */
            SHR_IF_ERR_EXIT_EXCEPT_IF(sw_state_index_htb_rh_delete_by_index
                                      (unit, multi_set->hash_table, 0, ll_head_id), _SHR_E_NOT_FOUND);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_same_data_member_exchange(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 flags,
    int nof_references,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_appear,
    uint8 *last_appear)
{
    MULTI_SET_KEY *old_profile_data = NULL, *new_profile_old_data = NULL;
    int ref_count_old;
    int new_profile_old_ref_count;
    uint8 old_profile_freed = FALSE;
    uint8 add_success = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * First, save the old data, in order to restore it in case the allocation fails, or in case of test.
     */
    SHR_ALLOC_ERR_EXIT(old_profile_data, multi_set->hash_table->htb_rh->key_size,
                       "Data buffer old.", "%s%s%s", "old_profile_data", EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(multi_set_same_data_get_by_index
                    (unit, node_id, multi_set, old_profile, &ref_count_old, old_profile_data));

    if ((ref_count_old == 0) && !_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE))
    {
        /*
         * User gave an old profile that was empty.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given old_profile %d doesn't exist.", old_profile);
    }

    if (ref_count_old != 0)
    {
        /*
         * Remove nof_references from old profile.
         */
        SHR_IF_ERR_EXIT(multi_set_same_data_member_free
                        (unit, node_id, multi_set, old_profile, nof_references, last_appear));
        /*
         * Mark that nof_references were freed in case we want/need to revert it later.
         */
        old_profile_freed = TRUE;
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA))
    {
        SHR_ALLOC_ERR_EXIT(new_profile_old_data, multi_set->hash_table->htb_rh->key_size,
                           "Old data of new profile", "%s%s%s", "new_profile_old_data", EMPTY, EMPTY);
        /*
         * WITH_ID means that the user would like to exchange the current (old) profile for a specific new profile
         * (as opposed to changing the profile to any new profile containing the supplied data).
         * If IGNORE_DATA is not specified, then the user would also like to change the value of the new template.
         */
        if (_SHR_IS_FLAG_SET(flags, MULTI_SET_ADD_WITH_ID))
        {
            SHR_IF_ERR_EXIT(multi_set_same_data_get_by_index
                            (unit, node_id, multi_set, *new_profile, &new_profile_old_ref_count, new_profile_old_data));
        }
        /*
         * Allocate nof_references to the new profile.
         * If IGNORE_DATA flag is set, take the existing data.
         */
        SHR_IF_ERR_EXIT(multi_set_same_data_member_alloc
                        (unit, node_id, multi_set, flags, nof_references, new_profile_old_data,
                         (void *) extra_arguments, new_profile, first_appear));
        add_success = TRUE;
    }
    else
    {
        /*
         * Allocate nof_references to the new profile.
         */
        SHR_IF_ERR_EXIT(multi_set_same_data_member_alloc
                        (unit, node_id, multi_set, flags, nof_references, (void *) profile_data,
                         (void *) extra_arguments, new_profile, first_appear));

        add_success = TRUE;
    }

exit:
    if (SHR_FUNC_ERR() || !add_success)
    {
        int rv;
        /*
         * If the adding to the new profile failed, or in test, restore old data (unless the free itself failed).
         */
        if (!add_success && !((SHR_GET_CURRENT_ERR() == _SHR_E_PARAM) || (SHR_GET_CURRENT_ERR() == _SHR_E_INTERNAL)))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_FULL);
        }

        if (old_profile_freed)
        {
            uint8 dummy_first_appear;
            rv = multi_set_same_data_member_alloc(unit, node_id, multi_set, MULTI_SET_ADD_WITH_ID, nof_references,
                                                  old_profile_data, NULL, &old_profile, &dummy_first_appear);
            if (SHR_FAILURE(rv))
            {
                /*
                 * Can't restore data. Internal error
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            }
        }
    }

    /*
     * Free allocated pointers.
     */
    SHR_FREE(old_profile_data);
    SHR_FREE(new_profile_old_data);

    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_same_data_member_lookup(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    const void *key,
    int *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    *profile = DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE;
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_same_data_get_by_index(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    int profile,
    int *ref_count,
    void *data)
{
    uint32 tmp_ref_count;
    uint8 tmp_cnt[sizeof(uint32)];
    uint32 _profile;
    uint32 ll_head_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init local variables
     */
    tmp_ref_count = 0;
    _profile = 0;
    ll_head_id = -1;
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));

    /*
     * calibrate the Data ID because the multiset starts from
     * ID 0 but the IDs the caller gives can start
     * from "first_profile" given in the init_info
     */
    if (profile >= multi_set->init_info.first_profile)
    {
        _profile = (uint32) profile - multi_set->init_info.first_profile;
    }
    else
    {
        _profile = multi_set->init_info.first_profile;
    }

    SHR_IF_ERR_EXIT(multi_set_same_data_get_head_id(unit, node_id, multi_set, _profile, &ll_head_id));
    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_index_htb_rh_get_by_index
                                    (unit, multi_set->hash_table, 0, ll_head_id, (uint8 *) data), _SHR_E_NOT_FOUND);
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        if (ref_count != NULL)
        {
            *ref_count = 0;
        }
        goto exit;
    }

    tmp_ref_count = 0;
    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * '(*profile) * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, _profile * multi_set->counter_size,
                         multi_set->counter_size, 0, "ref counter");

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &tmp_ref_count));

    if (ref_count != NULL)
    {
        *ref_count = tmp_ref_count;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
multi_set_same_data_clear(
    int unit,
    uint32 node_id,
    multi_set_t multi_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Clear template, resource and linked list.
     */
    DNX_SW_STATE_MEMSET(unit, node_id, multi_set->head_ids, 0, 0x00,
                        multi_set->init_info.nof_members * (sizeof(uint32)), 0, "sw_state multi_set head id map");

    SHR_IF_ERR_EXIT(multi_set_clear(unit, node_id, multi_set));

    SHR_IF_ERR_EXIT(resource_tag_bitmap_clear(unit, node_id, multi_set->allocation_bitmap));

    SHR_IF_ERR_EXIT(sw_state_ll_clear(unit, node_id, multi_set->ll_list));

exit:
    SHR_FUNC_EXIT;
}

#else /* BCM_DNX_SUPPORT */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
multi_set_same_data_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif /* BCM_DNX_SUPPORT */
