/** \file vxlan_port_flow.h
 * xvlan port for DNX tunnel termination using flow lif
*/

#ifndef _VXLAN_PORT_FLOW_INCLUDED__
/*
 * {
 */
#define _VXLAN_PORT_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

/************
 * INCLUDES *
 ************/
#include <bcm/vxlan.h>

/************************
 * FUNCTION DECLARATION *
 ************************/
/**
 * \brief
 * update both tunnel termination inlif and tunnel encapsulation
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] vxlan_port - A pointer to the struct that holds information for vxlan_port, \see bcm_vxlan_port_t
 */
shr_error_e dnx_vxlan_port_create_flow(
    int unit,
    bcm_vxlan_port_t * vxlan_port);

/**
 * \brief
 * Get both tunnel termination and tunnel encapsulation information
 * \param [in] unit -
 *   Relevant unit.
 * \param [in,out] vxlan_port -
 *  A pointer to the struct that holds information for vxlan_port, \see bcm_vxlan_port_t.
 */
shr_error_e dnx_vxlan_port_get_flow(
    int unit,
    bcm_vxlan_port_t * vxlan_port);

/**
 * \brief
 * Delete both tunnel termination and tunnel encapsulation information
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] vxlan_port_id -
 *  vxlan port id.
 */
shr_error_e dnx_vxlan_port_delete_flow(
    int unit,
    bcm_gport_t vxlan_port_id);

#endif /** _VXLAN_PORT_FLOW_INCLUDED__ */
