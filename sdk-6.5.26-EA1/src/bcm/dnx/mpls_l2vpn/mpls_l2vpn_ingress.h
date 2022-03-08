/** \file mpls_l2vpn_ingress.h
 *      Used by mpls_l2vpn.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _MPLS_L2VPN_INGRESS_INCLUDED__
          /*
           * {
           */
#define _MPLS_L2VPN_INGRESS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

#include <soc/dnx/dbal/dbal.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>

/**
* \brief
* Creates the mpls port object described above with its ingress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] l2vpn_info  -  Holds all the relevant information for the different schemes that might be defined on a L2VPN LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_add_ingress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief
* Get the mpls port object described above with its ingress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] l2vpn_info  -  Holds all the relevant information for the different schemes that might be defined on a L2VPN LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_get_ingress_only(
    int unit,
    dnx_mpls_l2vpn_info_t * l2vpn_info);

/**
* \brief
* Delete the mpls port object with its ingress objects.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_l2vpn_id  - The port to be deleted.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_l2vpn_delete_ingress_only(
    int unit,
    bcm_gport_t mpls_l2vpn_id);

/**
 * \brief
 * Fill the dnx_mpls_termination_profile_t structure
 * According to the information in bcm_mpls_port_t
 *
 * \param [in] unit - Relevant unit.
 * \param [in] iml_flags - iml flags
 * \param [in] has_cw - is control word enabled
 * \param [in] entropy_enable - is EL enabled
 * \param [in] vccv_type - vccv type
 * \param [out] term_profile_info  -  A pointer to the struct representing the tunnel term profile entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_l2vpn_to_termination_profile(
    int unit,
    uint32 iml_flags,
    uint32 has_cw,
    uint32 entropy_enable,
    bcm_mpls_port_control_channel_type_t vccv_type,
    dnx_mpls_termination_profile_t * term_profile_info);

/**
 * \brief Sets matching information fields in 'l2vpn' structure according to the matching information in 'match'.
 * \param [in] unit - Relevant unit.
 * \param [in] match - contains the match criteria and information relevant to this critiria
 * \param [out] l2vpn - mpls_l2vpn structure with BCM API facing information.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_match_info_to_l2vpn(
    int unit,
    dnx_mpls_term_match_t * match,
    dnx_mpls_l2vpn_info_t * l2vpn);

/**
 * \brief
 *   Fill MPLS match information SW state. InLIF will return
 *   MPLS label that is the key to that lif.
 * \param [in] unit - unit number.
 * \param [in] core_id - core id in case only one core is set. _SHR_CORE_ALL for both.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \param [in] match - match information and criteria to be stored.
 * \return
 *   shr_error_e - Non-zero in case of an error.
 */
shr_error_e dnx_mpls_l2vpn_termination_match_information_set(
    int unit,
    int core_id,
    int local_inlif,
    dnx_mpls_term_match_t * match);
/**
 * \brief
 *   Read MPLS match information SW state. InLIF will return
 *   MPLS label that is the key to that lif.
 * \param [in] unit - Relevant unit.
 * \param [in] core_id - core id in case only one core is set. _SHR_CORE_ALL for both.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \param [out] match - matching information and criteria to be retrieved.
 * \return
 *   shr_error_e - Non-zero in case of an error.
 */
shr_error_e dnx_mpls_l2vpn_termination_match_information_get(
    int unit,
    int core_id,
    int local_inlif,
    dnx_mpls_term_match_t * match);

/**
 * \brief Fills the match information for L2VPN object.
 * For PWE/EVPN, this will be the match label.
 * For IML, this will be the match label + BOS expectation
 * \param [in] unit
 * \param [in] flags
 * \param [in] mpls_port - Input structure with the relevant information of mpls port
 * \param [out] match - Output structure with the matching information
 */
shr_error_e dnx_mpls_l2vpn_match_info_fill(
    int unit,
    uint32 flags,
    bcm_mpls_port_t * mpls_port,
    dnx_mpls_term_match_t * match);

/**
 * \brief
 *   Free MPLS match information SW state.
 * \param [in] unit - unit number.
 * \param [in] core_id - core id in case only one core is set. _SHR_CORE_ALL for both.
 * \param [in] local_inlif - local inlif is the key to the db.
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 */
shr_error_e dnx_mpls_l2vpn_termination_match_information_delete(
    int unit,
    int core_id,
    int local_inlif);

/**
* \brief
* Translates the fwd information in mpls port structure to internal fwd information structure.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
*   \param [out] forward_info - Forward information buffer.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_port_to_forward_information(
    int unit,
    bcm_mpls_port_t * mpls_port,
    dnx_algo_gpm_forward_info_t * forward_info);

/**
* \brief
* Get gport from the accompanying forwarding information.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] gport      -  global lif gport
*   \param [in] mpls_port  -  Holds all the relevant information for the different schemes that might be defined on a VPLS LIF.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_port_gport_to_forward_information_get(
    int unit,
    bcm_gport_t gport,
    bcm_mpls_port_t * mpls_port);

/**
* \brief
* delete mpls port forwarding information.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] mpls_port_id  -  mpls port ID.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mpls_port_gport_to_forward_information_delete(
    int unit,
    bcm_gport_t mpls_port_id);
#endif  /*_MPLS_L2VPN_INGRESS_INCLUDED__*/
