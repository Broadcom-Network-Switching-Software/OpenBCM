

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_INGR_CONGESTION_H_

#define _DNX_DATA_INTERNAL_INGR_CONGESTION_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_ingr_congestion_submodule_config,
    dnx_data_ingr_congestion_submodule_info,
    dnx_data_ingr_congestion_submodule_fadt_tail_drop,
    dnx_data_ingr_congestion_submodule_dram_bound,
    dnx_data_ingr_congestion_submodule_voq,
    dnx_data_ingr_congestion_submodule_vsq,
    dnx_data_ingr_congestion_submodule_init,
    dnx_data_ingr_congestion_submodule_dbal,
    dnx_data_ingr_congestion_submodule_mirror_on_drop,
    dnx_data_ingr_congestion_submodule_sram_buffer,

    
    _dnx_data_ingr_congestion_submodule_nof
} dnx_data_ingr_congestion_submodule_e;








int dnx_data_ingr_congestion_config_feature_get(
    int unit,
    dnx_data_ingr_congestion_config_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_config_define_guarantee_mode,
    dnx_data_ingr_congestion_config_define_wred_packet_size,

    
    _dnx_data_ingr_congestion_config_define_nof
} dnx_data_ingr_congestion_config_define_e;



uint32 dnx_data_ingr_congestion_config_guarantee_mode_get(
    int unit);


uint32 dnx_data_ingr_congestion_config_wred_packet_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_congestion_config_table_nof
} dnx_data_ingr_congestion_config_table_e;









int dnx_data_ingr_congestion_info_feature_get(
    int unit,
    dnx_data_ingr_congestion_info_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_info_define_threshold_granularity,
    dnx_data_ingr_congestion_info_define_words_resolution,
    dnx_data_ingr_congestion_info_define_bytes_threshold_granularity,
    dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm,
    dnx_data_ingr_congestion_info_define_wred_max_gain,
    dnx_data_ingr_congestion_info_define_wred_granularity,
    dnx_data_ingr_congestion_info_define_nof_pds_in_pdb,
    dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value,
    dnx_data_ingr_congestion_info_define_latency_based_admission_cgm_extended_profiles_enable_value,
    dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile,
    dnx_data_ingr_congestion_info_define_max_sram_pdbs,
    dnx_data_ingr_congestion_info_define_max_dram_bdbs,

    
    _dnx_data_ingr_congestion_info_define_nof
} dnx_data_ingr_congestion_info_define_e;



uint32 dnx_data_ingr_congestion_info_threshold_granularity_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_words_resolution_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_bytes_threshold_granularity_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_wred_max_gain_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_wred_granularity_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_nof_pds_in_pdb_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_latency_based_admission_reject_flow_profile_value_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable_value_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_latency_based_admission_max_supported_profile_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_max_sram_pdbs_get(
    int unit);


uint32 dnx_data_ingr_congestion_info_max_dram_bdbs_get(
    int unit);



typedef enum
{
    dnx_data_ingr_congestion_info_table_resource,
    dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop,
    dnx_data_ingr_congestion_info_table_admission_preferences,

    
    _dnx_data_ingr_congestion_info_table_nof
} dnx_data_ingr_congestion_info_table_e;



const dnx_data_ingr_congestion_info_resource_t * dnx_data_ingr_congestion_info_resource_get(
    int unit,
    int type);


const dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t * dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_get(
    int unit,
    int dp);


const dnx_data_ingr_congestion_info_admission_preferences_t * dnx_data_ingr_congestion_info_admission_preferences_get(
    int unit,
    int dp);



shr_error_e dnx_data_ingr_congestion_info_resource_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_info_admission_preferences_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_congestion_info_resource_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_info_admission_preferences_info_get(
    int unit);






int dnx_data_ingr_congestion_fadt_tail_drop_feature_get(
    int unit,
    dnx_data_ingr_congestion_fadt_tail_drop_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only,

    
    _dnx_data_ingr_congestion_fadt_tail_drop_define_nof
} dnx_data_ingr_congestion_fadt_tail_drop_define_e;



