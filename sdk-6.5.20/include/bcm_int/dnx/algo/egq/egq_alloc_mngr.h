/**
 * \file algo/egq/egq_alloc_mngr.h 
 *
 * Internal DNX EGQ Alloc Mngr APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef EGQ_ALLOC_MNGR_H_INCLUDED
/* { */
#define EGQ_ALLOC_MNGR_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
/*
 * }
 */

/*
 * Defines:
 * {
 */
/* COSQ Egress mapping */
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_LOW_ID             (0)
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_COUNT              (16)
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit) (dnx_data_port.general.nof_tm_ports_get(unit))
#define DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_SIZE               (sizeof(dnx_cosq_egress_queue_mapping_info_t))

#define DNX_AM_TEMPLATE_FQP_LOW_ID                   (0)

/*
 * Template name for TC mapping
 */
#define DNX_ALGO_TEMPLATE_EGR_TC_DP_MAPPING             "Egr TC DP mapping"
#define DNX_ALGO_TEMPLATE_FQP_PROFILE_PARAMS    "Egr dequeue profile parameters"

/*
 * }
 */

/*
 * Enums:
 * {
 */

/*
 * }
 */

/*
 * Structs:
 * {
 */

/*
 * }
 */

/*
 * Function prototypes:
 * {
 */
/*
 * COSQ Egress queue mapping - Start
 * {
 */
int dnx_am_template_egress_queue_mapping_create(
    int unit,
    int template_init_id,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile);

int dnx_am_template_egress_queue_mapping_data_get(
    int unit,
    uint32 pp_port,
    int core,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile);

int dnx_am_template_egress_queue_mapping_exchange(
    int unit,
    uint32 tm_port,
    int core,
    const dnx_cosq_egress_queue_mapping_info_t * mapping_profile,
    int *old_profile,
    int *is_last,
    int *new_profile,
    int *is_allocated);

shr_error_e dnx_am_fqp_profile_alloc_single(
    int unit,
    int core,
    int *profile_id,
    dnx_fqp_profile_params_t * profile_params,
    uint8 *is_first);

shr_error_e dnx_am_fqp_profile_free_single(
    int unit,
    int core,
    int profile_id,
    uint8 *is_last);

/*
 * COSQ Egress queue mapping - End
 * }
 */

/*
 * } 
 */
/* } */
#endif /* EGQ_ALLOC_MNGR_H_INCLUDED */
