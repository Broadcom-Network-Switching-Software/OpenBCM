/*
 * \file bcm_int/dnx/tsn/port_preemption.h
 * 
 *
 *  Created on: March 11, 2022
 *      Author: dk032354
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_PORT_PREEMPTION_INCLUDED_
#define _DNX_PORT_PREEMPTION_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * \brief -
 *  Set various configurations for Preemption.
 *
 * \param [in] unit - the relevant unit
 * \param [in] gport - logical port number
 * \param [in] type - preemption control type defined in bcm_port_preempt_control_t
 * \param [in] arg - the preemption control value
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_preemption_control_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 arg);

/**
 * \brief -
 *  Get various configurations for Preemption.
 *
 * \param [in] unit - the relevant unit
 * \param [in] gport - logical port number
 * \param [in] type - preemption control type defined in bcm_port_preempt_control_t
 * \param [out] arg - the preemption control value
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_preemption_control_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 *arg);

/**
 * \brief -
 *  Get the specified preemption status.
 *
 * \param [in] unit - the relevant unit
 * \param [in] gport - logical port number
 * \param [in] type - preemption status type defined in bcm_port_preempt_status_t
 * \param [out] status - the preemption status
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_preemption_status_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_status_t type,
    uint32 *status);

#endif /* _DNX_PORT_PREEMPTION_INCLUDED_ */
