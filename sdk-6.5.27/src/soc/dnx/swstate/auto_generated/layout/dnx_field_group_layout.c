
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_group_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_group[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW_NOF_PARAMS)];

shr_error_e
dnx_field_group_init_layout_structure(int unit)
{
    dnx_field_stage_e dnx_field_group_sw__fg_info__field_stage__default_val = DNX_FIELD_STAGE_NOF;
    dnx_field_group_type_e dnx_field_group_sw__fg_info__fg_type__default_val = DNX_FIELD_GROUP_TYPE_NOF;
    dnx_field_key_length_type_e dnx_field_group_sw__fg_info__key_length_type__default_val = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
    dnx_field_action_length_type_e dnx_field_group_sw__fg_info__action_length_type__default_val = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;
    dbal_tables_e dnx_field_group_sw__fg_info__dbal_table_id__default_val = DBAL_TABLE_EMPTY;
    dbal_enum_value_field_field_key_e dnx_field_group_sw__fg_info__context_info__key_id__id__default_val = DNX_FIELD_KEY_ID_INVALID;
    dnx_field_input_type_e dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_type__default_val = DNX_FIELD_INPUT_TYPE_INVALID;
    int dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_arg__default_val = 0xFFFF;
    int dnx_field_group_sw__fg_info__context_info__qual_attach_info__offset__default_val = 0xFFFF;
    dnx_field_qual_t dnx_field_group_sw__fg_info__context_info__qual_attach_info__base_qual__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_qual_t dnx_field_group_sw__fg_info__key_template__key_qual_map__qual_type__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_action_t dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dnx_action__default_val = DNX_FIELD_ACTION_INVALID;
    dnx_field_group_cache_mode_e dnx_field_group_sw__fg_info__tcam_info__cache_mode__default_val = DNX_FIELD_GROUP_CACHE_MODE_CLEAR;
    dnx_field_ace_format_add_flags_e dnx_field_group_sw__ace_format_info__flags__default_val = DNX_FIELD_ACE_FORMAT_ADD_FLAG_NOF;
    dnx_field_action_t dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dnx_action__default_val = DNX_FIELD_ACTION_INVALID;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_sw_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_sw_t);
    layout_array_dnx_field_group[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_fg_sw_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_fg_sw_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = dnx_data_field.group.nof_fgs_get(unit);
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_group[idx].parent  = 0;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_ace_format_sw_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_ace_format_sw_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = dnx_data_field.ace.nof_ace_id_get(unit);
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_group[idx].parent  = 0;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FIELD_STAGE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__field_stage";
    layout_array_dnx_field_group[idx].type = "dnx_field_stage_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__field_stage__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_stage_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FG_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__fg_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_type_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__fg_type__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_type_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__NAME);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__name";
    layout_array_dnx_field_group[idx].type = "field_group_name_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(field_group_name_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__NAME__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__NAME__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_attach_context_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_attach_context_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template";
    layout_array_dnx_field_group[idx].type = "dnx_field_key_template_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_key_template_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_LENGTH_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_length_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_key_length_type_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__key_length_type__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_key_length_type_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTION_LENGTH_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__action_length_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_length_type_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__action_length_type__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_length_type_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_actions_fg_payload_sw_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_actions_fg_payload_sw_info_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__DBAL_TABLE_ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__dbal_table_id";
    layout_array_dnx_field_group[idx].type = "dbal_tables_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__dbal_table_id__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dbal_tables_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FLAGS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__flags";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_add_flags_e";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_add_flags_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_fg_tcam_sw_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_fg_tcam_sw_info_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__NAME__VALUE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__name__value";
    layout_array_dnx_field_group[idx].type = "char";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(char);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 5;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id";
    layout_array_dnx_field_group[idx].type = "dnx_field_key_id_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_key_id_t);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__CONTEXT_INFO_VALID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__context_info_valid";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__NOF_CASCADING_REFS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__nof_cascading_refs";
    layout_array_dnx_field_group[idx].type = "uint16";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__PAYLOAD_OFFSET);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__payload_offset";
    layout_array_dnx_field_group[idx].type = "uint32";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__PAYLOAD_ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__payload_id";
    layout_array_dnx_field_group[idx].type = "uint32";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__FLAGS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__flags";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_context_attach_flags_e";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_context_attach_flags_e);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id__id";
    layout_array_dnx_field_group[idx].type = "dbal_enum_value_field_field_key_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__key_id__id__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dbal_enum_value_field_field_key_e);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 15;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__OFFSET_ON_FIRST_KEY);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id__offset_on_first_key";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 15;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__NOF_BITS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id__nof_bits";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 15;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__INPUT_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_type__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__INPUT_ARG);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_arg";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_arg__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__OFFSET);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__offset";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__offset__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__BASE_QUAL);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__base_qual";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__base_qual__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_map_in_key_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_map_in_key_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 7;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_size_in_bits";
    layout_array_dnx_field_group[idx].type = "uint16";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_group[idx].parent  = 7;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map__qual_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__key_template__key_qual_map__qual_type__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_group[idx].parent  = 29;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map__lsb";
    layout_array_dnx_field_group[idx].type = "uint16";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_group[idx].parent  = 29;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map__size";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 29;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_in_group_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_in_group_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 10;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION | DNXC_SW_STATE_LAYOUT_LABEL_ATTRIBUTE_PACKED ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DNX_ACTION);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dnx_action";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dnx_action__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_group[idx].parent  = 34;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DONT_USE_VALID_BIT);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dont_use_valid_bit";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 34;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__LSB);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__lsb";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 34;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__AUTO_TCAM_BANK_SELECT);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__auto_tcam_bank_select";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__NOF_TCAM_BANKS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__nof_tcam_banks";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__TCAM_BANK_IDS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__tcam_bank_ids";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = dnx_data_field.tcam.nof_banks_get(unit);
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__CACHE_MODE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__cache_mode";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_cache_mode_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__tcam_info__cache_mode__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_cache_mode_e);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__NOF_VALID_ENTRIES);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__nof_valid_entries";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_actions_ace_payload_sw_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_actions_ace_payload_sw_info_t);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__FLAGS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__flags";
    layout_array_dnx_field_group[idx].type = "dnx_field_ace_format_add_flags_e";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__ace_format_info__flags__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_ace_format_add_flags_e);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__NAME);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__name";
    layout_array_dnx_field_group[idx].type = "field_ace_format_name_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(field_ace_format_name_t);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__NAME__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__NAME__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__USE_PPMC_RANGE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__use_ppmc_range";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_in_group_info_t";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_in_group_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 43;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__FIRST);
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__LAST)-1;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ATTRIBUTE_PACKED;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DNX_ACTION);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dnx_action";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dnx_action__default_val);
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_group[idx].parent  = 47;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DONT_USE_VALID_BIT);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dont_use_valid_bit";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 47;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__LSB);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__lsb";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 47;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__NAME__VALUE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__name__value";
    layout_array_dnx_field_group[idx].type = "char";
    layout_array_dnx_field_group[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(char);
    layout_array_dnx_field_group[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 45;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_GROUP_SW, layout_array_dnx_field_group, sw_state_layout_array[unit][DNX_FIELD_GROUP_MODULE_ID], DNX_SW_STATE_DNX_FIELD_GROUP_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
