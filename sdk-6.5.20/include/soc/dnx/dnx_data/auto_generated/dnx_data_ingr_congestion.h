

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INGR_CONGESTION_H_

#define _DNX_DATA_INGR_CONGESTION_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingr_congestion.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_ingr_congestion_init(
    int unit);







typedef enum
{
    
    dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite,
    
    dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix,

    
    _dnx_data_ingr_congestion_config_feature_nof
} dnx_data_ingr_congestion_config_feature_e;



typedef int(
    *dnx_data_ingr_congestion_config_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_config_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_config_guarantee_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_config_wred_packet_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_congestion_config_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_config_guarantee_mode_get_f guarantee_mode_get;
    
    dnx_data_ingr_congestion_config_wred_packet_size_get_f wred_packet_size_get;
} dnx_data_if_ingr_congestion_config_t;






typedef struct
{
    
    uint32 max;
    
    uint32 hw_resolution_max;
    
    uint32 hw_resolution_nof_bits;
    
    int fadt_alpha_min;
    
    int fadt_alpha_max;
} dnx_data_ingr_congestion_info_resource_t;


typedef struct
{
    
    uint32 drop_precentage;
} dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t;


typedef struct
{
    
    uint32 is_guarantee_over_admit;
} dnx_data_ingr_congestion_info_admission_preferences_t;



typedef enum
{
    
    dnx_data_ingr_congestion_info_soft_dram_block_supported,
    
    dnx_data_ingr_congestion_info_latency_based_admission,
    
    dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable,
    
    dnx_data_ingr_congestion_info_cgm_hw_support,
    
    dnx_data_ingr_congestion_info_global_ecn_marking_supported,

    
    _dnx_data_ingr_congestion_info_feature_nof
} dnx_data_ingr_congestion_info_feature_e;



typedef int(
    *dnx_data_ingr_congestion_info_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_info_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_info_threshold_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_words_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_bytes_threshold_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_wred_max_gain_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_wred_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_nof_pds_in_pdb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_latency_based_admission_reject_flow_profile_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_latency_based_admission_max_supported_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_max_sram_pdbs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_info_max_dram_bdbs_get_f) (
    int unit);


typedef const dnx_data_ingr_congestion_info_resource_t *(
    *dnx_data_ingr_congestion_info_resource_get_f) (
    int unit,
    int type);


typedef const dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *(
    *dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_get_f) (
    int unit,
    int dp);


typedef const dnx_data_ingr_congestion_info_admission_preferences_t *(
    *dnx_data_ingr_congestion_info_admission_preferences_get_f) (
    int unit,
    int dp);



typedef struct
{
    
    dnx_data_ingr_congestion_info_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_info_threshold_granularity_get_f threshold_granularity_get;
    
    dnx_data_ingr_congestion_info_words_resolution_get_f words_resolution_get;
    
    dnx_data_ingr_congestion_info_bytes_threshold_granularity_get_f bytes_threshold_granularity_get;
    
    dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_get_f nof_dropped_reasons_cgm_get;
    
    dnx_data_ingr_congestion_info_wred_max_gain_get_f wred_max_gain_get;
    
    dnx_data_ingr_congestion_info_wred_granularity_get_f wred_granularity_get;
    
    dnx_data_ingr_congestion_info_nof_pds_in_pdb_get_f nof_pds_in_pdb_get;
    
    dnx_data_ingr_congestion_info_latency_based_admission_reject_flow_profile_value_get_f latency_based_admission_reject_flow_profile_value_get;
    
    dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable_value_get_f latency_based_admission_cgm_extended_profiles_enable_value_get;
    
    dnx_data_ingr_congestion_info_latency_based_admission_max_supported_profile_get_f latency_based_admission_max_supported_profile_get;
    
    dnx_data_ingr_congestion_info_max_sram_pdbs_get_f max_sram_pdbs_get;
    
    dnx_data_ingr_congestion_info_max_dram_bdbs_get_f max_dram_bdbs_get;
    
    dnx_data_ingr_congestion_info_resource_get_f resource_get;
    
    dnxc_data_table_info_get_f resource_info_get;
    
    dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_get_f dp_free_res_presentage_drop_get;
    
    dnxc_data_table_info_get_f dp_free_res_presentage_drop_info_get;
    
    dnx_data_ingr_congestion_info_admission_preferences_get_f admission_preferences_get;
    
    dnxc_data_table_info_get_f admission_preferences_info_get;
} dnx_data_if_ingr_congestion_info_t;







