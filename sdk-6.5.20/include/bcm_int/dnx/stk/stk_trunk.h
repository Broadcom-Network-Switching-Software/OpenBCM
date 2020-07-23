/** \file stk/stk_trunk.h
 *
 * Stacking trunk procedures.
 *
 *  stacking trunk , stacking fec  ...
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DNX_STK_TRUNK_H_INCLUDED
/*
 * {
 */
#define DNX_STK_TRUNK_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/stack.h>
#include <bcm/trunk.h>

#include <shared/shrextend/shrextend_debug.h>

#define DNX_TRUNK_STACKING_PEER_TMD_IS_VALID(unit, peer_tm_domain) \
    ((peer_tm_domain < 0) || (peer_tm_domain >= dnx_data_stack.general.nof_tm_domains_max_get(unit)))

/**
 * \brief 
 * Get stk trunks given a tm domain id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * \param [in] stk_trunk_max - max amount of stack trunks in supplied array
 * \param [out] stk_trunk_array - returned found stack trunks in supplied array
 * \param [out] stk_trunk_count - the found amount of stack trunks 
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_domain_get(
    int unit,
    int tm_domain,
    int stk_trunk_max,
    bcm_trunk_t * stk_trunk_array,
    int *stk_trunk_count);

/**
 * \brief 
 * Update trunk domain fec map.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_domain_fec_map_update(
    int unit,
    int tm_domain);

/**
 * \brief 
 * Set stack lag fec mapping.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * \param [in] entry - entry id for  Stack fec resolve table 
 * \param [in] stack_lag - stack lag connect to peer domain
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_fec_map_stack_lag_set(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 stack_lag);

/**
 * \brief 
 * Get stack lag fec mapping.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * \param [in] entry - entry id for  Stack fec resolve table 
 * \param [out] stack_lag - stack lag connect to peer domain
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_fec_map_stack_lag_get(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 *stack_lag);

/**
 * \brief 
 * Set stack fec resolve table.
 * 
 * \param [in] unit - Unit # 
 * \param [in] flow_id - flow id
 * \param [in] lag_lb_key - lag load balance key
 * \param [in] stack_lag - stack lag connect to peer domain
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stack_fec_resolve_set(
    int unit,
    uint32 flow_id,
    uint32 lag_lb_key,
    uint32 stack_lag);

/**
 * \brief 
 * Get stack fec resolve table.
 * 
 * \param [in] unit - Unit # 
 * \param [in] flow_id - flow id
 * \param [in] lag_lb_key - lag load balance key
 * \param [out] stack_lag - stack lag connect to peer domain
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stack_fec_resolve_get(
    int unit,
    uint32 flow_id,
    uint32 lag_lb_key,
    uint32 *stack_lag);

/**
 * \brief 
 * Set stack lag pacekts base queue id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * \param [in] entry -entry id for  Stack trunk resolve table 
 * \param [in] base_queue - base queue for destination system port
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stack_lag_packets_base_queue_id_set(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 base_queue);

/**
 * \brief 
 * Get stack lag pacekts base queue id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * \param [in] entry -entry id for  Stack trunk resolve table 
 * \param [out] base_queue - base queue for destination system port
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stack_lag_packets_base_queue_id_get(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 *base_queue);

/**
 * \brief 
 * Set stack trunk resolve table.
 * 
 * \param [in] unit - Unit # 
 * \param [in] stack_lag - stack lag connect to peer domain
 * \param [in] lag_lb_key -lag load balance key
 * \param [in] base_queue - base queue for destination system port
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stack_trunk_resolve_set(
    int unit,
    uint32 stack_lag,
    uint32 lag_lb_key,
    uint32 base_queue);

/**
 * \brief 
 * Get stack trunk resolve table.
 * 
 * \param [in] unit - Unit # 
 * \param [in] stack_lag - stack lag connect to peer domain
 * \param [in] lag_lb_key -lag load balance key
 * \param [out] base_queue - base queue for destination system port
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stack_trunk_resolve_get(
    int unit,
    uint32 lag_lb_key,
    uint32 stack_lag,
    uint32 *base_queue);

/**
 * \brief - Verify stack trunk id.
 */
int dnx_stk_trunk_trunk_id_verify(
    int unit,
    bcm_trunk_t stk_trunk);

/**
 * \brief - set stacking trunk with members.
 *
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member_count - number of members in array
 * \param [in] member_array - array of members to put in the
 *        trunk
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stacking_set(
    int unit,
    bcm_trunk_t trunk_id,
    int member_count,
    bcm_trunk_member_t * member_array);

/**
 * \brief - get trunk info and members
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member_max - max members to get
 * \param [in] member_array - array for retrived members
 * \param [in] member_count - nof retrieved members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stacking_get(
    int unit,
    bcm_trunk_t trunk_id,
    int member_max,
    bcm_trunk_member_t * member_array,
    int *member_count);

/**
 * \brief - add member to stacking trunk. added member can have flags as
 *        mentioned in bcm_dnx_trunk_set API.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - member info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stacking_member_add(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member);

/**
 * \brief - delete a member from trunk.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - member info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_trunk_stacking_member_delete(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member);
#endif /* DNX_STK_TRUNK_H_INCLUDED */
