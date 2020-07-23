/*
 * phantom_queues_dbal.h
 *
 *  Created on: Dec 11, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _EGRESS_PAHNTOM_QUEUES_DBAL_H_INCLUDED_
#define _EGRESS_PAHNTOM_QUEUES_DBAL_H_INCLUDED_

#include <bcm/types.h>

/**
* \brief -  Enable phantom queues per EGQ IF, and set a threshold profile to take the threshold from
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_if - egq if
* \param [in] enable - enable phantom queues indication
* \param [in] threshold_profile - threshold profile to connect this EGQ IF
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_phantom_queues_dbal_egq_if_enable_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 enable,
    uint32 threshold_profile);

/**
* \brief -  get Enable phantom queues per EGQ IF, and the relevant threshold profile
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_if - egq if
* \param [out] enable - enable phantom queues indication
* \param [out] threshold_profile - threshold profile to connect this EGQ IF
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_phantom_queues_dbal_egq_if_enable_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *enable,
    uint32 *threshold_profile);

/**
* \brief -  set phantom queues threshold value per threshold profile
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] threshold_profile - threshold profile id
* \param [in] threshold_value - threshold value
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_phantom_queues_dbal_threshold_set(
    uint32 unit,
    bcm_core_t core_id,
    uint32 threshold_profile,
    uint32 threshold_value);

/**
* \brief -  get phantom queues threshold value per threshold profile
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] threshold_profile - threshold profile id
* \param [out] threshold_value - threshold value
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_phantom_queues_dbal_threshold_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 threshold_profile,
    uint32 *threshold_value);

/**
* \brief -  global enable for phantom queues feature
*
* \param [in] unit - chip unit id
* \param [in] enable - enable indication
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_phantom_queues_dbal_global_enable_set(
    uint32 unit,
    uint32 enable);

/**
* \brief -  global enable indication for phantom queues feature
*
* \param [in] unit - chip unit id
* \param [out] enable - enable indication
*
* \return
*   shr_error_e
* \see
*   * None
*/
shr_error_e dnx_phantom_queues_dbal_global_enable_get(
    uint32 unit,
    uint32 *enable);

#endif /* _EGRESS_PAHNTOM_QUEUES_DBAL_H_INCLUDED_ */
