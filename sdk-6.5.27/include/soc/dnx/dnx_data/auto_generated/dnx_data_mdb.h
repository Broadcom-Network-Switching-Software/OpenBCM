
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MDB_H_

#define _DNX_DATA_MDB_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/mdb_global.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mdb.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_mdb_init(
    int unit);







typedef enum
{
    dnx_data_mdb_global_hitbit_support,
    dnx_data_mdb_global_capacity_support,
    dnx_data_mdb_global_init_enable,
    dnx_data_mdb_global_block_id_fixed_mdb_value,
    dnx_data_mdb_global_lpm_acl_enable,
    dnx_data_mdb_global_mdb_access_version,

    
    _dnx_data_mdb_global_feature_nof
} dnx_data_mdb_global_feature_e;



typedef int(
    *dnx_data_mdb_global_feature_get_f) (
    int unit,
    dnx_data_mdb_global_feature_e feature);


typedef uint32(
    *dnx_data_mdb_global_utilization_api_nof_dbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_global_adapter_stub_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_mdb_global_feature_get_f feature_get;
    
    dnx_data_mdb_global_utilization_api_nof_dbs_get_f utilization_api_nof_dbs_get;
    
    dnx_data_mdb_global_adapter_stub_enable_get_f adapter_stub_enable_get;
} dnx_data_if_mdb_global_t;






typedef struct
{
    uint32 table_macro_interface_mapping[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_EM_OVF_OR_EEDB_BANK];
    uint32 cluster_if_offsets_values[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH];
    uint32 if_cluster_offsets_values[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH];
    uint32 macro_mapping[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES_PLUS_DATA_WIDTH];
    uint32 dh_in_width;
    uint32 dh_out_width;
    uint32 two_ways_connectivity_bm;
} dnx_data_mdb_dh_dh_info_t;


typedef struct
{
    dbal_enum_value_field_mdb_block_types_e block_type;
    int block_index;
} dnx_data_mdb_dh_block_info_t;


typedef struct
{
    mdb_macro_types_e macro_type;
    int macro_index;
    int is_dpc;
} dnx_data_mdb_dh_logical_macro_info_t;


typedef struct
{
    uint8 nof_macros;
    uint8 nof_clusters;
    uint32 nof_rows;
    uint8 nof_address_bits;
    uint8 global_start_index;
    char *xml_str;
    char *name;
    uint8 hitbit_support;
    uint8 blk_core_group_inc;
} dnx_data_mdb_dh_macro_type_info_t;


typedef struct
{
    uint8 macro_halved;
} dnx_data_mdb_dh_mdb_75_macro_halved_t;


typedef struct
{
    uint32 entry_bank;
    uint32 overflow_reg;
    uint32 abk_bank_a;
    uint32 abk_bank_b;
} dnx_data_mdb_dh_entry_banks_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_1_field_size;
    uint32 mdb_item_2_array_size;
    uint32 mdb_item_3_field_size;
} dnx_data_mdb_dh_mdb_1_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_1_field_size;
} dnx_data_mdb_dh_mdb_2_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_array_size;
} dnx_data_mdb_dh_mdb_3_info_t;


typedef struct
{
    mdb_macro_types_e macro_types[DNX_DATA_MAX_MDB_DH_NOF_FEC_MACRO_TYPES];
} dnx_data_mdb_dh_mdb_fec_macro_types_info_t;


typedef struct
{
    uint32 is_valid[2];
    uint32 total_nof_active_macroes;
} dnx_data_mdb_dh_macro_global_indexes_t;



typedef enum
{
    dnx_data_mdb_dh_macro_A_half_nof_clusters_enable,
    dnx_data_mdb_dh_macro_A_75_nof_clusters_enable,
    dnx_data_mdb_dh_bpu_setup_extended_support,

    
    _dnx_data_mdb_dh_feature_nof
} dnx_data_mdb_dh_feature_e;



typedef int(
    *dnx_data_mdb_dh_feature_get_f) (
    int unit,
    dnx_data_mdb_dh_feature_e feature);


typedef uint32(
    *dnx_data_mdb_dh_total_nof_macroes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_total_nof_macroes_plus_data_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_max_nof_cluster_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_max_nof_clusters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_max_nof_db_clusters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_bucket_clusters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_buckets_in_macro_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_pair_clusters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_ddha_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_ddhb_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_dhc_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_ddhc_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_ddhx_blocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_cluster_row_width_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_cluster_row_width_uint32_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_data_out_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_bb_connected_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_120_240_120_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_480_odd_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_480_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_360_120_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_120_360_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_240_240_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_120_120_240_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_240_120_120_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_dh_nof_fec_macro_types_get_f) (
    int unit);


