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
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
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
    dnx_sch_flow_quartet_type_e quartet_type,
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
            switch (region_type)
            {
                case DNX_SCH_REGION_TYPE_INTERDIG_1:
                case DNX_SCH_REGION_TYPE_SE:
                    return 1;
                    break;

                case DNX_SCH_REGION_TYPE_SE_HR:
                    if (quartet_type != DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_FQ)
                    {
                        return 1;
                    }
                    break;

                case DNX_SCH_REGION_TYPE_CON:
                default:
                    break;
            }

            return 0;
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2:
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
        odd_even_mode = dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->odd_even_mode;
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
        if (dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->flow_type[i] == desired_type)
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
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int region_nof_remote_cores;
    int is_region_interdigitated;
    int low_rate_factor;
    int group_index;

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
        group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
        quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
        region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;
        *is_fit = dnx_scheduler_is_type_available_in_region(unit, quartet_type, region_type, flow_type);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 */
static shr_error_e
dnx_scheduler_quartet_order_group_verify(
    int unit,
    int core,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    int group_index,
    int *is_fit)
{

    dnx_sch_flow_quartet_type_e quartet_type;

    SHR_FUNC_INIT_VARS(unit);

    *is_fit = 1;

    if (!dnx_data_sch.flow.feature_get(unit, dnx_data_sch_flow_flex_quartet_supported))
    {
        SHR_EXIT();
    }

    if ((flow_type == DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2) ||
        (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4) || (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8))
    {
        *is_fit = 0;

        quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;

        switch (shared_order)
        {
            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ:
                switch (quartet_type)
                {
                    case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
                    case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                    case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                        *is_fit = 1;
                        break;
                    default:
                /** Do nothing */
                        break;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_CL:
                switch (quartet_type)
                {
                    case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
                    case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
                        *is_fit = 1;
                        break;
                    default:
                /** Do nothing */
                        break;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL:
                switch (quartet_type)
                {
                    case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                    case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
                    case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                        *is_fit = 1;
                        break;
                    default:
                /** Do nothing */
                        break;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ_FQ_FQ:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_FQ)
                {
                    *is_fit = 1;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ_CL_FQ:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ)
                {
                    *is_fit = 1;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_CL_FQ_FQ:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ)
                {
                    *is_fit = 1;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ_FQ_CL:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL)
                {
                    *is_fit = 1;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL_FQ_CL:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL)
                {
                    *is_fit = 1;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_FQ_CL_CL:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL)
                {
                    *is_fit = 1;
                }
                break;

            case DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL_CL_FQ:
                if (quartet_type == DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ)
                {
                    *is_fit = 1;
                }
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal shared shaper order %d \n", shared_order);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill alloc_info with the required parameters for simple elements
 */
shr_error_e
dnx_scheduler_flow_shared_order_from_flow_type_get(
    int unit,
    int core,
    dnx_sch_alloc_flow_type_e flow_type,
    int flow_id,
    dnx_scheduler_shared_shaper_order_t * shared_order)
{
    int group_index;
    int quartet_index;
    dnx_sch_flow_quartet_type_e quartet_type;

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2:
            group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_FLOW_INDEX_GET(flow_id);
            quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
            quartet_index = flow_id % DNX_SCHEDULER_QUARTET_SIZE;

            switch (quartet_type)
            {
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_CL;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                    if (quartet_index == 0)
                    {
                        *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ;
                    }
                    else
                    {
                        *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL;
                    }
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_CL;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                    if (quartet_index == 0)
                    {
                        *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL;
                    }
                    else
                    {
                        *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ;
                    }
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "quartet_type is invalid\n");
            }

            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_FLOW_INDEX_GET(flow_id);
            quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;

            switch (quartet_type)
            {
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ_FQ_FQ;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ_CL_FQ;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_CL_FQ_FQ;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ_FQ_CL;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL_FQ_CL;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_FQ_CL_CL;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                    *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL_CL_FQ;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "quartet_type is invalid\n");
            }
            break;

        default:
            *shared_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_NONE;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill alloc_info with the required parameters for simple elements
 */
static shr_error_e
dnx_scheduler_flow_simple_element_fill_alloc_info(
    int unit,
    int core,
    dnx_sch_flow_quartet_type_e quartet_type,
    dnx_sch_region_type_e region_type,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_algo_sch_alloc_info_t * alloc_info)
{
    int indx;
    int pattern_size;
    uint32 half_pattern = 0;
    dnx_sch_element_se_type_e se_type;

    const dnx_data_sch_sch_alloc_region_t *region_info =
        dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type);

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
            se_type = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            half_pattern = region_info->fq_half_pattern;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
            se_type = DNX_SCH_ELEMENT_SE_TYPE_CL;
            half_pattern = region_info->cl_half_pattern;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
            se_type = DNX_SCH_ELEMENT_SE_TYPE_HR;
            half_pattern = 0;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow_type is invalid\n");
    }

    pattern_size = half_pattern ? (DNX_SCHEDULER_QUARTET_SIZE / 2) : DNX_SCHEDULER_QUARTET_SIZE;

    alloc_info->pattern = 1;
    alloc_info->align = pattern_size;
    alloc_info->length = pattern_size;

    alloc_info->nof_offsets = 0;
    for (indx = 0; indx < pattern_size; indx++)
    {
        if (region_info->flow_type[indx] == se_type)
        {
            alloc_info->offs[alloc_info->nof_offsets] = indx;
            alloc_info->nof_offsets++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill alloc_info with the required parameters for composite elements
 */
static shr_error_e
dnx_scheduler_flow_composite_element_fill_alloc_info(
    int unit,
    int core,
    dnx_sch_flow_quartet_type_e quartet_type,
    dnx_sch_region_type_e region_type,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_algo_sch_alloc_info_t * alloc_info)
{
    int indx;
    int pattern_size;
    int start_index;
    int end_index;
    int skip;
    uint32 half_pattern = 0;
    dnx_sch_element_se_type_e se_type;

    const dnx_data_sch_sch_alloc_region_t *region_info =
        dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type);

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
            se_type = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            half_pattern = region_info->fq_half_pattern;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
            se_type = DNX_SCH_ELEMENT_SE_TYPE_CL;
            half_pattern = region_info->cl_half_pattern;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
            se_type = DNX_SCH_ELEMENT_SE_TYPE_HR;
            half_pattern = 0;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow_type is invalid\n");
    }

    if (region_info->odd_even_mode == DNX_SCHEDULER_REGION_ODD)
    {
        alloc_info->pattern = 0x3;
        pattern_size = half_pattern ? (DNX_SCHEDULER_QUARTET_SIZE / 2) : DNX_SCHEDULER_QUARTET_SIZE;

        start_index = 0;
        end_index = pattern_size;
        skip = 2;
    }
    else
    {
        alloc_info->pattern = 0x5;
        pattern_size = DNX_SCHEDULER_QUARTET_SIZE;

        start_index = 0;
        end_index = (DNX_SCHEDULER_QUARTET_SIZE / 2);
        skip = 1;
    }

    alloc_info->align = pattern_size;
    alloc_info->length = pattern_size;

    alloc_info->nof_offsets = 0;
    for (indx = start_index; indx < end_index; indx += skip)
    {

        if (region_info->flow_type[indx] == se_type)
        {
            alloc_info->offs[alloc_info->nof_offsets] = indx;
            alloc_info->nof_offsets++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill alloc_info with the required parameters for enhanced cl elements
 */
static shr_error_e
dnx_scheduler_flow_enhanced_cl_element_fill_alloc_info(
    int unit,
    int core,
    dnx_sch_flow_quartet_type_e quartet_type,
    dnx_sch_region_type_e region_type,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_algo_sch_alloc_info_t * alloc_info)
{
    const dnx_data_sch_sch_alloc_region_t *region_info =
        dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type);

    SHR_FUNC_INIT_VARS(unit);

    if (region_info->enhanced_cl_compact)
    {
        alloc_info->pattern = 0x3;

        switch (region_type)
        {
            case DNX_SCH_REGION_TYPE_SE:
                alloc_info->length = DNX_SCHEDULER_QUARTET_SIZE / 2;
                alloc_info->align = DNX_SCHEDULER_QUARTET_SIZE / 2;
                alloc_info->nof_offsets = 1;
                alloc_info->offs[0] = 0;
                break;
            case DNX_SCH_REGION_TYPE_SE_HR:
                alloc_info->length = DNX_SCHEDULER_QUARTET_SIZE;
                alloc_info->align = DNX_SCHEDULER_QUARTET_SIZE;
                alloc_info->nof_offsets = 1;
                alloc_info->offs[0] = 2;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "region_type is invalid\n");
        }
    }
    else
    {
        alloc_info->pattern = 0x5;
        alloc_info->length = DNX_SCHEDULER_QUARTET_SIZE;
        alloc_info->align = DNX_SCHEDULER_QUARTET_SIZE;

        switch (region_type)
        {
            case DNX_SCH_REGION_TYPE_SE:
                alloc_info->nof_offsets = 2;
                alloc_info->offs[0] = 0;
                alloc_info->offs[1] = 1;
                break;
            case DNX_SCH_REGION_TYPE_SE_HR:
                alloc_info->nof_offsets = 1;
                alloc_info->offs[0] = 1;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "region_type is invalid\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill alloc_info with the required parameters for dual shaper elements
 */
static shr_error_e
dnx_scheduler_flow_dual_shaper_element_fill_alloc_info(
    int unit,
    int core,
    dnx_sch_flow_quartet_type_e quartet_type,
    dnx_sch_region_type_e region_type,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    dnx_algo_sch_alloc_info_t * alloc_info)
{
    int outer_indx;
    int inner_index;
    int pattern_size;
    uint8 pattern_found;
    uint32 half_pattern = 0;
    dnx_sch_element_se_type_e se_type[2];

    const dnx_data_sch_sch_alloc_region_t *region_info =
        dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type);

    SHR_FUNC_INIT_VARS(unit);

    switch (shared_order)
    {
        case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_FQ:
            se_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
            se_type[1] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            break;
        case DNX_SCHEDULER_SHARED_SHAPER_ORDER_FQ_CL:
            se_type[0] = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            se_type[1] = DNX_SCH_ELEMENT_SE_TYPE_CL;
            break;
        case DNX_SCHEDULER_SHARED_SHAPER_ORDER_CL_CL:
            se_type[0] = DNX_SCH_ELEMENT_SE_TYPE_CL;
            se_type[1] = DNX_SCH_ELEMENT_SE_TYPE_CL;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "shared_order is invalid\n");
    }

    half_pattern = region_info->cl_half_pattern && region_info->fq_half_pattern;

    pattern_size = half_pattern ? (DNX_SCHEDULER_QUARTET_SIZE / 2) : DNX_SCHEDULER_QUARTET_SIZE;

    alloc_info->pattern = 0x3;
    alloc_info->align = pattern_size;
    alloc_info->length = pattern_size;

    alloc_info->nof_offsets = 0;
    for (outer_indx = 0; outer_indx < pattern_size; outer_indx += 2)
    {
        pattern_found = TRUE;
        for (inner_index = 0; inner_index < 2; inner_index++)
        {
            if (region_info->flow_type[outer_indx + inner_index] != se_type[inner_index])
            {
                pattern_found = FALSE;
                break;
            }
        }

        if (pattern_found == TRUE)
        {
            alloc_info->offs[alloc_info->nof_offsets] = outer_indx;
            alloc_info->nof_offsets++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduler_flow_fill_alloc_info_flex_quartet_internal(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    int region_index,
    dnx_algo_sch_alloc_info_t * alloc_info)
{
    int composite_conn, interdigitated;
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
    quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
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

        /** simple elements */
        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_simple_element_fill_alloc_info
                            (unit, core, quartet_type, region_type, flow_type, alloc_info));
            break;

        /** composite elements */
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_composite_element_fill_alloc_info
                            (unit, core, quartet_type, region_type, flow_type, alloc_info));
            break;

        /** enhanced cl */
        case DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_enhanced_cl_element_fill_alloc_info
                            (unit, core, quartet_type, region_type, flow_type, alloc_info));
            break;

        /** dual shapers */
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_dual_shaper_element_fill_alloc_info
                            (unit, core, quartet_type, region_type, flow_type, shared_order, alloc_info));
            break;

        /** quad shapers */
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
            alloc_info->pattern = 0xf;
            alloc_info->length = DNX_SCHEDULER_QUARTET_SIZE;
            alloc_info->align = DNX_SCHEDULER_QUARTET_SIZE;
            break;

        /** octet shapers */
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            alloc_info->pattern = 0xff;
            alloc_info->length = DNX_SCHEDULER_QUARTET_SIZE * 2;
            alloc_info->align = DNX_SCHEDULER_QUARTET_SIZE * 2;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow_type is invalid\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduler_flow_fill_alloc_info_internal(
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
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2:
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
 * \brief - Fill alloc_info with the required parameters
 *          According to flow type and region type
 *        see dnx_algo_sch_alloc_info_t for all alloc info params
 *        see  dnx_scheduler_allocate for other params
 */
shr_error_e
dnx_scheduler_flow_fill_alloc_info(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    int region_index,
    dnx_algo_sch_alloc_info_t * alloc_info)
{

    SHR_FUNC_INIT_VARS(unit);

    alloc_info->offs[0] = 0;
    alloc_info->ignore_tag = 0;
    alloc_info->repeats = 1;
    alloc_info->nof_offsets = 1;

    if (!dnx_data_sch.flow.feature_get(unit, dnx_data_sch_flow_flex_quartet_supported))
    {
        /** fill alloc info for non-flexible flow quartet devices */
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info_internal
                        (unit, core, num_cos, flow_type, region_index, alloc_info));
    }
    else
    {
        /** fill alloc info for devices supporting flexible flow quartet */
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info_flex_quartet_internal
                        (unit, core, num_cos, flow_type, shared_order, region_index, alloc_info));
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
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2:
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
    dnx_scheduler_shared_shaper_order_t shared_order;

    SHR_FUNC_INIT_VARS(unit);

    *nof_flows = 0;

    /** get region_index */
    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(first_flow_id);

    /** get shared shaper order */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_shared_order_from_flow_type_get
                    (unit, core, flow_type, first_flow_id, &shared_order));

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info
                    (unit, core, num_cos, flow_type, shared_order, region_index, &alloc_info));

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
 * Convert the allocate flow id, for enhanced CL, to the actual CL ID.
 * This function is required for cases in which the FQ comes first in flow order.
 */
static shr_error_e
dnx_scheduler_flow_cl_from_base_enhanced_cl_get(
    int unit,
    int core,
    int region_index,
    int src_id,
    int *dst_id)
{
    int offset;
    int enhanced_cl_compact;
    dnx_sch_element_se_type_e se_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, src_id, &se_type));

    offset = 0;
    if (se_type != DNX_SCH_ELEMENT_SE_TYPE_CL)
    {
        /** if SE type is not CL, add an offset according to quartet order */
        SHR_IF_ERR_EXIT(dnx_scheduler_region_is_enhanced_cl_compact_get
                        (unit, core, region_index, &enhanced_cl_compact));
        offset = (enhanced_cl_compact ? 1 : 2);
    }

    *dst_id = src_id + offset;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Convert the CL flow ID to the base flow id, allocate for enhanced.
 * This function is required for cases in which the FQ comes first in flow order.
 */
static shr_error_e
dnx_scheduler_flow_base_enhanced_cl_from_cl_get(
    int unit,
    int core,
    int region_index,
    int src_id,
    int *dst_id)
{
    int enhanced_cl_compact;

    int compact_offsets[DNX_SCHEDULER_QUARTET_SIZE] = { 0, 1, 0, 1 };
    int non_compact_offsets[DNX_SCHEDULER_QUARTET_SIZE] = { 0, 0, 2, 2 };
    int *offsets_ptr;
    int offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_enhanced_cl_compact_get(unit, core, region_index, &enhanced_cl_compact));

    /** Remove offset according to position in quartet */
    offsets_ptr = enhanced_cl_compact ? compact_offsets : non_compact_offsets;
    offset = offsets_ptr[src_id % DNX_SCHEDULER_QUARTET_SIZE];

    *dst_id = src_id - offset;

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
    dnx_scheduler_shared_shaper_order_t shared_order,
    int src_modid,
    int *flow_id)
{
    shr_error_e res = _SHR_E_INTERNAL;
    int flow_type_first_region, flow_type_end_region;
    int start_index, end_index, region_index;
    int start_group, end_group, group_index;
    int element = dnx_data_sch.sch_alloc.alloc_invalid_flow_get(unit);
    dnx_algo_sch_alloc_info_t alloc_info;
    uint32 alloc_flags = 0;
    int is_type_available_in_region = 0;
    int is_type_available_in_order_group = 0;
    int allocate_with_id;
    int is_flow_available = FALSE;
    int flow_id_temp;

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR:
            /** for connectors tag is the remote device modid*/
            alloc_info.tag = src_modid;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_2:
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
        group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_FLOW_INDEX_GET(*flow_id);
        SHR_IF_ERR_EXIT(dnx_scheduler_quartet_order_group_verify
                        (unit, core, flow_type, shared_order, group_index, &is_type_available_in_order_group));
        if (!is_type_available_in_order_group)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "required element %d can't be allocated in quartet order group %d\n", *flow_id,
                         group_index);

        }

        region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(*flow_id);
        SHR_IF_ERR_EXIT(dnx_scheduler_region_verify(unit, core, is_non_contiguous, is_low_rate, nof_remote_cores,
                                                    num_cos, flow_type, region_index, &is_type_available_in_region));
        if (is_type_available_in_region)
        {
            /** prepare alloc_info for the allocation */
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info
                            (unit, core, num_cos, flow_type, shared_order, region_index, &alloc_info));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "required element %d can't be allocated in region %d\n", *flow_id, region_index);

        }

        if (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_base_enhanced_cl_from_cl_get
                            (unit, core, region_index, *flow_id, &flow_id_temp));
            *flow_id = flow_id_temp;
        }

        /** update needed element in specific region */
        element = DNX_SCH_ELEMENT_INDEX_FROM_FLOW_INDEX_GET(*flow_id);
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {
        /** with_id is not specified -- need to search for the available element */
        flow_type_first_region = dnx_scheduler_flow_find_search_start_index(unit, flow_type);
        flow_type_end_region = dnx_data_sch.flow.nof_regions_get(unit);

        start_group = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(flow_type_first_region);
        end_group = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(flow_type_end_region);

        for (group_index = start_group; group_index < end_group; group_index++)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_quartet_order_group_verify
                            (unit, core, flow_type, shared_order, group_index, &is_type_available_in_order_group));
            if (!is_type_available_in_order_group)
            {
                /** flow type is not available in quartet order group */
                continue;
            }

            start_index =
                UTILEX_MAX(group_index * dnx_data_sch.flow.nof_regions_in_quartet_order_group_get(unit),
                           flow_type_first_region);
            end_index =
                UTILEX_MIN((group_index + 1) * dnx_data_sch.flow.nof_regions_in_quartet_order_group_get(unit),
                           flow_type_end_region);

            /** search in all relevant regions available flow id */
            for (region_index = start_index; region_index < end_index; region_index++)
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_region_verify
                                (unit, core, is_non_contiguous, is_low_rate, nof_remote_cores, num_cos, flow_type,
                                 region_index, &is_type_available_in_region));
                if (is_type_available_in_region)
                {
                    /** if type is available in this region, try to allocate */
                    SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info
                                    (unit, core, num_cos, flow_type, shared_order, region_index, &alloc_info));
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
                        is_flow_available = TRUE;
                        break;
                    }
                }
            }

            if (is_flow_available == TRUE)
            {
                /** flow can be allocated */
                break;
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

    if (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_cl_from_base_enhanced_cl_get
                        (unit, core, region_index, *flow_id, &flow_id_temp));
        *flow_id = flow_id_temp;
    }

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
    int flow_id_temp;

    SHR_FUNC_INIT_VARS(unit);

    /** get region_index */
    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);

    if (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_base_enhanced_cl_from_cl_get
                        (unit, core, region_index, flow_id, &flow_id_temp));
        flow_id = flow_id_temp;
    }

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
