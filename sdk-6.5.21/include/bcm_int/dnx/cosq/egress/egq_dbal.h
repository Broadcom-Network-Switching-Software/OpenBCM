/**
 * \file bcm_int/dnx/cosq/egress/egq_dbal.h
 * 
 *
 * Mainly prototypes of low level DBAL access procedures, for EGQ on DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef EGQ_DBAL_H_INCLUDED
/* { */
#define EGQ_DBAL_H_INCLUDED

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
* \brief - sets tcg cir sp. configuration per core
*
* \param [in] unit -
 *  Unit-ID
* \param [in] gport -
*   If 'gport' is non zero then core is extracted from it. Otherwise,
*   all cores are loaded with the input 'enable' value.
* \param [in] enable - enable
*
* \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
* \remark
*   * If 'enable' is asserted then  HW chooses CIR according to SP to
 *    lower TCG, otherwise do RR between all TCGs
* \see
*   * None
*/
shr_error_e dnx_egq_dbal_tcg_cir_sp_en_set(
    int unit,
    bcm_gport_t gport,
    uint32 enable);
/**
* \brief - gets tcg cir sp. configuration per core
*
* \param [in] unit -
*   Unit-ID
* \param [in] gport -
*   If 'gport' is non zero then core is extracted from it. Otherwise,
*   core '0' is used to retrieve output into *enable_p.
* \param [out] enable_p -
*   This procedure loads pointed memory by 'enable' flag as written in HW.
*
* \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
* \remark
*   * If 'enable' is asserted (This procedure loads it by a non-zero value) then
*     HW chooses CIR according to SP to lower TCG, otherwise do RR between all TCGs
* \see
*   * None
*/
shr_error_e dnx_egq_dbal_tcg_cir_sp_en_get(
    int unit,
    bcm_gport_t gport,
    uint32 *enable_p);
/**
 * \brief - configures egress multicast replication fifo parameters received from dnx data to HW
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core-id
 * \param [in] tdm_exist -  indicates if there is tdm traffic in the system
 *  *
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_emr_fifo_default_set(
    int unit,
    int core,
    uint32 tdm_exist);
/**
 * \brief - Set the flow control mapping CMIC to EGQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core ID
 * \param [in] cpu_cos -  CPU_COS which is channel number + cosq
 * \param [in] qpair -  Q-pair to be mapped to the corresponding CPU_COS
 *
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_cmic_egq_map_set(
    int unit,
    bcm_core_t core,
    int cpu_cos,
    int qpair);
/*********************************************************************
*     Enable/disable the egress shaping.
 */
