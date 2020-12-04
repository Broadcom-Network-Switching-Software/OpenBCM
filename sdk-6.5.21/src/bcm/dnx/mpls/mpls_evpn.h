/** \file mpls_evpn.h
 *      Used by mpls to handle EVPN configuration through
 *      MPLS APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __MPLS_EVPN_INCLUDED__
#define __MPLS_EVPN_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/*
 * MACROS
 * {
 */

/**
 * \brief Is the switch object EVPN UC label?
 */
#define DNX_MPLS_SWITCH_IS_EVPN_UC(p_info)      (_SHR_IS_FLAG_SET((p_info)->flags, BCM_MPLS_SWITCH_NEXT_HEADER_L2))
/**
 * \brief Is the switch object EVPN MC label (IML)?
 */
#define DNX_MPLS_SWITCH_IS_EVPN_MC(p_info)      (_SHR_IS_FLAG_SET((p_info)->flags, BCM_MPLS_SWITCH_EVPN_IML))
/**
 * \brief Is the switch object EVPN UC/MC?
 */
#define DNX_MPLS_SWITCH_IS_EVPN(p_info)   (DNX_MPLS_SWITCH_IS_EVPN_UC(p_info) || DNX_MPLS_SWITCH_IS_EVPN_MC(p_info))
/**
 * \brief Is the tunnel initiator object EVPN (or IML)?
 */
#define DNX_MPLS_TUNNEL_INIT_IS_EVPN(p_eg_label)   (_SHR_IS_FLAG_SET((p_eg_label)->flags, BCM_MPLS_EGRESS_LABEL_EVPN) \
                                                 || _SHR_IS_FLAG_SET((p_eg_label)->flags, BCM_MPLS_EGRESS_LABEL_IML))

/*
 * }
 * MACROS
 */

/*
 * UTILS
 * {
 */

/**
 * \brief Finds a local in lif according to 'info' and checks
 * if this local in-LIF pointer is for EVPN or not. in 'info'
 * is NULL, skip the in-lif finding and assumes local_in_lif
 * points to a valid in lif value; Just returns whether the lif
 * is EVPN/IML in this case.
 * \param [in] unit
 * \param [in] info - All tunnel switch information, can be NULL
 * \param [in,out] local_in_lif - the in lif found for the supplied information, or
 *                                  externally supplied in lf in case info is NULL
 * \param [out] is_evpn - Is the in lif used for EVPN/IML
 */
shr_error_e dnx_mpls_evpn_local_in_lif_is_evpn(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    int *local_in_lif,
    uint8 *is_evpn);

/**
 * \brief Checks if the supplied egress label gport is
 * EVPN/IML encapsulation or not. Returns the gport
 * of the egress label outlif. If this is a EVPN/IML
 * egress label, this will be an MPLS Port Gport, and
 * not tunnel.
 */
shr_error_e dnx_mpls_evpn_egress_label_is_evpn(
    int unit,
    bcm_if_t intf,
    bcm_gport_t * gport,
    uint8 *is_evpn);
/*
 * }
 * UTILS
 */

/*
 * INTERNAL APIs
 * {
 */

/**
 * \brief Adds EVPN/IML ingress label termination.
 */
shr_error_e dnx_mpls_evpn_term_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info);

/**
 * \brief Gets EVPN/IML ingress label information
 */
shr_error_e dnx_mpls_evpn_term_get(
    int unit,
    uint32 local_in_lif,
    bcm_mpls_tunnel_switch_t * info);

/**
 * \brief Deletes EVPN/IML ingress label termination object
 */
shr_error_e dnx_mpls_evpn_term_delete(
    int unit,
    int local_in_lif);

/**
 * \brief Adds EVPN/IML egress label encapsulation.
 */
shr_error_e dnx_mpls_evpn_encap_add(
    int unit,
    bcm_mpls_egress_label_t * eg_label);

/**
 * \brief Gets EVPN/IML egress label encapsulation
 */
shr_error_e dnx_mpls_evpn_encap_get(
    int unit,
    bcm_gport_t gport,
    bcm_mpls_egress_label_t * eg_label,
    int *label_count);

/**
 * \brief Delete EVPN/IML egress label encapsulation
 */
shr_error_e dnx_mpls_evpn_encap_delete(
    int unit,
    bcm_gport_t gport);

/*
 * }
 * INTERNAL APIs
 */

#endif  /*__MPLS_EVPN_INCLUDED__*/
