
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <soc/dnx/swstate/auto_generated/layout/flow_layout.h>

dnxc_sw_state_layout_t layout_array_flow[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB_NOF_PARAMS)];

shr_error_e
flow_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB);
    layout_array_flow[idx].name = "flow_db";
    layout_array_flow[idx].type = "flow_sw_state_t";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(flow_sw_state_t);
    layout_array_flow[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__FIRST);
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO);
    layout_array_flow[idx].name = "flow_db__flow_application_info";
    layout_array_flow[idx].type = "dnx_flow_app_common_config_t";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(dnx_flow_app_common_config_t);
    layout_array_flow[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_flow[idx].parent  = 0;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__FIRST);
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__LAST)-1;
    layout_array_flow[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE);
    layout_array_flow[idx].name = "flow_db__valid_phases_per_dbal_table";
    layout_array_flow[idx].type = "sw_state_htbl_t";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_flow[idx].parent  = 0;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES);
    layout_array_flow[idx].name = "flow_db__dbal_table_to_valid_result_types";
    layout_array_flow[idx].type = "sw_state_htbl_t";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_flow[idx].parent  = 0;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB);
    layout_array_flow[idx].name = "flow_db__ingress_gport_to_match_info_htb";
    layout_array_flow[idx].type = "sw_state_htbl_t";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_flow[idx].parent  = 0;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED);
    layout_array_flow[idx].name = "flow_db__flow_application_info__is_verify_disabled";
    layout_array_flow[idx].type = "uint8";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(uint8);
    layout_array_flow[idx].parent  = 1;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED);
    layout_array_flow[idx].name = "flow_db__flow_application_info__is_error_recovery_disabled";
    layout_array_flow[idx].type = "uint8";
    layout_array_flow[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].size_of = sizeof(uint8);
    layout_array_flow[idx].parent  = 1;
    layout_array_flow[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flow[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_FLOW_DB, layout_array_flow, sw_state_layout_array[unit][FLOW_MODULE_ID], DNX_SW_STATE_FLOW_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
