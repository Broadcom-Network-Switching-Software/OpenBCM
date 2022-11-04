 /** \file bcm_int/dnx/stat/crps/crps_mgmt.h
 * 
 * Internal DNX CRPS MGMT
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNX_CRPS_MGMT_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/policer.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>

/*
 * MACROs
 * {
 */

#define DNX_CRPS_DATABASE_RESOURCE "CRPS DATABASE"

/*
 *  Subcounter index -- each counter is a group, consisting of sub counters.
 */
#define DNX_CRPS_MGMT_SUB_CTR1 0  /** sub counter for packets (ctr1) */
#define DNX_CRPS_MGMT_SUB_CTR2 1 /** sub counter for bytes (ctr2)  */
#define DNX_CRPS_MGMT_SUB_COUNT 2 /** number of sub counters */

#define DNX_CRPS_MGMT_IS_DOUAL_COUNTER_FORMAT(_unit, _format_type) \
    (((dnx_data_crps.engine.counter_format_types_get(_unit, _format_type)->nof_bits_for_packets > 0) && \
            (dnx_data_crps.engine.counter_format_types_get(_unit, _format_type)->nof_bits_for_bytes > 0)) ? TRUE : FALSE)

/** If dual counter or full size counter*/
#define DNX_CRPS_MGMT_NOF_SUB_COUNT(_unit, _format_type) \
    ((DNX_CRPS_MGMT_IS_DOUAL_COUNTER_FORMAT(_unit, _format_type)) ? 2 : 1)

#define DNX_CRPS_EXPANSION_TOTAL_NOF_BITS bcmStatExpansionTypeMax

/** 
  * \brief - engine is valid only if the configuration started and ended
  */
#define DNX_CRPS_MGMT_ENGINE_VALID_GET(_unit, _core_id, _engine_id, _valid) \
{\
        int _conf_started; \
        int _conf_done; \
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_started.get(_unit, _core_id, _engine_id, &_conf_started)); \
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.config_done.get(_unit, _core_id, _engine_id, &_conf_done)); \
        _valid = ((_conf_started == TRUE) && (_conf_done == TRUE)) ? TRUE : FALSE; \
}

/** 
  * \brief -  check if the engine_id is one of the engines that can be used for eth policer. there is one for ingress and one for egress.
  *             thiese engines are small and can be used as single bucket mode only
  */
#define DNX_CRPS_METER_SHMEM_IS_METER_SMALL_ENGINE(_unit, _engine_id) \
    (_engine_id == dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(_unit) || \
     _engine_id == dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(_unit))

/**
  * \brief - return if the given source is OAM source
  */
#define DNX_CRPS_MGMT_SOURCE_IS_OAM(_unit, _source) \
        (_source == bcmStatCounterInterfaceIngressOam || _source == bcmStatCounterInterfaceEgressOam || _source == bcmStatCounterInterfaceOamp)

/**
  * \brief - invalid dma channel definition
  */
#define DNX_CRPS_INVALID_DMA_CHANNEL (-1)

/**
  * \brief - is user bitmap relevant for the expansion type
  */
#define DNX_CRPS_EXPANSION_TYPE_BITMAP_IS_RELEVANT(_expansion_type) \
    (_expansion_type == bcmStatExpansionTypeMetaData || - \
        _expansion_type == bcmStatExpansionTypePortMetaData || - \
        _expansion_type == bcmStatExpansionTypeTmPortMetaData || - \
        _expansion_type == bcmStatExpansionTypePpPortMetaData)

/**
  * \brief - is the given interface source has a dynamic metadata
  */
#define DNX_CRPS_EXPANSION_METADATA_IS_DYNAMIC(_source) \
    (_source == bcmStatCounterInterfaceEgressTransmitPp || _source == bcmStatCounterInterfaceEgressPp)

/**
  * \brief - first crps small engine
  */
#define FIRST_A_ENGINE_ID (0)

/**
  * \brief - first crps medium engine
  */
