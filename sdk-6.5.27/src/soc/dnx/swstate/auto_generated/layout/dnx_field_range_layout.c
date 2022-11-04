
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_range_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_range_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_range[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB_NOF_PARAMS)];

shr_error_e
dnx_field_range_init_layout_structure(int unit)
{
    dnx_field_range_type_e dnx_field_range_sw_db__ext_l4_ops_range_types__default_val = DNX_FIELD_RANGE_TYPE_INVALID;
    dnx_field_qual_t dnx_field_range_sw_db__ext_l4_ops_ffc_quals__dnx_qual__default_val = DNX_FIELD_QUAL_ID_INVALID;
    dnx_field_input_type_e dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_type__default_val = DNX_FIELD_INPUT_TYPE_INVALID;
    int dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_arg__default_val = 0xFFFF;
    int dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__offset__default_val = 0xFFFF;
    dnx_field_qual_t dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__base_qual__default_val = DNX_FIELD_QUAL_TYPE_INVALID;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db";
    layout_array_dnx_field_range[idx].type = "dnx_field_range_sw_db_t";
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_range_sw_db_t);
    layout_array_dnx_field_range[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__FIRST);
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_RANGE_TYPES);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_range_types";
    layout_array_dnx_field_range[idx].type = "dnx_field_range_type_e";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_range_types__default_val);
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_range_type_e);
    layout_array_dnx_field_range[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES;
    layout_array_dnx_field_range[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_range[idx].parent  = 0;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals";
    layout_array_dnx_field_range[idx].type = "dnx_field_range_type_qual_info_t";
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_range_type_qual_info_t);
    layout_array_dnx_field_range[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES;
    layout_array_dnx_field_range[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_range[idx].parent  = 0;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__FIRST);
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__LAST)-1;
    layout_array_dnx_field_range[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__DNX_QUAL);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__dnx_qual";
    layout_array_dnx_field_range[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__dnx_qual__default_val);
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_range[idx].parent  = 2;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info";
    layout_array_dnx_field_range[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_range[idx].parent  = 2;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__FIRST);
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__INPUT_TYPE);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_type";
    layout_array_dnx_field_range[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_type__default_val);
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__INPUT_ARG);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_arg";
    layout_array_dnx_field_range[idx].type = "int";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_arg__default_val);
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(int);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__OFFSET);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__offset";
    layout_array_dnx_field_range[idx].type = "int";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__offset__default_val);
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(int);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__BASE_QUAL);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__base_qual";
    layout_array_dnx_field_range[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__base_qual__default_val);
    layout_array_dnx_field_range[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB, layout_array_dnx_field_range, sw_state_layout_array[unit][DNX_FIELD_RANGE_MODULE_ID], DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
