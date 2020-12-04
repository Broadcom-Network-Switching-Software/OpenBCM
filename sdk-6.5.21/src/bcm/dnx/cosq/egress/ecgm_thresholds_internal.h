/**
* \file src/bcm/dnx/cosq/egress/ecgm_thresholds_internal.h
* 
*
* internal threshold management for Egress Congestion functions
* this file includes threshold elements that are shared between
* ecgm files.
* these mainly include threshold values validation.
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _EGRESS_CONGESTION_THRESHOLDS_INTERNAL_H_INCLUDED_
/** { */
#define _EGRESS_CONGESTION_THRESHOLDS_INTERNAL_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
* Included files.
* {
*/

#include <bcm/types.h>  /* uint32 */
#include <shared/shrextend/shrextend_debug.h>   /* debug defines */
#include <bcm_int/dnx/cosq/cosq.h>      /* typedefs */

/**
* }
*/

/**
* Typedefs
* {
*/

typedef enum
{
    PD_DROP,
    PD_MIN_DROP,
    PD_MAX_DROP,
    PD_ALPHA_DROP,
    PD_FC,
    PD_MIN_FC,
    PD_MAX_FC,
    PD_ALPHA_FC,
    PD_DROP_PER_DP,
    PD_MIN_DROP_PER_DP,
    PD_MAX_DROP_PER_DP,
    PD_ALPHA_DROP_PER_DP,
    DB_FC,
    DB_DROP,
    DB_DROP_PER_DP,
    DB_MIN_FC,
    DB_MAX_FC,
    DB_ALPHA_FC,
    UC_PD_FC,
    UC_PD_MIN_FC,
    UC_PD_MAX_FC,
    UC_PD_ALPHA_FC,
    UC_DB_FC,
    UC_DB_MIN_FC,
    UC_DB_MAX_FC,
    UC_DB_ALPHA_FC,
    UC_PD_DROP,
    UC_DB_DROP,
    MC_PD_FC,
    MC_DB_FC,
    MC_PD_DROP,
    MC_DB_DROP,
    SP_PD_FC,
    SP_DB_FC,
    SP_PD_DROP,
    SP_DB_DROP,
    SP_DB_MIN_DROP,
    SP_DB_MAX_DROP,
    SP_DB_ALPHA_0_DROP,
    SP_DB_ALPHA_1_DROP,
    SP_PD_RESERVED,
    SP_DB_RESERVED
} dnx_ecgm_threshold_tag_t;

/* threshold information */
typedef struct
{
    bcm_cos_queue_t cosq;
    dnx_ecgm_threshold_tag_t tag;
    bcm_cosq_egress_multicast_service_pools_t sp;
    bcm_cosq_threshold_t *threshold;
} dnx_ecgm_threshold_data_t;

typedef enum
{
    DNX_ECGM_THRESHOLD_SP_SET,
    DNX_ECGM_THRESHOLD_SP_INCREASE,
    DNX_ECGM_THRESHOLD_SP_DECREASE
} dnx_ecgm_threshold_sp_action_e;

/** } */

/** \brief - verify legal alpha value */
shr_error_e dnx_ecgm_alpha_verify(
    int unit,
    int alpha);

/** \brief - verify legal min value (min <= max) */
shr_error_e dnx_ecgm_min_threshold_verify(
    int unit,
    int max,
    int min);

/** \brief - verify legal PD value (0 <= value < total core PDs) */
shr_error_e dnx_ecgm_pd_threshold_verify(
    int unit,
    int value);

/** \brief - verify legal DB value (0 <= value < total core DBs) */
shr_error_e dnx_ecgm_db_threshold_verify(
    int unit,
    int value);

/**
* \brief - modify HW values for amount of reserved DBs for a service pool.
*          verifing function will fail if new reserves value makes the
*          total reserves for both SPs higher than core total DBs,
*          and will print warning if the new total is higher than
*          core multicast DB drop value.
*/
shr_error_e dnx_ecgm_sp_reserved_db_threshold_modify(
    int unit,
    int core_id,
    int sp,
    int reserved_db,
    dnx_ecgm_threshold_sp_action_e action);

/**
* \brief - verify dp value.
*          returns error if given dp is not 0-DNX_COSQ_NOF_DP.
*
* \param[in] unit - unit id.
* \param[in] dp - dp value to test.
*
* \return
*      shr_error_e
*/
shr_error_e dnx_ecgm_dp_verify(
    int unit,
    int dp);

/** } */

#endif  /** _EGRESS_CONGESTION_THRESHOLDS_INTERNAL_H_INCLUDED_ */
