
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_port_imb_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_port_imb[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM_NOF_PARAMS)];

shr_error_e
dnx_port_imb_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM);
    layout_array_dnx_port_imb[idx].name = "imbm";
    layout_array_dnx_port_imb[idx].type = "dnx_imbm_info_t";
    layout_array_dnx_port_imb[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].size_of = sizeof(dnx_imbm_info_t);
    layout_array_dnx_port_imb[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__FIRST);
    layout_array_dnx_port_imb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__IMBS_IN_USE);
    layout_array_dnx_port_imb[idx].name = "imbm__imbs_in_use";
    layout_array_dnx_port_imb[idx].type = "uint32";
    layout_array_dnx_port_imb[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].size_of = sizeof(uint32);
    layout_array_dnx_port_imb[idx].parent  = 0;
    layout_array_dnx_port_imb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__IMB);
    layout_array_dnx_port_imb[idx].name = "imbm__imb";
    layout_array_dnx_port_imb[idx].type = "imb_create_info_t";
    layout_array_dnx_port_imb[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].size_of = sizeof(imb_create_info_t);
    layout_array_dnx_port_imb[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_port_imb[idx].parent  = 0;
    layout_array_dnx_port_imb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__IMB_TYPE);
    layout_array_dnx_port_imb[idx].name = "imbm__imb_type";
    layout_array_dnx_port_imb[idx].type = "imb_dispatch_type_t";
    layout_array_dnx_port_imb[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].size_of = sizeof(imb_dispatch_type_t);
    layout_array_dnx_port_imb[idx].array_indexes[0].num_elements = SOC_MAX_NUM_PORTS;
    layout_array_dnx_port_imb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_port_imb[idx].parent  = 0;
    layout_array_dnx_port_imb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER);
    layout_array_dnx_port_imb[idx].name = "imbm__pmd_lock_counter";
    layout_array_dnx_port_imb[idx].type = "uint32";
    layout_array_dnx_port_imb[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].size_of = sizeof(uint32);
    layout_array_dnx_port_imb[idx].array_indexes[0].num_elements = SOC_MAX_NUM_PORTS;
    layout_array_dnx_port_imb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_port_imb[idx].parent  = 0;
    layout_array_dnx_port_imb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX);
    layout_array_dnx_port_imb[idx].name = "imbm__credit_tx_reset_mutex";
    layout_array_dnx_port_imb[idx].type = "sw_state_mutex_t";
    layout_array_dnx_port_imb[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_dnx_port_imb[idx].parent  = 0;
    layout_array_dnx_port_imb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_port_imb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_IMBM, layout_array_dnx_port_imb, sw_state_layout_array[unit][DNX_PORT_IMB_MODULE_ID], DNX_SW_STATE_IMBM_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
