
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#include <soc/dnx/swstate/auto_generated/layout/kbp_layout.h>

dnxc_sw_state_layout_t layout_array_kbp[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE_NOF_PARAMS)];

shr_error_e
kbp_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE);
    layout_array_kbp[idx].name = "kbp_sw_state";
    layout_array_kbp[idx].type = "kbp_sw_state_t";
    layout_array_kbp[idx].doc = "KBP related sw state";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_sw_state_t);
    layout_array_kbp[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].first_child_index = 1;
    layout_array_kbp[idx].last_child_index = 4;
    layout_array_kbp[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__FWD_CACHING_ENABLED);
    layout_array_kbp[idx].name = "kbp_sw_state__fwd_caching_enabled";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].doc = "Global indication showing that caching is enabled or disabled for all forwarding tables";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 0;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__IS_DEVICE_LOCKED);
    layout_array_kbp[idx].name = "kbp_sw_state__is_device_locked";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 0;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info";
    layout_array_kbp[idx].type = "kbp_db_handles_t";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_db_handles_t);
    layout_array_kbp[idx].array_indexes[0].size = DNX_KBP_NOF_DBS;
    layout_array_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_kbp[idx].parent  = 0;
    layout_array_kbp[idx].first_child_index = 5;
    layout_array_kbp[idx].last_child_index = 15;
    layout_array_kbp[idx].next_node_index = 4;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO);
    layout_array_kbp[idx].name = "kbp_sw_state__instruction_info";
    layout_array_kbp[idx].type = "kbp_instruction_handles_t";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_instruction_handles_t);
    layout_array_kbp[idx].array_indexes[0].size = DNX_KBP_NOF_INSTRUCTIONS;
    layout_array_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_kbp[idx].parent  = 0;
    layout_array_kbp[idx].first_child_index = 16;
    layout_array_kbp[idx].last_child_index = 16;
    layout_array_kbp[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_BMP);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__caching_bmp";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CORE_ID);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__core_id";
    layout_array_kbp[idx].type = "uint8";
    layout_array_kbp[idx].doc = "Indication for DPC core, used to set the entry handles to DBAL tables. For SBC it is always 0.";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(uint8);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 7;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CACHING_ENABLED);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__caching_enabled";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].doc = "Indication showing that caching is currently enabled or disabled for the DB.";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 8;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__DB_P);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__db_p";
    layout_array_kbp[idx].type = "kbp_db_t_p";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_db_t_p);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 9;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_ZERO_SIZE_P);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__ad_db_zero_size_p";
    layout_array_kbp[idx].type = "kbp_ad_db_t_p";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_ad_db_t_p);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 10;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_ENTRY_ZERO_SIZE_P);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__ad_entry_zero_size_p";
    layout_array_kbp[idx].type = "kbp_ad_entry_t_p";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_ad_entry_t_p);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 11;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__AD_DB_P);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__ad_db_p";
    layout_array_kbp[idx].type = "kbp_ad_db_t_p";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_ad_db_t_p);
    layout_array_kbp[idx].array_indexes[0].size = DNX_KBP_NOF_AD_DB_INDEX;
    layout_array_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 12;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__OPT_RESULT_SIZE);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__opt_result_size";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 13;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__LARGE_OPT_RESULT_SIZE);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__large_opt_result_size";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 14;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__CLONED_DB_ID);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__cloned_db_id";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = 15;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__DB_HANDLES_INFO__ASSOCIATED_DBAL_TABLE_ID);
    layout_array_kbp[idx].name = "kbp_sw_state__db_handles_info__associated_dbal_table_id";
    layout_array_kbp[idx].type = "int";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(int);
    layout_array_kbp[idx].parent  = 3;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_SW_STATE__INSTRUCTION_INFO__INST_P);
    layout_array_kbp[idx].name = "kbp_sw_state__instruction_info__inst_p";
    layout_array_kbp[idx].type = "kbp_instruction_t_p";
    layout_array_kbp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].size_of = sizeof(kbp_instruction_t_p);
    layout_array_kbp[idx].array_indexes[0].size = DNX_KBP_NOF_SMTS;
    layout_array_kbp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_kbp[idx].parent  = 4;
    layout_array_kbp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_KBP_SW_STATE, layout_array_kbp, sw_state_layout_array[unit][KBP_MODULE_ID], DNX_SW_STATE_KBP_SW_STATE_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#endif  
#endif  
#undef BSL_LOG_MODULE
