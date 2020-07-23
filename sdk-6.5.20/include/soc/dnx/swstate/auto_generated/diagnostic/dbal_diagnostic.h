
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DBAL_DIAGNOSTIC_H__
#define __DBAL_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dbal_types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DBAL_DB_INFO,
    DBAL_DB_SW_TABLES_INFO,
    DBAL_DB_SW_TABLES_TABLE_TYPE_INFO,
    DBAL_DB_SW_TABLES_ENTRIES_INFO,
    DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
    DBAL_DB_MDB_ACCESS_INFO,
    DBAL_DB_MDB_ACCESS_SKIP_READ_FROM_SHADOW_INFO,
    DBAL_DB_MDB_PHY_TABLES_INFO,
    DBAL_DB_MDB_PHY_TABLES_NOF_ENTRIES_INFO,
    DBAL_DB_TABLES_PROPERTIES_INFO,
    DBAL_DB_TABLES_PROPERTIES_TABLE_STATUS_INFO,
    DBAL_DB_TABLES_PROPERTIES_NOF_SET_OPERATIONS_INFO,
    DBAL_DB_TABLES_PROPERTIES_NOF_GET_OPERATIONS_INFO,
    DBAL_DB_TABLES_PROPERTIES_NOF_ENTRIES_INFO,
    DBAL_DB_TABLES_PROPERTIES_TCAM_HANDLER_ID_INFO,
    DBAL_DB_TABLES_PROPERTIES_INDICATIONS_BM_INFO,
    DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_INFO,
    DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_BITMAP_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_TABLE_ID_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_TABLE_NAME_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_LABELS_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_KEY_INFO_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_MULTI_RES_INFO_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_RES_INFO_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_TABLES_ACCESS_INFO_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_TABLE_ID_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_TABLE_NAME_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_LABELS_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_KEY_INFO_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_MULTI_RES_INFO_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_RES_INFO_DATA_INFO,
    DBAL_DB_DBAL_DYNAMIC_XML_TABLES_ACCESS_INFO_INFO,
    DBAL_DB_DBAL_FIELDS_INFO,
    DBAL_DB_DBAL_FIELDS_FIELD_ID_INFO,
    DBAL_DB_DBAL_FIELDS_FIELD_TYPE_INFO,
    DBAL_DB_DBAL_FIELDS_FIELD_NAME_INFO,
    DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_INFO,
    DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_STATUS_INFO,
    DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_INFO,
    DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_MULTI_RES_INFO_DATA_INFO,
    DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_RES_INFO_DATA_INFO,
    DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
    DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
    DBAL_DB_INFO_NOF_ENTRIES
} sw_state_dbal_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dbal_db_info[SOC_MAX_NUM_DEVICES][DBAL_DB_INFO_NOF_ENTRIES];

extern const char* dbal_db_layout_str[DBAL_DB_INFO_NOF_ENTRIES];
int dbal_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dbal_db_sw_tables_dump(
    int unit, int sw_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_sw_tables_table_type_dump(
    int unit, int sw_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_sw_tables_entries_dump(
    int unit, int sw_tables_idx_0, int entries_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_sw_tables_hash_table_dump(
    int unit, int sw_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_mdb_access_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dbal_db_mdb_access_skip_read_from_shadow_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dbal_db_mdb_phy_tables_dump(
    int unit, int mdb_phy_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_mdb_phy_tables_nof_entries_dump(
    int unit, int mdb_phy_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_table_status_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_nof_set_operations_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_nof_get_operations_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_nof_entries_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_tcam_handler_id_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_indications_bm_dump(
    int unit, int tables_properties_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_iterator_optimized_dump(
    int unit, int tables_properties_idx_0, int iterator_optimized_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_tables_properties_iterator_optimized_bitmap_dump(
    int unit, int tables_properties_idx_0, int iterator_optimized_idx_0, int bitmap_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_dump(
    int unit, int dbal_dynamic_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_table_id_dump(
    int unit, int dbal_dynamic_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_table_name_dump(
    int unit, int dbal_dynamic_tables_idx_0, int table_name_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_labels_dump(
    int unit, int dbal_dynamic_tables_idx_0, int labels_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_data_dump(
    int unit, int dbal_dynamic_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_key_info_data_dump(
    int unit, int dbal_dynamic_tables_idx_0, int key_info_data_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_multi_res_info_data_dump(
    int unit, int dbal_dynamic_tables_idx_0, int multi_res_info_data_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_res_info_data_dump(
    int unit, int dbal_dynamic_tables_idx_0, int res_info_data_idx_0, int res_info_data_idx_1, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_tables_access_info_dump(
    int unit, int dbal_dynamic_tables_idx_0, int access_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_table_id_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_table_name_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, int table_name_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_labels_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, int labels_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_data_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_key_info_data_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, int key_info_data_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, int multi_res_info_data_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_res_info_data_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, int res_info_data_idx_0, int res_info_data_idx_1, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_dynamic_xml_tables_access_info_dump(
    int unit, int dbal_dynamic_xml_tables_idx_0, int access_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_fields_dump(
    int unit, int dbal_fields_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_fields_field_id_dump(
    int unit, int dbal_fields_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_fields_field_type_dump(
    int unit, int dbal_fields_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_fields_field_name_dump(
    int unit, int dbal_fields_idx_0, int field_name_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_ppmc_table_res_types_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_dump(
    int unit, int multi_res_info_status_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_dump(
    int unit, int multi_res_info_data_sep_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_dump(
    int unit, int multi_res_info_data_sep_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_dump(
    int unit, int multi_res_info_data_sep_idx_0, int res_info_data_idx_0, dnx_sw_state_dump_filters_t filters);

int dbal_db_hw_ent_direct_map_hash_tbl_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dbal_db_hw_ent_groups_map_hash_tbl_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
