/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file source/bcm/dnx/tune/egr_queuing_tune.h
 * Reserved.$ 
 */

#ifndef _DNX_EGR_QUEUING_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_EGR_QUEUING_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief -  configures port, interface, queues and TCGs rate to the with the interface speed.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  logical port which need the rate tuning
 * \param [in] if_speed -  interface speed
 * \param [in] commit_changes - If set, the changes will be committed to the HW
 * \param [in] is_remove - Set when calling the API during port removal. Clear if calling it otherwise (during port add / speed change / other tuning).
 *  *
 * \return
 *   shr_error_e
 * \remark
 *   * This function is also used as cint example 
 *     (called by the cint function add_port_full_example)
 * \see
 *   * None
 */
shr_error_e dnx_tune_egr_ofp_rate_set(
    int unit,
    bcm_port_t port,
    int if_speed,
    int commit_changes,
    int is_remove);

/**
 * \brief -  egress queuing rate implementation for device tuning.
 *
 * \param [in] unit -  Unit-ID
  *  *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_egr_queuing_rate_tune(
    int unit);

/**
 * \brief -  egress queuing implementation for bcm_port_add tuning.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  logical port-ID
 *  *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_egr_queuing_port_add(
    int unit,
    bcm_port_t port);

/** } */
#endif /*_DNX_EGR_QUEUING_TUNE_H_INCLUDED_*/
