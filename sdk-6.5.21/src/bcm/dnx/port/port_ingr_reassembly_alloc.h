/** \file port_ingr_reassembly_alloc.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_INGR_REASSEMBLY_ALLOC_H_INCLUDED_
/** { */
#define _PORT_INGR_REASSEMBLY_ALLOC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief - should alloc be called for the provided port and priority
 */
shr_error_e dnx_port_ingr_reassembly_should_call_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    int *call_alloc);

/**
 * \brief - allocate reassembly context for the port
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_context_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context);

/**
 * \brief - deallocate reassembly context for the port
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_context_dealloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context,
    int force_dealloc,
    int *is_dealloc);

/**
 * \brief - mark reassembly context as allocated on all cores
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_context_mark_allocated(
    int unit,
    uint32 reassembly_context);

/**
 * \brief - Initialize resource manager used by port_ingr_reassembly dnx module
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
shr_error_e dnx_port_ingr_reassembly_res_mngr_init(
    int unit);

/**
 * \brief - Get number of contexts per RCY interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [out] nof_contexts_per_rcy_if - number of contexts per RCY interface
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
shr_error_e dnx_port_ingr_reassembly_nof_contexts_per_rcy_if_get(
    int unit,
    int *nof_contexts_per_rcy_if);

/** } */
#endif /*_PORT_INGR_REASSEMBLY_ALLOC_H_INCLUDED_*/
