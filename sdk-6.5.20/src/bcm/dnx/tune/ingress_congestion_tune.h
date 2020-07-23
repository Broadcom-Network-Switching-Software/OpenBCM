/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file source/bcm/dnx/tune/ingress_congestion_tune.h
 * Reserved.$ 
 */

#ifndef _DNX_INGRESS_CONGESTION_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_INGRESS_CONGESTION_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif


/**
 * \brief - perform initial tuning of ingress congestion module
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
shr_error_e dnx_tune_ingress_congestion_set(
    int unit);


/** } */
#endif /*_DNX_INGRESS_CONGESTION_TUNE_H_INCLUDED_*/



