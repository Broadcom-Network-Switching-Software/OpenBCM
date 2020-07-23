/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/cosq/scheduler_allocators.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_ALLOCATORS__H_INCLUDED_
#define _DNX_SCHEDULER_ALLOCATORS__H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>
#include "scheduler_dbal.h"

typedef enum
{
    DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_NON_CHANNELIZED = 0,
    DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_SMALL,
    DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG
} dnx_scheduler_allocators_interface_type_t;

/**
 * \brief
 *   Initialize SCH allocation manager.
 *
 * \param [in] unit -
 *   Relevant unit.
 * \retval shr_error_e - 
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_scheduler_allocators_init(
    int unit);

/**
 * \brief -
 * allocate new scheduler interface
 * if DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG is specified, big interface would be allocated if available.
 * if DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_SMALL is specidied, regular interface would be allocated if available.
 */
shr_error_e dnx_scheduler_allocators_interface_allocate(
    int unit,
    int core,
    dnx_scheduler_allocators_interface_type_t interface_type,
    int nof_interfaces,
    int *interface_id);

/**
 * \brief -
 * deallocate scheduler interface
 */
shr_error_e dnx_scheduler_allocators_interface_deallocate(
    int unit,
    int core,
    int interface_id,
    int nof_interfaces);

/**
 * \brief - return 1 iff the interface tag is clear
 */
shr_error_e dnx_scheduler_allocators_interface_is_tag_clear(
    int unit,
    int core,
    int interface_id,
    int *is_clear);

/**
 * \brief - allocate CL class profile
 */
shr_error_e dnx_scheduler_allocators_cl_profile_alloc(
    int unit,
    int core_id,
    uint32 flags,
    dnx_sch_cl_info_t * profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

/**
 * \brief - deallocate cl profile 
 */
shr_error_e dnx_scheduler_allocators_cl_profile_dealloc(
    int unit,
    int core_id,
    int profile,
    uint8 *last_reference);

/**
 * \brief - get cl profile template data
 */
shr_error_e dnx_scheduler_allocators_cl_profile_data_get(
    int unit,
    int core_id,
    int profile,
    int *ref_count,
    dnx_sch_cl_info_t * profile_data);

/**
 * \brief - Exchange cl profile template data
 */
shr_error_e dnx_scheduler_allocators_cl_profile_exchange(
    int unit,
    int core_id,
    uint32 flags,
    const dnx_sch_cl_info_t * profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference);

#endif