/**
 * \brief -
 *    Set HW to enable or disable all the following shapers (on all cores):
 *      OTM_SPR_ENA,
 *      QPAIR_SPR_ENA,
 *      TCG_SPR_ENA,
 * \param [in] unit -
 *   Unit HW ID
 * \param [in] enable - 
 *   Value to load into HW. '1' indicates 'enable'
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_shaping_enable_set(
    int unit,
    uint8 enable);
/**
 * \brief -
 *    Get, from HW, the flag indicating whether to to enable or disable all
 *    the following shapers (on all cores):
 *      OTM_SPR_ENA,
 *      QPAIR_SPR_ENA,
 *      TCG_SPR_ENA,
 *    Actually, only OTM_SPR_ENA is read since they are all loaded by
 *    the same value.
 * \param [in] unit -
 *   Unit HW ID
 * \param [out] enable_p - 
 *   Pointer to uint8. This procedure loads pointed memory by the value of
 *   the OTM_SPR_ENA flag. A value of '1' indicates 'enable'
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_shaping_enable_get(
    int unit,
    uint8 *enable_p);
/**
 * \brief - get egq rqp counter information .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [out] counter_info -  store counter information
 * \param [out] nof_counter -  return number of counters
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_rqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter);
/**
 * \brief - clear egq pqp counter .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_pqp_counter_configuration_reset(
    int unit,
    int core);
/**
 * \brief - get egq pqp counter information .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [out] counter_info -  store counter information
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_pqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info);
/**
 * \brief - clear egq epni counter .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_epni_counter_configuration_reset(
    int unit,
    int core);
/**
 * \brief - get egq epni counter information .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] flag -  flag
 * \param [out] counter_info -  store counter information
 * \param [out] nof_counter -  return number of counters
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_epni_counter_info_get(
    int unit,
    int core,
    int flag,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter);
/**
 * \brief
 *   Get data from HW. Given qpair, as key, get WFQ-related weigths corresponding
 *   to unicast and to multicast traffic.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] entry_offset -
 *   Qpair for which information is to be retrieved. Range is 0 - 511
 *   (DNX_EGR_NOF_Q_PAIRS)
 * \param [out] egq_dwm_tbl_data_p -
 *   Pointer to dnx_egq_dwm_tbl_data_t. This procedure loads
 *   egq_dwm_tbl_data_p->mc_or_mc_low_queue_weight,
 *   egq_dwm_tbl_data_p->uc_or_uc_low_queue_weight (each assumed to be uint32!) from HW table
 *   at location corresponding to specified 'entry_offset' (qpair).
 *   Both parameters are 8 bits in size and indicate the relative weight for uc/mc
 *   traffic associated with this qpair. 
 *   Weight of 0 for one queue indicates that it has SP (strict priority) over the other.
 *   When both have equal weights, this implies simple RR (round robin). Maximum weight
 *   difference is 255:1
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_DWM, fields UC_OR_UC_LOW_QUEUE_WEIGHT, MC_OR_MC_LOW_QUEUE_WEIGHT
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, fields MC_OR_MC_LOW_QUEUE_WEIGHT,
 *     UC_OR_UC_LOW_QUEUE_WEIGHT using 'entry_offset' and 'core' as key.
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_weights_tbl_get(
    int unit,
    int core,
    uint32 entry_offset,
    dnx_egq_dwm_tbl_data_t * egq_dwm_tbl_data_p);
/**
 * \brief
 *   Write data into HW. Given qpair, as key, set WFQ-related weigths corresponding
 *   to unicast and to multicast traffic.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] entry_offset -
 *   Qpair for which information is to be set. Range is 0 - 511
 *   (DNX_EGR_NOF_Q_PAIRS)
 * \param [in] egq_dwm_tbl_data_p -
 *   Pointer to dnx_egq_dwm_tbl_data_t. This procedure loads
 *   egq_dwm_tbl_data_p->mc_or_mc_low_queue_weight,
 *   egq_dwm_tbl_data_p->uc_or_uc_low_queue_weight (each assumed to be uint32!) into HW table
 *   at location corresponding to specified 'entry_offset' (qpair).
 *   Both parameters are 8 bits in size and indicate the relative weight for uc/mc
 *   traffic associated with this qpair. 
 *   Weight of 0 for one queue indicates that it has SP (strict priority) over the other.
 *   When both have equal weights, this implies simple RR (round robin). Maximum weight
 *   difference is 255:1
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_DWM, fields UC_OR_UC_LOW_QUEUE_WEIGHT, MC_OR_MC_LOW_QUEUE_WEIGHT
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, fields MC_OR_MC_LOW_QUEUE_WEIGHT,
 *     UC_OR_UC_LOW_QUEUE_WEIGHT using 'entry_offset' and 'core' as key.
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_weights_tbl_set(
    int unit,
    int core,
    uint32 entry_offset,
    dnx_egq_dwm_tbl_data_t * egq_dwm_tbl_data_p);
/**
 * \brief
 *   Get data from HW. Given TC, DP, MC flag, and profile, all
 *   used, together, as key, get TC and DP for traffic corresponding to
 *   this profile.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core_id -
 *   Core to use for HW access.
 * \param [in] entry -
 *   Pointer to dnx_egq_tc_dp_map_tbl_entry_t. Input key to table of profiles:
 *     map_profile (4b)
 *     is_egr_mc   (1b)
 *     tc          (3b)
 *     dp          (2b)
 * \param [out] tbl_data -
 *   Pointer to dnx_egq_tc_dp_map_tbl_data_t. This procedure loads
 *   tbl_data->tc, tbl_data->dp (each assumed to be uint32!) from HW table
 *   at location corresponding to specified 'entry'. TC is 3 bits. DP is 2 bits.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table PQP_TC_DP_MAP, fields EGRESS_TC, CGM_MC_DP
 *   * Accessing DBAL table EGQ_TC_DP_MAP, fields EGRESS_TC, CGM_MC_DP
 *     using 'entry' and 'core_id' as key.
 * \see
 *   * None
 */
