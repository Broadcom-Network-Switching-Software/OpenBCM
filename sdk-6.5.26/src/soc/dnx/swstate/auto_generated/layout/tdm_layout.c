
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/tdm_types.h>
#include <soc/dnx/swstate/auto_generated/layout/tdm_layout.h>

dnxc_sw_state_layout_t layout_array_tdm[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TDM_FTMH_INFO_NOF_PARAMS)];

shr_error_e
tdm_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TDM_FTMH_INFO);
    layout_array_tdm[idx].name = "tdm_ftmh_info";
    layout_array_tdm[idx].type = "tdm_ftmh_headers_params";
    layout_array_tdm[idx].doc = "Information about FTMH header in TDM mode";
    layout_array_tdm[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_tdm[idx].size_of = sizeof(tdm_ftmh_headers_params);
    layout_array_tdm[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_tdm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TDM_FTMH_INFO__FIRST);
    layout_array_tdm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TDM_FTMH_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TDM_FTMH_INFO__TDM_HEADER_IS_FTMH);
    layout_array_tdm[idx].name = "tdm_ftmh_info__tdm_header_is_ftmh";
    layout_array_tdm[idx].type = "uint8";
    layout_array_tdm[idx].doc = "if set, TDM header is FTMH";
    layout_array_tdm[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_tdm[idx].size_of = sizeof(uint8);
    layout_array_tdm[idx].parent  = 0;
    layout_array_tdm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_tdm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_TDM_FTMH_INFO__USE_OPTIMIZED_FTMH);
    layout_array_tdm[idx].name = "tdm_ftmh_info__use_optimized_ftmh";
    layout_array_tdm[idx].type = "uint8";
    layout_array_tdm[idx].doc = "if set, and FTMH TDM header is optimized";
    layout_array_tdm[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_tdm[idx].size_of = sizeof(uint8);
    layout_array_tdm[idx].parent  = 0;
    layout_array_tdm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_tdm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_TDM_FTMH_INFO, layout_array_tdm, sw_state_layout_array[unit][TDM_MODULE_ID], DNX_SW_STATE_TDM_FTMH_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
