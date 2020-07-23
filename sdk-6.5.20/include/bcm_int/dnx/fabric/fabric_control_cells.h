/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/fabric/fabric_control_cells.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_CONTROL_CELLS_H_INCLUDED_
#define _DNX_FABRIC_CONTROL_CELLS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

/*
 * Typedefs:
 * {
 */
/*
 * } 
 */

/*
 * Functions:
 * {
 */

/**
 * \brief
 *   Get whether the receiving and transmitting control cells is enabled
 *   in the device.
 * \param [in] unit -
 *   The unit number.
 * \param [out] enable -
 *   1 - enabled
 *   0 - disabled
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_ctrl_cells_enable_get(
    int unit,
    int *enable);

/**
 * \brief
 *   Enable/Disable the device from receiving and transmitting control cells.
 * \param [in] unit -
 *   The unit number.
 * \param [in] enable -
 *   1 - enable
 *   0 - disable
 * \param [in] is_soft_reset -
 *   Is this function is called as part of soft reset.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_ctrl_cells_enable_set(
    int unit,
    int enable,
    int is_soft_reset);

/**
 * \brief
 *   Decide whether the device is standalone or not according to the
 *   received control cells.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_ctrl_cells_standalone_state_modify(
    int unit);

/*
 * } 
 */

#endif /*_DNX_FABRIC_CONTROL_CELLS_H_INCLUDED_*/
