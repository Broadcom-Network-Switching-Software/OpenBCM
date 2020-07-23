
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_TYPES_H__
#define __MDB_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb_global.h>



#define MDB_INVALID_EM_FORMAT_VALUE 0xFF


#define MDB_EM_MAX_KEYS_PLUS_TID_VALS_SIZE 320



typedef struct {
    
    mdb_macro_types_e macro_type;
    
    uint8 macro_index;
    
    uint8 cluster_index;
    
    uint8 cluster_position_in_entry;
    
    int start_address;
    
    int end_address;
} mdb_cluster_info_t;


typedef struct {
    
    uint32* fec_hierarchy_map;
    
    uint32* fec_id_range_start;
    
    uint32* fec_id_range_size;
} l3_fec_hierarchy_info_t;

typedef struct {
    
    int number_of_alloc_clusters;
    
    int number_of_clusters;
    
    int nof_cluster_per_entry;
    
    mdb_cluster_info_t* clusters_info;
} mdb_db_info_t;


typedef struct {
    
    uint8 is_flex;
    
    uint32 bank_size_used_macro_b;
    
    uint8 macro_index;
    
    mdb_macro_types_e macro_type;
    
    uint32 cluster_mapping;
} dh_em_bank_info;


typedef struct {
    
    uint8 key_size[DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS];
    
    uint8 format_count;
    
    uint32 key_to_format_map[MDB_EM_MAX_KEYS_PLUS_TID_VALS_SIZE];
    
    dh_em_bank_info em_bank_info[DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];
    
    uint32 nof_address_bits;
} em_key_format_info;


typedef struct {
    
    dbal_enum_value_field_mdb_physical_table_e pdb;
    
    uint8 flags;
} cluster_assoc_info;


typedef struct {
    
    uint32 (*data)[DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32];
} cluster_data;


typedef struct {
    
    uint8 cache_enabled;
    
    cluster_assoc_info* macro_cluster_assoc[MDB_NOF_MACRO_TYPES];
    
    cluster_data* macro_cluster_cache[MDB_NOF_MACRO_TYPES];
    
    uint32* eedb_banks_hitbit[DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS];
    
    uint32** small_kaps_bb_hitbit;
    
    uint32** small_kaps_rpb_hitbit;
    
    uint32* direct_address_mapping[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES];
    
    uint32* eedb_address_mapping;
} mdb_cluster_info;


typedef struct {
    
    int* local_lifs[2];
    
    int global_lif_destination[2];
} eedb_bank_traffic_lock_t;


typedef struct {
    
    em_key_format_info em_key_format[DBAL_NOF_PHYSICAL_TABLES];
    
    mdb_db_info_t db[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES];
    
    int capacity[DBAL_NOF_PHYSICAL_TABLES];
    
    int em_entry_count[DBAL_NOF_PHYSICAL_TABLES][DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES];
    
    int em_entry_capacity_estimate[DBAL_NOF_PHYSICAL_TABLES][DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES];
    
    int em_entry_capacity_estimate_average[DBAL_NOF_PHYSICAL_TABLES][DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES];
    
    uint32 eedb_payload_type[DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES];
    
    mdb_cluster_info mdb_cluster_infos;
    
    uint32 max_fec_id_value;
    eedb_bank_traffic_lock_t eedb_bank_traffic_lock;
    
    l3_fec_hierarchy_info_t fec_hierarchy_info;
    
    uint8 mdb_profile;
    
    uint8 mdb_em_init_done;
    
    uint32 kaps_scan_cfg_on[2];
    
    uint32 kaps_scan_cfg_paused[2];
} mdb_tables_info;


#endif 
