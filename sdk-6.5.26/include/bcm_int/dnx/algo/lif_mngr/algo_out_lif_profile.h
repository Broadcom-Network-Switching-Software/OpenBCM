/**
 * \file algo_out_lif_profile.h
 * Internal DNX L3 Management APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_OUT_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define ALGO_OUT_LIF_PROFILE_H_INCLUDED

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * Resource name defines for algo out_lif_profile.
 * \see
 * dnx_algo_out_lif_profile_template_create
 * {
 */
#define DNX_ALGO_OUT_LIF_PROFILE                 "out lif profile table"
#define DNX_ALGO_ERPP_OUT_LIF_PROFILE            "erpp out lif profile table"
/** Number of out_lif profiles when iop_mode_outlif_selection is true */
#define DNX_ALGO_ERPP_IOP_MODE_OUTLIF_SELECTION_NOF_PROFILES 2
/** Default profile for ETPP out_lif smart template manager */
#define DNX_ALGO_ETPP_OUT_LIF_DEFAULT_PROFILE 0
/** Used for differentiating all the profiles in the smart template different from MPLS and IP_TUNNEL */
#define DNX_ALGO_OUT_LIF_TAG_GENERAL_PROFILE 0
/** Used for differentiating of the MPLS profile in the smart template */
#define DNX_ALGO_OUT_LIF_TAG_MPLS_PROFILE 1
/** Used for differentiating of the IP_TUNNEL profile in the smart template */
#define DNX_ALGO_OUT_LIF_TAG_IP_TUNNEL_PROFILE 2
/** Used for differentiating the Port outlif profile range in the smart template */
#define DNX_ALGO_OUT_LIF_TAG_PORT_PROFILE 3
/** Number of ETPP outlif profile smart template ranges per outlif */
#define DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES_PER_OUTLIF 3
/** Number of ETPP outlif profile smart template ranges per port */
#define DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES_PER_PORT 1

/**
 * }
 */

/**
 * \brief - Initialize algo outlif profile template managers
 *
 * \param [in] unit - Unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *
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
*****************************************************/
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
*****************************************************/
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
*****************************************************/
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
*****************************************************/
void dnx_algo_erpp_out_lif_profile_print_entry_cb(
    int unit,
    const void *data);
/*
 * }
 */
#endif /* ALGO_OUT_LIF_PROFILE_H_INCLUDED */
