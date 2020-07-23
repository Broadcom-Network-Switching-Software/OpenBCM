 /** \file bcm_int/dnx/stat/crps/crps_mgmt.h
 * 
 * Internal DNX CRPS MGMT
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_MGMT_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/policer.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>

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
#define DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(_unit, _engine_id) \
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

#endif/*_DNX_CRPS_MGMT_INCLUDED__*/