typedef const dnx_data_mdb_dh_dh_info_t *(
    *dnx_data_mdb_dh_dh_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_dh_block_info_t *(
    *dnx_data_mdb_dh_block_info_get_f) (
    int unit,
    int global_macro_index);


typedef const dnx_data_mdb_dh_logical_macro_info_t *(
    *dnx_data_mdb_dh_logical_macro_info_get_f) (
    int unit,
    int global_macro_index);


typedef const dnx_data_mdb_dh_macro_type_info_t *(
    *dnx_data_mdb_dh_macro_type_info_get_f) (
    int unit,
    int macro_type);


typedef const dnx_data_mdb_dh_mdb_75_macro_halved_t *(
    *dnx_data_mdb_dh_mdb_75_macro_halved_get_f) (
    int unit,
    int macro_index);


typedef const dnx_data_mdb_dh_entry_banks_info_t *(
    *dnx_data_mdb_dh_entry_banks_info_get_f) (
    int unit,
    int global_macro_index);


typedef const dnx_data_mdb_dh_mdb_1_info_t *(
    *dnx_data_mdb_dh_mdb_1_info_get_f) (
    int unit,
    int global_macro_index);


typedef const dnx_data_mdb_dh_mdb_2_info_t *(
    *dnx_data_mdb_dh_mdb_2_info_get_f) (
    int unit,
    int global_macro_index);


typedef const dnx_data_mdb_dh_mdb_3_info_t *(
    *dnx_data_mdb_dh_mdb_3_info_get_f) (
    int unit,
    int global_macro_index);


typedef const dnx_data_mdb_dh_mdb_fec_macro_types_info_t *(
    *dnx_data_mdb_dh_mdb_fec_macro_types_info_get_f) (
    int unit);


typedef const dnx_data_mdb_dh_macro_global_indexes_t *(
    *dnx_data_mdb_dh_macro_global_indexes_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_mdb_dh_feature_get_f feature_get;
    
    dnx_data_mdb_dh_total_nof_macroes_get_f total_nof_macroes_get;
    
    dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_get_f total_nof_macroes_plus_em_ovf_or_eedb_bank_get;
    
    dnx_data_mdb_dh_total_nof_macroes_plus_data_width_get_f total_nof_macroes_plus_data_width_get;
    
    dnx_data_mdb_dh_max_nof_cluster_interfaces_get_f max_nof_cluster_interfaces_get;
    
    dnx_data_mdb_dh_max_nof_clusters_get_f max_nof_clusters_get;
    
    dnx_data_mdb_dh_max_nof_db_clusters_get_f max_nof_db_clusters_get;
    
    dnx_data_mdb_dh_nof_bucket_clusters_get_f nof_bucket_clusters_get;
    
    dnx_data_mdb_dh_nof_buckets_in_macro_get_f nof_buckets_in_macro_get;
    
    dnx_data_mdb_dh_nof_pair_clusters_get_f nof_pair_clusters_get;
    
    dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_get_f nof_data_rows_per_hitbit_row_get;
    
    dnx_data_mdb_dh_nof_ddha_blocks_get_f nof_ddha_blocks_get;
    
    dnx_data_mdb_dh_nof_ddhb_blocks_get_f nof_ddhb_blocks_get;
    
    dnx_data_mdb_dh_nof_dhc_blocks_get_f nof_dhc_blocks_get;
    
    dnx_data_mdb_dh_nof_ddhc_blocks_get_f nof_ddhc_blocks_get;
    
    dnx_data_mdb_dh_nof_ddhx_blocks_get_f nof_ddhx_blocks_get;
    
    dnx_data_mdb_dh_cluster_row_width_bits_get_f cluster_row_width_bits_get;
    
    dnx_data_mdb_dh_cluster_row_width_uint32_get_f cluster_row_width_uint32_get;
    
    dnx_data_mdb_dh_data_out_granularity_get_f data_out_granularity_get;
    
    dnx_data_mdb_dh_bpu_setup_bb_connected_get_f bpu_setup_bb_connected_get;
    
    dnx_data_mdb_dh_bpu_setup_size_120_240_120_get_f bpu_setup_size_120_240_120_get;
    
    dnx_data_mdb_dh_bpu_setup_size_480_odd_get_f bpu_setup_size_480_odd_get;
    
    dnx_data_mdb_dh_bpu_setup_size_480_get_f bpu_setup_size_480_get;
    
    dnx_data_mdb_dh_bpu_setup_size_360_120_get_f bpu_setup_size_360_120_get;
    
    dnx_data_mdb_dh_bpu_setup_size_120_360_get_f bpu_setup_size_120_360_get;
    
    dnx_data_mdb_dh_bpu_setup_size_240_240_get_f bpu_setup_size_240_240_get;
    
    dnx_data_mdb_dh_bpu_setup_size_120_120_240_get_f bpu_setup_size_120_120_240_get;
    
    dnx_data_mdb_dh_bpu_setup_size_240_120_120_get_f bpu_setup_size_240_120_120_get;
    
    dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_get_f bpu_setup_size_120_120_120_120_get;
    
    dnx_data_mdb_dh_nof_fec_macro_types_get_f nof_fec_macro_types_get;
    
    dnx_data_mdb_dh_dh_info_get_f dh_info_get;
    
    dnxc_data_table_info_get_f dh_info_info_get;
    
    dnx_data_mdb_dh_block_info_get_f block_info_get;
    
    dnxc_data_table_info_get_f block_info_info_get;
    
    dnx_data_mdb_dh_logical_macro_info_get_f logical_macro_info_get;
    
    dnxc_data_table_info_get_f logical_macro_info_info_get;
    
    dnx_data_mdb_dh_macro_type_info_get_f macro_type_info_get;
    
    dnxc_data_table_info_get_f macro_type_info_info_get;
    
    dnx_data_mdb_dh_mdb_75_macro_halved_get_f mdb_75_macro_halved_get;
    
    dnxc_data_table_info_get_f mdb_75_macro_halved_info_get;
    
    dnx_data_mdb_dh_entry_banks_info_get_f entry_banks_info_get;
    
    dnxc_data_table_info_get_f entry_banks_info_info_get;
    
    dnx_data_mdb_dh_mdb_1_info_get_f mdb_1_info_get;
    
    dnxc_data_table_info_get_f mdb_1_info_info_get;
    
    dnx_data_mdb_dh_mdb_2_info_get_f mdb_2_info_get;
    
    dnxc_data_table_info_get_f mdb_2_info_info_get;
    
    dnx_data_mdb_dh_mdb_3_info_get_f mdb_3_info_get;
    
    dnxc_data_table_info_get_f mdb_3_info_info_get;
    
    dnx_data_mdb_dh_mdb_fec_macro_types_info_get_f mdb_fec_macro_types_info_get;
    
    dnxc_data_table_info_get_f mdb_fec_macro_types_info_info_get;
    
    dnx_data_mdb_dh_macro_global_indexes_get_f macro_global_indexes_get;
    
    dnxc_data_table_info_get_f macro_global_indexes_info_get;
} dnx_data_if_mdb_dh_t;






typedef struct
{
    char *profile;
} dnx_data_mdb_pdbs_mdb_profile_t;


typedef struct
{
    uint8 supported;
    char *name;
} dnx_data_mdb_pdbs_mdb_profiles_info_t;


typedef struct
{
    uint32 val;
} dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t;


typedef struct
{
    uint8 valid;
    char *xml_name;
} dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t;


typedef struct
{
    uint8 valid;
    char *xml_name;
    uint8 vmv_xml_chain;
} dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t;


typedef struct
{
    dbal_physical_tables_e physical_to_logical;
    dbal_enum_value_field_mdb_db_type_e db_type;
    uint8 db_subtype;
    char *name;
    uint32 row_width;
    mdb_macro_types_e macro_granularity;
    dbal_enum_value_field_direct_payload_sizes_e direct_payload_type;
    dbal_enum_value_field_direct_payload_sizes_e direct_max_payload_type;
} dnx_data_mdb_pdbs_mdb_pdb_info_t;


typedef struct
{
    mdb_physical_table_e logical_to_physical;
    dbal_enum_value_field_mdb_db_type_e db_type;
    uint8 db_subtype;
    char *name;
    char *dbal_str;
    dbal_core_mode_e core_mode;
    uint32 row_width;
    uint32 max_key_size;
    uint32 max_payload_size;
} dnx_data_mdb_pdbs_dbal_pdb_info_t;


typedef struct
{
    uint8 mdb_type;
    uint8 memory_id;
    int capacity;
} dnx_data_mdb_pdbs_mdb_adapter_mapping_t;


typedef struct
{
    uint8 bank_alloc_resolution;
    mdb_macro_types_e base_macro_type;
} dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
} dnx_data_mdb_pdbs_mdb_51_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
} dnx_data_mdb_pdbs_mdb_53_info_t;



typedef enum
{

    
    _dnx_data_mdb_pdbs_feature_nof
} dnx_data_mdb_pdbs_feature_e;



typedef int(
    *dnx_data_mdb_pdbs_feature_get_f) (
    int unit,
    dnx_data_mdb_pdbs_feature_e feature);


typedef uint32(
    *dnx_data_mdb_pdbs_max_nof_interface_dhs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_mesh_mode_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_max_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_max_payload_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_mdb_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_ddha_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_ddhab_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdba_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdbb_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_table_mdbk_dynamic_memory_access_memory_access_field_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_pdbs_adapter_mapping_crps_id_get_f) (
    int unit);


typedef const dnx_data_mdb_pdbs_mdb_profile_t *(
    *dnx_data_mdb_pdbs_mdb_profile_get_f) (
    int unit);


typedef const dnx_data_mdb_pdbs_mdb_profiles_info_t *(
    *dnx_data_mdb_pdbs_mdb_profiles_info_get_f) (
    int unit,
    int mdb_profile_index);


typedef const dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t *(
    *dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_get_f) (
    int unit);


