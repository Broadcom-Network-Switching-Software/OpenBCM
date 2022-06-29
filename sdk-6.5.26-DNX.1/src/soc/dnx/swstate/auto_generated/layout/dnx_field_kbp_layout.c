
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef INCLUDE_KBP
#include <soc/dnx/swstate/auto_generated/types/dnx_field_kbp_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_kbp_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_kbp[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW_NOF_PARAMS)];

shr_error_e
dnx_field_kbp_init_layout_structure(int unit)
{

    uint8 dnx_field_kbp_sw__opcode_info__master_key_info__segment_info__qual_idx__default_val = DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID;
    dnx_field_group_t dnx_field_kbp_sw__opcode_info__master_key_info__segment_info__fg_id__default_val = DNX_FIELD_GROUP_INVALID;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw";
    layout_array_dnx_field_kbp[idx].type = "dnx_field_kbp_sw_t";
    layout_array_dnx_field_kbp[idx].doc = "Hold the needed info to manage KBP";
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(dnx_field_kbp_sw_t);
    layout_array_dnx_field_kbp[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__FIRST);
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info";
    layout_array_dnx_field_kbp[idx].type = "dnx_field_kbp_opcode_info_t*";
    layout_array_dnx_field_kbp[idx].doc = "Hold the information about the external TCAM per apptype.";
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(dnx_field_kbp_opcode_info_t*);
    layout_array_dnx_field_kbp[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_kbp[idx].parent  = 0;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__FIRST);
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__LAST)-1;
    layout_array_dnx_field_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info__master_key_info";
    layout_array_dnx_field_kbp[idx].type = "dnx_field_kbp_master_key_info_t";
    layout_array_dnx_field_kbp[idx].doc = "This is the info that is used to receive the information for\n                             the Master Key. The information is per Opcode.";
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(dnx_field_kbp_master_key_info_t);
    layout_array_dnx_field_kbp[idx].parent  = 1;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__FIRST);
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__LAST)-1;
    layout_array_dnx_field_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__IS_VALID);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info__is_valid";
    layout_array_dnx_field_kbp[idx].type = "uint8";
    layout_array_dnx_field_kbp[idx].doc = "Holds the information about the validity of the opcode. The information is per Opcode.";
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_kbp[idx].parent  = 1;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info__master_key_info__segment_info";
    layout_array_dnx_field_kbp[idx].type = "dnx_field_kbp_segment_info_t";
    layout_array_dnx_field_kbp[idx].doc = "This is the info that is used to receive the information for a given segment\n                             in the Master Key. The segment information is per Opcode.";
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(dnx_field_kbp_segment_info_t);
    layout_array_dnx_field_kbp[idx].array_indexes[0].num_elements = DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY;
    layout_array_dnx_field_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_kbp[idx].parent  = 2;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__FIRST);
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__LAST)-1;
    layout_array_dnx_field_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__NOF_FWD_SEGMENTS);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info__master_key_info__nof_fwd_segments";
    layout_array_dnx_field_kbp[idx].type = "uint8";
    layout_array_dnx_field_kbp[idx].doc = "Indicates the number of segments (including zero padding) used by FWD, before new segments are added by ACL.";
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_kbp[idx].parent  = 2;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__QUAL_IDX);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info__master_key_info__segment_info__qual_idx";
    layout_array_dnx_field_kbp[idx].type = "uint8";
    layout_array_dnx_field_kbp[idx].doc = "This is the index of the qualifier attach_info that is stored in qual_attach_info in dnx_field_attach_context_info_t\n                             The index is stored for a postponed hw configuration of the ffc's for the external TCAM group. \n                             The index represents the information required for the segment configuration (in the Master Key) \n                             Note: this qual_idx is also valid for receiving the qual_type from the key_template. The other \n                             information in the key_template is irrelevant. \n                             An array for all packed qualifiers packed into the segment if we enable packing of qualifiers in the field group.";
    layout_array_dnx_field_kbp[idx].default_value= &(dnx_field_kbp_sw__opcode_info__master_key_info__segment_info__qual_idx__default_val);
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_kbp[idx].array_indexes[0].num_elements = DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT;
    layout_array_dnx_field_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_kbp[idx].parent  = 4;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__FG_ID);
    layout_array_dnx_field_kbp[idx].name = "dnx_field_kbp_sw__opcode_info__master_key_info__segment_info__fg_id";
    layout_array_dnx_field_kbp[idx].type = "dnx_field_group_t";
    layout_array_dnx_field_kbp[idx].doc = "This is the id of the field_group for a given qualifier.\n                             Used for retrieving the qual_attach_info.";
    layout_array_dnx_field_kbp[idx].default_value= &(dnx_field_kbp_sw__opcode_info__master_key_info__segment_info__fg_id__default_val);
    layout_array_dnx_field_kbp[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_field_kbp[idx].parent  = 4;
    layout_array_dnx_field_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW, layout_array_dnx_field_kbp, sw_state_layout_array[unit][DNX_FIELD_KBP_MODULE_ID], DNX_SW_STATE_DNX_FIELD_KBP_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#endif  
#undef BSL_LOG_MODULE
