/*
 * ! \file bcm_int/dnx/fabric/fabric_control_cells.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_CONTROL_CELLS_H_INCLUDED_
#define _DNX_FABRIC_CONTROL_CELLS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

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
 *   Enable delay test control cells
 * \param [in] unit - Unit #
 * \param [in] is_soft_reset - is_soft_reset #
 * \param [in] enable_fabric_traffic - if to also enable the fabric traffic #
 *
 * \remark
 *
 */
shr_error_e dnx_fabric_delay_test_ctrl_cells_enable_set(
    int unit,
    int is_soft_reset,
    int enable_fabric_traffic);

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

/**
  * \brief
  *   Does polling to check if MESH TOPOLOGY is synced.
  * \param [in] unit -
  *   The unit number.
  * \param [in] is_soft_reset -
  *   Indicates if this procedure is called under soft reset.
  * \return
  *   See \ref shr_error_e
  * \remark
  *   * None.
  * \see
  *   * None.
  */
shr_error_e dnx_fabric_ctrl_cells_mesh_topology_sync(
    int unit,
    int is_soft_reset);
/*
 * } 
 */

#endif /*_DNX_FABRIC_CONTROL_CELLS_H_INCLUDED_*/
