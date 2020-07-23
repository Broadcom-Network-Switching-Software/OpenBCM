/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * system_red.h
 *
 *  Created on: Sep 12, 2018
 *      Author: si888124
 */
#ifndef _SRC_SYSTEM_RED_H_INCLUDED_

#define _SRC_SYSTEM_RED_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Init system RED feature.
 * configure HW according to system RED soc property.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_system_red_init(
    int unit);

/**
 * \brief - Set max VOQ size to be mapped to system RED index (RED-Q-Size)
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class gport
 * \param [in] red_q_size - red-q-size index to map the VOQ range to.
 * \param [in] gport_size - gport size info. only used size_max: max VOQ size to be mapped to the specified red-q-size
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_voq_max_size_set(
    int unit,
    bcm_gport_t gport,
    int red_q_size,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - Get max VOQ size mapped to system RED index (RED-Q-Size)
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class gport
 * \param [in] red_q_size - red-q-size index mapped to VOQ range
 * \param [out] gport_size - gport size info. only used size_max: max VOQ size mapped to the specified red-q-size

 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_voq_max_size_get(
    int unit,
    bcm_gport_t gport,
    int red_q_size,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - Set discard params to be used in system RED admission test
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class gport.
 * \param [in] cosq - irrelevant. must be set to 0.
 * \param [in] discard - discard params. relevant fields:
 *          - flags - discard flags: BCM_COSQ_DISCARD_COLOR_XXX, BCM_COSQ_DISCARD_ENABLE, BCM_COSQ_DISCARD_PROBABILITY1/2
 *          - min_thresh - min threshold in RED-Q-Size (0-15)
 *          - max_thresh - min threshold in RED-Q-Size (0-15)
 *          - drop_probability - drop probability (0-0xFFFF)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard);

/**
 * \brief - Get discard params used in system RED admission test
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class gport.
 * \param [in] cosq - irrelevant. must be set to 0.
 * \param [in,out] discard - discard params. relevant fields:
 *          - flags (input) - discard flags: BCM_COSQ_DISCARD_COLOR_XXX, BCM_COSQ_DISCARD_PROBABILITY1/2
 *          - flags (output) - BCM_COSQ_DISCARD_ENABLE
 *          - min_thresh (output) - min threshold in RED-Q-Size (0-15)
 *          - max_thresh (output) - min threshold in RED-Q-Size (0-15)
 *          - drop_probability (output) - drop probability (0-0xFFFF)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard);

#endif
