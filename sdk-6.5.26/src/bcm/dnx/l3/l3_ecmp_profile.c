/** \file l3_ecmp_profile.c
 *
 * Handles the control over the ECMP profile entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files.
 * {
 */

#include <bcm/types.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <shared/utilex/utilex_bitstream.h>

/*
 * }
 */

/**
 * Macros
 * {
 */
/** Get the consistent hashing mode - small, medium or large - based on the provided BCM_L3_ECMP_* flags */
#define DNX_L3_ECMP_CONSISTENT_HASHING_MODE_GET(unit, flags) ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_LARGE_TABLE)) ? DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE : \
            (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_MEDIUM_TABLE) ? DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE : DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE))

/*
 * }
 */

/**
 * \brief
 *  Builds the ecmp_profile structure using the data in the ecmp structure.
 * \param [in] unit - The unit number.
 * \param [in] ecmp - An input structure that holds the data for the UDEP.
 * \param [out] ecmp_profile - ECMP user profile data
 * \param [in] intf_count - number of members in the offsets array.
 * \param [in] intf_array -
 *      An array of offsets from the base
 * \return
 *   \retval None
 */
static shr_error_e
dnx_l3_egress_ecmp_user_profile_build_struct(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    dnx_l3_ecmp_user_profile_t * ecmp_profile,
    int intf_count,
    bcm_if_t * intf_array)
{
    int nof_unique_members = 0;
    uint32 exists_members_bmp[BITS2WORDS(DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE)];
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(ecmp_profile, 0, sizeof(dnx_l3_ecmp_user_profile_t));

    ecmp_profile->ecmp_mode = DNX_L3_ECMP_CONSISTENT_HASHING_MODE_GET(unit, ecmp->ecmp_group_flags);
    ecmp_profile->rpf_mode = L3_ECMP_RPF_MODE_BCM_ENUM_TO_DBAL_ENUM(ecmp->rpf_mode);
    ecmp_profile->hierarchy = L3_ECMP_GET_HIERARCHY(ecmp->flags);

    /** Tunnel priority */
    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        ecmp_profile->tunnel_priority_map_profile = (uint8) ecmp->tunnel_priority.map_profile;
        ecmp_profile->tunnel_priority_mode = ecmp->tunnel_priority.mode;
    }
    ecmp_profile->chm_alloc_state = 0;
    SHR_IF_ERR_EXIT(utilex_bitstream_clear(exists_members_bmp, BITS2WORDS(dnx_data_l3.ecmp.max_group_size_get(unit))));

    for (int idx = 0; idx < intf_count; idx++)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(exists_members_bmp, intf_array[idx]));
    }
    for (int idx = 0; idx < intf_count; idx++)
    {
        if (utilex_bitstream_test_bit(exists_members_bmp, idx))
        {
            nof_unique_members++;
        }
    }
    ecmp_profile->group_size = nof_unique_members;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configures the consistent hashing member table for the given profile.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_profile - A structure that holds the data for the UDEP.
 * \param [in] intf_count - number of members in the offsets array.
 * \param [in] intf_array - An array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egress_ecmp_consistent_hashing_table_set(
    int unit,
    dnx_l3_ecmp_user_profile_t * ecmp_profile,
    int intf_count,
    bcm_if_t * intf_array)
{
    consistent_hashing_calendar_t calendar;
    uint32 table_offset;
    int member;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&calendar, 0, sizeof(consistent_hashing_calendar_t));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                    (unit, ecmp_profile->ecmp_mode, &consistent_table_type));
    table_offset =
        ecmp_profile->consistent_members_table_addr * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                             consistent_table_type);
    for (member = 0; member < intf_count; member++)
    {
        calendar.lb_key_member_array[member] = intf_array[member];
    }
    calendar.profile_type = ecmp_profile->ecmp_mode;
    SHR_IF_ERR_EXIT(ecmp_chm_members_table_set(unit, table_offset, &calendar));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Gets the configured consistent hashing members for the given profile.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_profile - A structure that holds the data for the UDEP.
 * \param [out] intf_count - number of members in the offsets array.
 * \param [out] intf_array - An array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_l3_egress_ecmp_consistent_hashing_table_get(
    int unit,
    dnx_l3_ecmp_user_profile_t * ecmp_profile,
    int *intf_count,
    bcm_if_t * intf_array)
{
    consistent_hashing_calendar_t calendar;
    uint32 table_offset;
    int member;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&calendar, 0, sizeof(consistent_hashing_calendar_t));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                    (unit, ecmp_profile->ecmp_mode, &consistent_table_type));
    table_offset =
        ecmp_profile->consistent_members_table_addr * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                             consistent_table_type);

    calendar.profile_type = ecmp_profile->ecmp_mode;
    SHR_IF_ERR_EXIT(ecmp_chm_members_table_get(unit, table_offset, &calendar));
    for (member = 0; member < dnx_data_l3.ecmp.consistent_tables_info_get(unit, calendar.profile_type)->nof_entries;
         member++)
    {
        intf_array[member] = calendar.lb_key_member_array[member];
    }
    *intf_count = dnx_data_l3.ecmp.consistent_tables_info_get(unit, calendar.profile_type)->nof_entries;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Gets the configured profile data and offsets array based on the profile ID.
 * \param [in] unit - The unit number.
 * \param [in,out] ecmp - An input structure that holds the data for the UDEP.
 * \param [in] intf_size - allocated number of members in the intf_array
 * \param [out] intf_count - number of members in the offsets array.
 * \param [out] intf_array - An array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   bcm_dnx_l3_egress_ecmp_user_profile_get
 */
