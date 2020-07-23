/*
 * ! \file bcm_int/dnx/mirror/mirror_rcy.h Internal DNX MIRROR APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MIRROR_RCY__H__INCLUDED__
/*
 * { 
 */
#define _DNX_MIRROR_RCY__H__INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

typedef enum
{
    /*
     *  In HW, The priorities are named as following:
     * * 0 - Lossless, named DNX_MIRROR_RCY_PRIORITY_LOSSLESS here
     * * 1 - High, named DNX_MIRROR_RCY_PRIORITY_NORMAL here
     * * 2 - Low. not actually implemented, so it is not exposed here. If need to expose, should be named DNX_MIRROR_RCY_PRIORITY_NORMAL_2, 
     *       as there is no prioritization between priority 1 and 2
     */
    DNX_MIRROR_RCY_PRIORITY_LOSSLESS = 0,
    DNX_MIRROR_RCY_PRIORITY_NORMAL = 1,
    DNX_MIRROR_RCY_PRIORITY_NOF
} dnx_mirror_rcy_priority_t;

/**
 * \brief - Return egress interface ID mapped to the provided mirror port
 */
shr_error_e dnx_mirror_port_mapped_egress_interface_get(
    int unit,
    bcm_port_t rcy_mirror_port,
    int *mapped_egress_interface);

/**
 * \brief - Callback to configure mirror egress information
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
shr_error_e dnx_mirror_rcy_port_add_handle(
    int unit);

/**
 * \brief - Callback to detach mirror egress info
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
shr_error_e dnx_mirror_rcy_port_remove_handle(
    int unit);

#endif /** _DNX_MIRROR_EGRESS__H__INCLUDED__ */