typedef const dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t *(
    *dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t *(
    *dnx_data_mdb_pdbs_dbal_vmv_str_mapping_get_f) (
    int unit,
    int dbal_table_id);


typedef const dnx_data_mdb_pdbs_mdb_pdb_info_t *(
    *dnx_data_mdb_pdbs_mdb_pdb_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_pdbs_dbal_pdb_info_t *(
    *dnx_data_mdb_pdbs_dbal_pdb_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_pdbs_mdb_adapter_mapping_t *(
    *dnx_data_mdb_pdbs_mdb_adapter_mapping_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t *(
    *dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_get_f) (
    int unit,
    int fec_physical_db);


typedef const dnx_data_mdb_pdbs_mdb_51_info_t *(
    *dnx_data_mdb_pdbs_mdb_51_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_pdbs_mdb_53_info_t *(
    *dnx_data_mdb_pdbs_mdb_53_info_get_f) (
    int unit,
    int mdb_table);



typedef struct
{
    
    dnx_data_mdb_pdbs_feature_get_f feature_get;
    
    dnx_data_mdb_pdbs_max_nof_interface_dhs_get_f max_nof_interface_dhs_get;
    
    dnx_data_mdb_pdbs_mesh_mode_support_get_f mesh_mode_support_get;
    
    dnx_data_mdb_pdbs_max_key_size_get_f max_key_size_get;
    
    dnx_data_mdb_pdbs_max_payload_size_get_f max_payload_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_get_f table_mdb_9_mdb_item_1_array_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_get_f table_mdb_9_mdb_item_2_array_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_get_f table_mdb_9_mdb_item_3_array_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_get_f table_mdb_40_mdb_item_0_array_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_get_f table_mdb_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_get_f table_mdb_mact_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get_f table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get_f table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_mdb_nof_profiles_get_f mdb_nof_profiles_get;
    
    dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_memory_access_field_size_get_f table_mdb_kaps_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_memory_access_field_size_get_f table_mdb_ddha_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_ddha_dynamic_memory_access_memory_access_field_size_get_f table_ddha_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_ddhab_dynamic_memory_access_memory_access_field_size_get_f table_ddhab_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdba_dynamic_memory_access_memory_access_field_size_get_f table_mdba_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdbb_dynamic_memory_access_memory_access_field_size_get_f table_mdbb_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_table_mdbk_dynamic_memory_access_memory_access_field_size_get_f table_mdbk_dynamic_memory_access_memory_access_field_size_get;
    
    dnx_data_mdb_pdbs_adapter_mapping_crps_id_get_f adapter_mapping_crps_id_get;
    
    dnx_data_mdb_pdbs_mdb_profile_get_f mdb_profile_get;
    
    dnxc_data_table_info_get_f mdb_profile_info_get;
    
    dnx_data_mdb_pdbs_mdb_profiles_info_get_f mdb_profiles_info_get;
    
    dnxc_data_table_info_get_f mdb_profiles_info_info_get;
    
    dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_get_f mdb_profile_kaps_cfg_get;
    
    dnxc_data_table_info_get_f mdb_profile_kaps_cfg_info_get;
    
    dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_get_f mdb_profile_table_str_mapping_get;
    
    dnxc_data_table_info_get_f mdb_profile_table_str_mapping_info_get;
    
    dnx_data_mdb_pdbs_dbal_vmv_str_mapping_get_f dbal_vmv_str_mapping_get;
    
    dnxc_data_table_info_get_f dbal_vmv_str_mapping_info_get;
    
    dnx_data_mdb_pdbs_mdb_pdb_info_get_f mdb_pdb_info_get;
    
    dnxc_data_table_info_get_f mdb_pdb_info_info_get;
    
    dnx_data_mdb_pdbs_dbal_pdb_info_get_f dbal_pdb_info_get;
    
    dnxc_data_table_info_get_f dbal_pdb_info_info_get;
    
    dnx_data_mdb_pdbs_mdb_adapter_mapping_get_f mdb_adapter_mapping_get;
    
    dnxc_data_table_info_get_f mdb_adapter_mapping_info_get;
    
    dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_get_f fec_physical_dbs_alloc_info_get;
    
    dnxc_data_table_info_get_f fec_physical_dbs_alloc_info_info_get;
    
    dnx_data_mdb_pdbs_mdb_51_info_get_f mdb_51_info_get;
    
    dnxc_data_table_info_get_f mdb_51_info_info_get;
    
    dnx_data_mdb_pdbs_mdb_53_info_get_f mdb_53_info_get;
    
    dnxc_data_table_info_get_f mdb_53_info_info_get;
} dnx_data_if_mdb_pdbs_t;






typedef struct
{
    uint8 dbal_id;
} dnx_data_mdb_em_em_utilization_api_db_translation_t;


typedef struct
{
    uint8 nof_spn_size;
    uint8 spn_array[DNX_DATA_MAX_MDB_EM_MAX_NOF_SPN_SIZES];
} dnx_data_mdb_em_spn_t;


typedef struct
{
    int lfsr_size;
} dnx_data_mdb_em_lfsr_t;


typedef struct
{
    uint8 max_compare_size;
} dnx_data_mdb_em_compare_size_t;


typedef struct
{
    dbal_physical_tables_e dbal_id;
} dnx_data_mdb_em_emp_index_table_mapping_t;


typedef struct
{
    uint8 max_nof_age_entry_bits;
    uint8 rbd_size;
    uint32 nof_age_banks;
    uint32 total_nof_aging_bits;
} dnx_data_mdb_em_em_aging_info_t;


typedef struct
{
    uint8 init_value;
    uint8 global_value;
    uint8 global_mask;
    uint8 external_profile;
    uint8 aging_disable;
    uint8 elephant_disable;
    uint8 elephant_values;
    uint8 mouse_values;
    uint8 age_max_values;
    uint8 increment_values;
    uint8 decrement_values;
    uint8 out_values;
    dbal_enum_value_field_mdb_em_aging_hit_bit_e hit_bit_mode;
} dnx_data_mdb_em_em_aging_cfg_t;


typedef struct
{
    uint32 tid_size;
    uint32 em_interface;
    uint32 entry_size;
    uint32 status_reg;
    uint32 step_table_size;
    uint32 ovf_cam_size;
} dnx_data_mdb_em_em_info_t;


typedef struct
{
    int instance;
    uint32 interrupt_register;
    uint32 interrupt_field;
} dnx_data_mdb_em_em_interrupt_t;


typedef struct
{
    uint32 max_depth;
} dnx_data_mdb_em_step_table_pdb_max_depth_t;


typedef struct
{
    uint32 max_depth;
} dnx_data_mdb_em_step_table_max_depth_possible_t;


typedef struct
{
    uint32 max_depth;
} dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_5_field_size;
} dnx_data_mdb_em_mdb_15_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_field_size;
} dnx_data_mdb_em_mdb_16_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_field_size;
} dnx_data_mdb_em_mdb_21_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_field_size;
    uint32 mdb_item_1_field_size;
} dnx_data_mdb_em_mdb_23_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_array_size;
    uint32 mdb_item_0_field_size;
    uint32 mdb_item_1_array_size;
    uint32 mdb_item_1_field_size;
} dnx_data_mdb_em_mdb_24_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_array_size;
    uint32 mdb_item_1_array_size;
} dnx_data_mdb_em_mdb_29_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 array_size;
    uint32 mdb_item_0_field_size;
} dnx_data_mdb_em_mdb_31_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_field_size;
} dnx_data_mdb_em_mdb_32_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 array_size;
    uint32 mdb_item_2_field_size;
    uint32 mdb_item_3_field_size;
} dnx_data_mdb_em_mdb_41_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 array_size;
    uint32 mdb_item_3_field_size;
    uint32 mdb_item_7_field_size;
} dnx_data_mdb_em_mdb_45_info_t;


typedef struct
{
    dbal_tables_e emp_age_cfg_table;
    dbal_tables_e mdb_22_table;
} dnx_data_mdb_em_mdb_emp_tables_mapping_t;