#define FIRST_B_ENGINE_ID(_unit) \
    (dnx_data_crps.engine.nof_engines_get(_unit) - \
            dnx_data_crps.engine.nof_mid_engines_get(_unit) - \
            dnx_data_crps.engine.nof_big_engines_get(_unit))

/**
  * \brief - first crps big engine
  */
#define FIRST_C_ENGINE_ID(_unit) \
    (dnx_data_crps.engine.nof_engines_get(_unit) - \
            dnx_data_crps.engine.nof_big_engines_get(_unit))

/**
  * \brief - number of counters in A-type engine
  */
#define IS_A_TYPE_ENGINE(_unit, _engine_id) \
    (dnx_data_crps.engine.engines_info_get(_unit, FIRST_A_ENGINE_ID)->nof_entries == dnx_data_crps.engine.engines_info_get(_unit, _engine_id)->nof_entries)

/**
  * \brief - number of counters in B-type engine
  */
#define IS_B_TYPE_ENGINE(_unit, _engine_id) \
    (dnx_data_crps.engine.engines_info_get(_unit, FIRST_B_ENGINE_ID(_unit))->nof_entries == dnx_data_crps.engine.engines_info_get(_unit, _engine_id)->nof_entries)

/**
  * \brief - number of counters in C-type engine
  */
#define IS_C_TYPE_ENGINE(_unit, _engine_id) \
    (dnx_data_crps.engine.engines_info_get(_unit, FIRST_C_ENGINE_ID(_unit))->nof_entries == dnx_data_crps.engine.engines_info_get(_unit, _engine_id)->nof_entries)

#define NOF_SMALL_ENGINES_GET(_unit) \
    (dnx_data_crps.engine.nof_engines_get(_unit) - dnx_data_crps.engine.nof_big_engines_get(_unit) - \
                    dnx_data_crps.engine.nof_mid_engines_get(_unit))

#define DNX_CRPS_COUNTER_FORMAT_TYPE_IS_STATE(_format_type) (_format_type == bcmStatCounterFormatNarrowState || _format_type == bcmStatCounterFormatWideState)

/*
 * }
 */

/*
 * Typdef/Enums/Structures
 * {
 */

/** internal Enum to select the section in the policer-crps shared memory engines. */
typedef enum
{
    dnx_crps_mgmt_shmem_section_all = bcmPolicerEngineSectionAll,
    dnx_crps_mgmt_shmem_section_low = bcmPolicerEngineSectionLow,
    dnx_crps_mgmt_shmem_section_high = bcmPolicerEngineSectionHigh
} dnx_crps_mgmt_shmem_section_t;

/*
 * Functions prototypes
 * {
 */

/** 
 * \brief - take lock of the counters dma fifos
 */
shr_error_e dnx_crps_fifo_state_lock_take(
    int unit);

/** 
 * \brief - give lock of the counters dma fifos
 */
shr_error_e dnx_crps_fifo_state_lock_give(
    int unit);

/** 
 * \brief - take lock of the counters cache
 */
shr_error_e dnx_crps_counter_state_lock_take(
    int unit);

/** 
 * \brief - give lock of the counters cache
 */
shr_error_e dnx_crps_counter_state_lock_give(
    int unit);

/**
 * \brief - Initialize dnx crps module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * DNX data related crps module information
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * crps HW related regs/mems
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_init(
    int unit);

/**
 * \brief - Deinitialize dnx crps module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_deinit(
    int unit);

/**
 * \brief - attach or detach an engine memory for meter use.
 *            called from policer mgmt.
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] core_id -  core-ID 
 *   \param [in] engine_id -  engine-ID 
 *   \param [in] section -  for big engines, there are two sections. One can use both, low or high.  
 *   \param [in] is_ingress - is the database belong to ingress meter
 *   \param [in] attach -  True for attach, false for detach
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * support core ALL
 * \see
 *   * None
 */
shr_error_e dnx_crps_mgmt_meter_engine_attach_detach(
    int unit,
    int core_id,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section,
    int is_ingress,
    int attach);