uint32 dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_congestion_fadt_tail_drop_table_nof
} dnx_data_ingr_congestion_fadt_tail_drop_table_e;









int dnx_data_ingr_congestion_dram_bound_feature_get(
    int unit,
    dnx_data_ingr_congestion_dram_bound_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min,
    dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max,

    
    _dnx_data_ingr_congestion_dram_bound_define_nof
} dnx_data_ingr_congestion_dram_bound_define_e;



uint32 dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_get(
    int unit);


uint32 dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_get(
    int unit);



typedef enum
{
    dnx_data_ingr_congestion_dram_bound_table_resource,

    
    _dnx_data_ingr_congestion_dram_bound_table_nof
} dnx_data_ingr_congestion_dram_bound_table_e;



const dnx_data_ingr_congestion_dram_bound_resource_t * dnx_data_ingr_congestion_dram_bound_resource_get(
    int unit,
    int type);



shr_error_e dnx_data_ingr_congestion_dram_bound_resource_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_congestion_dram_bound_resource_info_get(
    int unit);






int dnx_data_ingr_congestion_voq_feature_get(
    int unit,
    dnx_data_ingr_congestion_voq_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_voq_define_nof_rate_class,
    dnx_data_ingr_congestion_voq_define_rate_class_nof_bits,
    dnx_data_ingr_congestion_voq_define_nof_compensation_profiles,
    dnx_data_ingr_congestion_voq_define_default_compensation,
    dnx_data_ingr_congestion_voq_define_latency_bins,
    dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size,

    
    _dnx_data_ingr_congestion_voq_define_nof
} dnx_data_ingr_congestion_voq_define_e;



uint32 dnx_data_ingr_congestion_voq_nof_rate_class_get(
    int unit);


uint32 dnx_data_ingr_congestion_voq_rate_class_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_voq_nof_compensation_profiles_get(
    int unit);


uint32 dnx_data_ingr_congestion_voq_default_compensation_get(
    int unit);


uint32 dnx_data_ingr_congestion_voq_latency_bins_get(
    int unit);


uint32 dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_congestion_voq_table_nof
} dnx_data_ingr_congestion_voq_table_e;









int dnx_data_ingr_congestion_vsq_feature_get(
    int unit,
    dnx_data_ingr_congestion_vsq_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_a_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_b_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_c_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_d_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_e_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_f_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof,
    dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof,
    dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof,
    dnx_data_ingr_congestion_vsq_define_pool_nof,
    dnx_data_ingr_congestion_vsq_define_connection_class_nof,
    dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof,
    dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size,
    dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size,
    dnx_data_ingr_congestion_vsq_define_vsq_e_default,
    dnx_data_ingr_congestion_vsq_define_vsq_f_default,

    
    _dnx_data_ingr_congestion_vsq_define_nof
} dnx_data_ingr_congestion_vsq_define_e;



uint32 dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_a_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_b_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_c_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_d_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_e_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_f_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_non_nif_vsq_f_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_pool_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_connection_class_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_e_default_get(
    int unit);


uint32 dnx_data_ingr_congestion_vsq_vsq_f_default_get(
    int unit);



typedef enum
{
    dnx_data_ingr_congestion_vsq_table_info,

    
    _dnx_data_ingr_congestion_vsq_table_nof
} dnx_data_ingr_congestion_vsq_table_e;



const dnx_data_ingr_congestion_vsq_info_t * dnx_data_ingr_congestion_vsq_info_get(
    int unit,
    int vsq_group);



shr_error_e dnx_data_ingr_congestion_vsq_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_congestion_vsq_info_info_get(
    int unit);






int dnx_data_ingr_congestion_init_feature_get(
    int unit,
    dnx_data_ingr_congestion_init_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_init_define_fifo_size,

    
    _dnx_data_ingr_congestion_init_define_nof
} dnx_data_ingr_congestion_init_define_e;



