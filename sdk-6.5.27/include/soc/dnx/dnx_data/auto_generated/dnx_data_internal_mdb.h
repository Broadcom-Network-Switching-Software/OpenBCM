
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MDB_H_

#define _DNX_DATA_INTERNAL_MDB_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_mdb_submodule_global,
    dnx_data_mdb_submodule_dh,
    dnx_data_mdb_submodule_pdbs,
    dnx_data_mdb_submodule_em,
    dnx_data_mdb_submodule_direct,
    dnx_data_mdb_submodule_eedb,
    dnx_data_mdb_submodule_kaps,
    dnx_data_mdb_submodule_feature,

    
    _dnx_data_mdb_submodule_nof
} dnx_data_mdb_submodule_e;








int dnx_data_mdb_global_feature_get(
    int unit,
    dnx_data_mdb_global_feature_e feature);



typedef enum
{
    dnx_data_mdb_global_define_utilization_api_nof_dbs,
    dnx_data_mdb_global_define_adapter_stub_enable,

    
    _dnx_data_mdb_global_define_nof
} dnx_data_mdb_global_define_e;



uint32 dnx_data_mdb_global_utilization_api_nof_dbs_get(
    int unit);


uint32 dnx_data_mdb_global_adapter_stub_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_mdb_global_table_nof
} dnx_data_mdb_global_table_e;









int dnx_data_mdb_dh_feature_get(
    int unit,
    dnx_data_mdb_dh_feature_e feature);



typedef enum
{
    dnx_data_mdb_dh_define_total_nof_macroes,
    dnx_data_mdb_dh_define_total_nof_macroes_plus_em_ovf_or_eedb_bank,
    dnx_data_mdb_dh_define_total_nof_macroes_plus_data_width,
    dnx_data_mdb_dh_define_max_nof_cluster_interfaces,
    dnx_data_mdb_dh_define_max_nof_clusters,
    dnx_data_mdb_dh_define_max_nof_db_clusters,
    dnx_data_mdb_dh_define_nof_bucket_clusters,
    dnx_data_mdb_dh_define_nof_buckets_in_macro,
    dnx_data_mdb_dh_define_nof_pair_clusters,
    dnx_data_mdb_dh_define_nof_data_rows_per_hitbit_row,
    dnx_data_mdb_dh_define_nof_ddha_blocks,
    dnx_data_mdb_dh_define_nof_ddhb_blocks,
    dnx_data_mdb_dh_define_nof_dhc_blocks,
    dnx_data_mdb_dh_define_nof_ddhc_blocks,
    dnx_data_mdb_dh_define_nof_ddhx_blocks,
    dnx_data_mdb_dh_define_cluster_row_width_bits,
    dnx_data_mdb_dh_define_cluster_row_width_uint32,
    dnx_data_mdb_dh_define_data_out_granularity,
    dnx_data_mdb_dh_define_bpu_setup_bb_connected,
    dnx_data_mdb_dh_define_bpu_setup_size_120_240_120,
    dnx_data_mdb_dh_define_bpu_setup_size_480_odd,
    dnx_data_mdb_dh_define_bpu_setup_size_480,
    dnx_data_mdb_dh_define_bpu_setup_size_360_120,
    dnx_data_mdb_dh_define_bpu_setup_size_120_360,
    dnx_data_mdb_dh_define_bpu_setup_size_240_240,
    dnx_data_mdb_dh_define_bpu_setup_size_120_120_240,
    dnx_data_mdb_dh_define_bpu_setup_size_240_120_120,
    dnx_data_mdb_dh_define_bpu_setup_size_120_120_120_120,
    dnx_data_mdb_dh_define_nof_fec_macro_types,

    
    _dnx_data_mdb_dh_define_nof
} dnx_data_mdb_dh_define_e;



uint32 dnx_data_mdb_dh_total_nof_macroes_get(
    int unit);


uint32 dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_get(
    int unit);


uint32 dnx_data_mdb_dh_total_nof_macroes_plus_data_width_get(
    int unit);


uint32 dnx_data_mdb_dh_max_nof_cluster_interfaces_get(
    int unit);


uint32 dnx_data_mdb_dh_max_nof_clusters_get(
    int unit);


uint32 dnx_data_mdb_dh_max_nof_db_clusters_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_bucket_clusters_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_buckets_in_macro_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_pair_clusters_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_ddha_blocks_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_ddhb_blocks_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_dhc_blocks_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_ddhc_blocks_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_ddhx_blocks_get(
    int unit);


uint32 dnx_data_mdb_dh_cluster_row_width_bits_get(
    int unit);


uint32 dnx_data_mdb_dh_cluster_row_width_uint32_get(
    int unit);


uint32 dnx_data_mdb_dh_data_out_granularity_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_bb_connected_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_120_240_120_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_480_odd_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_480_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_360_120_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_120_360_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_240_240_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_120_120_240_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_240_120_120_get(
    int unit);


uint32 dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_get(
    int unit);


uint32 dnx_data_mdb_dh_nof_fec_macro_types_get(
    int unit);



