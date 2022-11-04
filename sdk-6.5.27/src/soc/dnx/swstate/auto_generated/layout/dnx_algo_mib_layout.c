
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_mib_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_algo_mib_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_algo_mib[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB_NOF_PARAMS)];

shr_error_e
dnx_algo_mib_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB);
    layout_array_dnx_algo_mib[idx].name = "dnx_algo_mib_db";
    layout_array_dnx_algo_mib[idx].type = "dnx_algo_mib_db_t";
    layout_array_dnx_algo_mib[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].size_of = sizeof(dnx_algo_mib_db_t);
    layout_array_dnx_algo_mib[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__FIRST);
    layout_array_dnx_algo_mib[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB);
    layout_array_dnx_algo_mib[idx].name = "dnx_algo_mib_db__mib";
    layout_array_dnx_algo_mib[idx].type = "soc_dnxc_mib_db_t";
    layout_array_dnx_algo_mib[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].size_of = sizeof(soc_dnxc_mib_db_t);
    layout_array_dnx_algo_mib[idx].parent  = 0;
    layout_array_dnx_algo_mib[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__FIRST);
    layout_array_dnx_algo_mib[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP);
    layout_array_dnx_algo_mib[idx].name = "dnx_algo_mib_db__mib__counter_pbmp";
    layout_array_dnx_algo_mib[idx].type = "bcm_pbmp_t";
    layout_array_dnx_algo_mib[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].size_of = sizeof(bcm_pbmp_t);
    layout_array_dnx_algo_mib[idx].parent  = 1;
    layout_array_dnx_algo_mib[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL);
    layout_array_dnx_algo_mib[idx].name = "dnx_algo_mib_db__mib__interval";
    layout_array_dnx_algo_mib[idx].type = "int";
    layout_array_dnx_algo_mib[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].size_of = sizeof(int);
    layout_array_dnx_algo_mib[idx].parent  = 1;
    layout_array_dnx_algo_mib[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_mib[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_ALGO_MIB_DB, layout_array_dnx_algo_mib, sw_state_layout_array[unit][DNX_ALGO_MIB_MODULE_ID], DNX_SW_STATE_DNX_ALGO_MIB_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
