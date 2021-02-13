/** \file dnx/swstate/auto_generated/types/dbal_types.h
 *
 * sw state types (structs/enums/typedefs)
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DBAL_TYPES_H__
#define __DBAL_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
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
/*
 * MACROs
 */

/**
 * max key size for sw direct hash tables in bits must be word align
 */
#define DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS 128

#define HW_ENT_DIRECT_MAP_HASH_KEY_SIZE 5

#define HW_ENT_GROUPS_MAP_HASH_KEY_SIZE 5

/*
 * ENUMs
 */

/**
 * represent the sw state table type for dbal tables
 */
typedef enum {
    /**
     * direct table - the entry is set by the key. used for direct logical tables
     */
    DBAL_SW_TABLE_DIRECT = 0,
    /**
     * hash table - the entry is decided by hash table mechanism. used for non-direct logical tables (EM,TCAM,LPM)
     */
    DBAL_SW_TABLE_HASH,
    DBAL_NOF_SW_TABLES_TYPES
} dbal_sw_state_table_type_e;

/**
 * represent the table status for dbal tables
 */
typedef enum {
    /**
     * table status NOT_INITIALIZED
     */
    DBAL_TABLE_NOT_INITIALIZED = 0,
    /**
     * table status INITIALIZED
     */
    DBAL_TABLE_INITIALIZED,
    /**
     * this table received error during L2P init, some of the HW element are not valid
     */
    DBAL_TABLE_HW_ERROR,
    /**
     * status for standard 1 table at not standard 1 init
     */
    DBAL_TABLE_INCOMPATIBLE_IMAGE,
    DBAL_NOF_TABLE_STATUS
} dbal_table_status_e;

/**
 * represent the sw state table type for dbal tables
 */
typedef enum {
    /**
     * multi result type entry status availiable
     */
    DBAL_MULTI_RES_INFO_AVAIL = 0,
    /**
     * multi result type entry status used
     */
    DBAL_MULTI_RES_INFO_USED,
    DBAL_NOF_MULTI_RES_INFO_TYPES
} dbal_multi_res_info_status_e;

/*
 * STRUCTs
 */

typedef struct {
    uint8 data[HW_ENT_DIRECT_MAP_HASH_KEY_SIZE];
} hw_ent_direct_map_hash_key;

typedef struct {
    uint8 data[HW_ENT_GROUPS_MAP_HASH_KEY_SIZE];
} hw_ent_groups_map_hash_key;

typedef struct {
    uint8 data[BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)];
} dbal_sw_state_hash_key;

typedef struct {
    /**
     * the table iterator_optimized db bitmap
     */
    SHR_BITDCL* bitmap;
} dbal_sw_state_iterator_optimized_t;

/**
 * dbal sw table. hash table will be allocated accotding to table type. All tables type (direct,hash) will store the payload in the entries
 */
typedef struct {
    /**
     * the table type
     */
    dbal_sw_state_table_type_e table_type;
    /**
     * the entries - where the table data is stored. allocated dynamically according to key size in direct table or user input in hash tables
     */
    SHR_BITDCL* entries;
    sw_state_htbl_t hash_table;
} dbal_sw_state_table_t;

/**
 * holds dbal tables dynamic data
 */
typedef struct {
    /**
     * the table status
     */
    dbal_table_status_e table_status;
    /**
     * the num of set operations
     */
    int nof_set_operations;
    /**
     * the num of get operations
     */
    int nof_get_operations;
    /**
     * the num of entries in table
     */
    int nof_entries;
    /**
     * tcam handler id
     */
    uint32 tcam_handler_id;
    /**
     * table indications
     */
    uint32 indications_bm;
    /**
     * the table iterator_optimized db
     */
    dbal_sw_state_iterator_optimized_t* iterator_optimized;
} dbal_sw_state_table_properties_t;

/**
 * holds dbal dynamic tables data
 */
typedef struct {
    /**
     * the dynamic table id
     */
    dbal_tables_e table_id;
    /**
     * table name
     */
    char table_name[DBAL_MAX_STRING_LENGTH];
    /**
     * table_labels
     */
    dbal_labels_e* labels;
    /**
     * the table configuration data
     */
    dbal_logical_table_t data;
    /**
     * the table key fields data
     */
    dbal_table_field_info_t* key_info_data;
    /**
     * the table result types data
     */
    multi_res_info_t* multi_res_info_data;
    /**
     * the table result types data
     */
    dbal_table_field_info_t** res_info_data;
    /**
     * the table access info
     */
    DNX_SW_STATE_BUFF* access_info;
} dbal_dynamic_table_t;

/**
 * holds dbal dynamic result types data, with res info data as a separate array
 */
typedef struct {
    /**
     * the table result types data
     */
    multi_res_info_t multi_res_info_data;
    /**
     * the table result types data
     */
    dbal_table_field_info_t* res_info_data;
} dbal_multi_res_info_data_t;

/**
 * holds dbal dynamic tables added result types
 */
typedef struct {
    /**
     * the status of the indexed result type
     */
    dbal_multi_res_info_status_e* multi_res_info_status;
    /**
     * the table result types data, with res info data as a separate array
     */
    dbal_multi_res_info_data_t* multi_res_info_data_sep;
} dbal_dynamic_table_res_types_t;

/**
 * holds dbal dynamic fields data
 */
typedef struct {
    /**
     * the dynamic field id
     */
    dbal_fields_e field_id;
    /**
     * the dynamic field type
     */
    dbal_field_types_defs_e field_type;
    /**
     * field name
     */
    char field_name[DBAL_MAX_STRING_LENGTH];
} dbal_dynamic_fields_t;

typedef struct {
    /**
     * if set, all get (read) MDB access will access HW, and not shadow (no effect on LPM tables, SW read only)
     */
    uint8 skip_read_from_shadow;
} mdb_access_t;

typedef struct {
    /**
     * the num of entries in table
     */
    int nof_entries;
} mdb_phy_table_t;

typedef struct {
    dbal_sw_state_table_t sw_tables[DBAL_NOF_TABLES];
    mdb_access_t mdb_access;
    mdb_phy_table_t mdb_phy_tables[DBAL_NOF_PHYSICAL_TABLES];
    dbal_sw_state_table_properties_t tables_properties[DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES];
    dbal_dynamic_table_t dbal_dynamic_tables[DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES];
    dbal_dynamic_table_t dbal_dynamic_xml_tables[DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES];
    dbal_dynamic_fields_t dbal_fields[DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF];
    dbal_dynamic_table_res_types_t dbal_ppmc_table_res_types;
    sw_state_htbl_t hw_ent_direct_map_hash_tbl;
    sw_state_htbl_t hw_ent_groups_map_hash_tbl;
} dnx_dbal_sw_state_t;


#endif /* __DBAL_TYPES_H__ */
