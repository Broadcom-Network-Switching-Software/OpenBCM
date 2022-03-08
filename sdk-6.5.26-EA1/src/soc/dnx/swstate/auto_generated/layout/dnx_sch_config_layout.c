
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_sch_config_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_sch_config_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_sch_config[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG_NOF_PARAMS)];

shr_error_e
dnx_sch_config_init_layout_structure(int unit)
{

    dnx_sch_ipf_config_mode_e cosq_config__ipf_config_mode__default_val = DNX_SCH_IPF_CONFIG_MODE_NONE;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG);
    layout_array_dnx_sch_config[idx].name = "cosq_config";
    layout_array_dnx_sch_config[idx].type = "dnx_sch_config_t";
    layout_array_dnx_sch_config[idx].doc = "legacy cosq config info";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(dnx_sch_config_t);
    layout_array_dnx_sch_config[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].first_child_index = 1;
    layout_array_dnx_sch_config[idx].last_child_index = 6;
    layout_array_dnx_sch_config[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW);
    layout_array_dnx_sch_config[idx].name = "cosq_config__hr_group_bw";
    layout_array_dnx_sch_config[idx].type = "int*";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(int*);
    layout_array_dnx_sch_config[idx].array_indexes[0].size = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_sch_config[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_sch_config[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_sch_config[idx].parent  = 0;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW);
    layout_array_dnx_sch_config[idx].name = "cosq_config__groups_bw";
    layout_array_dnx_sch_config[idx].type = "int";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(int);
    layout_array_dnx_sch_config[idx].array_indexes[0].size = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_sch_config[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_sch_config[idx].array_indexes[1].size = 3;
    layout_array_dnx_sch_config[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_sch_config[idx].parent  = 0;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE);
    layout_array_dnx_sch_config[idx].name = "cosq_config__ipf_config_mode";
    layout_array_dnx_sch_config[idx].type = "dnx_sch_ipf_config_mode_e";
    layout_array_dnx_sch_config[idx].default_value= &(cosq_config__ipf_config_mode__default_val);
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(dnx_sch_ipf_config_mode_e);
    layout_array_dnx_sch_config[idx].parent  = 0;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = 4;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__FLOW);
    layout_array_dnx_sch_config[idx].name = "cosq_config__flow";
    layout_array_dnx_sch_config[idx].type = "dnx_sch_flow_swstate_info_t*";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(dnx_sch_flow_swstate_info_t*);
    layout_array_dnx_sch_config[idx].array_indexes[0].size = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_sch_config[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_sch_config[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_sch_config[idx].parent  = 0;
    layout_array_dnx_sch_config[idx].first_child_index = 7;
    layout_array_dnx_sch_config[idx].last_child_index = 7;
    layout_array_dnx_sch_config[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__CONNECTOR);
    layout_array_dnx_sch_config[idx].name = "cosq_config__connector";
    layout_array_dnx_sch_config[idx].type = "dnx_sch_connector_swstate_info_t*";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(dnx_sch_connector_swstate_info_t*);
    layout_array_dnx_sch_config[idx].array_indexes[0].size = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_sch_config[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_sch_config[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_sch_config[idx].parent  = 0;
    layout_array_dnx_sch_config[idx].first_child_index = 10;
    layout_array_dnx_sch_config[idx].last_child_index = 12;
    layout_array_dnx_sch_config[idx].next_node_index = 6;
    layout_array_dnx_sch_config[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__SE);
    layout_array_dnx_sch_config[idx].name = "cosq_config__se";
    layout_array_dnx_sch_config[idx].type = "dnx_sch_se_swstate_info_t*";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(dnx_sch_se_swstate_info_t*);
    layout_array_dnx_sch_config[idx].array_indexes[0].size = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_sch_config[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_sch_config[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_sch_config[idx].parent  = 0;
    layout_array_dnx_sch_config[idx].first_child_index = 13;
    layout_array_dnx_sch_config[idx].last_child_index = 13;
    layout_array_dnx_sch_config[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC);
    layout_array_dnx_sch_config[idx].name = "cosq_config__flow__credit_src";
    layout_array_dnx_sch_config[idx].type = "dnx_sch_credit_src_info_t";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(dnx_sch_credit_src_info_t);
    layout_array_dnx_sch_config[idx].parent  = 4;
    layout_array_dnx_sch_config[idx].first_child_index = 8;
    layout_array_dnx_sch_config[idx].last_child_index = 9;
    layout_array_dnx_sch_config[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT);
    layout_array_dnx_sch_config[idx].name = "cosq_config__flow__credit_src__weight";
    layout_array_dnx_sch_config[idx].type = "int";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(int);
    layout_array_dnx_sch_config[idx].parent  = 7;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = 9;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE);
    layout_array_dnx_sch_config[idx].name = "cosq_config__flow__credit_src__mode";
    layout_array_dnx_sch_config[idx].type = "int";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(int);
    layout_array_dnx_sch_config[idx].parent  = 7;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS);
    layout_array_dnx_sch_config[idx].name = "cosq_config__connector__num_cos";
    layout_array_dnx_sch_config[idx].type = "uint8";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(uint8);
    layout_array_dnx_sch_config[idx].parent  = 5;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = 11;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID);
    layout_array_dnx_sch_config[idx].name = "cosq_config__connector__connection_valid";
    layout_array_dnx_sch_config[idx].type = "uint8";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(uint8);
    layout_array_dnx_sch_config[idx].parent  = 5;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = 12;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID);
    layout_array_dnx_sch_config[idx].name = "cosq_config__connector__src_modid";
    layout_array_dnx_sch_config[idx].type = "uint8";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(uint8);
    layout_array_dnx_sch_config[idx].parent  = 5;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT);
    layout_array_dnx_sch_config[idx].name = "cosq_config__se__child_count";
    layout_array_dnx_sch_config[idx].type = "uint32";
    layout_array_dnx_sch_config[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].size_of = sizeof(uint32);
    layout_array_dnx_sch_config[idx].parent  = 6;
    layout_array_dnx_sch_config[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_sch_config[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_COSQ_CONFIG, layout_array_dnx_sch_config, sw_state_layout_array[unit][DNX_SCH_CONFIG_MODULE_ID], DNX_SW_STATE_COSQ_CONFIG_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