typedef struct
{
    uint8 nof_valid_regs;
    dbal_tables_e table_name_arr[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE];
    dbal_fields_e field_name_arr[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE];
    dbal_fields_e key_name_arr[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE];
    uint8 stage_index_arr[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE];
    uint8 esem_cmd_indication[DNX_DATA_MAX_MDB_EM_SHIFT_VMV_MAX_REGS_PER_TABLE];
} dnx_data_mdb_em_mdb_em_shift_vmv_regs_t;


typedef struct
{
    uint32 age_mem;
    uint32 age_ovf_cam_mem;
    uint32 emp_scan_status_field;
    uint32 emp_age_cfg_reg;
    uint32 global_value;
    uint32 global_masks;
    uint32 external_profile;
    uint32 age_disable;
    uint32 elephant_disable;
    uint32 initial_values;
} dnx_data_mdb_em_em_emp_t;


typedef struct
{
    dbal_tables_e dbal_table;
} dnx_data_mdb_em_mdb_step_table_map_t;


typedef struct
{
    dbal_tables_e mdb_13_table;
    dbal_tables_e mdb_14_table;
    dbal_tables_e mdb_17_table;
    dbal_tables_e mdb_18_table;
    dbal_tables_e mdb_33_table;
    dbal_tables_e mdb_34_table;
    dbal_tables_e mdb_46_table;
    dbal_tables_e mdb_52_table;
} dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t;


typedef struct
{
    uint32 table_name;
    uint32 valid;
    uint32 entry_mask;
    uint32 entry_filter;
    uint32 src_mask;
    uint32 src_filter;
    uint32 appdb_id_mask;
    uint32 appdb_id_filter;
    uint32 accessed_mask;
    uint32 accessed_filter;
} dnx_data_mdb_em_filter_rules_t;


typedef struct
{
    uint32 table_name;
    uint32 command;
    uint32 payload_mask;
    uint32 payload;
} dnx_data_mdb_em_data_rules_t;



typedef enum
{
    dnx_data_mdb_em_entry_counter_decrease_support,
    dnx_data_mdb_em_entry_type_parser,
    dnx_data_mdb_em_step_table_any_cmds_support,
    dnx_data_mdb_em_multi_interface_blocks,
    dnx_data_mdb_em_is_isem_dpc_in_vtt5,

    
    _dnx_data_mdb_em_feature_nof
} dnx_data_mdb_em_feature_e;



typedef int(
    *dnx_data_mdb_em_feature_get_f) (
    int unit,
    dnx_data_mdb_em_feature_e feature);


typedef uint32(
    *dnx_data_mdb_em_age_row_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_age_ovfcam_row_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_ovf_cam_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_age_support_per_entry_size_ratio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flush_tcam_rule_counter_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_aging_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_aging_profiles_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_max_tid_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_max_nof_tids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flush_support_tids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flush_max_supported_key_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flush_max_supported_key_plus_payload_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_format_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_max_nof_vmv_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_vmv_nof_values_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_vmv_size_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_esem_nof_vmv_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_glem_nof_vmv_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_mact_nof_vmv_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_mact_max_payload_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_shift_vmv_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_shift_vmv_max_regs_per_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_min_nof_app_id_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_max_nof_spn_sizes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_lfsr_sizes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_formats_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_defragmentation_priority_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_disable_cuckoo_loop_detection_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_age_profile_per_ratio_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flex_mag_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flex_fully_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_step_table_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_encoding_values_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_encoding_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_extra_vmv_shift_registers_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_sbus_error_code_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_nof_emp_tables_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_defrag_interrupt_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_glem_bypass_option_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flush_max_supported_payload_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_flush_payload_max_bus_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_my_mac_ippa_em_dbal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_my_mac_ippf_em_dbal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_em_mact_dbal_id_get_f) (
    int unit);


typedef const dnx_data_mdb_em_em_utilization_api_db_translation_t *(
    *dnx_data_mdb_em_em_utilization_api_db_translation_get_f) (
    int unit,
    int resource_id);


