
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dbal_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dbal_layout.h>

dnxc_sw_state_layout_t layout_array_dbal[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB_NOF_PARAMS)];

shr_error_e
dbal_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB);
    layout_array_dbal[idx].name = "dbal_db";
    layout_array_dbal[idx].type = "dnx_dbal_sw_state_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dnx_dbal_sw_state_t);
    layout_array_dbal[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__LAST)-1;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__SW_TABLES);
    layout_array_dbal[idx].name = "dbal_db__sw_tables";
    layout_array_dbal[idx].type = "dbal_sw_state_table_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_sw_state_table_t);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_NOF_TABLES;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__SW_TABLES__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__SW_TABLES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_ACCESS);
    layout_array_dbal[idx].name = "dbal_db__mdb_access";
    layout_array_dbal[idx].type = "mdb_access_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(mdb_access_t);
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_ACCESS__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_ACCESS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES);
    layout_array_dbal[idx].name = "dbal_db__mdb_phy_tables";
    layout_array_dbal[idx].type = "mdb_phy_table_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(mdb_phy_table_t);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_NOF_PHYSICAL_TABLES;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES);
    layout_array_dbal[idx].name = "dbal_db__tables_properties";
    layout_array_dbal[idx].type = "dbal_sw_state_table_properties_t*";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_sw_state_table_properties_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables";
    layout_array_dbal[idx].type = "dbal_dynamic_table_t*";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_dynamic_table_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables";
    layout_array_dbal[idx].type = "dbal_dynamic_table_t*";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_dynamic_table_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_xml_tables_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_FIELDS);
    layout_array_dbal[idx].name = "dbal_db__dbal_fields";
    layout_array_dbal[idx].type = "dbal_dynamic_fields_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_dynamic_fields_t);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_NOF_DYNAMIC_FIELDS;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES);
    layout_array_dbal[idx].name = "dbal_db__dbal_ppmc_table_res_types";
    layout_array_dbal[idx].type = "dbal_dynamic_table_res_types_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_dynamic_table_res_types_t);
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL);
    layout_array_dbal[idx].name = "dbal_db__hw_ent_direct_map_hash_tbl";
    layout_array_dbal[idx].type = "sw_state_idx_htbl_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(sw_state_idx_htbl_t);
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL);
    layout_array_dbal[idx].name = "dbal_db__hw_ent_groups_map_hash_tbl";
    layout_array_dbal[idx].type = "sw_state_idx_htbl_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(sw_state_idx_htbl_t);
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS);
    layout_array_dbal[idx].name = "dbal_db__field_names_override_locations";
    layout_array_dbal[idx].type = "sw_state_idx_htbl_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(sw_state_idx_htbl_t);
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS);
    layout_array_dbal[idx].name = "dbal_db__field_names_override_strings";
    layout_array_dbal[idx].type = "encap_string_type_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(encap_string_type_t);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_NOF_FIELD_NAME_OVERRIDE_INSTANCES;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 0;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS__LAST)-1;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR | DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE);
    layout_array_dbal[idx].name = "dbal_db__sw_tables__table_type";
    layout_array_dbal[idx].type = "dbal_sw_state_table_type_e";
    layout_array_dbal[idx].doc = "the table type";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_sw_state_table_type_e);
    layout_array_dbal[idx].parent  = 1;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES);
    layout_array_dbal[idx].name = "dbal_db__sw_tables__entries";
    layout_array_dbal[idx].type = "SHR_BITDCL*";
    layout_array_dbal[idx].doc = "the entries - where the table data is stored. allocated dynamically according to key size in direct table or user input in hash tables";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dbal[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_dbal[idx].parent  = 1;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE);
    layout_array_dbal[idx].name = "dbal_db__sw_tables__hash_table";
    layout_array_dbal[idx].type = "sw_state_idx_htbl_t";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(sw_state_idx_htbl_t);
    layout_array_dbal[idx].parent  = 1;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW);
    layout_array_dbal[idx].name = "dbal_db__mdb_access__skip_read_from_shadow";
    layout_array_dbal[idx].type = "uint8";
    layout_array_dbal[idx].doc = "if set, all get (read) MDB access will access HW, and not shadow (no effect on LPM tables, SW read only)";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(uint8);
    layout_array_dbal[idx].parent  = 2;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES);
    layout_array_dbal[idx].name = "dbal_db__mdb_phy_tables__nof_entries";
    layout_array_dbal[idx].type = "int";
    layout_array_dbal[idx].doc = "the num of entries in table";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(int);
    layout_array_dbal[idx].parent  = 3;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__table_status";
    layout_array_dbal[idx].type = "dbal_table_status_e";
    layout_array_dbal[idx].doc = "the table status";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_table_status_e);
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__nof_set_operations";
    layout_array_dbal[idx].type = "int";
    layout_array_dbal[idx].doc = "the num of set operations";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(int);
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER | DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__nof_get_operations";
    layout_array_dbal[idx].type = "int";
    layout_array_dbal[idx].doc = "the num of get operations";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(int);
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER | DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__nof_entries";
    layout_array_dbal[idx].type = "int";
    layout_array_dbal[idx].doc = "the num of entries in table";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(int);
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__tcam_handler_id";
    layout_array_dbal[idx].type = "uint32";
    layout_array_dbal[idx].doc = "tcam handler id";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(uint32);
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__indications_bm";
    layout_array_dbal[idx].type = "uint32";
    layout_array_dbal[idx].doc = "table indications";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(uint32);
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__iterator_optimized";
    layout_array_dbal[idx].type = "dbal_sw_state_iterator_optimized_t*";
    layout_array_dbal[idx].doc = "the table iterator_optimized db";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_sw_state_iterator_optimized_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 4;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP);
    layout_array_dbal[idx].name = "dbal_db__tables_properties__iterator_optimized__bitmap";
    layout_array_dbal[idx].type = "SHR_BITDCL*";
    layout_array_dbal[idx].doc = "the table iterator_optimized db bitmap";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dbal[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_dbal[idx].parent  = 24;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__table_id";
    layout_array_dbal[idx].type = "dbal_tables_e";
    layout_array_dbal[idx].doc = "the dynamic table id";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_tables_e);
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__table_name";
    layout_array_dbal[idx].type = "char";
    layout_array_dbal[idx].doc = "table name";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(char);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__labels";
    layout_array_dbal[idx].type = "dbal_labels_e*";
    layout_array_dbal[idx].doc = "table_labels";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_labels_e*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__data";
    layout_array_dbal[idx].type = "dbal_logical_table_t";
    layout_array_dbal[idx].doc = "the table configuration data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_logical_table_t);
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__key_info_data";
    layout_array_dbal[idx].type = "dbal_table_field_info_t*";
    layout_array_dbal[idx].doc = "the table key fields data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_table_field_info_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__multi_res_info_data";
    layout_array_dbal[idx].type = "multi_res_info_t*";
    layout_array_dbal[idx].doc = "the table result types data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(multi_res_info_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__res_info_data";
    layout_array_dbal[idx].type = "dbal_table_field_info_t**";
    layout_array_dbal[idx].doc = "the table result types data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_table_field_info_t**);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].array_indexes[1].num_elements = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit);
    layout_array_dbal[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_tables__access_info";
    layout_array_dbal[idx].type = "DNX_SW_STATE_BUFF*";
    layout_array_dbal[idx].doc = "the table access info";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(DNX_SW_STATE_BUFF*);
    layout_array_dbal[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BUFFER;
    layout_array_dbal[idx].parent  = 5;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__table_id";
    layout_array_dbal[idx].type = "dbal_tables_e";
    layout_array_dbal[idx].doc = "the dynamic table id";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_tables_e);
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__table_name";
    layout_array_dbal[idx].type = "char";
    layout_array_dbal[idx].doc = "table name";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(char);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__labels";
    layout_array_dbal[idx].type = "dbal_labels_e*";
    layout_array_dbal[idx].doc = "table_labels";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_labels_e*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__data";
    layout_array_dbal[idx].type = "dbal_logical_table_t";
    layout_array_dbal[idx].doc = "the table configuration data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_logical_table_t);
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__key_info_data";
    layout_array_dbal[idx].type = "dbal_table_field_info_t*";
    layout_array_dbal[idx].doc = "the table key fields data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_table_field_info_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__multi_res_info_data";
    layout_array_dbal[idx].type = "multi_res_info_t*";
    layout_array_dbal[idx].doc = "the table result types data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(multi_res_info_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__res_info_data";
    layout_array_dbal[idx].type = "dbal_table_field_info_t**";
    layout_array_dbal[idx].doc = "the table result types data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_table_field_info_t**);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].array_indexes[1].num_elements = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit);
    layout_array_dbal[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO);
    layout_array_dbal[idx].name = "dbal_db__dbal_dynamic_xml_tables__access_info";
    layout_array_dbal[idx].type = "DNX_SW_STATE_BUFF*";
    layout_array_dbal[idx].doc = "the table access info";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(DNX_SW_STATE_BUFF*);
    layout_array_dbal[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BUFFER;
    layout_array_dbal[idx].parent  = 6;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE);
    layout_array_dbal[idx].name = "dbal_db__dbal_fields__field_type";
    layout_array_dbal[idx].type = "dbal_field_types_defs_e";
    layout_array_dbal[idx].doc = "the dynamic field type";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_field_types_defs_e);
    layout_array_dbal[idx].parent  = 7;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME);
    layout_array_dbal[idx].name = "dbal_db__dbal_fields__field_name";
    layout_array_dbal[idx].type = "char";
    layout_array_dbal[idx].doc = "field name";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(char);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 7;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS);
    layout_array_dbal[idx].name = "dbal_db__dbal_ppmc_table_res_types__multi_res_info_status";
    layout_array_dbal[idx].type = "dbal_multi_res_info_status_e*";
    layout_array_dbal[idx].doc = "the status of the indexed result type";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_multi_res_info_status_e*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 8;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP);
    layout_array_dbal[idx].name = "dbal_db__dbal_ppmc_table_res_types__multi_res_info_data_sep";
    layout_array_dbal[idx].type = "dbal_multi_res_info_data_t*";
    layout_array_dbal[idx].doc = "the table result types data, with res info data as a separate array";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_multi_res_info_data_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 8;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__FIRST);
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_ppmc_table_res_types__multi_res_info_data_sep__multi_res_info_data";
    layout_array_dbal[idx].type = "multi_res_info_t";
    layout_array_dbal[idx].doc = "the table result types data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(multi_res_info_t);
    layout_array_dbal[idx].parent  = 45;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA);
    layout_array_dbal[idx].name = "dbal_db__dbal_ppmc_table_res_types__multi_res_info_data_sep__res_info_data";
    layout_array_dbal[idx].type = "dbal_table_field_info_t*";
    layout_array_dbal[idx].doc = "the table result types data";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(dbal_table_field_info_t*);
    layout_array_dbal[idx].array_indexes[0].num_elements = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit);
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dbal[idx].parent  = 45;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS__STRING);
    layout_array_dbal[idx].name = "dbal_db__field_names_override_strings__string";
    layout_array_dbal[idx].type = "char";
    layout_array_dbal[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].size_of = sizeof(char);
    layout_array_dbal[idx].array_indexes[0].num_elements = DBAL_MAX_STRING_LENGTH;
    layout_array_dbal[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dbal[idx].parent  = 12;
    layout_array_dbal[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dbal[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DBAL_DB, layout_array_dbal, sw_state_layout_array[unit][DBAL_MODULE_ID], DNX_SW_STATE_DBAL_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
