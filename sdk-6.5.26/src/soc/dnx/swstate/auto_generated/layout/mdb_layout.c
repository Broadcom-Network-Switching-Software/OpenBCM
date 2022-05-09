
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/swstate/auto_generated/layout/mdb_layout.h>

dnxc_sw_state_layout_t layout_array_mdb[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS_NOF_PARAMS)];

shr_error_e
mdb_init_layout_structure(int unit)
{

    uint32 mdb_db_infos__eedb_payload_type__default_val = DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES;
    uint32 mdb_db_infos__em_key_format__key_to_format_map__default_val = MDB_INVALID_EM_FORMAT_VALUE;
    mdb_macro_types_e mdb_db_infos__em_key_format__em_bank_info__macro_type__default_val = MDB_NOF_MACRO_TYPES;
    mdb_macro_types_e mdb_db_infos__db__clusters_info__macro_type__default_val = MDB_NOF_MACRO_TYPES;
    uint8 mdb_db_infos__mdb_cluster_infos__cache_enabled__default_val = TRUE;
    uint32 mdb_db_infos__mdb_cluster_infos__direct_address_mapping__default_val = SAL_UINT32_MAX;
    uint32 mdb_db_infos__mdb_cluster_infos__eedb_address_mapping__default_val = SAL_UINT32_MAX;
    mdb_physical_table_e mdb_db_infos__mdb_cluster_infos__macro_cluster_assoc__pdb__default_val = MDB_NOF_PHYSICAL_TABLES;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS);
    layout_array_mdb[idx].name = "mdb_db_infos";
    layout_array_mdb[idx].type = "mdb_tables_info";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_tables_info);
    layout_array_mdb[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format";
    layout_array_mdb[idx].type = "em_key_format_info";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(em_key_format_info);
    layout_array_mdb[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__LAST);
    layout_array_mdb[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB);
    layout_array_mdb[idx].name = "mdb_db_infos__db";
    layout_array_mdb[idx].type = "mdb_db_info_t";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_db_info_t);
    layout_array_mdb[idx].array_indexes[0].num_elements = MDB_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__CAPACITY);
    layout_array_mdb[idx].name = "mdb_db_infos__capacity";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_INTERFACE_BLK);
    layout_array_mdb[idx].name = "mdb_db_infos__em_interface_blk";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE);
    layout_array_mdb[idx].name = "mdb_db_infos__em_entry_capacity_estimate";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE);
    layout_array_mdb[idx].name = "mdb_db_infos__em_entry_capacity_estimate_average";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE);
    layout_array_mdb[idx].name = "mdb_db_infos__eedb_payload_type";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__eedb_payload_type__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].array_indexes[0].num_elements = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos";
    layout_array_mdb[idx].type = "mdb_cluster_info";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_cluster_info);
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE);
    layout_array_mdb[idx].name = "mdb_db_infos__max_fec_id_value";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK);
    layout_array_mdb[idx].name = "mdb_db_infos__eedb_bank_traffic_lock";
    layout_array_mdb[idx].type = "eedb_bank_traffic_lock_t";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(eedb_bank_traffic_lock_t);
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO);
    layout_array_mdb[idx].name = "mdb_db_infos__fec_db_info";
    layout_array_mdb[idx].type = "l3_fec_db_info_t";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(l3_fec_db_info_t);
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_profile";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_em_init_done";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON);
    layout_array_mdb[idx].name = "mdb_db_infos__kaps_scan_cfg_on";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].array_indexes[0].num_elements = 2;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED);
    layout_array_mdb[idx].name = "mdb_db_infos__kaps_scan_cfg_paused";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].array_indexes[0].num_elements = 2;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 0;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__key_size";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__FORMAT_COUNT);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__format_count";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_TO_FORMAT_MAP);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__key_to_format_map";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__em_key_format__key_to_format_map__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].array_indexes[0].num_elements = MDB_EM_MAX_KEYS_PLUS_TID_VALS_SIZE;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__em_bank_info";
    layout_array_mdb[idx].type = "dh_em_bank_info";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(dh_em_bank_info);
    layout_array_mdb[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__nof_address_bits";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__nof_ways";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__nof_flex_ways";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].parent  = 1;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__em_bank_info__is_flex";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 19;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__em_bank_info__macro_index";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 19;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__em_bank_info__bank_size_used_nof_rows";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].parent  = 19;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__em_bank_info__macro_type";
    layout_array_mdb[idx].type = "mdb_macro_types_e";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__em_key_format__em_bank_info__macro_type__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_macro_types_e);
    layout_array_mdb[idx].parent  = 19;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING);
    layout_array_mdb[idx].name = "mdb_db_infos__em_key_format__em_bank_info__cluster_mapping";
    layout_array_mdb[idx].type = "uint32";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32);
    layout_array_mdb[idx].parent  = 19;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS);
    layout_array_mdb[idx].name = "mdb_db_infos__db__number_of_alloc_clusters";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].parent  = 2;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS);
    layout_array_mdb[idx].name = "mdb_db_infos__db__number_of_clusters";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].parent  = 2;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY);
    layout_array_mdb[idx].name = "mdb_db_infos__db__nof_cluster_per_entry";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].parent  = 2;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info";
    layout_array_mdb[idx].type = "mdb_cluster_info_t*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_cluster_info_t*);
    layout_array_mdb[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].parent  = 2;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__LAST);
    layout_array_mdb[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS);
    layout_array_mdb[idx].name = "mdb_db_infos__db__address_space_bits";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].parent  = 2;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info__macro_type";
    layout_array_mdb[idx].type = "mdb_macro_types_e";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__db__clusters_info__macro_type__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_macro_types_e);
    layout_array_mdb[idx].parent  = 31;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info__macro_index";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 31;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info__cluster_index";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 31;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info__cluster_position_in_entry";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 31;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info__start_address";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].parent  = 31;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS);
    layout_array_mdb[idx].name = "mdb_db_infos__db__clusters_info__end_address";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].parent  = 31;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__cache_enabled";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__mdb_cluster_infos__cache_enabled__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__macro_cluster_assoc";
    layout_array_mdb[idx].type = "cluster_assoc_info*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(cluster_assoc_info*);
    layout_array_mdb[idx].array_indexes[0].num_elements = MDB_NOF_MACRO_TYPES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__macro_cluster_cache";
    layout_array_mdb[idx].type = "cluster_data*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(cluster_data*);
    layout_array_mdb[idx].array_indexes[0].num_elements = MDB_NOF_MACRO_TYPES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__FIRST);
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__LAST);
    layout_array_mdb[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__eedb_banks_hitbit";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows/dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get(unit);
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__small_kaps_bb_hitbit";
    layout_array_mdb[idx].type = "uint32**";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32**);
    layout_array_mdb[idx].array_indexes[0].num_elements = dnx_data_mdb.kaps.nof_small_bbs_get(unit)*dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].array_indexes[1].num_elements = dnx_data_mdb.kaps.nof_rows_in_small_bb_max_get(unit);
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__small_kaps_rpb_hitbit";
    layout_array_mdb[idx].type = "uint32**";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32**);
    layout_array_mdb[idx].array_indexes[0].num_elements = dnx_data_mdb.kaps.nof_dbs_get(unit);
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].array_indexes[1].num_elements = dnx_data_mdb.kaps.nof_rows_in_small_rpb_hitbits_max_get(unit);
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__direct_address_mapping";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__mdb_cluster_infos__direct_address_mapping__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].num_elements = MDB_NOF_PHYSICAL_TABLES;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__eedb_address_mapping";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__mdb_cluster_infos__eedb_address_mapping__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].parent  = 8;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__macro_cluster_assoc__pdb";
    layout_array_mdb[idx].type = "mdb_physical_table_e";
    layout_array_mdb[idx].default_value= &(mdb_db_infos__mdb_cluster_infos__macro_cluster_assoc__pdb__default_val);
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(mdb_physical_table_e);
    layout_array_mdb[idx].parent  = 40;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__macro_cluster_assoc__flags";
    layout_array_mdb[idx].type = "uint8";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint8);
    layout_array_mdb[idx].parent  = 40;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA);
    layout_array_mdb[idx].name = "mdb_db_infos__mdb_cluster_infos__macro_cluster_cache__data";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 41;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS | DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS);
    layout_array_mdb[idx].name = "mdb_db_infos__eedb_bank_traffic_lock__local_lifs";
    layout_array_mdb[idx].type = "int*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int*);
    layout_array_mdb[idx].array_indexes[0].num_elements = 2;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_mdb[idx].parent  = 10;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION);
    layout_array_mdb[idx].name = "mdb_db_infos__eedb_bank_traffic_lock__global_lif_destination";
    layout_array_mdb[idx].type = "int";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(int);
    layout_array_mdb[idx].array_indexes[0].num_elements = 2;
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb[idx].parent  = 10;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP);
    layout_array_mdb[idx].name = "mdb_db_infos__fec_db_info__fec_db_map";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].name = "bank_id";
    layout_array_mdb[idx].array_indexes[0].num_elements = dnx_data_l3.fec.max_nof_banks_get(unit);
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].parent  = 11;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START);
    layout_array_mdb[idx].name = "mdb_db_infos__fec_db_info__fec_id_range_start";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].name = "index";
    layout_array_mdb[idx].array_indexes[0].num_elements = dnx_data_l3.fec.nof_fec_dbs_all_cores_get(unit);
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].parent  = 11;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE);
    layout_array_mdb[idx].name = "mdb_db_infos__fec_db_info__fec_id_range_size";
    layout_array_mdb[idx].type = "uint32*";
    layout_array_mdb[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].size_of = sizeof(uint32*);
    layout_array_mdb[idx].array_indexes[0].name = "index";
    layout_array_mdb[idx].array_indexes[0].num_elements = dnx_data_l3.fec.nof_fec_dbs_all_cores_get(unit);
    layout_array_mdb[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_mdb[idx].parent  = 11;
    layout_array_mdb[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_MDB_DB_INFOS, layout_array_mdb, sw_state_layout_array[unit][MDB_MODULE_ID], DNX_SW_STATE_MDB_DB_INFOS_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
