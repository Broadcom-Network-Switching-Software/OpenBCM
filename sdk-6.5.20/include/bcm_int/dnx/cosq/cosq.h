/** \file bcm_int/dnx/cosq/cosq.h
 * 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_COSQ_INCLUDED__
#define _DNX_COSQ_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
/*
 * Defines:
 * {
 */

#define DNX_COSQ_NOF_TC (8)
#define DNX_COSQ_NOF_DP (4)

#define DNX_COSQ_MIN_COS (4)
#define DNX_COSQ_MAX_COS (8)

#define DNX_COSQ_RESERVED_FLOW_START (0)
#define DNX_COSQ_RESERVED_FLOW_NOF (16)
/*
 * } 
 */
/*
 * Typedefs:
 * {
 */

typedef enum
{
    DNX_COSQ_THRESHOLD_INDEX_INVALID = -1,
    DNX_COSQ_THRESHOLD_INDEX_DP,
    DNX_COSQ_THRESHOLD_INDEX_TC,
    DNX_COSQ_THRESHOLD_INDEX_POOL_DP,
    DNX_COSQ_THRESHOLD_INDEX_CAST_PRIO_FDWACT,
    DNX_COSQ_THRESHOLD_INDEX_PRIO,
    DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO
} dnx_cosq_threshold_index_type_t;

typedef struct
{
    int dp;
    int tc;
    int pool_id;
    bcm_cast_t cast;
    bcm_cosq_ingress_forward_priority_t priority;
    bcm_cosq_forward_decision_type_t fwd_action;
} dnx_cosq_threshold_index_info_t;

typedef struct
{
    /*
     *  Passing this threshold from above means activation.
     */
    uint32 set;
    /*
     *  Passing this threshold from below means deactivation.
     */
    uint32 clear;
} dnx_cosq_hyst_threshold_t;

typedef struct
{
    /**
     * Maximum FADT threshold
     */
    uint32 max_threshold;
    /**
     * Minimum FADT threshold
     */
    uint32 min_threshold;
    /**
     * If AdjustFactor3 is set,  
     *    Dynamic-Max-Th = Free-Resource << AdjustFactor2:0 
     * Otherwise, 
     *    Dynamic-Max-Th = Free-Resource >> AdjustFactor2:0
     */
    int alpha;
} dnx_cosq_fadt_threshold_t;

typedef struct
{
    /**
     *  Passing this threshold from above means activation.
     *  The threshold is dynamic (FADT).
     */
    dnx_cosq_fadt_threshold_t set;
    /**
     *  Passing this threshold from below means deactivation.
     *  The threshold is an offset below the FADT set threshold.
     */
    uint32 clear_offset;
} dnx_cosq_fadt_hyst_threshold_t;

 /*
  * } 
  */
/*
 * Functions
 * {
 */

/**
 * \brief - internal function for setting static (non FADT) threshold,
 *          this function supports all available thresh_info->index types, unlike the BCM API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - additional flags. currently not in use
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in] threshold - threshold value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_gport_static_threshold_internal_set(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief - internal function getting static (non FADT) threshold
 *          this function supports all available thresh_info->index types, unlike the BCM API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - additional flags. currently not in use
 *   \param [in] thresh_info - complementary information describing the threshold to be get
 *   \param [in] threshold - obtained threshold value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_gport_static_threshold_internal_get(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/*
 * }
 */

/**
 * \brief - get the queue id and core from gport type queue
 * \param [in] unit - unit
 * \param [in] gport_queue -  gport to retrieve the information for
 * \param [in] cosq - offset of the queue group
 * \param [in] force_core_all - indicate if to verify that the core is BCM_CORE_ALL
 * \param [out] core - core the queue belong
 * \param [out] queue_id - queue id
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e dnx_cosq_gport_to_voq_get(
    int unit,
    bcm_gport_t gport_queue,
    bcm_cos_queue_t cosq,
    int force_core_all,
    bcm_core_t * core,
    int *queue_id);

/**
 * \brief -
 * verify gport and cosq for rate class threshold APIs
 *
 * \param [in] unit - unit index
 * \param [in] gport - gport to verify. BCM_GPORT_IS_PROFILE for rate class profile. if is_vsq_allowed is set BCM_COSQ_GPORT_IS_VSQ is also accepted.
 * \param [in] cosq - cosq to verify. expected 0 or -1
 * \param [in] is_vsq_allowed - indication whether to allow VSQ Gport as well.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
int dnx_cosq_rate_class_gport_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int is_vsq_allowed);

#endif /* COSQ_H_INCLUDED */
