
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_actions_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_actions[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB_NOF_PARAMS)];

shr_error_e
dnx_field_actions_init_layout_structure(int unit)
{
    dbal_fields_e dnx_field_action_sw_db__info__field_id__default_val = DBAL_FIELD_EMPTY;
    int dnx_field_action_sw_db__info__bcm_id__default_val = bcmFieldActionCount;
    dnx_field_action_t dnx_field_action_sw_db__info__base_dnx_action__default_val = DNX_FIELD_ACTION_INVALID;
    dnx_field_action_t dnx_field_action_sw_db__fem_info__fg_id_info__fem_encoded_actions__default_val = DNX_FIELD_ACTION_INVALID;
    dnx_field_group_t dnx_field_action_sw_db__fem_info__fg_id_info__second_fg_id__default_val = DNX_FIELD_GROUP_INVALID;
    dnx_field_group_t dnx_field_action_sw_db__fem_info__fg_id_info__fg_id__default_val = DNX_FIELD_GROUP_INVALID;
    uint8 dnx_field_action_sw_db__fem_info__fg_id_info__ignore_actions__default_val = DNX_FIELD_IGNORE_ALL_ACTIONS;
    dbal_fields_e dnx_field_action_sw_db__predefined__field_id__default_val = DBAL_FIELD_EMPTY;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_sw_db_t";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_sw_db_t);
    layout_array_dnx_field_actions[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info";
    layout_array_dnx_field_actions[idx].type = "dnx_field_user_action_info_t";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_user_action_info_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_actions[idx].parent  = 0;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__LAST)-1;
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info";
    layout_array_dnx_field_actions[idx].type = "dnx_field_fem_state_t";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_fem_state_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = dnx_data_field.stage.stage_info_get(unit,DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_actions[idx].parent  = 0;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__LAST)-1;
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__predefined";
    layout_array_dnx_field_actions[idx].type = "dnx_field_predefined_action_info_t";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_predefined_action_info_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_ACTION_PREDEFINED_NOF;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_actions[idx].parent  = 0;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED__LAST)-1;
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__FIELD_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__field_id";
    layout_array_dnx_field_actions[idx].type = "dbal_fields_e";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__info__field_id__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__FLAGS);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__flags";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_flags_e";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_flags_e);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__BCM_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__bcm_id";
    layout_array_dnx_field_actions[idx].type = "int";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__info__bcm_id__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(int);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__VALID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__valid";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__SIZE);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__size";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__PREFIX);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__prefix";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__REF_COUNT);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__ref_count";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__BASE_DNX_ACTION);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__base_dnx_action";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__info__base_dnx_action__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__PREFIX_SIZE);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__prefix_size";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info";
    layout_array_dnx_field_actions[idx].type = "dnx_field_fg_id_info_t";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_fg_id_info_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = dnx_data_field.stage.stage_info_get(unit,DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_actions[idx].parent  = 2;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__LAST)-1;
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__FEM_ENCODED_ACTIONS);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__fem_encoded_actions";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__fem_encoded_actions__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = dnx_data_field.stage.stage_info_get(unit,DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__SECOND_FG_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__second_fg_id";
    layout_array_dnx_field_actions[idx].type = "dnx_field_group_t";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__second_fg_id__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__FG_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__fg_id";
    layout_array_dnx_field_actions[idx].type = "dnx_field_group_t";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__fg_id__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__INPUT_OFFSET);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__input_offset";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__INPUT_SIZE);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__input_size";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__IGNORE_ACTIONS);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__ignore_actions";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__ignore_actions__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED__FIELD_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__predefined__field_id";
    layout_array_dnx_field_actions[idx].type = "dbal_fields_e";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__predefined__field_id__default_val);
    layout_array_dnx_field_actions[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_actions[idx].parent  = 3;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB, layout_array_dnx_field_actions, sw_state_layout_array[unit][DNX_FIELD_ACTIONS_MODULE_ID], DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
