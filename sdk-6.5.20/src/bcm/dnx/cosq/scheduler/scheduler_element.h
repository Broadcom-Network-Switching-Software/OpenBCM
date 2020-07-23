/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_element.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_ELEMENT_H_INCLUDED_
#define _DNX_SCHEDULER_ELEMENT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include "scheduler_dbal.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include "scheduler_convert.h"

#define DNX_SCHEDULER_ELEMENT_IS_HR(unit, se_id) \
    UTILEX_IS_IN_RANGE(se_id, dnx_data_sch.flow.hr_se_id_min_get(unit), dnx_data_sch.flow.hr_se_id_max_get(unit))
#define DNX_SCHEDULER_ELEMENT_IS_CL(unit, se_id) \
    (dnx_data_sch.flow.quartet_offset_to_cl_num_get(unit, DNX_SCH_CONVERT_SE_ID_TO_QUARTET_OFFSET(unit, se_id))->cl_num != -1)

#define DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id) \
    (dnx_data_sch.flow.quartet_offset_to_cl_num_get(unit, DNX_SCH_CONVERT_SE_ID_TO_QUARTET_OFFSET(unit, se_id))->cl_num \
     * DNX_SCH_SE_QUARTER_SIZE(unit) + DNX_SCH_CONVERT_SE_ID_TO_QUARTET_NUMBER(unit, se_id))

typedef struct
{
    const char *description;
    int nof_base_priorities;
    int weighted_or_default_priority;
    int nof_descrete_priority;
    int max_independent_weight;
    int nof_unused_lsb;
} dnx_scheduler_element_cl_type_info_t;

extern const dnx_scheduler_element_cl_type_info_t dnx_sch_cl_info[];

/**
 * \brief -
 * Set Color group for a scheduling element.
 * Sets the color on both flows in case of composite flow.
 * Should be called for base flow only.
 */
shr_error_e dnx_scheduler_element_color_group_set(
    int unit,
    int core_id,
    int flow_id,
    int group);

/**
 * \brief -
 * Get Color group of the scheduling element.
 */
shr_error_e dnx_scheduler_element_color_group_get(
    int unit,
    int core_id,
    int flow_id,
    int *group);

/**
 * \brief -
 * Decide and set Color group of the scheduling element.
 * Sets the color on both flows in case of composite flow.
 * Should be called for base flow only.
 */
shr_error_e dnx_scheduler_element_color_group_decide_and_set(
    int unit,
    int core_id,
    int flow_id,
    int parent_se_id);

/**
 * \brief - 
 * Calculate credit source configuration according to parent configuration 
 * and child attach information
 */
shr_error_e dnx_scheduler_element_credit_src_calc(
    int unit,
    int core_id,
    int parent_se_id,
    int child_mode,
    int child_weight,
    dnx_sch_flow_credit_source_t * credit_source);

/**
 * \brief -
 * Decode cos value into mode and weight for the specified SE
 */
shr_error_e dnx_scheduler_element_cos_decode(
    int unit,
    int core_id,
    int parent_se_id,
    dnx_sch_flow_credit_source_cos_data_t * se_cos,
    int *child_mode,
    int *child_weight);

/**
 * \brief - 
 * Get CL configuration
 */
shr_error_e dnx_scheduler_element_cl_config_get(
    int unit,
    int core_id,
    int cl_id,
    dnx_sch_cl_config_t * cl_config);

/**
 * \brief - 
 * Convert CL WFQ index to index to HW
 */
shr_error_e dnx_scheduler_element_cl_wfq_indx_to_hw(
    int unit,
    dbal_enum_value_field_cl_mode_e cl_mode,
    int wfq_indx,
    int *hw_indx);

#endif /** _DNX_SCHEDULER_ELEMENT_H_INCLUDED_ */
