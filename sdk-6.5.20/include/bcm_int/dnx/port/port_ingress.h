/** \file port_ingress.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_INGRESS_H_INCLUDED_
/** { */
#define _PORT_INGRESS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief - Callback to configure ingress port information
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
 *   *  HW configuration of ingress port
 * \remark
 *   * Invoked during bcm_dnx_port_add
 * \see
 *   * None
 */
shr_error_e dnx_port_ingress_port_add_handle(
    int unit);

/**
 * \brief - Callback to detach ingress port info
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
shr_error_e dnx_port_ingress_port_remove_handle(
    int unit);

/**
 * \brief - Initialize dnx port_ingress  module
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
shr_error_e dnx_port_ingress_init(
    int unit);

/** } */
#endif /** _PORT_INGRESS_H_INCLUDED_ */
