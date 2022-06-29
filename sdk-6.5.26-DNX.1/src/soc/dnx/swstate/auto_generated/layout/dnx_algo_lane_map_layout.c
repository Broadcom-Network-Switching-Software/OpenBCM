
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_lane_map_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_algo_lane_map_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_algo_lane_map[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB_NOF_PARAMS)];

shr_error_e
dnx_algo_lane_map_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db";
    layout_array_dnx_algo_lane_map[idx].type = "dnx_algo_lane_map_db_t";
    layout_array_dnx_algo_lane_map[idx].doc = "Lane Mapping Data Base";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(dnx_algo_lane_map_db_t);
    layout_array_dnx_algo_lane_map[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__FIRST);
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map";
    layout_array_dnx_algo_lane_map[idx].type = "soc_dnxc_lane_map_db_t";
    layout_array_dnx_algo_lane_map[idx].doc = "NIF and Fabric lane map database";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(soc_dnxc_lane_map_db_t);
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].num_elements = DNX_ALGO_NOF_LANE_MAP_TYPES;
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_lane_map[idx].parent  = 0;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__FIRST);
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__otn";
    layout_array_dnx_algo_lane_map[idx].type = "dnx_algo_lane_map_db_otn_t";
    layout_array_dnx_algo_lane_map[idx].doc = "OTN DBs";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(dnx_algo_lane_map_db_otn_t);
    layout_array_dnx_algo_lane_map[idx].parent  = 0;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__FIRST);
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__map_size";
    layout_array_dnx_algo_lane_map[idx].type = "int";
    layout_array_dnx_algo_lane_map[idx].doc = "nof lanes represented by the map database";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(int);
    layout_array_dnx_algo_lane_map[idx].parent  = 1;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__lane2serdes";
    layout_array_dnx_algo_lane_map[idx].type = "soc_dnxc_lane_map_db_map_t*";
    layout_array_dnx_algo_lane_map[idx].doc = "defines serdes rx and tx for each lane";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(soc_dnxc_lane_map_db_map_t*);
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_algo_lane_map[idx].parent  = 1;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__FIRST);
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__LAST)-1;
    layout_array_dnx_algo_lane_map[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__serdes2lane";
    layout_array_dnx_algo_lane_map[idx].type = "soc_dnxc_lane_map_db_map_t*";
    layout_array_dnx_algo_lane_map[idx].doc = "defines lane rx and tx for each serdes";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(soc_dnxc_lane_map_db_map_t*);
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_algo_lane_map[idx].parent  = 1;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__FIRST);
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__LAST)-1;
    layout_array_dnx_algo_lane_map[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__lane2serdes__rx_id";
    layout_array_dnx_algo_lane_map[idx].type = "int";
    layout_array_dnx_algo_lane_map[idx].doc = "lane is mapped to this serdes rx or serdes is mapped to this lane rx";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(int);
    layout_array_dnx_algo_lane_map[idx].parent  = 4;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__lane2serdes__tx_id";
    layout_array_dnx_algo_lane_map[idx].type = "int";
    layout_array_dnx_algo_lane_map[idx].doc = "lane is mapped to this serdes tx or serdes is mapped to this lane tx";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(int);
    layout_array_dnx_algo_lane_map[idx].parent  = 4;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__serdes2lane__rx_id";
    layout_array_dnx_algo_lane_map[idx].type = "int";
    layout_array_dnx_algo_lane_map[idx].doc = "lane is mapped to this serdes rx or serdes is mapped to this lane rx";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(int);
    layout_array_dnx_algo_lane_map[idx].parent  = 5;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__lane_map__serdes2lane__tx_id";
    layout_array_dnx_algo_lane_map[idx].type = "int";
    layout_array_dnx_algo_lane_map[idx].doc = "lane is mapped to this serdes tx or serdes is mapped to this lane tx";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(int);
    layout_array_dnx_algo_lane_map[idx].parent  = 5;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__otn__logical_lanes";
    layout_array_dnx_algo_lane_map[idx].type = "bcm_pbmp_t";
    layout_array_dnx_algo_lane_map[idx].doc = "framer logical lane bitmap";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(bcm_pbmp_t);
    layout_array_dnx_algo_lane_map[idx].parent  = 2;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__otn__logical_lane_map";
    layout_array_dnx_algo_lane_map[idx].type = "dnx_algo_lane_map_db_otn_logical_lane_map_t*";
    layout_array_dnx_algo_lane_map[idx].doc = "The logical otn lane map info for logical ports";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(dnx_algo_lane_map_db_otn_logical_lane_map_t*);
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_algo_lane_map[idx].parent  = 2;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__FIRST);
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__otn__logical_lane_map__logical_otn_lane";
    layout_array_dnx_algo_lane_map[idx].type = "uint16";
    layout_array_dnx_algo_lane_map[idx].doc = "The first logical otn lane for given port";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(uint16);
    layout_array_dnx_algo_lane_map[idx].parent  = 11;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES);
    layout_array_dnx_algo_lane_map[idx].name = "dnx_algo_lane_map_db__otn__logical_lane_map__nof_lanes";
    layout_array_dnx_algo_lane_map[idx].type = "uint16";
    layout_array_dnx_algo_lane_map[idx].doc = "The number of logical otn lanes for given port";
    layout_array_dnx_algo_lane_map[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].size_of = sizeof(uint16);
    layout_array_dnx_algo_lane_map[idx].parent  = 11;
    layout_array_dnx_algo_lane_map[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_lane_map[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB, layout_array_dnx_algo_lane_map, sw_state_layout_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID], DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
