/** \file src/bcm/dnx/trunk/trunk_chm.h
 * 
 * Internal DNX TRUNK CHM CBs to be used in CHM init
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKCHMCBS_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_TRUNK_TRUNKCHMCBS_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <bcm/trunk.h>

/** HW table size is 16K, smallest profile used is taking 64 entries, so reduced resources number is 256 */
/** consistent hashing profile types */
typedef enum
{
    TRUNK_INVALID_PROFILE = -1,
    C_LAG_PROFILE_16 = 0,
    C_LAG_PROFILE_64 = 1,
    C_LAG_PROFILE_256 = 2,
} c_lag_profile_type_e;

typedef struct
{
    int profile_type;
    int max_nof_members_in_profile;
} profile_assign_user_info_t;

/**
 * \brief - consistent hashing manager nof resources per profile type get cb
 * 
 * \param [in] unit - unit number
 * \param [in] profile_type - profile type see enum
 * \param [out] nof_resources - number of reduced resources needed for profile type
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_nof_resources_per_profile_type_get(
    int unit,
    int profile_type,
    uint32 *nof_resources);

/** THIS CB is not used and might be removed */
/**
 * \brief - THIS CB is not used and might be removed.
 * This CB is not implemented for that reason.
 *
 * \param [in] unit - unit number
 * \param [in] nof_resources - nof resources
 * \param [out] profile_type - profile type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_profile_type_per_nof_resources_get(
    int unit,
    uint32 nof_resources,
    int *profile_type);

/**
 * \brief - Call Back function to set a profile in the HW with its corresponding ch_calendar.
 *
 * \param [in] unit - unit number
 * \param [in] profile_offset - profile offset to set
 * \param [in] calendar - calendar to set to profile in offset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_calendar_set(
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar);

/**
 * \brief - Call Back function to get a ch_calendar with its
 * corresponding profile in the HW. in this function the
 * calendar is both in and out, the in direction is needed to
 * input the profile's type
 *
 * \param [in] unit - unit number
 * \param [in] profile_offset - profile offset from which to get the calendar
 * \param [in,out] calendar - retrieved calendar (matching provided calendar type)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_calendar_get(
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar);

/**
 * \brief - Call Back function to move a profile from old offset to new
 * offset in the HW, and to change profile mapping from old
 * offset to new offset.
 *
 * \param [in] unit - unit number
 * \param [in] old_offset - old profile offset
 * \param [in] new_offset - new profile offset
 * \param [in] profile_type - profile type
 * \param [in] nof_consecutive_profiles - not relevant for c-lag should always be 1
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_profile_move(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type,
    uint32 nof_consecutive_profiles);

/**
 * \brief - Call Back function to assign a profile to an object
 * identified by unique_identifyer.
 *
 * \param [in] unit - unit number
 * \param [in] chm_handle - CHM handle
 * \param [in] trunk_id - unique id of the object, trunk id in this case
 * \param [in] profile_offset - offset to which to assign the profile
 * \param [in] user_info - user info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_profile_assign(
    int unit,
    uint32 chm_handle,
    uint32 trunk_id,
    uint32 profile_offset,
    void *user_info);

/**
 * \brief - Call Back function to get the profile offset from the connected object unique_identifyer
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - unique id of the object
 * \param [out] profile_offset - profile's offset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_profile_offset_get(
    int unit,
    uint32 trunk_id,
    uint32 *profile_offset);

/**
 * \brief - Call Back function to get the number of entries in a calendar
 * a certain profile type requires. this is usually different
 * than number of resources.
 *
 * \param [in] unit - unit number
 * \param [in] profile_type - profile type
 * \param [in] max_nof_members_in_profile - maximal number of entries in calendar
 * \param [out] nof_calendar_entries - nof entries in calendar
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_calendar_entries_in_profile_get(
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries);

/**
 * \brief - function to get the profile type based on max number of members profile need to support
 *
 * \param [in] unit - unit number
 * \param [in] max_nof_members_in_profile - maximal number of members in profile
 * \param [out] profile_type - profile type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_c_lag_profile_type_get(
    int unit,
    int max_nof_members_in_profile,
    int *profile_type);

/*
 * } 
 */
#endif /* _BCM_DNX_TRUNK_TRUNKCHMCBS_H_INCLUDED */
