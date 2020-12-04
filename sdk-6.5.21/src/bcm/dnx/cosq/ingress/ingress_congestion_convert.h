/** \file src/bcm/dnx/cosq/ingress/ingress_congestion_convert.h
 * 
 * 
 * Ingress congestion conversion functions
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SRC_BCM_DNX_INGRESS_CONGESTION_CONVERT_H_INCLUDED_
/** { */
#define _SRC_BCM_DNX_INGRESS_CONGESTION_CONVERT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include "ingress_congestion_dbal.h"
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

/**
 * \brief - convert threshold to internal representation
 */
#define DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, threshold) \
    ((threshold) / (((resource) == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES) ? \
      dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit) : \
      dnx_data_ingr_congestion.info.threshold_granularity_get(unit)))

/**
 * \brief - convert internal representation to threshold
 */
#define DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, internal_threshold) \
    ((internal_threshold) * (((resource) == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES) ? \
      dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit) : \
      dnx_data_ingr_congestion.info.threshold_granularity_get(unit)))

/**
 * \brief - get threshold value as it would be configured to HW
 */
#define DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_EXACT_GET(unit, resource, threshold) \
    DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET((unit), (resource), \
                                                             DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET((unit), (resource), (threshold)))

/**
 * \brief - convert threshold to internal representation - round
 */
#define DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_ROUND_UP_GET(unit, resource, threshold) \
    (UTILEX_DIV_ROUND_UP((threshold), (((resource) == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES) ? \
                                       dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit) : \
                                       dnx_data_ingr_congestion.info.threshold_granularity_get(unit))))

/**
 * \brief - convert DRAM threshold to internal representation
 */
#define DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, threshold) \
    ((threshold) / (((resource == DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES)) ? \
      dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit) : \
      dnx_data_ingr_congestion.info.threshold_granularity_get(unit)))

/**
 * \brief - convert internal representation to DRAM threshold
 */
#define DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource, internal_threshold) \
    ((internal_threshold) * (((resource == DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES)) ? \
      dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit) : \
      dnx_data_ingr_congestion.info.threshold_granularity_get(unit)))

/**
 * \brief -
 * convert BCM WRED parameters to HW WRED data
 */
void dnx_ingress_congestion_convert_wred_to_hw_get(
    int unit,
    dnx_ingress_congestion_wred_info_t * wred,
    dnx_ingress_congestion_wred_hw_params_t * wred_hw_params);

/**
 * \brief -
 * convert HW WRED data to BCM WRED parameters
 */
void dnx_ingress_congestion_convert_hw_to_wred_get(
    int unit,
    dnx_ingress_congestion_wred_hw_params_t * wred_hw_params,
    dnx_ingress_congestion_wred_info_t * wred);

/*
 * brief -
 * convert fadt DRAM threshold values to internal values, according to HW resolution
 */
shr_error_e dnx_ingress_cogestion_convert_fadt_dram_threshold_to_internal_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * dram_thr,
    bcm_cosq_fadt_threshold_t * internal_thr);

/*
 * brief -
 * convert internal values to fadt DRAM threshold values, according to HW resolution
 */
shr_error_e dnx_ingress_cogestion_convert_fadt_internal_to_dram_threshold_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * internal_thr,
    bcm_cosq_fadt_threshold_t * dram_thr);

#endif