static shr_error_e
dnx_l3_egress_ecmp_from_user_profile_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    int *intf_count,
    bcm_if_t * intf_array)
{
    dnx_l3_ecmp_user_profile_t ecmp_profile_data;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_data_get(unit, ecmp->user_profile, &ecmp_profile_data));

    if (ecmp_profile_data.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_LARGE_TABLE)
    {
        ecmp->ecmp_group_flags |= BCM_L3_ECMP_LARGE_TABLE;
    }
    else if (ecmp_profile_data.ecmp_mode == DBAL_ENUM_FVAL_ECMP_MODE_CONSISTENT_MEDIUM_TABLE)
    {
        ecmp->ecmp_group_flags |= BCM_L3_ECMP_MEDIUM_TABLE;
    }
    ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;

    ecmp->rpf_mode = L3_ECMP_RPF_MODE_DBAL_ENUM_TO_BCM_ENUM(ecmp_profile_data.rpf_mode);

    ecmp->flags |= L3_EGRESS_ECMP_GET_HIER_FLAGS(ecmp_profile_data.hierarchy);
    *intf_count = dnx_data_l3.ecmp.consistent_tables_info_get(unit, ecmp_profile_data.ecmp_mode)->nof_entries;
    /** Tunnel priority */
    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        ecmp->tunnel_priority.map_profile = ecmp_profile_data.tunnel_priority_map_profile;
        ecmp->tunnel_priority.mode = ecmp_profile_data.tunnel_priority_mode;
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_table_get(unit, &ecmp_profile_data, intf_count, intf_array));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Update the offset table for a profile based on given input array.
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - the index of the custom profile
 * \param [in] ecmp_profile - a structure that holds the profile data.
 * \param [in] intf_count - number of members in the offsets array.
 * \param [in] intf_array - new array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 * bcm_dnx_l3_egress_ecmp_user_profile_create + BCM_L3_REPLACE
 */
