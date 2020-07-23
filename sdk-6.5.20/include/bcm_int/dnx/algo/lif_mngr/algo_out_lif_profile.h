/**
 * \file algo_out_lif_profile.h
 * Internal DNX L3 Management APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_OUT_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define ALGO_OUT_LIF_PROFILE_H_INCLUDED

#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo out_lif_profile.
 * \see
 * dnx_algo_out_lif_profile_template_create
 * {
 */
#define DNX_ALGO_OUT_LIF_PROFILE                 "out lif profile table"
#define DNX_ALGO_ERPP_OUT_LIF_PROFILE            "erpp out lif profile table"
/** Default profile for ETPP out_lif smart template manager */
#define DNX_ALGO_ETPP_OUT_LIF_DEFAULT_PROFILE 0
/** Used for differentiating all the profiles in the smart template different from MPLS and IP_TUNNEL */
#define DNX_ALGO_OUT_LIF_TAG_GENERAL_PROFILE 0
/** Used for differentiating of the MPLS profile in the smart template */
#define DNX_ALGO_OUT_LIF_TAG_MPLS_PROFILE 1
/** Used for differentiating of the IP_TUNNEL profile in the smart template */
#define DNX_ALGO_OUT_LIF_TAG_IP_TUNNEL_PROFILE 2
/** Number of ETPP outlif profile smart template ranges */
#define DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES 3
/**
 * }
 */

/**
 * \brief
 *   Initialize out_lif_profile algorithms.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number
 * \par INDIRECT INPUT
 *    \see
 *    * dnx_algo_out_lif_profile_template_create header
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *    \see
 *    * dnx_algo_out_lif_profile_template_create header
 * \see
 *   * None
 */
shr_error_e dnx_algo_out_lif_profile_init(
    int unit);

/**
 * \brief
 *   Print an out_lif_profile template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 *   \param [in] data -
 *     Pointer to data \n
 *     \b As \b input - \n
 *       pointer to data of type out_lif_template_data_t which
 *       includes properties of outlif profile
 * \par INDIRECT INPUT
 *   * \b *data \n
 *     See 'data' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
 */
void dnx_algo_etpp_out_lif_profile_print_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Print an out_lif_profile template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 *   \param [in] data -
 *     Pointer to data \n
 *     \b As \b input - \n
 *       pointer to data of type out_lif_template_data_t which
 *       includes properties of outlif profile for oam profile resources
 * \par INDIRECT INPUT
 *   * \b *data \n
 *     See 'data' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
 */
void dnx_algo_etpp_out_lif_profile_print_oam_profile_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Print an out_lif_profile template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 *   \param [in] data -
 *     Pointer to data \n
 *     \b As \b input - \n
 *       pointer to data of type out_lif_template_data_t which
 *       includes properties of outlif profile for orientation resources
 * \par INDIRECT INPUT
 *   * \b *data \n
 *     See 'data' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
 */
void dnx_algo_etpp_out_lif_profile_orientation_print_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Print an erpp_out_lif_profile template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 *   \param [in] data -
 *     Pointer to data \n
 *     \b As \b input - \n
 *       pointer to data of type erpp_out_lif_template_data_t which
 *       includes properties of erpp outlif profile for orientation resources
 * \par INDIRECT INPUT
 *   * \b *data \n
 *     See 'data' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
 */
void dnx_algo_erpp_out_lif_profile_print_entry_cb(
    int unit,
    const void *data);
/*
 * }
 */
#endif /* ALGO_OUT_LIF_PROFILE_H_INCLUDED */
