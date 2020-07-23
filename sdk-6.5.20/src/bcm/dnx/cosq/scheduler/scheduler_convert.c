

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
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include "scheduler_convert.h"
#include "scheduler_element.h"




shr_error_e
dnx_sch_convert_flow_to_se_id(
    int unit,
    int flow_id,
    int *se_id)
{
    int flow_id_in_quartet, shifted_flow_id, quartet_id;

    int type0_end = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) * dnx_data_sch.flow.region_size_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(flow_id, type0_end, dnx_data_sch.flow.nof_flows_get(unit) - 1, _SHR_E_PARAM,
                     "flow id %d in not in appropriate range", flow_id);

    shifted_flow_id = flow_id - type0_end;
    quartet_id = shifted_flow_id / DNX_SCHEDULER_QUARTET_SIZE;
    flow_id_in_quartet = shifted_flow_id % DNX_SCHEDULER_QUARTET_SIZE;

    *se_id = (dnx_data_sch.flow.nof_se_get(unit) / DNX_SCHEDULER_QUARTET_SIZE) * flow_id_in_quartet + quartet_id;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_sch_convert_se_to_flow_id(
    int unit,
    int se_id,
    int *flow_id)
{

    int type0_end = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) * dnx_data_sch.flow.region_size_get(unit);
    int single_se_type_size = (dnx_data_sch.flow.nof_se_get(unit) / DNX_SCHEDULER_QUARTET_SIZE);

    int flow_id_in_quartet, shifted_flow_id, quartet_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(se_id, 0, dnx_data_sch.flow.nof_se_get(unit) - 1, _SHR_E_PARAM, "se id %d is invalid", se_id);

    flow_id_in_quartet = se_id / single_se_type_size;
    quartet_id = se_id % single_se_type_size;
    shifted_flow_id = quartet_id * DNX_SCHEDULER_QUARTET_SIZE + flow_id_in_quartet;
    *flow_id = type0_end + shifted_flow_id;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_sch_convert_port_tc_to_se_id(
    int unit,
    bcm_port_t logical_port,
    int tc,
    int *core,
    int *se_id)
{
    int base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, core));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    if (tc >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid  TC %d\n", tc);
    }
    else
    {
        *se_id = base_hr + tc + dnx_data_sch.flow.hr_se_id_min_get(unit);
    }

exit:
    SHR_FUNC_EXIT;
}


int
dnx_sch_convert_se_id_to_port_tc(
    int unit,
    int core,
    int se_id,
    bcm_port_t * logical_port,
    int *tc)
{
    int base_hr, hr_id;
    int is_port;

    SHR_FUNC_INIT_VARS(unit);

    *logical_port = DNX_ALGO_PORT_INVALID;
    *tc = 0;

    if (DNX_SCHEDULER_ELEMENT_IS_HR(unit, se_id))
    {
        hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);

        
        SHR_IF_ERR_EXIT(dnx_sch_hr_is_port_get(unit, core, hr_id, &is_port));

        if (is_port)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_port_hr_to_logical_get(unit, core, hr_id, logical_port));

            SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, *logical_port, &base_hr));

            *tc = hr_id - base_hr;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
