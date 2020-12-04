/** \file port_utils.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_UTILS_H_INCLUDED_
/** { */
#define _PORT_UTILS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm/port.h>

/**
 * \brief - internal function to the bcm_port_add API. 
 * this function is used to retrieve the added port and call the IMB layer 
 * port_add API to open the port path in the IMB.
 * 
 * \param [in] unit - chip unit id
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * imb_port_add
 */
int dnx_port_imb_path_port_add_handle(
    int unit);

/**
 * \brief - internal function to the bcm_port_remove API. 
 * this function is used to retrieve the removed port and call 
 * the IMB layer port_remove API to close the port path in the 
 * IMB. 
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * imb_port_remove
 */
int dnx_port_imb_path_port_remove_handle(
    int unit);

/*
 * \brief PORT Interfaces procedures for port_control_set
 * see port_if.c for details
 */
int dnx_port_if_control_set(
    int,
    bcm_port_t,
    bcm_port_control_t,
    int);
/*
 * \brief PORT Interfaces procedures for port_control_get
 * see port_if.c for details
 */
int dnx_port_if_control_get(
    int,
    bcm_port_t,
    bcm_port_control_t,
    int *);

/**
 * \brief - INTERNAL FUNCTION initialize the Port module
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e dnx_port_init(
    int unit);

/**
 * \brief - initialize the Port module
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e dnx_port_deinit(
    int unit);

/**
 * \brief - Get support action mask for port info (bcm_port_info_t)
 */
shr_error_e dnx_port_info_support_action_mask_get(
    int unit,
    bcm_port_t port,
    uint32 *mask,
    uint32 *mask2);

/**
 * \brief - initialize the Port module
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port to be added
 * \param [in] phys - phys of the port to be added
 * \return
 *   shr_error_e 
 *   
 * \remarks
 * This is implemented for calling port checks in an early stage
 * before port is actually added to any SW DB
 * \see
 *   * None
 */
int dnx_port_pre_add_validate(
    int unit,
    int port,
    bcm_pbmp_t phys);

/**
 * \brief - INTERNAL FUNCTION initialize port interrupts
 *
 * \param [in] unit - chip unit id.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e dnx_port_interrupt_init(
    int unit);

#endif /*_PORT_UTILS_H_INCLUDED_*/