int dnx_egq_dbal_tc_dp_map_tbl_get(
    int unit,
    int core_id,
    dnx_egq_tc_dp_map_tbl_entry_t * entry,
    dnx_egq_tc_dp_map_tbl_data_t * tbl_data);
/**
 * \brief
 *   Load data into HW. Given TC, DP, MC flag, and profile, all
 *   used, together, as key, set TC and DP for traffic corresponding to
 *   this profile.
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core_id -
 *   Core to use for HW access.
 * \param [in] entry -
 *   Pointer to dnx_egq_tc_dp_map_tbl_entry_t. Input key to table of profiles:
 *     map_profile (4b)
 *     is_egr_mc   (1b)
 *     tc          (3b)
 *     dp          (2b)
 * \param [in] tbl_data -
 *   Pointer to dnx_egq_tc_dp_map_tbl_data_t. This procedure loads
 *   tbl_data->tc, tbl_data->dp (each assumed to be uint32!) into HW table
 *   at location corresponding to specified 'entry'. TC is 3 bits. DP is 2 bits.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table PQP_TC_DP_MAP, fields EGRESS_TC, CGM_MC_DP
 *   * Accessing DBAL table EGQ_TC_DP_MAP, fields EGRESS_TC, CGM_MC_DP
 *     using 'entry' and 'core_id' as key.
 * \see
 *   * None
 */
int dnx_egq_dbal_tc_dp_map_tbl_set(
    int unit,
    int core_id,
    dnx_egq_tc_dp_map_tbl_entry_t * entry,
    dnx_egq_tc_dp_map_tbl_data_t * tbl_data);
/**
 * \brief
 *   Extract data from HW. Given PP port, get its assigned profile (which
 *   determines the TC for traffic on this port).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core_id -
 *   Core to retrieve info for.
 * \param [in] internal_port -
 *   uint32. PP port for which to get required info. Range: 0 - 255 (8 bits)
 * \param [out] egq_ppct_tbl_data -
 *   Pointer to dnx_egq_ppct_tbl_data_t. This procedure loads
 *   egq_ppct_tbl_data->cos_map_profile (assumed to be uint32!) by profile
 *   value corresponding to specified port. Profile range: 0 - 7 (3 bits)
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table ERPP_PER_PORT_TABLE, field TC_MAP_PROFILE
 *   * Accessing DBAL table EGRESS_PP_PORT, field COS_MAP_PROFILE
 *     using 'internal_port' and 'core_id' as key.
 * \see
 *   * None
 */
int dnx_egq_dbal_cos_map_profile_get(
    int unit,
    int core_id,
    uint32 internal_port,
    dnx_egq_ppct_tbl_data_t * egq_ppct_tbl_data);
/**
 * \brief
 *   Load data into HW. Given PP port, set its assigned profile (which
 *   determines the TC for traffic on this port).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core_id -
 *   Core to use for HW access.
 * \param [in] internal_port -
 *   uint32. PP port for which to set required info. Range: 0 - 255 (8 bits)
 * \param [out] egq_ppct_tbl_data -
 *   Pointer to dnx_egq_ppct_tbl_data_t. This procedure loads
 *   egq_ppct_tbl_data->cos_map_profile (assumed to be uint32!) into HW
 *   at location corresponding to specified port. Profile range: 0 - 7 (3 bits)
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table ERPP_PER_PORT_TABLE, field TC_MAP_PROFILE
 *   * Accessing DBAL table EGRESS_PP_PORT, field COS_MAP_PROFILE
 *     using 'internal_port' and 'core_id' as key.
 * \see
 *   * None
 */
int dnx_egq_dbal_cos_map_profile_set(
    int unit,
    int core_id,
    uint32 internal_port,
    dnx_egq_ppct_tbl_data_t * egq_ppct_tbl_data);