uint32 dnx_data_ingr_congestion_init_fifo_size_get(
    int unit);



typedef enum
{
    dnx_data_ingr_congestion_init_table_vsq_words_rjct_map,
    dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map,
    dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop,
    dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop,
    dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop,
    dnx_data_ingr_congestion_init_table_equivalent_global_drop,

    
    _dnx_data_ingr_congestion_init_table_nof
} dnx_data_ingr_congestion_init_table_e;



const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t * dnx_data_ingr_congestion_init_vsq_words_rjct_map_get(
    int unit,
    int index);


const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t * dnx_data_ingr_congestion_init_vsq_sram_rjct_map_get(
    int unit,
    int index);


const dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t * dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_get(
    int unit,
    int dp);


const dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t * dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_get(
    int unit,
    int dp);


const dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t * dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_get(
    int unit,
    int dp);


const dnx_data_ingr_congestion_init_equivalent_global_drop_t * dnx_data_ingr_congestion_init_equivalent_global_drop_get(
    int unit,
    int type);



shr_error_e dnx_data_ingr_congestion_init_vsq_words_rjct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_init_vsq_sram_rjct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ingr_congestion_init_equivalent_global_drop_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_congestion_init_vsq_words_rjct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_init_vsq_sram_rjct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ingr_congestion_init_equivalent_global_drop_info_get(
    int unit);






int dnx_data_ingr_congestion_dbal_feature_get(
    int unit,
    dnx_data_ingr_congestion_dbal_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_dbal_define_admission_test_nof,
    dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits,
    dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits,
    dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits,
    dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits,

    
    _dnx_data_ingr_congestion_dbal_define_nof
} dnx_data_ingr_congestion_dbal_define_e;



uint32 dnx_data_ingr_congestion_dbal_admission_test_nof_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_get(
    int unit);


uint32 dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_get(
    int unit);



typedef enum
{
    dnx_data_ingr_congestion_dbal_table_admission_bits_mapping,

    
    _dnx_data_ingr_congestion_dbal_table_nof
} dnx_data_ingr_congestion_dbal_table_e;



const dnx_data_ingr_congestion_dbal_admission_bits_mapping_t * dnx_data_ingr_congestion_dbal_admission_bits_mapping_get(
    int unit,
    int rjct_bit);



shr_error_e dnx_data_ingr_congestion_dbal_admission_bits_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_congestion_dbal_admission_bits_mapping_info_get(
    int unit);






int dnx_data_ingr_congestion_mirror_on_drop_feature_get(
    int unit,
    dnx_data_ingr_congestion_mirror_on_drop_feature_e feature);



typedef enum
{
    dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups,
    dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution,

    
    _dnx_data_ingr_congestion_mirror_on_drop_define_nof
} dnx_data_ingr_congestion_mirror_on_drop_define_e;



uint32 dnx_data_ingr_congestion_mirror_on_drop_nof_groups_get(
    int unit);


uint32 dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_congestion_mirror_on_drop_table_nof
} dnx_data_ingr_congestion_mirror_on_drop_table_e;









int dnx_data_ingr_congestion_sram_buffer_feature_get(
    int unit,
    dnx_data_ingr_congestion_sram_buffer_feature_e feature);



typedef enum
{

    
    _dnx_data_ingr_congestion_sram_buffer_define_nof
} dnx_data_ingr_congestion_sram_buffer_define_e;




typedef enum
{
    dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds,

    
    _dnx_data_ingr_congestion_sram_buffer_table_nof
} dnx_data_ingr_congestion_sram_buffer_table_e;



const dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t * dnx_data_ingr_congestion_sram_buffer_drop_tresholds_get(
    int unit,
    int index);



shr_error_e dnx_data_ingr_congestion_sram_buffer_drop_tresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_congestion_sram_buffer_drop_tresholds_info_get(
    int unit);



shr_error_e dnx_data_ingr_congestion_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

