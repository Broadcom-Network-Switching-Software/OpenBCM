
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_sch_config_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_sch_config_layout.h>





int
cosq_config_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_COSQ_CONFIG,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]),
        "cosq_config[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_COSQ_CONFIG,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_sch_config_t,
        DNX_SW_STATE_COSQ_CONFIG_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "cosq_config_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_sch_config_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_COSQ_CONFIG,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]),
        "cosq_config[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG,
        cosq_config_info,
        COSQ_CONFIG_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->ipf_config_mode,
        DNX_SCH_IPF_CONFIG_MODE_NONE);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_hr_group_bw_set(int unit, uint32 hr_group_bw_idx_0, uint32 hr_group_bw_idx_1, int hr_group_bw)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        hr_group_bw_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0],
        hr_group_bw_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0][hr_group_bw_idx_1],
        hr_group_bw,
        int,
        0,
        "cosq_config_hr_group_bw_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        &hr_group_bw,
        "cosq_config[%d]->hr_group_bw[%d][%d]",
        unit, hr_group_bw_idx_0, hr_group_bw_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        cosq_config_info,
        COSQ_CONFIG_HR_GROUP_BW_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_hr_group_bw_get(int unit, uint32 hr_group_bw_idx_0, uint32 hr_group_bw_idx_1, int *hr_group_bw)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        hr_group_bw_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        hr_group_bw);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0],
        hr_group_bw_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        DNX_SW_STATE_DIAG_READ);

    *hr_group_bw = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0][hr_group_bw_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0][hr_group_bw_idx_1],
        "cosq_config[%d]->hr_group_bw[%d][%d]",
        unit, hr_group_bw_idx_0, hr_group_bw_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        cosq_config_info,
        COSQ_CONFIG_HR_GROUP_BW_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_hr_group_bw_alloc(int unit, uint32 hr_group_bw_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        hr_group_bw_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0],
        int,
        dnx_data_sch.flow.nof_hr_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "cosq_config_hr_group_bw_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0],
        "cosq_config[%d]->hr_group_bw[%d]",
        unit, hr_group_bw_idx_0,
        dnx_data_sch.flow.nof_hr_get(unit) * sizeof(int) / sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[hr_group_bw_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
        cosq_config_info,
        COSQ_CONFIG_HR_GROUP_BW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_groups_bw_set(int unit, uint32 groups_bw_idx_0, uint32 groups_bw_idx_1, int groups_bw)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        groups_bw_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        groups_bw_idx_1,
        3);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[groups_bw_idx_0][groups_bw_idx_1],
        groups_bw,
        int,
        0,
        "cosq_config_groups_bw_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        &groups_bw,
        "cosq_config[%d]->groups_bw[%d][%d]",
        unit, groups_bw_idx_0, groups_bw_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        cosq_config_info,
        COSQ_CONFIG_GROUPS_BW_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_groups_bw_get(int unit, uint32 groups_bw_idx_0, uint32 groups_bw_idx_1, int *groups_bw)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        groups_bw_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        groups_bw);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        groups_bw_idx_1,
        3);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        DNX_SW_STATE_DIAG_READ);

    *groups_bw = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[groups_bw_idx_0][groups_bw_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[groups_bw_idx_0][groups_bw_idx_1],
        "cosq_config[%d]->groups_bw[%d][%d]",
        unit, groups_bw_idx_0, groups_bw_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
        cosq_config_info,
        COSQ_CONFIG_GROUPS_BW_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_ipf_config_mode_set(int unit, dnx_sch_ipf_config_mode_e ipf_config_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->ipf_config_mode,
        ipf_config_mode,
        dnx_sch_ipf_config_mode_e,
        0,
        "cosq_config_ipf_config_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        &ipf_config_mode,
        "cosq_config[%d]->ipf_config_mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        cosq_config_info,
        COSQ_CONFIG_IPF_CONFIG_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_ipf_config_mode_get(int unit, dnx_sch_ipf_config_mode_e *ipf_config_mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        ipf_config_mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        DNX_SW_STATE_DIAG_READ);

    *ipf_config_mode = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->ipf_config_mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->ipf_config_mode,
        "cosq_config[%d]->ipf_config_mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
        cosq_config_info,
        COSQ_CONFIG_IPF_CONFIG_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_alloc(int unit, uint32 flow_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        dnx_sch_flow_swstate_info_t,
        dnx_data_sch.flow.nof_flows_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "cosq_config_flow_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_COSQ_CONFIG__FLOW,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        "cosq_config[%d]->flow[%d]",
        unit, flow_idx_0,
        dnx_data_sch.flow.nof_flows_get(unit) * sizeof(dnx_sch_flow_swstate_info_t) / sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW,
        cosq_config_info,
        COSQ_CONFIG_FLOW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_set(int unit, uint32 flow_idx_0, uint32 flow_idx_1, CONST dnx_sch_credit_src_info_t *credit_src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src,
        credit_src,
        dnx_sch_credit_src_info_t,
        0,
        "cosq_config_flow_credit_src_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        credit_src,
        "cosq_config[%d]->flow[%d][%d].credit_src",
        unit, flow_idx_0, flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        cosq_config_info,
        COSQ_CONFIG_FLOW_CREDIT_SRC_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_get(int unit, uint32 flow_idx_0, uint32 flow_idx_1, dnx_sch_credit_src_info_t *credit_src)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        credit_src);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        flow_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        DNX_SW_STATE_DIAG_READ);

    *credit_src = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src,
        "cosq_config[%d]->flow[%d][%d].credit_src",
        unit, flow_idx_0, flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
        cosq_config_info,
        COSQ_CONFIG_FLOW_CREDIT_SRC_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_weight_set(int unit, uint32 flow_idx_0, uint32 flow_idx_1, int weight)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src.weight,
        weight,
        int,
        0,
        "cosq_config_flow_credit_src_weight_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        &weight,
        "cosq_config[%d]->flow[%d][%d].credit_src.weight",
        unit, flow_idx_0, flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        cosq_config_info,
        COSQ_CONFIG_FLOW_CREDIT_SRC_WEIGHT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_weight_get(int unit, uint32 flow_idx_0, uint32 flow_idx_1, int *weight)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        weight);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        flow_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        DNX_SW_STATE_DIAG_READ);

    *weight = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src.weight;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src.weight,
        "cosq_config[%d]->flow[%d][%d].credit_src.weight",
        unit, flow_idx_0, flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
        cosq_config_info,
        COSQ_CONFIG_FLOW_CREDIT_SRC_WEIGHT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_mode_set(int unit, uint32 flow_idx_0, uint32 flow_idx_1, int mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src.mode,
        mode,
        int,
        0,
        "cosq_config_flow_credit_src_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        &mode,
        "cosq_config[%d]->flow[%d][%d].credit_src.mode",
        unit, flow_idx_0, flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        cosq_config_info,
        COSQ_CONFIG_FLOW_CREDIT_SRC_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_mode_get(int unit, uint32 flow_idx_0, uint32 flow_idx_1, int *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        flow_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        mode);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0],
        flow_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        DNX_SW_STATE_DIAG_READ);

    *mode = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[flow_idx_0][flow_idx_1].credit_src.mode,
        "cosq_config[%d]->flow[%d][%d].credit_src.mode",
        unit, flow_idx_0, flow_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
        cosq_config_info,
        COSQ_CONFIG_FLOW_CREDIT_SRC_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_set(int unit, uint32 connector_idx_0, uint32 connector_idx_1, CONST dnx_sch_connector_swstate_info_t *connector)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1],
        connector,
        dnx_sch_connector_swstate_info_t,
        0,
        "cosq_config_connector_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        connector,
        "cosq_config[%d]->connector[%d][%d]",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_get(int unit, uint32 connector_idx_0, uint32 connector_idx_1, dnx_sch_connector_swstate_info_t *connector)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        connector);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        DNX_SW_STATE_DIAG_READ);

    *connector = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1],
        "cosq_config[%d]->connector[%d][%d]",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_alloc(int unit, uint32 connector_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        dnx_sch_connector_swstate_info_t,
        dnx_data_sch.flow.nof_flows_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "cosq_config_connector_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        "cosq_config[%d]->connector[%d]",
        unit, connector_idx_0,
        dnx_data_sch.flow.nof_flows_get(unit) * sizeof(dnx_sch_connector_swstate_info_t) / sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_num_cos_set(int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 num_cos)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].num_cos,
        num_cos,
        uint8,
        0,
        "cosq_config_connector_num_cos_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        &num_cos,
        "cosq_config[%d]->connector[%d][%d].num_cos",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_NUM_COS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_num_cos_get(int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 *num_cos)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        num_cos);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        DNX_SW_STATE_DIAG_READ);

    *num_cos = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].num_cos;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].num_cos,
        "cosq_config[%d]->connector[%d][%d].num_cos",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_NUM_COS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_connection_valid_set(int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 connection_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].connection_valid,
        connection_valid,
        uint8,
        0,
        "cosq_config_connector_connection_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        &connection_valid,
        "cosq_config[%d]->connector[%d][%d].connection_valid",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_CONNECTION_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_connection_valid_get(int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 *connection_valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        connection_valid);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        DNX_SW_STATE_DIAG_READ);

    *connection_valid = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].connection_valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].connection_valid,
        "cosq_config[%d]->connector[%d][%d].connection_valid",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_CONNECTION_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_src_modid_set(int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 src_modid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].src_modid,
        src_modid,
        uint8,
        0,
        "cosq_config_connector_src_modid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        &src_modid,
        "cosq_config[%d]->connector[%d][%d].src_modid",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_SRC_MODID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_src_modid_get(int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 *src_modid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        connector_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        src_modid);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0],
        connector_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        DNX_SW_STATE_DIAG_READ);

    *src_modid = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].src_modid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[connector_idx_0][connector_idx_1].src_modid,
        "cosq_config[%d]->connector[%d][%d].src_modid",
        unit, connector_idx_0, connector_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
        cosq_config_info,
        COSQ_CONFIG_CONNECTOR_SRC_MODID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_se_alloc(int unit, uint32 se_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        se_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0],
        dnx_sch_se_swstate_info_t,
        dnx_data_sch.flow.nof_se_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "cosq_config_se_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_COSQ_CONFIG__SE,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0],
        "cosq_config[%d]->se[%d]",
        unit, se_idx_0,
        dnx_data_sch.flow.nof_se_get(unit) * sizeof(dnx_sch_se_swstate_info_t) / sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE,
        cosq_config_info,
        COSQ_CONFIG_SE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_se_child_count_set(int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 child_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        se_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0],
        se_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count,
        child_count,
        uint32,
        0,
        "cosq_config_se_child_count_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        &child_count,
        "cosq_config[%d]->se[%d][%d].child_count",
        unit, se_idx_0, se_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        cosq_config_info,
        COSQ_CONFIG_SE_CHILD_COUNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_se_child_count_get(int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 *child_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        se_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        child_count);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0],
        se_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        DNX_SW_STATE_DIAG_READ);

    *child_count = ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count,
        "cosq_config[%d]->se[%d][%d].child_count",
        unit, se_idx_0, se_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        cosq_config_info,
        COSQ_CONFIG_SE_CHILD_COUNT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_se_child_count_inc(int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        se_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0],
        se_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count,
        inc_value,
        uint32,
        0,
        "cosq_config_se_child_count_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count,
        "cosq_config[%d]->se[%d][%d].child_count",
        unit, se_idx_0, se_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        cosq_config_info,
        COSQ_CONFIG_SE_CHILD_COUNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_se_child_count_dec(int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        se_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0],
        se_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count,
        dec_value,
        uint32,
        0,
        "cosq_config_se_child_count_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[se_idx_0][se_idx_1].child_count,
        "cosq_config[%d]->se[%d][%d].child_count",
        unit, se_idx_0, se_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
        cosq_config_info,
        COSQ_CONFIG_SE_CHILD_COUNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