typedef enum
{

    
    _dnx_data_ingr_congestion_fadt_tail_drop_feature_nof
} dnx_data_ingr_congestion_fadt_tail_drop_feature_e;



typedef int(
    *dnx_data_ingr_congestion_fadt_tail_drop_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_fadt_tail_drop_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_congestion_fadt_tail_drop_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_get_f default_max_size_byte_threshold_for_ocb_only_get;
} dnx_data_if_ingr_congestion_fadt_tail_drop_t;






typedef struct
{
    
    uint32 max;
    
    int is_resource_range;
} dnx_data_ingr_congestion_dram_bound_resource_t;



typedef enum
{

    
    _dnx_data_ingr_congestion_dram_bound_feature_nof
} dnx_data_ingr_congestion_dram_bound_feature_e;



typedef int(
    *dnx_data_ingr_congestion_dram_bound_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_dram_bound_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_get_f) (
    int unit);


typedef const dnx_data_ingr_congestion_dram_bound_resource_t *(
    *dnx_data_ingr_congestion_dram_bound_resource_get_f) (
    int unit,
    int type);



typedef struct
{
    
    dnx_data_ingr_congestion_dram_bound_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_get_f fadt_alpha_min_get;
    
    dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_get_f fadt_alpha_max_get;
    
    dnx_data_ingr_congestion_dram_bound_resource_get_f resource_get;
    
    dnxc_data_table_info_get_f resource_info_get;
} dnx_data_if_ingr_congestion_dram_bound_t;







typedef enum
{

    
    _dnx_data_ingr_congestion_voq_feature_nof
} dnx_data_ingr_congestion_voq_feature_e;



typedef int(
    *dnx_data_ingr_congestion_voq_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_voq_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_voq_nof_rate_class_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_voq_rate_class_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_voq_nof_compensation_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_voq_default_compensation_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_voq_latency_bins_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_congestion_voq_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_voq_nof_rate_class_get_f nof_rate_class_get;
    
    dnx_data_ingr_congestion_voq_rate_class_nof_bits_get_f rate_class_nof_bits_get;
    
    dnx_data_ingr_congestion_voq_nof_compensation_profiles_get_f nof_compensation_profiles_get;
    
    dnx_data_ingr_congestion_voq_default_compensation_get_f default_compensation_get;
    
    dnx_data_ingr_congestion_voq_latency_bins_get_f latency_bins_get;
    
    dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_get_f voq_congestion_notification_fifo_size_get;
} dnx_data_if_ingr_congestion_voq_t;






typedef struct
{
    
    uint32 nof;
} dnx_data_ingr_congestion_vsq_info_t;



typedef enum
{
    
    dnx_data_ingr_congestion_vsq_size_watermark_support,

    
    _dnx_data_ingr_congestion_vsq_feature_nof
} dnx_data_ingr_congestion_vsq_feature_e;



typedef int(
    *dnx_data_ingr_congestion_vsq_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_vsq_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_a_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_b_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_c_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_d_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_e_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_f_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_non_nif_vsq_f_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_pool_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_connection_class_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_e_default_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_vsq_vsq_f_default_get_f) (
    int unit);


typedef const dnx_data_ingr_congestion_vsq_info_t *(
    *dnx_data_ingr_congestion_vsq_info_get_f) (
    int unit,
    int vsq_group);



