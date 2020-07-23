/** \file out_lif_profile.h
 *
 * out_lif profile management for DNX.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef OUT_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define OUT_LIF_PROFILE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/l3.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/***************************************************************************************/
/*
 * Defines
 */
/***************************************************************************************/
/*
 * Default profiles/values defines
 */
/**
 * Specify default out_lif_profile
 */
#define DNX_OUT_LIF_PROFILE_DEFAULT (0)
/**
 * Specify default out_lif_orientation
 */
#define DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION (0)
#define DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE (56)
/**
 * Specify default split_horizon enable
 */
#define DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE (1)
/**
 * Specify default split_horizon disable
 */
#define DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_DISABLE (0)
/**
 * Specify default OAM LIF Profile
 */
#define DNX_OUT_LIF_PROFILE_OAM_PROFILE (0)
/**
 * Specify default TTL Scope Threshold
 */
#define DNX_OUT_LIF_PROFILE_DEFAULT_TTL_THRESHOLD (0)
/**
 * Used for smart template manager to indicate to keep default ip tunnel profile
 */
#define DNX_OUT_LIF_PROFILE_KEEP_DEFAULT_IP_TUNNEL_PROFILE 1

/***************************************************************************************/
/*
 * Enums
 */
/***************************************************************************************/
/*
 * Structures
 */

/**
 * /brief - out_lif template data structure.
 * The template manager's data of out_lif_profile used for orientation only.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 out_lif_orientation;
} etpp_out_lif_template_data_orientation_t;

/**
 * /brief - out_lif template data structure.
 * The template manager's data of out_lif_profile used for oam_profile only.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 oam_lif_profile;
} etpp_out_lif_template_data_oam_profile_t;

/**
 * /brief - out_lif template data structure.
 * The template manager's data of out_lif_profile used for all data-fields.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 out_lif_orientation;
    uint8 etpp_split_horizon_enable;
    uint8 oam_lif_profile;
    /** Used only for smart template manager */
    uint8 keep_internal_default_profile;
} etpp_out_lif_template_data_t;

/**
 * \brief - the input structure of dnx_out_lif_profile_exchange,
 * includes the out_lif profile properties to set.
 * \remark
 *  need to call out_lif_profile_info_t_init in order to initiate the fields to the default valid values
 *  and then to update the structure and send it to the API.
 */
typedef struct
{
    uint8 out_lif_orientation;
    uint8 etpp_split_horizon_enable;
    uint8 oam_lif_profile;
    /** Used only for smart template manager */
    uint8 keep_internal_default_profile;
} etpp_out_lif_profile_info_t;

/**
 * \brief - the input structure of dnx_erpp_out_lif_profile_exchange,
 * includes the erpp_out_lif profile properties to set.
 * \remark
 *  need to call erpp_out_lif_profile_info_t_init in order to initiate the fields to the default valid values
 *  and then to update the structure and send it to the API.
 */
typedef struct
{
    uint8 ttl_scope_threshold;
} erpp_out_lif_profile_info_t;
/**
 * /brief - erpp_out_lif_profile data structure.
 * The template manager's data of ERPP outlif profile.
 * this structure is for inner usage only.
 */
typedef struct
{
    uint8 ttl_scope_threshold;
} erpp_out_lif_template_data_t;
/***************************************************************************************/
/*
 * API
 */
/**
 * \brief
 *   initializes the module out_lif_profile:
 *   writes default profiles/values to relevant tables, for example:
 *   OUT_LIF_PROFILE_TABLE
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * updates dbal tables OUT_LIF_PROFILE_TABLE
 * \see
 *   * None
 */
shr_error_e dnx_out_lif_profile_module_init(
    int unit);

/**
 * \brief
 *   updates template manager for L2/L3 profiles, and exchanges the old out_lif_profile with
 *   new out_lif_profile according to LIF properties which are stored in out_lif_profile_info.
 * \param [in] unit -
 *   The unit number.
 * \param [out] dbal_table_id -
 *   DBAL table ID, used to differentiate between the supported outlif profile supported ranges \n
 * \param [out] flags -
 *   Flags, which might be used in future, to differentiate between the supported outlif profile supported ranges \n
 * \param [in] etpp_out_lif_profile_info -
 *   Pointer to out_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties
 * \param [in] old_etpp_out_lif_profile -
 *   old out_lif_profile id to exchange
 * \param [out] new_etpp_out_lif_profile -
 *   Pointer to new_out_lif_profile \n
 *     This procedure loads pointed memory by the newly assigned out_lif_profile
 *     id in template manager
 * \param [out] is_last -
 *     Determines if it is last reference in the out_lif template manager
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   We don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the InLIF
 *   to the new place traffic will get some "undefine" state.
 * \see
 *   * dnx_out_lif_profile_algo_handle_routing_enablers_profile
 *   * dnx_out_lif_profile_algo_convert_to_rpf_mode
 *   * dnx_algo_template_exchange
 *   * DBAL_TABLE_OUT_LIF_PROFILE_TABLE
 *   * NOF_ROUTING_PROFILES_DUPLICATES
 */
