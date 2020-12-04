/** \file appl_ref_l2_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_L2_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_L2_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define OLP_MC_ID (dnx_data_multicast.params.max_ing_mc_groups_get(unit)-1)

/**
 * \brief
 * This function initialize L2 application.
 * The function is configuring a MC group with all the OLP ports and set it as the destination for the DSPs of the OLP.
 * Learn mode is set to be ingress distributed. 
 * L2 learning can work with a default behavior after running this function.
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_l2_init(
    int unit);

/**
 * \brief - Get the OLP logical port.
 *
 * \param [in] unit - unit ID
 * \param [out] olp_port - return OLP logical port. Return -1 if more than 1 OLP port were configured or 0 ports configured.

 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_olp_port_get(
    int unit,
    bcm_gport_t * olp_port);
/*
 * }
 */
#endif /* APPL_REF_L2_INIT_H_INCLUDED */
