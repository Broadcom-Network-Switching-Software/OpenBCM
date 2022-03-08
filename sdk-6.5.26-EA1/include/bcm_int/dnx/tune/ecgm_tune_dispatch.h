/*
 * ecgm_tune_dispatch.h
 *
 *  Created on: Dec 13, 2021
 *      Author: nt893888
 */

#ifndef INCLUDE_BCM_INT_DNX_TUNE_ECGM_TUNE_DISPATCH_H_
#define INCLUDE_BCM_INT_DNX_TUNE_ECGM_TUNE_DISPATCH_H_

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>

/**
* \brief - configures a port's interface with ecgm resources thresholds.
*          values are extracted from default thresholds per port rate.
*
* \param [in] unit -Unit-ID.
* \param [in] port -logical port.
* \param [in] port_rate - configured port rate (speed).
*
* \return
*   shr_error_e
*
* \remark
*/
shr_error_e dnx_ecgm_interface_tune(
    int unit,
    bcm_port_t port,
    int port_rate);

/**
* \brief - configure ECGM thresholds for single port
*
* \param [in] unit - Unit-ID.
* \param [in] port - logical port.
* \param [in] port_rate - configured port rate (speed).
*
* \return
*   shr_error_e
*
* \remark
*/
shr_error_e dnx_ecgm_port_tune(
    int unit,
    bcm_port_t port,
    int port_rate);

#endif /* INCLUDE_BCM_INT_DNX_TUNE_ECGM_TUNE_DISPATCH_H_ */