static shr_error_e
dnx_l3_egress_ecmp_user_profile_update(
    int unit,
    int user_profile_id,
    dnx_l3_ecmp_user_profile_t ecmp_profile,
    int intf_count,
    bcm_if_t * intf_array)
{
    dnx_l3_ecmp_user_profile_t ecmp_profile_old;
    bcm_if_t intf_array_empty[L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES];
    uint32 user_info = 0;
    uint32 consistent_hashing_manager_handle;
    uint32 old_offset;
    uint32 new_offset;
    uint32 new_profile_key;
    uint32 old_profile_key;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(intf_array_empty, 0, intf_count * sizeof(bcm_if_t));
    /** Overwrite input ecmp_profile */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_data_get(unit, user_profile_id, &ecmp_profile));
    sal_memcpy(&ecmp_profile_old, &ecmp_profile, sizeof(dnx_l3_ecmp_user_profile_t));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                    (unit, ecmp_profile.ecmp_mode, &consistent_table_type));
    old_offset =
        ecmp_profile_old.consistent_members_table_addr * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                                consistent_table_type);
    old_profile_key =
        L3_ECMP_CHM_KEY_GET(unit, user_profile_id, ecmp_profile.hierarchy,
                            DNX_L3_ECMP_UDEP_ALLOC_STATE_APPLY(unit, ecmp_profile.chm_alloc_state));

    ecmp_profile.chm_alloc_state = 1 - ecmp_profile.chm_alloc_state;
    /** Allocate an address in the members table and update the profile */
    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
    /** Configure the new profile key with the updated chm_alloc_state */
    new_profile_key =
        L3_ECMP_CHM_KEY_GET(unit, user_profile_id, ecmp_profile.hierarchy,
                            DNX_L3_ECMP_UDEP_ALLOC_STATE_APPLY(unit, ecmp_profile.chm_alloc_state));
    /** Allocate new table with a special CHM key */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc(unit, consistent_hashing_manager_handle,
                                                                    /** Update user profile id with encoding */
                                                                      new_profile_key,
                                                                      ecmp_profile.hierarchy, ecmp_profile.ecmp_mode,
                                                                      1 /** nof consecutive tables*/ ,
                                                                      0 /** TP index */ ,
                                                                      2 /** max_paths */ , 1 /** intf_count */ , 0,
                                                                      NULL, (void *) &user_info));
    ecmp_profile.consistent_members_table_addr = user_info;
    new_offset =
        ecmp_profile.consistent_members_table_addr * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                            consistent_table_type);
    /** Update the member group with the offsets array provided by the user*/
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_table_set(unit, &ecmp_profile, intf_count, intf_array));
    /** Free the old CHM profile */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, consistent_hashing_manager_handle,
                                                                     old_profile_key, ecmp_profile.ecmp_mode));
    /** Replace ECMP profile data with the new member table address */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_allocate(unit, &user_profile_id, &ecmp_profile));

    {
        sw_state_ll_node_t node;
        sw_state_ll_node_t *node_ptr;
        uint32 profile_key, profile_key_temp;

        /*
         * Read the profile that need to be updated
         */

        node_ptr = &node;

        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, old_offset, node_ptr));

        while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
        {
            /*
             * Read the profile that need to be updated
             */
            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.node_value(unit, *node_ptr, &profile_key));
            if (profile_key == old_profile_key)
            {
                profile_key_temp = new_profile_key;
            }
            else
            {
                profile_key_temp = profile_key;
            }
            /*
             * Update the profile in the HW with the new address
             */
            SHR_IF_ERR_EXIT(update_members_address_in_ecmp_profile(unit, profile_key_temp, new_offset));
            /*
             * Add the new address resource the update profile
             */
            SHR_IF_ERR_EXIT(ecmp_db_info.
                            members_tbl_addr_profile_link_lists.add_last(unit, new_offset, &profile_key_temp));

            /*
             * Remove the profile from the old resource
             */
            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.remove_node(unit, old_offset, *node_ptr));
            /*
             * Take the next profile from the old resource list if exists for update with the new address
             */
            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, old_offset, node_ptr));
        }
    }

    /** Set the CHM hardware memory to zeros */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_table_set
                    (unit, &ecmp_profile_old, intf_count, intf_array_empty));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the input to the bcm_dnx_l3_egress_ecmp_user_profile_get API
 * \param [in] unit - The unit number.
 * \param [in] ecmp - An input structure that holds the data for the UDEP.
 * \param [in] intf_size - allocated number of members in the intf_array
 * \param [in] intf_count - number of members in the offsets array.
 * \param [in] intf_array - An array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 * bcm_dnx_l3_egress_ecmp_user_profile_get
 */
static shr_error_e
dnx_l3_egress_ecmp_user_profile_get_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    dnx_l3_ecmp_user_profile_t ecmp_profile_data;
    uint8 is_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");
    SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "interface array");
    SHR_NULL_CHECK(intf_count, _SHR_E_PARAM, "interface array count");

    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_data_get(unit, ecmp->user_profile, &ecmp_profile_data));
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_is_allocated(unit, ecmp->user_profile, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "User profile %d has not been configured.\n", ecmp->user_profile);
    }

    if (intf_size < dnx_data_l3.ecmp.consistent_tables_info_get(unit, ecmp_profile_data.ecmp_mode)->nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Array size is too small - given array has %d elements but need to return %d elements.", intf_size,
                     dnx_data_l3.ecmp.consistent_tables_info_get(unit, ecmp_profile_data.ecmp_mode)->nof_entries);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the input to the bcm_dnx_l3_egress_ecmp_user_profile_create API
 * \param [in] unit - The unit number.
 * \param [in] ecmp - An input structure that holds the data for the UDEP
 * \param [in] intf_count - number of members in the offsets array.
 * \param [in] intf_array - An array of offsets
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 * bcm_dnx_l3_egress_ecmp_user_profile_create
 */
