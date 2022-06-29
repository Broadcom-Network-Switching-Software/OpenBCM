
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_qualifier_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_qualifier_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_qualifier[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB_NOF_PARAMS)];

shr_error_e
dnx_field_qualifier_init_layout_structure(int unit)
{

    dbal_fields_e dnx_field_qual_sw_db__info__field_id__default_val = DBAL_FIELD_EMPTY;
    int dnx_field_qual_sw_db__info__bcm_id__default_val = bcmFieldQualifyCount;
    dbal_fields_e dnx_field_qual_sw_db__predefined__field_id__default_val = DBAL_FIELD_EMPTY;
    dbal_fields_e dnx_field_qual_sw_db__layer_record__field_id__default_val = DBAL_FIELD_EMPTY;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db";
    layout_array_dnx_field_qualifier[idx].type = "dnx_field_qual_sw_db_t";
    layout_array_dnx_field_qualifier[idx].doc = "Sw State DB for User Qualifiers";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dnx_field_qual_sw_db_t);
    layout_array_dnx_field_qualifier[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__FIRST);
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info";
    layout_array_dnx_field_qualifier[idx].type = "dnx_field_user_qual_info_t";
    layout_array_dnx_field_qualifier[idx].doc = "Hold the Qualifier params";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dnx_field_user_qual_info_t);
    layout_array_dnx_field_qualifier[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
    layout_array_dnx_field_qualifier[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_qualifier[idx].parent  = 0;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIRST);
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__LAST)-1;
    layout_array_dnx_field_qualifier[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__predefined";
    layout_array_dnx_field_qualifier[idx].type = "dnx_field_predefined_qual_info_t";
    layout_array_dnx_field_qualifier[idx].doc = "Hold the predefined Qualifier info";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dnx_field_predefined_qual_info_t);
    layout_array_dnx_field_qualifier[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF;
    layout_array_dnx_field_qualifier[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_qualifier[idx].parent  = 0;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIRST);
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__LAST)-1;
    layout_array_dnx_field_qualifier[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__layer_record";
    layout_array_dnx_field_qualifier[idx].type = "dnx_field_predefined_qual_info_t";
    layout_array_dnx_field_qualifier[idx].doc = "Hold the predefined Qualifier info";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dnx_field_predefined_qual_info_t);
    layout_array_dnx_field_qualifier[idx].array_indexes[0].num_elements = DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF;
    layout_array_dnx_field_qualifier[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_qualifier[idx].parent  = 0;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIRST);
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__LAST)-1;
    layout_array_dnx_field_qualifier[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info__valid";
    layout_array_dnx_field_qualifier[idx].type = "uint32";
    layout_array_dnx_field_qualifier[idx].doc = "TRUE if the entry is valid, false otherwise";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_qualifier[idx].parent  = 1;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info__field_id";
    layout_array_dnx_field_qualifier[idx].type = "dbal_fields_e";
    layout_array_dnx_field_qualifier[idx].doc = "DBAL field id";
    layout_array_dnx_field_qualifier[idx].default_value= &(dnx_field_qual_sw_db__info__field_id__default_val);
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_qualifier[idx].parent  = 1;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info__size";
    layout_array_dnx_field_qualifier[idx].type = "uint32";
    layout_array_dnx_field_qualifier[idx].doc = "Size of the created qualifer";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_qualifier[idx].parent  = 1;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info__bcm_id";
    layout_array_dnx_field_qualifier[idx].type = "int";
    layout_array_dnx_field_qualifier[idx].doc = "user bcm qualifier";
    layout_array_dnx_field_qualifier[idx].default_value= &(dnx_field_qual_sw_db__info__bcm_id__default_val);
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(int);
    layout_array_dnx_field_qualifier[idx].parent  = 1;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info__ref_count";
    layout_array_dnx_field_qualifier[idx].type = "uint32";
    layout_array_dnx_field_qualifier[idx].doc = "Number of users(Field Groups) refering to this qualifier";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_qualifier[idx].parent  = 1;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__info__flags";
    layout_array_dnx_field_qualifier[idx].type = "dnx_field_qual_flags_e";
    layout_array_dnx_field_qualifier[idx].doc = "DNX level flags.\n                                                                Saved for Get function to return all configured flags";
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dnx_field_qual_flags_e);
    layout_array_dnx_field_qualifier[idx].parent  = 1;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__predefined__field_id";
    layout_array_dnx_field_qualifier[idx].type = "dbal_fields_e";
    layout_array_dnx_field_qualifier[idx].doc = "DBAL field id";
    layout_array_dnx_field_qualifier[idx].default_value= &(dnx_field_qual_sw_db__predefined__field_id__default_val);
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_qualifier[idx].parent  = 2;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID);
    layout_array_dnx_field_qualifier[idx].name = "dnx_field_qual_sw_db__layer_record__field_id";
    layout_array_dnx_field_qualifier[idx].type = "dbal_fields_e";
    layout_array_dnx_field_qualifier[idx].doc = "DBAL field id";
    layout_array_dnx_field_qualifier[idx].default_value= &(dnx_field_qual_sw_db__layer_record__field_id__default_val);
    layout_array_dnx_field_qualifier[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].size_of = sizeof(dbal_fields_e);
    layout_array_dnx_field_qualifier[idx].parent  = 3;
    layout_array_dnx_field_qualifier[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_qualifier[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB, layout_array_dnx_field_qualifier, sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID], DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
