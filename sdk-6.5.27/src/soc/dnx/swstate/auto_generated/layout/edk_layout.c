
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/edk_types.h>
#include <soc/dnx/swstate/auto_generated/layout/edk_layout.h>

dnxc_sw_state_layout_t layout_array_edk[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EDK_STATE_NOF_PARAMS)];

shr_error_e
edk_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EDK_STATE);
    layout_array_edk[idx].name = "edk_state";
    layout_array_edk[idx].type = "edk_sw_state";
    layout_array_edk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_edk[idx].size_of = sizeof(edk_sw_state);
    layout_array_edk[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_edk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EDK_STATE__FIRST);
    layout_array_edk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EDK_STATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_EDK_STATE__EDK_SEQ);
    layout_array_edk[idx].name = "edk_state__edk_seq";
    layout_array_edk[idx].type = "int";
    layout_array_edk[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_edk[idx].size_of = sizeof(int);
    layout_array_edk[idx].array_indexes[0].num_elements = EDK_SEQ_MAX;
    layout_array_edk[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_edk[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_edk[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_edk[idx].parent  = 0;
    layout_array_edk[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_edk[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_edk[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_EDK_STATE, layout_array_edk, sw_state_layout_array[unit][EDK_MODULE_ID], DNX_SW_STATE_EDK_STATE_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
