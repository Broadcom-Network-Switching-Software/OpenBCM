 /** crps_engine.h
 *
 *  DNX CRPS ENGINE H FILE. (INTERNAL CRPS FILE)
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_ENGINE_INCLUDED__
/*
 * {
 */
#define _DNX_CRPS_ENGINE_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
* \brief
*   set to base offset (called also base normalize) value of the engine in the hw.
*   it determnine the first Object-id that is admitted for this engine. (the offset of the engine)
*   Logical-Object-ID = (Object-ID - cfg-Base-Obj-ID) * cfg-Counter-Set-Size
*        + Counter-Set-Offset(5) + Object-Type-Offset + sampling_offset
*   Engine-First-Stat-Ndx = Logical-Object-ID -which its Object-Id is the smallest one that is admitted to the engine.
*   address-base_normalize-cfg = Engine-First-Stat-Ndx
* \param [in] unit          -  unid id
* \param [in] engine_p  -  pointer to the engine id and core.
* \param [in] set_val  -  base normalize base value
* \return
*   shr_error_e - Error Type
* \remark
*   none
* \see
*   none
*/
shr_error_e dnx_crps_engine_base_offset_hw_set(
    int unit,
    bcm_stat_engine_t * engine_p,
    uint32 set_val);
/**
* \brief
*   function handle the HW configuration for the DBAL table "CRPS_ENGINE_CFG"
* \param [in] unit    -
* \param [in] engine  -  pointer to the engine id and core.
* \param [in] config  -  pointer to the structure that hold the relevant parameters
* \return
*   shr_error_e - Error Type
* \remark
*    some fields that depended on the counter_set size, will be updaed only after the API bcm_dnx_stat_counter_set_mapping_set will be called,
 *   since contain the counter_set_size parameter.
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_hw_config_set(
    int unit,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_interface_t * config);

/**
* \brief
*   function handle the HW configuration for the DBAL table "CRPS_ENGINE_TYPE_CONFIG"
* \param [in] unit    -
* \param [in] engine  -  pointer to the engine id and core.
* \param [in] valid  - valid array for each type_id
* \param [in] offset  - offset array for each type_id
* \return
*   shr_error_e - Error Type
* \remark
*    some fields that depended on the counter_set size, will be updaed only after the API bcm_dnx_stat_counter_set_mapping_set will be called,
 *   since contain the counter_set_size parameter.
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_type_hw_set(
    int unit,
    bcm_stat_engine_t * engine,
    int *valid,
    uint32 *offset);

/**
* \brief
*   function handle the HW configuration for the DBAL table "CRPS_ENGINE_CFG"
* \param [in] unit    -
* \param [in] engine  -  pointer to the engine id and core.
* \param [out] clear_done  -  indicates if HW finished its clear memory action
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_clear_hw_mem_done_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *clear_done);

/**
* \brief
*   get enables parameters from HW per engine
* \param [in] unit    -  unit id
* \param [in] engine  -  pointer to the engine id and core.
* \param [out] enable_eviction  -  enable the eviction (output of the engine)
* \param [out] enable_counting  -  enable the counting (input to the engine)
* \return
*   shr_error_e - Error Type
 *\remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_enable_hw_get(
    int unit,
    bcm_stat_engine_t * engine,
    int *enable_eviction,
    int *enable_counting);

/**
* \brief
*
* \param [in] unit    -  unit id
* \param [in] engine  -  pointer to the engine id and core.
* \param [in] enable_eviction  -  enable the eviction (output of the engine)
* \param [in] enable_counting  -  enable the counting (input to the engine)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_enable_hw_set(
    int unit,
    bcm_stat_engine_t * engine,
    int enable_eviction,
    int enable_counting);

/**
* \brief
*   enable engine access to counters memory (mcp)
* \param [in] unit    -  unit id
* \param [in] engine  -  pointer to the engine id and core.
* \param [in] enable  -  enable/disable access
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_memory_access_enable(
    int unit,
    bcm_stat_engine_t * engine,
    int enable);

/**
* \brief
*   reset counters memory in HW. operation take time in HW
* \param [in] unit    -  unit id
* \param [in] engine_p  -  pointer to the engine id and core.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_hw_counters_reset(
    int unit,
    bcm_stat_engine_t * engine_p);

/**
* \brief
*   function read from data mapping hw table the values {addmission and counter_set_offset} in a given index
* \param [in] unit    -  unit id
* \param [in] engine  -  engineid and core id
* \param [in] index   -  entry in the data mapping table
* \param [out] value   -  strucutre of offset and addmission
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_data_mapping_hw_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 index,
    bcm_stat_expansion_data_value_t * value);

/**
* \brief
*   this function handle set API and get API. It configure one entry from the API. each entry defines one or more expansion data mapping indexes
*   the function calculate all the relevant data mapping table indexes that need to update. and set/get the value: {valid, counter_set_offset)
* \param [in] unit          -  unid id
* \param [in] write_value  -  is to write the value to the hw or read it.
* \param [in] engine  -  pointer to the engine id and core.
* \param [in] nof_interface_keys  - the size of the interfac_key array
* \param [in] interface_key           -  array of interface keys that defined by {core, command_id, src, type_id}
* \param [in] selection  - expansion selection bitmap array. (selection[4 types][2] - 4 types, bitmap of upto64 bits.)
* \param [in] data_mapping  -  strucutre of several conditions that generates the keys for the tables that need to update/read and
*                              its value structure.
*                              For get API, the structre type is "out".
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_data_mapping(
    int unit,
    int write_value,
    bcm_stat_engine_t * engine,
    int nof_interface_keys,
    bcm_stat_counter_interface_key_t * interface_key,
    uint32 selection[BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES][2],
    bcm_stat_expansion_data_mapping_t * data_mapping);

/**
* \brief
*   configure the data mapping table, in full spread mode.
*   In this mode, we will configure a range of entries starting form zero upto size-1 (input param).
*   the offset field = entry_index and the valid field = input_valid param.
*   For example:
*    index 0: {valid=TRUE, offset=0}, index 1: {valid=TRUE, offset=1},...,index N: {valid=TRUE, offset=(size-1)},
* \param [in] unit          -  unid id
* \param [in] engine  -  pointer to the engine id and core.
* \param [in] size  -  how many entries to configure: entries {0..(size-1)}
* \param [in] valid  - refer to HW valid field. is the entry valid or not.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*   .
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_data_mapping_full_spread_set(
    int unit,
    bcm_stat_engine_t * engine,
    int size,
    int valid);

/**
 * \brief -set which source is connected for each bank id (from the crps-meter shared memory banks).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  core-ID (support core_all)
 * \param [in] engine_id -  engine-ID
 * \param [in] section -  for big engines, there are two sections. One can use both, low or high.
 * \param [in] src_type - src type: crps, ing_meter, eg_meter
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 *   .
 */
