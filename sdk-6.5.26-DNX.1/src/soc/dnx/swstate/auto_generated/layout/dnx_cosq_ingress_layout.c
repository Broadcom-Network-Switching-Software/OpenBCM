
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_cosq_ingress_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_cosq_ingress_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_cosq_ingress[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB_NOF_PARAMS)];

shr_error_e
dnx_cosq_ingress_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB);
    layout_array_dnx_cosq_ingress[idx].name = "dnx_cosq_ingress_db";
    layout_array_dnx_cosq_ingress[idx].type = "dnx_cosq_ingress_db_t";
    layout_array_dnx_cosq_ingress[idx].doc = "DB for used by cosq ingress";
    layout_array_dnx_cosq_ingress[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].size_of = sizeof(dnx_cosq_ingress_db_t);
    layout_array_dnx_cosq_ingress[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__FIRST);
    layout_array_dnx_cosq_ingress[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__FIXED_PRIORITY);
    layout_array_dnx_cosq_ingress[idx].name = "dnx_cosq_ingress_db__fixed_priority";
    layout_array_dnx_cosq_ingress[idx].type = "dnx_cosq_ingress_fixed_priority_t";
    layout_array_dnx_cosq_ingress[idx].doc = "ports with fixed (unchangable) priority";
    layout_array_dnx_cosq_ingress[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].size_of = sizeof(dnx_cosq_ingress_fixed_priority_t);
    layout_array_dnx_cosq_ingress[idx].parent  = 0;
    layout_array_dnx_cosq_ingress[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__FIXED_PRIORITY__FIRST);
    layout_array_dnx_cosq_ingress[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__FIXED_PRIORITY__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__FIXED_PRIORITY__NIF);
    layout_array_dnx_cosq_ingress[idx].name = "dnx_cosq_ingress_db__fixed_priority__nif";
    layout_array_dnx_cosq_ingress[idx].type = "bcm_pbmp_t";
    layout_array_dnx_cosq_ingress[idx].doc = "nif ports with fixed (unchangable) priority - indexed by phy port";
    layout_array_dnx_cosq_ingress[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].size_of = sizeof(bcm_pbmp_t);
    layout_array_dnx_cosq_ingress[idx].parent  = 1;
    layout_array_dnx_cosq_ingress[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_COSQ_INGRESS_DB__FIXED_PRIORITY__MIRROR);
    layout_array_dnx_cosq_ingress[idx].name = "dnx_cosq_ingress_db__fixed_priority__mirror";
    layout_array_dnx_cosq_ingress[idx].type = "SHR_BITDCL";
    layout_array_dnx_cosq_ingress[idx].doc = "mirror ports with fixed (unchangable) priority - indexed by core id * egress interface id";
    layout_array_dnx_cosq_ingress[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].size_of = sizeof(SHR_BITDCL);
    layout_array_dnx_cosq_ingress[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_cosq_ingress[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_cosq_ingress[idx].array_indexes[1].num_elements = ((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1;
    layout_array_dnx_cosq_ingress[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_dnx_cosq_ingress[idx].parent  = 1;
    layout_array_dnx_cosq_ingress[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_cosq_ingress[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_COSQ_INGRESS_DB, layout_array_dnx_cosq_ingress, sw_state_layout_array[unit][DNX_COSQ_INGRESS_MODULE_ID], DNX_SW_STATE_DNX_COSQ_INGRESS_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
