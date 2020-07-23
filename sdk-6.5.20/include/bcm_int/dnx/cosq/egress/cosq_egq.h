/**
 * \file bcm_int/dnx/cosq/egress/cosq_egq.h
 * 
 *
 * Mainly prototypes of cosq API functions, which are purely
 * EGQ related, for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef COSQ_EGQ_H_INCLUDED
#define COSQ_EGQ_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Defines:
 * {
 */
/*
 * } 
 */
/*
 * Typedefs:
 * {
 */
 /*
  * } 
  */
/*
 * Functions
 * {
 */
/**
 * \brief
 *    Set a single mapping element which indicates, for specified port,  to which TCG,
 *    specified priority is mapped.
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] gport -
 *    uint32. Gport index to configure. (Converted to logical port to carry out
 *    virtually all required operations.)
 * \param [in] cosq -
 *    bcm_cos_queue_t. Priority (one of 8) to use in mapping TCG. Essentially,
 *    indicated TCG is mapped to 'priority' which is one of the entries
 *    assigned to 'logical_port'. Basic range is 0 to 7 but it must
 *    also be smaller than the number of priorities assigned to this port.
 *    See dnx_algo_port_priorities_nof_get().
 * \param [in] mode
 *    int. Identifier of the TCG to map to indicated priority and logical_port.
 *    If it is in the range BCM_COSQ_SP0 to BCM_COSQ_SP7 then TCG index is
 *    (mode - BCM_COSQ_SP0).
 *    If it is in the range DNX_TCG_MIN to DNX_TCG_MAX then TCG index is
 *    'mode' itself.
 * \param [in] weight
 *    int. Ignored. Not used.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Function description, in other words: Attach Port-Priority of a
 *     given port priority TC-X to the TCG-Y.
 * \see
 *   * bcm_dnx_cosq_gport_sched_set
 *   * bcm_dnx_cosq_gport_sched_get
 *   * dnx_egr_queuing_tc_to_tcg_mapping_set
 */
shr_error_e dnx_cosq_egq_gport_egress_tc_to_tcg_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);
/**
 * \brief
 *    Set a single mapping element which indicates, for specified port,  to which TCG,
 *    specified priority is mapped.
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] gport -
 *    uint32. Gport index to configure. (Converted to logical port to carry out
 *    virtually all required operations.)
 * \param [in] cosq -
 *    bcm_cos_queue_t. Priority (one of 8) to use in mapping TCG. Essentially,
 *    indicated TCG is mapped to 'priority' which is one of the entries
 *    assigned to 'logical_port'. Basic range is 0 to 7 but it must
 *    also be smaller than the number of priorities assigned to this port.
 *    See dnx_algo_port_priorities_nof_get().
 * \param [out] mode_p
 *    Pointer to int. This procedure loads pointed memory by identifier of
 *    the TCG which is mapped to indicated priority and logical_port.
 *    It is in the range BCM_COSQ_SP0 to BCM_COSQ_SP7.
 * \param [out] weight_p
 *    Pointer to int. Ignored. Not used.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Function description, in other words: Get TCG-Y Port-Priority of a
 *     given port priority TC-X.
 * \see
 *   * bcm_dnx_cosq_gport_sched_set
 *   * bcm_dnx_cosq_gport_sched_get
 *   * dnx_egr_queuing_tc_to_tcg_mapping_set
 */
shr_error_e dnx_cosq_egq_gport_egress_tc_to_tcg_map_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode_p,
    int *weight_p);
/**
 * Purpose: 
 * Set EIR Weight for Egress Port TCG
 */
