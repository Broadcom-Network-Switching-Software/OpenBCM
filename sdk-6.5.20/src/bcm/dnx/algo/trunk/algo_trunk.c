/** \file algo/trunk/algo_trunk.c
 *
 * Implementation of trunk algos
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/**
* Include files:
* {
*/
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_trunk_access.h>

/** 
 * } 
 */

/**
* Functions:
* {
*/

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_smooth_division_profile_set(
    int unit,
    bcm_trunk_psc_profile_info_t * psc_info,
    int nof_enabled_members,
    int *psc_profile)
{
    SHR_FUNC_INIT_VARS(unit);
    if (nof_enabled_members > dnx_data_trunk.psc.smooth_division_max_nof_member_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Smooth Division PSC doesn't support above %d members\n",
                     dnx_data_trunk.psc.smooth_division_max_nof_member_get(unit));
    }
    *psc_profile = (nof_enabled_members == 0) ? 0 : nof_enabled_members - 1;
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_smooth_division_profile_get(
    int unit,
    int psc_profile,
    bcm_trunk_psc_profile_info_t * psc_info)
{
    SHR_FUNC_INIT_VARS(unit);
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "PSC Smooth Division doesn't have any psc_info\n")));
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ideal destribution according number of 
 *        members in profile and number of lb keys per profile -
 *        if for example we have 3 members and 27 lb-keys
 *        - each member will recieve 9 lb-keys, if on the other
 *        hand we have 5 members and 27 lb-keys - each
 *        member will recieve 5 lb-keys except of the first 2
 *        which will recieve 6. common value for nof_lb_key per
 *        profile is 256, however it might change in the future,
 *        the number of members in profile changes from profile
 *        to profile.
 * 
 * \param [in] unit - unit number
 * \param [in] nof_member_in_profile - number of members in the 
 *        smd profile for which i want to get the ideal
 *        destribution
 * \param [in] nof_lb_keys_per_profile - number of lb-keys 
 *        dedicated for each profile
 * \param [in] nof_lb_keys_per_member_array - array that will 
 *        contain how many lb-keys each member in a profile will
 *        get, the extra members of the array, if exists, will
 *        be zeroed
 * \param [in] size_of_nof_lb_keys_per_member_array - size of 
 *        the mentioned previous array
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * the size of the array must be greater or equal to the
 *     number of entries in profile
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_trunk_smd_ideal_destribution_get(
    int unit,
    int nof_member_in_profile,
    int nof_lb_keys_per_profile,
    int nof_lb_keys_per_member_array[],
    int size_of_nof_lb_keys_per_member_array)
{
    int member;
    int remainder;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(nof_lb_keys_per_member_array, 0,
               size_of_nof_lb_keys_per_member_array * sizeof(nof_lb_keys_per_member_array[0]));
    if (nof_member_in_profile > size_of_nof_lb_keys_per_member_array)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "size of nof_lb_keys_per_mmember array must be bigger than nof members in profile\n");
    }

    remainder = nof_lb_keys_per_profile % nof_member_in_profile;
    for (member = 0; member < nof_member_in_profile; ++member)
    {
        nof_lb_keys_per_member_array[member] = nof_lb_keys_per_profile / nof_member_in_profile;
        if (remainder != 0)
        {
            ++nof_lb_keys_per_member_array[member];
            --remainder;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get an array with indication which member should get 
 *        how many lb-keys on the acount of the member that is
 *        being removed.
 * 
 * \param [in] unit - unit nubmer
 * \param [in] array_size - size of the next arrays, all arrays 
 *        must have the same size
 * \param [in] original_nof_lb_keys_per_member_array - array to 
 *        indicate the original number of lb-keys each member
 *        has
 * \param [in] wanted_nof_lb_keys_per_member_array - array the 
 *        contains the desiered result in the sense of how many
 *        lb-keys each member should have
 * \param [in] removed_member - the member which is being 
 *        removed and its lb-keys are distributed between the
 *        others.
 * \param [in] nof_lb_keys_per_member_to_redistribute_array -the 
 *        reuslted array with the number of additional lb-keys
 *        each member should recieve.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_trunk_receiving_members_array_get(
    int unit,
    int array_size,
    int original_nof_lb_keys_per_member_array[],
    int wanted_nof_lb_keys_per_member_array[],
    int removed_member,
    int nof_lb_keys_per_member_to_redistribute_array[])
{
    int member;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(nof_lb_keys_per_member_to_redistribute_array, 0,
               array_size * sizeof(nof_lb_keys_per_member_to_redistribute_array[0]));
    if (removed_member >= array_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "removed member must be smaller then array size\n");
    }

    for (member = 0; member < array_size; ++member)
    {
        nof_lb_keys_per_member_to_redistribute_array[member] =
            wanted_nof_lb_keys_per_member_array[member] - original_nof_lb_keys_per_member_array[member];
    }
    nof_lb_keys_per_member_to_redistribute_array[removed_member] = 0;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_smd_pre_defined_profiles_get(
    int unit,
    int *profile_array,
    int nof_profiles,
    int nof_lb_keys_per_profile)
{
    int lb_key;
    int last_profile;
    int member_currently_recieving_lb_keys;
    int removed_member;
    int nof_members_in_profile;
    int previous_profile;
    int profile;
    int *ideal_destribution = NULL;
    int *previous_destribution = NULL;
    int *destribution_delta = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (nof_lb_keys_per_profile % nof_profiles != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "this function works only under the assumption that an even "
                     "destribution can be achieved for the last profile\n");
    }

    /** create the last profile - corresponding to profile with max number of different unique members */
    last_profile = nof_profiles - 1;
    for (lb_key = 0; lb_key < nof_lb_keys_per_profile; ++lb_key)
    {
        profile_array[last_profile * nof_lb_keys_per_profile + lb_key] = lb_key / nof_profiles;
    }

    /** allocate arrays to manage destribution of members to profiles */
    /** ideal_destribution contians the ideal number of each member in the profile */
    ideal_destribution = sal_alloc(sizeof(*ideal_destribution) * nof_profiles, "ideal_destribution array");
    /** previous_destribution contains the destribution of the previous profile */
    previous_destribution = sal_alloc(sizeof(*previous_destribution) * nof_profiles, "previous_destribution array");
    /** destribution_delta contains the difference between the ideal and previous destribution after spliting the
     *  removed member between all the others */
    destribution_delta = sal_alloc(sizeof(*destribution_delta) * nof_profiles, "destribution_delta array");
    if (ideal_destribution == NULL || previous_destribution == NULL || destribution_delta == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "allocation_failed");
    }

    /** get destribution of the last profile - it should always be ideal because of the way it was initialized */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_smd_ideal_destribution_get(unit, nof_profiles, nof_lb_keys_per_profile,
                                                              previous_destribution, nof_profiles));

    for (profile = last_profile - 1; profile >= 0; --profile)
    {
        removed_member = profile + 1;
        nof_members_in_profile = profile + 1;
        previous_profile = profile + 1;
        /** get wanted destribution */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_smd_ideal_destribution_get(unit, nof_members_in_profile, nof_lb_keys_per_profile,
                                                                  ideal_destribution, nof_profiles));
        /** get which members should receive from removed member and how many */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_receiving_members_array_get(unit, nof_profiles, previous_destribution,
                                                                   ideal_destribution, removed_member,
                                                                   destribution_delta));
        /** set profile according to previous one, destribute removed member entries between other members according to
         *  destribution delta */
        member_currently_recieving_lb_keys = 0;
        for (lb_key = 0; lb_key < nof_lb_keys_per_profile; ++lb_key)
        {
            if (profile_array[previous_profile * nof_lb_keys_per_profile + lb_key] == removed_member)
            {
                while (destribution_delta[member_currently_recieving_lb_keys] == 0)
                {
                    ++member_currently_recieving_lb_keys;
                }
                --destribution_delta[member_currently_recieving_lb_keys];
                profile_array[profile * nof_lb_keys_per_profile + lb_key] = member_currently_recieving_lb_keys;
            }
            else
            {
                profile_array[profile * nof_lb_keys_per_profile + lb_key]
                    = profile_array[previous_profile * nof_lb_keys_per_profile + lb_key];
            }
        }

        /** copy ideal destribution to previous destribution to be used in next iteration */
        sal_memcpy(previous_destribution, ideal_destribution, sizeof(int) * nof_profiles);
    }

