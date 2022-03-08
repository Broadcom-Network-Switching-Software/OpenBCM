
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/oam_types.h>
#include <soc/dnx/swstate/auto_generated/layout/oam_layout.h>

dnxc_sw_state_layout_t layout_array_oam[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO_NOF_PARAMS)];

shr_error_e
oam_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO);
    layout_array_oam[idx].name = "oam_sw_db_info";
    layout_array_oam[idx].type = "oam_sw_db_info_t";
    layout_array_oam[idx].doc = "A struct used to hold OAM SW info";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(oam_sw_db_info_t);
    layout_array_oam[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].first_child_index = 1;
    layout_array_oam[idx].last_child_index = 3;
    layout_array_oam[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO);
    layout_array_oam[idx].name = "oam_sw_db_info__oam_group_sw_db_info";
    layout_array_oam[idx].type = "oam_group_sw_db_info_t";
    layout_array_oam[idx].doc = "OAM group SW db info";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(oam_group_sw_db_info_t);
    layout_array_oam[idx].parent  = 0;
    layout_array_oam[idx].first_child_index = 4;
    layout_array_oam[idx].last_child_index = 4;
    layout_array_oam[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO);
    layout_array_oam[idx].name = "oam_sw_db_info__oam_endpoint_sw_db_info";
    layout_array_oam[idx].type = "oam_endpoint_sw_db_info_t";
    layout_array_oam[idx].doc = "OAM endpoint SW db info";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(oam_endpoint_sw_db_info_t);
    layout_array_oam[idx].parent  = 0;
    layout_array_oam[idx].first_child_index = 5;
    layout_array_oam[idx].last_child_index = 6;
    layout_array_oam[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR);
    layout_array_oam[idx].name = "oam_sw_db_info__reflector";
    layout_array_oam[idx].type = "oam_reflector_sw_db_info_t";
    layout_array_oam[idx].doc = "OAM reflector SW db info";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(oam_reflector_sw_db_info_t);
    layout_array_oam[idx].parent  = 0;
    layout_array_oam[idx].first_child_index = 7;
    layout_array_oam[idx].last_child_index = 8;
    layout_array_oam[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__OAM_GROUP_SW_DB_INFO__OAM_GROUP_ARRAY_OF_LINKED_LISTS);
    layout_array_oam[idx].name = "oam_sw_db_info__oam_group_sw_db_info__oam_group_array_of_linked_lists";
    layout_array_oam[idx].type = "sw_state_multihead_ll_t";
    layout_array_oam[idx].doc = "an array of groups with a linked list of EPs";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(sw_state_multihead_ll_t);
    layout_array_oam[idx].parent  = 1;
    layout_array_oam[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS);
    layout_array_oam[idx].name = "oam_sw_db_info__oam_endpoint_sw_db_info__oam_endpoint_array_of_rmep_linked_lists";
    layout_array_oam[idx].type = "sw_state_multihead_ll_t";
    layout_array_oam[idx].doc = "an array of local endpoints with a linked list of RMEPs";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(sw_state_multihead_ll_t);
    layout_array_oam[idx].parent  = 2;
    layout_array_oam[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__OAM_ENDPOINT_SW_DB_INFO__OAM_ENDPOINT_ARRAY_OF_MEP_LINKED_LISTS);
    layout_array_oam[idx].name = "oam_sw_db_info__oam_endpoint_sw_db_info__oam_endpoint_array_of_mep_linked_lists";
    layout_array_oam[idx].type = "sw_state_multihead_ll_t";
    layout_array_oam[idx].doc = "an array of local endpoints with a linked list of MEPs on same LIF, level and direction";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(sw_state_multihead_ll_t);
    layout_array_oam[idx].parent  = 2;
    layout_array_oam[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__ENCAP_ID);
    layout_array_oam[idx].name = "oam_sw_db_info__reflector__encap_id";
    layout_array_oam[idx].type = "int";
    layout_array_oam[idx].doc = "global encap_id allocated for oam oam reflector";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(int);
    layout_array_oam[idx].parent  = 3;
    layout_array_oam[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].next_node_index = 8;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_OAM_SW_DB_INFO__REFLECTOR__IS_ALLOCATED);
    layout_array_oam[idx].name = "oam_sw_db_info__reflector__is_allocated";
    layout_array_oam[idx].type = "int";
    layout_array_oam[idx].doc = "1 if encap for oam reflector allocated";
    layout_array_oam[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].size_of = sizeof(int);
    layout_array_oam[idx].parent  = 3;
    layout_array_oam[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_oam[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_OAM_SW_DB_INFO, layout_array_oam, sw_state_layout_array[unit][OAM_MODULE_ID], DNX_SW_STATE_OAM_SW_DB_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
