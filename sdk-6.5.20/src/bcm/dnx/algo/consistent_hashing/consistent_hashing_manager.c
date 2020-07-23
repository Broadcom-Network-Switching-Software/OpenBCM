/** 
 * \file algo/consistent_hashing/consistent_hashing_manager.c
 *
 * Implementation of consistent hashing 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_UTILS

/** Include files: */
/** {  */
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include "consistent_hashing_calendar.h"
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_consistent_hashing_access.h>
#include <sal/limits.h>
/** } */

/** Defines */
/** { */
#define DNX_ALGO_CHM_INVALID_UNIQUE_IDENTIFYER 0xffffffff
/** } */

/** Structs */
/** { */
/** } */

/** Functions: */
/** { */

/** forward declarations */
static shr_error_e dnx_algo_consistent_hashing_manager_template_id_get(
    int unit,
    char *instance_name,
    int *template_id);

/**
 * \brief - move function is used to change the offset of a
 *        profile from old to new offset. used as a CB
 *        function for the defragmented chunk. this routine
 *        moves the offset in SW and HW.
 * 
 * \param [in] unit - unit number
 * \param [in] old_offset - old offset of the profile
 * \param [in] new_offset - new offset of the profile 
 * \param [in] nof_reaources_profile_use - number of resources
 *        in defragmented chunk that the profile consumes
 * \param [in] move_cb_additional_info - additional info - in
 *        this case this is the chm_handle.
 * 
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_consistent_hashing_manager_profile_move(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    uint32 nof_reaources_profile_use,
    void *move_cb_additional_info)
{
    uint32 chm_handle;
    int profile_handle;
    const consistent_hashing_cbs_t *cbs;
    template_mngr_handle_t template_mngr_handle;
    int chm_template_id;
    int dummy_ref_count;
    dnx_algo_consistent_hashing_manager_template_manager_key_t profile_key;
    SHR_FUNC_INIT_VARS(unit);

    /** cast additional info to chm_handle */
    chm_handle = *((uint32 *) move_cb_additional_info);
    /** get profile handle */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.get
                    (unit, chm_handle, old_offset, &profile_handle));
    /** get CBs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.get(unit, chm_handle, &cbs));
    /** get template manager handle */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                    template_mngr_handle.get(unit, chm_handle, &template_mngr_handle));
    /** get profile_key for profile_type and consecutive_profiles */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                    (unit, template_mngr_handle.name, &chm_template_id));
    SHR_IF_ERR_EXIT(consistent_hashing_manager.
                    chm_templates.profile_data_get(unit, chm_template_id, profile_handle, &dummy_ref_count,
                                                   &profile_key));

    /** use move_cb */
    /** should copy out members from old offset to new offset,
     *  and should replace instances of old profile with new
     *  profile in module tables accordingly */
    SHR_IF_ERR_EXIT(cbs->profile_move(unit, old_offset, new_offset, profile_key.profile_type,
                                      profile_key.consecutive_profiles));
    /** change mapping between profile and offset to new offset */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.set
                    (unit, chm_handle, new_offset, profile_handle));
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.set
                    (unit, chm_handle, profile_handle, new_offset));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify chm_handle was initialized
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - consistent hashing manager handle to 
 *        check
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
dnx_algo_consistent_hashing_manager_verify_chm_handle(
    int unit,
    uint32 chm_handle)
{
    uint8 bit_val;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(consistent_hashing_manager.occupied_bmp.bit_get(unit, chm_handle, &bit_val));
    if (bit_val == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Uninitialized chm_handle:0x%x\n", chm_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify chm_handle was initialized
 *
 * \param [in] unit - unit number
 * \param [in] instance_name - consistent hashing manager template name.
 * \param [out] template_id - consistent hashing manager template id.
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
dnx_algo_consistent_hashing_manager_template_id_get(
    int unit,
    char *instance_name,
    int *template_id)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(instance_name, _SHR_E_PARAM, "instance_name");

    if (!(sal_strcmp(instance_name, "ECMP consistent hashing manager handle")))
    {
        *template_id = L3_ECMP_CONSISTENT_MANAGER;
        SHR_EXIT();
    }

    if (!(sal_strcmp(instance_name, "chm unit testing one")))
    {
        *template_id = CHM_UNIT_TEST_1;
        SHR_EXIT();
    }

    if (!(sal_strcmp(instance_name, "chm unit testing two")))
    {
        *template_id = CHM_UNIT_TEST_2;
        SHR_EXIT();
    }

    if (!(sal_strcmp(instance_name, "chm unit testing three")))
    {
        *template_id = CHM_UNIT_TEST_3;
        SHR_EXIT();
    }

    if (!(sal_strcmp(instance_name, "chm unit testing four")))
    {
        *template_id = CHM_UNIT_TEST_4;
        SHR_EXIT();
    }

    if (!(sal_strcmp(instance_name, "chm unit testing five")))
    {
        *template_id = CHM_UNIT_TEST_5;
        SHR_EXIT();
    }

    if (!(sal_strcmp(instance_name, "chm unit testing six")))
    {
        *template_id = CHM_UNIT_TEST_6;
        SHR_EXIT();
    }

    /*
     * If we are in this point there is no template with this instance_name. It will return invalid id.
     */
    *template_id = CONSISTENT_MANAGER_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(consistent_hashing_manager.init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_create(
    int unit,
    uint32 nof_resources_to_manage,
    char *instance_name,
    consistent_hashing_cbs_t * cbs,
    uint32 *chm_handle)
{
    uint8 bit_val;
    uint32 chi_index;
    int name_len;
    dnx_algo_template_create_data_t template_create_data;
    template_mngr_handle_t template_handle;
    sw_state_defragmented_chunk_init_info_t chunk_init_info;
    int chm_template_id;
    SHR_FUNC_INIT_VARS(unit);

    /** sanity checks */
    SHR_NULL_CHECK(instance_name, _SHR_E_PARAM, "instance_name");
    SHR_NULL_CHECK(cbs, _SHR_E_PARAM, "cbs");
    SHR_NULL_CHECK(chm_handle, _SHR_E_PARAM, "chm_handle");
    if (nof_resources_to_manage == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value for nof_resources_to_manage (%d)\n", nof_resources_to_manage);
    }
    name_len = sal_strlen(instance_name);
    if (name_len >= DNX_ALGO_CONSISTENT_HASHING_MAX_TEMPLATE_MNGR_HANDLE_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid length for instance, use smaller size, max size is %d\n",
                     DNX_ALGO_CONSISTENT_HASHING_MAX_TEMPLATE_MNGR_HANDLE_LENGTH - 1);
    }

    /** find empty instance of consistent hashing */
    /** iterate on consistent hashing instances (chi_index) */
    for (chi_index = 0; chi_index < DNX_ALGO_CONSISTENT_HASHING_MAX_NOF_INSTANCES; ++chi_index)
    {
        SHR_IF_ERR_EXIT(consistent_hashing_manager.occupied_bmp.bit_get(unit, chi_index, &bit_val));
        if (bit_val == 0)
        {
            /** found free instance, mark as occupied */
            SHR_IF_ERR_EXIT(consistent_hashing_manager.occupied_bmp.bit_set(unit, chi_index));
            *chm_handle = chi_index;
            break;
        }
    }
    if (chi_index == DNX_ALGO_CONSISTENT_HASHING_MAX_NOF_INSTANCES)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Consistent hashing manager reached its maximum instances occupancy\n");
    }

    /** create a template manager instance */
    template_create_data.flags = 0;
    template_create_data.first_profile = 0;
    template_create_data.nof_profiles = nof_resources_to_manage;
    template_create_data.max_references = SAL_INT32_MAX;
    template_create_data.data_size = sizeof(dnx_algo_consistent_hashing_manager_template_manager_key_t);
    template_create_data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC;
    sal_strncpy(template_create_data.name, instance_name, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get(unit, instance_name, &chm_template_id));

    SHR_IF_ERR_EXIT(consistent_hashing_manager.
                    chm_templates.create(unit, chm_template_id, &template_create_data, NULL));

    /** allocate profile_offset to profile mapping array */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.alloc
                    (unit, *chm_handle, nof_resources_to_manage));

    /** allocate profile to profile_offset mapping array */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.alloc
                    (unit, *chm_handle, nof_resources_to_manage));

    /** copy template manager instance handler to consistent hashing instance */
    /** force last byte in template_handle.name to be '\0' to make coverity happy,
     * although an error is not expected there anyways */
    sal_strncpy(template_handle.name, instance_name, DNX_ALGO_CONSISTENT_HASHING_MAX_TEMPLATE_MNGR_HANDLE_LENGTH - 1);
    template_handle.name[DNX_ALGO_CONSISTENT_HASHING_MAX_TEMPLATE_MNGR_HANDLE_LENGTH - 1] = '\0';
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.template_mngr_handle.set
                    (unit, *chm_handle, &template_handle));

    /** create a defragmented chunk instance */
    chunk_init_info.chunk_size = nof_resources_to_manage;
    sal_strncpy(chunk_init_info.move_function, "dnx_algo_consistent_hashing_manager_profile_move",
                SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.def_chunk.create(unit, *chm_handle, &chunk_init_info));

    /** set CBs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.set(unit, *chm_handle, cbs));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_destroy(
    int unit,
    uint32 chm_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    /** sanity checks */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_verify_chm_handle(unit, chm_handle));

    /** All sw state resources will be destroyed at deinit. */

    /** destroy template manager */
    /** Sw state template will be destroy with the entire sw state. */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_profile_alloc(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    int profile_type,
    uint32 nof_profiles,
    uint32 profile_index,
    uint32 max_nof_members_in_profile,
    uint32 current_nof_members_in_profile,
    uint32 *specific_members_array,
    uint32 *profile_offset_ptr,
    void *user_info)
{
    int profile_handle;
    uint32 profile_offset;
    uint8 first_reference;
    template_mngr_handle_t template_mngr_handle;
    dnx_algo_consistent_hashing_manager_template_manager_key_t t_mngr_key;
    const consistent_hashing_cbs_t *cbs;
    int chm_template_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Sanity checks */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_verify_chm_handle(unit, chm_handle));
    if (current_nof_members_in_profile > max_nof_members_in_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid current number of members\n");
    }
    if (1 >= max_nof_members_in_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid max number of members in profile, must be bigger than 1\n");
    }
    if (!utilex_is_power_of_2(nof_profiles))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of members consecutive profiles, has to be power of 2\n");
    }
    if (profile_index >= nof_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index, has to be smaller than number of profiles\n");
    }

    /** check if profile exists in template manager */
    /** get handle to template manager */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.template_mngr_handle.get
                    (unit, chm_handle, &template_mngr_handle));
    /** configure key for template manager */
    t_mngr_key.max_nof_elements_in_profile = max_nof_members_in_profile;
    t_mngr_key.profile_type = profile_type;
    t_mngr_key.consecutive_profiles = nof_profiles;
    /** if allocated profile is full change its unique identifier
     *  to be invalid, the idea is that a full profile is not
     *  uniquely identified with an object and can be used by many
     *  objects, the exception to this rule is allocation of consecutive profiles,
     *  in those cases, always use unique ID */
    if ((max_nof_members_in_profile != current_nof_members_in_profile) || (nof_profiles > 1))
    {
        t_mngr_key.unique_identifyer = unique_identifyer;
    }
    else
    {
        /** single full profile, can be reused so a non unique ID is used */
        t_mngr_key.unique_identifyer = DNX_ALGO_CHM_INVALID_UNIQUE_IDENTIFYER;
    }
    /** allocate profile in template manager */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                    (unit, template_mngr_handle.name, &chm_template_id));

    SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.allocate_single
                    (unit, chm_template_id, 0, &t_mngr_key, NULL, &profile_handle, &first_reference));

    /** get cbs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.get(unit, chm_handle, &cbs));
    if (first_reference)
    {
        int rv = _SHR_E_NONE;
        uint32 nof_resources;
        uint32 nof_entries_in_calendar;
        uint32 piece_alloc_size;
        uint32 total_offset;
        uint8 dummy_last_reference;
        consistent_hashing_calendar_t calendar;
        /** allocate in defragmented chunk */
        SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_type, &nof_resources));
        piece_alloc_size = nof_resources * nof_profiles;
        rv = consistent_hashing_manager.ch_instance_array.def_chunk.piece_alloc(unit, chm_handle, piece_alloc_size,
                                                                                (void *) &chm_handle, &profile_offset);
        if (rv != _SHR_E_NONE)
        {
            /** need to free profile handle in template manager if defragmented chunk alloc returned an error */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                            (unit, template_mngr_handle.name, &chm_template_id));

            SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.free_single
                            (unit, chm_template_id, profile_handle, &dummy_last_reference));
            SHR_IF_ERR_EXIT(rv);
        }
        /** map profile and profile offset on both directions  */
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.set
                        (unit, chm_handle, profile_offset, profile_handle));
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.set
                        (unit, chm_handle, profile_handle, profile_offset));
        /** get nof entries for calendar */
        SHR_IF_ERR_EXIT(cbs->calendar_entries_in_profile_get
                        (unit, profile_type, max_nof_members_in_profile, &nof_entries_in_calendar));
        /** calculate consistent hashing calendar */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_calendar_calculate
                        (unit, max_nof_members_in_profile, nof_entries_in_calendar, current_nof_members_in_profile,
                         specific_members_array, &calendar));
        /** set profile_type to calendar */
        calendar.profile_type = profile_type;
        /** calculate total offset for calendar assignment, should factor in the profile index as well */
        total_offset = profile_offset + (nof_resources * profile_index);
        /** use CBs to set calculated calendar to hw */
        SHR_IF_ERR_EXIT(cbs->calendar_set(unit, total_offset, &calendar));
    }
    else
    {
        /** get profile offset of existing profile */
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.get
                        (unit, chm_handle, profile_handle, &profile_offset));
    }

    /** check if user provided a pointer to return profile_offset */
    if (profile_offset_ptr == NULL)
    {
        /** use CB to assign profile */
        SHR_IF_ERR_EXIT(cbs->profile_assign(unit, chm_handle, unique_identifyer, profile_offset, user_info));
    }
    else
    {
        /** return profile offset to the user to assign the profile on his own */
        *profile_offset_ptr = profile_offset;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_profile_free(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    int profile_type)
{
    int profile_handle;
    uint8 last_reference;
    uint32 profile_offset;
    template_mngr_handle_t template_mngr_handle;
    const consistent_hashing_cbs_t *cbs;
    int chm_template_id;
    dnx_algo_consistent_hashing_manager_template_manager_key_t profile_key;
    int dummy_ref_count;
    SHR_FUNC_INIT_VARS(unit);

    /** sanity checks */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_verify_chm_handle(unit, chm_handle));

    /** get template manager handle */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.template_mngr_handle.get
                    (unit, chm_handle, &template_mngr_handle));
    /** get cbs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.get(unit, chm_handle, &cbs));
    /** use CBs to get profile_offset */
    SHR_IF_ERR_EXIT(cbs->profile_offset_get(unit, unique_identifyer, &profile_offset));
    /** get profile */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.get
                    (unit, chm_handle, profile_offset, &profile_handle));
    /** get profile_key for later in case it is the last reference */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                    (unit, template_mngr_handle.name, &chm_template_id));
    SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.profile_data_get
                    (unit, chm_template_id, profile_handle, &dummy_ref_count, &profile_key));
    /** free profile in template manager */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.free_single
                    (unit, chm_template_id, profile_handle, &last_reference));
    /** if freed last reference - free chunk in defragmented chunk as well */
    if (last_reference)
    {
        consistent_hashing_calendar_t calendar;
        uint32 nof_resources;
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                        def_chunk.piece_free(unit, chm_handle, profile_offset));
        /** Set all members of the calendar to 0 */
        sal_memset(calendar.lb_key_member_array, 0,
                   DNX_DATA_MAX_CONSISTENT_HASHING_CALENDAR_MAX_NOF_ENTRIES_IN_CALENDAR *
                   sizeof(calendar.lb_key_member_array[0]));
        /** set profile_type to calendar */
        calendar.profile_type = profile_type;
        /** get number if resources for this profile type */
        SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_type, &nof_resources));
        /** use number of consecutive allocated to clear all the relevant HW */
        for (int profile_index = 0; profile_index < profile_key.consecutive_profiles; ++profile_index)
        {
            uint32 total_offset = profile_offset + (nof_resources * profile_index);
            /** use CBs to set the empty calendar to hw in order to free the FEC ECMP members HW */
            SHR_IF_ERR_EXIT(cbs->calendar_set(unit, total_offset, &calendar));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_profile_member_add(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_index,
    uint32 member_to_add,
    uint32 current_nof_members,
    void *user_info)
{
    template_mngr_handle_t template_mngr_handle;
    uint32 profile_offset;
    int dummy_ref_count;
    int profile_handle;
    const consistent_hashing_cbs_t *cbs;
    dnx_algo_consistent_hashing_manager_template_manager_key_t profile_key;
    consistent_hashing_calendar_t current_calendar;
    consistent_hashing_calendar_t new_calendar;
    int chm_template_id;

    SHR_FUNC_INIT_VARS(unit);

    /** sanity checks */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_verify_chm_handle(unit, chm_handle));

    /** get template manager handle */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.template_mngr_handle.get
                    (unit, chm_handle, &template_mngr_handle));
    /** get CBs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.get(unit, chm_handle, &cbs));
    /** use CBs to get profile_offset */
    SHR_IF_ERR_EXIT(cbs->profile_offset_get(unit, unique_identifyer, &profile_offset));
    /** get profile */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.get
                    (unit, chm_handle, profile_offset, &profile_handle));
    /** get profile key */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                    (unit, template_mngr_handle.name, &chm_template_id));

    SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.profile_data_get
                    (unit, chm_template_id, profile_handle, &dummy_ref_count, &profile_key));
    /** check if current_nof_members allows adding another member by comparing to max number of members */
    if (current_nof_members >= profile_key.max_nof_elements_in_profile)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Profile is FULL, cannot add additional members to it\n");
    }
    /** check that profile_index is valid */
    if (profile_index >= profile_key.consecutive_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "profile index is Invalid\n");
    }

    /** check if adding a member will cause profile to be full,
     *   in that case - need to change from partial profile to full
     *  profile and free the partial profile, this is relevant only
     *  if we don't have consecutive profiles */
    if ((current_nof_members + 1 == profile_key.max_nof_elements_in_profile) && (profile_key.consecutive_profiles == 1))
    {
        int new_profile_handle;
        uint32 new_profile_offset;
        uint8 first_reference;
        uint8 last_reference;
        /** set profile key with invalid unique_identifyer */
        profile_key.unique_identifyer = DNX_ALGO_CHM_INVALID_UNIQUE_IDENTIFYER;
        /** allocate profile in template manager */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                        (unit, template_mngr_handle.name, &chm_template_id));

        SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.allocate_single
                        (unit, chm_template_id, 0, &profile_key, NULL, &new_profile_handle, &first_reference));
        if (first_reference)
        {
            int rv = _SHR_E_NONE;
            uint32 nof_resources;
            uint32 nof_entries_in_calendar;
            uint8 dummy_last_reference;
            /** allocate in defragmented chunk */
            SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_key.profile_type, &nof_resources));
            rv = consistent_hashing_manager.ch_instance_array.def_chunk.piece_alloc(unit, chm_handle, nof_resources,
                                                                                    (void *) &chm_handle,
                                                                                    &new_profile_offset);
            if (rv != _SHR_E_NONE)
            {
                /** need to free profile handle in template manager if defragmented chunk alloc returned an error */
                SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                                (unit, template_mngr_handle.name, &chm_template_id));

                SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.free_single
                                (unit, chm_template_id, new_profile_handle, &dummy_last_reference));
                SHR_IF_ERR_EXIT(rv);
            }
            /** map profile and profile offset on both directions  */
            SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.set
                            (unit, chm_handle, new_profile_offset, new_profile_handle));
            SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.set
                            (unit, chm_handle, new_profile_handle, new_profile_offset));
            /** get calendar */
            current_calendar.profile_type = profile_key.profile_type;
            SHR_IF_ERR_EXIT(cbs->calendar_get(unit, profile_offset, &current_calendar));
            /** get nof entries for calendar */
            SHR_IF_ERR_EXIT(cbs->calendar_entries_in_profile_get
                            (unit, profile_key.profile_type, profile_key.max_nof_elements_in_profile,
                             &nof_entries_in_calendar));
            /** add member to calendar */
            /** here first member in calendar indication is always false,
             *  the only possible scenario that it shouldn't be false is
             *  when a user defined consistent hashing profile with a max
             *  amout of members = 1, which doesn't make any sense */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_calendar_member_add
                            (unit, &current_calendar, member_to_add, profile_key.max_nof_elements_in_profile,
                             nof_entries_in_calendar, 0, &new_calendar));
            /** use CBs to set calculated calendar to hw */
            SHR_IF_ERR_EXIT(cbs->calendar_set(unit, new_profile_offset, &new_calendar));
        }
        else
        {
            /** if not first ref, can get allocated profile_offset */
            SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.get
                            (unit, chm_handle, new_profile_handle, &new_profile_offset));
        }
        /** use CB to assign profile */
        SHR_IF_ERR_EXIT(cbs->profile_assign(unit, chm_handle, unique_identifyer, new_profile_offset, user_info));
        /** free old profile */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                        (unit, template_mngr_handle.name, &chm_template_id));

        SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.free_single
                        (unit, chm_template_id, profile_handle, &last_reference));
        /** the following is a sanity check, if this branch of the
         *  code is happening - we should have had only a single ref to
         *  the freed profile prior to freeing it, this means
         *  last_reference indication should be true */
        if (last_reference == TRUE)
        {
            /** free from defragmented chunk old profile offset */
            SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                            def_chunk.piece_free(unit, chm_handle, profile_offset));
            /** there's no need to clean-up the profile to profile offset
             *  or the reversed mapping. those will be overrun when
             *  reused either way. */
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Something went wrong, seems like the consistent hashing got corrupted"
                         "and currently Phantom profiles exist\n");
        }
    }
    else
    {
        uint32 total_offset;
        uint32 nof_resources;
        uint32 nof_entries_in_calendar;
        uint8 first_member_in_calendar = (current_nof_members == 0) ? 1 : 0;
        SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_key.profile_type, &nof_resources));
        /** get calendar */
        total_offset = profile_offset + (nof_resources * profile_index);
        current_calendar.profile_type = profile_key.profile_type;
        SHR_IF_ERR_EXIT(cbs->calendar_get(unit, total_offset, &current_calendar));
        /** get nof entries for calendar */
        SHR_IF_ERR_EXIT(cbs->calendar_entries_in_profile_get
                        (unit, profile_key.profile_type, profile_key.max_nof_elements_in_profile,
                         &nof_entries_in_calendar));
        /** add member to calendar */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_calendar_member_add
                        (unit, &current_calendar, member_to_add, profile_key.max_nof_elements_in_profile,
                         nof_entries_in_calendar, first_member_in_calendar, &new_calendar));
        /** set calendar */
        SHR_IF_ERR_EXIT(cbs->calendar_set(unit, total_offset, &new_calendar));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_profile_member_remove(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_index,
    uint32 member_to_remove,
    uint32 current_nof_members,
    void *user_info)
{
    template_mngr_handle_t template_mngr_handle;
    consistent_hashing_calendar_t current_calendar;
    consistent_hashing_calendar_t new_calendar;
    const consistent_hashing_cbs_t *cbs;
    int dummy_ref_count;
    int profile_handle;
    uint32 total_offset;
    uint32 profile_offset;
    uint32 nof_resources;
    uint32 nof_entries_in_calendar;
    dnx_algo_consistent_hashing_manager_template_manager_key_t profile_key;
    int chm_template_id;
    SHR_FUNC_INIT_VARS(unit);

    /** sanity checks */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_verify_chm_handle(unit, chm_handle));
    /** check if current_nof_members allows removing another member by comparing it to 0 */
    if (current_nof_members == 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Cannot remove members, current number of members is 0\n");
    }

    /** get CBs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.get(unit, chm_handle, &cbs));
    /** get template manager handle */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.template_mngr_handle.get
                    (unit, chm_handle, &template_mngr_handle));
    /** use CBs to get profile_offset */
    SHR_IF_ERR_EXIT(cbs->profile_offset_get(unit, unique_identifyer, &profile_offset));
    /** get profile */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.get
                    (unit, chm_handle, profile_offset, &profile_handle));
    /** get profile key */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                    (unit, template_mngr_handle.name, &chm_template_id));
    SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.profile_data_get
                    (unit, chm_template_id, profile_handle, &dummy_ref_count, &profile_key));
    /** check that profile_index is valid */
    if (profile_index >= profile_key.consecutive_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "profile index is Invalid\n");
    }
    /** get calendar */
    SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_key.profile_type, &nof_resources));
    current_calendar.profile_type = profile_key.profile_type;
    total_offset = profile_offset + (nof_resources * profile_index);
    SHR_IF_ERR_EXIT(cbs->calendar_get(unit, total_offset, &current_calendar));
    /** get nof entries for calendar */
    SHR_IF_ERR_EXIT(cbs->calendar_entries_in_profile_get
                    (unit, profile_key.profile_type, profile_key.max_nof_elements_in_profile,
                     &nof_entries_in_calendar));
    /** remove member from calendar */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_calendar_member_remove
                    (unit, &current_calendar, member_to_remove, profile_key.max_nof_elements_in_profile,
                     nof_entries_in_calendar, &new_calendar));
    /** check if current is max - this case requires allocating a
     *  new profile - this new profile will always be the first
     *  reference so defrag chunk can be allocated before template
     *  allocated - this is true only if there's a single consecutive profile,
     *  if not, just recalculate existing profile */
    if ((current_nof_members == profile_key.max_nof_elements_in_profile) && (profile_key.consecutive_profiles == 1))
    {
        uint32 nof_resources;
        uint32 new_profile_offset;
        uint8 dummy_first_reference;
        uint8 last_reference;
        int new_profile_handle;
        /** allocate in defragmented chunk */
        SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_key.profile_type, &nof_resources));
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                        def_chunk.piece_alloc(unit, chm_handle, nof_resources, (void *) &chm_handle,
                                              &new_profile_offset));

        /** allocate in template manager */
        profile_key.unique_identifyer = unique_identifyer;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                        (unit, template_mngr_handle.name, &chm_template_id));

        SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.allocate_single
                        (unit, chm_template_id, 0, &profile_key, NULL, &new_profile_handle, &dummy_first_reference));
        /** map profile and profile offset on both directions  */
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.profile_to_offset_map.set
                        (unit, chm_handle, new_profile_handle, new_profile_offset));
        SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.offset_to_profile_map.set
                        (unit, chm_handle, new_profile_offset, new_profile_handle));
        /** set calendar */
        SHR_IF_ERR_EXIT(cbs->calendar_set(unit, new_profile_offset, &new_calendar));
        /** use assign_cb to set the profile to the user's object which is identified by unique_identifyer */
        SHR_IF_ERR_EXIT(cbs->profile_assign(unit, chm_handle, unique_identifyer, new_profile_offset, user_info));
        /** free old profile */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                        (unit, template_mngr_handle.name, &chm_template_id));

        SHR_IF_ERR_EXIT(consistent_hashing_manager.chm_templates.free_single
                        (unit, chm_template_id, profile_handle, &last_reference));
        /** if last reference need to free piece in defragmented
         *  chunk as well */
        if (last_reference == TRUE)
        {
            /** free from defragmented chunk old profile offset */
            SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                            def_chunk.piece_free(unit, chm_handle, profile_offset));
            /** there's no need to clean-up the profile to profile offset
             *  or the reversed mapping. those will be overrun when
             *  reused either way. */
        }
    }
    else
    {
        /** set calendar */
        SHR_IF_ERR_EXIT(cbs->calendar_set(unit, total_offset, &new_calendar));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_manager_calender_set(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_index,
    uint32 current_nof_members_in_profile,
    uint32 *specific_members_array,
    void *user_info)
{
    const consistent_hashing_cbs_t *cbs;
    uint32 profile_offset;
    uint32 total_offset;
    uint32 nof_resources;
    int profile_handle;
    template_mngr_handle_t template_mngr_handle;
    int chm_template_id;
    int dummy_ref_count;
    dnx_algo_consistent_hashing_manager_template_manager_key_t profile_key;
    uint32 nof_entries_in_calendar;
    consistent_hashing_calendar_t calendar;
    SHR_FUNC_INIT_VARS(unit);

    /** get cbs */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.cbs.get(unit, chm_handle, &cbs));
    /** use CBs to get profile_offset */
    SHR_IF_ERR_EXIT(cbs->profile_offset_get(unit, unique_identifyer, &profile_offset));
    /** get profile */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                    offset_to_profile_map.get(unit, chm_handle, profile_offset, &profile_handle));
    /** get handle to template manager */
    SHR_IF_ERR_EXIT(consistent_hashing_manager.ch_instance_array.
                    template_mngr_handle.get(unit, chm_handle, &template_mngr_handle));
    /** get profile key */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_template_id_get
                    (unit, template_mngr_handle.name, &chm_template_id));
    SHR_IF_ERR_EXIT(consistent_hashing_manager.
                    chm_templates.profile_data_get(unit, chm_template_id, profile_handle, &dummy_ref_count,
                                                   &profile_key));
    /** get nof entries for calendar */
    SHR_IF_ERR_EXIT(cbs->calendar_entries_in_profile_get(unit, profile_key.profile_type,
                                                         profile_key.max_nof_elements_in_profile,
                                                         &nof_entries_in_calendar));
    if (profile_key.consecutive_profiles == 1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Invalid Usage of function. should be used only for profiles with consecutive_profiles > 1\n");
    }
    if (profile_index >= profile_key.consecutive_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid profile index\n");
    }
    /** calculate consistent hashing calendar */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_calendar_calculate
                    (unit, profile_key.max_nof_elements_in_profile, nof_entries_in_calendar,
                     current_nof_members_in_profile, specific_members_array, &calendar));
    /** set profile_type to calendar */
    calendar.profile_type = profile_key.profile_type;
    /** use CBs to set calculated calendar to hw */
    SHR_IF_ERR_EXIT(cbs->nof_resources_per_profile_type_get(unit, profile_key.profile_type, &nof_resources));
    total_offset = profile_offset + (nof_resources * profile_index);
    SHR_IF_ERR_EXIT(cbs->calendar_set(unit, total_offset, &calendar));

exit:
    SHR_FUNC_EXIT;
}

void
dnx_algo_consistent_hashing_manager_print_cb(
    int unit,
    const void *data)
{
    dnx_algo_consistent_hashing_manager_template_manager_key_t *t_mngr_key =
        (dnx_algo_consistent_hashing_manager_template_manager_key_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Max nof members",
                                   t_mngr_key->max_nof_elements_in_profile, "Max number of members in profile", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Profile type", t_mngr_key->profile_type,
                                   "Profile type", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ID", t_mngr_key->unique_identifyer,
                                   "Unique identifier", "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Consecutive profiles",
                                   t_mngr_key->consecutive_profiles, "Consecutive profiles", "0x%x");
    SW_STATE_PRETTY_PRINT_FINISH();
}

/** } */
