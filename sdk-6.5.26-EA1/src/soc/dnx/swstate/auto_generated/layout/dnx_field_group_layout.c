
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
    layout_array_dnx_field_group[idx].doc = "Hold the needed info to manage Field group database module";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_sw_t);
    layout_array_dnx_field_group[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].first_child_index = 1;
    layout_array_dnx_field_group[idx].last_child_index = 2;
    layout_array_dnx_field_group[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_fg_sw_info_t*";
    layout_array_dnx_field_group[idx].doc = "Hold the information about the Field groups.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_fg_sw_info_t*);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_group[idx].parent  = 0;
    layout_array_dnx_field_group[idx].first_child_index = 3;
    layout_array_dnx_field_group[idx].last_child_index = 13;
    layout_array_dnx_field_group[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_ace_format_sw_info_t*";
    layout_array_dnx_field_group[idx].doc = "Hold the information about the ACE Formats.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_ace_format_sw_info_t*);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_group[idx].parent  = 0;
    layout_array_dnx_field_group[idx].first_child_index = 43;
    layout_array_dnx_field_group[idx].last_child_index = 46;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FIELD_STAGE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__field_stage";
    layout_array_dnx_field_group[idx].type = "dnx_field_stage_e";
    layout_array_dnx_field_group[idx].doc = "Field stage, for which stage the Database was created";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__field_stage__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_stage_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 4;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FG_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__fg_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_type_e";
    layout_array_dnx_field_group[idx].doc = " Field group type: The field group may be:TCAM, Direct Table, direct extraction...";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__fg_type__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_type_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__NAME);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__name";
    layout_array_dnx_field_group[idx].type = "field_group_name_t";
    layout_array_dnx_field_group[idx].doc = "Field group name.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(field_group_name_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = 14;
    layout_array_dnx_field_group[idx].last_child_index = 14;
    layout_array_dnx_field_group[idx].next_node_index = 6;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_attach_context_info_t";
    layout_array_dnx_field_group[idx].doc = "Hold the array of PMF programs that are attached to Field Group \n                                                          each index in array is related to PMF program ID \n                                                          I.e PMF program 7 information is located in index 7";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_attach_context_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = 15;
    layout_array_dnx_field_group[idx].last_child_index = 21;
    layout_array_dnx_field_group[idx].next_node_index = 7;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template";
    layout_array_dnx_field_group[idx].type = "dnx_field_key_template_t";
    layout_array_dnx_field_group[idx].doc = "Mapping of the qualifier inside the key";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_key_template_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = 29;
    layout_array_dnx_field_group[idx].last_child_index = 30;
    layout_array_dnx_field_group[idx].next_node_index = 8;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_LENGTH_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_length_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_key_length_type_e";
    layout_array_dnx_field_group[idx].doc = "This enum indicate the size of the Key, some Field Groups \n                                                                                        use only part of the key";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__key_length_type__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_key_length_type_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 9;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTION_LENGTH_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__action_length_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_length_type_e";
    layout_array_dnx_field_group[idx].doc = "This enum indicate the size of the payload block used. \n                                                                                         For example a TCAM field group can use 32, 64 or 128 bit payloads.\n                                                                                         Not used by HASH or direct extraction field groups.";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__action_length_type__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_length_type_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 10;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_actions_fg_payload_sw_info_t";
    layout_array_dnx_field_group[idx].doc = "Hold the information regarding the configured actions for Field Group database \n                                                             Later will also be used by 'entry_add' API to construct 'payload' for TCAM lookup result \n                                                             in same order as action were placed in action place method";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_actions_fg_payload_sw_info_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = 34;
    layout_array_dnx_field_group[idx].last_child_index = 34;
    layout_array_dnx_field_group[idx].next_node_index = 11;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__DBAL_TABLE_ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__dbal_table_id";
    layout_array_dnx_field_group[idx].type = "dbal_tables_e";
    layout_array_dnx_field_group[idx].doc = "Holds the ID of DBAL table that was created of pair (key,action_payload) which \n                                                             is later used by entry_add to add entries to TCAM";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__dbal_table_id__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dbal_tables_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 12;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__FLAGS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__flags";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_add_flags_e";
    layout_array_dnx_field_group[idx].doc = "Flags with which Field Group was created, needed for Get functionality";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_add_flags_e);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 13;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_fg_tcam_sw_info_t";
    layout_array_dnx_field_group[idx].doc = "Holds the TCAM information that needs to be saved in SWSTATE for a TCAM FG";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_fg_tcam_sw_info_t);
    layout_array_dnx_field_group[idx].parent  = 1;
    layout_array_dnx_field_group[idx].first_child_index = 38;
    layout_array_dnx_field_group[idx].last_child_index = 42;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__NAME__VALUE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__name__value";
    layout_array_dnx_field_group[idx].type = "char";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(char);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DBAL_MAX_STRING_LENGTH;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 5;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id";
    layout_array_dnx_field_group[idx].type = "dnx_field_key_id_t";
    layout_array_dnx_field_group[idx].doc = "Hold the information regarding the key allocated for the Context ID \n                            Several usages: \n                              1. KBR config in the Key module \n                              2. FEM/FES config ";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_key_id_t);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = 22;
    layout_array_dnx_field_group[idx].last_child_index = 24;
    layout_array_dnx_field_group[idx].next_node_index = 16;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_group[idx].doc = "Attach Info of the qualifiers inside the key. \n                             Indexes match the qual_type in the key_template";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = 25;
    layout_array_dnx_field_group[idx].last_child_index = 28;
    layout_array_dnx_field_group[idx].next_node_index = 17;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__CONTEXT_INFO_VALID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__context_info_valid";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "If set to True, the information was set, i.e. program (context) was attached";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 18;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__NOF_CASCADING_REFS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__nof_cascading_refs";
    layout_array_dnx_field_group[idx].type = "uint16";
    layout_array_dnx_field_group[idx].doc = "Holds the number of qualifiers in iPMF2 that are cascading from this FG on this context (only relevant for iPMF1 FGs).\n                      When the value of this variable is larger than 0, the FG can't be detached from the context.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 19;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__PAYLOAD_OFFSET);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__payload_offset";
    layout_array_dnx_field_group[idx].type = "uint32";
    layout_array_dnx_field_group[idx].doc = "Payload location inside the payload buffer returned from the external tcam.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 20;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__PAYLOAD_ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__payload_id";
    layout_array_dnx_field_group[idx].type = "uint32";
    layout_array_dnx_field_group[idx].doc = "Payload Id for current field group inside the payload buffer. Payloads returned from the external \n                             tcam are acumulated sequentially. Indicates the valid hit bit for current field group.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 21;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__FLAGS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__flags";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_context_attach_flags_e";
    layout_array_dnx_field_group[idx].doc = "Flags with which Field Group was attached to a context.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_context_attach_flags_e);
    layout_array_dnx_field_group[idx].parent  = 6;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__ID);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id__id";
    layout_array_dnx_field_group[idx].type = "dbal_enum_value_field_field_key_e";
    layout_array_dnx_field_group[idx].doc = "Hold the information regarding the key allocated for the Context ID";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__key_id__id__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dbal_enum_value_field_field_key_e);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 15;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 23;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__OFFSET_ON_FIRST_KEY);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id__offset_on_first_key";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "Hold the lsb of the allocated bits within the first key.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 15;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 24;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__KEY_ID__NOF_BITS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__key_id__nof_bits";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "Hold the number of bits allocated within the key.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 15;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__INPUT_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_group[idx].doc = "The base of qualifier indicates from which part of the PBUS the info is taken                                                                          \n relevant for all type of qualifiers                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_FWD  - Takes data from a layer in the header, using the forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE - Takes data from a layer in the header, ignoring forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA - Takes data from the Metadata.                                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA2 - Takes data from the native Metadata of iPMF2, relevant only for iPMF2.                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD  -  Takes data about a layer in the header (layer record), using the forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE - Takes data about a layer in the header (layer record), ignoring the forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_EXTERNAL - Takes data from the results of an external stage field group.                                                                          \n DNX_FIELD_INPUT_TYPE_CASCADED - Takes data from the result of a field group from an earlier PMF stage, using cascading.                                                                          \n DNX_FIELD_INPUT_TYPE_CONST - Writes a constant value.";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_type__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 26;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__INPUT_ARG);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_arg";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].doc = " Input arg has different meaning per input type as follows:                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_FWD  - how many layer to increment from the forwarding layer (i.e. FWD+input_arg). Can take negative or positive values.                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE - how many layer to increment from the outermost layer (i.e. 0+input_arg). Can only take non-negative values.                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA - Has no meaning                                                           \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD  -  how many layer to increment from the forwarding layer (i.e. FWD+input_arg). Can take negative or positive values.                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE - how many layer to increment from the absolute layer (i.e. 0+input_arg). Can only take non-negative values.                                                          \n DNX_FIELD_INPUT_TYPE_EXTERNAL - Has no meaning                                                           \n DNX_FIELD_INPUT_TYPE_CASCADED - Field group Id of the 'Cascaded from' result                                                          \n DNX_FIELD_INPUT_TYPE_CONST - A constant, up to 32 bit.";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__input_arg__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 27;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__OFFSET);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__offset";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].doc = "1) This filed is bit resolution                                                          \n 2) In case the qualifier was created by dnx_field_qual_create() this field is always relevant                                                          \n    it is the offset of the field inside the PBUS (MD/LR/Layer)                                                          \n 3) For predefined qualifiers based on input_type:                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_FWD  - offset inside the layer  (chosen by input_type+input_arg)                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE - offset inside the layer  (chosen by input_type+input_arg)                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA - NA                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD  -  NA                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE - NA                                                          \n DNX_FIELD_INPUT_TYPE_EXTERNAL - offset of the field group in the external stage result buffer, plus the offset of the action withing the field group.                                                          \n DNX_FIELD_INPUT_TYPE_CASCADED - offset in the field group of 'Cascaded from' result buffer                                                          \n DNX_FIELD_INPUT_TYPE_CONST - NA";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__offset__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 28;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__CONTEXT_INFO__QUAL_ATTACH_INFO__BASE_QUAL);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__context_info__qual_attach_info__base_qual";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_group[idx].doc = "The qualifier to be based upon. used for user defined qualifers that relate to split predefined qualifers.";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__context_info__qual_attach_info__base_qual__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_group[idx].parent  = 16;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_map_in_key_t";
    layout_array_dnx_field_group[idx].doc = "Mapping of each qualifier inside the key. End of 'valid' entries is marked \n                            by an entry with 'key_qual_map.qual_type = DNX_FIELD_QUAL_TYPE_INVALID'";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_map_in_key_t);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 7;
    layout_array_dnx_field_group[idx].first_child_index = 31;
    layout_array_dnx_field_group[idx].last_child_index = 33;
    layout_array_dnx_field_group[idx].next_node_index = 30;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_size_in_bits";
    layout_array_dnx_field_group[idx].type = "uint16";
    layout_array_dnx_field_group[idx].doc = "Hold the actual key size in Bit Resolution,\n                            i.e. sum, in bits, of all qualifiers configured for this key";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_group[idx].parent  = 7;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map__qual_type";
    layout_array_dnx_field_group[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_group[idx].doc = "Qualifier type";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__key_template__key_qual_map__qual_type__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_group[idx].parent  = 29;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 32;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map__lsb";
    layout_array_dnx_field_group[idx].type = "uint16";
    layout_array_dnx_field_group[idx].doc = "LSB bit of the qualifier inside the key";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_group[idx].parent  = 29;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 33;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__key_template__key_qual_map__size";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "Size of the qualifier in the key";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 29;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_in_group_info_t";
    layout_array_dnx_field_group[idx].doc = " Holds all the relevant info to constuct a payload of entry for a Field Group.\n                                                                         Each action, configured for Field Group, is specified by size (bits) and place (lsb) \n                                                                         End of list of valid entries is marked by an entry with 'dnx_action = DNX_FIELD_ACTION_INVALID'";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_in_group_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 10;
    layout_array_dnx_field_group[idx].first_child_index = 35;
    layout_array_dnx_field_group[idx].last_child_index = 37;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION | DNXC_SW_STATE_LAYOUT_LABEL_ATTRIBUTE_PACKED ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DNX_ACTION);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dnx_action";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_group[idx].doc = "DNX 'action'.                                                                          \n Encoded with ACTION CLASS, Stage and ID                                                                          \n If either                                                                          \n   db_info_dnx_action is DNX_FIELD_ACTION_INVALID or                                                                          \n   db_info_actions_size_wo_valid is '0'                                                                          \n then all other elements below are meaningless.                                                                          \n                                                                          \n It is assumed that the first 'db_info_dnx_action' which is marked 'invalid' (DNX_FIELD_ACTION_INVALID)                                                                          \n indicates the end of valid entries.\n";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dnx_action__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_group[idx].parent  = 34;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 36;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DONT_USE_VALID_BIT);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__dont_use_valid_bit";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "Indication on whether this action uses its ls bit                                                                                  \n as 'valid' indication for this action:                                                                                  \n If 'dont_use_valid_bit' is '1' then lsb is NOT used as 'valid' indication                                                                                  \n If 'dont_use_valid_bit' is '0' then lsb IS used as 'valid' indication.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 34;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 37;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__LSB);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__actions_payload_info__actions_on_payload_info__lsb";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "LS bit of this action int the payload.\n                               If the action uses valid bit, this is the location of the valid bit.\n                                                       If the action doesn't use valid bit, this is the location of the lsb of the action.\n                                              \n                                               Here is an example to clarify what LSB stands for:\n                                               Suppose the size of 'actions_block' is 32 and suppose\n                                               it was loaded by 0x11223344 and suppose a specific action, XX, is\n                                               located at LSB '0' and its length is 8 bits then its value is 0x44.\n                                               Suppose the following action is located at LSB '8' and its\n                                               size is 4 bits then its value would be 0x3 and its 'index_within_block'\n                                               would be '1'.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 34;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__AUTO_TCAM_BANK_SELECT);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__auto_tcam_bank_select";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].doc = "If true, forces SDK to auto select TCAM bank ids to be reserved, ignoring tcam_bank_ids";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 39;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__NOF_TCAM_BANKS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__nof_tcam_banks";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].doc = "Number of TCAM banks to reserve for the given field group.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 40;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__TCAM_BANK_IDS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__tcam_bank_ids";
    layout_array_dnx_field_group[idx].type = "int*";
    layout_array_dnx_field_group[idx].doc = "The list of TCAM bank IDs to reserve for the field group to be created.\n                            This list is only valid for TCAM field groups.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int*);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 41;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__CACHE_MODE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__cache_mode";
    layout_array_dnx_field_group[idx].type = "dnx_field_group_cache_mode_e";
    layout_array_dnx_field_group[idx].doc = "TCAM cache mode";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__fg_info__tcam_info__cache_mode__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_group_cache_mode_e);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 42;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__FG_INFO__TCAM_INFO__NOF_VALID_ENTRIES);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__fg_info__tcam_info__nof_valid_entries";
    layout_array_dnx_field_group[idx].type = "int";
    layout_array_dnx_field_group[idx].doc = "Number of valid TCAM entrieson cache";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(int);
    layout_array_dnx_field_group[idx].parent  = 13;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_actions_ace_payload_sw_info_t";
    layout_array_dnx_field_group[idx].doc = "Hold the information regarding the configured actions for the ACE Format. \n                                                             Later will also be used by 'entry_add' API to construct 'payload' for PPMC lookup result \n                                                             in same order as action were placed in action place method";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_actions_ace_payload_sw_info_t);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = 47;
    layout_array_dnx_field_group[idx].last_child_index = 47;
    layout_array_dnx_field_group[idx].next_node_index = 44;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__FLAGS);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__flags";
    layout_array_dnx_field_group[idx].type = "dnx_field_ace_format_add_flags_e";
    layout_array_dnx_field_group[idx].doc = "Flags with which ACE Format was created, needed for Get functionality";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__ace_format_info__flags__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_ace_format_add_flags_e);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 45;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__NAME);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__name";
    layout_array_dnx_field_group[idx].type = "field_ace_format_name_t";
    layout_array_dnx_field_group[idx].doc = "ACE Format name.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(field_ace_format_name_t);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = 51;
    layout_array_dnx_field_group[idx].last_child_index = 51;
    layout_array_dnx_field_group[idx].next_node_index = 46;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__USE_PPMC_RANGE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__use_ppmc_range";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "The ACE format uses the entry range of PPMC entries.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 2;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_in_group_info_t";
    layout_array_dnx_field_group[idx].doc = " Holds all the relevant info to constuct a payload of entry for an ACE format.\n                                                                         Each action is specified by size (bits) and place (lsb) \n                                                                         End of list of valid entries is marked by an entry with 'dnx_action = DNX_FIELD_ACTION_INVALID'";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_in_group_info_t);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 43;
    layout_array_dnx_field_group[idx].first_child_index = 48;
    layout_array_dnx_field_group[idx].last_child_index = 50;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ATTRIBUTE_PACKED;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DNX_ACTION);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dnx_action";
    layout_array_dnx_field_group[idx].type = "dnx_field_action_t";
    layout_array_dnx_field_group[idx].doc = "DNX 'action'.                                                                          \n Encoded with ACTION CLASS, Stage and ID                                                                          \n If either                                                                          \n   db_info_dnx_action is DNX_FIELD_ACTION_INVALID or                                                                          \n   db_info_actions_size_wo_valid is '0'                                                                          \n then all other elements below are meaningless.                                                                          \n                                                                          \n It is assumed that the first 'db_info_dnx_action' which is marked 'invalid' (DNX_FIELD_ACTION_INVALID)                                                                          \n indicates the end of valid entries.\n";
    layout_array_dnx_field_group[idx].default_value= &(dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dnx_action__default_val);
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(dnx_field_action_t);
    layout_array_dnx_field_group[idx].parent  = 47;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 49;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__DONT_USE_VALID_BIT);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__dont_use_valid_bit";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "Indication on whether this action uses its ls bit                                                                                  \n as 'valid' indication for this action:                                                                                  \n If 'dont_use_valid_bit' is '1' then lsb is NOT used as 'valid' indication                                                                                  \n If 'dont_use_valid_bit' is '0' then lsb IS used as 'valid' indication.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 47;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = 50;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__ACTIONS_PAYLOAD_INFO__ACTIONS_ON_PAYLOAD_INFO__LSB);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__actions_payload_info__actions_on_payload_info__lsb";
    layout_array_dnx_field_group[idx].type = "uint8";
    layout_array_dnx_field_group[idx].doc = "LS bit of this action int the payload.\n                               If the action uses valid bit, this is the location of the valid bit.\n                                                       If the action doesn't use valid bit, this is the location of the lsb of the action.\n                                              \n                                               Here is an example to clarify what LSB stands for:\n                                               Suppose the size of 'actions_block' is 32 and suppose\n                                               it was loaded by 0x11223344 and suppose a specific action, XX, is\n                                               located at LSB '0' and its length is 8 bits then its value is 0x44.\n                                               Suppose the following action is located at LSB '8' and its\n                                               size is 4 bits then its value would be 0x3 and its 'index_within_block'\n                                               would be '1'.";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_group[idx].parent  = 47;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_GROUP_SW__ACE_FORMAT_INFO__NAME__VALUE);
    layout_array_dnx_field_group[idx].name = "dnx_field_group_sw__ace_format_info__name__value";
    layout_array_dnx_field_group[idx].type = "char";
    layout_array_dnx_field_group[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].size_of = sizeof(char);
    layout_array_dnx_field_group[idx].array_indexes[0].size = DBAL_MAX_STRING_LENGTH;
    layout_array_dnx_field_group[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_group[idx].parent  = 45;
    layout_array_dnx_field_group[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_group[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_GROUP_SW, layout_array_dnx_field_group, sw_state_layout_array[unit][DNX_FIELD_GROUP_MODULE_ID], DNX_SW_STATE_DNX_FIELD_GROUP_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