typedef enum
{
    dnx_data_mdb_dh_table_dh_info,
    dnx_data_mdb_dh_table_block_info,
    dnx_data_mdb_dh_table_logical_macro_info,
    dnx_data_mdb_dh_table_macro_type_info,
    dnx_data_mdb_dh_table_mdb_75_macro_halved,
    dnx_data_mdb_dh_table_entry_banks_info,
    dnx_data_mdb_dh_table_mdb_1_info,
    dnx_data_mdb_dh_table_mdb_2_info,
    dnx_data_mdb_dh_table_mdb_3_info,
    dnx_data_mdb_dh_table_mdb_fec_macro_types_info,
    dnx_data_mdb_dh_table_macro_global_indexes,

    
    _dnx_data_mdb_dh_table_nof
} dnx_data_mdb_dh_table_e;



const dnx_data_mdb_dh_dh_info_t * dnx_data_mdb_dh_dh_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_dh_block_info_t * dnx_data_mdb_dh_block_info_get(
    int unit,
    int global_macro_index);


const dnx_data_mdb_dh_logical_macro_info_t * dnx_data_mdb_dh_logical_macro_info_get(
    int unit,
    int global_macro_index);


const dnx_data_mdb_dh_macro_type_info_t * dnx_data_mdb_dh_macro_type_info_get(
    int unit,
    int macro_type);


const dnx_data_mdb_dh_mdb_75_macro_halved_t * dnx_data_mdb_dh_mdb_75_macro_halved_get(
    int unit,
    int macro_index);


const dnx_data_mdb_dh_entry_banks_info_t * dnx_data_mdb_dh_entry_banks_info_get(
    int unit,
    int global_macro_index);


const dnx_data_mdb_dh_mdb_1_info_t * dnx_data_mdb_dh_mdb_1_info_get(
    int unit,
    int global_macro_index);


const dnx_data_mdb_dh_mdb_2_info_t * dnx_data_mdb_dh_mdb_2_info_get(
    int unit,
    int global_macro_index);


const dnx_data_mdb_dh_mdb_3_info_t * dnx_data_mdb_dh_mdb_3_info_get(
    int unit,
    int global_macro_index);


const dnx_data_mdb_dh_mdb_fec_macro_types_info_t * dnx_data_mdb_dh_mdb_fec_macro_types_info_get(
    int unit);


const dnx_data_mdb_dh_macro_global_indexes_t * dnx_data_mdb_dh_macro_global_indexes_get(
    int unit);



shr_error_e dnx_data_mdb_dh_dh_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_block_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_logical_macro_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_macro_type_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_mdb_75_macro_halved_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_entry_banks_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_mdb_1_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_mdb_2_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_mdb_3_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_mdb_fec_macro_types_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_dh_macro_global_indexes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mdb_dh_dh_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_block_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_logical_macro_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_macro_type_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_mdb_75_macro_halved_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_entry_banks_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_mdb_1_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_mdb_2_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_mdb_3_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_mdb_fec_macro_types_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_dh_macro_global_indexes_info_get(
    int unit);






int dnx_data_mdb_pdbs_feature_get(
    int unit,
    dnx_data_mdb_pdbs_feature_e feature);



typedef enum
{
    dnx_data_mdb_pdbs_define_max_nof_interface_dhs,
    dnx_data_mdb_pdbs_define_mesh_mode_support,
    dnx_data_mdb_pdbs_define_max_key_size,
    dnx_data_mdb_pdbs_define_max_payload_size,
    dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size,
    dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size,
    dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size,
    dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size,
    dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_mdb_nof_profiles,
    dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_ddha_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_ddhab_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdba_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdbb_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_table_mdbk_dynamic_memory_access_memory_access_field_size,
    dnx_data_mdb_pdbs_define_adapter_mapping_crps_id,

    
    _dnx_data_mdb_pdbs_define_nof
} dnx_data_mdb_pdbs_define_e;



uint32 dnx_data_mdb_pdbs_max_nof_interface_dhs_get(
    int unit);


uint32 dnx_data_mdb_pdbs_mesh_mode_support_get(
    int unit);


uint32 dnx_data_mdb_pdbs_max_key_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_max_payload_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_mdb_nof_profiles_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_ddha_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_ddhab_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdba_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdbb_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_table_mdbk_dynamic_memory_access_memory_access_field_size_get(
    int unit);


uint32 dnx_data_mdb_pdbs_adapter_mapping_crps_id_get(
    int unit);



typedef enum
{
    dnx_data_mdb_pdbs_table_mdb_profile,
    dnx_data_mdb_pdbs_table_mdb_profiles_info,
    dnx_data_mdb_pdbs_table_mdb_profile_kaps_cfg,
    dnx_data_mdb_pdbs_table_mdb_profile_table_str_mapping,
    dnx_data_mdb_pdbs_table_dbal_vmv_str_mapping,
    dnx_data_mdb_pdbs_table_mdb_pdb_info,
    dnx_data_mdb_pdbs_table_dbal_pdb_info,
    dnx_data_mdb_pdbs_table_mdb_adapter_mapping,
    dnx_data_mdb_pdbs_table_fec_physical_dbs_alloc_info,
    dnx_data_mdb_pdbs_table_mdb_51_info,
    dnx_data_mdb_pdbs_table_mdb_53_info,

    
    _dnx_data_mdb_pdbs_table_nof
} dnx_data_mdb_pdbs_table_e;



