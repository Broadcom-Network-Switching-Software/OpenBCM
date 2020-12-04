 /** \file crps_ctrs_db.h
 * 
 *  DNX CRPS CTRS DB H FILE. (INTERNAL CRPS FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_CRPS_CTRS_DB_INCLUDED__
/*
 * { 
 */
#define _DNX_CRPS_CTRS_DB_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/stat.h>

 /** 
  *  \brief
  *  when flag is set - change counter value with the given
  *  value, if no flag - handle the counter value according to
  *  the format */
#define DNX_CRPS_CTRS_OVERWRITE_COUNTER_VALUE 0x1
/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
* \brief  
*   init the counters database space. allocate the relevant memory in the CPU
* \par DIRECT INPUT: 
*   \param [in] unit          -  unid id
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
shr_error_e dnx_crps_ctrs_db_init(
    int unit);

/**
* \brief  
*   deinit the counters database space. free the relevant memory in the CPU
* \par DIRECT INPUT: 
*   \param [in] unit          -  unid id
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
shr_error_e dnx_crps_ctrs_db_deinit(
    int unit);

/**
* \brief  
*   update a uint64 counter into the sw counters memory space
* \par DIRECT INPUT: 
*   \param [in] unit                -  unid id
*   \param [in] flags             - flags -
*         if DNX_CRPS_CTRS_OVERWRITE_COUNTER_VALUE - change the
*         counter value with the given value
*         if no flags - set counter value according to format
*   \param [in] core_id          -  core_id
*   \param [in] engine_id       -  engine_id
*   \param [in] counter_id     -  counter_id (HW counter id)
*   \param [in] sub_counter  -  the sub counter that user want to update. 
*                                              for example: in format packets_anf bytes: packets=sub_count0, bytes=sub_count1
*   \param [in] format            -  counter format
*   \param [in] value             -  counter value to update
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - 
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   Input parameter validity was made in previous function, while it was read from HW. see function (dnx_crps_eviction_counter_result_update)
* \see
*   * None
*/
shr_error_e dnx_crps_ctrs_db_counter_set(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int counter_id,
    int sub_counter,
    bcm_stat_counter_format_type_t format,
    uint64 value);

/**
* \brief  
*   get a uint64 counter from the sw counters memory space
* \par DIRECT INPUT: 
*   \param [in] unit                -  unid id
*   \param [in] flags             - flags - get counter value or
*          get value counter and clear counter after read
*   \param [in] core_id          -  core_id
*   \param [in] engine_id       -  engine_id
*   \param [in] counter_id     -  counter_id (HW counter id)
*   \param [in] sub_counter  -  the sub counter that user want to get. 
*                                              for example: in format packets_anf bytes: packets=sub_count0, bytes=sub_count1
*   \param [in] value             -  pointer to counter value
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
shr_error_e dnx_crps_ctrs_db_counter_get(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int counter_id,
    int sub_counter,
    uint64 *value);

/**
 * \brief
 *      set to zero all counters in specific engine
 * \param [in] unit -
 *     unit id
 * \param [in] core_id -
 *     core id 
 * \param [in] engine_id -
 *     engine id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_crps_ctrs_db_counter_clear(
    int unit,
    int core_id,
    int engine_id);

#endif/*_DNX_CRPS_CTRS_DB_INCLUDED__*/
