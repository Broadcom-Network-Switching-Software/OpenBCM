/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * system_red_dbal.h
 *
 *  Created on: Sep 12, 2018
 *      Author: si888124
 */

#ifndef _SYSTEM_RED_DBAL_H_INCLUDED_
#define _SYSTEM_RED_DBAL_H_INCLUDED_

#include <bcm_int/dnx/cosq/ingress/system_red.h>

#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

typedef struct
{
    uint32 enable;
    uint32 sch_aging_period;
    dbal_enum_value_field_system_red_aging_mode_e aging_mode;
    uint32 is_only_aging_dec;
} dnx_system_red_dbal_aging_t;

typedef struct
{
    uint32 enable;
    uint32 admit_thr;
    uint32 prob_thr;
    uint32 reject_thr;
    uint32 drop_prob_low;
    uint32 drop_prob_high;
} dnx_system_red_dbal_discard_params_t;

/**
 * \brief - Set system RED free resource threshold
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource to set the threshold for (PDBs, SRAM buffers, DRAM BDBs)
 * \param [in] thr_index - (0-2) there are three thresholds for each resource, to define 4 ranges of free resources. each range is mapped to a RED-Q-Size value.
 * \param [in] thr_value - threshold value.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_free_res_thr_set(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 thr_index,
    int thr_value);

/**
 * \brief - Get system RED free resource threshold
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource to set the threshold for (PDBs, SRAM buffers, DRAM BDBs)
 * \param [in] thr_index - (0-2) there are three thresholds for each resource, to define 4 ranges of free resources. each range is mapped to a RED-Q-Size value.
 * \param [out] thr_value - threshold value.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_free_res_thr_get(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 thr_index,
    int *thr_value);

/**
 * \brief - Map system RED index to free resource range
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource to map the RED-Q-size for (PDBs, SRAM buffers, DRAM BDBs)
 * \param [in] range_index - (0-3) there are three thresholds for each resource, to define 4 ranges of free resources. each range is mapped to a RED-Q-Size value.
 * \param [in] red_q_size_index - RED-Q-Size index to map to the specific range (0-15).
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_free_res_range_to_red_q_size_map_set(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 range_index,
    uint32 red_q_size_index);

/**
 * \brief - Get mapping of system RED index to free resource range
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource to map the RED-Q-size for (PDBs, SRAM buffers, DRAM BDBs)
 * \param [in] range_index - (0-3) there are three thresholds for each resource, to define 4 ranges of free resources. each range is mapped to a RED-Q-Size value.
 * \param [out] red_q_size_index - RED-Q-Size index mapped to the specified range (0-15).
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_free_res_range_to_red_q_size_map_get(
    int unit,
    dnx_system_red_resource_type_e resource,
    uint32 range_index,
    uint32 *red_q_size_index);

/**
 * \brief - Map system RED index to VOQ size range
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class id
 * \param [in] red_q_size_index - RED-Q-Size index to map to the specific range (0-14).
 * \param [in] voq_range_max_size - max range for the VOQ size mapped to the specified red_q_size_index. min range is calculated as: voq_range_max_size[red_q_size_index-1]+1
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_voq_size_range_to_red_q_size_map_set(
    int unit,
    uint32 rate_class,
    uint32 red_q_size_index,
    uint32 voq_range_max_size);

/**
 * \brief - Get mapping of system RED index to VOQ size range
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class id
 * \param [in] red_q_size_index - RED-Q-Size index to map to the specific range (0-14).
 * \param [out] voq_range_max_size - max range for the VOQ size mapped to the specified red_q_size_index.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_voq_size_range_to_red_q_size_map_get(
    int unit,
    uint32 rate_class,
    uint32 red_q_size_index,
    uint32 *voq_range_max_size);

/**
 * \brief - Set system RED discard params
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class id
 * \param [in] dp - DP (0-3)
 * \param [in] discard_params - specific discrad params:
 *      admit_thr - (0-15). RED-Q-Size < admit_thr is admitted
 *      prob_thr - RED-Q-Size > admit_thr and < prob_thr is dropped in probability drop_prob_low
 *      drop_prob_low - low drop probability
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_discard_prob_low_set(
    int unit,
    uint32 rate_class,
    uint32 dp,
    dnx_system_red_dbal_discard_params_t * discard_params);

/**
 * \brief - Set system RED discard params
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class id
 * \param [in] dp - DP (0-3)
 * \param [in] discard_params - specific discrad params:
 *      prob_thr - RED-Q-Size > prob_thr and < reject_thr is dropped in probability drop_prob_high.
 *      reject_thr - RED-Q-Size > Reject_thr is alwats dropped.
 *      drop_prob_high - high drop probability
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_discard_prob_high_set(
    int unit,
    uint32 rate_class,
    uint32 dp,
    dnx_system_red_dbal_discard_params_t * discard_params);

/**
 * \brief - Get system RED discard params
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class id
 * \param [in] dp - DP (0-3)
 * \param [out] discard_params - specific discrad params. see dnx_system_red_dbal_discard_prob_low/high_set
 */