const dnx_data_mdb_pdbs_mdb_profile_t * dnx_data_mdb_pdbs_mdb_profile_get(
    int unit);


const dnx_data_mdb_pdbs_mdb_profiles_info_t * dnx_data_mdb_pdbs_mdb_profiles_info_get(
    int unit,
    int mdb_profile_index);


const dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_t * dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_get(
    int unit);


const dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_t * dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_pdbs_dbal_vmv_str_mapping_t * dnx_data_mdb_pdbs_dbal_vmv_str_mapping_get(
    int unit,
    int dbal_table_id);


const dnx_data_mdb_pdbs_mdb_pdb_info_t * dnx_data_mdb_pdbs_mdb_pdb_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_pdbs_dbal_pdb_info_t * dnx_data_mdb_pdbs_dbal_pdb_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_pdbs_mdb_adapter_mapping_t * dnx_data_mdb_pdbs_mdb_adapter_mapping_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_t * dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_get(
    int unit,
    int fec_physical_db);


const dnx_data_mdb_pdbs_mdb_51_info_t * dnx_data_mdb_pdbs_mdb_51_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_pdbs_mdb_53_info_t * dnx_data_mdb_pdbs_mdb_53_info_get(
    int unit,
    int mdb_table);



shr_error_e dnx_data_mdb_pdbs_mdb_profile_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_profiles_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_dbal_vmv_str_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_pdb_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_dbal_pdb_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_adapter_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_51_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_pdbs_mdb_53_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_profile_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_profiles_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_dbal_vmv_str_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_pdb_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_dbal_pdb_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_adapter_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_51_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_pdbs_mdb_53_info_info_get(
    int unit);






int dnx_data_mdb_em_feature_get(
    int unit,
    dnx_data_mdb_em_feature_e feature);



typedef enum
{
    dnx_data_mdb_em_define_age_row_size_bits,
    dnx_data_mdb_em_define_age_ovfcam_row_size_bits,
    dnx_data_mdb_em_define_ovf_cam_max_size,
    dnx_data_mdb_em_define_age_support_per_entry_size_ratio,
    dnx_data_mdb_em_define_flush_tcam_rule_counter_support,
    dnx_data_mdb_em_define_nof_aging_profiles,
    dnx_data_mdb_em_define_aging_profiles_size_in_bits,
    dnx_data_mdb_em_define_max_tid_size,
    dnx_data_mdb_em_define_max_nof_tids,
    dnx_data_mdb_em_define_flush_support_tids,
    dnx_data_mdb_em_define_flush_max_supported_key,
    dnx_data_mdb_em_define_flush_max_supported_key_plus_payload,
    dnx_data_mdb_em_define_dh_120_entry_encoding_nof_bits,
    dnx_data_mdb_em_define_dh_240_entry_encoding_nof_bits,
    dnx_data_mdb_em_define_format_granularity,
    dnx_data_mdb_em_define_max_nof_vmv_size,
    dnx_data_mdb_em_define_vmv_nof_values,
    dnx_data_mdb_em_define_nof_vmv_size_nof_bits,
    dnx_data_mdb_em_define_esem_nof_vmv_size,
    dnx_data_mdb_em_define_glem_nof_vmv_size,
    dnx_data_mdb_em_define_mact_nof_vmv_size,
    dnx_data_mdb_em_define_mact_max_payload_size,
    dnx_data_mdb_em_define_shift_vmv_max_size,
    dnx_data_mdb_em_define_shift_vmv_max_regs_per_table,
    dnx_data_mdb_em_define_min_nof_app_id_bits,
    dnx_data_mdb_em_define_max_nof_spn_sizes,
    dnx_data_mdb_em_define_nof_lfsr_sizes,
    dnx_data_mdb_em_define_nof_formats,
    dnx_data_mdb_em_define_defragmentation_priority_supported,
    dnx_data_mdb_em_define_disable_cuckoo_loop_detection_support,
    dnx_data_mdb_em_define_disable_cuckoo_hit_bit_sync,
    dnx_data_mdb_em_define_age_profile_per_ratio_support,
    dnx_data_mdb_em_define_flex_mag_supported,
    dnx_data_mdb_em_define_flex_fully_supported,
    dnx_data_mdb_em_define_step_table_max_size,
    dnx_data_mdb_em_define_nof_encoding_values,
    dnx_data_mdb_em_define_nof_encoding_types,
    dnx_data_mdb_em_define_extra_vmv_shift_registers,
    dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support,
    dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_partial_support,
    dnx_data_mdb_em_define_sbus_error_code_support,
    dnx_data_mdb_em_define_nof_emp_tables,
    dnx_data_mdb_em_define_defrag_interrupt_support,
    dnx_data_mdb_em_define_glem_bypass_option_support,
    dnx_data_mdb_em_define_flush_max_supported_payload,
    dnx_data_mdb_em_define_flush_payload_max_bus,
    dnx_data_mdb_em_define_my_mac_ippa_em_dbal_id,
    dnx_data_mdb_em_define_my_mac_ippf_em_dbal_id,
    dnx_data_mdb_em_define_mact_dbal_id,

    
    _dnx_data_mdb_em_define_nof
} dnx_data_mdb_em_define_e;



