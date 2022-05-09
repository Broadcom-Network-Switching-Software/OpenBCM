/** \file bcm_int/dnx/spb/spb.h
 *
 * sram packet buffer internal.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_SPB_API_INCLUDED__
/*
 * { 
 */
#define _DNX_SPB_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
/*
 * }
 */

/**
 * \brief - Call Back function for mem_init mechanism, will init the context MRU to simulation values
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_context_mru_default_get_f(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - set spb drop threshold value
 *
 * \param [in] unit - unit number
 * \param [in] priority - threshold index (0: High Priority, 1: Mid Priority, 2: Low Priority)
 * \param [in] value - threshold value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_drop_threshold_set(
    int unit,
    int priority,
    uint32 value);

/**
 * \brief - get spb drop threshold value
 *
 * \param [in] unit - unit number
 * \param [in] priority - threshold index (0: High Priority, 1: Mid Priority, 2: Low Priority)
 * \param [out] value - threshold value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_drop_threshold_get(
    int unit,
    int priority,
    uint32 *value);

/**
 * \brief - map port reassembly context to spb drop profile
 *
 * \param [in] unit - unit number
 * \param [in] core - device core
 * \param [in] reassembly_context - port reassembly context
 * \param [in] profile_id - spb drop profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_drop_reassembly_context_profile_map_set(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 profile_id);

/**
 * \brief - get mapped spb drop profile of port reassembly context
 *
 * \param [in] unit - unit number
 * \param [in] core - device core
 * \param [in] reassembly_context - port reassembly context
 * \param [out] profile_id - spb drop profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_drop_reassembly_context_profile_map_get(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 *profile_id);

/**
 * \brief - map priority to [profile,TC,DP]
 *
 * \param [in] unit - unit number
 * \param [in] profile_id - spb drop profile id
 * \param [in] tc - traffic class
 * \param [in] dp - drop precedence
 * \param [in] priority - threshold index (0: High Priority, 1: Mid Priority, 2: Low Priority)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_drop_profile_priority_map_set(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 priority);

/**
 * \brief - get mapped priority from [profile,TC,DP]
 *
 * \param [in] unit - unit number
 * \param [in] profile_id - spb drop profile id
 * \param [in] tc - traffic class
 * \param [in] dp - drop precedence
 * \param [out] priority - threshold index (0: High Priority, 1: Mid Priority, 2: Low Priority)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_drop_profile_priority_map_get(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 *priority);

/**
 * \brief - Initialize dnx spb module
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_init(
    int unit);

/**
 * \brief - De-initialize dnx spb module
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_deinit(
    int unit);

#endif/*_DNX_SPB_API_INCLUDED__*/