/**
 * \brief -
 *   Configure WFQ parameters (weight) for specified TCG gport
 *   (Sets EIR Weight for Egress Port TCG).
 *
 *   Sets, for a specified TCG within a certain OFP, its excess rate.
 *   Excess traffic is scheduled between other TCGs according to
 *   weighted fair queueing or strict priority policy.
 *   Sets to 'invalid', in case TCG does not take part in EIR.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Scheduler port in the format of gport. This parameter determines the
 *   base qpair for which weight is to be set.
 *   See 'mode' below.
 * \param [in] cosq -
 *   The priority (corresponding to the base qpair related to gport) for which
 *   WFQ (EIR) parameters (weights) are to be set. Note that these parameters
 *   may also be configured to indicate SP (strict priority). 
 * \param [out] mode -
 *   int. Flag.
 *   Can be:
 *     -1
 *     BCM_COSQ_NONE
 *   If set to '-1' then input 'weight' is used. This procedure will select an
 *     element on 'tcg' as per 'cosq' and will load it by input 'weight'.
 *     Note that a 'weight' of '0' indicates 'Strict Priority'.
 *   If set to 'BCM_COSQ_NONE' then input 'weight' is ignored. This procedure
 *     will select an element on 'tcg' as per 'cosq' and will disconnect it from
 *     EIR altogether
 * \param [out] weight -
 *   This is the weight to assign as per 'mode'. (Only used when mode is -1).
 *   Allowed range is 0 - DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX.
 *   A value of '0' indicates 'Strict Priority'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_tcg_weight_get
 *   * bcm_dnx_cosq_gport_sched_set
 */
shr_error_e dnx_cosq_egq_gport_egress_port_tcg_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);
/**
 * Purpose: 
 * Get EIR Weight for Egress Port TCG.
 */
/**
 * \brief -
 *   Get WFQ parameters (weight) for specified TCG gport
 *   (Gets EIR Weight for Egress Port TCG).
 *
 *   Gets, for a specified TCG within a certain OFP its excess rate.
 *   Excess traffic is scheduled between other TCGs according to
 *   weighted fair queueing or strict priority policy.
 *   Gets 'invalid' indication, in case TCG does not take part in EIR.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Scheduler port in the format of gport. This parameter determines the
 *   base qpair for which weight is to be retrieved.
 *   See 'mode' below.
 * \param [in] cosq -
 *   The priority (corresponding to the base qpair related to gport) for which
 *   WFQ (EIR) parameters (weights) are to be retrieved. Note that these parameters
 *   may also be configured to indicate SP (strict priority). 
 * \param [out] mode -
 *   Pointer to int. Flag.
 *   Can be:
 *     -1
 *     BCM_COSQ_NONE
 *   If set to '-1' then '*weight' is meaningful. This procedure selected an
 *     element on 'tcg' as per 'cosq' and has loaded it into '*weight'.
 *     (If 'weight' is '0' then it is configured to 'Strict Priority' over
 *     the other TCG elements).
 *   If set to 'BCM_COSQ_NONE' then '*weight' should be ignored. This procedure
 *     selected an element on 'tcg' as per 'cosq' and has found out it is
 *     disconnected from EIR altogether
 * \param [out] weight -
 *   See 'mode' above. 
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_tcg_weight_get
 *   * bcm_dnx_cosq_gport_sched_get
 */
shr_error_e dnx_cosq_egq_gport_egress_port_tcg_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);
/**
 * \brief -
 *   Configure WFQ parameters (weights) for specified gport.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Scheduler port in the format of gport. This parameter determines the
 *   base qpair and the traffic type (UC/MC) for which weights are to be set.
 *   See 'mode' below.
 * \param [in] cosq -
 *   The priority (corresponding to the qpair related to gport) for which
 *   WFQ parameters (weights) are to be set. Note that these parameters may
 *   also indicate SP (strict priority). 
 * \param [in] mode -
 *   int. Flag.
 *   Can be:
 *     -1
 *     DNX_COSQ_EGR_OFP_SCH_SP_HIGH
 *     DNX_COSQ_EGR_OFP_SCH_SP_LOW
 *   If set to '-1' then input 'weight' is used. This procedure will select an
 *     element on 'sch_wfq_p' as per 'is_ucast_egress_queue' and will load it
 *     by input 'weight'. The other element is not touched.
 *   Else
 *     If unicast traffic (non-zero is_ucast_egress_queue)
 *       If 'mode' is SP_HIGH then,
 *         Set unicast weight to '0' and multicast to '1'. This
 *         setup gives strictly high priority to unicast.
 *       If 'mode' is SP_LOW then reverse unicast/multicast roles.
 *     Else (multicast traffic)
 *       If 'mode' is SP_HIGH then,
 *         Set unicast weight to '1' and multicast to '0'. This
 *         setup gives strictly high priority to multicast.
 *       If 'mode' is SP_LOW then reverse unicast/multicast roles.
 * \param [in] weight -
 *   This is the weight to assign as per 'mode'. (Only used when mode is -1)
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   Set scheduling mode in OTM port scheduler. 
 *   UC and MC Egress queues are handled here.
 *   Configure the SP to connect to and the weighted distribution 
 *   between the other queue on the same priority. 
 * \see
 *   * dnx_ofp_rates_wfq_set
 */
