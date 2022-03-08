
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/auto_generated/types/dnx_stk_sys_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_stk_sys_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_stk_sys[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STK_SYS_DB_NOF_PARAMS)];

shr_error_e
dnx_stk_sys_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STK_SYS_DB);
    layout_array_dnx_stk_sys[idx].name = "dnx_stk_sys_db";
    layout_array_dnx_stk_sys[idx].type = "dnx_stk_sys_db_t";
    layout_array_dnx_stk_sys[idx].doc = "DB for use by stack module";
    layout_array_dnx_stk_sys[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].size_of = sizeof(dnx_stk_sys_db_t);
    layout_array_dnx_stk_sys[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].first_child_index = 1;
    layout_array_dnx_stk_sys[idx].last_child_index = 2;
    layout_array_dnx_stk_sys[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STK_SYS_DB__NOF_FAP_IDS);
    layout_array_dnx_stk_sys[idx].name = "dnx_stk_sys_db__nof_fap_ids";
    layout_array_dnx_stk_sys[idx].type = "int";
    layout_array_dnx_stk_sys[idx].doc = "Number of defined fap ids";
    layout_array_dnx_stk_sys[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].size_of = sizeof(int);
    layout_array_dnx_stk_sys[idx].parent  = 0;
    layout_array_dnx_stk_sys[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].next_node_index = 2;
    layout_array_dnx_stk_sys[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_STK_SYS_DB__MODULE_ENABLE_DONE);
    layout_array_dnx_stk_sys[idx].name = "dnx_stk_sys_db__module_enable_done";
    layout_array_dnx_stk_sys[idx].type = "int";
    layout_array_dnx_stk_sys[idx].doc = "Is module enable done for the first time";
    layout_array_dnx_stk_sys[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].size_of = sizeof(int);
    layout_array_dnx_stk_sys[idx].parent  = 0;
    layout_array_dnx_stk_sys[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_stk_sys[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_STK_SYS_DB, layout_array_dnx_stk_sys, sw_state_layout_array[unit][DNX_STK_SYS_MODULE_ID], DNX_SW_STATE_DNX_STK_SYS_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#endif  
#undef BSL_LOG_MODULE
