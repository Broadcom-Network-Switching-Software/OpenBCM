
/** \file scheduler_allocators.c
 * 
 *
 * e2e scheduler functionality for DNX --  allocations
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <bcm/types.h>
#include <shared/shrextend/shrextend_error.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_sch_alloc_mngr_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

#include "scheduler_flow_alloc.h"
#include "scheduler_dbal.h"
#include "scheduler_allocators.h"

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

/**
 * \brief -
 * initialize CL class template manager
 */
static shr_error_e
dnx_scheduler_allocators_cl_class_tmpl_mngr_create(
    int unit)
{
    dnx_algo_template_create_data_t create_info;
    dnx_sch_cl_info_t cl_template_data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));
    sal_memset(&cl_template_data, 0, sizeof(dnx_sch_cl_info_t));

    create_info.flags = 0;
    create_info.first_profile = 0;
    create_info.nof_profiles = dnx_data_sch.se.cl_class_profile_nof_get(unit);
    /** one additional dummy reference for default profile */
    create_info.max_references = dnx_data_sch.flow.nof_cl_get(unit) + 1;
    create_info.default_profile = 0;
    create_info.data_size = sizeof(dnx_sch_cl_info_t);
    create_info.default_data = &cl_template_data;
    sal_strncpy(create_info.name, "TEMPLATE_COSQ_SCHED_CLASS", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.create(unit, core_id, &create_info, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
void
dnx_sch_cl_class_print_cb(
    int unit,
    const void *data)
{
    dnx_sch_cl_info_t *profile_data;

    if (data)
    {
        SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

        profile_data = (dnx_sch_cl_info_t *) data;

        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "CL config", NULL, NULL);
        {
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "cl_mode", profile_data->config.cl_mode,
                                           NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "wfq_weight_mode",
                                           profile_data->config.wfq_weight_mode, NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "enhanced_mode",
                                           profile_data->config.enhanced_mode, NULL, NULL);
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
        }

        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "CL weight", NULL, NULL);
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
        {
             /* coverity[overrun-buffer-val:FALSE]  */
            SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_INT, "weight", profile_data->weight.weight,
                                          NULL, NULL, DNX_SCH_CL_WEIGHTS_NOF);
        }
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();

        SW_STATE_PRETTY_PRINT_FINISH();
    }
    else
    {
        LOG_CLI((BSL_META_U(unit, "invalid profile data\n")));
    }

    return;
}

