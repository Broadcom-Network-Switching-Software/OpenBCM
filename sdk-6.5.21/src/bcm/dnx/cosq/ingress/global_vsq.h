/** \file src/bcm/dnx/cosq/ingress/global_vsq.h
 * 
 * 
 * Global VSQ functionality
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _GLOBAL_VSQ_H_INCLUDED_
#define _GLOBAL_VSQ_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include "ingress_congestion_dbal.h"

/**
 * \brief -
 * Set global VSQ resource allocation
 */
shr_error_e dnx_global_vsq_resource_alloc_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    int pool_id,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Set global VSQ resource allocation
 */
shr_error_e dnx_global_vsq_resource_alloc_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    int pool_id,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Get global VSQ resource allocation.
 * return values in HW units
 */
shr_error_e dnx_global_vsq_resource_alloc_hw_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    int pool_id,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * clear global VSQ resource allocation
 *
 * VSQ resource allocation registers have reset value of maximum.
 * It does not allow to call resource allocation API, since sum of all parts is above the total.
 * Clear the thresholds here. 
 * Actual resource allocation would be done in Tune module.
 */
shr_error_e dnx_global_vsq_resource_alloc_clear(
    int unit);

#endif