shr_error_e dnx_cosq_egq_queue_wfq_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);
/**
 * \brief -
 *   Get 'mode' and 'weight' parameters from WFQ parameters (weights) for
 *   specified gport.
 *   See 'remark' below.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Scheduler port in the format of gport. This parameter determines the
 *   base qpair and the traffic type (UC/MC) for which mode/weight are to be
 *   derived. See 'mode' below.
 * \param [in] cosq -
 *   The priority (corresponding to the qpair related to gport) for which
 *   WFQ parameters (weights) are effective. Note that these parameters may
 *   also indicate SP (strict priority). 
 * \param [out] mode -
 *   Pointer to int. This procedure loads pointed memory by a flag as described below.
 *   Can be:
 *     -1
 *     DNX_COSQ_EGR_OFP_SCH_SP_HIGH
 *     DNX_COSQ_EGR_OFP_SCH_SP_LOW
 * \param [out] weight -
 *   Pointer to int. This procedure loads pointed memory by a flag as described below.
 *   Only meaningful if '*mode' is '-1'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   Do the reverse operation of dnx_cosq_egq_queue_wfq_set():
 *   Given the weights, get 'mode' and ;weight' 
 *
 *   The algorithm is:
 *     If 'gport' indicates 'unicast' flag
 *       If 'unicast weight' is '0' (SP case)
 *         Set *mode to SP_HIGH (and *weight to -1)
 *       Else if 'multicast weight' is '0'  (SP case)
 *         Set *mode to SP_LOW (and *weight to -1)
 *       Else do take weight into consideration:
 *         Set '*mode' to '-1' (meaning 'weight is meaningful')
 *         Set *weight with input 'unicast weight'
 *     Else (gport indicates 'multicast' flag)
 *       If 'multicast weight' is '0' (SP case)
 *         Set *mode to SP_HIGH (and *weight to -1)
 *       Else if 'unicast weight' is '0'  (SP case)
 *         Set *mode to SP_LOW (and *weight to -1)
 *       Else do take weight into consideration:
 *         Set '*mode' to '-1' (meaning 'weight is meaningful')
 *         Set *weight with input 'multicast weight'
 * \see
 *   * dnx_cosq_egress_fill_mode_weight_from_ofp_wfq_sch
 */
shr_error_e dnx_cosq_egq_queue_wfq_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);
/**
 * \brief -
 *   Configure Port-TC (q-pair) shaping rate, set single port rate.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   TC port in the format of gport.
 * \param [in] cosq -
 *   Indicates the 'priority' of the selected port (q-pair):
 *     0 indicates DNX_EGR_Q_PRIO_0
 *     1 indicates DNX_EGR_Q_PRIO_1
 *     ...
 *     7 indicates DNX_EGR_Q_PRIO_7
 *   Other values are illegal
 * \param [in] kbits_sec_min -
 *   Not used. Must be set to '0'.
 * \param [in] kbits_sec_max -
 *   Maximal bandwidth in kbits/sec 
 * \param [in] flags -
 *   Not used. Must be set to '0'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated sw state
 *   * * Updated hw
 * \see
 *   * dnx_ofp_rates_egq_port_priority_rate_sw_set
 *   * dnx_ofp_rates_egq_port_priority_rate_hw_set
 */
shr_error_e dnx_cosq_egq_gport_egress_port_tc_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);
/**
 * Purpose:
 * Get Port-TC (q-pair) shaping rate
 */
