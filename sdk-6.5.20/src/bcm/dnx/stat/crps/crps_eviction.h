 /** \file crps_eviction.h
 * 
 *  DNX CRPS SRC_INTERFACE H FILE. (INTERNAL CRPS FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_EVICTION_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_EVICTION_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
* \brief  
*  
* \par DIRECT INPUT: 
*   \param [in] unit      -  unit id
*   \param [in] seq_timer_interval  -  sequiential timer interval
*   \param [in] engine    -  pointer to the engine id and core.
*   \param [in] eviction  -  structure configuration
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_eviction_engine_hw_set(
    int unit,
    uint32 seq_timer_interval,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction);

/**
* \brief  
*   init the bg thread and other databases related to the eviction
* \par DIRECT INPUT: 
*   \param [in] unit      -  unit id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_eviction_init(
    int unit);
/**
* \brief  
*   deinit the bg thread and other databases related to the eviction
* \par DIRECT INPUT: 
*   \param [in] unit      -  unit id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_eviction_deinit(
    int unit);

/**
* \brief  
*   run over all engines and find all active CRPS FIFOs and connect DMA channels for them
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core    -  0/1/ALL
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * used for warm-boot. (in cold boot, all engines are not valid yet)
* \see
*   * None
*/
shr_error_e dnx_crps_dma_fifo_full_attach(
    int unit,
    bcm_core_t core);

/**
 * \brief
 *      connect one CRPS FIFO to DMA channel
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id -0/1
 * \param [in] fifo_id -
 *     fifo_id 0/1
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_dma_fifo_attach(
    int unit,
    int core_id,
    int fifo_id);

/**
* \brief  
*    run over all CRPS FIFOs and disconnect them from CMIC dma channels mechanism.
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
*   \param [in] core    -  0/1/ALL
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_eviction_dma_fifo_full_detach(
    int unit,
    bcm_core_t core);
/**
 * \brief
 *      disconnect one CRPS FIFO from DMA channel
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id 
 * \param [in] fifo_id -
 *     fifo_id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_dma_fifo_detach(
    int unit,
    int core_id,
    int fifo_id);

/**
 * \brief
 *      get a list of counters from sw counters database.
 * \param [in] unit -unit id 
 * \param [in] flags - flags 
 * \param [in] core_id -core id 
 * \param [in] engine_id -engine_id
 * \param [in] nstat -number of counters to get. also indicates the size of arrays: counters_id and sub_count
 * \param [in] counters_id -array of counters_id to get. counter_id is the HW entry in the engine. 
 * \param [in] sub_count -array of sub counter. 
 *                   Driver holds for each HW counter entry two counters. This array indicates which sub-counter to read.
 * \param [out] output_data - array which holds the value of each counter.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   In order to get the entire stats synced a single time, we lock the counters and give after getting all stats.
 *   This is the reason for getting array of stats to this function and one stat each call.
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_counters_get(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int nstat,
    int *counters_id,
    int *sub_count,
    bcm_stat_counter_output_data_t * output_data);

/**
* \brief  
*   create and activate bg thread using sal mechanism
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_eviction_bg_thread_attach(
    int unit);

/**
* \brief  
*   stop and destroy the bg thread using sal mechanism
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_eviction_bg_thread_detach(
    int unit);

/**
 * \brief
 *      set the sequential timer interval in HW
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     engine id and core_id
 * \param [in] time_between_scan_usec -
 *     time between sequential scans. in micro seconds units
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_seq_timer_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 time_between_scan_usec);

/**
 * \brief
 *      get the sequential time (in usec) between scan from HW
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     engine id and core_id
 * \param [out] time_between_scan_usec -
 *     time between sequential scans. in micro seconds units
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */

shr_error_e dnx_crps_eviction_seq_timer_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *time_between_scan_usec);

/**
 * \brief
 *      set the sequential boundaries in HW
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     flags - use default boundaries or set new
 * \param [in] engine -
 *     engine id and core_id
 * \param [in] boundaries -
 *     start and end of the sequential boundaries
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief
 *      get the sequential boundaries from HW
 * \param [in] unit -
 *     unit id
 * \param [in] flags - currently not used
 *     flags 
 * \param [in] engine -
 *     engine id and core_id
 * \param [out] boundaries -
 *     start and end of the sequential boundaries
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief
 *  wrapper function which check if need to enable the relevant DMA FIFO and activate the bg thread (if not already active)
 *
 * \param [in] unit -unit id
 * \param [in] core_id -core_id
 * \param [in] engine_id -engine_id 
 * \param [out] bg_thread_enable_called -indicates if the bg thread enable was already called, as protection mechanism, 
 *                                                                  in order that it will not called twice, when calling this function in all engines loop (init stage)
 * \return
 *  shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_collection_handle(
    int unit,
    int core_id,
    int engine_id,
    int *bg_thread_enable_called);

/**
 * \brief
 *  function check if need to detach bg thread. If so, it detach it.
 *  It go over all DMA FIFOs, from all cores. If all FIFOs are invalid - it detach the bg thread.
 * \param [in] unit -unit id
 * \return
 *  shr_error_e
 * \remark
 *   We call this function, each time that engine is removed
 * \see
 *   NONE
 */
shr_error_e dnx_crps_bg_thread_detach_check(
    int unit);

/**
 * \brief
 *      set the sequential boundaries in HW - when  using the
 *      flag BCM_STAT_EVICTION_RANGE_ALL should set range all
 *      for all engines in the database
 * \param [in] unit -
 *     unit id
 * \param [in] database -
 *     database id and core_id
 * \param [in] type_id - type id
 * \param [in] boundaries -
 *     start and end of the sequential boundaries - not used
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_database_eviction_boundaries_range_all_set(
    int unit,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief
 *      set the sequential boundaries in HW
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     flags - use default boundaries or set new
 * \param [in] database -
 *     database id and core_id
 * \param [in] type_id - type id
 * \param [in] boundaries -
 *     start and end of the sequential boundaries
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_database_eviction_boundaries_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief
 *      get the sequential boundaries from HW
 * \param [in] unit -
 *     unit id
 * \param [in] flags - currently not used
 *     flags 
 * \param [in] database -
 *     database id and core_id
 * \param [in] type_id - type_id 
 * \param [out] boundaries -
 *     start and end of the sequential boundaries
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_database_eviction_boundaries_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries);

/*
 * } 
 */
#endif/*_DNX_CRPS_EVICTION_INCLUDED__*/
