/** \file imb_ethu_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_ETHU_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_ETHU_INTERNAL_H_INCLUDED

/*
 * MACROs
 * {
 */

 /*
  * }
  */
/*
 * Structs
 * {
 */

 /*
  * }
  */

/**
 * \brief -
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id - ETHU index inside the core
 * \param [in] value - 1: enable TDM mask, 0: disable TDM mask
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_tdm_mask_hw_set(
    int unit,
    int core,
    int ethu_id,
    uint32 value);

/**
 * \brief - Enable / disable Tx data to the PM
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] lane - lane to enable data for
 * \param [in] enable - Enable/Disable value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_tx_data_to_pm_enable_hw_set(
    int unit,
    bcm_core_t core,
    int lane,
    int enable);

/**
 * \brief - Configure the ETHU in the specified Scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] sch_prio - scheduler to configure. see
 *        bcm_port_nif_scheduler_t
 * \param [in] sch_weight - each scheduler has more than one bit per
 *        ETHU. each bit translates to BW in the scheduler.
 *        weight specifies how many bits the ETHU should have in
 *        the specified scheduler.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * High/TDM prio schedulers have 2 bits per ETHU. (valid
 *     values are 0-2)
 *   * Low prio scheduler has 4 bits per ETHU. (valid values are
 *     0-4)
 * \see
 *   * None
 */
int imb_ethu_scheduler_config_hw_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    uint32 sch_prio,
    uint32 sch_weight);

