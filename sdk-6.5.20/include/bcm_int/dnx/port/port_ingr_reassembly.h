/** \file include/bcm_int/dnx/port/port_ingr_reassembly.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_INGR_REASSEMBLY_H_INCLUDED_
/** { */
#define _PORT_INGR_REASSEMBLY_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/**
 * \brief Used to indicate port does not exist in ingress
 */
#define DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT 0xffffffff

/**
 * \brief - Configure reassembly and port termination context for port 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  Port-ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly and port termination context
 * \remark
 *   * Invoked from port_add callback
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_port_add(
    int unit,
    bcm_port_t port);

/**
 * \brief - Detach reassembly and port termination context from port 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  Port-ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly and port termination context
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_port_remove(
    int unit,
    bcm_port_t port);

/**
 * \brief - Get reassembly and port termination context of port 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  Port-ID
 *   \param [in] priority -  Port priority
 *   \param [in] reassembly_context - pointer to return reassembly context value
 *   \param [in] port_termination_context - pointer to return port termination context value
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_and_port_termination_get(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context,
    uint32 *port_termination_context);

/**
 * \brief - Get reassembly and port termination context of port 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  Port-ID
 *   \param [in] reassembly_context[] - array to return reassembly context values (per priority)
 *   \param [in] port_termination_context - pointer to return port termination context value
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_and_port_termination_get_all(
    int unit,
    bcm_port_t port,
    uint32 reassembly_context[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_PRIORITIES_NOF],
    uint32 *port_termination_context);

/**
 * \brief - Special version returning reassembly context for CGM 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  Port-ID
 *   \param [in] reassembly_context[] - array to return reassembly context values (per priority)
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_context_for_cgm_get_all(
    int unit,
    bcm_port_t port,
    uint32 reassembly_context[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF]);

/**
 * \brief - Configure reassembly context for port according to priority
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] port -  Port-ID
 *   \param [in] priority -  Port priority
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   *  None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_map_set(
    int unit,
    bcm_port_t port,
    int priority);

/**
 * \brief - Callback to configure reassembly and port termination context
 *  for port to be added
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * port which is in process of being added
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly and port termination context
 * \remark
 *   * Invoked during bcm_dnx_port_add
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_port_add_handle(
    int unit);

/**
 * \brief - Callback to detach reassembly and port termination context
 *  from port which is in process of removing
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * port which is in process of being removed
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly and port termination context
 * \remark
 *   * Invoked during bcm_dnx_port_remove
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_port_remove_handle(
    int unit);

/**
 * \brief - unset reassembly context of port due to priority removal
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  port-ID 
 *   \param [in] priority -  removed priority
 */
shr_error_e dnx_port_ingr_reassembly_priority_unset(
    int unit,
    bcm_port_t port,
    int priority);

/**
 * \brief - Initialize dnx port_ingr_reassembly  module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_init(
    int unit);

/**
 * \brief - set reassembly context for a port 
 *  according to existing priorities
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  port id
 *   \param [in] per_priority_exist[] - contains 1 iff corresponding priority exists
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly context
 * \remark
 *    * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_per_priority_context_set(
    int unit,
    bcm_port_t port,
    int per_priority_exist[]);

/**
 * \brief - unset reassembly context for a port 
 *  according to non existing priorities
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] port -  port id
 *   \param [in] per_priority_exist[] - contains 1 iff corresponding priority exists
 *   
 * \par INDIRECT INPUT:
 *   * None  
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of reassembly context
 * \see
 *   * None
 */
shr_error_e dnx_port_ingr_reassembly_per_priority_context_unset(
    int unit,
    bcm_port_t port,
    int per_priority_exist[]);

/** } */
#endif /*_PORT_INGR_REASSEMBLY_H_INCLUDED_*/
