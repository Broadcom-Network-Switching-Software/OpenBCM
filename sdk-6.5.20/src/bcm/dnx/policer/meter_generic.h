 /** meter_generic.h
 * 
 *  DNX METER GENERIC H FILE. (INTERNAL METER FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_METER_GENERIC_INCLUDED__
/*
 * { 
 */
#define _DNX_METER_GENERIC_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/policer/policer_mgmt.h>

/*
 * MACROs
 * {
 */

#define DNX_METER_PTR_MAP_INDEX_ALL (-1)

/*
 * }
 */

/**
* \brief
*      first initialization of the generic ingress meter database
* \param [in] unit -unit id
* \param [in] database_id -database id
* \param [in] init_stage - indicates if the function is called from init stage or not
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_generic_ingress_db_hw_init(
    int unit,
    int database_id,
    int init_stage);

/**
* \brief
*      first initialization of the generic egress meter database
* \param [in] unit -unit id
* \param [in] database_id -database id
* \param [in] init_stage - indicates if the function is called from init stage or not
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_generic_egress_db_hw_init(
    int unit,
    int database_id,
    int init_stage);

/**
 * \brief
 *      first initialization of the generic ingress and egress meter
 * \param [in] unit -unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_init(
    int unit);

/**
 * \brief
 *      configure a generic meter profile
 * \param [in] unit -unit id
 * \param [in] core_id -core id
 * \param [in] is_ingress -indicates ingress or egress
 * \param [in] database_id -database id
 * \param [in] profile_idx -profile  index
 * \param [in] profile_info -profile configuration
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_profile_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int profile_idx,
    dnx_meter_profile_info_t * profile_info);

/**
 * \brief
 *      get generic meter profile configuration
 * \param [in] unit -unit id
 * \param [in] core_id -core id
 * \param [in] is_ingress -indicates ingress or egress
 * \param [in] database_id -database id
 * \param [in] profile_idx -profile  index
 * \param [out] profile_info -profile configuration
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_profile_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int profile_idx,
    dnx_meter_profile_info_t * profile_info);

/**
* \brief
*      set hw configuration for the profile id for given meter index
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] meter_idx -meter  index
* \param [in] profile_idx -profile  index
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_generic_profile_id_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int profile_idx);

/**
* \brief
*     get hw profile id for given meter index
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] meter_idx -meter  index
* \param [out] profile_idx -profile  index
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_generic_profile_id_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int *profile_idx);

/**
* \brief
*      HW set for each 4 consecutive meters, the global sharing flag. (relevant for MEF10.2/10.3 mode)
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] meter_idx -meter  index
* \param [in] global_sharing -is global_sharing
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_generic_global_sharing_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int global_sharing);

/**
* \brief
*      get from HW the global sharing flag. (relevant for MEF10.2/10.3 mode). 
* \param [in] unit -unit id
* \param [in] core_id -core id
* \param [in] is_ingress -indicates ingress or egress
* \param [in] database_id -database id
* \param [in] meter_idx -meter  index
* \param [out] global_sharing -is global_sharing
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_generic_global_sharing_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int *global_sharing);

/**
* \brief
*      set the HW pointer mapping table for meter. indirection between the meter pointer Msb bits and the bank_id.
*   
*    for Jr2, pointer  val: bit19      18      17       16        15          14      13      12
*                                       0           0  bank-id bank-id bank-id bank-id   0        0
* \param [in] unit -unit id
* \param [in] core_id -
* \param [in] is_ingress -ingress or egress meter database
* \param [in] database_id -
* \param [in] ptr_base - base pointer. will be the input to calculate the indexes of the table.
* \param [in] bank_id - the bank id to update in the mapping table
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
 */
shr_error_e dnx_meter_generic_ptr_map_hw_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int ptr_base,
    uint32 bank_id);

