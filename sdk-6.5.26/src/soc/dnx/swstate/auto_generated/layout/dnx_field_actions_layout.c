
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
    layout_array_dnx_field_actions[idx].doc = "Sw State Db of Action Field Sub-Module.";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_sw_db_t);
    layout_array_dnx_field_actions[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info";
    layout_array_dnx_field_actions[idx].type = "dnx_field_user_action_info_t";
    layout_array_dnx_field_actions[idx].doc = "Hold the Action params";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_user_action_info_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_actions[idx].parent  = 0;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__LAST);
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info";
    layout_array_dnx_field_actions[idx].type = "dnx_field_fem_state_t*";
    layout_array_dnx_field_actions[idx].doc = "Hold information on params of each FEM and allocations related to each FEM";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_fem_state_t*);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = dnx_data_field.stage.stage_info_get(unit,DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_actions[idx].parent  = 0;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__LAST);
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__predefined";
    layout_array_dnx_field_actions[idx].type = "dnx_field_predefined_action_info_t";
    layout_array_dnx_field_actions[idx].doc = "Hold the predefined Actions info";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_predefined_action_info_t);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_ACTION_PREDEFINED_NOF;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_actions[idx].parent  = 0;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED__LAST);
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__FIELD_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__field_id";
    layout_array_dnx_field_actions[idx].type = "dbal_fields_e";
    layout_array_dnx_field_actions[idx].doc = "DBAL field id";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__info__field_id__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__FLAGS);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__flags";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_flags_e";
    layout_array_dnx_field_actions[idx].doc = "DNX level flags.\n Saved for Get function to return all configured flags";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_flags_e);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__BCM_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__bcm_id";
    layout_array_dnx_field_actions[idx].type = "int";
    layout_array_dnx_field_actions[idx].doc = "BCM user action ID that maps to the user defined qualifier";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__info__bcm_id__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(int);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__VALID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__valid";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].doc = "TRUE if the entry is valid, false otherwise";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__SIZE);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__size";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].doc = "Size of the created action";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__PREFIX);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__prefix";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].doc = "A constant value to be appended to the action value at the MSB.";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__REF_COUNT);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__ref_count";
    layout_array_dnx_field_actions[idx].type = "uint32";
    layout_array_dnx_field_actions[idx].doc = "Number of Field Groups referring to this action";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__BASE_DNX_ACTION);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__base_dnx_action";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_actions[idx].doc = "The DNX action upon which the user defined action is based.\n                                                                           Contains the DNX action ID, the stage and the class of the action.";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__info__base_dnx_action__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__INFO__PREFIX_SIZE);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__info__prefix_size";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].doc = "The number of bits in the prefix.";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 1;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info";
    layout_array_dnx_field_actions[idx].type = "dnx_field_fg_id_info_t*";
    layout_array_dnx_field_actions[idx].doc = "Information on all potential Field Groups which use this FEM. \n                     A value of 'fg_id=DNX_FIELD_GROUP_INVALID' indicates invalid entry. \n                     We currently allow one field group per one pair ('FEM id','FEM program')";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_fg_id_info_t*);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = dnx_data_field.stage.stage_info_get(unit,DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_actions[idx].parent  = 2;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__FIRST);
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__LAST);
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__FEM_ENCODED_ACTIONS);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__fem_encoded_actions";
    layout_array_dnx_field_actions[idx].type = "dnx_field_action_t*";
    layout_array_dnx_field_actions[idx].doc = "List of the 'encoded' actions which were specified for this FEM on this selected 'FEM programs'. \n                     A value of 'DNX_FIELD_ACTION_INVALID' indicates invalid entry.";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__fem_encoded_actions__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_action_t*);
    layout_array_dnx_field_actions[idx].array_indexes[0].num_elements = dnx_data_field.stage.stage_info_get(unit,DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    layout_array_dnx_field_actions[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__SECOND_FG_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__second_fg_id";
    layout_array_dnx_field_actions[idx].type = "dnx_field_group_t";
    layout_array_dnx_field_actions[idx].doc = "Identifier of a TCAM field group from whose result, the LS 16 bits are to be taken. \n                     A value of DNX_FIELD_GROUP_INVALID indicates 'no such field group'.";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__second_fg_id__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__FG_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__fg_id";
    layout_array_dnx_field_actions[idx].type = "dnx_field_group_t";
    layout_array_dnx_field_actions[idx].doc = "If this Field group uses this FEM, this is its identifier. \n                     A value of DNX_FIELD_GROUP_INVALID indicates 'no field group'.\n                     In that case, all other parameters corresponding to field group, are not meaningful.";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__fg_id__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__INPUT_OFFSET);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__input_offset";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].doc = "Location (starting from LS bit of meaningful data), on 'key template', where the \n                     32-bits, known as 'fem key select', are to be taken. This is the location within\n                     the meaningful bits only. See header of dnx_field_fem_action_add(). See 'fg_id' below.\n                     Note that location on actual input to FEM, such as, say, key I, is set after allocation,\n                     on 'context attach', and can be found on swstate. See dnx_field_group_context_info_get()";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__INPUT_SIZE);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__input_size";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].doc = "Number of meaningful, on input to FEM, on the 'chuck' referred to, \n                     as 'fem key select'. See header of dnx_field_fem_action_add(). See 'fg_id' below.";
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__FEM_INFO__FG_ID_INFO__IGNORE_ACTIONS);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__fem_info__fg_id_info__ignore_actions";
    layout_array_dnx_field_actions[idx].type = "uint8";
    layout_array_dnx_field_actions[idx].doc = "Bit map of 4 bits. If a bit is non-zero then corrsponding 'fem_action_entry' and 'fem_adder' \n                   are ignored. For example, if BIT(0) is '1' then 'fem_action_entry[0]' and 'fem_adder[0]' \n                   are ignored. This allows for actions of the same FEM to be used, via a few 'FEM programs', \n                   by more than one FG or, even, by the same FG (but for a different set of conditions). \n                   Note that if the caller tries to overwrite an action/adder which is NOT marked as 'invalid' \n                   then this procedure will eject an error!.";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__fem_info__fg_id_info__ignore_actions__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_actions[idx].parent  = 13;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB__PREDEFINED__FIELD_ID);
    layout_array_dnx_field_actions[idx].name = "dnx_field_action_sw_db__predefined__field_id";
    layout_array_dnx_field_actions[idx].type = "dbal_fields_e";
    layout_array_dnx_field_actions[idx].doc = "DBAL field id";
    layout_array_dnx_field_actions[idx].default_value= &(dnx_field_action_sw_db__predefined__field_id__default_val);
    layout_array_dnx_field_actions[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_actions[idx].parent  = 3;
    layout_array_dnx_field_actions[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_actions[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB, layout_array_dnx_field_actions, sw_state_layout_array[unit][DNX_FIELD_ACTIONS_MODULE_ID], DNX_SW_STATE_DNX_FIELD_ACTION_SW_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