typedef struct
{
    
    dnx_data_ingr_congestion_vsq_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_get_f vsq_rate_class_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_get_f vsq_a_rate_class_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_a_nof_get_f vsq_a_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_b_nof_get_f vsq_b_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_c_nof_get_f vsq_c_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_d_nof_get_f vsq_d_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_e_nof_get_f vsq_e_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_get_f vsq_e_hw_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_f_nof_get_f vsq_f_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_get_f vsq_f_hw_nof_get;
    
    dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_get_f nif_vsq_e_nof_get;
    
    dnx_data_ingr_congestion_vsq_non_nif_vsq_f_nof_get_f non_nif_vsq_f_nof_get;
    
    dnx_data_ingr_congestion_vsq_pool_nof_get_f pool_nof_get;
    
    dnx_data_ingr_congestion_vsq_connection_class_nof_get_f connection_class_nof_get;
    
    dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_get_f tc_pg_profile_nof_get;
    
    dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_get_f vsq_e_congestion_notification_fifo_size_get;
    
    dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_get_f vsq_f_congestion_notification_fifo_size_get;
    
    dnx_data_ingr_congestion_vsq_vsq_e_default_get_f vsq_e_default_get;
    
    dnx_data_ingr_congestion_vsq_vsq_f_default_get_f vsq_f_default_get;
    
    dnx_data_ingr_congestion_vsq_info_get_f info_get;
    
    dnxc_data_table_info_get_f info_info_get;
} dnx_data_if_ingr_congestion_vsq_t;






typedef struct
{
    
    uint32 is_total_shared_blocked;
    
    uint32 is_port_pg_shared_blocked;
    
    uint32 is_total_headroom_blocked;
    
    uint32 is_port_pg_headroom_blocked;
    
    uint32 is_voq_in_guaranteed;
    
    uint32 vsq_guaranteed_status;
} dnx_data_ingr_congestion_init_vsq_words_rjct_map_t;


typedef struct
{
    
    uint32 is_total_shared_blocked;
    
    uint32 is_port_pg_shared_blocked;
    
    uint32 is_headroom_extension_blocked;
    
    uint32 is_total_headroom_blocked;
    
    uint32 is_port_headroom_blocked;
    
    uint32 is_pg_headroom_blocked;
    
    uint32 is_voq_in_guaranteed;
    
    uint32 vsq_guaranteed_status;
} dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t;


typedef struct
{
    
    uint32 set_threshold;
    
    uint32 clear_threshold;
} dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t;


typedef struct
{
    
    uint32 set_threshold;
    
    uint32 clear_threshold;
} dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t;


typedef struct
{
    
    uint32 set_threshold;
    
    uint32 clear_threshold;
} dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t;


typedef struct
{
    
    uint32 drop;
} dnx_data_ingr_congestion_init_equivalent_global_drop_t;



typedef enum
{

    
    _dnx_data_ingr_congestion_init_feature_nof
} dnx_data_ingr_congestion_init_feature_e;



typedef int(
    *dnx_data_ingr_congestion_init_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_init_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_init_fifo_size_get_f) (
    int unit);


typedef const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *(
    *dnx_data_ingr_congestion_init_vsq_words_rjct_map_get_f) (
    int unit,
    int index);


typedef const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *(
    *dnx_data_ingr_congestion_init_vsq_sram_rjct_map_get_f) (
    int unit,
    int index);


typedef const dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *(
    *dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_get_f) (
    int unit,
    int dp);


typedef const dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *(
    *dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_get_f) (
    int unit,
    int dp);


typedef const dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *(
    *dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_get_f) (
    int unit,
    int dp);


typedef const dnx_data_ingr_congestion_init_equivalent_global_drop_t *(
    *dnx_data_ingr_congestion_init_equivalent_global_drop_get_f) (
    int unit,
    int type);



typedef struct
{
    
    dnx_data_ingr_congestion_init_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_init_fifo_size_get_f fifo_size_get;
    
    dnx_data_ingr_congestion_init_vsq_words_rjct_map_get_f vsq_words_rjct_map_get;
    
    dnxc_data_table_info_get_f vsq_words_rjct_map_info_get;
    
    dnx_data_ingr_congestion_init_vsq_sram_rjct_map_get_f vsq_sram_rjct_map_get;
    
    dnxc_data_table_info_get_f vsq_sram_rjct_map_info_get;
    
    dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_get_f dp_global_sram_buffer_drop_get;
    
    dnxc_data_table_info_get_f dp_global_sram_buffer_drop_info_get;
    
    dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_get_f dp_global_sram_pdb_drop_get;
    
    dnxc_data_table_info_get_f dp_global_sram_pdb_drop_info_get;
    
    dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_get_f dp_global_dram_bdb_drop_get;
    
    dnxc_data_table_info_get_f dp_global_dram_bdb_drop_info_get;
    
    dnx_data_ingr_congestion_init_equivalent_global_drop_get_f equivalent_global_drop_get;
    
    dnxc_data_table_info_get_f equivalent_global_drop_info_get;
} dnx_data_if_ingr_congestion_init_t;






