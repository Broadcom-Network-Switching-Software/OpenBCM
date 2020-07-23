/** \file scheduler_flow_alloc.c
 * 
 *
 * e2e scheduler functionality for DNX -- element allocations
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
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include "scheduler_flow_alloc.h"
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_sch_alloc_mngr_access.h>
#include "scheduler_convert.h"
#include "scheduler_region.h"
#include "scheduler_flow.h"

#define DNX_SCHEDULER_NOF_FQ_IN_QUARTET 3

/* Resource name define for SCH */
#define DNX_SCH_ALLOC_MNGR_RESOURCE    "sch alloc mngr"

/**
 * See .h file.
 */
shr_error_e
dnx_scheduler_flow_alloc_mngr_create(
    int unit)
{
    dnx_algo_res_create_data_t create_data;
    uint32 core_id, sub_resource_id;
    dnx_algo_sch_extra_args_t extra_args;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    /*
     * Set bitmap info
     */
    create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_sch.flow.region_size_get(unit);
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_SCH;
    sal_strncpy(create_data.name, DNX_SCH_ALLOC_MNGR_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create queues bitmap
     */
    SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.alloc(unit, dnx_data_device.general.nof_cores_get(unit),
                                                           dnx_data_sch.flow.nof_regions_get(unit)));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        for (sub_resource_id = 0; sub_resource_id < dnx_data_sch.flow.nof_regions_get(unit); sub_resource_id++)
        {
            extra_args.core_id = core_id;
            extra_args.region_index = sub_resource_id;
            SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.create
                            (unit, core_id, sub_resource_id, &create_data, (void *) &extra_args));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if "flow_type" is available 
 *          in the specific "region_type", return TRUE if it is.
 *        see  dnx_scheduler_allocate for other params
 */
static int
dnx_scheduler_is_type_available_in_region(
    int unit,
    dnx_sch_region_type_e region_type,
    dnx_sch_alloc_flow_type_e flow_type)
{
    int i;
    dnx_sch_element_se_type_e desired_type;
    int is_composite = 0;
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
    int start_index, end_index, skip;

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_NONE;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_NONE;
            is_composite = 1;
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            is_composite = 1;
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_CL;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_CL;
            is_composite = 1;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL:
            for (i = 0; i < DNX_SCHEDULER_QUARTET_SIZE; i++)
            {
                if (dnx_data_sch.sch_alloc.region_get(unit, region_type)->flow_type[i] == DNX_SCH_ELEMENT_SE_TYPE_CL)
                {
                    if (i < 3 && dnx_data_sch.sch_alloc.region_get(unit, region_type)->flow_type[i + 1]
                        == DNX_SCH_ELEMENT_SE_TYPE_FQ)
                    {
                        return 1;
                    }
                }
            }
            return 0;
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            return (region_type == DNX_SCH_REGION_TYPE_SE);
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_HR;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
            desired_type = DNX_SCH_ELEMENT_SE_TYPE_HR;
            is_composite = 1;
            break;
        default:
            return 0;
            break;
    }

    start_index = 0;
    end_index = DNX_SCHEDULER_QUARTET_SIZE;
    skip = 1;

    if (is_composite)
    {
        odd_even_mode = dnx_data_sch.sch_alloc.region_get(unit, region_type)->odd_even_mode;
        if (odd_even_mode == DNX_SCHEDULER_REGION_ODD)
        {
            /** check indexes 0 and 2 */
            start_index = 0;
            skip = 2;
        }
        else
        {
            /** check indexes 0 and 1 */
            start_index = 0;
            end_index = 2;
        }
    }
    for (i = start_index; i < end_index; i += skip)
    {
        if (dnx_data_sch.sch_alloc.region_get(unit, region_type)->flow_type[i] == desired_type)
        {
            return 1;
        }
    }
    return 0;

}

/**
 * \brief - Check if region fit allocation parameters
 */