/**
 * \brief - verify funcion to check that the engine+section is available for meter usage
 *            called policer_mgmt
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   \param [in] core_id -  core-ID 
 *   \param [in] engine_id -  engine-ID 
 *   \param [in] section -  for big engines, there are two sections. One can use both, low or high.  
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * support core ALL
 * \see
 *   * None
 */
shr_error_e dnx_crps_meter_engine_available_verify(
    int unit,
    int core_id,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section);

/**
 * \brief
 *  wrapper function for disable the eviction of all engines.
 *  function perform:
 *  1. disable all active eviction engines.
 *  2. wait that the last counters will be copied to host.
 *  3. process the host fifo by demend in order to clear the fifo.
 *
 * \param [in] unit -unit id
 * \return
 *  shr_error_e
 * \remark
 *   function should be called before doing manual sync, part of warm boot or during de-init, in order to support automatic sync warmboot.
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_disable_wrapper(
    int unit);

/**
* \brief
*  verify function, which serve other modules who use the crps and wants to verify that the counter that is generated can be counted.
*  the function search for an engine that can count the input object_stat_id according to the parameters given.
*
* \param [in] unit -unit id
* \param [in] core_id - core id
* \param [in] source_type - the source type that connected to the engine
* \param [in] command_id - command/interface id
* \param [in] type_id - type id
* \param [in] object_stat_id - the counter pointer
* \return
*  shr_error_e
* \remark
*   NONE
* \see
*   NONE
 */
shr_error_e dnx_crps_mgmt_counter_generation_verify(
    int unit,
    int core_id,
    bcm_stat_counter_interface_type_t source_type,
    int command_id,
    int type_id,
    int object_stat_id);

