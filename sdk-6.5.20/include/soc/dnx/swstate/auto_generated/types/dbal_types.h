
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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



#define DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS 96

#define HW_ENT_DIRECT_MAP_HASH_KEY_SIZE 5

#define HW_ENT_GROUPS_MAP_HASH_KEY_SIZE 5




typedef enum {
    
    DBAL_SW_TABLE_DIRECT = 0,
    
    DBAL_SW_TABLE_HASH,
    DBAL_NOF_SW_TABLES_TYPES
} dbal_sw_state_table_type_e;


typedef enum {
    
    DBAL_TABLE_NOT_INITIALIZED = 0,
    
    DBAL_TABLE_INITIALIZED,
    
    DBAL_TABLE_HW_ERROR,
    
    DBAL_TABLE_INCOMPATIBLE_IMAGE,
    DBAL_NOF_TABLE_STATUS
} dbal_table_status_e;


typedef enum {
    
    DBAL_MULTI_RES_INFO_AVAIL = 0,
    
    DBAL_MULTI_RES_INFO_USED,
    DBAL_NOF_MULTI_RES_INFO_TYPES
} dbal_multi_res_info_status_e;



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
    
    SHR_BITDCL* bitmap;
} dbal_sw_state_iterator_optimized_t;


typedef struct {
    
    dbal_sw_state_table_type_e table_type;
    
    SHR_BITDCL* entries;
    sw_state_htbl_t hash_table;
} dbal_sw_state_table_t;


typedef struct {
    
    dbal_table_status_e table_status;
    
    int nof_set_operations;
    
    int nof_get_operations;
    
    int nof_entries;
    
    uint32 tcam_handler_id;
    
    uint32 indications_bm;
    
    dbal_sw_state_iterator_optimized_t* iterator_optimized;
} dbal_sw_state_table_properties_t;


typedef struct {
    
    dbal_tables_e table_id;
    
    char table_name[DBAL_MAX_STRING_LENGTH];
    
    dbal_labels_e* labels;
    
    dbal_logical_table_t data;
    
    dbal_table_field_info_t* key_info_data;
    
    multi_res_info_t* multi_res_info_data;
    
    dbal_table_field_info_t** res_info_data;
    
    DNX_SW_STATE_BUFF* access_info;
} dbal_dynamic_table_t;


typedef struct {
    
    multi_res_info_t multi_res_info_data;
    
    dbal_table_field_info_t* res_info_data;
} dbal_multi_res_info_data_t;


typedef struct {
    
    dbal_multi_res_info_status_e* multi_res_info_status;
    
    dbal_multi_res_info_data_t* multi_res_info_data_sep;
} dbal_dynamic_table_res_types_t;


typedef struct {
    
    dbal_fields_e field_id;
    
    dbal_field_types_defs_e field_type;
    
    char field_name[DBAL_MAX_STRING_LENGTH];
} dbal_dynamic_fields_t;

typedef struct {
    
    uint8 skip_read_from_shadow;
} mdb_access_t;

typedef struct {
    
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


#endif 
