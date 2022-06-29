
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/srv6_types.h>
#include <soc/dnx/swstate/auto_generated/layout/srv6_layout.h>

dnxc_sw_state_layout_t layout_array_srv6[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES_NOF_PARAMS)];

shr_error_e
srv6_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES);
    layout_array_srv6[idx].name = "srv6_modes";
    layout_array_srv6[idx].type = "srv6_modes_struct";
    layout_array_srv6[idx].doc = "Define SRv6 Tunnel modes";
    layout_array_srv6[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].size_of = sizeof(srv6_modes_struct);
    layout_array_srv6[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES__FIRST);
    layout_array_srv6[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP);
    layout_array_srv6[idx].name = "srv6_modes__egress_is_psp";
    layout_array_srv6[idx].type = "uint8";
    layout_array_srv6[idx].doc = "if set, SRv6 Egress node is PSP mode";
    layout_array_srv6[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].size_of = sizeof(uint8);
    layout_array_srv6[idx].parent  = 0;
    layout_array_srv6[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED);
    layout_array_srv6[idx].name = "srv6_modes__encap_is_reduced";
    layout_array_srv6[idx].type = "uint8";
    layout_array_srv6[idx].doc = "if set, encapsulation is Reduced mode";
    layout_array_srv6[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].size_of = sizeof(uint8);
    layout_array_srv6[idx].parent  = 0;
    layout_array_srv6[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B);
    layout_array_srv6[idx].name = "srv6_modes__gsid_prefix_is_64b";
    layout_array_srv6[idx].type = "uint8";
    layout_array_srv6[idx].doc = "if set, GSID prefix is 64b, else 48b";
    layout_array_srv6[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].size_of = sizeof(uint8);
    layout_array_srv6[idx].parent  = 0;
    layout_array_srv6[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SRV6_MODES__USID_PREFIX_IS_48B);
    layout_array_srv6[idx].name = "srv6_modes__usid_prefix_is_48b";
    layout_array_srv6[idx].type = "uint8";
    layout_array_srv6[idx].doc = "if set, USID prefix is 48b, otherwise 32b";
    layout_array_srv6[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].size_of = sizeof(uint8);
    layout_array_srv6[idx].parent  = 0;
    layout_array_srv6[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_srv6[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_SRV6_MODES, layout_array_srv6, sw_state_layout_array[unit][SRV6_MODULE_ID], DNX_SW_STATE_SRV6_MODES_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
