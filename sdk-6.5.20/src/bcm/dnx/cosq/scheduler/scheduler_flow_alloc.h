/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/cosq/scheduler_flow_.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_FLOW_ALLOC__H_INCLUDED_
/*
 * { 
 */
#define _DNX_SCHEDULER_FLOW_ALLOC__H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>

/*
 * Defines:
 * {
 */

#define DNX_SCH_PATTERN_SIMPLE_CON_REG_0_COS(cos_num) ((1 << cos_num) -1)

#define DNX_SCH_PATTERN_SIMPLE_CON_REG_1_COS_REPEATS(cos_num) ((cos_num <= 4)? 2 : 4)

#define DNX_SCH_ELEMENT_INDEX_FROM_FLOW_INDEX_GET(flow_id) (flow_id&dnx_data_sch.flow.flow_in_region_mask_get(unit))
#define DNX_SCH_FLOW_INDEX_FROM_ELEMENT_INDEX_GET(elemnt_id,region) (elemnt_id + (region*dnx_data_sch.flow.region_size_get(unit)))

/*
 * }
 */

/**
 * \brief
 *   Initialize SCH allocation manager.
 *
 * \param [in] unit -
 *   Relevant unit.
 * \retval shr_error_e - 
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_scheduler_flow_alloc_mngr_create(
    int unit);

/**
 * \brief - 
 *  Allocate a flow id for a specific flow type
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core - core id
 * \param [in] nof_remote_cores - number of remote cores
 * \param [in] flags - used Flags : SW_STATE_RES_ALLOC_WITH_ID
 * \param [in] is_non_contiguous - to not allocate from contiguous region
 * \param [in] is_low_rate - should be allocated on region marked as low rate
 * \param [in] num_cos - number of cos (4/8)
 * \param [in] flow_type - the required flow type see dnx_sch_alloc_flow_type_e
 * \param [in] src_modid - the remote device modid, used for connectors
 * \param [out] flow_id - the allocated flow id
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * dnx_sch_alloc_flow_type_e
 */
shr_error_e dnx_scheduler_flow_allocate(
    int unit,
    int core,
    uint32 nof_remote_cores,
    uint32 flags,
    int is_non_contiguous,
    int is_low_rate,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int src_modid,
    int *flow_id);

/**
 * \brief - 
 *  Deallocate a flow id
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core - core id
 * \param [in] num_cos - number of cos (4/8)
 * \param [in] flow_type - the required flow type see dnx_sch_alloc_flow_type_e
 * \param [out] flow_id - the allocated flow id
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_scheduler_flow_deallocate(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int flow_id);

/**
 * \brief - return all allocated flow IDs and the number of allocated flows
 */
shr_error_e dnx_scheduler_flow_alloc_flow_ids_get(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int first_flow_id,
    int flow_id[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS],
    int *nof_flows);

/**
 * \brief - return all real flow IDs and the number of flows
 */
shr_error_e dnx_scheduler_flow_real_flow_ids_get(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int first_flow_id,
    int flow_id[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS],
    int *nof_flows);

/**
 * \brief - return 1 iff the shared shaper tag is clear
 */
shr_error_e dnx_scheduler_flow_alloc_is_shared_shaper_tag_clear(
    int unit,
    int core,
    int flow_id,
    int *is_clear);

/**
 * \brief - return 1 iff there is an allocated flow in the range
 */
shr_error_e dnx_scheduler_flow_alloc_is_allocated_in_range(
    int unit,
    int core,
    int region_start,
    int region_end,
    int *is_allocated);

#endif /** _DNX_SCHEDULER_FLOW_ALLOC__H_INCLUDED_ */
