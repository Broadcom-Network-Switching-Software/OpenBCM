/** \file src/bcm/dnx/cosq/ingress/vsq_rate_class.h
 * 
 * 
 * VSQ rate class functionality
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _VSQ_RATE_CLASS_H_INCLUDED_
#define _VSQ_RATE_CLASS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include "ingress_congestion_dbal.h"

/**
 * \brief - 
 * get default rate class data for provided VSQ type
 */
int dnx_vsq_rate_class_default_data_get(
    int unit,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    dnx_ingress_congestion_vsq_rate_class_info_t * vsq_rate_class_info);

/**
 * \brief -
 * Get a pointer to the VSQ rate class wred data
 */
dnx_ingress_congestion_vsq_rate_class_wred_info_t *dnx_vsq_rate_class_wred_ptr_get(
    int unit,
    const dnx_ingress_congestion_vsq_group_e vsq_group,
    const int pool_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls);

/**
 * \brief -
 * Get a pointer to the VSQ rate class exp weight wred data
 */
uint32 *dnx_vsq_rate_class_exp_weight_wred_ptr_get(
    int unit,
    const dnx_ingress_congestion_vsq_group_e vsq_group,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls);

/**
 * \brief -
 * Set vsq to rate class mapping
 */
int dnx_vsq_rate_class_mapping_set(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int vsq_id,
    int rt_cls);

/**
 * \brief -
 * VSQ rate class exchange set commit includes allocation management and HW.
 */
int dnx_vsq_rate_class_exchange_and_set(
    int unit,
    int core_id,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int vsq_index,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_class);

/**
 * \brief -
 * Commit in HW VSQ rate class information 
 */
int dnx_vsq_rate_class_hw_set(
    int unit,
    int core_id,
    int rate_class,
    dnx_ingress_congestion_vsq_group_e vsq_category_index,
    dnx_ingress_congestion_vsq_rate_class_info_t * rate_class_info,
    dnx_ingress_congestion_vsq_rate_class_info_t * exact_rate_class_info);

/**
 * \brief -
 * unset rate class of the VSQ by setting it to default rate class
 */
int dnx_vsq_rate_class_destroy(
    int unit,
    int core_id,
    int vsq_index,
    dnx_ingress_congestion_vsq_group_e vsq_type);

/**
 * \brief - 
 * retrieve VSQ WRED 
 */
int dnx_vsq_rate_class_wred_get(
    int unit,
    bcm_core_t core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int rate_class,
    int pool,
    int dp,
    dnx_ingress_congestion_wred_info_t * wred_data,
    uint32 *gain);

/**
 * \brief -
 * Configure VSQ rate class tail drop threshold
 */
int dnx_vsq_rate_class_tail_drop_set(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    dnx_ingress_congestion_resource_type_e resource,
    int rt_cls,
    int dp,
    uint32 max_size);

/**
 * \brief -
 * Obtain VSQ rate class tail drop threshold
 */
int dnx_vsq_rate_class_tail_drop_get(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    dnx_ingress_congestion_resource_type_e resource,
    int rt_cls,
    int dp,
    uint32 *max_size);

/**
 * \brief -
 * Sets hysteresis FC parameters
 */
int dnx_vsq_rate_class_hyst_fc_set(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int pool,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_hyst_threshold_t * fc);

/**
 * \brief -
 * Gets hysteresis FC parameters
 */
int dnx_vsq_rate_class_hyst_fc_get(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int pool,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_hyst_threshold_t * fc);

/**
 * \brief -
 * Sets FADT FC parameters (for VSQ-F)
 */
int dnx_vsq_rate_class_fadt_fc_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc);

/**
 * \brief -
 * Gets FADT FC parameters (for VSQ-F)
 */
int dnx_vsq_rate_class_fadt_fc_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc);

/**
 * \brief -
 * Get VSQ-E resource allocation
 */
int dnx_vsq_e_rate_class_resource_allocation_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params);

/**
 * \brief -
 * Get VSQ-F resource allocation
 */
int dnx_vsq_f_rate_class_resource_allocation_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params);

#endif
