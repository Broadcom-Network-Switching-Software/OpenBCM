
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_context_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_context_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_context[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW_NOF_PARAMS)];

shr_error_e
dnx_field_context_init_layout_structure(int unit)
{
    dnx_field_context_compare_mode_e dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__mode__default_val = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
    dnx_field_qual_t dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_qual_map__qual_type__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dbal_enum_value_field_field_key_e dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_id__id__default_val = DNX_FIELD_KEY_ID_INVALID;
    dnx_field_input_type_e dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__input_type__default_val = DNX_FIELD_INPUT_TYPE_INVALID;
    int dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__input_arg__default_val = 0xFFFF;
    int dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__offset__default_val = 0xFFFF;
    dnx_field_qual_t dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__base_qual__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_context_compare_mode_e dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__mode__default_val = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
    dnx_field_qual_t dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_qual_map__qual_type__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dbal_enum_value_field_field_key_e dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_id__id__default_val = DNX_FIELD_KEY_ID_INVALID;
    dnx_field_input_type_e dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__input_type__default_val = DNX_FIELD_INPUT_TYPE_INVALID;
    int dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__input_arg__default_val = 0xFFFF;
    int dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__offset__default_val = 0xFFFF;
    dnx_field_qual_t dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__base_qual__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_context_hash_mode_e dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__mode__default_val = DNX_FIELD_CONTEXT_HASH_MODE_DISABLED;
    dnx_field_qual_t dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_qual_map__qual_type__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dbal_enum_value_field_field_key_e dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_id__id__default_val = DNX_FIELD_KEY_ID_INVALID;
    dnx_field_input_type_e dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__input_type__default_val = DNX_FIELD_INPUT_TYPE_INVALID;
    int dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__input_arg__default_val = 0xFFFF;
    int dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__offset__default_val = 0xFFFF;
    dnx_field_qual_t dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__base_qual__default_val = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_context_t dnx_field_context_sw__context_info__context_ipmf2_info__cascaded_from__default_val = DNX_FIELD_CONTEXT_ID_INVALID;
    dnx_field_context_state_table_mode_e dnx_field_context_sw__context_info__state_table_info__mode__default_val = DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_sw_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_sw_t);
    layout_array_dnx_field_context[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_sw_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_sw_info_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = dnx_data_field.common_max_val.nof_contexts_get(unit);
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_context[idx].parent  = 0;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_ipmf1_sw_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_ipmf1_sw_info_t);
    layout_array_dnx_field_context[idx].parent  = 1;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__LAST)-1;
    layout_array_dnx_field_context[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf2_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_ipmf2_sw_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_ipmf2_sw_info_t);
    layout_array_dnx_field_context[idx].parent  = 1;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__LAST)-1;
    layout_array_dnx_field_context[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__state_table_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_state_table_sw_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_state_table_sw_info_t);
    layout_array_dnx_field_context[idx].parent  = 1;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__LAST)-1;
    layout_array_dnx_field_context[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__name";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_name_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_name_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_FIELD_STAGE_NOF;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 1;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_compare_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_compare_info_t);
    layout_array_dnx_field_context[idx].parent  = 3;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__LAST)-1;
    layout_array_dnx_field_context[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_hashing_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_hashing_info_t);
    layout_array_dnx_field_context[idx].parent  = 3;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__LAST)-1;
    layout_array_dnx_field_context[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_compare_mode_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_compare_mode_info_t);
    layout_array_dnx_field_context[idx].parent  = 8;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_compare_mode_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_compare_mode_info_t);
    layout_array_dnx_field_context[idx].parent  = 8;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__MODE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__mode";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_compare_mode_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__mode__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_compare_mode_e);
    layout_array_dnx_field_context[idx].parent  = 10;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__IS_SET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__is_set";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 10;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_key_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_key_info_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 10;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template";
    layout_array_dnx_field_context[idx].type = "dnx_field_key_template_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_key_template_t);
    layout_array_dnx_field_context[idx].parent  = 14;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_id";
    layout_array_dnx_field_context[idx].type = "dnx_field_key_id_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_key_id_t);
    layout_array_dnx_field_context[idx].parent  = 14;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 14;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_qual_map";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_map_in_key_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_map_in_key_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 15;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_size_in_bits";
    layout_array_dnx_field_context[idx].type = "uint16";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_context[idx].parent  = 15;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_qual_map__qual_type";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_qual_map__qual_type__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_context[idx].parent  = 18;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_qual_map__lsb";
    layout_array_dnx_field_context[idx].type = "uint16";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_context[idx].parent  = 18;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_template__key_qual_map__size";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 18;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__ID);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_id__id";
    layout_array_dnx_field_context[idx].type = "dbal_enum_value_field_field_key_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_id__id__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dbal_enum_value_field_field_key_e);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 16;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_id__offset_on_first_key";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 16;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__KEY_ID__NOF_BITS);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__key_id__nof_bits";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 16;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_TYPE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__input_type";
    layout_array_dnx_field_context[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__input_type__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_context[idx].parent  = 17;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__INPUT_ARG);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__input_arg";
    layout_array_dnx_field_context[idx].type = "int";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__input_arg__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(int);
    layout_array_dnx_field_context[idx].parent  = 17;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__OFFSET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__offset";
    layout_array_dnx_field_context[idx].type = "int";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__offset__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(int);
    layout_array_dnx_field_context[idx].parent  = 17;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_1__KEY_INFO__ATTACH_INFO__BASE_QUAL);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__base_qual";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_1__key_info__attach_info__base_qual__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_context[idx].parent  = 17;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__MODE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__mode";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_compare_mode_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__mode__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_compare_mode_e);
    layout_array_dnx_field_context[idx].parent  = 11;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__IS_SET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__is_set";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 11;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_key_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_key_info_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_COMPARE_NOF_KEYS;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 11;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template";
    layout_array_dnx_field_context[idx].type = "dnx_field_key_template_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_key_template_t);
    layout_array_dnx_field_context[idx].parent  = 32;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_id";
    layout_array_dnx_field_context[idx].type = "dnx_field_key_id_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_key_id_t);
    layout_array_dnx_field_context[idx].parent  = 32;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 32;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_qual_map";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_map_in_key_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_map_in_key_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 33;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_size_in_bits";
    layout_array_dnx_field_context[idx].type = "uint16";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_context[idx].parent  = 33;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_qual_map__qual_type";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_qual_map__qual_type__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_context[idx].parent  = 36;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_qual_map__lsb";
    layout_array_dnx_field_context[idx].type = "uint16";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_context[idx].parent  = 36;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_template__key_qual_map__size";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 36;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__ID);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_id__id";
    layout_array_dnx_field_context[idx].type = "dbal_enum_value_field_field_key_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_id__id__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dbal_enum_value_field_field_key_e);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 34;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_id__offset_on_first_key";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 34;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__KEY_ID__NOF_BITS);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__key_id__nof_bits";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 34;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_TYPE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__input_type";
    layout_array_dnx_field_context[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__input_type__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_context[idx].parent  = 35;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__INPUT_ARG);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__input_arg";
    layout_array_dnx_field_context[idx].type = "int";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__input_arg__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(int);
    layout_array_dnx_field_context[idx].parent  = 35;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__OFFSET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__offset";
    layout_array_dnx_field_context[idx].type = "int";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__offset__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(int);
    layout_array_dnx_field_context[idx].parent  = 35;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__COMPARE_INFO__PAIR_2__KEY_INFO__ATTACH_INFO__BASE_QUAL);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__base_qual";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__compare_info__pair_2__key_info__attach_info__base_qual__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_context[idx].parent  = 35;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__MODE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__mode";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_hash_mode_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__mode__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_hash_mode_e);
    layout_array_dnx_field_context[idx].parent  = 9;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__IS_SET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__is_set";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 9;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_key_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_key_info_t);
    layout_array_dnx_field_context[idx].parent  = 9;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template";
    layout_array_dnx_field_context[idx].type = "dnx_field_key_template_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_key_template_t);
    layout_array_dnx_field_context[idx].parent  = 50;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_id";
    layout_array_dnx_field_context[idx].type = "dnx_field_key_id_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_key_id_t);
    layout_array_dnx_field_context[idx].parent  = 50;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 50;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_qual_map";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_map_in_key_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_map_in_key_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 51;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__FIRST);
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_SIZE_IN_BITS);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_size_in_bits";
    layout_array_dnx_field_context[idx].type = "uint16";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_context[idx].parent  = 51;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__QUAL_TYPE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_qual_map__qual_type";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_qual_map__qual_type__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_context[idx].parent  = 54;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__LSB);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_qual_map__lsb";
    layout_array_dnx_field_context[idx].type = "uint16";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint16);
    layout_array_dnx_field_context[idx].parent  = 54;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_TEMPLATE__KEY_QUAL_MAP__SIZE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_template__key_qual_map__size";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 54;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__ID);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_id__id";
    layout_array_dnx_field_context[idx].type = "dbal_enum_value_field_field_key_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_id__id__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dbal_enum_value_field_field_key_e);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 52;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__OFFSET_ON_FIRST_KEY);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_id__offset_on_first_key";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 52;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__KEY_ID__NOF_BITS);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__key_id__nof_bits";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_context[idx].parent  = 52;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_TYPE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__input_type";
    layout_array_dnx_field_context[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__input_type__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_context[idx].parent  = 53;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__INPUT_ARG);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__input_arg";
    layout_array_dnx_field_context[idx].type = "int";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__input_arg__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(int);
    layout_array_dnx_field_context[idx].parent  = 53;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__OFFSET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__offset";
    layout_array_dnx_field_context[idx].type = "int";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__offset__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(int);
    layout_array_dnx_field_context[idx].parent  = 53;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF1_INFO__HASHING_INFO__KEY_INFO__ATTACH_INFO__BASE_QUAL);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__base_qual";
    layout_array_dnx_field_context[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf1_info__hashing_info__key_info__attach_info__base_qual__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_context[idx].parent  = 53;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__CONTEXT_IPMF2_INFO__CASCADED_FROM);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__context_ipmf2_info__cascaded_from";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_t";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__context_ipmf2_info__cascaded_from__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_t);
    layout_array_dnx_field_context[idx].parent  = 4;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__MODE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__state_table_info__mode";
    layout_array_dnx_field_context[idx].type = "dnx_field_context_state_table_mode_e";
    layout_array_dnx_field_context[idx].default_value= &(dnx_field_context_sw__context_info__state_table_info__mode__default_val);
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(dnx_field_context_state_table_mode_e);
    layout_array_dnx_field_context[idx].parent  = 5;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__STATE_TABLE_INFO__IS_SET);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__state_table_info__is_set";
    layout_array_dnx_field_context[idx].type = "uint8";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_context[idx].parent  = 5;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_CONTEXT_SW__CONTEXT_INFO__NAME__VALUE);
    layout_array_dnx_field_context[idx].name = "dnx_field_context_sw__context_info__name__value";
    layout_array_dnx_field_context[idx].type = "sw_state_string_t";
    layout_array_dnx_field_context[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].size_of = sizeof(sw_state_string_t);
    layout_array_dnx_field_context[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dnx_field_context[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STRING;
    layout_array_dnx_field_context[idx].parent  = 7;
    layout_array_dnx_field_context[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_context[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_CONTEXT_SW, layout_array_dnx_field_context, sw_state_layout_array[unit][DNX_FIELD_CONTEXT_MODULE_ID], DNX_SW_STATE_DNX_FIELD_CONTEXT_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