uint32 dnx_data_mdb_em_age_row_size_bits_get(
    int unit);


uint32 dnx_data_mdb_em_age_ovfcam_row_size_bits_get(
    int unit);


uint32 dnx_data_mdb_em_ovf_cam_max_size_get(
    int unit);


uint32 dnx_data_mdb_em_age_support_per_entry_size_ratio_get(
    int unit);


uint32 dnx_data_mdb_em_flush_tcam_rule_counter_support_get(
    int unit);


uint32 dnx_data_mdb_em_nof_aging_profiles_get(
    int unit);


uint32 dnx_data_mdb_em_aging_profiles_size_in_bits_get(
    int unit);


uint32 dnx_data_mdb_em_max_tid_size_get(
    int unit);


uint32 dnx_data_mdb_em_max_nof_tids_get(
    int unit);


uint32 dnx_data_mdb_em_flush_support_tids_get(
    int unit);


uint32 dnx_data_mdb_em_flush_max_supported_key_get(
    int unit);


uint32 dnx_data_mdb_em_flush_max_supported_key_plus_payload_get(
    int unit);


uint32 dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_get(
    int unit);


uint32 dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_get(
    int unit);


uint32 dnx_data_mdb_em_format_granularity_get(
    int unit);


uint32 dnx_data_mdb_em_max_nof_vmv_size_get(
    int unit);


uint32 dnx_data_mdb_em_vmv_nof_values_get(
    int unit);


uint32 dnx_data_mdb_em_nof_vmv_size_nof_bits_get(
    int unit);


uint32 dnx_data_mdb_em_esem_nof_vmv_size_get(
    int unit);


uint32 dnx_data_mdb_em_glem_nof_vmv_size_get(
    int unit);


uint32 dnx_data_mdb_em_mact_nof_vmv_size_get(
    int unit);


uint32 dnx_data_mdb_em_mact_max_payload_size_get(
    int unit);


uint32 dnx_data_mdb_em_shift_vmv_max_size_get(
    int unit);


uint32 dnx_data_mdb_em_shift_vmv_max_regs_per_table_get(
    int unit);


uint32 dnx_data_mdb_em_min_nof_app_id_bits_get(
    int unit);


uint32 dnx_data_mdb_em_max_nof_spn_sizes_get(
    int unit);


uint32 dnx_data_mdb_em_nof_lfsr_sizes_get(
    int unit);


uint32 dnx_data_mdb_em_nof_formats_get(
    int unit);


uint32 dnx_data_mdb_em_defragmentation_priority_supported_get(
    int unit);


uint32 dnx_data_mdb_em_disable_cuckoo_loop_detection_support_get(
    int unit);


uint32 dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_get(
    int unit);


uint32 dnx_data_mdb_em_age_profile_per_ratio_support_get(
    int unit);


uint32 dnx_data_mdb_em_flex_mag_supported_get(
    int unit);


uint32 dnx_data_mdb_em_flex_fully_supported_get(
    int unit);


uint32 dnx_data_mdb_em_step_table_max_size_get(
    int unit);


uint32 dnx_data_mdb_em_nof_encoding_values_get(
    int unit);


uint32 dnx_data_mdb_em_nof_encoding_types_get(
    int unit);


uint32 dnx_data_mdb_em_extra_vmv_shift_registers_get(
    int unit);


uint32 dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_get(
    int unit);


uint32 dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_get(
    int unit);


uint32 dnx_data_mdb_em_sbus_error_code_support_get(
    int unit);


uint32 dnx_data_mdb_em_nof_emp_tables_get(
    int unit);


uint32 dnx_data_mdb_em_defrag_interrupt_support_get(
    int unit);


uint32 dnx_data_mdb_em_glem_bypass_option_support_get(
    int unit);


uint32 dnx_data_mdb_em_flush_max_supported_payload_get(
    int unit);


uint32 dnx_data_mdb_em_flush_payload_max_bus_get(
    int unit);


uint32 dnx_data_mdb_em_my_mac_ippa_em_dbal_id_get(
    int unit);


uint32 dnx_data_mdb_em_my_mac_ippf_em_dbal_id_get(
    int unit);


uint32 dnx_data_mdb_em_mact_dbal_id_get(
    int unit);



