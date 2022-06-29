
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <soc/dnx/swstate/auto_generated/layout/mdb_kaps_layout.h>

dnxc_sw_state_layout_t layout_array_mdb_kaps[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB_NOF_PARAMS)];

shr_error_e
mdb_kaps_init_layout_structure(int unit)
{

    uint8 mdb_kaps_db__ad_size_supported__default_val = FALSE;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_sw_state_t";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_sw_state_t);
    layout_array_mdb_kaps[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__FIRST);
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__db_info";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_db_handles_t*";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_db_handles_t*);
    layout_array_mdb_kaps[idx].array_indexes[0].num_elements = dnx_data_mdb.kaps.nof_dbs_get(unit);
    layout_array_mdb_kaps[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb_kaps[idx].parent  = 0;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__FIRST);
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__LAST)-1;
    layout_array_mdb_kaps[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__search_instruction_info";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_instruction_handles_t*";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_instruction_handles_t*);
    layout_array_mdb_kaps[idx].array_indexes[0].num_elements = dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR;
    layout_array_mdb_kaps[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb_kaps[idx].parent  = 0;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO__FIRST);
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO__LAST)-1;
    layout_array_mdb_kaps[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__BIG_KAPS_LARGE_BB_MODE);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__big_kaps_large_bb_mode";
    layout_array_mdb_kaps[idx].type = "uint32";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(uint32);
    layout_array_mdb_kaps[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS;
    layout_array_mdb_kaps[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_kaps[idx].parent  = 0;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__ad_size_supported";
    layout_array_mdb_kaps[idx].type = "uint8*";
    layout_array_mdb_kaps[idx].default_value= &(mdb_kaps_db__ad_size_supported__default_val);
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(uint8*);
    layout_array_mdb_kaps[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb_kaps[idx].array_indexes[1].num_elements = MDB_NOF_LPM_AD_SIZES;
    layout_array_mdb_kaps[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_kaps[idx].parent  = 0;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__DB_P);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__db_info__db_p";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_db_t_p";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_db_t_p);
    layout_array_mdb_kaps[idx].parent  = 1;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__HB_DB_P);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__db_info__hb_db_p";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_hb_db_t_p";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_hb_db_t_p);
    layout_array_mdb_kaps[idx].parent  = 1;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__AD_DB_P_ARR);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__db_info__ad_db_p_arr";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_ad_db_t_p";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_ad_db_t_p);
    layout_array_mdb_kaps[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES;
    layout_array_mdb_kaps[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_kaps[idx].parent  = 1;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__AD_DB_SIZE_ARR);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__db_info__ad_db_size_arr";
    layout_array_mdb_kaps[idx].type = "mdb_lpm_ad_size_e";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_lpm_ad_size_e);
    layout_array_mdb_kaps[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES;
    layout_array_mdb_kaps[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_kaps[idx].parent  = 1;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__IS_VALID);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__db_info__is_valid";
    layout_array_mdb_kaps[idx].type = "uint8";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(uint8);
    layout_array_mdb_kaps[idx].parent  = 1;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_KAPS_DB__SEARCH_INSTRUCTION_INFO__INST_P);
    layout_array_mdb_kaps[idx].name = "mdb_kaps_db__search_instruction_info__inst_p";
    layout_array_mdb_kaps[idx].type = "mdb_kaps_instruction_t_p";
    layout_array_mdb_kaps[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].size_of = sizeof(mdb_kaps_instruction_t_p);
    layout_array_mdb_kaps[idx].parent  = 2;
    layout_array_mdb_kaps[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_kaps[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_MDB_KAPS_DB, layout_array_mdb_kaps, sw_state_layout_array[unit][MDB_KAPS_MODULE_ID], DNX_SW_STATE_MDB_KAPS_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
