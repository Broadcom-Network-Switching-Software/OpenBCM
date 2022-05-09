
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_egr_db_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_egr_db_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_egr_db[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB_NOF_PARAMS)];

shr_error_e
dnx_egr_db_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db";
    layout_array_dnx_egr_db[idx].type = "dnx_egr_db_t";
    layout_array_dnx_egr_db[idx].doc = "info about EGR module";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(dnx_egr_db_t);
    layout_array_dnx_egr_db[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__FIRST);
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__PS);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__ps";
    layout_array_dnx_egr_db[idx].type = "dnx_ps_db_t";
    layout_array_dnx_egr_db[idx].doc = "ps information";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(dnx_ps_db_t);
    layout_array_dnx_egr_db[idx].parent  = 0;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__PS__FIRST);
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__PS__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__INTERFACE_OCC);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__interface_occ";
    layout_array_dnx_egr_db[idx].type = "int";
    layout_array_dnx_egr_db[idx].doc = "Boolean. egress interface occupation";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(int);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES;
    layout_array_dnx_egr_db[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 0;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__TOTAL_EGR_IF_CREDITS);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__total_egr_if_credits";
    layout_array_dnx_egr_db[idx].type = "int";
    layout_array_dnx_egr_db[idx].doc = "Total number of credits in all egress interfaces per core";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(int);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 0;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__EGR_IF_CREDITS_CONFIGURED);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__egr_if_credits_configured";
    layout_array_dnx_egr_db[idx].type = "uint8";
    layout_array_dnx_egr_db[idx].doc = "Boolean. Indication that Egress interface's credits have been configured.";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(uint8);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES;
    layout_array_dnx_egr_db[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 0;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__TOTAL_ESB_QUEUES_ALLOCATED);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__total_esb_queues_allocated";
    layout_array_dnx_egr_db[idx].type = "int";
    layout_array_dnx_egr_db[idx].doc = "Total number of ESB queues allocated per core";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(int);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 0;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__NOF_NEGATIVE_COMPENSATION_PORTS);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__nof_negative_compensation_ports";
    layout_array_dnx_egr_db[idx].type = "int";
    layout_array_dnx_egr_db[idx].doc = "Total number of ports with negative compensation per core";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(int);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 0;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__PS__ALLOCATION_BMAP);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__ps__allocation_bmap";
    layout_array_dnx_egr_db[idx].type = "uint32";
    layout_array_dnx_egr_db[idx].doc = "ps allocation bitmap";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(uint32);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS;
    layout_array_dnx_egr_db[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 1;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__PS__PRIO_MODE);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__ps__prio_mode";
    layout_array_dnx_egr_db[idx].type = "uint32";
    layout_array_dnx_egr_db[idx].doc = "priority mode 1/2/4/8";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(uint32);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS;
    layout_array_dnx_egr_db[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 1;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EGR_DB__PS__IF_IDX);
    layout_array_dnx_egr_db[idx].name = "dnx_egr_db__ps__if_idx";
    layout_array_dnx_egr_db[idx].type = "int";
    layout_array_dnx_egr_db[idx].doc = "interface index 0-63 for channelized. -1 for non channelized";
    layout_array_dnx_egr_db[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].size_of = sizeof(int);
    layout_array_dnx_egr_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_egr_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS;
    layout_array_dnx_egr_db[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_egr_db[idx].parent  = 1;
    layout_array_dnx_egr_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_egr_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_EGR_DB, layout_array_dnx_egr_db, sw_state_layout_array[unit][DNX_EGR_DB_MODULE_ID], DNX_SW_STATE_DNX_EGR_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