typedef enum
{
    dnx_data_mdb_em_table_em_utilization_api_db_translation,
    dnx_data_mdb_em_table_spn,
    dnx_data_mdb_em_table_lfsr,
    dnx_data_mdb_em_table_compare_size,
    dnx_data_mdb_em_table_emp_index_table_mapping,
    dnx_data_mdb_em_table_em_aging_info,
    dnx_data_mdb_em_table_em_aging_cfg,
    dnx_data_mdb_em_table_em_info,
    dnx_data_mdb_em_table_em_interrupt,
    dnx_data_mdb_em_table_step_table_pdb_max_depth,
    dnx_data_mdb_em_table_step_table_max_depth_possible,
    dnx_data_mdb_em_table_step_table_max_depth_nof_ways_ar,
    dnx_data_mdb_em_table_mdb_15_info,
    dnx_data_mdb_em_table_mdb_16_info,
    dnx_data_mdb_em_table_mdb_21_info,
    dnx_data_mdb_em_table_mdb_23_info,
    dnx_data_mdb_em_table_mdb_24_info,
    dnx_data_mdb_em_table_mdb_29_info,
    dnx_data_mdb_em_table_mdb_31_info,
    dnx_data_mdb_em_table_mdb_32_info,
    dnx_data_mdb_em_table_mdb_41_info,
    dnx_data_mdb_em_table_mdb_45_info,
    dnx_data_mdb_em_table_mdb_emp_tables_mapping,
    dnx_data_mdb_em_table_mdb_em_shift_vmv_regs,
    dnx_data_mdb_em_table_em_emp,
    dnx_data_mdb_em_table_mdb_step_table_map,
    dnx_data_mdb_em_table_mdb_em_dbal_phy_tables_info,
    dnx_data_mdb_em_table_filter_rules,
    dnx_data_mdb_em_table_data_rules,

    
    _dnx_data_mdb_em_table_nof
} dnx_data_mdb_em_table_e;



const dnx_data_mdb_em_em_utilization_api_db_translation_t * dnx_data_mdb_em_em_utilization_api_db_translation_get(
    int unit,
    int resource_id);


const dnx_data_mdb_em_spn_t * dnx_data_mdb_em_spn_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_lfsr_t * dnx_data_mdb_em_lfsr_get(
    int unit,
    int lfsr_index);


const dnx_data_mdb_em_compare_size_t * dnx_data_mdb_em_compare_size_get(
    int unit,
    int entry_size);


const dnx_data_mdb_em_emp_index_table_mapping_t * dnx_data_mdb_em_emp_index_table_mapping_get(
    int unit,
    int emp_table_idx);


const dnx_data_mdb_em_em_aging_info_t * dnx_data_mdb_em_em_aging_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_em_aging_cfg_t * dnx_data_mdb_em_em_aging_cfg_get(
    int unit,
    int profile,
    int dbal_id);


const dnx_data_mdb_em_em_info_t * dnx_data_mdb_em_em_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_em_interrupt_t * dnx_data_mdb_em_em_interrupt_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_step_table_pdb_max_depth_t * dnx_data_mdb_em_step_table_pdb_max_depth_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_step_table_max_depth_possible_t * dnx_data_mdb_em_step_table_max_depth_possible_get(
    int unit,
    int step_table_size_indication,
    int aspect_ratio_combination);


const dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_t * dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_get(
    int unit,
    int aspect_ratio,
    int nof_ways);


const dnx_data_mdb_em_mdb_15_info_t * dnx_data_mdb_em_mdb_15_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_mdb_16_info_t * dnx_data_mdb_em_mdb_16_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_em_mdb_21_info_t * dnx_data_mdb_em_mdb_21_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_em_mdb_23_info_t * dnx_data_mdb_em_mdb_23_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_em_mdb_24_info_t * dnx_data_mdb_em_mdb_24_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_em_mdb_29_info_t * dnx_data_mdb_em_mdb_29_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_em_mdb_31_info_t * dnx_data_mdb_em_mdb_31_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_mdb_32_info_t * dnx_data_mdb_em_mdb_32_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_mdb_41_info_t * dnx_data_mdb_em_mdb_41_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_mdb_45_info_t * dnx_data_mdb_em_mdb_45_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_mdb_emp_tables_mapping_t * dnx_data_mdb_em_mdb_emp_tables_mapping_get(
    int unit,
    int mdb_table);


const dnx_data_mdb_em_mdb_em_shift_vmv_regs_t * dnx_data_mdb_em_mdb_em_shift_vmv_regs_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_em_emp_t * dnx_data_mdb_em_em_emp_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_mdb_step_table_map_t * dnx_data_mdb_em_mdb_step_table_map_get(
    int unit,
    int mdb_table);


const dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_t * dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_filter_rules_t * dnx_data_mdb_em_filter_rules_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_em_data_rules_t * dnx_data_mdb_em_data_rules_get(
    int unit,
    int dbal_id);



shr_error_e dnx_data_mdb_em_em_utilization_api_db_translation_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_spn_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_lfsr_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_compare_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_emp_index_table_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_em_aging_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_em_aging_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_em_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_em_interrupt_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_step_table_pdb_max_depth_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_step_table_max_depth_possible_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_15_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_16_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_21_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_23_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_24_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_29_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_31_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_32_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_41_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_45_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_emp_tables_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_em_shift_vmv_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_em_emp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_step_table_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_filter_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_em_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mdb_em_em_utilization_api_db_translation_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_spn_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_lfsr_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_compare_size_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_emp_index_table_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_em_aging_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_em_aging_cfg_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_em_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_em_interrupt_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_step_table_pdb_max_depth_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_step_table_max_depth_possible_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_15_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_16_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_21_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_23_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_24_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_29_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_31_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_32_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_41_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_45_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_emp_tables_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_em_shift_vmv_regs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_em_emp_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_step_table_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_filter_rules_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_em_data_rules_info_get(
    int unit);