const char *
dnx_sch_ipf_config_mode_e_get_name(dnx_sch_ipf_config_mode_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_SCH_IPF_CONFIG_MODE_INVERSE", value, DNX_SCH_IPF_CONFIG_MODE_INVERSE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_SCH_IPF_CONFIG_MODE_PROPORTIONAL", value, DNX_SCH_IPF_CONFIG_MODE_PROPORTIONAL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_SCH_IPF_CONFIG_MODE_NONE", value, DNX_SCH_IPF_CONFIG_MODE_NONE);

    return NULL;
}





cosq_config_cbs cosq_config = 	{
	
	cosq_config_is_init,
	cosq_config_init,
		{
		
		cosq_config_hr_group_bw_set,
		cosq_config_hr_group_bw_get,
		cosq_config_hr_group_bw_alloc}
	,
		{
		
		cosq_config_groups_bw_set,
		cosq_config_groups_bw_get}
	,
		{
		
		cosq_config_ipf_config_mode_set,
		cosq_config_ipf_config_mode_get}
	,
		{
		
		cosq_config_flow_alloc,
			{
			
			cosq_config_flow_credit_src_set,
			cosq_config_flow_credit_src_get,
				{
				
				cosq_config_flow_credit_src_weight_set,
				cosq_config_flow_credit_src_weight_get}
			,
				{
				
				cosq_config_flow_credit_src_mode_set,
				cosq_config_flow_credit_src_mode_get}
			}
		}
	,
		{
		
		cosq_config_connector_set,
		cosq_config_connector_get,
		cosq_config_connector_alloc,
			{
			
			cosq_config_connector_num_cos_set,
			cosq_config_connector_num_cos_get}
		,
			{
			
			cosq_config_connector_connection_valid_set,
			cosq_config_connector_connection_valid_get}
		,
			{
			
			cosq_config_connector_src_modid_set,
			cosq_config_connector_src_modid_get}
		}
	,
		{
		
		cosq_config_se_alloc,
			{
			
			cosq_config_se_child_count_set,
			cosq_config_se_child_count_get,
			cosq_config_se_child_count_inc,
			cosq_config_se_child_count_dec}
		}
	}
;
#undef BSL_LOG_MODULE