/**
 * \brief - allocate and configure CL class profile
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_alloc(
    int unit,
    int core_id,
    uint32 flags,
    dnx_sch_cl_info_t * profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{

    SHR_FUNC_INIT_VARS(unit);

    /** allocate profile */
    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.allocate_single(unit, core_id, flags, profile_data,
                                                                     extra_arguments, profile, first_reference));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - deallocate cl profile 
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_dealloc(
    int unit,
    int core_id,
    int profile,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.free_single(unit, core_id, profile, last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get cl profile template data
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_data_get(
    int unit,
    int core_id,
    int profile,
    int *ref_count,
    dnx_sch_cl_info_t * profile_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.profile_data_get(unit, core_id, profile, ref_count, profile_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Exchange cl profile template data
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_exchange(
    int unit,
    int core_id,
    uint32 flags,
    const dnx_sch_cl_info_t * profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.exchange(unit, core_id, flags, profile_data, old_profile,
                                                              extra_arguments, new_profile,
                                                              first_reference, last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create scheduler interface allocation manager
 */
shr_error_e
dnx_scheduler_interface_allocator_create(
    int unit)
{
    resource_tag_bitmap_extra_arguments_create_info_t create_extra_info;
    dnx_algo_res_create_data_t create_data;

    uint32 core_id;
    int reserved_sch_if = dnx_data_sch.interface.reserved_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));
    sal_memset(&create_extra_info, 0, sizeof(create_extra_info));

    /*
     * Set bitmap info
     */
    create_data.flags = 0;
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_sch.interface.nof_sch_interfaces_get(unit);

    create_extra_info.grain_size = 8;
    create_extra_info.max_tag_value = 8;

    sal_strncpy(create_data.name, "SCH interface allocator", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create sch interface allocator
     */
    SHR_IF_ERR_EXIT(sch_alloc_db.interface.alloc(unit, dnx_data_device.general.nof_cores_get(unit)));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(sch_alloc_db.interface.create(unit, core_id, &create_data, &create_extra_info));

        if (reserved_sch_if < create_data.nof_elements)
        {
            /** mark reserved SCH interface taken */
            SHR_IF_ERR_EXIT(sch_alloc_db.interface.allocate_several(unit, core_id, DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                    1, NULL, &reserved_sch_if));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * allocate new scheduler interface
 * if DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_NON_CHANNELIZED is specified and non channelized calendar is supported,
 * non channelized interface is allocated.
 * if DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG is specified,
 * big interface would be allocated if available (otherwise regular).
 * if DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_SMALL is specidied,
 * regular interface would be allocated if available (otherwise big).
 */
shr_error_e
dnx_scheduler_allocators_interface_allocate(
    int unit,
    int core,
    dnx_scheduler_allocators_interface_type_t interface_type,
    int nof_interfaces,
    int *interface_id)
{
    shr_error_e res = _SHR_E_INTERNAL;
    int allocated_id = -1;

    resource_tag_bitmap_extra_arguments_alloc_info_t alloc_info;

    uint32 flags = RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_extra_arguments_alloc_info_t));

    /*
     * Tags are used for Port-Priority-Propagation sub-interface marking
     */
    alloc_info.tag = nof_interfaces;

    if (interface_type == DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_NON_CHANNELIZED &&
        dnx_data_sch.interface.feature_get(unit, dnx_data_sch_interface_non_channelized_calendar))
    {
        alloc_info.range_start = dnx_data_sch.interface.nof_channelized_calendars_get(unit);
        alloc_info.range_end = dnx_data_sch.interface.nof_sch_interfaces_get(unit);
        res = sch_alloc_db.interface.allocate_several(unit, core, flags | DNX_ALGO_RES_ALLOCATE_SIMULATION,
                                                      nof_interfaces, (void *) &alloc_info, &allocated_id);
        if (res != _SHR_E_RESOURCE)
        {
            /** actually allocate the interface */
            SHR_IF_ERR_EXIT(sch_alloc_db.interface.allocate_several(unit, core, flags, nof_interfaces,
                                                                    (void *) &alloc_info, &allocated_id));
        }
    }
    if (res != _SHR_E_NONE)
    {
        if (interface_type == DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG)
        {
            alloc_info.range_start = 0;
            alloc_info.range_end = dnx_data_sch.interface.nof_big_calendars_get(unit);
        }
        else
        {
            alloc_info.range_start = dnx_data_sch.interface.nof_big_calendars_get(unit);
            alloc_info.range_end = dnx_data_sch.interface.nof_channelized_calendars_get(unit);

        }

        /** check if can allocate */
        res = sch_alloc_db.interface.allocate_several(unit, core,
                                                      flags | DNX_ALGO_RES_ALLOCATE_SIMULATION,
                                                      nof_interfaces, (void *) &alloc_info, &allocated_id);

        if (res == _SHR_E_RESOURCE)
        {
            /** can't allocate interface of the preferred type, try the other type */
            if (interface_type == DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG)
            {
                alloc_info.range_start = dnx_data_sch.interface.nof_big_calendars_get(unit);
                alloc_info.range_end = dnx_data_sch.interface.nof_channelized_calendars_get(unit);
            }
            else
            {
                alloc_info.range_start = 0;
                alloc_info.range_end = dnx_data_sch.interface.nof_big_calendars_get(unit);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(res);
        }

        /*
         * Now we actually can allocate
         */
        SHR_IF_ERR_EXIT(sch_alloc_db.interface.allocate_several(unit, core, flags, nof_interfaces,
                                                                (void *) &alloc_info, &allocated_id));
    }

    *interface_id = allocated_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * deallocate scheduler interface
 */
shr_error_e
dnx_scheduler_allocators_interface_deallocate(
    int unit,
    int core,
    int interface_id,
    int nof_interfaces)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.interface.free_several(unit, core, nof_interfaces, interface_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return 1 iff the interface tag is clear
 */
shr_error_e
dnx_scheduler_allocators_interface_is_tag_clear(
    int unit,
    int core,
    int interface_id,
    int *is_clear)
{
    int range_size, range_start;
    int nof_allocated;

    SHR_FUNC_INIT_VARS(unit);

    range_size = dnx_data_sch.ps.nof_hrs_in_ps_get(unit);

    range_start = interface_id / range_size * range_size;

    SHR_IF_ERR_EXIT(sch_alloc_db.interface.nof_allocated_elements_in_range_get(unit, core, range_start, range_size,
                                                                               &nof_allocated));

    *is_clear = (nof_allocated == 0);

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_scheduler_allocators_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.init(unit));

    /** create allocator for scheduler flow */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_mngr_create(unit));

    /** create allocator for scheduler interface */
    SHR_IF_ERR_EXIT(dnx_scheduler_interface_allocator_create(unit));

    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_class_tmpl_mngr_create(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_scheduler_allocators_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    /*
     * Resource and template manager don't require deinitialization per instance.
     */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