shr_error_e dnx_system_red_dbal_discard_params_get(
    int unit,
    uint32 rate_class,
    uint32 dp,
    dnx_system_red_dbal_discard_params_t * discard_params);

/**
 * \brief - Set system RED aging params in SCH
 *
 * \param [in] unit - The unit number.
 * \param [in] aging_params - aging period params:
 *      enable - enable aging (means enabling system RED in scheduler)
 *      sch_aging_period - aging period in scheduler
 *      age_mode - action to be done when aging is expired - reset or decrement
 *      is_only_aging_dec - if set, it means onlt aging mechanism can decrement RED-Q-Size index. otherwise, also other mechanisms can.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_sch_aging_params_set(
    int unit,
    dnx_system_red_dbal_aging_t * aging_params);

/**
 * \brief - Get system RED aging params in SCH
 *
 * \param [in] unit - The unit number.
 * \param [out] aging_params - aging period params: see dnx_system_red_dbal_aging_params_set
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_sch_aging_params_get(
    int unit,
    dnx_system_red_dbal_aging_t * aging_params);

/**
 * \brief - Set system RED aging params in Ingress side
 *
 * \param [in] unit - The unit number.
 * \param [in] aging_period - aging period
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_ing_params_set(
    int unit,
    uint32 aging_period);

/**
 * \brief - Get system RED aging params in Ingress side
 *
 * \param [in] unit - The unit number.
 * \param [out] aging_period - aging period
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_ing_params_get(
    int unit,
    uint32 *aging_period);

/**
 * \brief - Enable system RED in Ingress side
 *
 * \param [in] unit - The unit number.
 * \param [in] enable - enable
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_enable_set(
    int unit,
    uint32 enable);

/**
 * \brief - Get system RED enable status in Ingress side
 *
 * \param [in] unit - The unit number.
 * \param [out] enable - enabled indication
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_enable_get(
    int unit,
    uint32 *enable);

/**
 * \brief - Get system RED global status (max and current)
 *
 * \param [in] unit - The unit number.
 * \param [in] core - core id.
 * \param [out] current_red_index - current RED index on the device
 * \param [out] max_red_index - max_red_index seen since last read
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_global_red_status_get(
    int unit,
    int core,
    uint32 *current_red_index,
    uint32 *max_red_index);

/**
 * \brief - Get system RED status per system port
 *
 * \param [in] unit - The unit number.
 * \param [in] core - core id.
 * \param [in] system_port - system port
 * \param [out] red_index - max_red_index for the system port
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_system_red_dbal_sysport_red_status_get(
    int unit,
    int core,
    uint32 system_port,
    uint32 *red_index);

#endif /* _SYSTEM_RED_DBAL_H_INCLUDED_ */
