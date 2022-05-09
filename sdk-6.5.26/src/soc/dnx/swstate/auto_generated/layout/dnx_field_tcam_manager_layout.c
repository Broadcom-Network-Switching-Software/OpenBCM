
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_manager_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_manager[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_manager_init_layout_structure(int unit)
{

    dnx_field_tcam_handler_mode_e dnx_field_tcam_manager_sw__tcam_handlers__mode__default_val = DNX_FIELD_TCAM_HANDLER_MODE_INVALID;
    dnx_field_tcam_handler_state_e dnx_field_tcam_manager_sw__tcam_handlers__state__default_val = DNX_FIELD_TCAM_HANDLER_STATE_CLOSE;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW);
    layout_array_dnx_field_tcam_manager[idx].name = "dnx_field_tcam_manager_sw";
    layout_array_dnx_field_tcam_manager[idx].type = "dnx_field_tcam_manager_sw_t";
    layout_array_dnx_field_tcam_manager[idx].doc = "Holds information about all the TCAM handlers in the system";
    layout_array_dnx_field_tcam_manager[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].size_of = sizeof(dnx_field_tcam_manager_sw_t);
    layout_array_dnx_field_tcam_manager[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__FIRST);
    layout_array_dnx_field_tcam_manager[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS);
    layout_array_dnx_field_tcam_manager[idx].name = "dnx_field_tcam_manager_sw__tcam_handlers";
    layout_array_dnx_field_tcam_manager[idx].type = "dnx_field_tcam_handler_t*";
    layout_array_dnx_field_tcam_manager[idx].doc = "Array that represents all TCAM handlers in the system";
    layout_array_dnx_field_tcam_manager[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].size_of = sizeof(dnx_field_tcam_handler_t*);
    layout_array_dnx_field_tcam_manager[idx].array_indexes[0].num_elements = dnx_data_field.tcam.nof_tcam_handlers_get(unit);
    layout_array_dnx_field_tcam_manager[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_manager[idx].parent  = 0;
    layout_array_dnx_field_tcam_manager[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS__FIRST);
    layout_array_dnx_field_tcam_manager[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS__LAST);
    layout_array_dnx_field_tcam_manager[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS__MODE);
    layout_array_dnx_field_tcam_manager[idx].name = "dnx_field_tcam_manager_sw__tcam_handlers__mode";
    layout_array_dnx_field_tcam_manager[idx].type = "dnx_field_tcam_handler_mode_e";
    layout_array_dnx_field_tcam_manager[idx].doc = "handler's mode";
    layout_array_dnx_field_tcam_manager[idx].default_value= &(dnx_field_tcam_manager_sw__tcam_handlers__mode__default_val);
    layout_array_dnx_field_tcam_manager[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].size_of = sizeof(dnx_field_tcam_handler_mode_e);
    layout_array_dnx_field_tcam_manager[idx].parent  = 1;
    layout_array_dnx_field_tcam_manager[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS__STATE);
    layout_array_dnx_field_tcam_manager[idx].name = "dnx_field_tcam_manager_sw__tcam_handlers__state";
    layout_array_dnx_field_tcam_manager[idx].type = "dnx_field_tcam_handler_state_e";
    layout_array_dnx_field_tcam_manager[idx].doc = "handler's state";
    layout_array_dnx_field_tcam_manager[idx].default_value= &(dnx_field_tcam_manager_sw__tcam_handlers__state__default_val);
    layout_array_dnx_field_tcam_manager[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].size_of = sizeof(dnx_field_tcam_handler_state_e);
    layout_array_dnx_field_tcam_manager[idx].parent  = 1;
    layout_array_dnx_field_tcam_manager[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_manager[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW, layout_array_dnx_field_tcam_manager, sw_state_layout_array[unit][DNX_FIELD_TCAM_MANAGER_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
