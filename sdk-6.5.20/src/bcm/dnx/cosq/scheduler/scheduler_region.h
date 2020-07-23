/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_dbal.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_REGION_H_INCLUDED_
#define _DNX_SCHEDULER_REGION_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>

/**
 * \brief - Return region configuration
 * * is the region interdigirated and
 * * is the region odd
 */
shr_error_e dnx_scheduler_region_config_get(
    int unit,
    int core,
    int region_index,
    int *is_interdigitated,
    dnx_scheduler_region_odd_even_mode_t * odd_even_mode);

/**
 * \brief - Has the region SEs
 */
shr_error_e dnx_scheduler_region_has_se(
    int unit,
    int core,
    int region_index,
    int *has_se);

/*
 * \brief - return number of SEs in the quartet of specified region
 */
shr_error_e dnx_scheduler_region_nof_se_in_quartet_get(
    int unit,
    int core,
    int region_index,
    int *nof_se_in_quartet);

/**
 * \brief
 *  Set configuration for 1K flows/aggregates (256
 *  quartets). Flows interdigitated mode configuration must
 *  match the interdigitated mode configurations of the
 *  queues they are mapped to.
 *
 *  \note - the following flow
 *     configuration is not allowed: interdigitated = TRUE,
 *     odd_even = FALSE. The reason for this is that
 *     interdigitated configuration defines flow-queue mapping,
 *     but a flow with odd_even configuration = FALSE cannot be
 *     mapped to a queue.
 *
 *  \note - this configuration is only
 *     relevant to flow_id-s in the range 64K - 96K.
 */
shr_error_e dnx_scheduler_region_info_config(
    int unit);
#endif /** _DNX_SCHEDULER_REGION_H_INCLUDED_ */
