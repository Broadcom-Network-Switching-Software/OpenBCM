
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_ecgm_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_ecgm_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_ecgm[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB_NOF_PARAMS)];

shr_error_e
dnx_ecgm_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db";
    layout_array_dnx_ecgm[idx].type = "dnx_ecgm_sw_state_t";
    layout_array_dnx_ecgm[idx].doc = "ECGM related sw state";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(dnx_ecgm_sw_state_t);
    layout_array_dnx_ecgm[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__FIRST);
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__port_info";
    layout_array_dnx_ecgm[idx].type = "dnx_ecgm_port_profile_info_t**";
    layout_array_dnx_ecgm[idx].doc = "port threshold information";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(dnx_ecgm_port_profile_info_t**);
    layout_array_dnx_ecgm[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_ecgm[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_ecgm[idx].array_indexes[1].num_elements = dnx_data_port.general.nof_out_tm_ports_get(unit);
    layout_array_dnx_ecgm[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_ecgm[idx].parent  = 0;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__interface_caching";
    layout_array_dnx_ecgm[idx].type = "dnx_ecgm_interface_data_t";
    layout_array_dnx_ecgm[idx].doc = "interface profile info";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(dnx_ecgm_interface_data_t);
    layout_array_dnx_ecgm[idx].parent  = 0;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__FIRST);
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__port_caching";
    layout_array_dnx_ecgm[idx].type = "dnx_ecgm_port_data_t";
    layout_array_dnx_ecgm[idx].doc = "port profile info";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(dnx_ecgm_port_data_t);
    layout_array_dnx_ecgm[idx].parent  = 0;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__FIRST);
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__interface_caching__port";
    layout_array_dnx_ecgm[idx].type = "bcm_port_t";
    layout_array_dnx_ecgm[idx].doc = "logical port";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(bcm_port_t);
    layout_array_dnx_ecgm[idx].parent  = 2;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__interface_caching__info";
    layout_array_dnx_ecgm[idx].type = "dnx_ecgm_interface_profile_info_t";
    layout_array_dnx_ecgm[idx].doc = "profile data";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(dnx_ecgm_interface_profile_info_t);
    layout_array_dnx_ecgm[idx].parent  = 2;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__interface_caching__profile_id";
    layout_array_dnx_ecgm[idx].type = "int";
    layout_array_dnx_ecgm[idx].doc = "profile id";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(int);
    layout_array_dnx_ecgm[idx].parent  = 2;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__interface_caching__valid";
    layout_array_dnx_ecgm[idx].type = "int";
    layout_array_dnx_ecgm[idx].doc = "is data valid";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(int);
    layout_array_dnx_ecgm[idx].parent  = 2;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__port_caching__port";
    layout_array_dnx_ecgm[idx].type = "bcm_port_t";
    layout_array_dnx_ecgm[idx].doc = "logical port";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(bcm_port_t);
    layout_array_dnx_ecgm[idx].parent  = 3;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__port_caching__info";
    layout_array_dnx_ecgm[idx].type = "dnx_ecgm_port_profile_info_t";
    layout_array_dnx_ecgm[idx].doc = "profile data";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(dnx_ecgm_port_profile_info_t);
    layout_array_dnx_ecgm[idx].parent  = 3;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__port_caching__profile_id";
    layout_array_dnx_ecgm[idx].type = "int";
    layout_array_dnx_ecgm[idx].doc = "profile id";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(int);
    layout_array_dnx_ecgm[idx].parent  = 3;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID);
    layout_array_dnx_ecgm[idx].name = "dnx_ecgm_db__port_caching__valid";
    layout_array_dnx_ecgm[idx].type = "int";
    layout_array_dnx_ecgm[idx].doc = "is data valid";
    layout_array_dnx_ecgm[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].size_of = sizeof(int);
    layout_array_dnx_ecgm[idx].parent  = 3;
    layout_array_dnx_ecgm[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ecgm[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_ECGM_DB, layout_array_dnx_ecgm, sw_state_layout_array[unit][DNX_ECGM_MODULE_ID], DNX_SW_STATE_DNX_ECGM_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
