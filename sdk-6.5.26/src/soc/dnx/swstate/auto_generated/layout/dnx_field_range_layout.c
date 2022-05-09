
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
    layout_array_dnx_field_range[idx].doc = "Sw State DB for Field Ranges";
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_range_sw_db_t);
    layout_array_dnx_field_range[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__FIRST);
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_RANGE_TYPES);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_range_types";
    layout_array_dnx_field_range[idx].type = "dnx_field_range_type_e";
    layout_array_dnx_field_range[idx].doc = "Holds the range type of each extended L4 OPs range index";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_range_types__default_val);
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_range_type_e);
    layout_array_dnx_field_range[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES;
    layout_array_dnx_field_range[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_range[idx].parent  = 0;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals";
    layout_array_dnx_field_range[idx].type = "dnx_field_range_type_qual_info_t";
    layout_array_dnx_field_range[idx].doc = "Holds DNX qualifier information (type, attach_info) for each L4 OPS FFC";
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_range_type_qual_info_t);
    layout_array_dnx_field_range[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES;
    layout_array_dnx_field_range[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_range[idx].parent  = 0;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__FIRST);
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__LAST);
    layout_array_dnx_field_range[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__DNX_QUAL);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__dnx_qual";
    layout_array_dnx_field_range[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_range[idx].doc = "The DNX qualifier";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__dnx_qual__default_val);
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_range[idx].parent  = 2;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info";
    layout_array_dnx_field_range[idx].type = "dnx_field_qual_attach_info_t";
    layout_array_dnx_field_range[idx].doc = "Indicate how to read the relevant qualifier from PBUS";
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_qual_attach_info_t);
    layout_array_dnx_field_range[idx].parent  = 2;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__FIRST);
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__INPUT_TYPE);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_type";
    layout_array_dnx_field_range[idx].type = "dnx_field_input_type_e";
    layout_array_dnx_field_range[idx].doc = "The base of qualifier indicates from which part of the PBUS the info is taken                                                                          \n relevant for all type of qualifiers                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_FWD  - Takes data from a layer in the header, using the forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE - Takes data from a layer in the header, ignoring forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA - Takes data from the Metadata.                                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA2 - Takes data from the native Metadata of iPMF2, relevant only for iPMF2.                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD  -  Takes data about a layer in the header (layer record), using the forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE - Takes data about a layer in the header (layer record), ignoring the forwarding layer information.                                                                          \n DNX_FIELD_INPUT_TYPE_EXTERNAL - Takes data from the results of an external stage field group.                                                                          \n DNX_FIELD_INPUT_TYPE_CASCADED - Takes data from the result of a field group from an earlier PMF stage, using cascading.                                                                          \n DNX_FIELD_INPUT_TYPE_CONST - Writes a constant value.";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_type__default_val);
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_input_type_e);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__INPUT_ARG);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_arg";
    layout_array_dnx_field_range[idx].type = "int";
    layout_array_dnx_field_range[idx].doc = " Input arg has different meaning per input type as follows:                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_FWD  - how many layer to increment from the forwarding layer (i.e. FWD+input_arg). Can take negative or positive values.                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE - how many layer to increment from the outermost layer (i.e. 0+input_arg). Can only take non-negative values.                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA - Has no meaning                                                           \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD  -  how many layer to increment from the forwarding layer (i.e. FWD+input_arg). Can take negative or positive values.                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE - how many layer to increment from the absolute layer (i.e. 0+input_arg). Can only take non-negative values.                                                          \n DNX_FIELD_INPUT_TYPE_EXTERNAL - Has no meaning                                                           \n DNX_FIELD_INPUT_TYPE_CASCADED - Field group Id of the 'Cascaded from' result                                                          \n DNX_FIELD_INPUT_TYPE_CONST - A constant, up to 32 bit.";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__input_arg__default_val);
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(int);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__OFFSET);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__offset";
    layout_array_dnx_field_range[idx].type = "int";
    layout_array_dnx_field_range[idx].doc = "1) This filed is bit resolution                                                          \n 2) In case the qualifier was created by dnx_field_qual_create() this field is always relevant                                                          \n    it is the offset of the field inside the PBUS (MD/LR/Layer)                                                          \n 3) For predefined qualifiers based on input_type:                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_FWD  - offset inside the layer  (chosen by input_type+input_arg)                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE - offset inside the layer  (chosen by input_type+input_arg)                                                          \n DNX_FIELD_INPUT_TYPE_META_DATA - NA                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD  -  NA                                                          \n DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE - NA                                                          \n DNX_FIELD_INPUT_TYPE_EXTERNAL - offset of the field group in the external stage result buffer, plus the offset of the action withing the field group.                                                          \n DNX_FIELD_INPUT_TYPE_CASCADED - offset in the field group of 'Cascaded from' result buffer                                                          \n DNX_FIELD_INPUT_TYPE_CONST - NA";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__offset__default_val);
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(int);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB__EXT_L4_OPS_FFC_QUALS__QUAL_INFO__BASE_QUAL);
    layout_array_dnx_field_range[idx].name = "dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__base_qual";
    layout_array_dnx_field_range[idx].type = "dnx_field_qual_t";
    layout_array_dnx_field_range[idx].doc = "The qualifier to be based upon. used for user defined qualifers that relate to split predefined qualifers.";
    layout_array_dnx_field_range[idx].default_value= &(dnx_field_range_sw_db__ext_l4_ops_ffc_quals__qual_info__base_qual__default_val);
    layout_array_dnx_field_range[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].size_of = sizeof(dnx_field_qual_t);
    layout_array_dnx_field_range[idx].parent  = 4;
    layout_array_dnx_field_range[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_range[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB, layout_array_dnx_field_range, sw_state_layout_array[unit][DNX_FIELD_RANGE_MODULE_ID], DNX_SW_STATE_DNX_FIELD_RANGE_SW_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
