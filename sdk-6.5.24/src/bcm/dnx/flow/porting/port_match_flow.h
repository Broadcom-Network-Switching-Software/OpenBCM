/** \file port_match_flow.h
 * Port match using flow lif
*/

#ifndef _PORT_MATCH_FLOW_INCLUDED__
/*
 * {
 */
#define _PORT_MATCH_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

/**
 * \brief -
 * Add a match to an existing port using Flow lif
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to the match information.

 * \return
 *   shr_error_e
 */
shr_error_e dnx_port_match_create_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);

/**
 * \brief -
 * Deleting a given match to an existing gport using Flow lif
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Physical port, virtual port or LIF-ID in gport format.
 * \param [in] match_info - pointer to the match information.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_port_match_clear_flow(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);

#endif  /*_PORT_MATCH_FLOW_INCLUDED__ */
