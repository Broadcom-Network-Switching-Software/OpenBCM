/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/tune/tune.h
 * Reserved.$ 
 */

#ifndef _DNX_TUNE_H_INCLUDED_
/*
 * { 
 */
#define _DNX_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/cosq.h>

/**
 * \brief - perform  tune module init
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
shr_error_e dnx_tune_init(
    int unit);

/**
 * \brief - perform  tune for bcm_port_add API
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
shr_error_e dnx_tune_port_add(
    int unit);

/**
 * \brief - perform  tune for bcm_port_remove API
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
shr_error_e dnx_tune_port_remove(
    int unit);

/**
 * \brief - Tune global reject drop thresholds
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] rate_class_id - rate class id to set with default parameters
 *   \param [in] create_info - additional information required to set defaults to a rate class.
 *              - rate - port rate
 *              - attributes - rate class attributes, to refine the defaults. see BCM_COSQ_RATE_CLASS_CREATE_ATTR_*
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tune_ingress_congestion_default_voq_rate_class_set(
    int unit,
    int rate_class_id,
    bcm_cosq_rate_class_create_info_t * create_info);

/*
 * This section is here only for use by ctest/diag. It is not part of the standard
 * operational code.
 * See ctest_dnx_dram_operation_test_cmd()
 * {
 */
/**
 * \brief - Tune global reject drop thresholds
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] rate_class_id - rate class id to set with default parameters
 *   \param [in] create_info - additional information required to set defaults to a rate class.
 *              - rate - port rate
 *              - attributes - rate class attributes, to refine the defaults. see BCM_COSQ_RATE_CLASS_CREATE_ATTR_*
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e ctest_dnx_tune_ingress_congestion_default_voq_rate_class_set(
    int unit,
    int rate_class_id,
    bcm_cosq_rate_class_create_info_t * create_info);
/* } */

/** } */
#endif /*_DNX_TUNE_H_INCLUDED_*/