shr_error_e dnx_etpp_out_lif_profile_exchange(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 flags,
    etpp_out_lif_profile_info_t * etpp_out_lif_profile_info,
    int old_etpp_out_lif_profile,
    int *new_etpp_out_lif_profile,
    uint8 *is_last);

/**
 * \brief
 *   updates template manager for L2/L3 profiles, and exchanges the old erpp_out_lif_profile with
 *   new erpp_out_lif_profile according to LIF properties which are stored in erpp_out_lif_profile_info.
 * \param [in] unit -
 *   The unit number.
 * \param [in] erpp_out_lif_profile_info -
 *   Pointer to erpp_out_lif_profile_info \n
 *     Pointed memory includes ERPP LIF-PROFILE properties
 * \param [in] old_erpp_out_lif_profile -
 *   old erpp_out_lif_profile id to exchange
 * \param [out] new_erpp_out_lif_profile -
 *   Pointer to new_erpp_out_lif_profile \n
 *     This procedure loads pointed memory by the newly assigned erpp_out_lif_profile
 *     id in template manager
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_erpp_out_lif_profile_exchange(
    int unit,
    erpp_out_lif_profile_info_t * erpp_out_lif_profile_info,
    int old_erpp_out_lif_profile,
    int *new_erpp_out_lif_profile);
/**
 * \brief
 *   gets out_lif_profile and returns references count
 * \param [in] unit -
 *   The unit number.
 * \param [in] etpp_out_lif_profile -
 *   out_lif_profile id to get ref_count for
 * \param [out] ref_count -
 *   Pointer to ref_count \n
 *     Pointed memory includes references count of the out_lif_profile
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_etpp_out_lif_profile_get_ref_count(
    int unit,
    int etpp_out_lif_profile,
    int *ref_count);

/**
 * \brief
 *   gets out_lif_profile and returns out_lif_profile data
 * \param [in] unit -
 *   The unit number.
 * \param [in] etpp_out_lif_profile -
 *   out_lif_profile id to get data for
 * \param [out] etpp_out_lif_profile_info -
 *   Pointer to out_lif_profile_info \n
 *     Pointed memory includes out_lif_profile properties
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_etpp_out_lif_profile_get_data(
    int unit,
    int etpp_out_lif_profile,
    etpp_out_lif_profile_info_t * etpp_out_lif_profile_info);

/**
 * \brief
 *   gets out_lif_profile and returns out_lif_profile data
 * \param [in] unit -
 *   The unit number.
 * \param [in] erpp_out_lif_profile -
 *   erpp_out_lif_profile id to get data for
 * \param [out] erpp_out_lif_profile_info -
 *   Pointer to erpp_out_lif_profile_info \n
 *     Pointed memory includes erpp_out_lif_profile properties
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_erpp_out_lif_profile_get_data(
    int unit,
    int erpp_out_lif_profile,
    erpp_out_lif_profile_info_t * erpp_out_lif_profile_info);

/*
 * \brief
 *      initialize fields of the structure out_lif_profile_info_t to the default values
 * \param [in] unit -
 *   The unit number.
 * \param [out] etpp_out_lif_profile_info -
 *   Pointer to out_lif_profile_info \n
 *     Pointed memory includes LIF-PROFILE properties initiated to the default values
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void etpp_out_lif_profile_info_t_init(
    int unit,
    etpp_out_lif_profile_info_t * etpp_out_lif_profile_info);

/*
 * \brief
 *      initialize fields of the structure erpp_out_lif_profile_info_t to the default values
 * \param [in] unit -
 *   The unit number.
 * \param [out] erpp_out_lif_profile_info -
 *   Pointer to erpp_out_lif_profile_info \n
 *     Pointed memory includes ERPP LIF-PROFILE properties initiated to the default values
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void erpp_out_lif_profile_info_t_init(
    int unit,
    erpp_out_lif_profile_info_t * erpp_out_lif_profile_info);

/**
* \brief
*  This function purpose is to set Out Lif profile in HW
* \par DIRECT INPUT:
*   \param [in] unit  -  unit Id
*   \param [in] port  - InLIF gport
*   \param [in] new_out_lif_profile  -  OutLIF-profile to configure
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_out_lif_profile_set(
    int unit,
    bcm_gport_t port,
    int new_out_lif_profile);

/**
* \brief
*  This function purpose is to get the out Lif profile for given inLIF
* \par DIRECT INPUT:
*   \param [in] unit  -  unit Id
*   \param [in] port  - OutLIF gport
*   \param [out] out_lif_profile_p  -  OutLIF-profile configured for InLIF given
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_out_lif_profile_get(
    int unit,
    bcm_gport_t port,
    int *out_lif_profile_p);
/*
 * }
 */
#endif /* OUT_LIF_PROFILE_H_INCLUDED */