static shr_error_e
dnx_scheduler_region_verify(
    int unit,
    int core,
    int is_non_contiguous,
    int is_low_rate,
    uint32 nof_remote_cores,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int region_index,
    int *is_fit)
{
    dnx_sch_region_type_e region_type;
    int region_nof_remote_cores;
    int is_region_interdigitated;
    int low_rate_factor;

    SHR_FUNC_INIT_VARS(unit);

    *is_fit = 1;

    region_nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, core, region_index)->val;
    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core, region_index, &is_region_interdigitated));

    if (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR || flow_type == DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR)
    {

        if (region_nof_remote_cores != nof_remote_cores)
        {
            /** nof_of_remote_cores is relevant for connector allocations only */
            *is_fit = 0;
        }
        if (is_non_contiguous == FALSE && is_region_interdigitated)
        {
            *is_fit = 0;
        }
        if (is_non_contiguous == TRUE && !is_region_interdigitated)
        {
            *is_fit = 0;
        }

        SHR_IF_ERR_EXIT(dnx_sch_flow_low_rate_factor_get(unit, core,
                                                         region_index * dnx_data_sch.flow.region_size_get(unit),
                                                         &low_rate_factor));

        if ((is_low_rate && low_rate_factor == 1) || (!is_low_rate && low_rate_factor != 1))
        {
            *is_fit = 0;
        }

    }
    if (*is_fit)
    {
        region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;
        *is_fit = dnx_scheduler_is_type_available_in_region(unit, region_type, flow_type);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Fill alloc_info with the required parameters 
 *          According to flow type and region type
 *        see dnx_algo_sch_alloc_info_t for all alloc info params
 *        see  dnx_scheduler_allocate for other params
 */
static shr_error_e
dnx_scheduler_flow_fill_alloc_info(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int region_index,
    dnx_algo_sch_alloc_info_t * alloc_info)
{
    int indx;
    int composite_conn, interdigitated;

    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
    dnx_sch_region_type_e region_type;

    SHR_FUNC_INIT_VARS(unit);

    alloc_info->offs[0] = 0;
    alloc_info->ignore_tag = 0;
    alloc_info->repeats = 1;
    alloc_info->nof_offsets = 1;

    region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;

    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core, region_index, &interdigitated));
    composite_conn = (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR);

    if (interdigitated)
    {
        /*
         * in interdigitated region 
         * - SE allocation does not require tagging
         * - connector allocation always takes whole block of 8 flows
         * so, the tagging is not necessary
         *
         * Supporting both tagged allocation and non tagged allocation on the same pool
         * requires additional bitmap, 
         * and since this is not functionally required, better be avoid
         */
        alloc_info->tag = 0;
    }
    /** switch on all flow types to fill the info */
    /** inside each type we handle the different regions */
    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR:
            alloc_info->pattern = dnx_data_sch.sch_alloc.connector_get(unit, composite_conn, interdigitated)->pattern;
            alloc_info->length =
                dnx_data_sch.sch_alloc.connector_get(unit, composite_conn, interdigitated)->pattern_size;
            alloc_info->align = dnx_data_sch.sch_alloc.connector_get(unit, composite_conn, interdigitated)->alignment;
            alloc_info->repeats =
                num_cos / dnx_data_sch.sch_alloc.connector_get(unit, composite_conn, interdigitated)->nof_in_pattern;
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
            if (dnx_data_sch.sch_alloc.composite_se_per_region_type_get(unit, flow_type, region_type)->valid)
            {
                /** use region type specific information */
                alloc_info->pattern =
                    dnx_data_sch.sch_alloc.composite_se_per_region_type_get(unit, flow_type, region_type)->pattern;
                alloc_info->length =
                    dnx_data_sch.sch_alloc.composite_se_per_region_type_get(unit, flow_type, region_type)->pattern_size;
                alloc_info->align =
                    dnx_data_sch.sch_alloc.composite_se_per_region_type_get(unit, flow_type, region_type)->alignment;

            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_region_odd_even_mode_get(unit, core, region_index, &odd_even_mode));
                alloc_info->pattern = dnx_data_sch.sch_alloc.composite_se_get(unit, flow_type, odd_even_mode)->pattern;
                alloc_info->length =
                    dnx_data_sch.sch_alloc.composite_se_get(unit, flow_type, odd_even_mode)->pattern_size;
                alloc_info->align = dnx_data_sch.sch_alloc.composite_se_get(unit, flow_type, odd_even_mode)->alignment;
            }
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL:
        /** shared shapers */
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
        {
            if (dnx_data_sch.sch_alloc.se_per_region_type_get(unit, flow_type, region_type)->valid)
            {
                /** use region type specific information */
                alloc_info->pattern =
                    dnx_data_sch.sch_alloc.se_per_region_type_get(unit, flow_type, region_type)->pattern;
                alloc_info->length =
                    dnx_data_sch.sch_alloc.se_per_region_type_get(unit, flow_type, region_type)->pattern_size;
                alloc_info->align =
                    dnx_data_sch.sch_alloc.se_per_region_type_get(unit, flow_type, region_type)->alignment;
                alloc_info->nof_offsets =
                    dnx_data_sch.sch_alloc.se_per_region_type_get(unit, flow_type, region_type)->nof_offsets;
                for (indx = 0; indx < alloc_info->nof_offsets; indx++)
                {
                    alloc_info->offs[indx] =
                        dnx_data_sch.sch_alloc.se_per_region_type_get(unit, flow_type, region_type)->offset[indx];
                }
            }
            else
            {
                /** use general information */

                alloc_info->pattern = dnx_data_sch.sch_alloc.se_get(unit, flow_type)->pattern;
                alloc_info->length = dnx_data_sch.sch_alloc.se_get(unit, flow_type)->pattern_size;
                alloc_info->align = dnx_data_sch.sch_alloc.se_get(unit, flow_type)->alignment;
                alloc_info->nof_offsets = dnx_data_sch.sch_alloc.se_get(unit, flow_type)->nof_offsets;
                for (indx = 0; indx < alloc_info->nof_offsets; indx++)
                {
                    alloc_info->offs[indx] = dnx_data_sch.sch_alloc.se_get(unit, flow_type)->offset[indx];
                }
            }
            break;
        }
        case DNX_SCH_ALLOC_FLOW_TYPE_INVALID:
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow_type is invalid\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the start searching index for allocation
 *        see dnx_scheduler_allocate for params
 */