int dnx_data_mdb_direct_feature_get(
    int unit,
    dnx_data_mdb_direct_feature_e feature);



typedef enum
{
    dnx_data_mdb_direct_define_physical_address_max_bits,
    dnx_data_mdb_direct_define_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc,
    dnx_data_mdb_direct_define_vsi_physical_address_max_bits,
    dnx_data_mdb_direct_define_fec_row_width,
    dnx_data_mdb_direct_define_fec_address_mapping_bits,
    dnx_data_mdb_direct_define_fec_address_mapping_bits_relevant,
    dnx_data_mdb_direct_define_fec_max_cluster_pairs,
    dnx_data_mdb_direct_define_max_payload_size,
    dnx_data_mdb_direct_define_nof_fecs_in_super_fec,
    dnx_data_mdb_direct_define_nof_fec_ecmp_hierarchies,
    dnx_data_mdb_direct_define_physical_address_ovf_enable_val,
    dnx_data_mdb_direct_define_nof_fec_dbs,
    dnx_data_mdb_direct_define_map_payload_size_enum,
    dnx_data_mdb_direct_define_map_payload_size,
    dnx_data_mdb_direct_define_fec_allocation_base_macro_type,

    
    _dnx_data_mdb_direct_define_nof
} dnx_data_mdb_direct_define_e;



uint32 dnx_data_mdb_direct_physical_address_max_bits_get(
    int unit);


uint32 dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get(
    int unit);


uint32 dnx_data_mdb_direct_vsi_physical_address_max_bits_get(
    int unit);


uint32 dnx_data_mdb_direct_fec_row_width_get(
    int unit);


uint32 dnx_data_mdb_direct_fec_address_mapping_bits_get(
    int unit);


uint32 dnx_data_mdb_direct_fec_address_mapping_bits_relevant_get(
    int unit);


uint32 dnx_data_mdb_direct_fec_max_cluster_pairs_get(
    int unit);


uint32 dnx_data_mdb_direct_max_payload_size_get(
    int unit);


uint32 dnx_data_mdb_direct_nof_fecs_in_super_fec_get(
    int unit);


uint32 dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_get(
    int unit);


uint32 dnx_data_mdb_direct_physical_address_ovf_enable_val_get(
    int unit);


uint32 dnx_data_mdb_direct_nof_fec_dbs_get(
    int unit);


uint32 dnx_data_mdb_direct_map_payload_size_enum_get(
    int unit);


uint32 dnx_data_mdb_direct_map_payload_size_get(
    int unit);


uint32 dnx_data_mdb_direct_fec_allocation_base_macro_type_get(
    int unit);



typedef enum
{
    dnx_data_mdb_direct_table_vsi_payload_type_per_range,
    dnx_data_mdb_direct_table_direct_utilization_api_db_translation,
    dnx_data_mdb_direct_table_direct_info,
    dnx_data_mdb_direct_table_hit_bit_pos_in_abk,
    dnx_data_mdb_direct_table_mdb_11_info,
    dnx_data_mdb_direct_table_mdb_26_info,
    dnx_data_mdb_direct_table_mdb_direct_mdb_phy_tables_info,

    
    _dnx_data_mdb_direct_table_nof
} dnx_data_mdb_direct_table_e;



const dnx_data_mdb_direct_vsi_payload_type_per_range_t * dnx_data_mdb_direct_vsi_payload_type_per_range_get(
    int unit,
    int range_idx);


const dnx_data_mdb_direct_direct_utilization_api_db_translation_t * dnx_data_mdb_direct_direct_utilization_api_db_translation_get(
    int unit,
    int resource_id);


const dnx_data_mdb_direct_direct_info_t * dnx_data_mdb_direct_direct_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_direct_hit_bit_pos_in_abk_t * dnx_data_mdb_direct_hit_bit_pos_in_abk_get(
    int unit,
    int hit_bit_index);


const dnx_data_mdb_direct_mdb_11_info_t * dnx_data_mdb_direct_mdb_11_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_direct_mdb_26_info_t * dnx_data_mdb_direct_mdb_26_info_get(
    int unit,
    int mdb_table_id);


const dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_t * dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_get(
    int unit,
    int mdb_table_id);



shr_error_e dnx_data_mdb_direct_vsi_payload_type_per_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_direct_direct_utilization_api_db_translation_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_direct_direct_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_direct_hit_bit_pos_in_abk_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_direct_mdb_11_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_direct_mdb_26_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mdb_direct_vsi_payload_type_per_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_direct_direct_utilization_api_db_translation_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_direct_direct_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_direct_hit_bit_pos_in_abk_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_direct_mdb_11_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_direct_mdb_26_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_info_get(
    int unit);






int dnx_data_mdb_eedb_feature_get(
    int unit,
    dnx_data_mdb_eedb_feature_e feature);