/**
 * \brief -
 *   Get Port-TC (q-pair) shaping rate, get single port rate.
 *   Data is extracted directly from HW.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   TC port in the format of gport.
 * \param [in] cosq -
 *   Indicates the 'priority' of the selected port (q-pair):
 *     0 indicates DNX_EGR_Q_PRIO_0
 *     1 indicates DNX_EGR_Q_PRIO_1
 *     ...
 *     7 indicates DNX_EGR_Q_PRIO_7
 *   Other values are illegal
 * \param [out] kbits_sec_min -
 *   Pointer to uint32. Not used. Set to '0' by this procedure.
 * \param [out] kbits_sec_max -
 *   Pointer to uint32. This procedure loads pointed memory by
 *   maximal bandwidth in kbits/sec 
 * \param [out] flags -
 *   Pointer to uint32. Not used. Set to '0' by this procedure.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * sw state
 * \see
 *   * dnx_ofp_rates_egq_port_priority_rate_hw_get
 */
shr_error_e dnx_cosq_egq_gport_egress_port_tc_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);
/**
 * \brief -
 *   Configure TCG shaping rate
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   TCG port in the format of gport.
 * \param [in] cosq -
 *   Indicates the offset of TCG from port scheduler corresponding to
 *   base q-pair of 'gport':
 *   Must be between 0 and the number of priorities assigned to this port.
 *   (The full TCG index range is between 0 (DNX_TCG_MIN) and 7 (DNX_TCG_MAX)).
 * \param [in] kbits_sec_min -
 *   Not used. Must be set to '0'.
 * \param [in] kbits_sec_max -
 *   Maximal bandwidth in kbits/sec. Must not be larger than DNX_IF_MAX_RATE_KBPS(unit)
 *   See dnx_data_egr_queuing.params.max_gbps_rate_egq
 * \param [in] flags -
 *   Not used. Must be set to '0'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect output:
 *   * * Updated sw state
 *   * * Updated hw
 *   * TCG is calculated by adding 'cosq' to the base qpair which matches input
 *     'gport' (modulo DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT (= 8)). The net result
 *     must be between DNX_TCG_MIN and DNX_TCG_MAX.
 * \see
 *   * dnx_ofp_rates_egq_tcg_rate_sw_set
 *   * dnx_ofp_rates_egq_tcg_rate_hw_set
 *   * dnx_algo_port_priorities_nof_get
 */
shr_error_e dnx_cosq_egq_gport_egress_port_tcg_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);
/**
 * \brief -
 *   Given TCG-port and tcg index on it, get corresponding shaping rate.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Gport to retrieve corresponding qpair for.
 * \param [in] cosq -
 *   TCG index (priority) on specified port. Must be smaller than the number
 *   of priorities assigned to this port.
 * \param [out] kbits_sec_min -
 *   Pointer to uint32. Not used. Called procedure loads it by '0'.
 * \param [in] kbits_sec_max -
 *   Pointer to uint32. Called procedure loads it by assigned bandwidth
 *   in kbits/sec 
 * \param [in] flags -
 *   Pointer to uint32. Not used. Called procedure loads it by '0'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that number of assigned prioriries for TCG port may be either 8 or 4
 * \see
 *   * bcm_dnx_cosq_gport_bandwidth_get
 */
shr_error_e dnx_cosq_egq_gport_egress_port_tcg_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);
/**
 * \brief -
 *   Configure OFP shaping rate, set configuration per interface.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Interface port in the format of gport.
 * \param [in] cosq -
 *   Not used. Must be set to '0'.
 * \param [in] kbits_sec_min -
 *   Not used. Must be set to '0'.
 * \param [in] kbits_sec_max -
 *   Maximal bandwidth in kbits/sec 
 * \param [in] flags -
 *   Not used. Must be set to '0'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Input port must NOT be channelized. For a channelized port, see
 *     dnx_egq_ofp_rates_port_bandwidth_set().
 *   * Indirect output:
 *   * * Updated sw state: dnx_ofp_rate_db.otm
 *   * * Updated hw
 * \see
 *   * dnx_ofp_rates_egq_otm_shapers_set
 */
