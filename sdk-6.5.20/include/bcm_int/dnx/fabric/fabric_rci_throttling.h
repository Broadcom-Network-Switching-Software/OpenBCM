/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/fabric/fabric_rci_throttling.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_RCI_THROTTLING_H_INCLUDED_
/*
 * { 
 */
#define _DNX_FABRIC_RCI_THROTTLING_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - 
 * function for managing RCI throttling  mechanism mode
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] mode - RCI throttling mechanism mode see 
 *        bcm_fabric_control_rci_throttling_mode_t
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * bcm_fabric_control_rci_throttling_mode_t
 */
shr_error_e dnx_fabric_rci_throttling_mode_set(
    int unit,
    bcm_fabric_control_rci_throttling_mode_t mode);

/**
 * \brief -
 * get RCI throttling mode from HW
 */
shr_error_e dnx_fabric_rci_throttling_mode_get(
    int unit,
    bcm_fabric_control_rci_throttling_mode_t * mode);

#endif /*_DNX_SCHEDULER_H_INCLUDED_*/