static int
dnx_scheduler_flow_find_search_start_index(
    int unit,
    dnx_sch_alloc_flow_type_e flow_type)
{
    switch (flow_type)
    {   /** return where we want to start the search according to flow type*/
        case DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR:
        {
            return dnx_data_sch.sch_alloc.type_con_reg_start_get(unit);
        }
        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
        {
            return (dnx_data_sch.sch_alloc.type_con_reg_end_get(unit) + 1);
        }
        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
        {
            return (dnx_data_sch.sch_alloc.type_hr_reg_start_get(unit));
        }
        case DNX_SCH_ALLOC_FLOW_TYPE_INVALID:
        default:

        {
        }
    }
    return 0;
}

/**
 * \brief - return all allocated flow IDs and the number of allocated flows
 */
shr_error_e
dnx_scheduler_flow_alloc_flow_ids_get(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int first_flow_id,
    int flow_id[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS],
    int *nof_flows)
{
    int flow_i;
    int repeat_indx, i;
    int first_flow_offset = 0;
    dnx_algo_sch_alloc_info_t alloc_info;
    int region_index;
    uint32 pattern[1];

    SHR_FUNC_INIT_VARS(unit);

    *nof_flows = 0;

    /** get region_index */
    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(first_flow_id);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info(unit, core, num_cos, flow_type, region_index, &alloc_info));

    flow_i = 0;

    pattern[0] = alloc_info.pattern;
    for (repeat_indx = 0; repeat_indx < alloc_info.repeats; repeat_indx++)
    {
        SHR_BIT_ITER(pattern, alloc_info.length, i)
        {
            if (flow_i == 0)
            {
                first_flow_offset = i;
            }

            flow_id[flow_i] = first_flow_id - first_flow_offset + repeat_indx * alloc_info.length + i;
            flow_i++;
        }
    }

    *nof_flows = flow_i;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return all real flow IDs and the number of flows
 */