shr_error_e dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);
/**
 * \brief -
 *   Get configuration of OFP shaping rate, get configuration per interface.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] gport -
 *   Interface port in the format of gport.
 * \param [in] cosq -
 *   Not used. Must be set to '0'.
 * \param [out] kbits_sec_min -
 *   Pointer to uint32. Not used. Set to '0' by this procedure.
 * \param [out] kbits_sec_max -
 *   Pointer to uint32. This procedure loads pointed memory by
 *   maximal bandwidth in kbits/sec 
 * \param [out] flags -
 *   Pointer to uint32. Not used. Set to '0' by this procedure.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Input port must NOT be channelized. For a channelized port, see
 *     dnx_egq_ofp_rates_port_bandwidth_get().
 *   * Indirect output:
 *   * * Sw state: dnx_ofp_rate_db.otm
 * \see
 *   * dnx_ofp_rates_egq_interface_shaper_get
 */
shr_error_e dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);
/**
 * \brief
 *   Set various features, depnding on type, for egress 'OTM port' scheduler.
 *   Given 'gport' and priority ('cosq') to identify a specific qpair,
 *   use input 'type' to identify the required operation and use 'arg' for
 *   that operation.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'.
 * \param [in] cosq -
 *   bcm_cos_queue_t. NOT USED by this procedure. Must be set to '0'.
 * \param [in] type -
 *   bcm_cosq_control_t. Can be:
 *     bcmCosqControlPacketLengthAdjust:
 *       Set 'compensation' for OTM port (on 'channelized arbiter' calendar,
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB). 'Arg' is the value of 'header compensation'
 *       to apply, in bytes. Value must be smaller than 64.
 *       Header Compensation is used to adjust for differences in the packet header size
 *       to account for network headers added by the network interface.
 *     bcmCosqControlBandwidthBurstMax:
 *       Set 'max burst' for all slots corresponding to OTM port (on
 *       'DNX_OFP_RATES_EGQ_CAL_CHAN_ARB' calendar).
 *       'Arg' is the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *     bcmCosqControlBandwidthBurstMaxEmptyQueue:
 *       Set 'max burst' for all OTM port shapers (on 'DNX_OFP_RATES_EGQ_CAL_CHAN_ARB'
 *       calendar) which are in 'empty' state.
 *       'Arg' is the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX.
 *     bcmCosqControlEgressPriorityOverCast:
 *       Select mode of operation for the two queues (unicast and multicast) corresponding to
 *       specified OTM port: If '0' then WFQ is done before SP. Else SP is done before WFQ.
 *       'Arg' is 'enable' value (0 or 1).
 * \param [in] arg -
 *   int. General parameter. See 'type' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_port_is_high_set
 */
shr_error_e dnx_cosq_egq_control_egress_port_scheduler_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);
/**
 * \brief
 *   Set various features, depnding on type, for egress 'TC port' scheduler.
 *   Given 'gport' and priority ('cosq') to identify a specific qpair,
 *   use input 'type' to identify the required operation and use 'arg' for that operation.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact target qpair can be identified ('target qpair' = 'base qpair' plus
 *   'priority')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Priority of target qpair. Can either be in the range from 0 to
 *   7 or BCM_COSQ_HIGH_PRIORITY or BCM_COSQ_LOW_PRIORITY. See dnx_cosq_egq_cosq_priority_convert().
 * \param [in] type -
 *   bcm_cosq_control_t. Can be:
 *     bcmCosqControlBandwidthBurstMax:
 *       Set 'max burst' for TC port (on 'priority' calendar). 'Arg' is the value of
 *       the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *     bcmCosqControlBandwidthBurstMaxEmptyQueue:
 *       Set 'max burst' for all TC port (on 'priority' calendar) which are in 'empty' state.
 *       'Arg' is the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *       The parameter 'cosq' is not used and 'gport' is only used to verify it is a TC port.
 *     bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
 *       Set 'max burst' for all TC port (on 'priority' calendar) which are in 'Flow Control'
 *       state. 'Arg' is the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *       The parameter 'cosq' is not used and 'gport' is only used to verify it is a TC port.
 *     bcmCosqControlPrioritySelect
 *       Set priority (High/low), to indicated qpair (port), to the inreface on which it is
 *       channelized. If indicated port is not channelized, this operation is ignored (by HW).
 *       'Arg' indicates the priority to assign to interface corresponding to indicated qpair.
 *       Can only be high (BCM_COSQ_SP0) or low (BCM_COSQ_SP1). See
 *       dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [in] arg -
 *   int. General parameter. See 'type' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_DWM, fields UC_OR_UC_LOW_QUEUE_WEIGHT, MC_OR_MC_LOW_QUEUE_WEIGHT
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, fields MC_OR_MC_LOW_QUEUE_WEIGHT,
 *     UC_OR_UC_LOW_QUEUE_WEIGHT using 'entry_offset' and 'core' as key.
 * \see
 *   * dnx_egr_queuing_port_is_high_set
 */
