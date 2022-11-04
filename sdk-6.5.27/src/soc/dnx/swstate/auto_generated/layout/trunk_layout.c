
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/dnx/swstate/auto_generated/layout/trunk_layout.h>

dnxc_sw_state_layout_t layout_array_trunk[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB_NOF_PARAMS)];

shr_error_e
trunk_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB);
    layout_array_trunk[idx].name = "trunk_db";
    layout_array_trunk[idx].type = "trunk_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(trunk_info_t);
    layout_array_trunk[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS);
    layout_array_trunk[idx].name = "trunk_db__pools";
    layout_array_trunk[idx].type = "trunk_pool_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(trunk_pool_info_t);
    layout_array_trunk[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_trunk[idx].parent  = 0;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB);
    layout_array_trunk[idx].name = "trunk_db__trunk_system_port_db";
    layout_array_trunk[idx].type = "trunk_system_port_db_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(trunk_system_port_db_t);
    layout_array_trunk[idx].parent  = 0;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info";
    layout_array_trunk[idx].type = "psc_general_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(psc_general_info_t);
    layout_array_trunk[idx].parent  = 0;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS);
    layout_array_trunk[idx].name = "trunk_db__pools__groups";
    layout_array_trunk[idx].type = "trunk_group_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(trunk_group_info_t);
    layout_array_trunk[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_trunk[idx].parent  = 1;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST)-1;
    layout_array_trunk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__psc";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PSC_PROFILE_ID);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__psc_profile_id";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__members";
    layout_array_trunk[idx].type = "trunk_group_member_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(trunk_group_member_info_t);
    layout_array_trunk[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__LAST)-1;
    layout_array_trunk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__NOF_MEMBERS);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__nof_members";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__LAST_MEMBER_ADDED_INDEX);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__last_member_added_index";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_USE);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__in_use";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__PP_PORTS);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__pp_ports";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__IN_HEADER_TYPE);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__in_header_type";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__OUT_HEADER_TYPE);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__out_header_type";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__FLAGS);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__flags";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MASTER_TRUNK_ID);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__master_trunk_id";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 4;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SYSTEM_PORT);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__members__system_port";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 7;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__FLAGS);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__members__flags";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 7;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__INDEX);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__members__index";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 7;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__SPA_MEMBER_ID);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__members__spa_member_id";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 7;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__POOLS__GROUPS__MEMBERS__PROTECTING_SYSTEM_PORT);
    layout_array_trunk[idx].name = "trunk_db__pools__groups__members__protecting_system_port";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 7;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES);
    layout_array_trunk[idx].name = "trunk_db__trunk_system_port_db__trunk_system_port_entries";
    layout_array_trunk[idx].type = "trunk_system_port_entry_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(trunk_system_port_entry_t);
    layout_array_trunk[idx].array_indexes[0].num_elements = dnx_data_device.general.max_nof_system_ports_get(unit);
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_trunk[idx].parent  = 2;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__LAST)-1;
    layout_array_trunk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__TRUNK_ID);
    layout_array_trunk[idx].name = "trunk_db__trunk_system_port_db__trunk_system_port_entries__trunk_id";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 21;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__TRUNK_SYSTEM_PORT_DB__TRUNK_SYSTEM_PORT_ENTRIES__FLAGS);
    layout_array_trunk[idx].name = "trunk_db__trunk_system_port_db__trunk_system_port_entries__flags";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 21;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__CHM_HANDLE);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__chm_handle";
    layout_array_trunk[idx].type = "uint32";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(uint32);
    layout_array_trunk[idx].parent  = 3;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__profile_info";
    layout_array_trunk[idx].type = "profile_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(profile_info_t);
    layout_array_trunk[idx].array_indexes[0].num_elements = dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit);
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_trunk[idx].parent  = 3;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__LAST)-1;
    layout_array_trunk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__user_profile_info";
    layout_array_trunk[idx].type = "user_profile_info_t";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(user_profile_info_t);
    layout_array_trunk[idx].array_indexes[0].num_elements = dnx_data_trunk.psc.consistant_hashing_nof_resources_get(unit);
    layout_array_trunk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_trunk[idx].parent  = 3;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__FIRST);
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__LAST)-1;
    layout_array_trunk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__PSC_PROFILE_TYPE);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__profile_info__psc_profile_type";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 25;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__PROFILE_INFO__MAX_NOF_MEMBERS);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__profile_info__max_nof_members";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 25;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__ASSIGNED_PSC_PROFILE);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__user_profile_info__assigned_psc_profile";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 26;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TRUNK_DB__PSC_GENERAL_INFO__USER_PROFILE_INFO__REF_COUNTER);
    layout_array_trunk[idx].name = "trunk_db__psc_general_info__user_profile_info__ref_counter";
    layout_array_trunk[idx].type = "int";
    layout_array_trunk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].size_of = sizeof(int);
    layout_array_trunk[idx].parent  = 26;
    layout_array_trunk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_trunk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_TRUNK_DB, layout_array_trunk, sw_state_layout_array[unit][TRUNK_MODULE_ID], DNX_SW_STATE_TRUNK_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
