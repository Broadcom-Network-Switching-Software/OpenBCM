
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_fabric_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_fabric_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_fabric[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB_NOF_PARAMS)];

shr_error_e
dnx_fabric_init_layout_structure(int unit)
{

    shr_thread_manager_handler_t dnx_fabric_db__load_balancing_shr_thread_manager_handler__default_val = NULL;
    int dnx_fabric_db__links__is_link_allowed__default_val = 1;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db";
    layout_array_dnx_fabric[idx].type = "dnx_fabric_db_t";
    layout_array_dnx_fabric[idx].doc = "DB for Fabric";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(dnx_fabric_db_t);
    layout_array_dnx_fabric[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__FIRST);
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__modids_to_group_map";
    layout_array_dnx_fabric[idx].type = "dnx_fabric_modids_group_map_t*";
    layout_array_dnx_fabric[idx].doc = "Map modids to a group";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(dnx_fabric_modids_group_map_t*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_fabric.mesh.nof_dest_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 0;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__FIRST);
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LINKS);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__links";
    layout_array_dnx_fabric[idx].type = "dnx_fabric_link_t*";
    layout_array_dnx_fabric[idx].doc = "links information";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(dnx_fabric_link_t*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_fabric.links.nof_links_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 0;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LINKS__FIRST);
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LINKS__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LOAD_BALANCING_SHR_THREAD_MANAGER_HANDLER);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__load_balancing_shr_thread_manager_handler";
    layout_array_dnx_fabric[idx].type = "shr_thread_manager_handler_t";
    layout_array_dnx_fabric[idx].doc = "Handler for the periodic event that changes the load balancing counting mode";
    layout_array_dnx_fabric[idx].default_value= &(dnx_fabric_db__load_balancing_shr_thread_manager_handler__default_val);
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(shr_thread_manager_handler_t);
    layout_array_dnx_fabric[idx].parent  = 0;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB | DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__system_upgrade";
    layout_array_dnx_fabric[idx].type = "dnx_fabric_system_upgrade_t";
    layout_array_dnx_fabric[idx].doc = "System upgrade states";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(dnx_fabric_system_upgrade_t);
    layout_array_dnx_fabric[idx].parent  = 0;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__FIRST);
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__cable_swap";
    layout_array_dnx_fabric[idx].type = "dnx_fabric_cable_swap_t";
    layout_array_dnx_fabric[idx].doc = "Cable swap Data Base";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(dnx_fabric_cable_swap_t);
    layout_array_dnx_fabric[idx].parent  = 0;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__FIRST);
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__mesh_topology";
    layout_array_dnx_fabric[idx].type = "dnx_fabric_mesh_topology_t*";
    layout_array_dnx_fabric[idx].doc = "Showing which links lead to a specific destination";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(dnx_fabric_mesh_topology_t*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_fabric.mesh.nof_dest_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 0;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__FIRST);
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__NOF_MODIDS);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__modids_to_group_map__nof_modids";
    layout_array_dnx_fabric[idx].type = "int";
    layout_array_dnx_fabric[idx].doc = "Number of modids mapped this group";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int);
    layout_array_dnx_fabric[idx].parent  = 1;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__modids_to_group_map__modids";
    layout_array_dnx_fabric[idx].type = "int*";
    layout_array_dnx_fabric[idx].doc = "The modids that mapped to this group";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_device.general.max_nof_fap_ids_per_device_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 1;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ALLOWED);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__links__is_link_allowed";
    layout_array_dnx_fabric[idx].type = "int";
    layout_array_dnx_fabric[idx].doc = "Boolean, is link is eligible for sending traffic";
    layout_array_dnx_fabric[idx].default_value= &(dnx_fabric_db__links__is_link_allowed__default_val);
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int);
    layout_array_dnx_fabric[idx].parent  = 2;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ISOLATED);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__links__is_link_isolated";
    layout_array_dnx_fabric[idx].type = "int";
    layout_array_dnx_fabric[idx].doc = "Boolean, is link isolated";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int);
    layout_array_dnx_fabric[idx].parent  = 2;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__STATE_0);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__system_upgrade__state_0";
    layout_array_dnx_fabric[idx].type = "uint32";
    layout_array_dnx_fabric[idx].doc = "System upgrade state 0";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(uint32);
    layout_array_dnx_fabric[idx].parent  = 4;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__STATE_1);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__system_upgrade__state_1";
    layout_array_dnx_fabric[idx].type = "uint32";
    layout_array_dnx_fabric[idx].doc = "System upgrade state 1";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(uint32);
    layout_array_dnx_fabric[idx].parent  = 4;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__STATE_2);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__system_upgrade__state_2";
    layout_array_dnx_fabric[idx].type = "uint32";
    layout_array_dnx_fabric[idx].doc = "System upgrade state 2";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(uint32);
    layout_array_dnx_fabric[idx].parent  = 4;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_ENABLE);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__cable_swap__is_enable";
    layout_array_dnx_fabric[idx].type = "int*";
    layout_array_dnx_fabric[idx].doc = "is cable swap enabled";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_fabric.blocks.nof_pms_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 5;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_MASTER);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__cable_swap__is_master";
    layout_array_dnx_fabric[idx].type = "int*";
    layout_array_dnx_fabric[idx].doc = "is cable swap operating as a master";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_fabric.blocks.nof_pms_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 5;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS);
    layout_array_dnx_fabric[idx].name = "dnx_fabric_db__mesh_topology__links";
    layout_array_dnx_fabric[idx].type = "int*";
    layout_array_dnx_fabric[idx].doc = "Boolean value, if fabric link leads to destination.";
    layout_array_dnx_fabric[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].size_of = sizeof(int*);
    layout_array_dnx_fabric[idx].array_indexes[0].num_elements = dnx_data_fabric.links.nof_links_get(unit);
    layout_array_dnx_fabric[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_fabric[idx].parent  = 6;
    layout_array_dnx_fabric[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fabric[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FABRIC_DB, layout_array_dnx_fabric, sw_state_layout_array[unit][DNX_FABRIC_MODULE_ID], DNX_SW_STATE_DNX_FABRIC_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
