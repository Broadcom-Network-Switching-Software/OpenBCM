/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/tune/ecgm_tune.h
 * Reserved.$ 
 */

#ifndef _DNX_ECGM_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_ECGM_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
* \brief -   configures a port's interface with ecgm resources thresholds.
*            values are extracted from default thresholds per
*            port rate.
*            each port is set with all the fields of
*            port profile and interface profile.
*
* \param [in] unit -  Unit-ID.
* \param [in] port -  logical port.
* \param [in] port_rate - configured port rate (speed).
*
* \return
*   shr_error_e
*
* \remark
*   if the exact set of thresholds appears in an existing profile
*   the existing profile would be used.
*   otherwise - if allocation is possible (up to 8 interface
*   profiles and 16 port profiles per core), a new profile
*   will be created for the new thresholds set.
*/
shr_error_e dnx_ecgm_tune_port_init(
    int unit,
    bcm_port_t port,
    int port_rate);

/**
* \brief - configures all cores with ecgm default thresholds.
*
* \param [in] unit -  Unit-ID.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_tune_init(
    int unit);

/** } */
#endif /*_DNX_ECGM_TUNE_H_INCLUDED_*/
