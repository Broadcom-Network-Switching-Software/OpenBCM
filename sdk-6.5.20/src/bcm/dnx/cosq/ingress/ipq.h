/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * \file src/bcm/dnx/cosq/ipq.h
 *
 *  Created on: May 30, 2018
 *      Author: si888124
 */

#ifndef _LOCAL_DNX_IPQ_H_INCLUDED_
#define _LOCAL_DNX_IPQ_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>

/**
 * \brief -
 * Map system TC (TC on the packet) and DP to VOQ offset, according to destination type.
 *
 * \param [in] unit - Unit-ID
 * \param [in] gport - Gport for the type of destination (system port or queue group)
 * \param [in] count - count of keys
 * \param [in] key_array - array of keys
 * \param [in] offset_array - array of offsets
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_voq_offset_mapping_set(
    int unit,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[]);

/**
 * \brief -
 * Get system TC (TC on the packet) and DP mapping to VOQ offset, according to destination type.
 *
 * \param [in] unit - Unit-ID
 * \param [in] gport - Gport for the type of destination (system port or queue group)
 * \param [in] count - count of keys
 * \param [in] key_array - array of keys
 * \param [in] offset_array - array of offsets
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_voq_offset_mapping_get(
    int unit,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[]);

/**
 * \brief -
 * Map system TC (TC on the packet) and DP to VOQ offset for FMQ
 */
shr_error_e dnx_cosq_ipq_fmq_voq_offset_mapping_set(
    int unit,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[]);

/**
 * \brief -
 * Get mapping of system TC and DP to VOQ offset for FMQ
 */
shr_error_e dnx_cosq_ipq_fmq_voq_offset_mapping_get(
    int unit,
    int count,
    bcm_cosq_queue_offset_mapping_key_t key_array[],
    bcm_cosq_queue_offset_t offset_array[]);

/**
 * \brief
 *   Initialize IPQ allocation manager.
 *
 * \param [in] unit -
 *   Relevant unit.
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_cosq_ipq_alloc_mngr_init(
    int unit);

/**
 * \brief -
 * Set VOQ to count all packets sent to invalid destination
 *
 * \param [in] unit - Unit-ID
 * \param [in] voq_gport - Gport for the base VOQ for the invalid destination queue
 * \param [in] cosq - VOQ offset to use as invalid destination
 * \param [in] enable - enable indication for setting the VOQ as invalid destination queue. if set to 0, the feature is disabled.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_invalid_destination_queue_set(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int enable);

/**
 * \brief -
 * Get indication whether this VOQ is used as invalid destination queue.
 *
 * \param [in] unit - Unit-ID
 * \param [in] voq_gport - Gport for the base VOQ for the invalid destination queue
 * \param [in] cosq - VOQ offset to use as invalid destination
 * \param [out] enable - set to true if the given VOQ is used as invalid destination queue
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_invalid_destination_queue_get(
    int unit,
    bcm_gport_t voq_gport,
    bcm_cos_queue_t cosq,
    int *enable);

/**
 * \brief -
 * Enable/Disable system port to voq mapping.
 *
 * \param [in] unit - Unit-ID
 * \param [in] sysport_gport -  system port
 * \param [in] enable - [Boolean] enable status of sysport to voq mapping
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_sysport_ingress_queue_map_enable_set(
    int unit,
    bcm_gport_t sysport_gport,
    int enable);

/**
 * \brief -
 * Get enable status of system port to voq mapping.
 *
 * \param [in] unit - Unit-ID
 * \param [in] sysport_gport -  system port
 * \param [in] enable - [Boolean] enable status of sysport to voq mapping
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_sysport_ingress_queue_map_enable_get(
    int unit,
    bcm_gport_t sysport_gport,
    int *enable);

/*
 * }
 */

#endif /** _LOCAL_DNX_IPQ_H_INCLUDED_ */
