/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file source/bcm/dnx/tune/scheduler_tune.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_SCHEDULER_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief -  configures port, interface, TC and TCGs rate according to interface speed.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  logical port which need the rate tuning
 * \param [in] flags - each bit represent a speed type, such as \
 *                  - DNX_ALGO_PORT_SPEED_F_MBPS \
 *                  - DNX_ALGO_PORT_SPEED_F_KBPS \
 * \param [in] if_speed -  interface speed in Kbits/sec or Mbits/sec
 * \param [in] commit_changes -  if set, the changes will be committed to the HW
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
shr_error_e dnx_tune_scheduler_port_rates_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int if_speed,
    int commit_changes,
    int is_remove);

/**
 * \brief - perform initial tuning of Scheduler
 * 
 * \param [in] unit -  Unit-ID   
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_scheduler_tune(
    int unit);

/** } */
#endif /** _DNX_SCHEDULER_TUNE_H_INCLUDED_ */