shr_error_e dnx_cosq_egq_control_egress_port_tc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);
/**
 * \brief
 *   Set various features, depnding on type, for egress 'TCG' scheduler.
 *   Given 'gport' and offset ('cosq') to identify a TCG index,
 *   use input 'type' to identify the required operation and use 'arg' for that operation.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact TCG can be identified ('TCG' = 'base qpair % TCG_NOF_PRIORITIES' plus 'cosq')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Offset of target TCG. Must be within the range from DNX_TCG_MIN to
 *   DNX_NOF_TCGS. See dnx_cosq_bcm_cosq_to_tcg().
 * \param [in] type -
 *   bcm_cosq_control_t. Can be:
 *     bcmCosqControlBandwidthBurstMax:
 *       Set 'max burst' for TCG (on 'TCG' calendar, DNX_OFP_RATES_EGQ_CAL_TCG). 'Arg'
 *       is the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \param [in] arg -
 *   int. General parameter. See 'type' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_TCG_CREDIT_TABLE, field TCG_MAX_BURST
 *   * Accessing DBAL table DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE, field DBAL_FIELD_TCG_MAX_BURST.
 * \see
 *   * dnx_ofp_rates_egq_tcg_max_burst_set
 */
shr_error_e dnx_cosq_egq_control_egress_port_tcg_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);
/**
 * \brief
 *   Get various parameters, depnding on type, for egress 'TC port' scheduler.
 *   Given 'gport' and priority ('cosq') to identify a specific qpair,
 *   use input 'type' to identify the required operation and use 'arg' for that operation.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact target qpair can be identified ('target qpair' = 'base qpair' plus
 *   'priority')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Priority of target qpair. Can either be in the range from 0 to
 *   7 or BCM_COSQ_HIGH_PRIORITY or BCM_COSQ_LOW_PRIORITY. See dnx_cosq_egq_cosq_priority_convert().
 * \param [in] type -
 *   bcm_cosq_control_t. Can be:
 *     bcmCosqControlBandwidthBurstMax:
 *       Get 'max burst' for TC port (on 'priority' calendar). '*Arg' is used to store the
 *       value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *     bcmCosqControlBandwidthBurstMaxEmptyQueue:
 *       Get 'max burst' for all TC port (on 'priority' calendar) which are in 'empty' state.
 *       '*Arg' is used to store the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *       The parameter 'cosq' is not used and 'gport' is only used to verify it is a TC port.
 *     bcmCosqControlBandwidthBurstMaxFlowControlledQueue:
 *       Get 'max burst' for all TC port (on 'priority' calendar) which are in 'Flow Control'
 *       state. '*Arg' is used to store the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *       The parameter 'cosq' is not used and 'gport' is only used to verify it is a TC port.
 *     bcmCosqControlPrioritySelect
 *       Get priority (High/low), related to indicated qpair (port), to the inreface on which
 *       it is channelized. (If indicated port is not channelized, this operation is meaningless).
 *       '*Arg' is used to indicate the priority assigned to the interface corresponding to
  *      indicated qpair.
 *       Can only be high (BCM_COSQ_SP0) or low (BCM_COSQ_SP1). See
 *       dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [in] arg -
 *   int. General parameter. See 'type' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_DWM, fields UC_OR_UC_LOW_QUEUE_WEIGHT, MC_OR_MC_LOW_QUEUE_WEIGHT
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, fields MC_OR_MC_LOW_QUEUE_WEIGHT,
 *     UC_OR_UC_LOW_QUEUE_WEIGHT using 'entry_offset' and 'core' as key.
 * \see
 *   * dnx_egr_queuing_port_is_high_set
 */
