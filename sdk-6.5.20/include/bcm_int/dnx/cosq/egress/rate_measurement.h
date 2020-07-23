/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * rate_measurement.h
 *
 *  Created on: Dec 12, 2018
 *      Author: si888124
 */

#ifndef _DNX_RATE_MEASUREMENT_H_INCLUDED_
#define _DNX_RATE_MEASUREMENT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * \brief - Set interval for egress rate measure.
 * interval=0 disables the feature.
 *
 * \param [in] unit - Unit-ID
 * \param [in] interval - interval value [usec]
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_interval_set(
    int unit,
    int interval);

/**
 * \brief - Get interval used for egress rate measure
 *
 * \param [in] unit - Unit-ID
 * \param [out] interval - interval value [usec]
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_interval_get(
    int unit,
    int *interval);

/**
 * \brief - Set rate weight for egress rate measure.
 *
 * \param [in] unit - Unit-ID
 * \param [in] rate_weight - rate weight according to defines BCM_COSQ_RATE_WEIGHT_XXX
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_weight_set(
    int unit,
    int rate_weight);

/**
 * \brief - Get rate weight for egress rate measure.
 *
 * \param [in] unit - Unit-ID
 * \param [out] rate_weight - rate weight according to defines BCM_COSQ_RATE_WEIGHT_XXX
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_weight_get(
    int unit,
    int *rate_weight);

/**
 * \brief - update new port rate in egress rate measure mechanism.
 *
 * \param [in] unit - Unit-ID
 * \param [in] port - logical port
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_port_rate_update_set(
    int unit,
    int port);

/**
 * \brief - set packet compensation for egress rate mechanism
 *
 * \param [in] unit - Unit-ID
 * \param [in] core_id - core
 * \param [in] egq_if - egress if id
 * \param [in] compensation - compensation value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_compensation_set(
    int unit,
    bcm_core_t core_id,
    uint32 egq_if,
    int compensation);

/**
 * \brief - Get packet compensation for egress rate mechanism
 *
 * \param [in] unit - Unit-ID
 * \param [in] core_id - core
 * \param [in] egq_if - egress if id
 * \param [out] compensation - compensation value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_rate_measurement_compensation_get(
    int unit,
    bcm_core_t core_id,
    uint32 egq_if,
    int *compensation);

/**
* \brief -  Get rate counter per EGQ IF
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_if - egq if
* \param [out] rate - rate counter
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_counter_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *rate);

/*
 * Exposing internal declarations -
 * the following function is required in shell command "tm egress rate interface" implementation.
 * see sh_dnx_egr_interface_rate_cmd()
 * for information about these functions, see declaration in rate_measurement_dbal.h
 */
extern shr_error_e dnx_rate_measurement_dbal_rate_promile_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *rate_promile,
    uint32 *granularity);

#endif /* _DNX_RATE_MEASUREMENT_H_INCLUDED_ */