exit:
    SHR_FREE(ideal_destribution);
    SHR_FREE(previous_destribution);
    SHR_FREE(destribution_delta);
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_egress_trunk_init(
    int unit)
{
    dnx_algo_template_create_data_t create_data;
    SHR_FUNC_INIT_VARS(unit);

    /** create data */
    sal_memset(&create_data, 0, sizeof(dnx_algo_template_create_data_t));
    create_data.flags = 0;
    create_data.first_profile = 0;
    create_data.nof_profiles = dnx_data_trunk.egress_trunk.nof_get(unit);
    create_data.max_references = SAL_INT32_MAX;
    /** default profile is not expected to be used - so an invalid value was provided */
    create_data.default_profile = -1;
    create_data.data_size = sizeof(bcm_trunk_t);
    create_data.default_data = NULL;
    sal_strncpy(create_data.name, DNX_ALGO_TRUNK_EGRESSTRUNK_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_egress_trunk_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_create(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id,
    uint8 *is_first)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.allocate_single
                    (unit, 0, (void *) &trunk_id, NULL, egress_trunk_id, is_first));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_destroy(
    int unit,
    int egress_trunk_id,
    uint8 *is_last)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_last, _SHR_E_PARAM, "is_last");

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.free_single(unit, egress_trunk_id, is_last));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_get(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id,
    int *profile_found)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(profile_found, _SHR_E_PARAM, "profile_found");
    SHR_NULL_CHECK(egress_trunk_id, _SHR_E_PARAM, "egress_trunk_id");

    rv = algo_trunk_db.egress_trunk.profile_get(unit, (void *) &trunk_id, egress_trunk_id);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *profile_found = 0;
    }
    else
    {
        /** either error or profile found */
        SHR_IF_ERR_EXIT(rv);
        *profile_found = 1;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_containing_egress_trunk_get(
    int unit,
    int egress_trunk_id,
    bcm_trunk_t * trunk_id)
{
    int ref_count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_trunk_db.egress_trunk.profile_data_get(unit, egress_trunk_id, &ref_count, (void *) trunk_id));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
void
dnx_algo_trunk_egresstrunk_profile_print_cb(
    int unit,
    const void *data)
{
    bcm_trunk_t *trunk_id = (bcm_trunk_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "trunk_id", *trunk_id, "trunk id", "0x%x");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * see header file
 */
shr_error_e
dnx_algo_trunk_egress_trunk_lb_key_destribution_get(
    int unit,
    int profile_lb_key_range_size,
    int profile_first_lb_key_value,
    int trunk_nof_enabled_members,
    int unique_members_array_size,
    dnx_trunk_unique_member_t * unique_members_array,
    int *lb_key_ranges_array_size,
    dnx_algo_trunk_member_lb_key_ranges_t * lb_key_ranges_array)
{
    int range;
    int range_per_member;
    int remaining;
    int member_id;
    SHR_FUNC_INIT_VARS(unit);

    *lb_key_ranges_array_size = 0;

    /** if trunk has no enabled members */
    if (trunk_nof_enabled_members == 0)
    {
        /** set array with 1 member, use invalid system port and give it the whole range */
        *lb_key_ranges_array_size = 1;
        lb_key_ranges_array[0].first_lb_key = profile_first_lb_key_value;
        lb_key_ranges_array[0].lb_key_range = profile_lb_key_range_size;
        BCM_GPORT_SYSTEM_PORT_ID_SET(lb_key_ranges_array[0].system_port,
                                     dnx_data_device.general.invalid_system_port_get(unit));
        SHR_EXIT();
    }

    /** calculate range per member and remaining */
    range_per_member = profile_lb_key_range_size / trunk_nof_enabled_members;
    remaining = profile_lb_key_range_size % trunk_nof_enabled_members;

    for (int unique_member = 0; unique_member < unique_members_array_size; ++unique_member)
    {
        /** Calculation of Member ID - in order to be aligned with Legacy,
         * the unique_members_array need so be accessed in reversed order.
         */
        member_id = (unique_members_array_size - unique_member) - 1;

        if (unique_members_array[member_id].is_enabled == FALSE)
        {
            continue;
        }

        /** get range of lb-keys to update for each member  */
        range = range_per_member * unique_members_array[member_id].nof_replications;
        if (remaining > unique_members_array[member_id].nof_replications)
        {
            range += unique_members_array[member_id].nof_replications;
            remaining -= unique_members_array[member_id].nof_replications;
        }
        else
        {
            range += remaining;
            remaining = 0;
        }

        /** set result array */
        lb_key_ranges_array[*lb_key_ranges_array_size].first_lb_key = profile_first_lb_key_value;
        lb_key_ranges_array[*lb_key_ranges_array_size].lb_key_range = range;
        lb_key_ranges_array[*lb_key_ranges_array_size].system_port = unique_members_array[member_id].system_port;

        /** prepare for next iteration */
        profile_first_lb_key_value += range;
        ++(*lb_key_ranges_array_size);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * }
 */
