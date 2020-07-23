/**
 * \file algo/consistent_hashing/consistent_hashing_manager.h
 *
 * consistent hashing APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_ALGO_CONSISTENTHASHINGMANAGER_H_INCLUDED
#define _BCMINT_DNX_ALGO_CONSISTENTHASHINGMANAGER_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** Includes */
/** { */
#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_consistent_hashing.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
/** } */

/** Defines */
/** { */
/** } */

/** The following structs and callbacks definitions need to
 *  be resolved using the new swstate cb registeration
 *  mechanism once available */
/** { */
typedef struct consistent_hashing_calendar_s
{
    int profile_type;
    uint32 lb_key_member_array[DNX_DATA_MAX_CONSISTENT_HASHING_CALENDAR_MAX_NOF_ENTRIES_IN_CALENDAR];
} consistent_hashing_calendar_t;

/**
 * Call Back function to get the needed resources per input
 * profile type. each profile type should return a power of 2
 * nof resources. the number of resources should represent the 
 * reduced number of resources the HW utilizes. for example if
 * you have a HW that uses 64, 256 and 512 HW entries per 
 * profile, and the table size is 4k, the preferred method to
 * deliver the information is 1, 4, and 8 in accordance to above
 * and during init the total number of resources should be 64 
 */
typedef shr_error_e(
    *consistent_hashing_manager_nof_resources_per_profile_type_get_cb) (
    int unit,
    int profile_type,
    uint32 *nof_resources);
/**
 * Call Back function to get the profile type from needed 
 * resources. input which is not a power of 2 should fail as 
 * invalid nof_resources.
 */
typedef shr_error_e(
    *consistent_hashing_manager_profile_type_per_nof_resources_get_cb) (
    int unit,
    uint32 nof_resources,
    int *profile_type);
/**
 * Call Back function to set a profile in the HW with its
 * corresponding ch_calendar.
 */
typedef shr_error_e(
    *consistent_hashing_manager_calendar_set_cb) (
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar);
/**
 * Call Back function to set a ch_calendar with its 
 * corresponding profile in the HW. in this function the 
 * calendar is both in and out, the in direction is needed to 
 * input the profile's type 
 */
typedef shr_error_e(
    *consistent_hashing_manager_calendar_get_cb) (
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar);

/**
 * Call Back function to move a profile from old offset to new 
 * offset in the HW, and to change profile mapping from old 
 * offset to new offset. 
 */
typedef shr_error_e(
    *consistent_hashing_manager_profile_move_cb) (
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type,
    uint32 nof_consecutive_profiles);
/**
 * Call Back function to assign a profile to an object 
 * identified by unique_identifyer. 
 */
typedef shr_error_e(
    *consistent_hashing_manager_profile_assign_cb) (
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_offset,
    void *user_info);

/**
 * Call Back function to get the profile offset from the
 * connected object unique_identifyer
 */
typedef shr_error_e(
    *consistent_hashing_manager_profile_offset_get_cb) (
    int unit,
    uint32 unique_identifyer,
    uint32 *profile_offset);

/**
 * Call Back function to get the number of entries in a calendar 
 * a certain profile type requires. this is usually different 
 * than number of resources.
 */
typedef shr_error_e(
    *consistent_hashing_manager_calendar_entries_in_profile_get_cb) (
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries);

typedef struct consistent_hashing_cbs_s
{
    consistent_hashing_manager_nof_resources_per_profile_type_get_cb nof_resources_per_profile_type_get;
    /** TBD this might be redundant, think about removing it */
    /** { */
    consistent_hashing_manager_profile_type_per_nof_resources_get_cb profile_type_per_nof_resources_get;
    /** } */
    consistent_hashing_manager_calendar_set_cb calendar_set;
    consistent_hashing_manager_calendar_get_cb calendar_get;
    consistent_hashing_manager_profile_move_cb profile_move;
    consistent_hashing_manager_profile_assign_cb profile_assign;
    consistent_hashing_manager_profile_offset_get_cb profile_offset_get;
    consistent_hashing_manager_calendar_entries_in_profile_get_cb calendar_entries_in_profile_get;
} consistent_hashing_cbs_t;
/** } */