/**
 * \brief
 *      get the from hw the data that is used for given meter pointer (after expansion)
 * \param [in] unit -unit id
 * \param [in] core_id -core_id
 * \param [in] is_ingress -is_ingress
 * \param [in] database_id -unit id
 * \param [in] index -index to hw table
 * \param [out] data -hw entry data
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_ptr_map_hw_data_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int index,
    uint32 *data);
/**
 * \brief
 *      get the from hw the bank id that is used for given meter pointer (after expansion)
 * \param [in] unit -unit id
 * \param [in] core_id -core_id
 * \param [in] is_ingress -is_ingress
 * \param [in] database_id -unit id
 * \param [in] index -index to hw table
 * \param [out] bank_id -hw bank id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_ptr_map_hw_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int index,
    uint32 *bank_id);

/**
 * \brief
 *      set in HW the address range for a meter database.
 *      address range determine how many banks to allocate
 * \param [in] unit -unit id
 * \param [in] database_core -core id (support ALL)
 * \param [in] is_ingress -ingress or egress meter database
 * \param [in] database_id -
 * \param [in] nof_meters - how many meters. this is the address range.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_address_range_hw_set(
    int unit,
    int database_core,
    int is_ingress,
    int database_id,
    int nof_meters);

/**
 * \brief
 *      set in HW the address banks mapping. In case of small engine configuration, no need to enable the mapping.
 *      it is like link list of banks that concatanate them. If dual bucket mode, HW expect to have only the low bank id in the list.
 * \param [in] unit -unit id
 * \param [in] core_id  -core id 
 * \param [in] is_ingress -ingress or egress meter database
 * \param [in] database_id -
 * \param [in] map_index - index to the mapping table
 * \param [in] bank_id - the bank id to map
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_address_map_hw_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int map_index,
    int bank_id);

/**
 * \brief
 *      clear all meters profiles pointers and global sharing flags and set all meters to use the default profile. 
 * \param [in] unit -unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_clear_all(
    int unit);

/**
 * \brief
 *      clear the entire memory space for a single engine (if section=ALl, it might be 2 banks)
 *      Note: currently, the dbal don;t give us a way to clear entire arrayEntry, 
 *                so we use the range option to clear the entire meter_idx key for the relevant tables 
 * \param [in] unit -unit id
 * \param [in] core_id -core_id
 * \param [in] engine -engine
 * \param [in] is_ingress -is_ingress
 * \param [in] database_id -database_id
 * \param [in] single_bucket_bank -small engines use special banks (single bucket only, DB_A in HW JR2) 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_generic_engine_clear(
    int unit,
    int core_id,
    bcm_policer_engine_t * engine,
    int is_ingress,
    int database_id,
    int single_bucket_bank);

/**
* \brief
*   enable/disable fairness mechasinm for ingress and egress meter databases
*   The fairness mechanism is adding a random number for the bucket level in order to prevent adverse synchronization.
*   between packet flows and provide fairness between multiple flows using the same meter.
* \param [in] unit        - unit id
* \param [in] core_id     - core id
* \param [in] is_ingress - True for ingress, false for egress
* \param [in] database_id - database id
* \param [in] enable   - if true enable fairmness mechanism. false for disable it.
* \return
*   shr_error_e - Error Type
* \remark
*  by default mechanism is disabled
* \see
*   * None
*/
shr_error_e dnx_meter_generic_fairness_set(
    int unit,
    bcm_core_t core_id,
    int is_ingress,
    int database_id,
    int enable);

/**
* \brief
*   get if ingress meter fairness mechainsm is enabled or disabled. get it from CBL.
*   it should be the same for EBL and also for all other meters. (egress/global)
* \param [in] unit        - unit id
* \param [in] core_id     - core id
* \param [in] is_ingress     - true for ingress. false for egress
* \param [in] database_id - database id
* \param [out] enabled   - if true enabled.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_meter_generic_fairness_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    uint32 *enabled);

/**
* \brief
*   function checks if the meter_idx belongs to the database.
*   it search in the memory bands, a bank that its range match to the meter index.
* \param [in] unit        - unit id
* \param [in] core_id     - core id
* \param [in] is_ingress     - ingress/egress meter
* \param [in] database_id - database id
* \param [in] meter_idx - meter id
* \param [out] valid   - valid=true if meter_idx belong to the database
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_meter_generic_index_verify(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    int *valid);

#endif/*_DNX_METER_GENERIC_INCLUDED__*/
