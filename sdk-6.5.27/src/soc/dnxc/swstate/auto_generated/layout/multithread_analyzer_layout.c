
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#include <soc/dnxc/swstate/auto_generated/layout/multithread_analyzer_layout.h>


extern dnxc_sw_state_layout_t* multithread_analyzer_layout_root[SOC_MAX_NUM_DEVICES];



dnxc_sw_state_layout_t layout_array_multithread_analyzer[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MULTITHREAD_ANALYZER_NOF_PARAMS)];

shr_error_e
multithread_analyzer_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MULTITHREAD_ANALYZER);
    layout_array_multithread_analyzer[idx].name = "multithread_analyzer";
    layout_array_multithread_analyzer[idx].type = "multithread_analyzer_t";
    layout_array_multithread_analyzer[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_multithread_analyzer[idx].size_of = sizeof(multithread_analyzer_t);
    layout_array_multithread_analyzer[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_multithread_analyzer[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MULTITHREAD_ANALYZER__FIRST);
    layout_array_multithread_analyzer[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MULTITHREAD_ANALYZER__LAST)-1;
    layout_array_multithread_analyzer[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_AFTER_INIT | DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MULTITHREAD_ANALYZER__HTB);
    layout_array_multithread_analyzer[idx].name = "multithread_analyzer__htb";
    layout_array_multithread_analyzer[idx].type = "sw_state_idx_htbl_t";
    layout_array_multithread_analyzer[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_multithread_analyzer[idx].size_of = sizeof(sw_state_idx_htbl_t);
    layout_array_multithread_analyzer[idx].parent  = 0;
    layout_array_multithread_analyzer[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_multithread_analyzer[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_multithread_analyzer[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_FREE | DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION ;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_MULTITHREAD_ANALYZER, layout_array_multithread_analyzer, multithread_analyzer_layout_root[unit], DNX_SW_STATE_MULTITHREAD_ANALYZER_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