shr_error_e dnx_cosq_egq_control_egress_port_tc_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);
/**
 * \brief
 *   Get various parameters, depnding on type, for egress 'TCG' scheduler.
 *   Given 'gport' and offset ('cosq') to identify a TCG index,
 *   use input 'type' to identify the required operation and use 'arg' for output.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact TCG can be identified ('TCG' = 'base qpair % TCG_NOF_PRIORITIES' plus 'cosq')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Offset of target TCG. Must be within the range from DNX_TCG_MIN to
 *   DNX_NOF_TCGS. See dnx_cosq_bcm_cosq_to_tcg().
 * \param [in] type -
 *   bcm_cosq_control_t. Can be:
 *     bcmCosqControlBandwidthBurstMax:
 *       Get 'max burst' for TCG (on 'TCG' calendar, DNX_OFP_RATES_EGQ_CAL_TCG). Use '*arg'
 *       to store retrieved maximal burst allowed, in bytes.
 * \param [out] arg -
 *   Pointer to int. General parameter. See 'type' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_TCG_CREDIT_TABLE, field TCG_MAX_BURST
 *   * Accessing DBAL table DBAL_TABLE_EGQ_SHAPING_TCG_CREDIT_TABLE, field DBAL_FIELD_TCG_MAX_BURST.
 * \see
 *   * dnx_ofp_rates_egq_tcg_max_burst_get
 */
shr_error_e dnx_cosq_egq_control_egress_port_tcg_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);
/**
 * \brief
 *   Get various parameters, depnding on type, for egress OTM port scheduler.
 *   Given 'gport' and priority ('cosq') to identify a specific qpair,
 *   use input 'type' to identify the required operation and use '*arg'
 *   to store output.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'.
 * \param [in] cosq -
 *   bcm_cos_queue_t. NOT USED by this procedure. Must be set to '0'.
 * \param [in] type -
 *   bcm_cosq_control_t. Can be:
 *     bcmCosqControlPacketLengthAdjust:
 *       Get 'compensation' for OTM port (on 'channelized arbiter' calendar,
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB). '*Arg' is used to store the value of 'header
 *       compensation', in bytes. Value should be smaller than 64.
 *       Header Compensation is used to adjust for differences in the packet header size
 *       to account for network headers added by the network interface.
 *     bcmCosqControlBandwidthBurstMax:
 *       Get 'max burst' for all slots corresponding to OTM port (on
 *       'DNX_OFP_RATES_EGQ_CAL_CHAN_ARB' calendar).
 *       '*Arg' is used to store the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_LIMIT_MAX.
 *     bcmCosqControlBandwidthBurstMaxEmptyQueue:
 *       Get 'max burst' for all OTM port shapers (on 'DNX_OFP_RATES_EGQ_CAL_CHAN_ARB'
 *       calendar) which are in 'empty' state.
 *       '*Arg' is used to store the value of the maximal burst allowed, in bytes.
 *       Value is limited by an upper limit of DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX.
 *     bcmCosqControlEgressPriorityOverCast:
 *       Get selected mode of operation for the two queues (unicast and multicast)
 *       corresponding to specified OTM port: If '0' then WFQ is done before SP.
 *       Else SP is done before WFQ. '*Arg' is used to store the 'enable' value
 *       (0 or 1).
 * \param [in] arg_p -
 *   Pointer to int. General output parameter. See 'type' above.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_port_is_high_get
 */
shr_error_e dnx_cosq_egq_control_egress_port_scheduler_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg_p);
/* 
 * This procedure is currently empty. Fill proper header
 * when it gets meaningful info.
 * Purpose:
 * Set various controls related to egress interface scheduler
 * (interface port)
 */
shr_error_e dnx_cosq_control_egress_interface_scheduler_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);
/**
 * Purpose:
 * Retrieve specific multicast configuration (out_dp) from port
 */
