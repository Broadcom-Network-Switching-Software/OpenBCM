
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_bier_db_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_bier_db_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_bier_db[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB_NOF_PARAMS)];

shr_error_e
dnx_bier_db_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB);
    layout_array_dnx_bier_db[idx].name = "dnx_bier_db";
    layout_array_dnx_bier_db[idx].type = "dnx_bier_db_t";
    layout_array_dnx_bier_db[idx].doc = "info about bier module";
    layout_array_dnx_bier_db[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].size_of = sizeof(dnx_bier_db_t);
    layout_array_dnx_bier_db[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__FIRST);
    layout_array_dnx_bier_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__SET_SIZE);
    layout_array_dnx_bier_db[idx].name = "dnx_bier_db__set_size";
    layout_array_dnx_bier_db[idx].type = "int";
    layout_array_dnx_bier_db[idx].doc = "set size";
    layout_array_dnx_bier_db[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].size_of = sizeof(int);
    layout_array_dnx_bier_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES;
    layout_array_dnx_bier_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_bier_db[idx].parent  = 0;
    layout_array_dnx_bier_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__INGRESS);
    layout_array_dnx_bier_db[idx].name = "dnx_bier_db__ingress";
    layout_array_dnx_bier_db[idx].type = "bier_db_status_t";
    layout_array_dnx_bier_db[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].size_of = sizeof(bier_db_status_t);
    layout_array_dnx_bier_db[idx].parent  = 0;
    layout_array_dnx_bier_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__INGRESS__FIRST);
    layout_array_dnx_bier_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__INGRESS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__EGRESS);
    layout_array_dnx_bier_db[idx].name = "dnx_bier_db__egress";
    layout_array_dnx_bier_db[idx].type = "bier_db_status_t";
    layout_array_dnx_bier_db[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].size_of = sizeof(bier_db_status_t);
    layout_array_dnx_bier_db[idx].parent  = 0;
    layout_array_dnx_bier_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__EGRESS__FIRST);
    layout_array_dnx_bier_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__EGRESS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__INGRESS__ACTIVE);
    layout_array_dnx_bier_db[idx].name = "dnx_bier_db__ingress__active";
    layout_array_dnx_bier_db[idx].type = "uint32";
    layout_array_dnx_bier_db[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].size_of = sizeof(uint32);
    layout_array_dnx_bier_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES;
    layout_array_dnx_bier_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_bier_db[idx].parent  = 2;
    layout_array_dnx_bier_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_BIER_DB__EGRESS__ACTIVE);
    layout_array_dnx_bier_db[idx].name = "dnx_bier_db__egress__active";
    layout_array_dnx_bier_db[idx].type = "uint32";
    layout_array_dnx_bier_db[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].size_of = sizeof(uint32);
    layout_array_dnx_bier_db[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES;
    layout_array_dnx_bier_db[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_bier_db[idx].parent  = 3;
    layout_array_dnx_bier_db[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_bier_db[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_BIER_DB, layout_array_dnx_bier_db, sw_state_layout_array[unit][DNX_BIER_DB_MODULE_ID], DNX_SW_STATE_DNX_BIER_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