/**
 * \brief - set the requested weight in the low priority RMC
 *        scheduler. this schduler is performing RR on all RMCs
 *        with traffic, and each rMC has more than one bit in
 *        the RR. the weight is actually the number of bits in
 *        the RR.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core
 * \param [in] ethu_id - ETHU id inside the core
 * \param [in] rmc_id - logical fifo id
 * \param [in] sch_weight - weight value
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_rmc_low_prio_scheduler_config_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int rmc_id,
    int sch_weight);

/**
 * \brief - Set the logical FIFO threshold after overflow.
 * after the fifo reaches overflow, it will not resume writing
 * until fifo level will get below this value.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] ethu_id - ETHU id inside the core
 * \param [in] rmc_id - logical fifo id
 * \param [in] thr_after_ovf - value to set as threshold after
 *        overflow
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rmc_thr_after_ovf_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int rmc_id,
    int thr_after_ovf);

/**
 * \brief - Set PRD logic in Bypass mode. the PRD parser will be bypassed.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] ethu_id_in_core - ethu index inside the core
 * \param [in] is_bypass - bypass mode
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_bypass_hw_set(
    int unit,
    int core,
    int ethu_id_in_core,
    uint32 is_bypass);

/**
 * \brief - set TX start threshold. this threshold represent the
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] lane - lane inside the Core
 * \param [in] start_thr - threshold value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_tx_start_thr_hw_set(
    int unit,
    bcm_core_t core,
    int lane,
    uint32 start_thr);

/**
 * \brief - Get ETHU configuration in specified scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] sch_prio - scheduler to configure.
 * \param [out] weight - each scheduler has more than one bit
 *        per ETHU. each bit translates to BW in the scheduler.
 *        weight specifies how many bits the ETHU should have in
 *        the specified scheduler.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_scheduler_config_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    uint32 sch_prio,
    uint32 *weight);

/**
 * \brief - Reset the context in the scheduler.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] sch_prio - scheduler to configure.
 * \param [in] in_reset - reset indication
 *        Can use one of the following:
 *          IMB_COMMON_IN_RESET - put the scheduler in reset.
 *          IMB_COMMON_OUT_OF_RESET - get the scheduler out of reset.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_nmg_reset_per_sch_prio_hw_set(
    int unit,
    bcm_port_t port,
    uint32 sch_prio,
    int in_reset);

/**
 * \brief - Reset the port TX in the NMG
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] in_reset - reset indication.
 * in_reset = 1 --> put in reset
 * in_reset = 0 --> take out of reset
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_tx_nmg_reset_hw_set(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Reset the port TX in the ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] in_reset - reset indication.
 * in_reset = TRUE  --> put in reset
 * in_reset = FALSE --> take out of reset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_tx_reset_hw_set(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Control for the flush process.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] lane - lane to set the flush for
 * \param [in] flush_enable - Enable/Disable value
 * flush_enable = TRUE  --> Lane in flush
 * flush_enable = FALSE --> take the Lane out of flush
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_tx_egress_flush_hw_set(
    int unit,
    bcm_core_t core,
    int lane,
    int flush_enable);

/**
 * \brief - Drop data to RMC from PM.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [in] rmc_id - logical fifo id
 *                      Use IMB_COSQ_ALL_RMCS to configure all valid
 *                      logical fifo ids of the port.
 * \param [in] enable - Enable/Disable value
 * enable = TRUE  --> Drop data comming from PM to RMC.
 * enable = FALSE --> Disable data drop.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_drop_data_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    int enable);

/**
 * \brief - Put RMC in flush mode.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [in] rmc_id - logical fifo id.
 *                      Use IMB_COSQ_ALL_RMCS to configure all valid
 *                      logical fifo ids of the port.
 * \param [in] enable - Enable/Disable value
 * enable = TRUE  --> Enable flush mode for RMC.
 * enable = FALSE --> Disable flush mode for RMC.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_rmc_flush_mode_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    int enable);

/**
 * \brief - Enable flush contexts in NMG
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [in] sch_priority - scheduler to configure. See bcm_port_nif_scheduler_t.
 * \param [in] enable - Enable/Disable value
 * enable = TRUE  --> Enable flush mode for sch_priority.
 * enable = FALSE --> Disable flush mode for sch_priority.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_nmg_flush_context_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    uint32 sch_priority,
    int enable);

/**
 * \brief - Put NMG in flush mode.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] enable - Enable/Disable value
 * enable = TRUE  --> Enable flush mode for NMG.
 * enable = FALSE --> Disable flush mode for NMG.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_nmg_flush_mode_enable_hw_set(
    int unit,
    bcm_core_t core,
    int enable);

/**
 * \brief - Execute ETHU RX flush.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [in] enable - Enable/Disable value
 * enable = TRUE  --> Start flushing.
 * enable = FALSE --> Stop flushing.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_flush_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int enable);

/**
 * \brief - Get RMC counter value
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [in] rmc_id - logical fifo id.
 * \param [in] counter - RMC number of entries
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_rmc_counter_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id,
    uint32 *counter);
/**
 * \brief - Polling RMC counters until they will be empty twice.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [in] rmc_id - logical fifo id.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_rmc_counter_polling(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc_id);

/**
 * \brief - Configure the RMC priority (Low, High, TDM)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to configure
 * \param [in] sch_prio - scheduler. see
 *        bcm_port_nif_scheduler_t
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_rmc_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    bcm_port_nif_scheduler_t sch_prio);

/**
 * \brief - map physical lane in the ETHU + PRD priority to a
 *        specific RMC (logical FIFO)
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info
 * \param [in] is_map_valid - map valid indication: TRUE => map,
 *        FALSE=> unmap
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_lane_to_rmc_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid);

/**
 * \brief - map PRD priority to a
 *        specific RMC (logical FIFO)
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info
 * \param [in] is_map_valid - map valid indication: TRUE => map,
 *        FALSE=> unmap
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_rx_priority_to_rmc_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid);

/**
 * \brief - map RMC (logical FIFO) to a physical lane in the ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] first_ethu_lane - port first lane index in the ethu
 * \param [in] rmc - logical fifo info.
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_rmc_to_lane_map_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int first_ethu_lane,
    dnx_algo_port_rmc_info_t * rmc);

/**
 * \brief - Set the Eth unit
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id
 * \param [in] ethu_id - ethu index inside the core
 * \param [in] threshold - threshold to set the scheduler counter decrement
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_sch_cnt_dec_thres_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int threshold);
/**
 * \brief - Reset the port in the NMG
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] rmc_id - logical fifo. if -1 means set all RMCs
 *        of the port.
 * \param [in] in_reset - reset indication.
 * in_reset = 1 --> put in reset
 * in_reset = 0 --> take out of reset
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_nmg_reset_hw_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset);

/**
 * \brief - Reset the port RX in the ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - logical fifo. if -1 means set all RMCs
 *        of the port.
 * \param [in] in_reset - reset indication.
 * in_reset = TRUE  --> put in reset
 * in_reset = FALSE --> take out of reset
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_reset_hw_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset);

/**
 * \brief - Enable Rx Fifos (RMCs) to generate Priority FC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] rmc - Rx Fifo to enable
 * \param [in] priority - priority of the generated PFC
 * \param [in] enable - TRUE or FALSE to enable/disable the PFC generation
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 priority,
    uint32 enable);

/**
 * \brief - Get the enable value for Rx Fifos (RMCs) to generate Priority FC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] rmc - Rx Fifo for which to get the value
 * \param [in] priority - priority of the generated PFC
 * \param [out] enable - TRUE or FALSE, indicating enabled/disabled PFC generation
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 priority,
    uint32 *enable);

/**
 * \brief - Enable/disable value Rx Fifos (RMCs) to generate Link Level FC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] lane_in_core - Lane inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] enable - TRUE or FALSE to enable/disable the LLFC generation
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core,
    int rmc,
    uint32 enable);

/**
 * \brief - Enable / disable PM to stop TX on all of a port's lanes according to a LLFC signal from CFC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] lane_in_core - Lane inside the core
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core,
    uint32 enable);

/**
 * \brief - Set enable / disable PM to stop TX on a port according to received PFC signal.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] pfc - PFC for which to get enable status
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_stop_pm_from_pfc_enable_set(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core,
    int pfc,
    uint32 enable);

/**
 * \brief - Get enable / disable PM to stop TX on a port according to received PFC signal.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] pfc - PFC for which to get enable status
 * \param [out] enable - enable / disable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_stop_pm_from_pfc_enable_get(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core,
    int pfc,
    uint32 *enable);

/**
 * \brief - Set enable/disable LLFC RMC threshold to generate PFC on the specified port.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_gen_pfc_from_llfc_thresh_enable_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    uint32 rmc,
    uint32 enable);

/**
 * \brief - Get enable / disable indication for LLFC RMC threshold to generate PFC on the specified port.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [out] enable - enable / disable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_gen_pfc_from_llfc_thresh_enable_get(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    uint32 rmc,
    uint32 *enable);

/**
 * \brief - Set the LLFC/PFC thresholds for FC generation from Quad MAC Lane FIFO (QMLF).
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] flags - flags indicating the threshold to set
 * \param [in] threshold - Threshold value
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 flags,
    uint32 threshold);

/**
 * \brief - Get the LLFC/PFC thresholds for FC generation from Quad MAC Lane FIFO (QMLF).
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] rmc - Rx Fifo for which to set the value
 * \param [in] flags - flags indicating the threshold to set
 * \param [out] threshold - Threshold value
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int rmc,
    uint32 flags,
    uint32 *threshold);

/**
 * \brief - Set the Flow Control Interface in/out of reset for a specified ETHU.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] in_reset - TRUE/FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_fc_reset_set(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    uint32 in_reset);

/**
 * \brief - Enable/disable the generation of a LLFC based on a signal from Global Resources thresholds for a specified ETHU.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] lane_in_core - Lane inside the core
 * \param [in] enable - TRUE/FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_llfc_from_glb_rsc_enable_set(
    int unit,
    bcm_core_t core,
    int lane_in_core,
    uint32 enable);

/**
 * \brief - Per ETHU map the received PFC - each priority received by NIF should be mapped to a priority that will be sent to EGQ in order to stop a Q-pair.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 * \param [in] nif_priority - priority of the received PFC
 * \param [in] egq_priority - priority to be sent to EGQ
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_pfc_rec_priority_map(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    uint32 nif_priority,
    uint32 egq_priority);

/**
 * \brief - Per ETHU unmap all priorities of the received PFC.
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Device core
 * \param [in] ethu_id_in_core - ETHU index inside the core
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_pfc_rec_priority_unmap(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core);

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] sif_instance_id - statistics interface instance (port) id
 * \param [in] ethu_id_in_core - ethu_id_in_core (which extracted from the logical port)
 * \param [in] first_lane_in_port - first_lane_in_port (which extracted from the logical port)
 * \param [in] ethu_select - select ETHU/CLU
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_to_stif_instance_map(
    int unit,
    int core,
    int sif_instance_id,
    int ethu_id_in_core,
    int first_lane_in_port,
    int ethu_select);

/**
 * \brief - get the RMC current FIFO level
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to check fifo level
 * \param [out] fifo_level - RMC FIFO fullness level
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_rmc_level_get(
    int unit,
    bcm_port_t port,
    uint32 rmc_id,
    uint32 *fifo_level);

/**
 * \brief - get PRD drop counter value, per RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC index inside the ETHU
 * \param [out] count - counter value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_drop_count_hw_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint64 *count);

/**
 * \brief - Reset the port credit in the ETHU.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_credit_tx_reset(
    int unit,
    bcm_port_t port);

/**
 * \brief - Get NIF RX fifo status using dbal
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc index
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    int rmc_id,
    uint32 *max_occupancy,
    uint32 *fifo_level);

/**
 * \brief - Get NIF TX fifo status using dbal
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] pm_credits - indicate the credits from PM tx buffer
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits);

/**
 * \brief - Mask EGQ credit per ETHU/CLU port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - mask value, 0 or 1
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_credit_mask_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief - enable PRD hard stage per RMC. this effectively
 *        enable the PRD feature.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC id inside the ETHU
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_hard_stage_enable_hw_set(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 enable);

/**
 * \brief - get enable indication for the PRD hard stage per RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - RMC id inside the ETHU
 * \param [out] enable - enable indication
 *
 * \return
 *   int -see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_hard_stage_enable_hw_get(
    int unit,
    bcm_port_t port,
    uint32 rmc,
    uint32 *enable);

/**
 * \brief - fill the TM priority map (PRD hard stage). add the
 *        the map priority value to be returned per TC + DP
 *        values
 *
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] tc - TC value (Traffic Class)
 * \param [in] dp - DP value (Drop Precedence)
 * \param [in] priority - priority to set for TC+DP
 * \param [in] is_tdm - TDM signal
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_tm_tc_dp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority,
    uint32 is_tdm);

/**
 * \brief - get information from TM priority map (PRD hard
 *        stage). get the priority given for a combination of
 *        TC+DP.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc - TC value (Traffic Class)
 * \param [in] dp - DP value (Drop Precedence)
 * \param [out] priority - priority for the TC+DP
 * \param [out] is_tdm - TDM signal
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_tm_tc_dp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority,
    uint32 *is_tdm);

/**
 * \brief - fill the IP priority table (PRD hard stage). set
 *        priority value for a specific DSCP.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] dscp - DSCP field value
 * \param [in] priority - priority for the DSCP
 * \param [in] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_ip_dscp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority,
    uint32 is_tdm);

/**
 * \brief - get information from IP priority table (PRD hard
 *        stage). get the priority returned for a specific DSCP
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] dscp - DSCP field value
 * \param [out] priority - returned priority
 * \param [out] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_ip_dscp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority,
    uint32 *is_tdm);

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set
 *        priority value for a specific EXP value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] exp - EXP field value
 * \param [in] priority - priority for the EXP
 * \param [in] is_tdm - TDM signal
 * \return
 *   int - see .h file
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_mpls_exp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority,
    uint32 is_tdm);

/**
 * \brief - get information from the MPLS priority table. get
 *        the priority returned for a specific EXP value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] exp - EXP field value
 * \param [out] priority - priority for the EXP
 * \param [out] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_mpls_exp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority,
    uint32 *is_tdm);

/**
 * \brief - fill the ETH (VLAN) Priority table (PRD hard
 *        stage). set a priority value to match a specific
 *        PCP+DEI combination
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pcp - PCP value
 * \param [in] dei - DEI value
 * \param [in] priority - priority for the PCP+DEI
 * \param [in] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_eth_pcp_dei_hw_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority,
    uint32 is_tdm);

/**
 * \brief - get information from the ETH (VLAN) priority table
 *        (PRD hard stage). get the priority returned for a
 *        specific combination of PCP+DEI
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pcp - PCP value
 * \param [in] dei - DEI value
 * \param [out] priority - priority for the PCP+DEI
 * \param [out] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_map_eth_pcp_dei_hw_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority,
    uint32 *is_tdm);

/**
 * \brief - set configurable ether type to a ether type code.
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code (1-6)
 * \param [in] ether_type_val - ether type value to map to the
 *        ether type code
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_custom_ether_type_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val);

/**
 * \brief - get the ether type value mapped to a specific ether
 *        type code, out of the configurable ether types (codes
 *        1-6).
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code (1-6)
 * \param [out] ether_type_val - ether type mapped to the ether
 *        type code
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_custom_ether_type_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val);

/**
 * \brief - set the offset base for the TCAM key. it means the
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet
 * 1=>end of eth header
 * 2=>end of header after eth header.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to set hte
 *        offset to. (for different ether types we might want
 *        different offset base)
 * \param [in] offset_base - offset base value. this offset base
 *        should already match the DBAL enum type. see
 *        NIF_PRD_TCAM_KEY_OFFSET_BASE
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base);

/**
 * \brief - get the current offset base for the given ether
 *        code.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to retrieve the
 *        offset base.
 * \param [out] offset_base - returned offset base.
 *
 * \return
 *   int -xee _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base);

/**
 * \brief - set ether type size (in bytes) for the given ether
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code of the ether
 *        type
 * \param [in] ether_type_size - ether type size value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ether_type_size_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size);

/**
 * \brief - get the ether type currently set for a specific
 *        ether type code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code for the ether
 *        type size
 * \param [out] ether_type_size - returned ether type size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ether_type_size_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size);

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key
 *        per ether type (total of 16 keys). the key is build of
 *        4 offsets given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCAM entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        -----------------------------------------------------
 *        35              31       23       15       7        0
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code (0-15)
 * \param [in] offset_index - index of the offset to set to the
 *        key (0-3)
 * \param [in] offset_value - offset value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tcam_entry_key_offset_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value);

/**
 * \brief - get information about the TCAM key for a specific
 *        ether type.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize
 *        the key.
 * \param [in] offset_index - which offset of the key to get
 * \param [out] offset_value - returned offset value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tcam_entry_key_offset_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value);

/**
 * \brief - fill the PRD soft stage TCAM table. there are 32
 *        entries in the table. if there is a hit, the priority
 *        for the packet will be taken from the TCAM entry
 *        information
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] entry_index - index in the table to set
 * \param [in] entry_info - information to fill the table entry
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tcam_entry_hw_set(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info);

/**
 * \brief - get information from the PRD soft stage (TCAM)
 *        table. get specific entry information
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] entry_index - index in the TCAM table to get
 *        information from
 * \param [out] entry_info - entry information
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tcam_entry_hw_get(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info);

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the higest priroity (3). each
 *        packet is compared against all control frame
 *        properties of the ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        set (0-3)
 * \param [in] control_frame_config - information to recognize
 *        to as control frame
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_ethu_prd_control_frame_hw_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/**
 * \brief - get Control Frame properties recognized by the
 *        parser.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        get (0-3)
 * \param [in] control_frame_config - control frame information
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_control_frame_hw_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/**
 * \brief - set MPLS special label properties. if one of the
 *        MPLS labels is identified as special label and its
 *        value match one of the special label values, priority
 *        for the packet is taken from the special label
 *        properties.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] special_label_index - special label index to
 *        configure (0-3)
 * \param [in] label_config - special label configuration:
 *        value, priority, TDM indication.
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_mpls_special_label_hw_set(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

/**
 * \brief - get current configuration of MPLS special label
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] special_label_index - index of the special label
 *        to retrieve
 * \param [out] label_config - special label properties:
 *        value, priority, TDM indication.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_mpls_special_label_hw_get(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc_offset - TC offset on ITMH header
 * \param [in] dp_offset - DP offset in ITMH header
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_itmh_offsets_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset);

/**
 * \brief - get ITMH TC and DP offstes in PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC offset in ITMH header
 * \param [out] dp_offset - DP offset in ITMH header
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_itmh_offsets_hw_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset);

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc_offset - TC offset in FTMH header
 * \param [in] dp_offset - DP offset in FTMH header
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ftmh_offsets_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc_offset,
    uint32 dp_offset);

/**
 * \brief - get FTMH TC + DP offsets in PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tc_offset - TC offset in FTMH header
 * \param [in] dp_offset - DP offset in FTMH header
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ftmh_offsets_hw_get(
    int unit,
    bcm_port_t port,
    uint32 *tc_offset,
    uint32 *dp_offset);

/**
 * \brief - enable checking for MPLS special label, if packet is
 *        identified as MPLS
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_mpls_special_label_enable_hw_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

/**
 * \brief - get enable indication for MPLS special label.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_mpls_special_label_enable_hw_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief - set one of the port's TPID values to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] tpid_index - TPID index (0-3)
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tpid_hw_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 tpid_value);

/**
 * \brief - get the port's TPID value from the PRD parser.
 *          if a packet TPID is not recognized,
 *          the packet will get the default priority. each port
 *          can have four TPID values.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] tpid_index - TPID index (0-3)
 * \param [out] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tpid_hw_get(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 *tpid_value);

/**
 * \brief - set PRD Parser whether to ignore indication of IP
 *        dscp even if packet is identified as IP.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] ignore_ip_dscp - ignore IP DSCP indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_ip_dscp_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_ip_dscp);

/**
 * \brief - get from PRD parser indication whether it ignores IP
 *        DSCP
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] ignore_ip_dscp - ignore IP DSCP indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_ip_dscp_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_ip_dscp);

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is
 *        identified as MPLS
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] ignore_mpls_exp - ignore MPLS EXP indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_mpls_exp_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_mpls_exp);

/**
 * \brief - get from PRD parser indication whether it ignores
 *        MPLS EXP
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] ignore_mpls_exp - ignore MPLS EXP indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_mpls_exp_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_mpls_exp);

/**
 * \brief - set PRD Parser whether to ignore inner tag PCP DEI
 *        indication even if packet is identified as double
 *        tagged
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] ignore_inner_tag - ignore inner VLAN tag
 *        indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_inner_tag_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_inner_tag);

/**
 * \brief - get from PRD parser indication whether it ignores
 *        inner VLAN tag
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] ignore_inner_tag - ignore inner VLAN tag
 *        indication
 *
 * \return
 *   int -see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_inner_tag_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_inner_tag);

/**
 * \brief - set PRD Parser whether to ignore outer tag PCP DEI
 *        indication even if packet is identified as VLAN tagged
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] ignore_outer_tag - ignore outer VLAN tag
 *        indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_outer_tag_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 ignore_outer_tag);

/**
 * \brief - get from PRD parser indication whether it ignores
 *        outer VLAN tag
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] ignore_outer_tag - ignore outer VLAN tag
 *        indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_ignore_outer_tag_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *ignore_outer_tag);

/**
 * \brief - set default priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overridden by the priority from the soft
 *        stage.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] default_priority - default priority
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_default_priority_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 default_priority);

/**
 * \brief - get default priority given in the PRD hard stage
 *        parser.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] default_priority - default priority
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_default_priority_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *default_priority);

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] enable_eth - enable soft stage for eth port
 *        (should only be set for eth ports)
 * \param [in] enable_tm - enable soft stage for TM ports
 *        (should only be set for ports with TM headers -
 *        ITMH/FTMH)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_soft_stage_enable_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 enable_eth,
    uint32 enable_tm);

/**
 * \brief - get enable indication for PRD soft stage
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] enable_eth - soft stage enabled for eth port
 * \param [out] enable_tm - soft stage enabled for TM port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_soft_stage_enable_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *enable_eth,
    uint32 *enable_tm);

/**
 * \brief - set outer tag size for the port. if port is port
 *        extander, the outer tag size should be set to 8B,
 *        otherwise 4B
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] outer_tag_size - outer tag size, compatibale to
 *        DBAL enum field. see VLAN_OUTER_TAG_SIZE.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_outer_tag_size_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 outer_tag_size);

/**
 * \brief - get current outer tag size for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] outer_tag_size - outer tag size, compatible to
 *        DBAL enum type. see VLAN_OUTER_TAG_SIZE
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_outer_tag_size_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *outer_tag_size);

/**
 * \brief - set PRD port type, according to the header type of
 *        the port
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [in] prd_port_type - PRD port type. should match DBAL
 *        enum type. see NIF_PRD_PORT_TYPE.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_port_type_hw_set(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 prd_port_type);

/**
 * \brief - get PRD port type for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_id - ethu id in core
 * \param [in] prd_profile - prd port profile.
 *                           For CDU units this is the lane in ethu.
 *                           For CLU units this is the port prd profile, according to mapping done by user.
 * \param [out] prd_port_type - PRD port type. matches the DBAL
 *        enum type. see NIF_PRD_PORT_TYPE.
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_port_type_hw_get(
    int unit,
    bcm_core_t core,
    int ethu_id,
    int prd_profile,
    uint32 *prd_port_type);

/**
* \brief - Reset per port instrumentation counters for PFC deadlock detection mechanism
*
* \param [in] unit - chip unit id
* \param [in] core - core id
* \param [in] lane_in_core - lane inside the core
* \return
*   int - see _SHR_E_ *
*
* \remarks
*   * None
* \see
*   * None
*/
int imb_ethu_internal_pfc_deadlock_counters_reset(
    int unit,
    bcm_core_t core,
    uint32 lane_in_core);

