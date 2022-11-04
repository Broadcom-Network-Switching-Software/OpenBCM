
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/types/adapter_types.h>
#include <soc/dnxc/swstate/auto_generated/layout/adapter_layout.h>


extern dnxc_sw_state_layout_t* adapter_layout_root[SOC_MAX_NUM_DEVICES];



dnxc_sw_state_layout_t layout_array_adapter[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB_NOF_PARAMS)];

shr_error_e
adapter_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB);
    layout_array_adapter[idx].name = "adapter_context_db";
    layout_array_adapter[idx].type = "adapter_basic_access_info_t";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(adapter_basic_access_info_t);
    layout_array_adapter[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__FIRST);
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS);
    layout_array_adapter[idx].name = "adapter_context_db__params";
    layout_array_adapter[idx].type = "dnxc_adapter_context_params_t";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(dnxc_adapter_context_params_t);
    layout_array_adapter[idx].array_indexes[0].num_elements = 2;
    layout_array_adapter[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_adapter[idx].parent  = 0;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__FIRST);
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_RX_TX_FD);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_rx_tx_fd";
    layout_array_adapter[idx].type = "int";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(int);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_MEM_REG_FD);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_mem_reg_fd";
    layout_array_adapter[idx].type = "int";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(int);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_SDK_INTERFACE_FD);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_sdk_interface_fd";
    layout_array_adapter[idx].type = "int";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(int);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_EXTERNAL_EVENTS_FD);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_external_events_fd";
    layout_array_adapter[idx].type = "int";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(int);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_EXTERNAL_EVENTS_PORT);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_external_events_port";
    layout_array_adapter[idx].type = "uint32";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(uint32);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_SERVER_ADDRESS);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_server_address";
    layout_array_adapter[idx].type = "uint32";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(uint32);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_RX_TX_MUTEX);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_rx_tx_mutex";
    layout_array_adapter[idx].type = "sw_state_mutex_t";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_MEM_REG_MUTEX);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_mem_reg_mutex";
    layout_array_adapter[idx].type = "sw_state_mutex_t";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_SDK_INTERFACE_MUTEX);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_sdk_interface_mutex";
    layout_array_adapter[idx].type = "sw_state_mutex_t";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_ADAPTER_CONTEXT_DB__PARAMS__ADAPTER_EXTERNAL_EVENTS_MUTEX);
    layout_array_adapter[idx].name = "adapter_context_db__params__adapter_external_events_mutex";
    layout_array_adapter[idx].type = "sw_state_mutex_t";
    layout_array_adapter[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_adapter[idx].parent  = 1;
    layout_array_adapter[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_adapter[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_ADAPTER_CONTEXT_DB, layout_array_adapter, adapter_layout_root[unit], DNX_SW_STATE_ADAPTER_CONTEXT_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
