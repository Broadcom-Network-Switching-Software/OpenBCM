 /** \file crps_verify.h
 * 
 *  DNX CRPS VERIFY H FILE. 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_VERIFY_INCLUDED__

/*
 * { 
 */
#define _DNX_CRPS_VERIFY_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
 * Functions prototypes
 * {
 */

 /**
* \brief  
*   verification on API bcm_dnx_stat_counter_expansion_select_set parameters
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
shr_error_e dnx_crps_mgmt_counter_expansion_select_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_expansion_select_get parameters
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
shr_error_e dnx_crps_mgmt_counter_expansion_select_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_interface_key_t * interface,
    bcm_stat_expansion_select_t * expansion_select);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_interface_set parameters
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] database  - defined by database_id and core.
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
shr_error_e dnx_crps_mgmt_counter_interface_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_interface_get parameters
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
shr_error_e dnx_crps_mgmt_counter_interface_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_interface_t * config);

/**
* \brief  
*   verification of dnx_crps_mgmt_range_config used in API
*   bcm_dnx_stat_counter_interface_set
* \par DIRECT INPUT:
*   \param [in] unit    -  Unit-ID
*   \param [in] engine  -  this is the key -defined by engine_id and core.
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
shr_error_e dnx_crps_mgmt_range_verify(
    int unit,
    bcm_stat_engine_t * engine);

/**
* \brief
*   verification on API bcm_stat_counter_set_mapping_set parameters
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] database  -  this is the key -defined by database_id and core.
*   \param [in] counter_set_map  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_set_mapping_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map);

/**
* \brief  
*   verification on API bcm_stat_counter_set_mapping_get parameters
*   2 sections; 1. verify given parameters 2. verify that the
*   configuration is the same as in the other engines from the
*   database
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] database  -  this is the key -defined by database_id and core.
*   \param [out] counter_set_map  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_set_mapping_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_set_map_t * counter_set_map);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_eviction_set parameters
* \par DIRECT INPUT: 
*   \param [in] unit    -  Unit-ID 
*   \param [in] flags   -  
*   \param [in] engine  -  this is the key -defined by engine_id and core.
*   \param [in] eviction  -  configuration structure
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
shr_error_e dnx_crps_mgmt_counter_eviction_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_eviction_get parameters
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
*   * sw state eviction parameters
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_eviction_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_t * eviction);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_database_enable_set parameters
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
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_database_enable_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable);

/**
* \brief  
*   verification on API bcm_dnx_stat_counter_engine_enable_get parameters
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
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_crps_mgmt_counter_database_enable_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_enable_t * enable);

/**
 * \brief
 *      verification on API bcm_stat_counter_explicit_get parameters.
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] stat_counter_input_data -
 *     structure of input parameters which difine which counter to collect
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
shr_error_e dnx_crps_mgmt_counter_explicit_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_explicit_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data);

/**
 * \brief
 *      verification on API bcm_stat_counter_get parameters
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
shr_error_e dnx_crps_mgmt_counter_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_input_data_t * stat_counter_input_data,
    bcm_stat_counter_output_data_t * stat_counter_output_data);

/**
 * \brief
 *      verification for API  bcm_stat_counter_engine_control_set
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] engine -
 *     structure of engine_id and core_id
 * \param [in] control
 *     control enum
 * \param [in] arg
 *     value 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_engine_control_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_engine_control_t control,
    uint32 arg);

/**
 * \brief
 *      verification for API  bcm_stat_counter_database_control_set
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] database -
 *     structure of database_id and core_id
 * \param [in] control
 *     control enum
 * \param [in] arg
 *     value 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_database_control_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 arg);

/**
 * \brief
 *      verification for API  bcm_stat_counter_engine_control_get
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] engine -
 *     structure of engine_id and core_id
 * \param [in] control
 *     control enum
 * \param [out] arg
 *     value 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_engine_control_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_counter_engine_control_t control,
    uint32 *arg);

/**
 * \brief
 *      verification for API  bcm_stat_counter_database_control_get
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     currently not used
 * \param [in] database -
 *     structure of database_id and core_id
 * \param [in] control
 *     control enum
 * \param [out] arg
 *     value 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_mgmt_counter_database_control_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    bcm_stat_counter_database_control_t control,
    uint32 *arg);

/**
 * \brief
 *      verification for API  bcm_stat_eviction_boundaries_set
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     set default or use new
 * \param [in] engine -
 *     structure of engine_id and core_id
 * \param [in] boundaries
 *     structure with start and end boundary
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_boundaries_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief
 *      verification for API bcm_stat_eviction_boundaries_get
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     set default or use new
 * \param [in] engine -
 *     structure of engine_id and core_id
 * \param [out] boundaries
 *     structure with start and end boundary
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_eviction_boundaries_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_engine_t * engine,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_set
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id 
 * \param [in] select_type - header adjust select type 
 * \param [in] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_ingress_pkt_size_adjust_select_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int enable);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_get
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id 
 * \param [in] select_type - header adjust select type 
 * \param [out] enable - header-truncate enabled/disabled
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_ingress_pkt_size_adjust_select_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_pkt_size_adjust_select_type_t select_type,
    int *enable);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_get
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id
 * \param [in] filter - select filter group to be set 
 * \param [in] is_active - activate/deactivate the filter group
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_mgmt_filter_group_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int is_active);

/**
 * \brief -
 * verification for API bcm_stat_pkt_size_adjust_select_get
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] key - core,source,command_id
 * \param [in] filter - select filter group to be set 
 * \param [out] is_active - is the filter group active 
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_mgmt_filter_group_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_command_id_key_t * key,
    bcm_stat_counter_group_filter_t filter,
    int *is_active);

/**
 * \brief -
 * verification for API bcm_stat_counter_database_create
 *   \param [in] unit    -  unit ID
 *   \param [in] flags   -  0 or BCM_STAT_DATABASE_CREATE_WITH_ID
 *   \param [in] core_id    -  specific core id
 *   \param [out] database_id    -  database id, if WITH_ID set by usr, otherwise set by SDK
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_counter_database_create_verify(
    int unit,
    uint32 flags,
    bcm_core_t core_id,
    int *database_id);

/**
 * \brief -
 * verification for API bcm_stat_counter_database_destroy
 *   \param [in] unit    -  unit ID
 *   \param [in] flags   -  0 
 *   \param [in] database    -  specify core id and database_id
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_crps_counter_database_destroy_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database);

/**
* \brief  
*   verification for API bcm_stat_counter_counter_engine_attach
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
shr_error_e dnx_crps_counter_engine_attach_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id);

/**
* \brief  
*   verification for API bcm_stat_counter_counter_engine_detach
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
shr_error_e dnx_crps_counter_engine_detach_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int engine_id);

/**
 * \brief
 *      verification for API  bcm_stat_eviction_boundaries_set
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     set default or use new
 * \param [in] database -
 *     structure of database_id and core_id
 * \param [in] type_id - relevant type id
 * \param [in] boundaries
 *     structure with start and end boundary in obj stat id
 *     resolution
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_database_eviction_boundaries_set_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries);

/**
 * \brief
 *      verification for API bcm_stat_eviction_boundaries_get
 * \param [in] unit -
 *     unit id
 * \param [in] flags -
 *     set default or use new
 * \param [in] database -
 *     structure of database_id and core_id
 * \param [in] type_id - relevant type id
 * \param [out] boundaries
 *     structure with start and end boundary
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_database_eviction_boundaries_get_verify(
    int unit,
    uint32 flags,
    bcm_stat_counter_database_t * database,
    int type_id,
    bcm_stat_eviction_boundaries_t * boundaries);

#endif/*_DNX_CRPS_VERIFY_INCLUDED__*/