typedef const dnx_data_mdb_em_spn_t *(
    *dnx_data_mdb_em_spn_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_lfsr_t *(
    *dnx_data_mdb_em_lfsr_get_f) (
    int unit,
    int lfsr_index);


typedef const dnx_data_mdb_em_compare_size_t *(
    *dnx_data_mdb_em_compare_size_get_f) (
    int unit,
    int entry_size);


typedef const dnx_data_mdb_em_emp_index_table_mapping_t *(
    *dnx_data_mdb_em_emp_index_table_mapping_get_f) (
    int unit,
    int emp_table_idx);


typedef const dnx_data_mdb_em_em_aging_info_t *(
    *dnx_data_mdb_em_em_aging_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_em_aging_cfg_t *(
    *dnx_data_mdb_em_em_aging_cfg_get_f) (
    int unit,
    int profile,
    int dbal_id);


typedef const dnx_data_mdb_em_em_info_t *(
    *dnx_data_mdb_em_em_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_em_interrupt_t *(
    *dnx_data_mdb_em_em_interrupt_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_step_table_pdb_max_depth_t *(
    *dnx_data_mdb_em_step_table_pdb_max_depth_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_step_table_max_depth_possible_t *(
    *dnx_data_mdb_em_step_table_max_depth_possible_get_f) (
    int unit,
    int step_table_size_indication,
    int aspect_ratio_combination);


typedef const dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t *(
    *dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_get_f) (
    int unit,
    int aspect_ratio,
    int nof_ways);


typedef const dnx_data_mdb_em_mdb_15_info_t *(
    *dnx_data_mdb_em_mdb_15_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_mdb_16_info_t *(
    *dnx_data_mdb_em_mdb_16_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_em_mdb_21_info_t *(
    *dnx_data_mdb_em_mdb_21_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_em_mdb_23_info_t *(
    *dnx_data_mdb_em_mdb_23_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_em_mdb_24_info_t *(
    *dnx_data_mdb_em_mdb_24_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_em_mdb_29_info_t *(
    *dnx_data_mdb_em_mdb_29_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_em_mdb_31_info_t *(
    *dnx_data_mdb_em_mdb_31_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_mdb_32_info_t *(
    *dnx_data_mdb_em_mdb_32_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_mdb_41_info_t *(
    *dnx_data_mdb_em_mdb_41_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_mdb_45_info_t *(
    *dnx_data_mdb_em_mdb_45_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_mdb_emp_tables_mapping_t *(
    *dnx_data_mdb_em_mdb_emp_tables_mapping_get_f) (
    int unit,
    int mdb_table);


typedef const dnx_data_mdb_em_mdb_em_shift_vmv_regs_t *(
    *dnx_data_mdb_em_mdb_em_shift_vmv_regs_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_em_emp_t *(
    *dnx_data_mdb_em_em_emp_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_mdb_step_table_map_t *(
    *dnx_data_mdb_em_mdb_step_table_map_get_f) (
    int unit,
    int mdb_table);


typedef const dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t *(
    *dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_filter_rules_t *(
    *dnx_data_mdb_em_filter_rules_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_em_data_rules_t *(
    *dnx_data_mdb_em_data_rules_get_f) (
    int unit,
    int dbal_id);



typedef struct
{
    
    dnx_data_mdb_em_feature_get_f feature_get;
    
    dnx_data_mdb_em_age_row_size_bits_get_f age_row_size_bits_get;
    
    dnx_data_mdb_em_age_ovfcam_row_size_bits_get_f age_ovfcam_row_size_bits_get;
    
    dnx_data_mdb_em_ovf_cam_max_size_get_f ovf_cam_max_size_get;
    
    dnx_data_mdb_em_age_support_per_entry_size_ratio_get_f age_support_per_entry_size_ratio_get;
    
    dnx_data_mdb_em_flush_tcam_rule_counter_support_get_f flush_tcam_rule_counter_support_get;
    
    dnx_data_mdb_em_nof_aging_profiles_get_f nof_aging_profiles_get;
    
    dnx_data_mdb_em_aging_profiles_size_in_bits_get_f aging_profiles_size_in_bits_get;
    
    dnx_data_mdb_em_max_tid_size_get_f max_tid_size_get;
    
    dnx_data_mdb_em_max_nof_tids_get_f max_nof_tids_get;
    
    dnx_data_mdb_em_flush_support_tids_get_f flush_support_tids_get;
    
    dnx_data_mdb_em_flush_max_supported_key_get_f flush_max_supported_key_get;
    
    dnx_data_mdb_em_flush_max_supported_key_plus_payload_get_f flush_max_supported_key_plus_payload_get;
    
    dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_get_f dh_120_entry_encoding_nof_bits_get;
    
    dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_get_f dh_240_entry_encoding_nof_bits_get;
    
    dnx_data_mdb_em_format_granularity_get_f format_granularity_get;
    
    dnx_data_mdb_em_max_nof_vmv_size_get_f max_nof_vmv_size_get;
    
    dnx_data_mdb_em_vmv_nof_values_get_f vmv_nof_values_get;
    
    dnx_data_mdb_em_nof_vmv_size_nof_bits_get_f nof_vmv_size_nof_bits_get;
    
    dnx_data_mdb_em_esem_nof_vmv_size_get_f esem_nof_vmv_size_get;
    
    dnx_data_mdb_em_glem_nof_vmv_size_get_f glem_nof_vmv_size_get;
    
    dnx_data_mdb_em_mact_nof_vmv_size_get_f mact_nof_vmv_size_get;
    
    dnx_data_mdb_em_mact_max_payload_size_get_f mact_max_payload_size_get;
    
    dnx_data_mdb_em_shift_vmv_max_size_get_f shift_vmv_max_size_get;
    
    dnx_data_mdb_em_shift_vmv_max_regs_per_table_get_f shift_vmv_max_regs_per_table_get;
    
    dnx_data_mdb_em_min_nof_app_id_bits_get_f min_nof_app_id_bits_get;
    
    dnx_data_mdb_em_max_nof_spn_sizes_get_f max_nof_spn_sizes_get;
    
    dnx_data_mdb_em_nof_lfsr_sizes_get_f nof_lfsr_sizes_get;
    
    dnx_data_mdb_em_nof_formats_get_f nof_formats_get;
    
    dnx_data_mdb_em_defragmentation_priority_supported_get_f defragmentation_priority_supported_get;
    
    dnx_data_mdb_em_disable_cuckoo_loop_detection_support_get_f disable_cuckoo_loop_detection_support_get;
    
    dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_get_f disable_cuckoo_hit_bit_sync_get;
    
    dnx_data_mdb_em_age_profile_per_ratio_support_get_f age_profile_per_ratio_support_get;
    
    dnx_data_mdb_em_flex_mag_supported_get_f flex_mag_supported_get;
    
    dnx_data_mdb_em_flex_fully_supported_get_f flex_fully_supported_get;
    
    dnx_data_mdb_em_step_table_max_size_get_f step_table_max_size_get;
    
    dnx_data_mdb_em_nof_encoding_values_get_f nof_encoding_values_get;
    
    dnx_data_mdb_em_nof_encoding_types_get_f nof_encoding_types_get;
    
    dnx_data_mdb_em_extra_vmv_shift_registers_get_f extra_vmv_shift_registers_get;
    
    dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_get_f scan_bank_participate_in_cuckoo_support_get;
    
    dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_get_f scan_bank_participate_in_cuckoo_partial_support_get;
    
    dnx_data_mdb_em_sbus_error_code_support_get_f sbus_error_code_support_get;
    
    dnx_data_mdb_em_nof_emp_tables_get_f nof_emp_tables_get;
    
    dnx_data_mdb_em_defrag_interrupt_support_get_f defrag_interrupt_support_get;
    
    dnx_data_mdb_em_glem_bypass_option_support_get_f glem_bypass_option_support_get;
    
    dnx_data_mdb_em_flush_max_supported_payload_get_f flush_max_supported_payload_get;
    
    dnx_data_mdb_em_flush_payload_max_bus_get_f flush_payload_max_bus_get;
    
    dnx_data_mdb_em_my_mac_ippa_em_dbal_id_get_f my_mac_ippa_em_dbal_id_get;
    
    dnx_data_mdb_em_my_mac_ippf_em_dbal_id_get_f my_mac_ippf_em_dbal_id_get;
    
    dnx_data_mdb_em_mact_dbal_id_get_f mact_dbal_id_get;
    
    dnx_data_mdb_em_em_utilization_api_db_translation_get_f em_utilization_api_db_translation_get;
    
    dnxc_data_table_info_get_f em_utilization_api_db_translation_info_get;
    
    dnx_data_mdb_em_spn_get_f spn_get;
    
    dnxc_data_table_info_get_f spn_info_get;
    
    dnx_data_mdb_em_lfsr_get_f lfsr_get;
    
    dnxc_data_table_info_get_f lfsr_info_get;
    
    dnx_data_mdb_em_compare_size_get_f compare_size_get;
    
    dnxc_data_table_info_get_f compare_size_info_get;
    
    dnx_data_mdb_em_emp_index_table_mapping_get_f emp_index_table_mapping_get;
    
    dnxc_data_table_info_get_f emp_index_table_mapping_info_get;
    
    dnx_data_mdb_em_em_aging_info_get_f em_aging_info_get;
    
    dnxc_data_table_info_get_f em_aging_info_info_get;
    
    dnx_data_mdb_em_em_aging_cfg_get_f em_aging_cfg_get;
    
    dnxc_data_table_info_get_f em_aging_cfg_info_get;
    
    dnx_data_mdb_em_em_info_get_f em_info_get;
    
    dnxc_data_table_info_get_f em_info_info_get;
    
    dnx_data_mdb_em_em_interrupt_get_f em_interrupt_get;
    
    dnxc_data_table_info_get_f em_interrupt_info_get;
    
    dnx_data_mdb_em_step_table_pdb_max_depth_get_f step_table_pdb_max_depth_get;
    
    dnxc_data_table_info_get_f step_table_pdb_max_depth_info_get;
    
    dnx_data_mdb_em_step_table_max_depth_possible_get_f step_table_max_depth_possible_get;
    
    dnxc_data_table_info_get_f step_table_max_depth_possible_info_get;
    
    dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_get_f step_table_max_depth_nof_ways_ar_get;
    
    dnxc_data_table_info_get_f step_table_max_depth_nof_ways_ar_info_get;
    
    dnx_data_mdb_em_mdb_15_info_get_f mdb_15_info_get;
    
    dnxc_data_table_info_get_f mdb_15_info_info_get;
    
    dnx_data_mdb_em_mdb_16_info_get_f mdb_16_info_get;
    
    dnxc_data_table_info_get_f mdb_16_info_info_get;
    
    dnx_data_mdb_em_mdb_21_info_get_f mdb_21_info_get;
    
    dnxc_data_table_info_get_f mdb_21_info_info_get;
    
    dnx_data_mdb_em_mdb_23_info_get_f mdb_23_info_get;
    
    dnxc_data_table_info_get_f mdb_23_info_info_get;
    
    dnx_data_mdb_em_mdb_24_info_get_f mdb_24_info_get;
    
    dnxc_data_table_info_get_f mdb_24_info_info_get;
    
    dnx_data_mdb_em_mdb_29_info_get_f mdb_29_info_get;
    
    dnxc_data_table_info_get_f mdb_29_info_info_get;
    
    dnx_data_mdb_em_mdb_31_info_get_f mdb_31_info_get;
    
    dnxc_data_table_info_get_f mdb_31_info_info_get;
    
    dnx_data_mdb_em_mdb_32_info_get_f mdb_32_info_get;
    
    dnxc_data_table_info_get_f mdb_32_info_info_get;
    
    dnx_data_mdb_em_mdb_41_info_get_f mdb_41_info_get;
    
    dnxc_data_table_info_get_f mdb_41_info_info_get;
    
    dnx_data_mdb_em_mdb_45_info_get_f mdb_45_info_get;
    
    dnxc_data_table_info_get_f mdb_45_info_info_get;
    
    dnx_data_mdb_em_mdb_emp_tables_mapping_get_f mdb_emp_tables_mapping_get;
    
    dnxc_data_table_info_get_f mdb_emp_tables_mapping_info_get;
    
    dnx_data_mdb_em_mdb_em_shift_vmv_regs_get_f mdb_em_shift_vmv_regs_get;
    
    dnxc_data_table_info_get_f mdb_em_shift_vmv_regs_info_get;
    
    dnx_data_mdb_em_em_emp_get_f em_emp_get;
    
    dnxc_data_table_info_get_f em_emp_info_get;
    
    dnx_data_mdb_em_mdb_step_table_map_get_f mdb_step_table_map_get;
    
    dnxc_data_table_info_get_f mdb_step_table_map_info_get;
    
    dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_get_f mdb_em_dbal_phy_tables_info_get;
    
    dnxc_data_table_info_get_f mdb_em_dbal_phy_tables_info_info_get;
    
    dnx_data_mdb_em_filter_rules_get_f filter_rules_get;
    
    dnxc_data_table_info_get_f filter_rules_info_get;
    
    dnx_data_mdb_em_data_rules_get_f data_rules_get;
    
    dnxc_data_table_info_get_f data_rules_info_get;
} dnx_data_if_mdb_em_t;






typedef struct
{
    uint32 payload_type;
} dnx_data_mdb_direct_vsi_payload_type_per_range_t;


typedef struct
{
    uint8 dbal_id;
} dnx_data_mdb_direct_direct_utilization_api_db_translation_t;


typedef struct
{
    dbal_enum_value_field_direct_payload_sizes_e payload_type;
} dnx_data_mdb_direct_direct_info_t;


typedef struct
{
    uint8 hit_bit_position;
} dnx_data_mdb_direct_hit_bit_pos_in_abk_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_array_size;
    uint32 mdb_item_0_field_size;
    uint32 mdb_item_1_field_size;
    uint32 mdb_item_3_array_size;
    uint32 mdb_item_3_field_size;
    uint32 mdb_item_4_array_size;
    uint32 mdb_item_4_field_size;
} dnx_data_mdb_direct_mdb_11_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
    uint32 mdb_item_0_field_size;
    uint32 mdb_item_1_field_size;
} dnx_data_mdb_direct_mdb_26_info_t;


typedef struct
{
    dbal_tables_e mdb_12_table;
    dbal_tables_e mdb_42_table;
} dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t;



typedef enum
{
    dnx_data_mdb_direct_fec_abk_mapping,
    dnx_data_mdb_direct_fec_abk_macro_mapping,
    dnx_data_mdb_direct_vsi_config_size,
    dnx_data_mdb_direct_fec_address_remap,

    
    _dnx_data_mdb_direct_feature_nof
} dnx_data_mdb_direct_feature_e;



typedef int(
    *dnx_data_mdb_direct_feature_get_f) (
    int unit,
    dnx_data_mdb_direct_feature_e feature);


typedef uint32(
    *dnx_data_mdb_direct_physical_address_max_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_vsi_physical_address_max_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_fec_row_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_fec_address_mapping_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_fec_address_mapping_bits_relevant_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_fec_max_cluster_pairs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_max_payload_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_nof_fecs_in_super_fec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_physical_address_ovf_enable_val_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_nof_fec_dbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_map_payload_size_enum_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_map_payload_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_direct_fec_allocation_base_macro_type_get_f) (
    int unit);


typedef const dnx_data_mdb_direct_vsi_payload_type_per_range_t *(
    *dnx_data_mdb_direct_vsi_payload_type_per_range_get_f) (
    int unit,
    int range_idx);


typedef const dnx_data_mdb_direct_direct_utilization_api_db_translation_t *(
    *dnx_data_mdb_direct_direct_utilization_api_db_translation_get_f) (
    int unit,
    int resource_id);


typedef const dnx_data_mdb_direct_direct_info_t *(
    *dnx_data_mdb_direct_direct_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_direct_hit_bit_pos_in_abk_t *(
    *dnx_data_mdb_direct_hit_bit_pos_in_abk_get_f) (
    int unit,
    int hit_bit_index);


typedef const dnx_data_mdb_direct_mdb_11_info_t *(
    *dnx_data_mdb_direct_mdb_11_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_direct_mdb_26_info_t *(
    *dnx_data_mdb_direct_mdb_26_info_get_f) (
    int unit,
    int mdb_table_id);


typedef const dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t *(
    *dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_get_f) (
    int unit,
    int mdb_table_id);



typedef struct
{
    
    dnx_data_mdb_direct_feature_get_f feature_get;
    
    dnx_data_mdb_direct_physical_address_max_bits_get_f physical_address_max_bits_get;
    
    dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get_f physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get;
    
    dnx_data_mdb_direct_vsi_physical_address_max_bits_get_f vsi_physical_address_max_bits_get;
    
    dnx_data_mdb_direct_fec_row_width_get_f fec_row_width_get;
    
    dnx_data_mdb_direct_fec_address_mapping_bits_get_f fec_address_mapping_bits_get;
    
    dnx_data_mdb_direct_fec_address_mapping_bits_relevant_get_f fec_address_mapping_bits_relevant_get;
    
    dnx_data_mdb_direct_fec_max_cluster_pairs_get_f fec_max_cluster_pairs_get;
    
    dnx_data_mdb_direct_max_payload_size_get_f max_payload_size_get;
    
    dnx_data_mdb_direct_nof_fecs_in_super_fec_get_f nof_fecs_in_super_fec_get;
    
    dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_get_f nof_fec_ecmp_hierarchies_get;
    
    dnx_data_mdb_direct_physical_address_ovf_enable_val_get_f physical_address_ovf_enable_val_get;
    
    dnx_data_mdb_direct_nof_fec_dbs_get_f nof_fec_dbs_get;
    
    dnx_data_mdb_direct_map_payload_size_enum_get_f map_payload_size_enum_get;
    
    dnx_data_mdb_direct_map_payload_size_get_f map_payload_size_get;
    
    dnx_data_mdb_direct_fec_allocation_base_macro_type_get_f fec_allocation_base_macro_type_get;
    
    dnx_data_mdb_direct_vsi_payload_type_per_range_get_f vsi_payload_type_per_range_get;
    
    dnxc_data_table_info_get_f vsi_payload_type_per_range_info_get;
    
    dnx_data_mdb_direct_direct_utilization_api_db_translation_get_f direct_utilization_api_db_translation_get;
    
    dnxc_data_table_info_get_f direct_utilization_api_db_translation_info_get;
    
    dnx_data_mdb_direct_direct_info_get_f direct_info_get;
    
    dnxc_data_table_info_get_f direct_info_info_get;
    
    dnx_data_mdb_direct_hit_bit_pos_in_abk_get_f hit_bit_pos_in_abk_get;
    
    dnxc_data_table_info_get_f hit_bit_pos_in_abk_info_get;
    
    dnx_data_mdb_direct_mdb_11_info_get_f mdb_11_info_get;
    
    dnxc_data_table_info_get_f mdb_11_info_info_get;
    
    dnx_data_mdb_direct_mdb_26_info_get_f mdb_26_info_get;
    
    dnxc_data_table_info_get_f mdb_26_info_info_get;
    
    dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_get_f mdb_direct_mdb_phy_tables_info_get;
    
    dnxc_data_table_info_get_f mdb_direct_mdb_phy_tables_info_info_get;
} dnx_data_if_mdb_direct_t;






typedef struct
{
    uint32 nof_available_banks;
} dnx_data_mdb_eedb_phase_info_t;


typedef struct
{
    dbal_tables_e bucket_enable_dbal_table;
} dnx_data_mdb_eedb_phase_ll_bucket_enable_t;


typedef struct
{
    mdb_physical_table_e mdb_physical_table_id;
} dnx_data_mdb_eedb_phase_to_table_t;


typedef struct
{
    uint32 data_granularity;
} dnx_data_mdb_eedb_outlif_physical_phase_granularity_t;



typedef enum
{
    dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear,

    
    _dnx_data_mdb_eedb_feature_nof
} dnx_data_mdb_eedb_feature_e;



typedef int(
    *dnx_data_mdb_eedb_feature_get_f) (
    int unit,
    dnx_data_mdb_eedb_feature_e feature);


typedef uint32(
    *dnx_data_mdb_eedb_phase_map_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_phase_map_array_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_nof_phase_per_mag_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_nof_phases_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_nof_eedb_mags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_nof_eedb_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_nof_phases_per_eedb_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_phase_bank_select_default_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_entry_format_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_entry_format_encoding_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_bank_id_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_entry_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_eedb_abk_bank_get_f) (
    int unit);


typedef const dnx_data_mdb_eedb_phase_info_t *(
    *dnx_data_mdb_eedb_phase_info_get_f) (
    int unit,
    int phase_number);


typedef const dnx_data_mdb_eedb_phase_ll_bucket_enable_t *(
    *dnx_data_mdb_eedb_phase_ll_bucket_enable_get_f) (
    int unit,
    int phase_number);


typedef const dnx_data_mdb_eedb_phase_to_table_t *(
    *dnx_data_mdb_eedb_phase_to_table_get_f) (
    int unit,
    int eedb_mem_type,
    int phase_number);


typedef const dnx_data_mdb_eedb_outlif_physical_phase_granularity_t *(
    *dnx_data_mdb_eedb_outlif_physical_phase_granularity_get_f) (
    int unit,
    int outlif_physical_phase);



typedef struct
{
    
    dnx_data_mdb_eedb_feature_get_f feature_get;
    
    dnx_data_mdb_eedb_phase_map_bits_get_f phase_map_bits_get;
    
    dnx_data_mdb_eedb_phase_map_array_size_get_f phase_map_array_size_get;
    
    dnx_data_mdb_eedb_nof_phase_per_mag_get_f nof_phase_per_mag_get;
    
    dnx_data_mdb_eedb_nof_phases_get_f nof_phases_get;
    
    dnx_data_mdb_eedb_nof_eedb_mags_get_f nof_eedb_mags_get;
    
    dnx_data_mdb_eedb_nof_eedb_banks_get_f nof_eedb_banks_get;
    
    dnx_data_mdb_eedb_nof_phases_per_eedb_bank_get_f nof_phases_per_eedb_bank_get;
    
    dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_get_f nof_phases_per_eedb_bank_size_get;
    
    dnx_data_mdb_eedb_phase_bank_select_default_get_f phase_bank_select_default_get;
    
    dnx_data_mdb_eedb_entry_format_bits_get_f entry_format_bits_get;
    
    dnx_data_mdb_eedb_entry_format_encoding_bits_get_f entry_format_encoding_bits_get;
    
    dnx_data_mdb_eedb_bank_id_bits_get_f bank_id_bits_get;
    
    dnx_data_mdb_eedb_entry_bank_get_f entry_bank_get;
    
    dnx_data_mdb_eedb_abk_bank_get_f abk_bank_get;
    
    dnx_data_mdb_eedb_phase_info_get_f phase_info_get;
    
    dnxc_data_table_info_get_f phase_info_info_get;
    
    dnx_data_mdb_eedb_phase_ll_bucket_enable_get_f phase_ll_bucket_enable_get;
    
    dnxc_data_table_info_get_f phase_ll_bucket_enable_info_get;
    
    dnx_data_mdb_eedb_phase_to_table_get_f phase_to_table_get;
    
    dnxc_data_table_info_get_f phase_to_table_info_get;
    
    dnx_data_mdb_eedb_outlif_physical_phase_granularity_get_f outlif_physical_phase_granularity_get;
    
    dnxc_data_table_info_get_f outlif_physical_phase_granularity_info_get;
} dnx_data_if_mdb_eedb_t;






typedef struct
{
    uint32 global_macro_index;
} dnx_data_mdb_kaps_big_bb_blk_id_mapping_t;


typedef struct
{
    dbal_physical_tables_e dbal_id;
    mdb_physical_table_e mdb_bb_id;
    mdb_physical_table_e mdb_ads_id;
    mdb_physical_table_e mdb_direct_id;
    char *name;
    dbal_tables_e dbal_table_db_atr;
    int nof_rows_in_rpb;
    int nof_rows_in_small_ads;
    int nof_rows_in_small_bb;
    int nof_rows_in_small_rpb_hitbits;
    int nof_connected_big_bbs;
    int nof_result_types;
} dnx_data_mdb_kaps_db_info_t;


typedef struct
{
    uint8 is_supported;
} dnx_data_mdb_kaps_db_supported_ad_sizes_t;


typedef struct
{
    uint32 rpb_global_cfg_reg;
    uint32 tcam_scan_cfg_reg;
    uint32 bb_global_cfg_reg;
    uint32 bb_global_cfg_rpb_sel_field;
    uint32 tcam_cmd_mem;
    uint32 small_ads_mem;
    uint32 small_bb_mem;
    uint32 search_cmd_mem;
} dnx_data_mdb_kaps_db_access_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
} dnx_data_mdb_kaps_mdb_28_info_t;


typedef struct
{
    dbal_tables_e dbal_table;
} dnx_data_mdb_kaps_mdb_53_info_t;


typedef struct
{
    uint32 lookup_result;
    uint32 payload_field;
    uint32 prefix_length_field;
    uint32 result_found_field;
    uint32 result_error_field;
    uint32 result_type_field;
} dnx_data_mdb_kaps_kaps_lookup_result_t;



typedef enum
{
    dnx_data_mdb_kaps_flex_bb_row,
    dnx_data_mdb_kaps_tcam_read_valid_bits_lsbits,
    dnx_data_mdb_kaps_hitbit_support,

    
    _dnx_data_mdb_kaps_feature_nof
} dnx_data_mdb_kaps_feature_e;



typedef int(
    *dnx_data_mdb_kaps_feature_get_f) (
    int unit,
    dnx_data_mdb_kaps_feature_e feature);


typedef uint32(
    *dnx_data_mdb_kaps_nof_dbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_small_bbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_rows_in_rpb_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_rows_in_small_ads_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_rows_in_small_bb_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_big_bbs_blk_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_key_lsn_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_key_prefix_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_max_prefix_in_bb_row_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_key_width_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_bb_byte_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_rpb_byte_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_ads_byte_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_direct_byte_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_rpb_valid_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_max_fmt_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_nof_app_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_max_nof_result_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_rpb_field_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_big_kaps_revision_val_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_small_kaps_ad_size_e_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mdb_kaps_mdb_lpm_blackhole_mode_enable_get_f) (
    int unit);


typedef const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t *(
    *dnx_data_mdb_kaps_big_bb_blk_id_mapping_get_f) (
    int unit,
    int pair_id,
    int blk_id);


typedef const dnx_data_mdb_kaps_db_info_t *(
    *dnx_data_mdb_kaps_db_info_get_f) (
    int unit,
    int db_idx);


typedef const dnx_data_mdb_kaps_db_supported_ad_sizes_t *(
    *dnx_data_mdb_kaps_db_supported_ad_sizes_get_f) (
    int unit,
    int db_idx,
    int ad_size);


typedef const dnx_data_mdb_kaps_db_access_info_t *(
    *dnx_data_mdb_kaps_db_access_info_get_f) (
    int unit,
    int db_idx);


typedef const dnx_data_mdb_kaps_mdb_28_info_t *(
    *dnx_data_mdb_kaps_mdb_28_info_get_f) (
    int unit,
    int mdb_table);


typedef const dnx_data_mdb_kaps_mdb_53_info_t *(
    *dnx_data_mdb_kaps_mdb_53_info_get_f) (
    int unit,
    int dbal_id);


typedef const dnx_data_mdb_kaps_kaps_lookup_result_t *(
    *dnx_data_mdb_kaps_kaps_lookup_result_get_f) (
    int unit,
    int db_idx,
    int core);



typedef struct
{
    
    dnx_data_mdb_kaps_feature_get_f feature_get;
    
    dnx_data_mdb_kaps_nof_dbs_get_f nof_dbs_get;
    
    dnx_data_mdb_kaps_nof_small_bbs_get_f nof_small_bbs_get;
    
    dnx_data_mdb_kaps_nof_rows_in_rpb_max_get_f nof_rows_in_rpb_max_get;
    
    dnx_data_mdb_kaps_nof_rows_in_small_ads_max_get_f nof_rows_in_small_ads_max_get;
    
    dnx_data_mdb_kaps_nof_rows_in_small_bb_max_get_f nof_rows_in_small_bb_max_get;
    
    dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_get_f nof_rows_in_small_rpb_hitbits_max_get;
    
    dnx_data_mdb_kaps_nof_big_bbs_blk_ids_get_f nof_big_bbs_blk_ids_get;
    
    dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_get_f nof_bytes_in_hitbit_row_get;
    
    dnx_data_mdb_kaps_key_lsn_bits_get_f key_lsn_bits_get;
    
    dnx_data_mdb_kaps_key_prefix_length_get_f key_prefix_length_get;
    
    dnx_data_mdb_kaps_max_prefix_in_bb_row_get_f max_prefix_in_bb_row_get;
    
    dnx_data_mdb_kaps_key_width_in_bits_get_f key_width_in_bits_get;
    
    dnx_data_mdb_kaps_bb_byte_width_get_f bb_byte_width_get;
    
    dnx_data_mdb_kaps_rpb_byte_width_get_f rpb_byte_width_get;
    
    dnx_data_mdb_kaps_ads_byte_width_get_f ads_byte_width_get;
    
    dnx_data_mdb_kaps_direct_byte_width_get_f direct_byte_width_get;
    
    dnx_data_mdb_kaps_rpb_valid_bits_get_f rpb_valid_bits_get;
    
    dnx_data_mdb_kaps_max_fmt_get_f max_fmt_get;
    
    dnx_data_mdb_kaps_nof_app_ids_get_f nof_app_ids_get;
    
    dnx_data_mdb_kaps_max_nof_result_types_get_f max_nof_result_types_get;
    
    dnx_data_mdb_kaps_rpb_field_get_f rpb_field_get;
    
    dnx_data_mdb_kaps_big_kaps_revision_val_get_f big_kaps_revision_val_get;
    
    dnx_data_mdb_kaps_small_kaps_ad_size_e_get_f small_kaps_ad_size_e_get;
    
    dnx_data_mdb_kaps_mdb_lpm_blackhole_mode_enable_get_f mdb_lpm_blackhole_mode_enable_get;
    
    dnx_data_mdb_kaps_big_bb_blk_id_mapping_get_f big_bb_blk_id_mapping_get;
    
    dnxc_data_table_info_get_f big_bb_blk_id_mapping_info_get;
    
    dnx_data_mdb_kaps_db_info_get_f db_info_get;
    
    dnxc_data_table_info_get_f db_info_info_get;
    
    dnx_data_mdb_kaps_db_supported_ad_sizes_get_f db_supported_ad_sizes_get;
    
    dnxc_data_table_info_get_f db_supported_ad_sizes_info_get;
    
    dnx_data_mdb_kaps_db_access_info_get_f db_access_info_get;
    
    dnxc_data_table_info_get_f db_access_info_info_get;
    
    dnx_data_mdb_kaps_mdb_28_info_get_f mdb_28_info_get;
    
    dnxc_data_table_info_get_f mdb_28_info_info_get;
    
    dnx_data_mdb_kaps_mdb_53_info_get_f mdb_53_info_get;
    
    dnxc_data_table_info_get_f mdb_53_info_info_get;
    
    dnx_data_mdb_kaps_kaps_lookup_result_get_f kaps_lookup_result_get;
    
    dnxc_data_table_info_get_f kaps_lookup_result_info_get;
} dnx_data_if_mdb_kaps_t;







typedef enum
{
    dnx_data_mdb_feature_step_table_supports_mix_ar_algo,
    dnx_data_mdb_feature_eedb_bank_traffic_lock,
    dnx_data_mdb_feature_em_dfg_ovf_cam_disabled,
    dnx_data_mdb_feature_em_mact_transplant_no_reply,
    dnx_data_mdb_feature_em_mact_use_refresh_on_insert,
    dnx_data_mdb_feature_em_mact_delete_transplant_no_reply,
    dnx_data_mdb_feature_em_mact_insert_flush_drop_on_exceed_limit,
    dnx_data_mdb_feature_em_sbus_interface_shutdown,
    dnx_data_mdb_feature_em_ecc_simple_command_deadlock,
    dnx_data_mdb_feature_em_compare_init,
    dnx_data_mdb_feature_bubble_pulse_width_too_short,
    dnx_data_mdb_feature_kaps_rpb_ecc_valid_shared,

    
    _dnx_data_mdb_feature_feature_nof
} dnx_data_mdb_feature_feature_e;



typedef int(
    *dnx_data_mdb_feature_feature_get_f) (
    int unit,
    dnx_data_mdb_feature_feature_e feature);



typedef struct
{
    
    dnx_data_mdb_feature_feature_get_f feature_get;
} dnx_data_if_mdb_feature_t;





typedef struct
{
    
    dnx_data_if_mdb_global_t global;
    
    dnx_data_if_mdb_dh_t dh;
    
    dnx_data_if_mdb_pdbs_t pdbs;
    
    dnx_data_if_mdb_em_t em;
    
    dnx_data_if_mdb_direct_t direct;
    
    dnx_data_if_mdb_eedb_t eedb;
    
    dnx_data_if_mdb_kaps_t kaps;
    
    dnx_data_if_mdb_feature_t feature;
} dnx_data_if_mdb_t;




extern dnx_data_if_mdb_t dnx_data_mdb;


#endif 

