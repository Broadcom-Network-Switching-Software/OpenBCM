/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * system_red.h
 *
 *  Created on: Sep 13, 2018
 *      Author: si888124
 */

#ifndef _DNX_SYSTEM_RED_H_INCLUDED_
/*
 * {
 */
#define _DNX_SYSTEM_RED_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/cosq/cosq.h>

typedef enum
{
    DNX_SYSTEM_RED_RESOURCE_INVALID = -1,
    DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS = 0,
    DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS,
    DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS,
    DNX_SYSTEM_RED_RESOURCE_NOF
} dnx_system_red_resource_type_e;

/**
 * \brief - Set system RED aging params in both Ingress and Sch
 *
 * \param [in] unit - The unit number.
 * \param [in] discard - aging params:
 *          - flags -   BCM_FABRIC_CONFIG_DISCARD_XXX flags
 *          - enable -  Enable Aging period functionality in SCH
 *          - age - aging period in milliseconds (for both sch and ingress).
 *          - age_mode - action to perform when aging period expires (in Sch). options: reset / decrement.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_aging_config_set(
    int unit,
    bcm_fabric_config_discard_t * discard);

/**
 * \brief - Get system RED aging params
 *
 * \param [in] unit - The unit number.
 * \param [in] discard - aging params: see dnx_system_red_aging_config_set
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_aging_config_get(
    int unit,
    bcm_fabric_config_discard_t * discard);

/**
 * \brief - Get system RED status per system port
 *
 * \param [in] unit - The unit number.
 * \param [in] system_port - system port
 * \param [out] red_index - max_red_index for the system port
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_sysport_red_status_get(
    int unit,
    uint32 system_port,
    uint32 *red_index);

/**
 * \brief - Get system RED global status (max and current)
 *
 * \param [in] unit - The unit number.
 * \param [out] current_red_index - current RED index on the device
 * \param [out] max_red_index - max_red_index seen since last read
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_global_red_status_get(
    int unit,
    uint32 *current_red_index,
    uint32 *max_red_index);

/**
 * \brief - Set free resource thresholds and map resource range to system RED index (RED-Q-Size)
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - irrelevant. must be set to 0.
 * \param [in] cosq - irrelevant. must be set to 0.
 * \param [in,out] threshold - threshold info. relevant fields:
 *          - flags - range flags BCM_COSQ_THRESHOLD_RANGE_0/1/2/3
 *          - type  - resource_type
 *          - dp    - red-q-size to map to thr relevant range
 *          - value - range threshold value (will be ignored for RANGE_3, since its max value is max_res)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_free_res_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/**
 * \brief - Get free resource thresholds and mapping of resource range to system RED index (RED-Q-Size)
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - irrelevant. must be set to 0.
 * \param [in] cosq - irrelevant. must be set to 0.
 * \param [in,out] threshold - threshold info. relevant fields:
 *          - flags (input) - range flags BCM_COSQ_THRESHOLD_RANGE_0/1/2/3
 *          - type  (input) - resource_type
 *          - dp    (output) - red-q-size mapped to thr relevant range
 *          - value (output) - range threshold value
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_free_res_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

#endif /* _DNX_SYSTEM_RED_H_INCLUDED_ */
