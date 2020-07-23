/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/tune/ecgm_tune.h
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

/*
 * Include files.
 * {
 */

/**
* \brief -   set port and queue ecgm thresholds for single port.
*
* \param [in] unit -  Unit-ID.
* \param [in] local_port -  logical port.
* \param [in] port_rate - configured port rate (speed).
*
* \return
*   shr_error_e
*
* \remark
*/
shr_error_e dnx_ecgm_tune_single_port(
    int unit,
    bcm_port_t local_port,
    int port_rate);

/** } */
#endif /*_DNX_ECGM_TUNE_H_INCLUDED_*/