/**
 * \brief - initiate consistent hashing manager
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_init(
    int unit);

/**
 * \brief - create consistent hashing manager instance
 * 
 * \param [in] unit - unit number
 * \param [in] nof_resources_to_manage - number of resources to 
 *        manage - the different profiles can consume a power of
 *        2 number of resources each. those resources
 *        represent the HW resources used. \see
 *        consistent_hashing_manager_nof_resources_per_profile_type_get_cb
 *        for farther explanation.
 * \param [in] instance_name - string describing the instance 
 *        name
 * \param [in] cbs - a set of call backs functions to allow 
 *        access from chm instance to callers HW as SW
 * \param [out] chm_handle - received handle for this instance
 *        of consistent hashing manager
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_create(
    int unit,
    uint32 nof_resources_to_manage,
    char *instance_name,
    consistent_hashing_cbs_t * cbs,
    uint32 *chm_handle);

/**
 * \brief - destroy a consistent hashing manager instance
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - handle to the instance to be 
 *        destroyed
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_destroy(
    int unit,
    uint32 chm_handle);

/**
 * \brief - allocate a profile in a consistent hashing 
 *        instance
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - consistent hashing manager handle
 * \param [in] unique_identifyer - unique identifier of the
 *        object that will use this profile (in case of trunk -
 *        it will make sense to use the trunk_id, in case of
 *        ecmp it will make sense to use the ecmp_id 
 * \param [in] profile_type - wanted profile type
 * \param [in] nof_profiles - number of consecutive profiles to allocate (has to be a power of 2 - 1,2,4...)
 * \param [in] profile_index - index in the consecutive profiles to init
 * \param [in] max_nof_members_in_profile - maximal number of 
 *        members in the profile
 * \param [in] current_nof_members_in_profile - current number 
 *        of members in profile
 * \param [in] specific_members_array - if the profile is not 
 *        full there's an option to indicate which members are
 *        found in it. If NULL this option is ignored and members are assumed to be consecutive.
 * \param [out] profile_offset_ptr - this is an optional parameter,
 *        it is relevant only when the user doesn't want the profile
 *        to be assigned with the user supplied CB profile_assign. in
 *        those cases the pointer must not be NULL and it is the user's
 *        responsibility to assign the profile himself.
 * \param [out] user_info - this is an optional parameter.
 *        the assign CB can use this additional user info if it is available.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * in cases when more than 1 consecutive profile is allocated, the profiles cannot be reused.
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_profile_alloc(
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
    void *user_info);

/**
 * \brief - free a profile in a consistent hashing instance
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - consistent hashing manager handle
 * \param [in] unique_identifyer - unique identifier of the
 *        object whose profile you want to free
 * \param [in] profile_type - Type of the profile
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_profile_free(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    int profile_type);

/**
 * \brief - add a member to a chm profile
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - consistent hashing manager handle
 * \param [in] unique_identifyer - unique identifier of the
 *        object to whom you want to add a member
 * \param [in] profile_index - index in the consecutive profiles (should be 0 if no consecutive profiles were allocated)
 * \param [in] member_to_add - the member number you want to add
 * \param [in] current_nof_members - current number of members 
 *        in the profile
 * \param [out] user_info - this is an optional parameter.
 *        the assign CB can use this additional user info if it is available.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_profile_member_add(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_index,
    uint32 member_to_add,
    uint32 current_nof_members,
    void *user_info);

/**
 * \brief - remove a member from a chm profile
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - consistent hashing manager handle
 * \param [in] unique_identifyer - unique identifier of the
 *        object from whom you want to remove a member
* \param [in] profile_index - index in the consecutive profiles (should be 0 if no consecutive profiles were allocated)
 * \param [in] member_to_remove - the member number you want to 
 *        remove
 * \param [in] current_nof_members - current number of members 
 *        in the profile
 * \param [out] user_info - this is an optional parameter.
 *        the assign CB can use this additional user info if it is available.
 *
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_profile_member_remove(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_index,
    uint32 member_to_remove,
    uint32 current_nof_members,
    void *user_info);

/**
 * \brief - set a calendar for specific profile index (provided the profile has consecutive profiles)
 *
 * \param [in] unit - unit number
 * \param [in] chm_handle - consistent hashing manager handle
 * \param [in] unique_identifyer - unique identifier of the
 *        object for which to set the calendar
 * \param [in] profile_index - index in the consecutive profiles to set a calendar to
 * \param [in] current_nof_members_in_profile - current number of members in profile
 * \param [in] specific_members_array - if the profile is not
 *        full there's an option to indicate which members are
 *        found in it. If NULL this option is ignored and members are assumed to be consecutive.
 * \param [out] user_info - this is an optional parameter.
 *        the assign CB can use this additional user info if it is available.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * This function is usable only for profiles which has consecutive profiles allocated with them,
 *   in addition it is the user's responsibility to use this API only on un-initialized profile indexes.
 *   Failure to do so will result in profiles losing their "consistent hashing" qualities.
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_manager_calender_set(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_index,
    uint32 current_nof_members_in_profile,
    uint32 *specific_members_array,
    void *user_info);

/**
 * \brief - print callback required internally for the template 
 *        manager used. when creating a new instance of chm, the
 *        instance_name used should be inserted to the template
 *        manager print cb structure with this cb function. this
 *        is a temporary state and in the future this step will
 *        not be needed.
 * 
 * \param [in] unit - unit number
 * \param [in] data - data to print
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_consistent_hashing_manager_print_cb(
    int unit,
    const void *data);

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
shr_error_e dnx_algo_consistent_hashing_manager_profile_move(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    uint32 nof_reaources_profile_use,
    void *move_cb_additional_info);

#endif /* _BCMINT_DNX_ALGO_CONSISTENTHASHINGMANAGER_H_INCLUDED */
