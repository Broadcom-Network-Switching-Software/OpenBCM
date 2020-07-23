/*
 * ! \file bcm_int/dnx/mirror/mirror_profile.h Internal DNX MIRROR PROFILE APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MIRROR_PROFILE_INCLUDED__
/*
 * { 
 */
#define _DNX_MIRROR_PROFILE_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/mirror.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * MACROs
 * {
 */

/**
 * \brief - Minimal snif profile ID, 0 is reserved for default action (no SNIF). \n 
 * The maximal value is extracted from DNX DATA. See dnx_data_snif.ingress.nof_profiles_get(unit). 
 */
#define DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN 1
/**
 * \brief - Maximal snif profile ID. 
 * This value is extracted from DNX DATA, according to nof_profiles.
 */
#define DNX_MIRROR_PROFILE_INGRESS_PROFILE_MAX(unit) (dnx_data_snif.ingress.nof_profiles_get(unit) - 1)
/**
 * \brief - Default mirror profile, indicates "no mirrorong" 
 */
#define DNX_MIRROR_PROFILE_DEFAULT 0

/**
 * \brief - Mirror Destination type Get
 */
#define DNX_MIRROR_DEST_TYPE_GET(flags)     ((flags & BCM_MIRROR_DEST_IS_SNOOP) ? DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP : \
                                             ((flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE) ? DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE : \
                                                     DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR))

/*
 * }
 */

/**
 * \brief - gets counter command attributes for sniff destination (profile)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] core_id - specific core or core "ALL". if "ALL" get from core-0. 
 *   \param [in] mirror_dest_id - Sniff destination (profile) index
 *   \param [in] counter_cmd_id - counter command id (interface)
 *   \param [in] mirror_count_info - structure which contains sniff counting attributes
 *
 * \remark
 *   * all members of mirro_count_info structure can get BCM_MIRROR_COUNT_AS_ORIG as value.
 *     at this case the original packet decision is taken
 * \see
 *   * None
 */
shr_error_e dnx_mirror_profile_count_command_get(
    int unit,
    bcm_core_t core_id,
    bcm_gport_t mirror_dest_id,
    int counter_cmd_id,
    bcm_mirror_profile_cmd_count_t * mirror_count_info);

/**
 * \brief - sets counter command attributes for sniff destination (profile)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] core_id - specific core or core "ALL" 
 *   \param [in] mirror_dest_id - Sniff destination (profile) index
 *   \param [in] counter_cmd_id - counter command id (interface)
 *   \param [in] mirror_count_info - structure which contains sniff counting attributes
 *
 * \remark
 *   * all members of mirro_count_info structure can get BCM_MIRROR_COUNT_AS_ORIG as value.
 *     at this case the original packet decision is taken
 * \see
 *   * None
 */
shr_error_e dnx_mirror_profile_count_command_set(
    int unit,
    bcm_core_t core_id,
    bcm_gport_t mirror_dest_id,
    int counter_cmd_id,
    bcm_mirror_profile_cmd_count_t * mirror_count_info);

/**
 * \brief - Allocate and set in HW a mirror profile
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest - Mirror profile attributes
 *   
 * \remark
 *   * This function isn't fully documented, the main API is.
 * \see
 *   * For more information see the BCM API \ref bcm_dnx_mirror_destination_create.
 */
shr_error_e dnx_mirror_profile_create(
    int unit,
    bcm_mirror_destination_t * mirror_dest);

/**
 * \brief - Get mirror profile attributes (From HW and SW)
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest_id - Mirror profile gport
 *   \param [in] mirror_dest - pointer to mirror profie attributes to fill
 *   
 * \remark
 *   * This function isn't fully documented, the main API is.
 * \see
 *   * For more information see the BCM API \ref bcm_dnx_mirror_destination_get. 
 */
shr_error_e dnx_mirror_profile_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_destination_t * mirror_dest);

/**
 * \brief - Deallocate a mirror profile and roll back HW to default values
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest_id - Mirror profile gport
 *   
 * \remark
 *   * None
 * \see
 *   * For more information see the BCM API \ref bcm_dnx_mirror_destination_destroy
 */
shr_error_e dnx_mirror_profile_destroy(
    int unit,
    bcm_gport_t mirror_dest_id);

/**
 * \brief - initialize profile mapping tables
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_profile_tables_init(
    int unit);
/*
 * } 
 */
#endif/*_DNX_MIRROR_PROFILE_INCLUDED__*/
