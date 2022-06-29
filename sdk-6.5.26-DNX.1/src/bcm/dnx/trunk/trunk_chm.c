/** \file src/bcm/dnx/trunk/trunk_chm.c
 * 
 *  This file contains the implementation on HM CBs functions.
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include "trunk_chm.h"
#include "trunk_temp_structs_to_skip_papi.h"
#include "trunk_dbal_access.h"
#include "trunk_sw_db.h"
#include "trunk_utils.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define NOF_ENTRIES_IN_C_LAG_PROFILE_16 (128)
#define NOF_ENTRIES_IN_C_LAG_PROFILE_64 (256)
#define MAX_MEMBER_ID_IN_C_LAG_PROFILE_16 (0xf)
#define MAX_MEMBER_ID_IN_C_LAG_DEFAULT (0xff)
#define HW_ENTRIES_USED_IN_C_LAG_PROFILE_16 (64)
#define HW_ENTRIES_USED_IN_C_LAG_PROFILE_64 (256)
#define HW_ENTRIES_USED_IN_C_LAG_PROFILE_256 (512)

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/** convert sw profile type to HW profile type */
/** the inverse conversion is not 1 to 1. we can have many types of sw profiles that receive the same hw profile type */
/** this will be used in 2 pass MC solution */
static shr_error_e
dnx_trunk_sw_profile_type_to_hw_profile_type_convert(
    int unit,
    int sw_profile_type,
    int *hw_profile_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (sw_profile_type)
    {
        case C_LAG_PROFILE_16:
        {
            *hw_profile_type = 0;
            break;
        }
        case C_LAG_PROFILE_64:
        {
            *hw_profile_type = 1;
            break;
        }
        case C_LAG_PROFILE_256:
        {
            *hw_profile_type = 2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile type (%d)\n", sw_profile_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header */
/** consistent_hashing_manager_nof_resources_per_profile_type_get_cb */
shr_error_e
dnx_trunk_c_lag_nof_resources_per_profile_type_get(
    int unit,
    int profile_type,
    uint32 *nof_resources)
{
    SHR_FUNC_INIT_VARS(unit);
    /** switch case of profile types and their resources. this is the reduced number of resources per profile */
    switch (profile_type)
    {
        case C_LAG_PROFILE_16:
        {
            *nof_resources = 1;
            break;
        }
        case C_LAG_PROFILE_64:
        {
            *nof_resources = 4;
            break;
        }
        case C_LAG_PROFILE_256:
        {
            *nof_resources = 8;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported profile type");
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header */
shr_error_e
dnx_trunk_c_lag_profile_type_per_nof_resources_get(
    int unit,
    uint32 nof_resources,
    int *profile_type)
{
    SHR_FUNC_INIT_VARS(unit);
    /** this CB is not used as of now. no need to implement it */
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to set a profile in the HW with its
 * corresponding ch_calendar.
 */
/** see header */
shr_error_e
dnx_trunk_c_lag_calendar_set(
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar)
{
    uint32 nof_resources;
    int nof_hw_entries;
    int is_double_entry;
    int entry_index;
    int calendar_step_size;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of hw entries from profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_nof_resources_per_profile_type_get(unit, calendar->profile_type, &nof_resources));
    nof_hw_entries = nof_resources * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);

    /** get indication if in this profile type the entries are double */
    is_double_entry = (calendar->profile_type == C_LAG_PROFILE_16);
    calendar_step_size = is_double_entry ? 2 : 1;

    /** calculate initial hw entry index */
    entry_index = profile_offset * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);

    for (int calendar_index = 0, entry_count = 0; entry_count < nof_hw_entries; ++entry_count)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_members_map_set(unit, entry_index++, is_double_entry,
                                                                                 calendar->lb_key_member_array
                                                                                 [calendar_index],
                                                                                 calendar->lb_key_member_array
                                                                                 [calendar_index + is_double_entry]));
        calendar_index += calendar_step_size;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to get a ch_calendar with its
 * corresponding profile in the HW. in this function the
 * calendar is both in and out, the in direction is needed to
 * input the profile's type
 */
shr_error_e
dnx_trunk_c_lag_calendar_get(
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar)
{
    uint32 nof_resources;
    int nof_hw_entries;
    int is_double_entry;
    int entry_index;
    int calendar_step_size;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of hw entries from profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_nof_resources_per_profile_type_get(unit, calendar->profile_type, &nof_resources));
    nof_hw_entries = nof_resources * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);

    /** get indication if in this profile type the entries are double */
    is_double_entry = (calendar->profile_type == C_LAG_PROFILE_16);
    calendar_step_size = is_double_entry ? 2 : 1;

    /** calculate initial hw entry index */
    entry_index = profile_offset * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);

    for (int calendar_index = 0, entry_count = 0; entry_count < nof_hw_entries; ++entry_count)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_members_map_get(unit, entry_index++, is_double_entry,
                                                                                 &calendar->lb_key_member_array
                                                                                 [calendar_index],
                                                                                 &calendar->lb_key_member_array
                                                                                 [calendar_index + is_double_entry]));
        calendar_index += calendar_step_size;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to move a profile from old offset to new
 * offset in the HW, and to change profile mapping from old
 * offset to new offset.
 */
/** see header */
shr_error_e
dnx_trunk_c_lag_profile_move(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type,
    uint32 nof_consecutive_profiles)
{
    uint32 nof_resources;
    int nof_hw_entries;
    int is_double_entry;
    int source_entry;
    int dest_entry;
    int psc_profile;
    int member_base;
    int member_mode;
    int max_size;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of hw entries from profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_nof_resources_per_profile_type_get(unit, profile_type, &nof_resources));
    nof_hw_entries = nof_resources * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);

    /** get entry offset from old offset */
    source_entry = old_offset * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);
    /** get entry offset from new offset */
    dest_entry = new_offset * dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get(unit);
    is_double_entry = (profile_type == C_LAG_PROFILE_16);
    /** loop on number of entries */
    for (int entry_count = 0; entry_count < nof_hw_entries; ++entry_count)
    {
        uint32 first_mapped_member;
        uint32 second_mapped_member;
        /** copy entry from old offset to new offset in member table */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_members_map_get(unit, source_entry++, is_double_entry,
                                                                                 &first_mapped_member,
                                                                                 &second_mapped_member));
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_members_map_set
                        (unit, dest_entry++, is_double_entry, first_mapped_member, second_mapped_member));
    }

    /** get profile in template manager using offset */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_get(unit, old_offset, &psc_profile));
    /** change HW table "Lag group profile" to the new offset */
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_profile_configurations_get
                    (unit, psc_profile, &member_base, &member_mode, &max_size));
    /** convert offset to member base */
    member_base = new_offset / nof_resources;
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_profile_configurations_set
                    (unit, psc_profile, member_base, member_mode, max_size));
    /** change the data in template manager for this profile to be new_offset */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_data_replace(unit, psc_profile, (int *) &new_offset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to assign a profile to an object
 * identified by unique_identifyer.
 */
shr_error_e
dnx_trunk_c_lag_profile_assign(
    int unit,
    uint32 chm_handle,
    uint32 trunk_id,
    uint32 profile_offset,
    void *user_info)
{
    profile_assign_user_info_t *profile_info;
    bcm_trunk_id_info_t id_info;
    int psc;
    int psc_profile;
    int new_psc_profile;
    uint8 is_first;
    SHR_FUNC_INIT_VARS(unit);

    /** cast user_info to profile_info */
    profile_info = (profile_assign_user_info_t *) user_info;
    /** get trunk id info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));

    /** if trunk is mapped to a profile already */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, id_info.pool_index, id_info.group_index, &psc, &psc_profile));
    if (psc == BCM_TRUNK_PSC_C_LAG)
    {
        uint8 is_last;

        /** free profile in template manager */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_destroy(unit, psc_profile, &is_last));
        if (is_last)
        {
            /** clear profile data */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_set(unit, psc_profile, TRUNK_INVALID_PROFILE, -1));
        }
    }

    /** allocate a profile in template manager */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_create(unit, profile_offset, &new_psc_profile, &is_first));
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_chm_psc_profile_is_first_config(unit, new_psc_profile, profile_info));
    }
    /** map trunk_id to profile (sw db and HW) */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_set
                    (unit, id_info.pool_index, id_info.group_index, BCM_TRUNK_PSC_C_LAG, new_psc_profile));
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set
                    (unit, id_info.pool_index, id_info.group_index, BCM_TRUNK_PSC_C_LAG, new_psc_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to get the profile offset from the
 * connected object unique_identifyer
 */
shr_error_e
dnx_trunk_c_lag_profile_offset_get(
    int unit,
    uint32 unique_id,
    uint32 *profile_offset)
{
    int psc_profile;
    SHR_FUNC_INIT_VARS(unit);

    /** check if unique_id has user_profile or trunk_id encoding */
    if (_SHR_USER_PROFILE_IS_VALID(unique_id))
    {
        int profile_id;
        /** unique_id provided is user_profile */
        _SHR_TRUNK_USER_PROFILE_ID_GET(profile_id, unique_id);
        /** Get c-lag profile from profile_id */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_psc_profile_get(unit, profile_id, &psc_profile));
    }
    else
    {
        bcm_trunk_id_info_t id_info;
        int psc;
        /** unique_id provided is trunk_id */
        /** get trunk id info */
        SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, unique_id, &id_info));
        /** get profile */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get
                        (unit, id_info.pool_index, id_info.group_index, &psc, &psc_profile));
    }
    /** get profile data from template manager */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_data_get(unit, psc_profile, (int *) profile_offset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to get the number of entries in a calendar
 * a certain profile type requires. this is usually different
 * than number of resources.
 */
shr_error_e
dnx_trunk_c_lag_calendar_entries_in_profile_get(
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (profile_type)
    {
        case C_LAG_PROFILE_16:
        {
            *nof_calendar_entries = NOF_ENTRIES_IN_C_LAG_PROFILE_16;
            break;
        }
        case C_LAG_PROFILE_64:
        {
            *nof_calendar_entries = NOF_ENTRIES_IN_C_LAG_PROFILE_64;
            break;
        }
        case C_LAG_PROFILE_256:
        {
            *nof_calendar_entries = max_nof_members_in_profile * 2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported profile type");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_chm_psc_profile_is_first_config(
    int unit,
    int psc_profile,
    profile_assign_user_info_t * profile_info)
{
    int hw_profile_type;
    int member_base;
    uint32 nof_resources_per_profile;
    int profile_offset;
    SHR_FUNC_INIT_VARS(unit);

    /** set profile data to sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_set
                    (unit, psc_profile, profile_info->profile_type, profile_info->max_nof_members_in_profile));
    /** convert profile_type to HW profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_profile_type_to_hw_profile_type_convert
                    (unit, profile_info->profile_type, &hw_profile_type));
    /** get nof resources for profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_nof_resources_per_profile_type_get
                    (unit, profile_info->profile_type, &nof_resources_per_profile));
    /** get offset and convert offset to member base */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_data_get(unit, psc_profile, &profile_offset));
    member_base = profile_offset / nof_resources_per_profile;
    /** set HW with profile info */
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_consistent_hashing_profile_configurations_set
                    (unit, psc_profile, member_base, hw_profile_type, profile_info->max_nof_members_in_profile));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_c_lag_profile_type_get(
    int unit,
    int max_nof_members_in_profile,
    int *profile_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (max_nof_members_in_profile <= 16)
    {
        *profile_type = C_LAG_PROFILE_16;
    }
    else if (max_nof_members_in_profile <= 64)
    {
        *profile_type = C_LAG_PROFILE_64;
    }
    else if (max_nof_members_in_profile <= 256)
    {
        *profile_type = C_LAG_PROFILE_256;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported number of member for existing profiles");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_user_profile_type_get(
    int unit,
    bcm_trunk_profile_t * profile,
    int *profile_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (profile->nof_members == NOF_ENTRIES_IN_C_LAG_PROFILE_16)
    {
        *profile_type = C_LAG_PROFILE_16;
    }
    else if (profile->nof_members == NOF_ENTRIES_IN_C_LAG_PROFILE_64)
    {
        *profile_type = C_LAG_PROFILE_64;
    }
    else
    {
        *profile_type = C_LAG_PROFILE_256;
    }

    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_user_profile_max_member_id_get(
    int unit,
    bcm_trunk_profile_t * profile,
    int *max_member_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (profile->nof_members == NOF_ENTRIES_IN_C_LAG_PROFILE_16)
    {
        *max_member_id = MAX_MEMBER_ID_IN_C_LAG_PROFILE_16;
    }
    else
    {
        *max_member_id = MAX_MEMBER_ID_IN_C_LAG_DEFAULT;
    }

    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_user_profile_hw_entries_used_get(
    int unit,
    bcm_trunk_profile_t * profile,
    int *hw_entries_used)
{
    SHR_FUNC_INIT_VARS(unit);

    if (profile->nof_members == NOF_ENTRIES_IN_C_LAG_PROFILE_16)
    {
        *hw_entries_used = HW_ENTRIES_USED_IN_C_LAG_PROFILE_16;
    }
    else if (profile->nof_members == NOF_ENTRIES_IN_C_LAG_PROFILE_64)
    {
        *hw_entries_used = HW_ENTRIES_USED_IN_C_LAG_PROFILE_64;
    }
    else
    {
        *hw_entries_used = HW_ENTRIES_USED_IN_C_LAG_PROFILE_256;
    }

    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_user_profile_get(
    int unit,
    int psc_profile,
    consistent_hashing_calendar_t * calendar,
    bcm_trunk_profile_t * profile)
{
    int psc_profile_type;
    int psc_max_nof_members;
    SHR_FUNC_INIT_VARS(unit);

    /** Get profile type from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get(unit, psc_profile, &psc_profile_type, &psc_max_nof_members));

    /** Copy members array */
    sal_memcpy(profile->members_array, calendar->lb_key_member_array, sizeof(profile->members_array));

    /** Set nof_members based on profile type */
    switch (psc_profile_type)
    {
        case C_LAG_PROFILE_16:
        {
            profile->nof_members = 128;
            break;
        }
        case C_LAG_PROFILE_64:
        {
            profile->nof_members = 256;
            break;
        }
        case C_LAG_PROFILE_256:
        {
            profile->nof_members = psc_max_nof_members * 2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported profile type");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
