 /** \file crps_mgmt_internal.h
 * 
 *  DNX CRPS MGMT INTERNAL H FILE. (INTERNAL CRPS FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_MGMT_INTERNAL_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_MGMT_INTERNAL_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>

/*
 * DEFINES
 * {
 */
 /** 
 * \brief
 * defines the size of array that hold selection bitmap. 
 */
#define CRPS_MGMT_SELECTION_ARR_SIZE (2)
#define DNX_CRPS_MGMT_ENGINE_INVALID (-1)

#define DNX_CRPS_MGMT_MAX_NSTAT (DNX_DATA_MAX_CRPS_ENGINE_MAX_COUNTER_SET_SIZE*2)

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * strucutures
 * {
 */

 /** strucutre which describle a range {start,end} */
typedef struct dnx_cprs_mgmt_range_s
{
    int start;
    int end;
} dnx_cprs_mgmt_range_t;

/*
 * }
 */

/**
* \brief  
*   function return how many counters avalable in one 68 bits counter entry. 2 for formats pkts_and_pks, max,latency. 1 for all the rest
* \par DIRECT INPUT:
*   \param [in] unit    -  unid id
*   \param [in] format  -  counter format
*   \param [out] nof_counters  -  number of counters : 1 or 2.
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
shr_error_e dnx_crps_mgmt_nof_counters_per_entry_get(
    int unit,
    bcm_stat_counter_format_type_t format,
    int *nof_counters);
/**
* \brief
*   handle the counter interface set api. update the sw state and the hw
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] database  -  this is the key -defined by database_id and core.
*   \param [in] config  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_interface_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config);

/*
 * { Functions prototypes
 */

/**
* \brief  
*   handle the counter interface get api. read the parameters from sw state
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit-id
*   \param [in] flags   -  
*   \param [in] database  -  this is the key -defined by database_id and core.
*   \param [out] config  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_interface_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config);

/**
* \brief  
*   handle the counter eviction set api
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] engine  -  this is the key -defined by engine_id and core.
*   \param [in] eviction  -  configuration structure
* \par INDIRECT INPUT: 
*   * 
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * NONE
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_eviction_set(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction);

/**
* \brief  
*  
* \par DIRECT INPUT: 
*   \param [in] unit      -  unit id
*   \param [in] flags     -  
*   \param [in] engine    - this is the key -defined by engine_id and core.  
*   \param [out] eviction  -  configuration structure
* \par INDIRECT INPUT: 
*   * sw state eviction parameters
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_eviction_get(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction);

/**
* \brief  
*   handle the counter enable set api. update the sw state and configure the HW
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] database  -  this is the key -defined by database_id and core.
*   \param [in] enable  -  configuration structure
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e -
* \par INDIRECT OUTPUT: 
*   * update sw state  - structure dnx_engine_info_t
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_database_enable_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable);

/**
* \brief  
*   handle the counter enable get api. read from sw state the configuration
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] database  -  this is the key -defined by database_id and core.
*   \param [out] enable  -  configuration structure
* \par INDIRECT INPUT: 
*   * sw state engine enable parameters
* \par DIRECT OUTPUT: 
*   shr_error_e -
* \par INDIRECT OUTPUT: 
*   * 
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_database_enable_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable);

/**
* \brief  
*   handle the expansion selection set api. define the expansion format
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [in] expansion_select  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_expansion_select_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   handle the expansion selection get api. get the configuration from HW
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] interface  -  this is the key -defined the interface by core, src, command_id and type
*   \param [out] expansion_select  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_expansion_select_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   handle the counter set mapping. defines the counter_set size and what each counter in the set represent.
*   it verify the input parameters and update the HW.
* \par DIRECT INPUT: 
*   \param [in] unit             -  unit ID
*   \param [in] flags            -  FULL_SPREAD - will spread all the expansion data in the counter_set.
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [in] counter_set_map  -  configuration strucutre
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * the function set also the range of the engine (together with parameters that was configure in API bcm_dnx_stat_counter_interface_set)
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_set_mapping_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map);

/**
* \brief  
*   get the counter set mapping configuration.
* \par DIRECT INPUT: 
*   \param [in] unit             -  unit ID
*   \param [in] flags            -  NONE
*   \param [in] database  -  pointer to structure which contain the database_id and core_id
*   \param [out] counter_set_map  -  configuration strucutre
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * 
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_set_mapping_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map);

/**
 * \brief
 *       check if an engine is a candidate to be evicted by the crps driver. It has to be valid, connected to local host and in physical record format
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id 
 * \param [in] engine_id -
 *     engine id
 * \param [out] valid -
 *     valid=TRUE if engine configuration is valid and the engine fifo connected to local host. otherwise valid=FALSE
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_engine_eviction_valid_get(
    int unit,
    int core_id,
    int engine_id,
    int *valid);

/**
 * \brief
 *      handle explicitly the counters collecting from sw counters database. 
 *      Driver get the object_stat_id and the engine to collect from
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] core_id -core_id
 * \param [in] engine_id -engine_id
 * \param [in] type_id -type_id
 * \param [in] object_stat_id -the counter pointer that the source send to CRPS 
 * \param [in] nstat -how many counters to get. (all must belong to the same set, therefore, nstat <= counter_set_size)
 * \param [in] stat_arr -array of statistics to get.  
 * \param [out] nof_found -how many statistics was found and updated (read) in this engine
 * \param [out] found_arr -array which indicate for each statistic if was found or not.
 * \param [out] output_data -
 *     structure of output counters value array
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   If found_arr == NULL, the function will return error for stats that weren't found. this is used for explicit_get API
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_explicit_get(
    int unit,
    uint32 flags,
    int core_id,
    int engine_id,
    int type_id,
    int object_stat_id,
    int nstat,
    int *stat_arr,
    int *nof_found,
    int *found_arr,
    bcm_stat_counter_output_data_t * output_data);

/**
 * \brief
 *      handle the counters collecting from sw counters database. the counter and engine is defined implictly.
 *      Driver calculate the engine and counter_id to collect from.
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] stat_counter_input_data -
 *     structure of input parameters which dfeine which counter to collect
 * \param [out] stat_counter_output_data -
 *     structure of output counters value array
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data);

/**
 * \brief
 *      enable or disable the bg thread if needed and update db
 * \param [in] unit -
 *     unit id
 * \param [in] enable -
 *     enable or disable bg thread
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_background_collection_set(
    int unit,
    int enable);

/**
 * \brief
 *      get if the bg thread is enabled or disabled (knowledge is from db)
 * \param [in] unit -
 *     unit id
 * \param [out] enable -
 *     enable or disable bg thread
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_background_collection_get(
    int unit,
    int *enable);

/**
 * \brief
 *      set the next engine id in sw crps database. Can be used to concatante new engine to a counters database.
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     core_id and engine_id
 * \param [in] next_engine_id -
 *     next engine id to connect
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_next_engine_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 next_engine_id);

/**
 * \brief
 *      get the next engine id from sw crps database. 
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     core_id and engine_id
 * \param [out] next_engine_id -
 *     next engine id that is connected connect
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_next_engine_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *next_engine_id);

/**
 * \brief
 *      set for crps engine the counter overflow mode
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     core_id and engine_id
 * \param [in] overflow_mode -
 *     the overflow mode to be set
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_overflow_counter_mode_set(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 overflow_mode);

/**
 * \brief
 *      get the counter overflow mode for given crps engine
 * \param [in] unit -
 *     unit id
 * \param [in] engine -
 *     core_id and engine_id
 * \param [out] overflow_mode -
 *     the overflow mode that is set for this engine
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_overflow_counter_mode_get(
    int unit,
    bcm_stat_engine_t * engine,
    uint32 *overflow_mode);

/**
 * \brief
 * set filter criteria group to be filtered in/out
 * \param [in] unit - unit id 
 * \param [in] flags - flags 
 * \param [in] key - core, source, command_id 
 * \param [in] filter - filter group 
 * \param [in] is_active - activate/deactivate the filter group
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_filter_group_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active);

/**
 * \brief
 * determine if certain filter is active
 * \param [in] unit - unit id 
 * \param [in] flags - flags 
 * \param [in] key - core, source, command_id 
 * \param [in] filter - filter group 
 * \param [out] is_active - is the filter active
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_filter_group_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active);

/**
* \brief  
*   API creates a counter database - SW only
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_DATABASE_CREATE_WITH_ID
*   \param [in] core_id    -  specific core id
*   \param [out] database_id    -  database id, if WITH_ID - set the id by user, otherwise set by SDK
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_database_create(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    int *database_id);

/**
* \brief  
*   API destroy a counter database - SW only
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0 or BCM_STAT_DATABASE_CREATE_WITH_ID
*   \param [in] database    -  structure that gather the core_id and database_id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_database_destroy(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database);

/**
* \brief  
*   attach an engine to a database. Will be set as the last engine in the database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] database    -  structure that gather the core_id and database_id
*   \param [in] engine_id   -  engine_id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_engine_attach(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id);

/**
* \brief  
*   detach an engine from a database. It is forbidden to detach the base engine of the database using the API
*   Detach is poosible only for the last engine in database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] core_id    -   core_id
*   \param [in] engine_id   -  engine_id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_engine_detach(
    int unit,
    uint32 flags,
    int core_id,
    int engine_id);

/**
* \brief  
*   control set API per database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] database    -   core_id and database_id
*   \param [in] control   -  control enum
*   \param [in] arg   -  arg

* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_database_control_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 arg);

/**
* \brief  
*   control get API per database
* \par DIRECT INPUT: 
*   \param [in] unit    -  unit ID
*   \param [in] flags   -  0
*   \param [in] database    -   core_id and database_id
*   \param [in] control   -  control enum
*   \param [out] arg   -  arg

* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   *
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_database_control_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 *arg);

/**
* \brief
*   Enable OAM (ILM/ELM/SLM) dummy reply mechanism
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
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
shr_error_e dnx_crps_mgmt_oam_dummy_reply_mechanism_init(
    int unit);

/**
 * \brief
 *      get a list of counters from sw counters database.
 * \param [in] unit -unit id
 * \param [in] core_id -core id 
 * \param [in] engine_id -engine_id
 * \param [in] type_id -type_id 
 * \param [in] object_stat_id -the counter pointer that the source send to CRPS. 
 * \param [in] stat -statistic type and offset that the user wants to read. (for example: offset0&Bytes)
 * \param [out] found -indicates if the counter was found or not.
 * \param [out] counter_id -The counter_id is the HW entry in the engine. 
 *                                          base on all inputs params, we calculate the HW entry that need to read from.
 * \param [out] sub_count -the sw sub counter to read from. 
 *                                      Driver holds for each HW counter entry two counters. This array indicates which sub-counter to read.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   the function doing the following steps:   
 *      1. Logical address computation:
 *             Logical-Object-ID = (Object-ID - cfg-Base-Obj-ID) * cfg-Counter-Set-Size 
 *                + Counter-Set-Offset(5) + Object-Type-Offset + sampling_offset
 *      2. Range selection: check if Logical-Object-ID match to the engine range
 *        (Logical-Object-ID >=  Engine-First-Stat-Ndx ) && (Logical-Stat-ID  <= Engine-Last-Ndx )
 *              first and last is taken from sw state, based on user configuration.
 *              From HW point of view:
 *                  Engine-First-Stat-Ndx = address-base_normalize-cfg. 
 *                  Engine-Last-Ndx = Engine-First-Stat-Ndx + engine_size(4K/8K/16K)*double_entry?2:1 - 1.                
 *      3. calc the Physical address for the  Logical-Object-ID
 *                   Physical_address = (Logical-Object-ID - address-base_normalize-cfg)/double_entry?2:1
 *      4. calc the sub-count, depend if double entry and the stat required.
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_id_calc(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    int object_stat_id,
    int stat,
    int *found,
    int *counter_id,
    int *sub_count);

/**
 * \brief
 *      get a list of counters from sw counters database.
 * \param [in] unit -unit id
 * \param [in] core_id -core id 
 * \param [in] engine_id -engine_id
 * \param [in] type_id -type_id 
 * \param [in] counter_id -the HW counter id 
 * \param [in] ctr_set_offset - set offset in the counter set 
 * \param [out] object_stat_id -calculated stat id according to 
 *        the inpu parameters
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark 
 *  NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_stat_id_find(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    uint32 counter_id,
    int ctr_set_offset,
    int *object_stat_id);

/**
 * \brief
 *      get a list of counters from sw counters database.
 * \param [in] unit -unit id
 * \param [in] core_id -core id 
 * \param [in] engine_id -engine_id
 * \param [in] type_id -type_id 
 * \param [in] object_stat_id -the counter pointer that the source send to CRPS. 
 *  \param [in] ctr_set_offset - counter set offset
 * \param [out] found -indicates if the counter was found or not.
 * \param [out] local_counter -The counter_id - HW entry in
 *                                          the engine. base on
 *                                          all inputs params,
 *                                          we calculate the HW
 *                                          entry that need to
 *                                          read from.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark 
 *  NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_find(
    int unit,
    int core_id,
    int engine_id,
    int type_id,
    int object_stat_id,
    int ctr_set_offset,
    int *found,
    uint32 *local_counter);

/*
 * } 
 */
#endif/*_DNX_CRPS_MGMT_INTERNAL_INCLUDED__*/
