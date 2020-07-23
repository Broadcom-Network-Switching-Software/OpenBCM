 /** meter_generic.h
 * 
 *  DNX METER GLOBAL H FILE. (INTERNAL METER FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_METER_GLOBAL_INCLUDED__
/*
 * { 
 */
#define _DNX_METER_GLOBAL_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/policer/policer_mgmt.h>

/*
 * MACROs
 * {
 */

/*
 * }
 */

 /**
  * \brief
  *      first initialization of the global meter
  * \param [in] unit -unit id
  * \param [out] profile_info -default profile configuration
  * \return
  *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
  *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
  * \remark
  *   NONE
  * \see
  *   NONE
  */
shr_error_e dnx_meter_global_init(
    int unit,
    dnx_meter_profile_info_t * profile_info);

/**
 * \brief
 *      configure one global meter profile
 * \param [in] unit -unit id
 * \param [in] core_id -code id 
 * \param [in] glbl_idx -the index of the global meter. (0-4. one per Fwd-Type)
 * \param [in] profile_info -profile configuration
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_global_profile_set(
    int unit,
    int core_id,
    int glbl_idx,
    dnx_meter_profile_info_t * profile_info);

/**
 * \brief
 *      get a global profile configuration from HW
 * \param [in] unit -unit id
 * \param [in] core_id -code id 
 * \param [in] glbl_idx -the index of the global meter. (0-4. one per Fwd-Type)
 * \param [out] profile_info -profile configuration
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_global_profile_get(
    int unit,
    int core_id,
    int glbl_idx,
    dnx_meter_profile_info_t * profile_info);

/**
* \brief
*   set global meter counters mode - packets or bytes
* \param [in] unit      - unit id
* \param [in] policer_database_handle - database handle
* \param [in] is_bytes - is bytes or packets
* \return
*   shr_error_e - Error Type
* \remark 
*   * None 
* \see
*   * None
*/
shr_error_e dnx_meter_global_ctr_mode_set(
    int unit,
    int policer_database_handle,
    uint32 is_bytes);

/**
* \brief
*   get global meter counters mode - packets or bytes
* \param [in] unit      - unit id
* \param [in] policer_database_handle - database handle
* \param [out] is_bytes - is bytes or packets
* \return
*   shr_error_e - Error Type
* \remark 
*   * None 
* \see
*   * None
*/
shr_error_e dnx_meter_global_ctr_mode_get(
    int unit,
    int policer_database_handle,
    uint32 *is_bytes);

/**
* \brief
*   get global meter counters for specified statistic
* \param [in] unit        - unit id
* \param [in] policer_id   - database handle
* \param [in] stat - select statistic
* \param [out] value - counter value
* \return
*   shr_error_e - Error Type
* \remark 
*   * None 
* \see
*   * None
*/
shr_error_e dnx_meter_global_stat_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_stat_t stat,
    uint64 *value);

/**
* \brief
*   enable/disable fairness mechasinm for global meter
*   The fairness mechanism is adding a random number for the bucket level in order to prevent adverse synchronization.
*   between packet flows and provide fairness between multiple flows using the same meter.
* \param [in] unit        - unit id
* \param [in] core_id        - core id
* \param [in] enable   - if true enable fairmness mechanism. false for disable it.
* \return
*   shr_error_e - Error Type
* \remark
*  by default mechanism is disabled
* \see
*   * None
*/
shr_error_e dnx_meter_global_fairness_set(
    int unit,
    bcm_core_t core_id,
    int enable);

/**
* \brief
*   get if fairness mechasinm for global meter enable/disable
* \param [in] unit        - unit id
* \param [in] core_id        - core id
* \param [in] enable   - if true fairmness mechanism is enabled.
* \return
*   shr_error_e - Error Type
* \remark
*  None
* \see
*   * None
*/
shr_error_e dnx_meter_global_fairness_get(
    int unit,
    bcm_core_t core_id,
    uint32 *enable);

#endif/*_DNX_METER_GLOBAL_INCLUDED__*/
