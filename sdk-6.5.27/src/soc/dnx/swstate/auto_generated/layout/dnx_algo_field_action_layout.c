
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_field_action_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_algo_field_action_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_algo_field_action[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW_NOF_PARAMS)];

shr_error_e
dnx_algo_field_action_init_layout_structure(int unit)
{
    dnx_field_group_t dnx_algo_field_action_sw__ipmf2__fes_state__field_group__default_val = DNX_FIELD_GROUP_INVALID;
    dnx_field_action_priority_t dnx_algo_field_action_sw__ipmf2__context_state__priority__default_val = DNX_FIELD_ACTION_PRIORITY_INVALID;
    dnx_field_group_t dnx_algo_field_action_sw__ipmf3__fes_state__field_group__default_val = DNX_FIELD_GROUP_INVALID;
    dnx_field_action_priority_t dnx_algo_field_action_sw__ipmf3__context_state__priority__default_val = DNX_FIELD_ACTION_PRIORITY_INVALID;
    dnx_field_group_t dnx_algo_field_action_sw__epmf__fes_state__field_group__default_val = DNX_FIELD_GROUP_INVALID;
    dnx_field_action_priority_t dnx_algo_field_action_sw__epmf__context_state__priority__default_val = DNX_FIELD_ACTION_PRIORITY_INVALID;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_sw_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_sw_t);
    layout_array_dnx_algo_field_action[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_ipmf2_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_ipmf2_t);
    layout_array_dnx_algo_field_action[idx].parent  = 0;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_ipmf3_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_ipmf3_t);
    layout_array_dnx_algo_field_action[idx].parent  = 0;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_epmf_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_epmf_t);
    layout_array_dnx_algo_field_action[idx].parent  = 0;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__FEM);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__fem";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_action_fem_priorities_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_action_fem_priorities_t);
    layout_array_dnx_algo_field_action[idx].parent  = 0;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__FEM__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__FEM__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FES_STATE);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2__fes_state";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_fes_state_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_fes_state_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 1;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FES_STATE__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FES_STATE__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__CONTEXT_STATE);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2__context_state";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_context_state_ipmf2_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_context_state_ipmf2_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 1;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__CONTEXT_STATE__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__CONTEXT_STATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FES_STATE__FIELD_GROUP);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2__fes_state__field_group";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_group_t";
    layout_array_dnx_algo_field_action[idx].default_value= &(dnx_algo_field_action_sw__ipmf2__fes_state__field_group__default_val);
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 5;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FES_STATE__PLACE_IN_FG);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2__fes_state__place_in_fg";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 5;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__FES_STATE__MASK_IS_ALLOC);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2__fes_state__mask_is_alloc";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 5;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF2__CONTEXT_STATE__PRIORITY);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf2__context_state__priority";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_action_priority_t";
    layout_array_dnx_algo_field_action[idx].default_value= &(dnx_algo_field_action_sw__ipmf2__context_state__priority__default_val);
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_action_priority_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 6;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FES_STATE);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3__fes_state";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_fes_state_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_fes_state_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 2;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FES_STATE__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FES_STATE__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__CONTEXT_STATE);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3__context_state";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_context_state_ipmf3_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_context_state_ipmf3_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 2;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__CONTEXT_STATE__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__CONTEXT_STATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FES_STATE__FIELD_GROUP);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3__fes_state__field_group";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_group_t";
    layout_array_dnx_algo_field_action[idx].default_value= &(dnx_algo_field_action_sw__ipmf3__fes_state__field_group__default_val);
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 11;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FES_STATE__PLACE_IN_FG);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3__fes_state__place_in_fg";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 11;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__FES_STATE__MASK_IS_ALLOC);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3__fes_state__mask_is_alloc";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 11;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__IPMF3__CONTEXT_STATE__PRIORITY);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__ipmf3__context_state__priority";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_action_priority_t";
    layout_array_dnx_algo_field_action[idx].default_value= &(dnx_algo_field_action_sw__ipmf3__context_state__priority__default_val);
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_action_priority_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 12;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FES_STATE);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf__fes_state";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_fes_state_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_fes_state_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 3;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FES_STATE__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FES_STATE__LAST)-1;
    layout_array_dnx_algo_field_action[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__CONTEXT_STATE);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf__context_state";
    layout_array_dnx_algo_field_action[idx].type = "dnx_algo_field_action_context_state_epmf_t";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_algo_field_action_context_state_epmf_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 3;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__CONTEXT_STATE__FIRST);
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__CONTEXT_STATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FES_STATE__FIELD_GROUP);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf__fes_state__field_group";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_group_t";
    layout_array_dnx_algo_field_action[idx].default_value= &(dnx_algo_field_action_sw__epmf__fes_state__field_group__default_val);
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 17;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FES_STATE__PLACE_IN_FG);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf__fes_state__place_in_fg";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 17;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__FES_STATE__MASK_IS_ALLOC);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf__fes_state__mask_is_alloc";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 17;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__EPMF__CONTEXT_STATE__PRIORITY);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__epmf__context_state__priority";
    layout_array_dnx_algo_field_action[idx].type = "dnx_field_action_priority_t";
    layout_array_dnx_algo_field_action[idx].default_value= &(dnx_algo_field_action_sw__epmf__context_state__priority__default_val);
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(dnx_field_action_priority_t);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 18;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW__FEM__IS_PRIORITY_POSITION);
    layout_array_dnx_algo_field_action[idx].name = "dnx_algo_field_action_sw__fem__is_priority_position";
    layout_array_dnx_algo_field_action[idx].type = "uint8";
    layout_array_dnx_algo_field_action[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].size_of = sizeof(uint8);
    layout_array_dnx_algo_field_action[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID;
    layout_array_dnx_algo_field_action[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].array_indexes[1].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_PROGRAMS;
    layout_array_dnx_algo_field_action[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_algo_field_action[idx].parent  = 4;
    layout_array_dnx_algo_field_action[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_algo_field_action[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW, layout_array_dnx_algo_field_action, sw_state_layout_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID], DNX_SW_STATE_DNX_ALGO_FIELD_ACTION_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
