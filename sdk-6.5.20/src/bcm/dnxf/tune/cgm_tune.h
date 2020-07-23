/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file source/bcm/dnx/tune/ingress_congestion_tune.h
 * Reserved.$ 
 */

#ifndef _DNXF_CGM_TUNE_H_INCLUDED_

#define _DNXF_CGM_TUNE_H_INCLUDED_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by Ramon only!"
#endif


/**
 * \brief - perform initial tuning of congestion module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_cgm_tune_init(
    int unit);


#endif /*_DNXF_CGM_TUNE_H_INCLUDED_*/


