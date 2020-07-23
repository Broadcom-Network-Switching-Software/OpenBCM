/*
 * rate_measurement_dbal.h
 *
 *  Created on: Dec 11, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _RATE_MEASUREMENT_DBAL_H_INCLUDED_
#define _RATE_MEASUREMENT_DBAL_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>

/**
* \brief -  Set interval for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [in] interval - interval (clocks)
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_interval_set(
    uint32 unit,
    uint32 interval);

/**
* \brief -  Get interval for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [out] interval - interval (clocks)
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_interval_get(
    uint32 unit,
    uint32 *interval);

/**
* \brief -  Set rate weight for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [in] rate_weight - rate weight according to dbal_enum_value_field_egress_rate_weight_e
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_rate_weight_set(
    uint32 unit,
    dbal_enum_value_field_egress_rate_weight_e rate_weight);

/**
* \brief -  Get rate weight for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [out] rate_weight - rate weight according to dbal_enum_value_field_egress_rate_weight_e
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_rate_weight_get(
    uint32 unit,
    dbal_enum_value_field_egress_rate_weight_e * rate_weight);

/**
* \brief -  Set compensation for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_if - egq if
* \param [in] compensation_abs - compensation value
* \param [in] compensation_sign - indication if compensation is negative or positive (if set, compensation is  negative)
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_compensation_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 compensation_abs,
    uint32 compensation_sign);

/**
* \brief -  Get compensation for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_if - egq if
* \param [out] compensation_abs - compensation value
* \param [out] compensation_sign - indication if compensation is negative or positive (if set, compensation is  negative)
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_compensation_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *compensation_abs,
    uint32 *compensation_sign);

/**
* \brief -  Set port rate promile and granularity for egress rate measurements
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_if - egq if
* \param [in] rate_promile - number of bytes to represent 0.1% of the rate for the specified interval
* \param [in] granularity - number of times we access the rate counter to update. according to the following calculation:
*               if      (PortPromile >= 700)        PortGranularity = 0;
*               else if (PortPromile * 2 >= 700)    PortGranularity = 1;
*               else if (PortPromile * 4 >= 700)    PortGranularity = 2;
*               else if (PortPromile * 8 >= 700)    PortGranularity = 3;
*               else if (PortPromile * 16 >= 700)   PortGranularity = 4;
*               else if (PortPromile * 32 >= 700)   PortGranularity = 5;
*               else if (PortPromile * 64 >= 700)   PortGranularity = 6;
*               else if (PortPromile * 128 >= 700)  PortGranularity = 7;
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_rate_promile_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 rate_promile,
    uint32 granularity);

/**
* \brief -  Set EGQ IF to get its rate counter
*
* \param [in] unit - chip unit id
* \param [in] egq_if - egq if
* \param [in] core_id - core id
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_rate_measurement_dbal_rate_counter_config_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if);

/**
* \brief -  get Rate counter for the corresponding EGQ IF. Note: this function should be called
* only after calling dnx_rate_measurement_dbal_rate_counter_config_set to set which EGQ IF rate to get
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [out] rate - rate counter
* \return
*   shr_error_e
* \see
*   dnx_rate_measurement_dbal_rate_counter_config_set
*/
shr_error_e dnx_rate_measurement_dbal_rate_counter_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 *rate);

#endif /* _RATE_MEASUREMENT_DBAL_H_INCLUDED_ */
