
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_ipq_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_ipq_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_ipq[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IPQ_DB_NOF_PARAMS)];

shr_error_e
dnx_ipq_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IPQ_DB);
    layout_array_dnx_ipq[idx].name = "dnx_ipq_db";
    layout_array_dnx_ipq[idx].type = "dnx_ipq_db_t";
    layout_array_dnx_ipq[idx].doc = "DB for Ingress Packet Queuing";
    layout_array_dnx_ipq[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].size_of = sizeof(dnx_ipq_db_t);
    layout_array_dnx_ipq[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].first_child_index = 1;
    layout_array_dnx_ipq[idx].last_child_index = 2;
    layout_array_dnx_ipq[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IPQ_DB__BASE_QUEUES);
    layout_array_dnx_ipq[idx].name = "dnx_ipq_db__base_queues";
    layout_array_dnx_ipq[idx].type = "dnx_ipq_base_queue_t**";
    layout_array_dnx_ipq[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].size_of = sizeof(dnx_ipq_base_queue_t**);
    layout_array_dnx_ipq[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_ipq[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_ipq[idx].parent  = 0;
    layout_array_dnx_ipq[idx].first_child_index = 3;
    layout_array_dnx_ipq[idx].last_child_index = 4;
    layout_array_dnx_ipq[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IPQ_DB__BASE_QUEUE_IS_ASYMM);
    layout_array_dnx_ipq[idx].name = "dnx_ipq_db__base_queue_is_asymm";
    layout_array_dnx_ipq[idx].type = "SHR_BITDCL*";
    layout_array_dnx_ipq[idx].doc = "Bitmap to indicate that the base queue is asymmetric.";
    layout_array_dnx_ipq[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnx_ipq[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_dnx_ipq[idx].parent  = 0;
    layout_array_dnx_ipq[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IPQ_DB__BASE_QUEUES__NUM_COS);
    layout_array_dnx_ipq[idx].name = "dnx_ipq_db__base_queues__num_cos";
    layout_array_dnx_ipq[idx].type = "uint8";
    layout_array_dnx_ipq[idx].doc = "Number of COS";
    layout_array_dnx_ipq[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].size_of = sizeof(uint8);
    layout_array_dnx_ipq[idx].parent  = 1;
    layout_array_dnx_ipq[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].next_node_index = 4;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IPQ_DB__BASE_QUEUES__SYS_PORT_REF_COUNTER);
    layout_array_dnx_ipq[idx].name = "dnx_ipq_db__base_queues__sys_port_ref_counter";
    layout_array_dnx_ipq[idx].type = "int";
    layout_array_dnx_ipq[idx].doc = "Reference counter for the number of systemeports that a re mapped to this VOQ";
    layout_array_dnx_ipq[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].size_of = sizeof(int);
    layout_array_dnx_ipq[idx].parent  = 1;
    layout_array_dnx_ipq[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ipq[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_IPQ_DB, layout_array_dnx_ipq, sw_state_layout_array[unit][DNX_IPQ_MODULE_ID], DNX_SW_STATE_DNX_IPQ_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