typedef enum
{
    dnx_data_mdb_eedb_define_phase_map_bits,
    dnx_data_mdb_eedb_define_phase_map_array_size,
    dnx_data_mdb_eedb_define_nof_phase_per_mag,
    dnx_data_mdb_eedb_define_nof_phases,
    dnx_data_mdb_eedb_define_nof_eedb_mags,
    dnx_data_mdb_eedb_define_nof_eedb_banks,
    dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank,
    dnx_data_mdb_eedb_define_nof_phases_per_eedb_bank_size,
    dnx_data_mdb_eedb_define_phase_bank_select_default,
    dnx_data_mdb_eedb_define_entry_format_bits,
    dnx_data_mdb_eedb_define_entry_format_encoding_bits,
    dnx_data_mdb_eedb_define_bank_id_bits,
    dnx_data_mdb_eedb_define_entry_bank,
    dnx_data_mdb_eedb_define_abk_bank,

    
    _dnx_data_mdb_eedb_define_nof
} dnx_data_mdb_eedb_define_e;



uint32 dnx_data_mdb_eedb_phase_map_bits_get(
    int unit);


uint32 dnx_data_mdb_eedb_phase_map_array_size_get(
    int unit);


uint32 dnx_data_mdb_eedb_nof_phase_per_mag_get(
    int unit);


uint32 dnx_data_mdb_eedb_nof_phases_get(
    int unit);


uint32 dnx_data_mdb_eedb_nof_eedb_mags_get(
    int unit);


uint32 dnx_data_mdb_eedb_nof_eedb_banks_get(
    int unit);


uint32 dnx_data_mdb_eedb_nof_phases_per_eedb_bank_get(
    int unit);


uint32 dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_get(
    int unit);


uint32 dnx_data_mdb_eedb_phase_bank_select_default_get(
    int unit);


uint32 dnx_data_mdb_eedb_entry_format_bits_get(
    int unit);


uint32 dnx_data_mdb_eedb_entry_format_encoding_bits_get(
    int unit);


uint32 dnx_data_mdb_eedb_bank_id_bits_get(
    int unit);


uint32 dnx_data_mdb_eedb_entry_bank_get(
    int unit);


uint32 dnx_data_mdb_eedb_abk_bank_get(
    int unit);



typedef enum
{
    dnx_data_mdb_eedb_table_phase_info,
    dnx_data_mdb_eedb_table_phase_ll_bucket_enable,
    dnx_data_mdb_eedb_table_phase_to_table,
    dnx_data_mdb_eedb_table_outlif_physical_phase_granularity,

    
    _dnx_data_mdb_eedb_table_nof
} dnx_data_mdb_eedb_table_e;



const dnx_data_mdb_eedb_phase_info_t * dnx_data_mdb_eedb_phase_info_get(
    int unit,
    int phase_number);


const dnx_data_mdb_eedb_phase_ll_bucket_enable_t * dnx_data_mdb_eedb_phase_ll_bucket_enable_get(
    int unit,
    int phase_number);


const dnx_data_mdb_eedb_phase_to_table_t * dnx_data_mdb_eedb_phase_to_table_get(
    int unit,
    int eedb_mem_type,
    int phase_number);


const dnx_data_mdb_eedb_outlif_physical_phase_granularity_t * dnx_data_mdb_eedb_outlif_physical_phase_granularity_get(
    int unit,
    int outlif_physical_phase);



shr_error_e dnx_data_mdb_eedb_phase_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_eedb_phase_ll_bucket_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_eedb_phase_to_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_eedb_outlif_physical_phase_granularity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mdb_eedb_phase_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_eedb_phase_ll_bucket_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_eedb_phase_to_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_eedb_outlif_physical_phase_granularity_info_get(
    int unit);






int dnx_data_mdb_kaps_feature_get(
    int unit,
    dnx_data_mdb_kaps_feature_e feature);



typedef enum
{
    dnx_data_mdb_kaps_define_nof_dbs,
    dnx_data_mdb_kaps_define_nof_small_bbs,
    dnx_data_mdb_kaps_define_nof_rows_in_rpb_max,
    dnx_data_mdb_kaps_define_nof_rows_in_small_ads_max,
    dnx_data_mdb_kaps_define_nof_rows_in_small_bb_max,
    dnx_data_mdb_kaps_define_nof_rows_in_small_rpb_hitbits_max,
    dnx_data_mdb_kaps_define_nof_big_bbs_blk_ids,
    dnx_data_mdb_kaps_define_nof_bytes_in_hitbit_row,
    dnx_data_mdb_kaps_define_key_lsn_bits,
    dnx_data_mdb_kaps_define_key_prefix_length,
    dnx_data_mdb_kaps_define_max_prefix_in_bb_row,
    dnx_data_mdb_kaps_define_key_width_in_bits,
    dnx_data_mdb_kaps_define_bb_byte_width,
    dnx_data_mdb_kaps_define_rpb_byte_width,
    dnx_data_mdb_kaps_define_ads_byte_width,
    dnx_data_mdb_kaps_define_direct_byte_width,
    dnx_data_mdb_kaps_define_rpb_valid_bits,
    dnx_data_mdb_kaps_define_max_fmt,
    dnx_data_mdb_kaps_define_nof_app_ids,
    dnx_data_mdb_kaps_define_max_nof_result_types,
    dnx_data_mdb_kaps_define_rpb_field,
    dnx_data_mdb_kaps_define_big_kaps_revision_val,
    dnx_data_mdb_kaps_define_small_kaps_ad_size_e,
    dnx_data_mdb_kaps_define_mdb_lpm_blackhole_mode_enable,

    
    _dnx_data_mdb_kaps_define_nof
} dnx_data_mdb_kaps_define_e;



