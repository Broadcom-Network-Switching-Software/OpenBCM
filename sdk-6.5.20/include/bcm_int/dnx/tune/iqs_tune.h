/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file bcm_int/dnx/tune/iqs_tune.h
 * Tuning for ingress queue scheduler.
 *
 * Reserved.$ 
 */

#ifndef _DNX_IQS_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_IQS_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>
#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - Getting the recommended values for credit request profile.
 *          This set of thresholds could be assign to credit request profile by API bcm_cosq_delay_tolerance_level_set()
 * 
 * \param [in] unit -  Unit-ID
 * \param [in] preset_attr - profile attributes used to calculate the recommended thresholds
 * \param [out] delay_tolerance - the recommended thresholds
 *
 * \return
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_iqs_delay_tolerance_preset_get(
    int unit,
    bcm_cosq_delay_tolerance_preset_attr_t * preset_attr,
    bcm_cosq_delay_tolerance_t * delay_tolerance);

/**
 * \brief -
 * Convert rate to required credit resolution
 *
 * \param [in] unit -  Unit-ID
 * \param [in] bw_level - level #
 * \param [out] credit_resolution -  required credit resolution
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_iqs_bw_level_to_credit_resolution_get(
    int unit,
    int bw_level,
    int *credit_resolution);

/**
 * \brief - Tune Ingress Queue Scheduler:
 *          Enable and configures credit watchdog mechanism
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_iqs_init(
    int unit);

/** } */
#endif /*_DNX_IQS_TUNE_H_INCLUDED_*/
