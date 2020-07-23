/** \file scheduler_flow.c
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
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>

#include "scheduler_dbal.h"
#include "scheduler_region.h"
#include "scheduler_convert.h"
#include "scheduler_element.h"
#include "scheduler.h"

/*
 * \brief - Configure flow attributes to their default (reset) values
 */
shr_error_e
dnx_scheduler_flow_default_attributes_set(
    int unit,
    int core,
    int flow_id)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * reset the following properties:
     *   CREDIT_SRC_SE_ID, CREDIT_SRC_COS, CREDIT_SRC_HR_SEL_DUAL, 
     *   IS_FABRIC, IS_HIGH_PRIO, IS_SLOW_ENABLE
     *
     * Note, that some of the properties are reset to non-zero default value 
     */
    SHR_IF_ERR_EXIT(dnx_sch_flow_attributes_reset(unit, core, flow_id));

    /*
     * reset the following properties:
     *   rate, max burst, slow rate profile
     *
     * Note, that some of the properties are reset to non-zero default value 
     */
    SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_attributes_reset(unit, core, flow_id));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_sch_flow_composite_id_get(
    int unit,
    int core,
    int base_flow_id,
    int *composite_flow_id)
{

    int flow_region;
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;

    SHR_FUNC_INIT_VARS(unit);

    flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_odd_even_mode_get(unit, core, flow_region, &odd_even_mode));

    if (odd_even_mode == DNX_SCHEDULER_REGION_ODD)
    {
        (*composite_flow_id) = base_flow_id + 1;
    }
    else
    {
        (*composite_flow_id) = base_flow_id + 2;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief get an ID of flow pair
 * used as key for is_composite dbal table
 */
static shr_error_e
dnx_sch_flow_pair_id_get(
    int unit,
    int core_id,
    int base_flow_id,
    int *flow_pair_id)
{
    int region_index;
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;

    SHR_FUNC_INIT_VARS(unit);

    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_odd_even_mode_get(unit, core_id, region_index, &odd_even_mode));

    *flow_pair_id = (odd_even_mode == DNX_SCHEDULER_REGION_ODD ?
                     (base_flow_id & (~0x1)) / 2 : (base_flow_id & (~0x2)) / 2 + base_flow_id % 2);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief Configure is_composite property of flow
 */
shr_error_e
dnx_sch_flow_is_composite_set(
    int unit,
    int core_id,
    int base_flow_id,
    int is_composite)
{
    int flow_pair_id;
    SHR_FUNC_INIT_VARS(unit);

    /** get pair_id -- key to dbal table */
    SHR_IF_ERR_EXIT(dnx_sch_flow_pair_id_get(unit, core_id, base_flow_id, &flow_pair_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_hw_set(unit, core_id, flow_pair_id, is_composite));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Configure is_composite property of flow
 * \note - this function should always be called for composite bundle only
 * either by creating composite bundle (is_composite = 1) or
 * by deleting composite bundle (is_compisite = 0)
 */
shr_error_e
dnx_sch_flow_bundle_is_composite_set(
    int unit,
    int core_id,
    int base_flow_id,
    int nof_flows,
    int is_composite)
{
    int flow_pair_id;
    int is_interdigitated;
    int cur_flow_subflow[2];
    int nof_subflows;
    int cos;
    int region_index;
    int num_pairs;
    int step;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * this function is called in context of composite connectors only
     * this each flow is actually pair of flows
     */
    num_pairs = nof_flows;

    /*
     * is_composite is configured in HW in two places:
     * FSF table - for all flows and 
     * Queue Mapping table (FQM) - for connectors only
     *
     * this function is used for connectors only 
     * and it configures both tables
     */

    /*
     * this function is used for connectors only
     * thus the only regions which are relevant here are
     * connectors only and interdigitated
     */

    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core_id, region_index, &is_interdigitated));

    /*
     * Connectors are created in bundle of 4 or 8
     * is_composite is vector of 16 bits for 16 flow pairs.
     * In order to allow performance optimization and access 4 or 8 bits by single access
     * range_set function is used.
     * However, if the region is interdigitated, 
     * this optimization is not possible, since connector bits are interleaved with SE bits
 */
    if (is_interdigitated)
    {
        /** no optimization - configure each pair separately */
        for (cos = 0; cos < num_pairs; cos++)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core_id,
                                                                 base_flow_id, cos, is_interdigitated, TRUE, FALSE,
                                                                 cur_flow_subflow, &nof_subflows));

            /** get pair_id -- key to dbal table */
            SHR_IF_ERR_EXIT(dnx_sch_flow_pair_id_get(unit, core_id, cur_flow_subflow[0], &flow_pair_id));

            SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_hw_set(unit, core_id, flow_pair_id, is_composite));

        }

    }
    else
    {
        /** use optimization - configure whole range */

        /** get pair_id -- key to dbal table */
        SHR_IF_ERR_EXIT(dnx_sch_flow_pair_id_get(unit, core_id, base_flow_id, &flow_pair_id));

        SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_hw_range_set_fsf(unit, core_id, flow_pair_id, num_pairs,
                                                                   is_composite));
    }

    /*
     * The key to mapping table is flow quartet
     * for composite flow
     * in case of interdigitated - each flow takes whole quartet;
     * in case of non-iterdigitated - each 2 flows take whole quartet
     */
    step = (is_interdigitated ? 1 : 2);

    for (cos = 0; cos < nof_flows; cos += step)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core_id,
                                                             base_flow_id, cos, is_interdigitated, TRUE, FALSE,
                                                             cur_flow_subflow, &nof_subflows));

        SHR_IF_ERR_EXIT(dnx_sch_connector_is_composite_mapping_hw_set(unit, core_id, cur_flow_subflow[0] / 4,
                                                                      is_composite));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Obtain is_composite property of flow
 */
