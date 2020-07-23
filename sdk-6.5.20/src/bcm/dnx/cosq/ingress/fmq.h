/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * fmq.h
 *
 *  Created on: Nov 4, 2018
 *      Author: si888124
 */

#ifndef _LOCAL_DNX_FMQ_H_INCLUDED_
#define _LOCAL_DNX_FMQ_H_INCLUDED_

/**
 * \brief - Initialize FMQ related configurations (part of IQS module)
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 */
shr_error_e dnx_fmq_init(
    int unit);

/**
 * \brief - Set credit distribution mode between FMQ class. (WFQ / SP)
 * if mode is WFQ, also set the weight for the requested class.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - FMQ class gport (if SP mode)
 * \param [in] cosq - irrelevant. set to 0
 * \param [in] mode - WFQ(-1) / SP(0).
 * \param [in] weight - only relevant for WFQ. set a weight for FMQ best-effort class
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/**
 * \brief - Get credit distribution mode between FMQ class. (WFQ / SP)
 * if mode is WFQ, also get the weight for the requested class.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - FMQ class gport (if SP mode)
 * \param [in] cosq - irrelevant. set to 0
 * \param [out] mode - WFQ(-1) / SP(0).
 * \param [out] weight - only relevant for WFQ. weight for the requested FMQ best-effort class
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/**
 * \brief - Set bandwidth of global, BE and guaranteed credit generation.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - FMQ shaper gport (root/ BE/ guaranteed)
 * \param [in] cosq - irrelevant. set to 0
 * \param [in] kbits_sec_min - irrelevant. set to 0.
 * \param [in] kbits_sec_max - Required bandwidth in kbps
 * \param [in] flags - irrelevant. set to 0.
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * \brief - Get bandwidth of global, BE and guaranteed credit generation.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - FMQ shaper gport (root/ BE/ guaranteed)
 * \param [in] cosq - irrelevant. set to 0
 * \param [out] kbits_sec_min - irrelevant. set to 0.
 * \param [out] kbits_sec_max - Required bandwidth in kbps
 * \param [out] flags - irrelevant. set to 0.
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Control Set function for FMQ types.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - FMQ shaper gport (root/ BE/ guaranteed)
 * \param [in] cosq - irrelevant. set to 0
 * \param [in] type - control type. currently supported bcmCosqControlBandwidthBurstMax
 * \param [in] arg - value to set
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_control_fmq_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - Control Get function for FMQ types.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port - FMQ shaper gport (root/ BE/ guaranteed)
 * \param [in] cosq - irrelevant. set to 0
 * \param [in] type - control type. currently supported bcmCosqControlBandwidthBurstMax
 * \param [out] arg - value to get
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_control_fmq_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

#endif /* _LOCAL_DNX_FMQ_H_INCLUDED_ */
