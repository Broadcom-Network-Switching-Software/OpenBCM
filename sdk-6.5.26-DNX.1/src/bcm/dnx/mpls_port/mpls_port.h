/** \file mpls_port.h
 *      Used by mpls_port files
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _MPLS_PORT_INCLUDED__
          /*
           * {
           */
#define _MPLS_PORT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <bcm/mpls.h>

/**
* \brief
* Insert requested 'cw' value to CW register CFG_MPLS_CONTROL_WORD.
*   \param [in] unit       -  Relevant unit.
*   \param [in] cw  -  Value to be saved in CFG_MPLS_CONTROL_WORD.
 * \return
 *   \retval - Non-zero in case of an error.
*/
shr_error_e dnx_mpls_cw_set(
    int unit,
    int cw);

/**
* \brief
* Get value from CW register CFG_MPLS_CONTROL_WORD.
*   \param [in] unit       -  Relevant unit.
*   \param [in] cw  -  Stores the retrieved value from CFG_MPLS_CONTROL_WORD.
 * \return
 *   \retval - Non-zero in case of an error.
*/
shr_error_e dnx_mpls_cw_get(
    int unit,
    int *cw);

/**
 * \brief
 *   Initialize and populate a mpls_l2vpn object from a mpls_port input
 * \param [in] unit - Unit ID
 * \param [in] vpn - VPN/VSI ID
 * \param [in] mpls_port - API input object
 * \param [out] l2vpn - pointer to allocated mpls_l2vpn object to initialize and fill.
 * \return Standard error handling
 */
shr_error_e dnx_mpls_port_to_l2vpn_translate(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port,
    dnx_mpls_l2vpn_info_t * l2vpn);

/**
 * \brief
 *   check if the given vpn is valid as l2 VPN
 * \param [in] unit - Unit ID
 * \param [in] vpn - VPN/VSI ID
 * \return Standard error handling
 */
shr_error_e dnx_mpls_port_l2_vpn_check(
    int unit,
    bcm_vpn_t vpn);
#endif  /*_MPLS_PORT_INCLUDED__*/