shr_error_e
dnx_sch_flow_is_composite_get(
    int unit,
    int core_id,
    int base_flow_id,
    int *is_composite)
{
    int flow_pair_id;

    SHR_FUNC_INIT_VARS(unit);

    /** get pair_id -- key to dbal table */
    SHR_IF_ERR_EXIT(dnx_sch_flow_pair_id_get(unit, core_id, base_flow_id, &flow_pair_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_hw_get(unit, core_id, flow_pair_id, is_composite));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief returns true iff flow is SE
 */
shr_error_e
dnx_sch_flow_is_se_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_se)
{
    int region_index;
    int quartet_index;
    dnx_sch_region_type_e region_type;

    SHR_FUNC_INIT_VARS(unit);

    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);
    quartet_index = flow_id % DNX_SCHEDULER_QUARTET_SIZE;
    region_type = dnx_data_sch.flow.region_type_get(unit, core_id, region_index)->type;

    *is_se = (dnx_data_sch.sch_alloc.region_get(unit, region_type)->flow_type[quartet_index]
              != DNX_SCH_ELEMENT_SE_TYPE_NONE);

    SHR_FUNC_EXIT;
}

/**
 * \brief Get scheduler element type from flow id.
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] core_id - core ID
 * \param [in] flow_id - flow ID
 * \param [out] se_type - element type
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e
dnx_sch_flow_se_type_get(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_element_se_type_e * se_type)
{
    int region_index;
    int quartet_index;
    dnx_sch_region_type_e region_type;

    SHR_FUNC_INIT_VARS(unit);

    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id);
    quartet_index = flow_id % DNX_SCHEDULER_QUARTET_SIZE;
    region_type = dnx_data_sch.flow.region_type_get(unit, core_id, region_index)->type;

    *se_type = dnx_data_sch.sch_alloc.region_get(unit, region_type)->flow_type[quartet_index];

    SHR_FUNC_EXIT;
}

/**
 * \brief returns true iff flow is port HR
 */
shr_error_e
dnx_sch_flow_is_port_hr_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_port_hr)
{
    int se_id, hr_id;

    SHR_FUNC_INIT_VARS(unit);

    *is_port_hr = FALSE;

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    if (DNX_SCHEDULER_ELEMENT_IS_HR(unit, se_id))
    {
        hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);

        SHR_IF_ERR_EXIT(dnx_sch_hr_is_port_get(unit, core_id, hr_id, is_port_hr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief configure credit source for a given flow
 */
shr_error_e
dnx_sch_flow_credit_src_set(
    int unit,
    int core,
    int child_flow_id,
    int parent_se_id,
    int mode,
    int weight)
{
    dnx_sch_flow_credit_source_t dbal_credit_source;

    int nof_remote_cores;
    int actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /** calculate credit source configuration according to parent element configuration and child attach information */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_credit_src_calc(unit, core, parent_se_id, mode, weight, &dbal_credit_source));

    /** set all symmetric connections */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, child_flow_id,
                                                              actual_flow_id, &nof_remote_cores));

    for (i = 0; i < nof_remote_cores; i++)
    {
        /** attach child to the parent - HW update*/
        SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_hw_set(unit, core, actual_flow_id[i], &dbal_credit_source));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get rate factor for the flow
 */
shr_error_e
dnx_sch_flow_low_rate_factor_get(
    int unit,
    bcm_core_t core,
    int flow_id,
    int *factor)
{
    int flow_range_start, flow_range_end;
    uint32 enabled;

    SHR_FUNC_INIT_VARS(unit);

    *factor = 1;
    SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_get(unit, core, &flow_range_start, &flow_range_end, &enabled));

    if (enabled && UTILEX_IS_IN_RANGE(flow_id, flow_range_start, flow_range_end))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_low_rate_factor_get(unit, factor));
    }

exit:
    SHR_FUNC_EXIT;
}
