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
