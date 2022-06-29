/** \file esem_flow.h
 * ESEM functionality for DNX using flow lif
*/

#ifndef _ESEM_FLOW_INCLUDED__
/*
 * {
 */
#define _ESEM_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>

/**
  * \brief
 * ESEM create using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in,out] vlan_port - vlan port
 */
shr_error_e dnx_esem_flow_create(
    int unit,
    bcm_vlan_port_t * vlan_port);

/**
  * \brief
 * ESEM get using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in,out] vlan_port - vlan port
 */
shr_error_e dnx_esem_flow_get(
    int unit,
    bcm_vlan_port_t * vlan_port);

/**
  * \brief
 * ESEM clear using flow lif
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] gport - VLAN PORT ID (Gport encoded with at least a type and subtype)
 */
shr_error_e dnx_esem_flow_delete(
    int unit,
    bcm_gport_t gport);

/**
  * \brief
 * Get ESEM/ESEM-Default flow handle
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] vlan_port - vlan_port structure
 * \param [out] flow_handle -flow handle
 * \param [out] is_lif_scoped - relevant for ESEM native only
 */
shr_error_e dnx_esem_initiator_flow_handle_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_flow_handle_t * flow_handle,
    int *is_lif_scoped);

/**
  * \brief
 * Get ESEM/ESEM-Default flow handle using gport
 * header profile
 * \param [in] unit -
 *     The unit number.
 * \param [in] gport - gport
 * \param [out] flow_handle -flow handle
 */
shr_error_e dnx_esem_initiator_flow_handle_from_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_flow_handle_t * flow_handle);

#endif  /*_ESEM_FLOW_INCLUDED__ */