uint32 dnx_data_mdb_kaps_nof_dbs_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_small_bbs_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_rows_in_rpb_max_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_rows_in_small_ads_max_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_rows_in_small_bb_max_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_big_bbs_blk_ids_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_get(
    int unit);


uint32 dnx_data_mdb_kaps_key_lsn_bits_get(
    int unit);


uint32 dnx_data_mdb_kaps_key_prefix_length_get(
    int unit);


uint32 dnx_data_mdb_kaps_max_prefix_in_bb_row_get(
    int unit);


uint32 dnx_data_mdb_kaps_key_width_in_bits_get(
    int unit);


uint32 dnx_data_mdb_kaps_bb_byte_width_get(
    int unit);


uint32 dnx_data_mdb_kaps_rpb_byte_width_get(
    int unit);


uint32 dnx_data_mdb_kaps_ads_byte_width_get(
    int unit);


uint32 dnx_data_mdb_kaps_direct_byte_width_get(
    int unit);


uint32 dnx_data_mdb_kaps_rpb_valid_bits_get(
    int unit);


uint32 dnx_data_mdb_kaps_max_fmt_get(
    int unit);


uint32 dnx_data_mdb_kaps_nof_app_ids_get(
    int unit);


uint32 dnx_data_mdb_kaps_max_nof_result_types_get(
    int unit);


uint32 dnx_data_mdb_kaps_rpb_field_get(
    int unit);


uint32 dnx_data_mdb_kaps_big_kaps_revision_val_get(
    int unit);


uint32 dnx_data_mdb_kaps_small_kaps_ad_size_e_get(
    int unit);


uint32 dnx_data_mdb_kaps_mdb_lpm_blackhole_mode_enable_get(
    int unit);



typedef enum
{
    dnx_data_mdb_kaps_table_big_bb_blk_id_mapping,
    dnx_data_mdb_kaps_table_db_info,
    dnx_data_mdb_kaps_table_db_supported_ad_sizes,
    dnx_data_mdb_kaps_table_db_access_info,
    dnx_data_mdb_kaps_table_mdb_28_info,
    dnx_data_mdb_kaps_table_mdb_53_info,
    dnx_data_mdb_kaps_table_kaps_lookup_result,

    
    _dnx_data_mdb_kaps_table_nof
} dnx_data_mdb_kaps_table_e;



const dnx_data_mdb_kaps_big_bb_blk_id_mapping_t * dnx_data_mdb_kaps_big_bb_blk_id_mapping_get(
    int unit,
    int pair_id,
    int blk_id);


const dnx_data_mdb_kaps_db_info_t * dnx_data_mdb_kaps_db_info_get(
    int unit,
    int db_idx);


const dnx_data_mdb_kaps_db_supported_ad_sizes_t * dnx_data_mdb_kaps_db_supported_ad_sizes_get(
    int unit,
    int db_idx,
    int ad_size);


const dnx_data_mdb_kaps_db_access_info_t * dnx_data_mdb_kaps_db_access_info_get(
    int unit,
    int db_idx);


const dnx_data_mdb_kaps_mdb_28_info_t * dnx_data_mdb_kaps_mdb_28_info_get(
    int unit,
    int mdb_table);


const dnx_data_mdb_kaps_mdb_53_info_t * dnx_data_mdb_kaps_mdb_53_info_get(
    int unit,
    int dbal_id);


const dnx_data_mdb_kaps_kaps_lookup_result_t * dnx_data_mdb_kaps_kaps_lookup_result_get(
    int unit,
    int db_idx,
    int core);



shr_error_e dnx_data_mdb_kaps_big_bb_blk_id_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_kaps_db_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_kaps_db_supported_ad_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_kaps_db_access_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_kaps_mdb_28_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_kaps_mdb_53_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mdb_kaps_kaps_lookup_result_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mdb_kaps_big_bb_blk_id_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_kaps_db_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_kaps_db_supported_ad_sizes_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_kaps_db_access_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_kaps_mdb_28_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_kaps_mdb_53_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mdb_kaps_kaps_lookup_result_info_get(
    int unit);






int dnx_data_mdb_feature_feature_get(
    int unit,
    dnx_data_mdb_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_mdb_feature_define_nof
} dnx_data_mdb_feature_define_e;




typedef enum
{

    
    _dnx_data_mdb_feature_table_nof
} dnx_data_mdb_feature_table_e;






shr_error_e dnx_data_mdb_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