typedef struct
{
    
    int index;
} dnx_data_ingr_congestion_dbal_admission_bits_mapping_t;



typedef enum
{

    
    _dnx_data_ingr_congestion_dbal_feature_nof
} dnx_data_ingr_congestion_dbal_feature_e;



typedef int(
    *dnx_data_ingr_congestion_dbal_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_dbal_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_admission_test_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_get_f) (
    int unit);


typedef const dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *(
    *dnx_data_ingr_congestion_dbal_admission_bits_mapping_get_f) (
    int unit,
    int rjct_bit);



typedef struct
{
    
    dnx_data_ingr_congestion_dbal_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_dbal_admission_test_nof_get_f admission_test_nof_get;
    
    dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_get_f dram_bdbs_th_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_get_f sram_pdbs_th_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_get_f sram_buffer_th_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_get_f sram_buffer_free_th_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_get_f sram_pds_th_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_get_f total_bytes_th_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_get_f dram_bdbs_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_get_f sram_pdbs_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_get_f sram_buffer_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_get_f sqm_debug_pkt_ctr_nof_bits_get;
    
    dnx_data_ingr_congestion_dbal_admission_bits_mapping_get_f admission_bits_mapping_get;
    
    dnxc_data_table_info_get_f admission_bits_mapping_info_get;
} dnx_data_if_ingr_congestion_dbal_t;







typedef enum
{
    
    dnx_data_ingr_congestion_mirror_on_drop_is_supported,

    
    _dnx_data_ingr_congestion_mirror_on_drop_feature_nof
} dnx_data_ingr_congestion_mirror_on_drop_feature_e;



typedef int(
    *dnx_data_ingr_congestion_mirror_on_drop_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_mirror_on_drop_feature_e feature);


typedef uint32(
    *dnx_data_ingr_congestion_mirror_on_drop_nof_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ingr_congestion_mirror_on_drop_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_mirror_on_drop_nof_groups_get_f nof_groups_get;
    
    dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_get_f aging_clocks_resolution_get;
} dnx_data_if_ingr_congestion_mirror_on_drop_t;






typedef struct
{
    
    uint32 value;
} dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t;



typedef enum
{

    
    _dnx_data_ingr_congestion_sram_buffer_feature_nof
} dnx_data_ingr_congestion_sram_buffer_feature_e;



typedef int(
    *dnx_data_ingr_congestion_sram_buffer_feature_get_f) (
    int unit,
    dnx_data_ingr_congestion_sram_buffer_feature_e feature);


typedef const dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *(
    *dnx_data_ingr_congestion_sram_buffer_drop_tresholds_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_ingr_congestion_sram_buffer_feature_get_f feature_get;
    
    dnx_data_ingr_congestion_sram_buffer_drop_tresholds_get_f drop_tresholds_get;
    
    dnxc_data_table_info_get_f drop_tresholds_info_get;
} dnx_data_if_ingr_congestion_sram_buffer_t;





typedef struct
{
    
    dnx_data_if_ingr_congestion_config_t config;
    
    dnx_data_if_ingr_congestion_info_t info;
    
    dnx_data_if_ingr_congestion_fadt_tail_drop_t fadt_tail_drop;
    
    dnx_data_if_ingr_congestion_dram_bound_t dram_bound;
    
    dnx_data_if_ingr_congestion_voq_t voq;
    
    dnx_data_if_ingr_congestion_vsq_t vsq;
    
    dnx_data_if_ingr_congestion_init_t init;
    
    dnx_data_if_ingr_congestion_dbal_t dbal;
    
    dnx_data_if_ingr_congestion_mirror_on_drop_t mirror_on_drop;
    
    dnx_data_if_ingr_congestion_sram_buffer_t sram_buffer;
} dnx_data_if_ingr_congestion_t;




extern dnx_data_if_ingr_congestion_t dnx_data_ingr_congestion;


#endif 

