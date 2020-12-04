
/** \file scheduler_region.c
 * 
 *
 * e2e scheduler functionality for DNX
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dbal/dbal.h>

#include "scheduler_region.h"
#include "scheduler_convert.h"
#include "scheduler_dbal.h"

/*
 * \brief - Return region configuration
 * * is the region interdigitated and
 * * is the region odd
 */
shr_error_e
dnx_scheduler_region_config_get(
    int unit,
    int core,
    int region_index,
    int *is_interdigitated,
    dnx_scheduler_region_odd_even_mode_t * odd_even_mode)
{
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
    quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
    region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;

    *is_interdigitated = (region_type == DNX_SCH_REGION_TYPE_INTERDIG_1);
    *odd_even_mode = dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->odd_even_mode;

    SHR_FUNC_EXIT;
}

/*
 * \brief - Get region odd/even mode
 */
shr_error_e
dnx_scheduler_region_odd_even_mode_get(
    int unit,
    int core,
    int region_index,
    dnx_scheduler_region_odd_even_mode_t * odd_even_mode)
{
    int is_interdigitated;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_region_config_get(unit, core, region_index, &is_interdigitated, odd_even_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Is the region interdigitated
 */
shr_error_e
dnx_scheduler_region_is_interdigitated_get(
    int unit,
    int core,
    int region_index,
    int *is_interdigitated)
{
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_region_config_get(unit, core, region_index, is_interdigitated, &odd_even_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Is the region enhanced cl compact
 */
shr_error_e
dnx_scheduler_region_is_enhanced_cl_compact_get(
    int unit,
    int core,
    int region_index,
    int *enhanced_cl_compact)
{
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
    quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
    region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;

    *enhanced_cl_compact = dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->enhanced_cl_compact;

    SHR_FUNC_EXIT;
}

/*
 * \brief - Get shared shaper propagation order of the region 
 */
shr_error_e
dnx_scheduler_region_prop_order_get(
    int unit,
    int core,
    int region_index,
    dbal_enum_value_field_shared_shaper_prop_order_e * prop_order)
{
    SHR_FUNC_INIT_VARS(unit);

    *prop_order = dnx_data_sch.flow.region_type_get(unit, core, region_index)->propagation_direction;

    SHR_FUNC_EXIT;
}

/*
 * \brief - Has the region connectors
 */
shr_error_e
dnx_scheduler_region_has_connector(
    int unit,
    int core,
    int region_index,
    int *has_connector)
{
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int group_index;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    *has_connector = 0;

    group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
    quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
    region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;

    for (i = 0; i < DNX_SCHEDULER_QUARTET_SIZE; i++)
    {
        if (dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->flow_type[i] ==
            DNX_SCH_ELEMENT_SE_TYPE_NONE)
        {
            *has_connector = 1;
            break;
        }
    }

    SHR_FUNC_EXIT;
}

/*
 * \brief - Has the region connectors
 */
shr_error_e
dnx_scheduler_region_has_se(
    int unit,
    int core,
    int region_index,
    int *has_se)
{
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int group_index;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    *has_se = 0;

    group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
    quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
    region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;

    for (i = 0; i < DNX_SCHEDULER_QUARTET_SIZE; i++)
    {
        if (dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->flow_type[i] !=
            DNX_SCH_ELEMENT_SE_TYPE_NONE)
        {
            *has_se = 1;
            break;
        }
    }

    SHR_FUNC_EXIT;
}

/*
 * \brief - return number of SEs in the quartet of specified region
 */
shr_error_e
dnx_scheduler_region_nof_se_in_quartet_get(
    int unit,
    int core,
    int region_index,
    int *nof_se_in_quartet)
{
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;
    int group_index;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    *nof_se_in_quartet = 0;

    group_index = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_index);
    quartet_type = dnx_data_sch.flow.quartet_type_get(unit, core, group_index)->type;
    region_type = dnx_data_sch.flow.region_type_get(unit, core, region_index)->type;

    for (i = 0; i < DNX_SCHEDULER_QUARTET_SIZE; i++)
    {
        if (dnx_data_sch.sch_alloc.region_get(unit, quartet_type, region_type)->flow_type[i] !=
            DNX_SCH_ELEMENT_SE_TYPE_NONE)
        {
            (*nof_se_in_quartet)++;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set configuration for 1K flows/aggregates (256
 *  quartets). Flows interdigitated mode configuration must
 *  match the interdigitated mode configurations of the
 *  queues they are mapped to.
 *
 *  \note - the following flow
 *     configuration is not allowed: interdigitated = TRUE,
 *     odd_even = FALSE. The reason for this is that
 *     interdigitated configuration defines flow-queue mapping,
 *     but a flow with odd_even configuration = FALSE cannot be
 *     mapped to a queue.
 *
 *  \note - this configuration is only
 *     relevant to flow_id-s in the range 64K - 96K.
 */
shr_error_e
dnx_scheduler_region_info_config(
    int unit)
{
    uint32 entry_handle_id;
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
    int is_interdigitated;
    dbal_enum_value_field_shared_shaper_prop_order_e prop_order = 0;
    int k_idx;

    int core, region_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_REGION_ODD_EVEN_MODE, &entry_handle_id));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        for (region_index = dnx_data_sch.flow.nof_connectors_only_regions_get(unit);
             region_index < dnx_data_sch.flow.nof_regions_get(unit); region_index++)
        {
            /*
             * translate region index
             */
            k_idx = region_index - dnx_data_sch.flow.nof_connectors_only_regions_get(unit);

            /*
             * OddEven
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_region_odd_even_mode_get(unit, core, region_index, &odd_even_mode));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ODD_EVEN_MODE, k_idx, odd_even_mode);
        }

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_REGION_INTERDIGITATED, entry_handle_id));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        for (region_index = dnx_data_sch.flow.nof_connectors_only_regions_get(unit);
             region_index < dnx_data_sch.flow.nof_regions_get(unit); region_index++)
        {
            /*
             * translate region index
             */
            k_idx = region_index - dnx_data_sch.flow.nof_connectors_only_regions_get(unit);

            /*
             * Interdigitated
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core, region_index, &is_interdigitated));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERDIGITATED, k_idx, is_interdigitated);
        }

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_REGION_SHARED_SHAPER_PROP_ORDER, entry_handle_id));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        for (region_index = dnx_data_sch.flow.nof_connectors_only_regions_get(unit);
             region_index < dnx_data_sch.flow.nof_regions_get(unit); region_index++)
        {
            /*
             * translate region index
             */
            k_idx = region_index - dnx_data_sch.flow.nof_connectors_only_regions_get(unit);

            /*
             * Priority propagation order (CIR/EIR)
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_region_prop_order_get(unit, core, region_index, &prop_order));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_SHAPER_PROP_ORDER, k_idx, prop_order);
        }

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - convert to DBAL enum value
 */
static shr_error_e
dnx_scheduler_region_dbal_quartet_type_get(
    int unit,
    dnx_sch_flow_quartet_type_e quartet_order,
    dbal_enum_value_field_quartet_order_e * dbal_quartet_order)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (quartet_order)
    {
        case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
            *dbal_quartet_order = DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_CL_FQ;
            break;
        case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
            *dbal_quartet_order = DBAL_ENUM_FVAL_QUARTET_ORDER_CL_CL_FQ_FQ;
            break;
        case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
            *dbal_quartet_order = DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_FQ_CL;
            break;
        case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
            *dbal_quartet_order = DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_FQ_CL;
            break;
        case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
            *dbal_quartet_order = DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_FQ_CL_CL;
            break;
        case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
            *dbal_quartet_order = DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_CL_FQ;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", quartet_order);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Init flow quartet order
 */
shr_error_e
dnx_scheduler_region_quartet_order_init(
    int unit)
{
    int core_idx;
    int group_start, group_end;
    int group_index;
    dnx_sch_flow_quartet_type_e quartet_order;
    dbal_enum_value_field_quartet_order_e dbal_quartet_order;

    SHR_FUNC_INIT_VARS(unit);

    group_start =
        DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(dnx_data_sch.flow.nof_connectors_only_regions_get(unit));
    group_end = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(dnx_data_sch.flow.nof_regions_get(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        for (group_index = group_start; group_index < group_end; group_index++)
        {
            quartet_order = dnx_data_sch.flow.quartet_type_get(unit, core_idx, group_index)->type;
            SHR_IF_ERR_EXIT(dnx_scheduler_region_dbal_quartet_type_get(unit, quartet_order, &dbal_quartet_order));
            SHR_IF_ERR_EXIT(dnx_sch_flow_quartet_order_set
                            (unit, core_idx, group_index - group_start /* SE group */ , dbal_quartet_order));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
