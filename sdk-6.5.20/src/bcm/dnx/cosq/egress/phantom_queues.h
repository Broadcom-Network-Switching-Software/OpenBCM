/*
 * phantom_queues.h
 *
 *  Created on: Dec 16, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _EGRESS_PAHNTOM_QUEUES_H_INCLUDED_
#define _EGRESS_PAHNTOM_QUEUES_H_INCLUDED_

#include <bcm/types.h>

/**
 * \brief - Initialize Phantom Queues related configuration
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_phantom_queues_init(
    int unit);

/**
 * \brief - Set phantom Queue threshold for the specified port.
 * 1. The threshold is per EGQ if (derived from the port).
 * 2. The threshold is allocated using template manager mechanism.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  logical port to represent EGQ if
 * \param [in] threshold - threshold value to set
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_phantom_queues_threshold_set(
    int unit,
    bcm_port_t port,
    int threshold);

/**
 * \brief - Get phantom Queue threshold for the specified port.
 * 1. The threshold is per EGQ if (derived from the port).
 * 2. The threshold is allocated using template manager mechanism.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  logical port to represent EGQ if
 * \param [out] threshold - threshold value
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_phantom_queues_threshold_get(
    int unit,
    bcm_port_t port,
    int *threshold);

#endif /* _EGRESS_PAHNTOM_QUEUES_H_INCLUDED_ */