/**
* \brief - Reset per ETHU instrumentation counters
*
* \param [in] unit - chip unit id
* \param [in] core - core id
* \param [in] ethu_in_core - ethu id in core
* \return
*   int - see _SHR_E_ *
*
* \remarks
*   * None
* \see
*   * None
*/
int imb_ethu_internal_instru_counters_reset(
    int unit,
    bcm_core_t core,
    int ethu_in_core);

/**
 * \brief - Get the debug info for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_in_core - ethu id in core
 * \param [out] ethu_port_num - port number internal for the ETHU ID
 *              of the port that triggered the PFC deadlock detection
 * \param [out] pfc_num - PFC that triggered the PFC deadlock detection
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_pfc_deadlock_debug_info_get(
    int unit,
    bcm_core_t core,
    int ethu_in_core,
    int *ethu_port_num,
    int *pfc_num);

/**
 * \brief - Get the max PFC duration for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_in_core - ethu id in core
 * \param [out] max_duration - max duration of the PFC
 *             that will trigger the PFC deadlock recovery
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_pfc_deadlock_max_duration_get(
    int unit,
    bcm_core_t core,
    int ethu_in_core,
    uint32 *max_duration);

/**
 * \brief - Set to HW the max PFC duration for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] core - chip core id
 * \param [in] ethu_in_core - ethu id in core
 * \param [in] max_duration - max duration of the PFC
 *             that will trigger the PFC deadlock recovery
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_pfc_deadlock_max_duration_set(
    int unit,
    bcm_core_t core,
    int ethu_in_core,
    uint32 max_duration);

/**
 * \brief - Configure start TX for ETHU RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] rmc_id - RMC ID
 * \param [in] start_tx - Start Tx value
 * \param [in] enable - Enable or disable start tx
 *
 * \return
 *   int - seee _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_internal_port_rx_start_tx_thr_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    uint32 start_tx,
    int enable);

/**
 * \brief - Reset the L1 TX credit counter
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] in_reset - In or out of reset
 *
 * \return
 *   int - seee _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_l1_tx_credit_counter_reset_hw_set(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Configure Rx credit counter for L1 mismatch rate
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] rmc_id - RMC ID
 * \param [in] credit_val - Credit value
 * \param [in] enable - Enable or disable start tx
 *
 * \return
 *   int - seee _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_credit_counter_hw_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int credit_val,
    int enable);

/**
 * \brief - Configure power down of ETHU
 */
int imb_ethu_power_down_set(
    int unit,
    bcm_core_t core_id,
    int ethu_id,
    int power_down);

#endif /* IMB_ETHU_INTERNAL_H_INCLUDED */
