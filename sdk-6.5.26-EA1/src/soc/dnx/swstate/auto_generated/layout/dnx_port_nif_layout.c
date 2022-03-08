
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_port_nif_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_port_nif[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB_NOF_PARAMS)];

shr_error_e
dnx_port_nif_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_t";
    layout_array_dnx_port_nif[idx].doc = "DB for handling nif blocks";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_t);
    layout_array_dnx_port_nif[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].first_child_index = 1;
    layout_array_dnx_port_nif[idx].last_child_index = 3;
    layout_array_dnx_port_nif[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_link_list_t";
    layout_array_dnx_port_nif[idx].doc = "DB for arbiter link list handling.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_link_list_t);
    layout_array_dnx_port_nif[idx].parent  = 0;
    layout_array_dnx_port_nif[idx].first_child_index = 4;
    layout_array_dnx_port_nif[idx].last_child_index = 6;
    layout_array_dnx_port_nif[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_link_list_t";
    layout_array_dnx_port_nif[idx].doc = "DB for OFT link list handling.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_link_list_t);
    layout_array_dnx_port_nif[idx].parent  = 0;
    layout_array_dnx_port_nif[idx].first_child_index = 11;
    layout_array_dnx_port_nif[idx].last_child_index = 13;
    layout_array_dnx_port_nif[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_double_priority_link_list_t";
    layout_array_dnx_port_nif[idx].doc = "DB for OFR link list handling.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_double_priority_link_list_t);
    layout_array_dnx_port_nif[idx].parent  = 0;
    layout_array_dnx_port_nif[idx].first_child_index = 18;
    layout_array_dnx_port_nif[idx].last_child_index = 20;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__list_info";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_list_info_t";
    layout_array_dnx_port_nif[idx].doc = "DB for general link list meta data.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_list_info_t);
    layout_array_dnx_port_nif[idx].parent  = 1;
    layout_array_dnx_port_nif[idx].first_child_index = 7;
    layout_array_dnx_port_nif[idx].last_child_index = 9;
    layout_array_dnx_port_nif[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__linking_info";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_linking_info_t*";
    layout_array_dnx_port_nif[idx].doc = "DB for all the sections next pointer.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_linking_info_t*);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 1;
    layout_array_dnx_port_nif[idx].first_child_index = 10;
    layout_array_dnx_port_nif[idx].last_child_index = 10;
    layout_array_dnx_port_nif[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__NOF_SECTIONS_ALLOCATED);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__nof_sections_allocated";
    layout_array_dnx_port_nif[idx].type = "uint32*";
    layout_array_dnx_port_nif[idx].doc = "Number of sections which allocated per client.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(uint32*);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 1;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__HEAD);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__list_info__head";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 4;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = 8;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__TAIL);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__list_info__tail";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 4;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = 9;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LIST_INFO__SIZE);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__list_info__size";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 4;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__arb_link_list__linking_info__next_section";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 5;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__list_info";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_list_info_t";
    layout_array_dnx_port_nif[idx].doc = "DB for general link list meta data.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_list_info_t);
    layout_array_dnx_port_nif[idx].parent  = 2;
    layout_array_dnx_port_nif[idx].first_child_index = 14;
    layout_array_dnx_port_nif[idx].last_child_index = 16;
    layout_array_dnx_port_nif[idx].next_node_index = 12;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__linking_info";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_linking_info_t*";
    layout_array_dnx_port_nif[idx].doc = "DB for all the sections next pointer.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_linking_info_t*);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 2;
    layout_array_dnx_port_nif[idx].first_child_index = 17;
    layout_array_dnx_port_nif[idx].last_child_index = 17;
    layout_array_dnx_port_nif[idx].next_node_index = 13;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__NOF_SECTIONS_ALLOCATED);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__nof_sections_allocated";
    layout_array_dnx_port_nif[idx].type = "uint32*";
    layout_array_dnx_port_nif[idx].doc = "Number of sections which allocated per client.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(uint32*);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 2;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__HEAD);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__list_info__head";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 11;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = 15;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__TAIL);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__list_info__tail";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 11;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = 16;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LIST_INFO__SIZE);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__list_info__size";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 11;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__oft_link_list__linking_info__next_section";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 12;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__list_info";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_list_info_t*";
    layout_array_dnx_port_nif[idx].doc = "DB for OFR free lists handling.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_list_info_t*);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 3;
    layout_array_dnx_port_nif[idx].first_child_index = 21;
    layout_array_dnx_port_nif[idx].last_child_index = 23;
    layout_array_dnx_port_nif[idx].next_node_index = 19;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__linking_info";
    layout_array_dnx_port_nif[idx].type = "dnx_port_nif_db_linking_info_t**";
    layout_array_dnx_port_nif[idx].doc = "DB for all the sections next pointer.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(dnx_port_nif_db_linking_info_t**);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 3;
    layout_array_dnx_port_nif[idx].first_child_index = 24;
    layout_array_dnx_port_nif[idx].last_child_index = 24;
    layout_array_dnx_port_nif[idx].next_node_index = 20;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__NOF_SECTIONS_ALLOCATED);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__nof_sections_allocated";
    layout_array_dnx_port_nif[idx].type = "uint32**";
    layout_array_dnx_port_nif[idx].doc = "Number of sections which allocated per client.";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(uint32**);
    layout_array_dnx_port_nif[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_nif[idx].parent  = 3;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__list_info__head";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 18;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = 22;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__list_info__tail";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 18;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = 23;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__list_info__size";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 18;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION);
    layout_array_dnx_port_nif[idx].name = "dnx_port_nif_db__ofr_link_list__linking_info__next_section";
    layout_array_dnx_port_nif[idx].type = "int";
    layout_array_dnx_port_nif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].size_of = sizeof(int);
    layout_array_dnx_port_nif[idx].parent  = 19;
    layout_array_dnx_port_nif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_nif[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_PORT_NIF_DB, layout_array_dnx_port_nif, sw_state_layout_array[unit][DNX_PORT_NIF_MODULE_ID], DNX_SW_STATE_DNX_PORT_NIF_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
