/** \file src/bcm/dnx/cosq/ingress/voq_rate_class.h
 * 
 * 
 * VOQ rate class functionality
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _VOQ_RATE_CLASS_H_INCLUDED_
#define _VOQ_RATE_CLASS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief -
 * Sets FADT tail drop parameters to the HW - per rate-class and drop precedence.
 * This function is not mutex protected, and doesn't update SW state.
 */

shr_error_e dnx_voq_rate_class_fadt_tail_drop_hw_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rate_class,
    uint32 drop_precedence,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief -
 * Sets FADT drop parameters - per rate-class and drop precedence.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 * This function is mutex protected and updates SW-State
 */
int dnx_voq_rate_class_fadt_tail_drop_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 dp,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief -
 * Gets FADT drop parameters - per rate-class and drop precedence.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
int dnx_voq_rate_class_fadt_tail_drop_sw_get(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 dp,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief -
 * Set the size of committed queue size (i.e., the
 * guaranteed memory) for each VOQ, even in the case that a
 * set of queues consume most of the memory resources.
 *
 * Note: threshold value is in HW granularity.
 */
int dnx_voq_rate_class_guaranteed_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 guaranteed_thresh);

/**
 * \brief -
 * Set ECN static drop threshold
 */
int dnx_voq_rate_class_ecn_drop_set(
    int unit,
    uint32 rt_cls,
    uint32 ecn_drop_thresh);

/**
 * \brief -
 * Get the size of committed queue size (i.e., the
 * guaranteed memory) for each VOQ, even in the case that a
 * set of queues consume most of the memory resources.
 *
 * Note: threshold value is in HW granularity.
 */
int dnx_voq_rate_class_guaranteed_get(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 *guaranteed_thresh);

/**
 * \brief -
 * Get ECN static drop threshold
 */
int dnx_voq_rate_class_ecn_drop_get(
    int unit,
    uint32 rt_cls,
    uint32 *ecn_drop_thresh);

/**
 * \brief - Set queue tail drop (FADT) and guaranteed size
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport.
 * \param [in] color - DP (0-3). relevant only for tail drop thresholds
 * \param [in] flags - resource flags (total bytes / PDs / Buffers), color blind (to set same tail drop for all DPs).
 * \param [in] gport_size - sizes to set:
 * size_min - the guaranteed queue size.
 * size_max - max FADT thr
 * size_fadt_min - min FADT thr
 * size_alpha_max  - alpha factor for FADT
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_color_size_set(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - Retrieve queue tail drop (FADT) and guaranteed size
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport.
 * \param [in] color - DP (0-3). relevant only for tail drop thresholds
 * \param [in] flags - resource flags (total bytes / PDs / Buffers), color blind (to set same tail drop for all DPs).
 * \param [in] gport_size - retrieved sizes.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_color_size_get(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - Set DRAM bound threshold (FADT) to HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource type to set the thresholds (SRAM bytes/ buffers/ PDs).
 * \param [in] rt_cls - rate class profile ID
 * \param [in] fadt - fadt thresholds. note that for some resources, there can be a free-resources limit on the FADT algorithm.
 *             it means the FADT algorithm will be used only when the number of free resources is between the specified MIN and MAX values.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_fadt_dram_bound_set(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Set DRAM recovery fail threshold (FADT) to HW. per rate class profile.
 *
 * \param [in] unit - The unit number.
 * \param [in] resource - resource type to set the thresholds (SRAM bytes/ buffers/ PDs).
 * \param [in] rt_cls - rate class profile ID
 * \param [in] fadt - fadt thresholds. note that for some resources, there can be a free-resources limit on the FADT algorithm.
 *             it means the FADT algorithm will be used only when the number of free resources is between the specified MIN and MAX values.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_fadt_dram_recovery_fail_set(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Set DRAM recovery threshold to HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] rt_cls - rate class profile ID
 * \param [in] recovery_threshold - recovery threshold in bytes
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_voq_rate_class_dram_recovery_set(
    int unit,
    uint32 rt_cls,
    uint32 recovery_threshold);

/**
 * \brief - Set congestion notification threshold to HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class profile ID
 * \param [in] resource - resource type (bytes, SRAM buffers, SRAM PDs)
 * \param [in] fadt - thresholds to set.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_voq_rate_class_congestion_notification_fadt_set(
    int unit,
    int rate_class,
    dnx_ingress_congestion_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Get congestion notification threshold from HW. per rate class profile
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class profile ID
 * \param [in] resource - resource type (bytes, SRAM buffers, SRAM PDs)
 * \param [out] fadt - thresholds to get.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_voq_rate_class_congestion_notification_fadt_get(
    int unit,
    int rate_class,
    dnx_ingress_congestion_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * fadt);

/**
 * \brief - Set rate class to operate on OCB only. no traffic should be directed to DRAM.
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class profile ID
 * \param [in] is_ocb_only - is rate class set to operate on OCB only
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_voq_rate_class_ocb_only_set(
    int unit,
    int rate_class,
    int is_ocb_only);

/**
 * \brief - Get is rate class set to operate on OCB only.
 *
 * \param [in] unit - The unit number.
 * \param [in] rate_class - rate class profile ID
 * \param [out] is_ocb_only - is rate class set to operate on OCB only
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_voq_rate_class_ocb_only_get(
    int unit,
    int rate_class,
    int *is_ocb_only);

#endif
