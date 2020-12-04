/** \file src/bcm/dnx/port/port_ingr_reassembly.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SRC_BCM_PORT_INGR_REASSEMBLY_H_INCLUDED_
#define _SRC_BCM_PORT_INGR_REASSEMBLY_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
/**
 * \brief - return whether an ILKN channel is enabled at ingress
 */
shr_error_e dnx_port_ingr_reassembly_enable_get(
    int unit,
    bcm_port_t port,
    int *enable);

/**
 * \brief - enable/disable single ILKN channel at ingress
 */
shr_error_e dnx_port_ingr_reassembly_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - get reassembly context and port termination context
 * consider disabled contexts as well
 */
shr_error_e dnx_port_ingr_reassembly_reassembly_and_port_termination_get_with_disabled(
    int unit,
    bcm_port_t port,
    int priority,
    int consider_disabled,
    uint32 *reassembly_context,
    uint32 *port_termination_context);

/**
 * \brief - For ILKN interface, disable the 144B check for SOP
 */
shr_error_e dnx_port_ingr_reassembly_sop_check_config(
    int unit,
    bcm_port_t port);

#endif
