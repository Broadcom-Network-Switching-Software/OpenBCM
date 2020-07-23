/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file source/bcm/dnx/tune/fabric_tune.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_FABRIC_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - perform initial tuning of fabric module
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
shr_error_e dnx_tune_credit_generator_init(
    int unit);

/**
 * \brief - perform initial tuning of DRM tables
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
shr_error_e dnx_tune_drm_tables_set(
    int unit);

/** } */
#endif /*_DNX_FABRIC_TUNE_H_INCLUDED_*/
