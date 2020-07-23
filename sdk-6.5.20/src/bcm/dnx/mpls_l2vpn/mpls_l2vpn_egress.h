/** \file mpls_l2vpn_egress.h
 *      Used by mpls_l2vpn.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _MPLS_L2VPN_EGRESS_INCLUDED__
     /*
      * {
      */
#define _MPLS_L2VPN_EGRESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/*
 * }
 */

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
/*
 * }
 */

/**
* \brief
* Creates the mpls port object described above with its egress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] l2vpn_info  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_add_egress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief
*  Get the mpls port object described above with its egress objects.
* \param [in] unit       - Relevant unit.
* \param [in] l2vpn_info  - Will hold all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_get_egress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief
*  Delete the mpls port object described above with its egress objects.
* \param [in] unit          - Relevant unit.
* \param [in] mpls_l2vpn_id  - The port to be deleted.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_delete_egress_only(
    int unit,
    bcm_gport_t mpls_l2vpn_id);

/**
* \brief
*  Calculate the required result type to be used for the EEDB entry based on label flags and cw indication.
*   \param [in] unit     -  Relevant unit.
*   \param [in] flags     -  Label flags - DNX_MPLS_TUNNEL_ENCAP_XXX.
*   \param [in] has_cw     -  whether cw is resent.
* \return
*   the relevant result type to be used
*/
dbal_enum_value_result_type_eedb_pwe_e dnx_mpls_port_egress_result_type_get(
    int unit,
    uint32 flags,
    int has_cw);

#endif  /*_MPLS_L2VPN_EGRESS_INCLUDED__*/
