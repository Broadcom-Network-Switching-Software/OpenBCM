/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_flow.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_FLOW_H_INCLUDED_
#define _DNX_SCHEDULER_FLOW_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - Configure flow attributes to their default (reset) values
 */
shr_error_e dnx_scheduler_flow_default_attributes_set(
    int unit,
    int core,
    int flow_id);

shr_error_e dnx_sch_flow_composite_id_get(
    int unit,
    int core,
    int base_flow_id,
    int *composite_flow_id);

/**
 * \brief Configure is_composite property of flow
 */
shr_error_e dnx_sch_flow_is_composite_set(
    int unit,
    int core_id,
    int base_flow_id,
    int is_composite);

/**
 * \brief Configure is_composite property of flow
 * \note - this function should always be called for composite bundle only
 * either by creating composite bundle (is_composite = 1) or
 * by deleting composite bundle (is_compisite = 0)
 */
shr_error_e dnx_sch_flow_bundle_is_composite_set(
    int unit,
    int core_id,
    int base_flow_id,
    int nof_flows,
    int is_composite);
/**
 * \brief returns true iff flow is port HR
 */
shr_error_e dnx_sch_flow_is_port_hr_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_port_hr);

/**
 * \brief configure credit source for a given flow
 */
shr_error_e dnx_sch_flow_credit_src_set(
    int unit,
    int core,
    int child_flow_id,
    int parent_se_id,
    int mode,
    int weight);

/**
 * \brief -
 * get rate factor for the flow
 */
shr_error_e dnx_sch_flow_low_rate_factor_get(
    int unit,
    bcm_core_t core,
    int flow_id,
    int *factor);

#endif /** _DNX_SCHEDULER_FLOW_H_INCLUDED_ */
