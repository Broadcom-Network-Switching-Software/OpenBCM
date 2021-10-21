/** \file tunnel_init_flow.h
 * Tunnel functionality for DNX tunnel encapsulation using flow lif
*/

#ifndef _TUNNEL_INIT_FLOW_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
 * Tunnel initiator get using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - tunnel struct
 */
shr_error_e dnx_tunnel_initiator_get_flow(
    int unit,
    bcm_tunnel_initiator_t * tunnel);

/**
  * \brief
 * Tunnel initiator clear using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel_gport - tunnel id
 */
shr_error_e dnx_tunnel_initiator_clear_flow(
    int unit,
    bcm_gport_t tunnel_gport);
#endif  /*_TUNNEL_INIT_FLOW_INCLUDED__ */
