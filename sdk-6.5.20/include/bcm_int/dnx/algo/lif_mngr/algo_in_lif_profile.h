/**
 * \file algo_in_lif_profile.h
 * Internal DNX L3 Managment APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_IN_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define ALGO_IN_LIF_PROFILE_H_INCLUDED

#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo in_lif_profile.
 * \see
 * dnx_algo_l3_enablers_vector_template_create
 * dnx_algo_in_lif_profile_template_create
 * dnx_algo_eth_rif_profile_template_create
 * dnx_algo_in_lif_flood_profile_template_create
 * {
 */
#define DNX_ALGO_IN_LIF_PROFILE_L3_ENABLERS             "enablers vector table"
#define DNX_ALGO_IN_LIF_PROFILE                         "in_lif_profile table"
#define DNX_ALGO_ETH_RIF_PROFILE                        "eth_rif_profile table"
#define DNX_ALGO_IN_LIF_DA_NOT_FOUND_DESTINATION        "flooding group profiles for unknown DA on inlif"
#define DNX_ALGO_IN_LIF_LB_PROFILE_ENABLERS             "in_lif_lb_profile enablers"

/**
 * }
 */

/**
 * \brief
 *   Initialize in_lif_profile algorithms.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number
 * \par INDIRECT INPUT
 *    \see
 *    * dnx_algo_l3_enablers_vector_template_create header
 *    * dnx_algo_in_lif_profile_template_create header
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *    \see
 *    * dnx_algo_l3_enablers_vector_template_create header
 *    * dnx_algo_in_lif_profile_template_create header
 * \see
 *   * None
 */
shr_error_e dnx_algo_in_lif_profile_init(
    int unit);

/**
* \brief
*   Print an entry of the enablers vector template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      the unit number
*    \param [in] data -
*      Pointer of the struct to be printed.
*      \b As \b input - \n
*       The pointer is expected to hold a struct of type uint32[2]. \n
*       It's the user's responsibility to verify it beforehand.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    None.
*  \par INDIRECT OUTPUT:
*      The default output stream.
*  \remark
*    None
*  \see
*    dnx_algo_template_print_data_cb
*    shr_error_e
 */
void dnx_algo_in_lif_profile_l3_enablers_profile_print_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Print an in_lif_profile template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 *   \param [in] data -
 *     Pointer to data \n
 *     \b As \b input - \n
 *       pointer to data of type in_lif_template_data_t which
 *       includes properties of lif profile
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
void dnx_algo_in_lif_profile_print_entry_cb(
    int unit,
    const void *data);

/**
 * \brief - Print a da_not_found_destination template entry.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] data - pointer to data of type dnx_default_frwrd_dst_t which
 *       includes properties of destination, offset and add_vsi_dst for unknwon
 *       unicast, multicast and broadcast packets separately.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
void dnx_algo_in_lif_flood_profile_print_entry_cb(
    int unit,
    const void *data);

/**
 * \brief - Print a in_lif_lb_profile template entry.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] data - pointer to data of type uint32 which
 *       includes represents the layers per which lb profile is disabled/enabled
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
void dnx_algo_in_lif_profile_in_lif_lb_profile_print_entry_cb(
    int unit,
    const void *data);
/*
 * }
 */
#endif /* ALGO_IN_LIF_PROFILE_H_INCLUDED */
