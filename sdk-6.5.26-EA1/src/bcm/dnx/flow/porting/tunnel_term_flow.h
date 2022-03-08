/** \file tunnel_term_flow.h
 * Tunnel functionality for DNX tunnel termination using flow lif
*/

#ifndef _TUNNEL_TERM_FLOW_INCLUDED__
/*
 * {
 */
#define _TUNNEL_TERM_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/**
  * \brief
 * Tunnel terminator create using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - terminator tunnel
 */
shr_error_e dnx_tunnel_term_create_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel);

/**
  * \brief
 * SRV6 Tunnel terminator create using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - terminator tunnel
 */
shr_error_e dnx_ipv6_srv6_create_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel);

/**
  * \brief
 * Tunnel terminator get using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [out] tunnel - terminator tunnel
 */
shr_error_e dnx_tunnel_term_get_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel);

/**
  * \brief
 * Tunnel terminator delete using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [out] tunnel - terminator tunnel
 */
shr_error_e dnx_tunnel_term_delete_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel);
#endif  /*_TUNNEL_TERM_FLOW_INCLUDED__ */