/**
* \brief
*   verification database - database_id is allocated and valid core_id
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] database  -  database configuration
*   \param [in] core_all_valid  -  TRUE - bcm_core_all allowed;
*          FALSE - unique core, bcm_core_all is not allowed
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
shr_error_e dnx_crps_mgmt_database_structure_verify(
    int unit,
    bcm_stat_counter_database_t * database,
    int core_all_valid);

/**
* \brief
 *  verification on command_id in correct range according to the
 *  source
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
 *  \param [in] source  -  source type
 *  \param [in] command_id  -  the command id that has to be
 *         checked
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
shr_error_e dnx_crps_mgmt_counter_command_id_verify(
    int unit,
    bcm_stat_counter_interface_type_t source,
    int command_id);

/**
* \brief
*  get the bank id (in the shared memory between crps and meter) according to the engine id and the section.
*  source
*  \par DIRECT INPUT:
*  \param [in] unit    -  Unit-ID
*  \param [in] engine_id  -  engine id 
*  \param [in] section  -  engine section. (each engine has two banks, called sections).
*  \param [out] bank_id    -  bank id
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
shr_error_e dnx_crps_mgmt_meter_shmem_bank_id_get(
    int unit,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section,
    int *bank_id);

/**
* \brief
*  find the set counts the given stat-object
*  source
*  \par DIRECT INPUT:
*  \param [in] unit    -  Unit-ID
*  \param [in] core_id  -  core id
*  \param [in] database_id  -  database id
*  \param [in] stat_object_id  -
*  \param [out] object_set_idx_get    -  set index
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
shr_error_e dnx_crps_mgmt_stat_object_id_set_id_get(
    int unit,
    int core_id,
    int database_id,
    int stat_object_id,
    int *object_set_idx_get);

/**
* \brief
*  rather or not, the database contains more than one counter set
*  source
*  \par DIRECT INPUT:
*  \param [in] unit    -  Unit-ID
*  \param [in] core_id  -  core id
*  \param [in] database_id  -  database id
*  \param [out] is_multi_set_database_get    -
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
shr_error_e dnx_crps_mgmt_is_multi_sets_database(
    int unit,
    int core_id,
    int database_id,
    int *is_multi_set_database_get);

/**
* \brief
*  return rather or not the given destination is supported for the given counter format_type.
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [in] format_type    -  counter format type
*  \param [in] destination_type  -  eviction destination_type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - TRUE/FALSE
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int dnx_crps_mgmt_eviction_destination_for_format_type_is_supported(
    int unit,
    bcm_stat_counter_format_type_t format_type,
    bcm_eviction_destination_type_t destination_type);

/**
* \brief
*  return the maximum number of counters can be in one engine's entry in the currend device
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [out] nof_sub_countes_per_entry    -
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
shr_error_e dnx_crps_max_nof_sub_counters_per_entry_get(
    int unit,
    int *nof_sub_countes_per_entry);

/**
* \brief
*  get the relevant algo rate shadow and convert it in 64bit variable
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [in] core_id - core id
*  \param [in] engine_id - counter engine id
*  \param [in] shadow_idx - the shadow index
*  \param [out] shadow_val -
*  * \par INDIRECT INPUT:
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
shr_error_e dnx_crps_mgmt_rate_algo_shadow_get(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 *shadow_val);

/**
* \brief
*  set the relevant algo rate shadow SW state
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [in] core_id - core id
*  \param [in] engine_id - counter engine id
*  \param [in] shadow_idx - the shadow index
*  \param [in] shadow_val -
*  * \par INDIRECT INPUT:
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
shr_error_e dnx_crps_mgmt_rate_algo_shadow_set(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 shadow_val);

/**
* \brief
*  get the relevant sequential rate shadow and convert it in 64bit variable
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [in] core_id - core id
*  \param [in] engine_id - counter engine id
*  \param [in] shadow_idx - the shadow index
*  \param [out] shadow_val -
*  * \par INDIRECT INPUT:
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
shr_error_e dnx_crps_mgmt_rate_seq_shadow_get(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 *shadow_val);

/**
* \brief
*  set the relevant sequential rate shadow SW state
*  \par DIRECT INPUT:
*  \param [in] unit -
*  \param [in] core_id - core id
*  \param [in] engine_id - counter engine id
*  \param [in] shadow_idx - the shadow index
*  \param [in] shadow_val -
*  * \par INDIRECT INPUT:
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
shr_error_e dnx_crps_mgmt_rate_seq_shadow_set(
    int unit,
    int core_id,
    int engine_id,
    int shadow_idx,
    uint64 shadow_val);

/**
 * \brief
 *      whether or not the rate calculation feature is enable
 * \param [in] unit - unit id
 * \param [in] core_id - one core at a time only
 * \param [in] engine_id - engine_id
 * \param [out] is_enable - TRUE/FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_rate_calculation_is_enable(
    int unit,
    int core_id,
    int engine_id,
    int *is_enable);

/**
 * \brief
 *      get from HW whether or not the conditional action enable
 * \param [in] unit - unit id
 * \param [in] core_id - one core at a time only
 * \param [in] engine_id - engine_id
 * \param [out] arg - TRUE/FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_conditional_action_enable_get(
    int unit,
    int core_id,
    int engine_id,
    uint32 *arg);

/**
 * \brief
 *      get from HW whether or not the no_eviction conditional action enable
 * \param [in] unit - unit id
 * \param [in] core_id - one core at a time only
 * \param [in] engine_id - engine_id
 * \param [out] arg - TRUE/FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_conditional_action_no_eviction_get(
    int unit,
    int core_id,
    int engine_id,
    uint32 *arg);

/**
* \brief
*   Initialize the next_seq_counter_idx parameter for rate calculation
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] core_id -  Specific core
*   \param [in] engine_id  -  engine identifier
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -
* \par INDIRECT OUTPUT:
*   * update sw state  - parameter next_seq_counter_idx
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_rate_calculation_expected_sequential_counter_idx_init(
    int unit,
    int core_id,
    int engine_id);

#endif/*_DNX_CRPS_MGMT_INCLUDED__*/
