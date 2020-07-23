/** \file src/bcm/dnx/cosq/ingress/iqs.h
 * 
 *  
 * Module IQS - Ingress Queue Scheduling 
 *  
 * Configuration of Ingress TM queue scheduling, includes: 
 *  * Credit request profiles
 *  * Credit worth
 *  * FSM reorder mechanism
 *  
 *  Not including:
 *  * FMQ Credit generation(implementation in fmq.c)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_SRC_IQS_INCLUDED__
#define _DNX_SRC_IQS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
/*
 * Typedefs:
 * {
 */
/*
 * } 
 */

/*
 * Credit Request Profile
 * { 
 */
/**
 * \brief - 
 * Set a set of thresholds of credit request profile. 
 * See Parameters for details about the threshold.
 * A credit request profile is disable by default and must be set before assigning a queue to the profile, 
 * A set of recommended thresholds can be retrieved using bcm_dnx_cosq_delay_tolerance_preset_get().
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] profile_id - credit request profile id
 * \param [in] thresholds - Set of thresholds - see description of structure for details about the thresholds. 
 *             credit_request_profile.flags - supported flags:
 *             BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY -set for high priority queues (otherwise low priority queues)
 *             BCM_COSQ_DELAY_TOLERANCE_BANDWIDTH_PROFILE_HIGH -  set for high bandwidth queues (used for congested scenarios).
 *             BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY - set for low latency queues.
 *             BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY - SRAM only queues.
 *             BCM_COSQ_DELAY_TOLERANCE_PUSH_QUEUES - Used for TDM queues (no credits are needed nor requested for the queue).
 *             Note that only one profile can be marked as push queue.
 *             credit_request_profile.credit_request_watchdog_status_msg_gen - not relevant - set to 0
 *             credit_request_profile.credit_request_watchdog_delete_queue_thresh - not relevant - set to 0
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_request_profile_set(
    int unit,
    int profile_id,
    bcm_cosq_delay_tolerance_t * thresholds);
/**
 * \brief - 
 * Get a set of thresholds of credit request profile. 
 * For details refer to dnx_iqs_credit_request_profile_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] profile_id - credit request profile id
 * \param [out] thresholds - Set of thresholds - see description of structure for details about the thresholds. 
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_request_profile_get(
    int unit,
    int profile_id,
    bcm_cosq_delay_tolerance_t * thresholds);

/*
 * }
 */

/*
 * Credit Watchdog
 * { 
 */

/**
 * \brief - 
 * Set retransmit time of watchdog mechanism.
 * Note that this function set sw data base only.
 * In order to configure the hw accordingly - call to dnx_iqs_credit_watchdog_hw_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] retransmit_msec - retransmit time in mili-sec
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_retransmit_sw_set(
    int unit,
    int retransmit_msec);

/**
 * \brief - 
 * Get retransmit time of watchdog mechanism.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [out] retransmit_msec - retransmit time in mili-sec
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_retransmit_sw_get(
    int unit,
    int *retransmit_msec);

/**
 * \brief - 
 * Set delete time of watchdog mechanism.
 * Note that this function set sw data base only.
 * In order to configure the hw accordingly - call to dnx_iqs_credit_watchdog_hw_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] profile_id -  credit request profile id 
 * \param [in] delete_msec - retransmit time in mili-sec
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_delete_sw_set(
    int unit,
    int profile_id,
    int delete_msec);

/**
 * \brief - 
 * Get delete time of watchdog mechanism.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] profile_id -  credit request profile id 
 * \param [out] delete_msec - delete time in mili-sec
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_delete_sw_get(
    int unit,
    int profile_id,
    int *delete_msec);

/*
 * }
 */
/*
 * Credit Worth
 * { 
 */
/**
 * \brief - 
 * There are two groups of credit worth.
 * Set credit worth  of each group.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] group_id -  group 0 or group 1 
 * \param [in] credit_worth_bytes -  credit worth in bytes 
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_worth_group_set(
    int unit,
    int group_id,
    uint32 credit_worth_bytes);
/**
 * \brief - 
 * Map FAP-ID to credit group
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] fap_id -  destination fap id
 * \param [in] group_id -  group 0 or group 1
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_worth_map_set(
    int unit,
    uint32 fap_id,
    uint32 group_id);
/**
 * \brief - 
 * Get mapping FAP-ID to credit group
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] fap_id -  destination fap id
 * \param [out] group_id -  group 0 or group 1
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_worth_map_get(
    int unit,
    uint32 fap_id,
    uint32 *group_id);

/*
 * }
 */
#endif/*_DNX_SRC_IQS_INCLUDED__*/
