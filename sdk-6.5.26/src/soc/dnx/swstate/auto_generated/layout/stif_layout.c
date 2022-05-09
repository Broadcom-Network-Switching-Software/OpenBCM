
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/stif_types.h>
#include <soc/dnx/swstate/auto_generated/layout/stif_layout.h>

dnxc_sw_state_layout_t layout_array_stif[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB_NOF_PARAMS)];

shr_error_e
stif_init_layout_structure(int unit)
{

    bcm_port_t dnx_stif_db__instance__logical_port__default_val = -1;
    bcm_port_t dnx_stif_db__source_mapping__first_port__default_val = -1;
    bcm_port_t dnx_stif_db__source_mapping__second_port__default_val = -1;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB);
    layout_array_stif[idx].name = "dnx_stif_db";
    layout_array_stif[idx].type = "dnx_stif_db_t";
    layout_array_stif[idx].doc = "STIF DB per unit";
    layout_array_stif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].size_of = sizeof(dnx_stif_db_t);
    layout_array_stif[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__FIRST);
    layout_array_stif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__INSTANCE);
    layout_array_stif[idx].name = "dnx_stif_db__instance";
    layout_array_stif[idx].type = "dnx_instance_info_t**";
    layout_array_stif[idx].doc = "instance config. array of [core][instance_id] ";
    layout_array_stif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].size_of = sizeof(dnx_instance_info_t**);
    layout_array_stif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_stif[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_stif[idx].parent  = 0;
    layout_array_stif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__INSTANCE__FIRST);
    layout_array_stif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__INSTANCE__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__SOURCE_MAPPING);
    layout_array_stif[idx].name = "dnx_stif_db__source_mapping";
    layout_array_stif[idx].type = "dnx_source_to_logical_port_mapping_t**";
    layout_array_stif[idx].doc = "source mapping config. array of [source_core][source_id] ";
    layout_array_stif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].size_of = sizeof(dnx_source_to_logical_port_mapping_t**);
    layout_array_stif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_stif[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_stif[idx].parent  = 0;
    layout_array_stif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__SOURCE_MAPPING__FIRST);
    layout_array_stif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__SOURCE_MAPPING__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__INSTANCE__LOGICAL_PORT);
    layout_array_stif[idx].name = "dnx_stif_db__instance__logical_port";
    layout_array_stif[idx].type = "bcm_port_t";
    layout_array_stif[idx].doc = "the logical port this instance is represented by";
    layout_array_stif[idx].default_value= &(dnx_stif_db__instance__logical_port__default_val);
    layout_array_stif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].size_of = sizeof(bcm_port_t);
    layout_array_stif[idx].parent  = 1;
    layout_array_stif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__SOURCE_MAPPING__FIRST_PORT);
    layout_array_stif[idx].name = "dnx_stif_db__source_mapping__first_port";
    layout_array_stif[idx].type = "bcm_port_t";
    layout_array_stif[idx].doc = "defines the first connected logical port to this source";
    layout_array_stif[idx].default_value= &(dnx_stif_db__source_mapping__first_port__default_val);
    layout_array_stif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].size_of = sizeof(bcm_port_t);
    layout_array_stif[idx].parent  = 2;
    layout_array_stif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STIF_DB__SOURCE_MAPPING__SECOND_PORT);
    layout_array_stif[idx].name = "dnx_stif_db__source_mapping__second_port";
    layout_array_stif[idx].type = "bcm_port_t";
    layout_array_stif[idx].doc = "defines the second connected logical port to this source";
    layout_array_stif[idx].default_value= &(dnx_stif_db__source_mapping__second_port__default_val);
    layout_array_stif[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].size_of = sizeof(bcm_port_t);
    layout_array_stif[idx].parent  = 2;
    layout_array_stif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_stif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_STIF_DB, layout_array_stif, sw_state_layout_array[unit][STIF_MODULE_ID], DNX_SW_STATE_DNX_STIF_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