shr_error_e
dnx_scheduler_flow_real_flow_ids_get(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int first_flow_id,
    int flow_id[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS],
    int *nof_flows)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_flow_ids_get(unit, core, num_cos, flow_type, first_flow_id,
                                                          flow_id, nof_flows));

    if (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL)
    {
        /** Enchanced CL allocates 2 elements (CL+FQ), but there is only one real element */
        (*nof_flows)--;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_scheduler_flow_allocate(
    int unit,
    int core,
    uint32 nof_remote_cores,
    uint32 flags,
    int is_non_contiguous,
    int is_low_rate,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int src_modid,
    int *flow_id)
{
    shr_error_e res = _SHR_E_INTERNAL;
    int start_index, end_index, region_index;
    int element = dnx_data_sch.sch_alloc.alloc_invalid_flow_get(unit);
    dnx_algo_sch_alloc_info_t alloc_info;
    uint32 alloc_flags = 0;
    int is_type_available_in_region = 0;
    int allocate_with_id;

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR:
            /** for connectors tag is the remote device modid*/
            alloc_info.tag = src_modid;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
            alloc_info.tag = 2;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
            alloc_info.tag = 4;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            alloc_info.tag = 8;
            break;
        default:
            alloc_info.tag = 0;
            break;
    }

    allocate_with_id = DNX_ALGO_RES_ALLOCATE_WITH_ID & flags;

    if (allocate_with_id)
    {
        region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(*flow_id);
        SHR_IF_ERR_EXIT(dnx_scheduler_region_verify(unit, core, is_non_contiguous, is_low_rate, nof_remote_cores,
                                                    num_cos, flow_type, region_index, &is_type_available_in_region));
        if (is_type_available_in_region)
        {
            /** prepare alloc_info for the allocation */
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info(unit, core, num_cos, flow_type, region_index,
                                                               &alloc_info));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "required element %d can't be allocated in region %d\n", *flow_id, region_index);

        }

        /** update needed element in specific region */
        element = DNX_SCH_ELEMENT_INDEX_FROM_FLOW_INDEX_GET(*flow_id);
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {
        /** with_id is not specified -- need to search for the available element */

        start_index = dnx_scheduler_flow_find_search_start_index(unit, flow_type);
        end_index = dnx_data_sch.flow.nof_regions_get(unit);

        /** search in all relevant regions available flow id */
        for (region_index = start_index; region_index < end_index; region_index++)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_region_verify(unit, core, is_non_contiguous, is_low_rate, nof_remote_cores,
                                                        num_cos, flow_type, region_index,
                                                        &is_type_available_in_region));
            if (is_type_available_in_region)
            {
                /** if type is available in this region, try to allocate */
                SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info(unit, core, num_cos, flow_type, region_index,
                                                                   &alloc_info));
                /** check if can allocate */
                res = sch_alloc_db.sch_flow_alloc_mngr.allocate_single(unit, core, region_index,
                                                                       DNX_ALGO_RES_ALLOCATE_SIMULATION,
                                                                       (void *) &alloc_info, &element);

                if (res == _SHR_E_RESOURCE)
                {
                    /** We couldn't allocate flow from this region, we try another region before throwing error */
                    continue;
                }
                else
                {
                    SHR_IF_ERR_EXIT(res);
                    /** flow can be allocated */
                    break;
                }
            }

        }

        if (res != _SHR_E_NONE)
        {
            /** if we reached here - we could not allocate */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Can't find free element\n");
        }

    }

    /*
     * Now we actually can allocate
     */
    SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.allocate_single(unit, core, region_index, alloc_flags,
                                                                     (void *) &alloc_info, &element));

    if (element == dnx_data_sch.sch_alloc.alloc_invalid_flow_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Can't find free element\n");
    }

    *flow_id = DNX_SCH_FLOW_INDEX_FROM_ELEMENT_INDEX_GET(element, region_index);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_scheduler_flow_deallocate(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    int flow_id)
{
    int element = dnx_data_sch.sch_alloc.alloc_invalid_flow_get(unit);
    int region_index;
    int flow_i = 0;
    int all_flow_ids[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS];
    int nof_flows;

    SHR_FUNC_INIT_VARS(unit);

    /** get region_index */
    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);

    /** fill all flows to deallocate according to type */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_flow_ids_get(unit, core, num_cos, flow_type, flow_id,
                                                          all_flow_ids, &nof_flows));

    for (flow_i = 0; flow_i < nof_flows; flow_i++)
    {
        element = DNX_SCH_ELEMENT_INDEX_FROM_FLOW_INDEX_GET(all_flow_ids[flow_i]);

        SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.free_single(unit, core, region_index, element));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_scheduler_flow_is_allocated(
    int unit,
    int core,
    int flow_id,
    int *is_allocated)
{
    int region_index;
    int element;

    uint8 is_allocated_byte;

    SHR_FUNC_INIT_VARS(unit);

    /** get region_index */
    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);

    element = DNX_SCH_ELEMENT_INDEX_FROM_FLOW_INDEX_GET(flow_id);

    SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.is_allocated(unit, core, region_index, element,
                                                                  &is_allocated_byte));

    *is_allocated = is_allocated_byte;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return 1 iff the shared shaper tag is clear
 */
shr_error_e
dnx_scheduler_flow_alloc_is_shared_shaper_tag_clear(
    int unit,
    int core,
    int flow_id,
    int *is_clear)
{
    int region_index;
    int element;
    int range_size, range_start;
    int nof_allocated;

    SHR_FUNC_INIT_VARS(unit);

    /** get region_index */
    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);

    element = DNX_SCH_ELEMENT_INDEX_FROM_FLOW_INDEX_GET(flow_id);

    range_size = dnx_data_sch.sch_alloc.tag_size_aggregate_se_get(unit);

    range_start = element / range_size * range_size;

    SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.nof_allocated_elements_in_range_get(unit, core, region_index,
                                                                                         range_start, range_size,
                                                                                         &nof_allocated));

    *is_clear = (nof_allocated == 0);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return 1 iff there is an allocated flow in the range
 */
shr_error_e
dnx_scheduler_flow_alloc_is_allocated_in_range(
    int unit,
    int core,
    int region_start,
    int region_end,
    int *is_allocated)
{
    int region_index;
    int nof_allocated;
    int region_size;

    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = FALSE;
    for (region_index = region_start; region_index <= region_end; region_index++)
    {
        region_size = dnx_data_sch.flow.region_size_get(unit) /
            dnx_data_sch.flow.nof_remote_cores_get(unit, core, region_index)->val;

        SHR_IF_ERR_EXIT(sch_alloc_db.sch_flow_alloc_mngr.nof_allocated_elements_in_range_get(unit, core, region_index,
                                                                                             0, region_size,
                                                                                             &nof_allocated));
        if (nof_allocated)
        {
            *is_allocated = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