/* 
 * \brief -
 *   Retrieve specific multicast configuration (out_tc,out_dp) from egress multicast port.
 *   (out_tc is, essentially, 'offset' from base qpair)
 *     Given a gport plus priority (TC) and Drop-precedence (DP) of multicast traffic
 *     entering egress, get the priority for outgoing traffic (by inquiring its port).
 *     Also, get DP of outgoing traffic into 'config' below.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] gport -
 *   bcm_gport_t. Identifier of traffic by its assigned gport.
 * \param [in] ingress_pri -
 *   bcm_cos_t. Priority (TC) of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_TC-1 (7).
 * \param [in] ingress_dp -
 *   bcm_color_t. Drop precedence of traffic entering egress. Must be between 0 and
 *   DNX_DEVICE_COSQ_ING_NOF_DP-1 (3).
 * \param [in] flags -
 *   uint32. Must be BCM_COSQ_MULTICAST_SCHEDULED
 * \param [out] config_p -
 *   Pointer to structure of type bcm_cosq_egress_multicast_config_t. This procedure loads only
 *   two elements:
 *     'priority' -
 *       Priority set to assign to outgoing multicast traffic. Must be between 0 and
 *       DNX_EGR_NOF_Q_PRIO-1 (7).
 *     'scheduled_dp' -
 *       Drop precedence set to assign to outgoing multicast traffic.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * All info is extracted from sw. See dnx_am_template_egress_queue_mapping_data_get().
 * \see
 *   * dnx_am_template_egress_queue_mapping_data_get
 */
shr_error_e dnx_cosq_egq_gport_egress_multicast_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    uint32 flags,
    bcm_cosq_egress_multicast_config_t * config);
/* 
 * This procedure is currently empty. Fill proper header
 * when it gets meaningful info.
 * Purpose:
 * Get various controls related to egress interface scheduler
 * (interface port)
 */
int dnx_cosq_control_egress_interface_scheduler_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);
/**
 * \brief -
 *   Given 'BCM cosq' (priority), get the corresponding local 'EGQ priority'
 * \param [in] unit -
 *   Unit HW id
 * \param [in] cosq -
 *   int. BCM priority, to convert.
 * \param [out] prio_p -
 *   Pointer to dnx_egr_q_prio_e. This procedure loads pointed memory by
 *   'EGQ priority' corresponding to input 'cosq'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_gport_egress_scheduler_queue_sched_set
 */
shr_error_e dnx_cosq_egq_cosq_priority_convert(
    int unit,
    int cosq,
    dnx_egr_q_prio_e * prio_p);
/**
 * \brief
 *   Given 'gport' and intended 'priority' (cosq), verify that the latter is within
 *   priority range assigned for this 'gport'
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'logical_port' using which, allowed
 *   'priority' range is extracted.
 * \param [in] cosq -
 *   bcm_cos_queue_t. Intended Priority for 'gport'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_gport_egress_scheduler_queue_sched_set
 *   * dnx_algo_port_db
 */
shr_error_e dnx_cosq_egq_port_priority_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq);
/**
 * \brief
 *   Schedule mode for Channelized NIF port and CPU interface.
 *   Get value of Strict priority currently used by this system. SP range: H/L
 *   Given 'gport' to identify a specific base qpair,
 *   get its priority as specified in 'mode' (high or low).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact target qpair can be identified ('target qpair' = 'base qpair' plus
 *   'priority')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Must be '0'.
 * \param [out] mode -
 *   Pointer to int. This procedure loads pointed memory by the priority assigned to interface
 *   corresponding to indicated qpair. Can only be high (BCM_COSQ_SP0) or low (BCM_COSQ_SP1).
 *   See dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [out] weight -
 *   Pointer int. Ignored by this procedure. (Loaded by '-1')
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_QP_CFG, field QP_IS_HP
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, field DBAL_FIELD_QP_IS_HP
 * \see
 *   * dnx_egr_queuing_port_is_high_set
 */
shr_error_e dnx_cosq_egq_gport_egress_scheduler_queue_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/**
 * \brief -
 *   Given 'gport', convert to the corresponding 'logical port'
 *
 * \param [in] unit -
 *   Unit HW id
 * \param [in] gport -
 *   bcm_gport_t. Gport to convert
 * \param [out] user_port_p -
 *   Pointer to bcm_port_t. This procedure loads pointed memory by 'logical port'
 *   corresponding to input 'gport'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_egq_port_priority_validate
 */
shr_error_e dnx_cosq_egq_user_port_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_t * user_port_p);
/* 
 * }
 */

#endif /* COSQ_EGQ_H_INCLUDED */
