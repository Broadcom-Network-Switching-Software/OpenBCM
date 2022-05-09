/** \file tunnel_init_flow.h
 * Tunnel functionality for DNX tunnel encapsulation using flow lif
*/

#ifndef _TUNNEL_INIT_FLOW_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

/**
  * \brief
 * Tunnel initiator create using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] intf - intf struct
 * \param [in] tunnel - tunnel struct
 */
shr_error_e dnx_tunnel_initiator_create_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel);

/**
  * \brief
 * Tunnel initiator ipv4 get using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] intf - intf
 * \param [in] tunnel - tunnel struct
 */
shr_error_e dnx_tunnel_initiator_ipv4_get_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel);

/**
  * \brief
 * Tunnel initiator ipv6 get using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] intf - intf
 * \param [in] tunnel - tunnel struct
 */
shr_error_e dnx_tunnel_initiator_ipv6_get_flow(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel);

/**
  * \brief
 * Tunnel initiator ipv4 clear using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel_gport - tunnel id
 */
shr_error_e dnx_tunnel_initiator_ipv4_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport);

/**
  * \brief
 * Tunnel initiator ipv6 clear using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel_gport - tunnel id
 */
shr_error_e dnx_tunnel_initiator_ipv6_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport);

/**
  * \brief
 * Tunnel initiator ipv4 traverse using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] cb - initiator cb
 * \param [in] user_data - initiator user_data
 */
shr_error_e dnx_tunnel_initiator_ipv4_traverse_flow(
    int unit,
    bcm_tunnel_initiator_traverse_cb cb,
    void *user_data);

/**
  * \brief
 * Tunnel initiator ipv6 traverse using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] cb - initiator cb
 * \param [in] user_data - initiator user_data
 */
shr_error_e dnx_tunnel_initiator_ipv6_traverse_flow(
    int unit,
    bcm_tunnel_initiator_traverse_cb cb,
    void *user_data);

#endif  /*_TUNNEL_INIT_FLOW_INCLUDED__ */
