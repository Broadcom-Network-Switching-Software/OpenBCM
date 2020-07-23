/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler.h
 * Reserved.$ 
 */

#ifndef _LOCAL_DNX_SCHEDULER_H_INCLUDED_
#define _LOCAL_DNX_SCHEDULER_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include "scheduler_element.h"

#define DNX_SCH_MAX_FLOW_ID(unit) (dnx_data_sch.flow.nof_flows_get(unit) - 1)

/**
 * \brief -
 * Calculate number of remote cores and 
 * actual flow id for each of the remote core
 */
shr_error_e dnx_scheduler_flow_id_for_remote_core_get(
    int unit,
    int core,
    int base_flow_id,
    int actual_flow_id[],
    int *nof_remote_cores);

/**
 * \brief - verify base flow id is allocated 
 * and cosq is consistent with allocation_bundle size
 */
shr_error_e dnx_scheduler_base_flow_id_cosq_verify(
    int unit,
    int core,
    int base_flow_id,
    uint32 cosq,
    int is_connector);

/**
 * \brief - obtain base flow id and core from gport and cosq
 */
shr_error_e dnx_scheduler_gport_core_base_flow_id_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *core,
    int *base_flow_id);

/**
 * \brief - attach flow to a parent
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] sched_port - parent flow gport
 *   \param [in] input_port - child flow gport
 *   \param [in] cosq - child flow cosq
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * The mode and weight of connection to the parent are determined by bcm_cosq_gport_sched_set
 * \see
 *   * None
 */
shr_error_e dnx_scheduler_gport_attach(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq);

/**
 * \brief - detach flow from a parent
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] sched_port - parent flow gport
 *   \param [in] input_port - child flow gport
 *   \param [in] cosq - child flow cosq
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * 
 * \see
 *   * None
 */
shr_error_e dnx_scheduler_gport_detach(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq);

/**
 * \brief - get a parent of the flow
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] child_port - child flow gport
 *   \param [in] cos - child flow cosq
 *   \param [out] parent_port - parent flow gport
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * 
 * \see
 *   * None
 */
shr_error_e dnx_scheduler_gport_parent_get(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cos,
    bcm_gport_t * parent_port);

/**
 * \brief - configure rate of slow profile
 */
shr_error_e dnx_scheduler_slow_profile_rate_set(
    int unit,
    int core,
    int slow_rate_profile,
    int level,
    int slow_rate);

/**
 * \brief - get rate of slow profile
 */
shr_error_e dnx_scheduler_slow_profile_rate_get(
    int unit,
    int core,
    int slow_rate_profile,
    int level,
    int *slow_rate);

/**
 * \brief - return type of scheduling element
 */
shr_error_e dnx_scheduler_gport_element_type_get(
    int unit,
    bcm_cosq_scheduler_gport_type_t type,
    dnx_sch_element_se_type_e * se_type);

/**
 * \brief - allocate and configure scheduling element
 */
shr_error_e dnx_scheduler_element_gport_add(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport);

/**
 * \brief - update configuration of existing scheduling element
 */
shr_error_e dnx_scheduler_element_gport_replace(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport);

/**
 * \brief - get scheduling element gport parameters
 */
int dnx_scheduler_gport_get(
    int unit,
    int core,
    int flow_id,
    bcm_cosq_scheduler_gport_params_t * params);

/**
 * \brief - attach port to LAG scheduler
 */
shr_error_e dnx_scheduler_attach_port_to_lag_scheduler(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq);

/**
 * \brief - detach port from LAG scheduler
 */
shr_error_e dnx_scheduler_detach_port_from_lag_scheduler(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq);

/**
 * \brief - get scheduler port for a given port
 */
shr_error_e dnx_scheduler_port_lag_scheduler_get(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cos,
    bcm_gport_t * parent_port);

#endif /** _LOCAL_DNX_SCHEDULER_H_INCLUDED_ */
