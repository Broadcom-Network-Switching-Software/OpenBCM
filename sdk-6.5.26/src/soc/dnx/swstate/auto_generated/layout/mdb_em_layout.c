
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/mdb_em_types.h>
#include <soc/dnx/swstate/auto_generated/layout/mdb_em_layout.h>

dnxc_sw_state_layout_t layout_array_mdb_em[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB_NOF_PARAMS)];

shr_error_e
mdb_em_init_layout_structure(int unit)
{

    uint8 mdb_em_db__vmv_info__encoding_map__encoding__default_val = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB);
    layout_array_mdb_em[idx].name = "mdb_em_db";
    layout_array_mdb_em[idx].type = "mdb_em_sw_state_t";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(mdb_em_sw_state_t);
    layout_array_mdb_em[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__FIRST);
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__SHADOW_EM_DB);
    layout_array_mdb_em[idx].name = "mdb_em_db__shadow_em_db";
    layout_array_mdb_em[idx].type = "sw_state_htbl_t";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 0;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info";
    layout_array_mdb_em[idx].type = "vmv_info_t";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(vmv_info_t);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 0;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__FIRST);
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__MACT_STAMP);
    layout_array_mdb_em[idx].name = "mdb_em_db__mact_stamp";
    layout_array_mdb_em[idx].type = "uint32";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(uint32);
    layout_array_mdb_em[idx].parent  = 0;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__VALUE);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__value";
    layout_array_mdb_em[idx].type = "uint8";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(uint8);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 2;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__SIZE);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__size";
    layout_array_mdb_em[idx].type = "uint8";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(uint8);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 2;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__encoding_map";
    layout_array_mdb_em[idx].type = "vmv_value_parameters_t";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(vmv_value_parameters_t);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 2;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP__FIRST);
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__APP_ID_SIZE);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__app_id_size";
    layout_array_mdb_em[idx].type = "uint8";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(uint8);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 2;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__MAX_PAYLOAD_SIZE);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__max_payload_size";
    layout_array_mdb_em[idx].type = "int";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(int);
    layout_array_mdb_em[idx].array_indexes[0].num_elements = DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS;
    layout_array_mdb_em[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_mdb_em[idx].parent  = 2;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP__ENCODING);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__encoding_map__encoding";
    layout_array_mdb_em[idx].type = "uint8";
    layout_array_mdb_em[idx].default_value= &(mdb_em_db__vmv_info__encoding_map__encoding__default_val);
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(uint8);
    layout_array_mdb_em[idx].parent  = 6;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP__SIZE);
    layout_array_mdb_em[idx].name = "mdb_em_db__vmv_info__encoding_map__size";
    layout_array_mdb_em[idx].type = "uint8";
    layout_array_mdb_em[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].size_of = sizeof(uint8);
    layout_array_mdb_em[idx].parent  = 6;
    layout_array_mdb_em[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_mdb_em[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_MDB_EM_DB, layout_array_mdb_em, sw_state_layout_array[unit][MDB_EM_MODULE_ID], DNX_SW_STATE_MDB_EM_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