static shr_error_e
dnx_l3_egress_ecmp_user_profile_create_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint8 is_allocated;
    dbal_enum_value_field_ecmp_mode_e ecmp_mode;
    uint32 max_offset;
    int idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");
    SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "interface array");

    ecmp_mode = DNX_L3_ECMP_CONSISTENT_HASHING_MODE_GET(unit, ecmp->ecmp_group_flags);
    max_offset = SAL_UPTO_BIT(dnx_data_l3.ecmp.consistent_tables_info_get(unit, ecmp_mode)->entry_size_in_bits);
    if (ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot use an ECMP User profile for multiply and divide mode.");
    }
    /*
     * Verify the flags supplied to the ECMP structure.
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, ~L3_ECMP_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     ecmp->flags, L3_ECMP_SUPPORTED_BCM_L3_FLAGS);
    }

    if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, ~L3_ECMP_UDEP_SUPPORTED_BCM_L3_ECMP_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 ECMP flags are used.");
    }

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_2ND_HIERARCHY) && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_3RD_HIERARCHY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Two hierarchy levels were selected when a single hierarchy per ECMP is supported.");
    }

    if ((ecmp->user_profile < 1 || ecmp->user_profile > L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit))
        && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "WITH_ID flag is provided, but profile ID %d is invalid - expected value between %d and %d.\n",
                     ecmp->user_profile, 1, L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit));
    }

    if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_MEDIUM_TABLE)
        && _SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_LARGE_TABLE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Both flags BCM_L3_ECMP_MEDIUM_TABLE and BCM_L3_ECMP_LARGE_TABLE are provided while only one or neither is expected.\n");
    }
    /** Validate size of member array */
    if (intf_count !=
        dnx_data_l3.ecmp.consistent_tables_info_get(unit, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->nof_entries
        && ecmp_mode == DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flags indicate a small consistent hashing table but the input array has %d members - expected %d.\n",
                     intf_count, dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                             DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->nof_entries);
    }
    if (intf_count !=
        dnx_data_l3.ecmp.consistent_tables_info_get(unit, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE)->nof_entries
        && ecmp_mode == DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flags indicate a medium consistent hashing table but the input array has %d members - expected %d.\n",
                     intf_count, dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                             DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE)->nof_entries);
    }
    if (intf_count !=
        dnx_data_l3.ecmp.consistent_tables_info_get(unit, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE)->nof_entries
        && ecmp_mode == DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flags indicate a large consistent hashing table but the input array has %d members - expected %d.\n",
                     intf_count, dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                             DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE)->nof_entries);
    }
    if (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID) && _SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "REPLACE flag is provided but WITH_ID flag is missing.\n");
    }
    /** Validate that the offsets in the array are in the expected range. */
    for (idx = 0; idx < intf_count; idx++)
    {
        if ((intf_array[idx] > max_offset) || (intf_array[idx] < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Offset array index %d is out of range - expected it to be between 0 and %d.\n", idx,
                         max_offset);
        }
    }
    /**
     * Support for Tunnel priority is currently disabled.
     */
    if (dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        if ((ecmp->tunnel_priority.mode != bcmL3EcmpTunnelPriorityModeDisable)
            || (ecmp->tunnel_priority.map_profile != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel priority is not supported for user profile definition.\n");
        }
    }
    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_is_allocated(unit, ecmp->user_profile, &is_allocated));
        if (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE) && is_allocated == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Profile %d exists but REPLACE flag was not provided\n", ecmp->user_profile);
        }
        else if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE) && is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Profile %d does not exist, but REPLACE flag is provided.\n",
                         ecmp->user_profile);
        }
        if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            dnx_l3_ecmp_user_profile_t ecmp_profile_data;
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_data_get(unit, ecmp->user_profile, &ecmp_profile_data));
            if (ecmp_profile_data.ecmp_mode != ecmp_mode)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot update ecmp mode value.\n");
            }
            if (ecmp_profile_data.rpf_mode != L3_ECMP_RPF_MODE_BCM_ENUM_TO_DBAL_ENUM(ecmp->rpf_mode))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot update rpf mode value.\n");
            }
            if (ecmp_profile_data.hierarchy != L3_ECMP_GET_HIERARCHY(ecmp->flags))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot update hierarchy value.\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Destroy entities linked to the user profile -
 *      linked list entry and CHM profile
 * \param [in] unit - The unit number.
 * \param [in] user_profile_id - The index of the user profile
 * \param [in] ecmp_user_profile - The user profile data structure
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 * bcm_dnx_l3_egress_ecmp_user_profile_destroy
 */