/* 
 * \brief -
 *   Set global maximal burst size for all 'OTM ports' (on ALL cores) which are
 *   in 'empty' state.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] arg -
 *   uint32. Value of maximal burst in credits. Note that if a value of '0' is selected
 *   then no maximal limit is set on collection of credits for qpairs which are empty.
 *   Value is limited to a maximal value of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that all cores are updated.
 * \see
 *   * dnx_egq_dbal_empty_port_max_credit_set
 *   * bcm_dnx_cosq_control_set
 */
shr_error_e dnx_egq_dbal_empty_port_max_credit_set(
    int unit,
    int arg);
/* 
 * \brief -
 *   Get global maximal burst size for all 'OTM ports' (on ALL cores) which are
 *   in 'empty' state.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] arg -
 *   Pointer to uint32. This procedure loads pointed memory by the value of
 *   maximal burst in credits. Note that if a value of '0' is selected
 *   then no maximal limit was set on collection of credits for qpairs which
  *  are empty.
 *   Value is limited to a maximal value of DNX_OFP_RATES_BURST_LIMIT_MAX.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Note that all cores are updated.
 * \see
 *   * dnx_egq_dbal_empty_port_max_credit_get
 *   * bcm_dnx_cosq_control_get
 */
shr_error_e dnx_egq_dbal_empty_port_max_credit_get(
    int unit,
    int *arg);
/**
 * \brief
 *   Schedule mode for Channelized NIF port and CPU interface.
 *   Choose Strict priority to Connect to. SP range: H/L
 *   Given 'gport' and priority ('cosq') to identify a specific qpair,
 *   set its priority as specified in 'mode' (high or low).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact target qpair can be identified ('target qpair' = 'base qpair' plus
 *   'priority')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Priority of target qpair. Can either be in the range from 0 to
 *   7 or BCM_COSQ_HIGH_PRIORITY or BCM_COSQ_LOW_PRIORITY. See dnx_cosq_bcm_cosq_to_egr_q_prio().
 * \param [in] mode -
 *   int. Priority to assign to interface corresponding to indicated qpair. Can only
 *   be high (BCM_COSQ_SP0) or low (BCM_COSQ_SP1). See dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [in] weight -
 *   int. Ignored by this procedure.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_QP_CFG, field QP_IS_HP
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, field DBAL_FIELD_QP_IS_HP
 * \see
 *   * dnx_cosq_gport_egress_scheduler_port_sched_set
 *   * dnx_cosq_gport_egress_scheduler_queue_sched_get
 */
shr_error_e dnx_egq_dbal_gport_scheduler_queue_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/**
 * \brief - set FQP interface profile parameters 
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core
 * \param [in] profile_id -  profile id
 * \param [in] profile_params - profile parameter structure
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_fqp_profile_params_set(
    int unit,
    int core,
    int profile_id,
    dnx_fqp_profile_params_t * profile_params);

/**
* \brief - set FQP interface profile id
*
* \param [in] unit -  Unit-ID
* \param [in] core -  core
* \param [in] egr_if -  egress interface
* \param [in] profile_id -  profile id
*
* \return
*   shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_egq_dbal_fqp_profile_id_set(
    int unit,
    int core,
    int egr_if,
    int profile_id);

/**
 * \brief - get FQP interface profile id 
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core
 * \param [in] egr_if -  egress interface
 * \param [out] profile_id -  profile id
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_fqp_profile_id_get(
    int unit,
    int core,
    int egr_if,
    int *profile_id);

/**
 * \brief - get FQP interface profile params 
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core
 * \param [in] egr_if -  egress interface
 * \param [in] profile_id -  profile id
 * \param [out] profile_params - profile parameter strucutre
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_fqp_profile_params_get(
    int unit,
    int core,
    int egr_if,
    int profile_id,
    dnx_fqp_profile_params_t * profile_params);

/**
 * \brief - set PQP almost empty thresholds
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egq_dbal_pqp_almost_empty_thresholds_set(
    int unit);
/* 
 * }
 */
/* } */
#endif /* EGQ_DBAL_H_INCLUDED */
