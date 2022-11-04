/**
 * \file algo/trunk/algo_trunk.h 
 *
 * trunk algo APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_ALGO_TRUNK_TRUNKALGO_H_INCLUDED
#define _BCMINT_DNX_ALGO_TRUNK_TRUNKALGO_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/types.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
/*
 * }
 */

/**
 * Defines
 * {
 */

#define DNX_ALGO_TRUNK_EGRESSTRUNK_TEMPLATE "Egress Trunk"
#define DNX_ALGO_TRUNK_PSC_PROFILES_TEMPLATE "PSC Profiles"

/**
 * }
 */

/**
 * struct for egress trunk distribution algo
 */
typedef struct dnx_algo_trunk_member_lb_key_ranges_s
{
    bcm_gport_t system_port;
    int first_lb_key;
    int lb_key_range;
} dnx_algo_trunk_member_lb_key_ranges_t;

/**
 * \brief - determines the psc profile according to the psc_info 
 *        and nof_enabled_members. can be overriden by customer
 *        if he wants to implement lags with unique profiles
 *        that has special weight per member
 * 
 * \param [in] unit - unit number
 * \param [in] psc_info - PSC info
 * \param [in] nof_enabled_members - number of enabled members
 * \param [in] psc_profile - result PSC profile
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_smooth_division_profile_set(
    int unit,
    bcm_trunk_psc_profile_info_t * psc_info,
    int nof_enabled_members,
    int *psc_profile);

/**
 * \brief - gets the psc_info according to the psc_profile when 
 *        smooth division is used. currently this is not used
 *        for the profile is determined according to the number
 *        of enabled members, however if a customer chose to
 *        override the set function to support unique weighted
 *        profiles - he needs to override this function as well
 *        for other get APIs to function as expected
 * 
 * \param [in] unit - unit number
 * \param [in] psc_profile - PSC profile
 * \param [in] psc_info - result PSC info
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_smooth_division_profile_get(
    int unit,
    int psc_profile,
    bcm_trunk_psc_profile_info_t * psc_info);

/**
 * \brief - get values of pre-defined SMD profiles to later set 
 *        them through the DBAL or what ever other need you
 *        might have
 * 
 * \param [in] unit - unit number
 * \param [in] profile_array - array of SMD profiles - in the 
 *        form of a 2D array
 * \param [in] nof_profiles - number of profiles - part of the 
 *        size of the array
 * \param [in] nof_lb_keys_per_profile - number of lb-keys per 
 *        profile - part of the size of the array
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_smd_pre_defined_profiles_get(
    int unit,
    int *profile_array,
    int nof_profiles,
    int nof_lb_keys_per_profile);

/**
 * \brief - initialize template manager stuff for egress trunks
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
shr_error_e dnx_algo_egress_trunk_init(
    int unit);

/**
 * \brief - create an Egress Trunk, to be used for resolving 
 *        Egress MC groups containing trunks
 * 
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id for which to create an egress 
 *        trunk
 * \param [in] egress_trunk_id - resulted egress trunk id
 * \param [in] is_first - return val, indication that this is 
 *        the first time this egress trunk id was created.
 *  
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_egress_create(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id,
    uint8 *is_first);

/**
 * \brief - destroy an egress trunk (only if this is its last 
 *        replication)
 * 
 * \param [in] unit - unit number
 * \param [in] egress_trunk_id - egress trunk id to destroy
 * \param [out] is_last - indication that this was the last reference to this profile
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_egress_destroy(
    int unit,
    int egress_trunk_id,
    uint8 *is_last);

/**
 * \brief - get the matching egress trunk id for a trunk id.
 * 
 * \param [in] unit - unit nubmber.
 * \param [in] trunk_id - trunk id - search matching egress 
 *        trunk id for this trunk.
 * \param [in] egress_trunk_id - result egress trunk id.
 * \param [in] profile_found - indication if profile was found.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_egress_get(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id,
    int *profile_found);

/**
 * \brief - get the matching trunk id for an egress trunk id
 * 
 * \param [in] unit - unit number
 * \param [in] egrss_trunk_id - search trunk id matching this 
 *        egress trunk id
 * \param [in] trunk_id - found trunk id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_containing_egress_trunk_get(
    int unit,
    int egrss_trunk_id,
    bcm_trunk_t * trunk_id);

/**
 * \brief - cb function for print for the template manager
 * 
 * \param [in] unit - unit number
 * \param [in] data - trunk id in this case
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_trunk_egresstrunk_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - initialize template manager stuff for user profiles
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
shr_error_e dnx_algo_trunk_user_profile_manager_init(
    int unit);

/**
 * \brief - create (allocate) a user profile
 *
 * \param [in] unit - unit number
 * \param [in] with_id - allocate with ID, id will be provided as input in profile_id
 * \param [in,out] profile_id - profile ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_user_profile_create(
    int unit,
    int with_id,
    int *profile_id);

/**
 * \brief - destroy (free) user profile
 *
 * \param [in] unit - unit number
 * \param [in] profile_id - profile ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_user_profile_destroy(
    int unit,
    int profile_id);

/**
 * \brief - check if user profile is allocated
 *
 * \param [in] unit - unit number
 * \param [in] profile_id - profile ID
 * \param [out] is_allocated - indication if profile was allocated
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_user_profile_is_allocated(
    int unit,
    int profile_id,
    uint8 *is_allocated);

/**
 * \brief - get the total number of supported user profiles
 *
 * \param [in] unit - unit number
 * \param [out] nof_elements - returned nof supported user profiles
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_user_profile_nof_elements_get(
    int unit,
    int *nof_elements);

/**
 * \brief - get the number of free user profiles
 *
 * \param [in] unit - unit number
 * \param [out] nof_free_elements - returned nof free user profiles
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_user_profile_nof_free_elements_get(
    int unit,
    int *nof_free_elements);

/**
 * \brief - initialize template manager stuff for psc profiles
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
shr_error_e dnx_algo_trunk_psc_profiles_init(
    int unit);

/**
 * \brief - create PSC profile
 *
 * \param [in] unit - unit number
 * \param [in] profile_offset - profile offset, the data according to which to allocate the profile
 * \param [out] psc_profile - allocated profile
 * \param [out] is_first - indication if this is the first instance of this profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_psc_profiles_profile_create(
    int unit,
    int profile_offset,
    int *psc_profile,
    uint8 *is_first);

/**
 * \brief - destroy PSC profile
 *
 * \param [in] unit - unit number
 * \param [in] psc_profile - profile id
 * \param [out] is_last - indication if this is the last instance of this profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_psc_profiles_profile_destroy(
    int unit,
    int psc_profile,
    uint8 *is_last);

/**
 * \brief - get profile according to data
 *
 * \param [in] unit - unit number
 * \param [in] profile_offset - profile offset, this is the data according to which to get the profile
 * \param [out] psc_profile - relevant profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_psc_profiles_profile_get(
    int unit,
    int profile_offset,
    int *psc_profile);

/**
 * \brief - get data from profile
 *
 * \param [in] unit - unit number
 * \param [in] psc_profile - profile id
 * \param [out] profile_offset - relevant profile's data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_psc_profiles_profile_data_get(
    int unit,
    int psc_profile,
    int *profile_offset);

/**
 * \brief - replace given profile's data with new data
 *
 * \param [in] unit - unit number
 * \param [in] psc_profile - profile id
 * \param [in] new_profile_offset - new_data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_psc_profiles_profile_data_replace(
    int unit,
    int psc_profile,
    int *new_profile_offset);

/**
 * \brief - cb function for print for the template manager
 *
 * \param [in] unit - unit number
 * \param [in] data - profile_offset in this case
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_trunk_psc_profiles_print_cb(
    int unit,
    const void *data);

/**
 * \brief - determine egress trunk LB-Key distribution for
 *        trunk within an egress MC group.
 * 
 * \param [in] unit - unit number
 * \param [in] profile_lb_key_range_size - number of LB-Keys 
 *        per profile
 * \param [in] profile_first_lb_key_value - value of the first 
 *        LB-Key of this profile - usually determined according
 *        to graceful modification mode.
 * \param [in] trunk_nof_enabled_members - number of enabled 
 *        member in the trunk
 * \param [in] unique_members_array_size - size of unique 
 *        members array
 * \param [in] unique_members_array - input array of unique 
 *        members, uniqueness is determined according to system
 *        port
 * \param [out] lb_key_ranges_array_size - size of lb_key_ranges
 *        array
 * \param [out] lb_key_ranges_array - output array of
 *        lb_key_ranges
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_trunk_egress_trunk_lb_key_destribution_get(
    int unit,
    int profile_lb_key_range_size,
    int profile_first_lb_key_value,
    int trunk_nof_enabled_members,
    int unique_members_array_size,
    dnx_trunk_unique_member_t * unique_members_array,
    int *lb_key_ranges_array_size,
    dnx_algo_trunk_member_lb_key_ranges_t * lb_key_ranges_array);

#endif /* _BCMINT_DNX_ALGO_TRUNK_TRUNKALGO_H_INCLUDED */