static shr_error_e
dnx_l3_egress_ecmp_consistent_hashing_destroy(
    int unit,
    int user_profile_id,
    dnx_l3_ecmp_user_profile_t * ecmp_user_profile)
{
    uint32 chm_offset;
    uint32 profile_key;
    uint32 consistent_hashing_manager_handle;
    consistent_hashing_calendar_t calendar;
    dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&calendar, 0, sizeof(consistent_hashing_calendar_t));

    calendar.profile_type = ecmp_user_profile->ecmp_mode;
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                    (unit, ecmp_user_profile->ecmp_mode, &consistent_table_type));
    chm_offset =
        ecmp_user_profile->consistent_members_table_addr * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                                  consistent_table_type);
    profile_key =
        L3_ECMP_CHM_KEY_GET(unit, user_profile_id, ecmp_user_profile->hierarchy,
                            DNX_L3_ECMP_UDEP_ALLOC_STATE_APPLY(unit, ecmp_user_profile->chm_alloc_state));

    /** Update the linked list */
    {
        sw_state_ll_node_t node;
        sw_state_ll_node_t *node_ptr;

        node_ptr = &node;
        SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.get_first(unit, chm_offset, node_ptr));
        /*
         * Iterate over the linked list to find the profile to remove.
         */
        while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
        {
            uint32 profile_key_temp;
            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.node_value(unit, *node_ptr,
                                                                                        &profile_key_temp));

            if (profile_key == profile_key_temp)
            {
                SHR_IF_ERR_EXIT(ecmp_db_info.
                                members_tbl_addr_profile_link_lists.remove_node(unit, chm_offset, *node_ptr));
                break;
            }

            SHR_IF_ERR_EXIT(ecmp_db_info.members_tbl_addr_profile_link_lists.next_node(unit, chm_offset, *node_ptr,
                                                                                       node_ptr));
        }

        if (!DNX_SW_STATE_LL_IS_NODE_VALID(node))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Released user profile %d (hierarchy %d) wasn't found in the members addresses %d linked list,",
                         user_profile_id, ecmp_user_profile->hierarchy, chm_offset);
        }
    }

    /*
     * free the consistent hashing manager from this profile
     */
    SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free
                    (unit, consistent_hashing_manager_handle, profile_key, ecmp_user_profile->ecmp_mode));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Creates or updates an ECMP user profile entry in the SW state.
 * \param [in] unit - The unit number.
 * \param [in,out] ecmp - The ECMP group information.
 *   The following are relevant field of the bcm_l3_egress_ecmp_t structure:
 *  - flags            -> BCM_L3_* flags (supported flags are BCM_L3_REPLACE, BCM_L3_WITH_ID, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY).
 *  - user_profile     -> The ECMP user profile ID(input parameter in case BCM_L3_WITH_ID flag is provided).
 *  - max_paths        -> The size of the ECMP group size.
 *  - dynamic_mode     -> The ECMP hashing mode, must be set to BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT.
 *  - rpf_mode         -> Indicates the URPF mode for the ECMP group - loose, strict or disabled.
 * \param [in] intf_count -
 *   The ECMP current number of members in the group.
 * \param [in] intf_array -
 *     base FEC for the consistent hashing member table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_user_profile_create(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    dnx_l3_ecmp_user_profile_t ecmp_profile;
    int user_profile_id = -1;
    uint32 user_info = 0;
    uint32 consistent_hashing_manager_handle;
    uint32 unique_identifyer = 0;
    uint32 chm_offset;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    /** Can update only the members in the member table. Cannot exchange the profile unless number of references is 0. */

    /** Validate input */
    SHR_INVOKE_VERIFY_DNXC(dnx_l3_egress_ecmp_user_profile_create_verify(unit, ecmp, intf_count, intf_array));

    /** Build ecmp profile data */
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_user_profile_build_struct(unit, ecmp, &ecmp_profile, intf_count, intf_array));

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        user_profile_id = ecmp->user_profile;
    }

    if (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
    {
        dbal_enum_value_field_ecmp_consistent_table_e consistent_table_type;
        /** Allocate profile data */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_allocate(unit, &user_profile_id, &ecmp_profile));
        ecmp->user_profile = user_profile_id;

        /*
         * Allocate an address in the members table and update the profile
         */
        SHR_IF_ERR_EXIT(ecmp_db_info.consistent_hashing_manager_handle.get(unit, &consistent_hashing_manager_handle));
        unique_identifyer = L3_ECMP_CHM_KEY_GET(unit, user_profile_id, ecmp_profile.hierarchy,
                                                DNX_L3_ECMP_UDEP_ALLOC_STATE_APPLY(unit, ecmp_profile.chm_alloc_state));
        /*
         * Having user_info declared will trigger returning the member_table_address in it.
         * Allocate the space for the Consistent hashing members
         */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc(unit, consistent_hashing_manager_handle,
                                                                          unique_identifyer,
                                                                          ecmp_profile.hierarchy,
                                                                          ecmp_profile.ecmp_mode,
                                                                          1 /** nof consecutive tables*/ ,
                                                                          0 /** TP index */ ,
                                                                          2 /** max_paths */ , 1 /** intf_count */ , 0,
                                                                          NULL, (void *) &user_info));

        ecmp_profile.consistent_members_table_addr = user_info;
        SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                        (unit, ecmp_profile.ecmp_mode, &consistent_table_type));
        chm_offset =
            ecmp_profile.consistent_members_table_addr * L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit,
                                                                                                consistent_table_type);
        SHR_IF_ERR_EXIT(ecmp_db_info.
                        members_tbl_addr_profile_link_lists.add_last(unit, chm_offset, &unique_identifyer));
        /** Replace ECMP profile data with the new member table address */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_allocate(unit, &user_profile_id, &ecmp_profile));
        /** Fill the member group with the offsets array provided by the user*/
        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_table_set(unit, &ecmp_profile, intf_count, intf_array));
    }
    else
    {
        /** Update group size data */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_update_group_size
                        (unit, user_profile_id, ecmp_profile.group_size));
        if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, BCM_L3_ECMP_RESILIENT_REPLACE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_data_get(unit, user_profile_id, &ecmp_profile));
            /** Update the member group with the offsets array provided by the user*/
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_table_set
                            (unit, &ecmp_profile, intf_count, intf_array));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_user_profile_update
                            (unit, user_profile_id, ecmp_profile, intf_count, intf_array));
        }

    }

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_ecmp_user_profile_destroy(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp)
{
    uint8 is_allocated = FALSE;
    dnx_l3_ecmp_user_profile_t ecmp_profile_data;
    int intf_count;
    bcm_if_t intf_array[L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES];
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    sal_memset(intf_array, 0, L3_ECMP_CONSISTENT_TABLE_MAX_NOF_ENTRIES * sizeof(bcm_if_t));
    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");

    /** check if profile ID has been configured */
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_is_allocated(unit, ecmp->user_profile, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "User profile %d has not been configured.\n", ecmp->user_profile);
    }
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_data_get(unit, ecmp->user_profile, &ecmp_profile_data));

    intf_count = dnx_data_l3.ecmp.consistent_tables_info_get(unit, ecmp_profile_data.ecmp_mode)->nof_entries;
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_destroy(unit, ecmp->user_profile, &ecmp_profile_data));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_consistent_hashing_table_set(unit, &ecmp_profile_data, intf_count, intf_array));
    SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_user_profile_destroy(unit, ecmp->user_profile));

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the data for an ECMP user profile entry from the SW state.
 * \param [in] unit - The unit number.
 * \param [in,out] ecmp - The ECMP group information.
 *   The following are relevant field of the bcm_l3_egress_ecmp_t structure:
 *  - flags            -> BCM_L3_* flags (returned flags can be BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY).
 *  - user_profile     -> The ECMP user profile ID.
 *  - max_paths        -> The size of the ECMP group size.
 *  - dynamic_mode     -> The ECMP hashing mode, must be set to BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT.
 *  - rpf_mode         -> Indicates the URPF mode for the ECMP group - loose, strict or disabled.
 *   * \param [in] intf_size -
 *   The ECMP current number of members in the intf array.
 * \param [out] intf_count -
 *   The ECMP current number of members in the group.
 * \param [out] intf_array -
 *     base FEC for the consistent hashing member table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_l3_egress_ecmp_user_profile_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    /** Validate input */
    SHR_INVOKE_VERIFY_DNXC(dnx_l3_egress_ecmp_user_profile_get_verify(unit, ecmp, intf_size, intf_count, intf_array));

    dnx_l3_egress_ecmp_from_user_profile_get(unit, ecmp, intf_size, intf_count, intf_array);
exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}