shr_error_e dnx_crps_engine_bank_src_hw_set(
    int unit,
    int core_id,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section,
    dbal_enum_value_field_meter_crps_smem_src_types_e src_type);

/**
 * \brief -get which source is connected for each bank id (from the crps-meter shared memory banks).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  core-ID (support core_all)
 * \param [in] engine_id -  engine-ID
 * \param [in] section -  for big engines, there are two sections. One can use both, low or high.
 *                        for big engines, which contain two banks, it is not allowed to give section=ALL.
 * \param [out] src_type - src type: crps, ing_meter, eg_meter
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_engine_bank_src_hw_get(
    int unit,
    int core_id,
    int engine_id,
    dnx_crps_mgmt_shmem_section_t section,
    dbal_enum_value_field_meter_crps_smem_src_types_e * src_type);

/**
* \brief
*   get last command received - last address admitted and last
*   data admitted
* \param [in] unit            -  unid id
* \param [in] engine          -  pointer to the engine id and core.
* \param [in] last_address    -  last address admitted
* \param [in] last_data       -  last data admitted
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_last_command_hw_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *last_address,
    uint64 *last_data);

/**
* \brief
*   function copy data mapping table from one engine to another engine
* \param [in] unit    -  unit id
* \param [in] src_engine  -  read data from engineid and core id
* \param [in] dest_engine  -  write data to engineid and core id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*   .
*/
shr_error_e dnx_crps_engine_data_mapping_hw_copy(
    int unit,
    bcm_stat_engine_t * src_engine,
    bcm_stat_engine_t * dest_engine);

/*
 * }
 */
#endif/*_DNX_CRPS_ENGINE_INCLUDED__*/
