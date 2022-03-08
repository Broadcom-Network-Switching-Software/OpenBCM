
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_hard_reset_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_hard_reset_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_hard_reset[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_HARD_RESET_DB_NOF_PARAMS)];

shr_error_e
dnx_hard_reset_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_HARD_RESET_DB);
    layout_array_dnx_hard_reset[idx].name = "dnx_hard_reset_db";
    layout_array_dnx_hard_reset[idx].type = "dnx_hard_reset_db_t";
    layout_array_dnx_hard_reset[idx].doc = "DB for used for hard reset";
    layout_array_dnx_hard_reset[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].size_of = sizeof(dnx_hard_reset_db_t);
    layout_array_dnx_hard_reset[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].first_child_index = 1;
    layout_array_dnx_hard_reset[idx].last_child_index = 1;
    layout_array_dnx_hard_reset[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_HARD_RESET_DB__HARD_RESET_CALLBACK);
    layout_array_dnx_hard_reset[idx].name = "dnx_hard_reset_db__hard_reset_callback";
    layout_array_dnx_hard_reset[idx].type = "dnx_hard_reset_t";
    layout_array_dnx_hard_reset[idx].doc = "hard reset callback information (as register by the user)";
    layout_array_dnx_hard_reset[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].size_of = sizeof(dnx_hard_reset_t);
    layout_array_dnx_hard_reset[idx].parent  = 0;
    layout_array_dnx_hard_reset[idx].first_child_index = 2;
    layout_array_dnx_hard_reset[idx].last_child_index = 3;
    layout_array_dnx_hard_reset[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_HARD_RESET_DB__HARD_RESET_CALLBACK__CALLBACK);
    layout_array_dnx_hard_reset[idx].name = "dnx_hard_reset_db__hard_reset_callback__callback";
    layout_array_dnx_hard_reset[idx].type = "bcm_switch_hard_reset_callback_t";
    layout_array_dnx_hard_reset[idx].doc = "pointer to hard reset callback function";
    layout_array_dnx_hard_reset[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].size_of = sizeof(bcm_switch_hard_reset_callback_t);
    layout_array_dnx_hard_reset[idx].parent  = 1;
    layout_array_dnx_hard_reset[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_HARD_RESET_DB__HARD_RESET_CALLBACK__USERDATA);
    layout_array_dnx_hard_reset[idx].name = "dnx_hard_reset_db__hard_reset_callback__userdata";
    layout_array_dnx_hard_reset[idx].type = "dnx_hard_reset_callback_userdata_t";
    layout_array_dnx_hard_reset[idx].doc = "user data pointer";
    layout_array_dnx_hard_reset[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].size_of = sizeof(dnx_hard_reset_callback_userdata_t);
    layout_array_dnx_hard_reset[idx].parent  = 1;
    layout_array_dnx_hard_reset[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_hard_reset[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_HARD_RESET_DB, layout_array_dnx_hard_reset, sw_state_layout_array[unit][DNX_HARD_RESET_MODULE_ID], DNX_SW_STATE_DNX_HARD_RESET_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
