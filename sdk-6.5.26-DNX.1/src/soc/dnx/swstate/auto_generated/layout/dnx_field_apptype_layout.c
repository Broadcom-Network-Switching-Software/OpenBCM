
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_apptype_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_apptype_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_apptype[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW_NOF_PARAMS)];

shr_error_e
dnx_field_apptype_init_layout_structure(int unit)
{

    uint8 dnx_field_apptype_sw__predef_info__fwd2_context_ids__default_val = -1;
    uint8 dnx_field_apptype_sw__predef_info__valid_for_kbp__default_val = FALSE;
    dnx_field_apptype_flags_e dnx_field_apptype_sw__user_def_info__flags__default_val = DNX_FIELD_APPTYPE_FLAG_NOF;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw";
    layout_array_dnx_field_apptype[idx].type = "dnx_field_apptype_sw_t";
    layout_array_dnx_field_apptype[idx].doc = "Sw State DB for user Aptypes";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(dnx_field_apptype_sw_t);
    layout_array_dnx_field_apptype[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__FIRST);
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__predef_info";
    layout_array_dnx_field_apptype[idx].type = "dnx_field_predef_apptype_info_t";
    layout_array_dnx_field_apptype[idx].doc = "Information about predefined qualifiers.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(dnx_field_predef_apptype_info_t);
    layout_array_dnx_field_apptype[idx].array_indexes[0].num_elements = bcmFieldAppTypeCount;
    layout_array_dnx_field_apptype[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_apptype[idx].parent  = 0;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FIRST);
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__LAST)-1;
    layout_array_dnx_field_apptype[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info";
    layout_array_dnx_field_apptype[idx].type = "dnx_field_user_apptype_info_t";
    layout_array_dnx_field_apptype[idx].doc = "Information about user_defined qualifiers.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(dnx_field_user_apptype_info_t);
    layout_array_dnx_field_apptype[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF;
    layout_array_dnx_field_apptype[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_apptype[idx].parent  = 0;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FIRST);
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__LAST)-1;
    layout_array_dnx_field_apptype[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__NOF_FWD2_CONTEXT_IDS);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__predef_info__nof_fwd2_context_ids";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "The number of FWD2 contexts in context_ids.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].parent  = 1;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__FWD2_CONTEXT_IDS);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__predef_info__fwd2_context_ids";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "The FWD2 context IDs used by the Apptype.";
    layout_array_dnx_field_apptype[idx].default_value= &(dnx_field_apptype_sw__predef_info__fwd2_context_ids__default_val);
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE;
    layout_array_dnx_field_apptype[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_apptype[idx].parent  = 1;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__PREDEF_INFO__VALID_FOR_KBP);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__predef_info__valid_for_kbp";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "If the Apptype is used as a valid opcode for KBP.";
    layout_array_dnx_field_apptype[idx].default_value= &(dnx_field_apptype_sw__predef_info__valid_for_kbp__default_val);
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].parent  = 1;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__OPCODE_ID);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info__opcode_id";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "The opcode ID of the Apptype.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].parent  = 2;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__ACL_CONTEXT);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info__acl_context";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "The ACL context ID (in iFWD2) used by the apptype";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].parent  = 2;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__BASE_APPTYPE);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info__base_apptype";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "The static apptype based on which we create the dynamic apptype.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].parent  = 2;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__PROFILE_ID);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info__profile_id";
    layout_array_dnx_field_apptype[idx].type = "uint8";
    layout_array_dnx_field_apptype[idx].doc = "Apptype CS profile.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_apptype[idx].parent  = 2;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__FLAGS);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info__flags";
    layout_array_dnx_field_apptype[idx].type = "dnx_field_apptype_flags_e";
    layout_array_dnx_field_apptype[idx].doc = "DNX level flags. Saved for Get function to return all configured flags";
    layout_array_dnx_field_apptype[idx].default_value= &(dnx_field_apptype_sw__user_def_info__flags__default_val);
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(dnx_field_apptype_flags_e);
    layout_array_dnx_field_apptype[idx].parent  = 2;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_APPTYPE_SW__USER_DEF_INFO__NAME);
    layout_array_dnx_field_apptype[idx].name = "dnx_field_apptype_sw__user_def_info__name";
    layout_array_dnx_field_apptype[idx].type = "sw_state_string_t";
    layout_array_dnx_field_apptype[idx].doc = "Apptype name.";
    layout_array_dnx_field_apptype[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].size_of = sizeof(sw_state_string_t);
    layout_array_dnx_field_apptype[idx].parent  = 2;
    layout_array_dnx_field_apptype[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_apptype[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_APPTYPE_SW, layout_array_dnx_field_apptype, sw_state_layout_array[unit][DNX_FIELD_APPTYPE_MODULE_ID], DNX_SW_STATE_DNX_FIELD_APPTYPE_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
