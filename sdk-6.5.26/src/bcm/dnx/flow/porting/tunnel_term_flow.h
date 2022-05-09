/** \file tunnel_term_flow.h
 * Tunnel functionality for DNX tunnel termination using flow lif
*/

#ifndef _TUNNEL_TERM_FLOW_INCLUDED__
/*
 * {
 */
#define _TUNNEL_TERM_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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
 * SRV6 Tunnel terminator create using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - terminator tunnel
 */
shr_error_e dnx_ipv6_srv6_delete_flow(
    int unit,
    bcm_tunnel_terminator_t * tunnel);

/**
  * \brief
 * Tunnel terminator ipv4 traverse using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] cb - terminator cb
 * \param [in] user_data - terminator user_data
 */
shr_error_e dnx_tunnel_terminator_ipv4_traverse_flow(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data);

/**
  * \brief
 * Tunnel terminator ipv6 traverse using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] cb - terminator cb
 * \param [in] user_data - terminator user_data
 */
shr_error_e dnx_tunnel_terminator_ipv6_traverse_flow(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data);

/**
 * \brief
 * Tunnel terminator config traverse using flow lif
 *
 * \param [in] unit -
 *     The unit number.
 * \param [in] cb - terminator config cb
 * \param [in] user_data - terminator user_data
 */
shr_error_e dnx_tunnel_term_config_traverse_flow(
    int unit,
    bcm_tunnel_terminator_config_traverse_cb cb,
    void *user_data);

#endif  /*_TUNNEL_TERM_FLOW_INCLUDED__ */
