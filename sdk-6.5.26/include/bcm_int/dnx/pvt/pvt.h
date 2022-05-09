/** \file bcm_int/dnx/pvt/pvt.h
 * 
 * Internal DNX PVT APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef PVT_H_INCLUDED
/* { */
#define PVT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/**
 * \brief
 *   Initialize PVT monitor module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pvt_mon_init(
    int unit);
/**
 * \brief
 *   Change PVTMON select.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] voltage -
 *   voltage selection
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pvt_mon_vol_change(
    int unit,
    int voltage);

/**
* \brief
*   API gets temperature info for PVT monitors
*   modes
* \par DIRECT INPUT:
*   \param [in] unit - unit ID
*   \param [in] voltage_max - Maximal number of acceptable temperature sensors
*   \param [out] voltage - Pointer to array to be loaded by this procedure. Each
*        element contains current voltage and peak voltage
*   \param [out] voltage_remainder - Pointer to array to be loaded by this procedure. Each
*        element contains current voltage remainder and peak voltage remainder
*   \param [out] voltage_count  - the number of sensors (and, therefore, the number of valid
*        elements on voltage).
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   *
* \see
*   * None
*/
int dnx_pvt_mon_voltage_get(
    int unit,
    int voltage_max,
    bcm_switch_temperature_monitor_t * voltage,
    bcm_switch_temperature_monitor_t * voltage_remainder,
    int *voltage_count);

/**
* \brief
*   Lock PVTMON reading temperature
*   modes
* \par DIRECT INPUT:
*   \param [in] unit - unit ID
*   \param [in] lock - lock/unlock pvtmon
* \return
*    See \ref shr_error_e
* \remark
*   * None
*/
shr_error_e dnx_pvt_mon_lock(
    int unit,
    int lock);

/* } */
#endif /* PVT_H_INCLUDED */
