
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_METER_H_

#define _DNX_DATA_METER_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/policer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_meter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_meter_init(
    int unit);







typedef enum
{
    dnx_data_meter_general_is_used,
    dnx_data_meter_general_power_down_supported,
    dnx_data_meter_general_egr_rslv_color_counter_support,

    
    _dnx_data_meter_general_feature_nof
} dnx_data_meter_general_feature_e;



typedef int(
    *dnx_data_meter_general_feature_get_f) (
    int unit,
    dnx_data_meter_general_feature_e feature);



typedef struct
{
    
    dnx_data_meter_general_feature_get_f feature_get;
} dnx_data_if_meter_general_t;







typedef enum
{

    
    _dnx_data_meter_profile_feature_nof
} dnx_data_meter_profile_feature_e;



typedef int(
    *dnx_data_meter_profile_feature_get_f) (
    int unit,
    dnx_data_meter_profile_feature_e feature);


typedef uint32(
    *dnx_data_meter_profile_large_bucket_mode_exp_add_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_max_regular_bucket_mode_burst_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_rev_exp_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_rev_exp_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_burst_size_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_nof_valid_global_meters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_nof_dp_command_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_global_engine_hw_stat_database_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_profile_ir_mant_nof_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_profile_feature_get_f feature_get;
    
    dnx_data_meter_profile_large_bucket_mode_exp_add_get_f large_bucket_mode_exp_add_get;
    
    dnx_data_meter_profile_max_regular_bucket_mode_burst_get_f max_regular_bucket_mode_burst_get;
    
    dnx_data_meter_profile_rev_exp_min_get_f rev_exp_min_get;
    
    dnx_data_meter_profile_rev_exp_max_get_f rev_exp_max_get;
    
    dnx_data_meter_profile_burst_size_min_get_f burst_size_min_get;
    
    dnx_data_meter_profile_nof_valid_global_meters_get_f nof_valid_global_meters_get;
    
    dnx_data_meter_profile_nof_dp_command_max_get_f nof_dp_command_max_get;
    
    dnx_data_meter_profile_global_engine_hw_stat_database_id_get_f global_engine_hw_stat_database_id_get;
    
    dnx_data_meter_profile_ir_mant_nof_bits_get_f ir_mant_nof_bits_get;
} dnx_data_if_meter_profile_t;







typedef enum
{
    dnx_data_meter_diag_rate_measurements_support,
    dnx_data_meter_diag_multi_dbs_rate_measurements_support,

    
    _dnx_data_meter_diag_feature_nof
} dnx_data_meter_diag_feature_e;



typedef int(
    *dnx_data_meter_diag_feature_get_f) (
    int unit,
    dnx_data_meter_diag_feature_e feature);



typedef struct
{
    
    dnx_data_meter_diag_feature_get_f feature_get;
} dnx_data_if_meter_diag_t;







typedef enum
{
    dnx_data_meter_meter_db_refresh_disable_required,
    dnx_data_meter_meter_db_refresh_range_minus_one,
    dnx_data_meter_meter_db_tcsm_support,
    dnx_data_meter_meter_db_resolve_table_mode_per_database,
    dnx_data_meter_meter_db_fixed_cmd_id,

    
    _dnx_data_meter_meter_db_feature_nof
} dnx_data_meter_meter_db_feature_e;



typedef int(
    *dnx_data_meter_meter_db_feature_get_f) (
    int unit,
    dnx_data_meter_meter_db_feature_e feature);


typedef uint32(
    *dnx_data_meter_meter_db_nof_ingress_db_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_meter_db_nof_egress_db_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_meter_db_nof_meters_id_in_group_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_meter_db_feature_get_f feature_get;
    
    dnx_data_meter_meter_db_nof_ingress_db_get_f nof_ingress_db_get;
    
    dnx_data_meter_meter_db_nof_egress_db_get_f nof_egress_db_get;
    
    dnx_data_meter_meter_db_nof_meters_id_in_group_get_f nof_meters_id_in_group_get;
} dnx_data_if_meter_meter_db_t;







typedef enum
{
    dnx_data_meter_mem_mgmt_is_ptr_map_to_bank_id_contains_bank_offset,
    dnx_data_meter_mem_mgmt_is_mid_engine_store_profiles,
    dnx_data_meter_mem_mgmt_is_small_engine_support_dual_bucket,
    dnx_data_meter_mem_mgmt_ptr_map_table_has_invalid_mapping_option,
    dnx_data_meter_mem_mgmt_fixed_nof_profile_in_entry,

    
    _dnx_data_meter_mem_mgmt_feature_nof
} dnx_data_meter_mem_mgmt_feature_e;



typedef int(
    *dnx_data_meter_mem_mgmt_feature_get_f) (
    int unit,
    dnx_data_meter_mem_mgmt_feature_e feature);


typedef uint32(
    *dnx_data_meter_mem_mgmt_adrress_range_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_base_big_engine_for_meter_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_base_mid_engine_for_meter_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_egress_single_bucket_engine_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_meter_pointer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_meter_pointer_map_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_invalid_bank_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_default_bank_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_ptr_map_table_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_bank_id_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_bank_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_sections_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_banks_in_big_engine_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_single_buckets_per_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_dual_buckets_per_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_profiles_per_entry_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_buckets_in_small_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_nof_buckets_in_big_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_bucket_size_nof_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_mem_mgmt_feature_get_f feature_get;
    
    dnx_data_meter_mem_mgmt_adrress_range_resolution_get_f adrress_range_resolution_get;
    
    dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_get_f object_stat_pointer_base_resolution_get;
    
    dnx_data_meter_mem_mgmt_base_big_engine_for_meter_get_f base_big_engine_for_meter_get;
    
    dnx_data_meter_mem_mgmt_base_mid_engine_for_meter_get_f base_mid_engine_for_meter_get;
    
    dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_get_f ingress_single_bucket_engine_get;
    
    dnx_data_meter_mem_mgmt_egress_single_bucket_engine_get_f egress_single_bucket_engine_get;
    
    dnx_data_meter_mem_mgmt_meter_pointer_size_get_f meter_pointer_size_get;
    
    dnx_data_meter_mem_mgmt_meter_pointer_map_size_get_f meter_pointer_map_size_get;
    
    dnx_data_meter_mem_mgmt_invalid_bank_id_get_f invalid_bank_id_get;
    
    dnx_data_meter_mem_mgmt_default_bank_id_get_f default_bank_id_get;
    
    dnx_data_meter_mem_mgmt_ptr_map_table_resolution_get_f ptr_map_table_resolution_get;
    
    dnx_data_meter_mem_mgmt_bank_id_size_get_f bank_id_size_get;
    
    dnx_data_meter_mem_mgmt_nof_bank_ids_get_f nof_bank_ids_get;
    
    dnx_data_meter_mem_mgmt_sections_offset_get_f sections_offset_get;
    
    dnx_data_meter_mem_mgmt_nof_banks_in_big_engine_get_f nof_banks_in_big_engine_get;
    
    dnx_data_meter_mem_mgmt_nof_single_buckets_per_entry_get_f nof_single_buckets_per_entry_get;
    
    dnx_data_meter_mem_mgmt_nof_dual_buckets_per_entry_get_f nof_dual_buckets_per_entry_get;
    
    dnx_data_meter_mem_mgmt_nof_profiles_per_entry_get_f nof_profiles_per_entry_get;
    
    dnx_data_meter_mem_mgmt_nof_buckets_in_small_bank_get_f nof_buckets_in_small_bank_get;
    
    dnx_data_meter_mem_mgmt_nof_buckets_in_big_bank_get_f nof_buckets_in_big_bank_get;
    
    dnx_data_meter_mem_mgmt_bucket_size_nof_bits_get_f bucket_size_nof_bits_get;
} dnx_data_if_meter_mem_mgmt_t;






typedef struct
{
    int min;
    int max;
} dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t;


typedef struct
{
    int min;
    int max;
} dnx_data_meter_compensation_per_meter_delta_range_t;



typedef enum
{
    dnx_data_meter_compensation_compensation_egress_support,

    
    _dnx_data_meter_compensation_feature_nof
} dnx_data_meter_compensation_feature_e;



typedef int(
    *dnx_data_meter_compensation_feature_get_f) (
    int unit,
    dnx_data_meter_compensation_feature_e feature);


typedef uint32(
    *dnx_data_meter_compensation_nof_delta_compensation_profiles_get_f) (
    int unit);


typedef const dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t *(
    *dnx_data_meter_compensation_per_ingress_pp_port_delta_range_get_f) (
    int unit);


typedef const dnx_data_meter_compensation_per_meter_delta_range_t *(
    *dnx_data_meter_compensation_per_meter_delta_range_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_compensation_feature_get_f feature_get;
    
    dnx_data_meter_compensation_nof_delta_compensation_profiles_get_f nof_delta_compensation_profiles_get;
    
    dnx_data_meter_compensation_per_ingress_pp_port_delta_range_get_f per_ingress_pp_port_delta_range_get;
    
    dnxc_data_table_info_get_f per_ingress_pp_port_delta_range_info_get;
    
    dnx_data_meter_compensation_per_meter_delta_range_get_f per_meter_delta_range_get;
    
    dnxc_data_table_info_get_f per_meter_delta_range_info_get;
} dnx_data_if_meter_compensation_t;






typedef struct
{
    int offset;
    int nof_bits;
} dnx_data_meter_expansion_source_metadata_types_t;



typedef enum
{
    dnx_data_meter_expansion_expansion_based_on_tc_support,
    dnx_data_meter_expansion_enhanced_expansion_mechanisim,
    dnx_data_meter_expansion_expansion_for_uuc_umc_support,

    
    _dnx_data_meter_expansion_feature_nof
} dnx_data_meter_expansion_feature_e;



typedef int(
    *dnx_data_meter_expansion_feature_get_f) (
    int unit,
    dnx_data_meter_expansion_feature_e feature);


typedef uint32(
    *dnx_data_meter_expansion_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_nof_expansion_sets_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_max_meter_set_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_max_meter_set_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_ingress_metadata_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_egress_metadata_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_expansion_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_expansion_expansion_offset_size_get_f) (
    int unit);


typedef const dnx_data_meter_expansion_source_metadata_types_t *(
    *dnx_data_meter_expansion_source_metadata_types_get_f) (
    int unit,
    int metadata_type);



typedef struct
{
    
    dnx_data_meter_expansion_feature_get_f feature_get;
    
    dnx_data_meter_expansion_max_size_get_f max_size_get;
    
    dnx_data_meter_expansion_nof_expansion_sets_get_f nof_expansion_sets_get;
    
    dnx_data_meter_expansion_max_meter_set_size_get_f max_meter_set_size_get;
    
    dnx_data_meter_expansion_max_meter_set_base_get_f max_meter_set_base_get;
    
    dnx_data_meter_expansion_ingress_metadata_size_get_f ingress_metadata_size_get;
    
    dnx_data_meter_expansion_egress_metadata_size_get_f egress_metadata_size_get;
    
    dnx_data_meter_expansion_expansion_size_get_f expansion_size_get;
    
    dnx_data_meter_expansion_expansion_offset_size_get_f expansion_offset_size_get;
    
    dnx_data_meter_expansion_source_metadata_types_get_f source_metadata_types_get;
    
    dnxc_data_table_info_get_f source_metadata_types_info_get;
} dnx_data_if_meter_expansion_t;





typedef struct
{
    
    dnx_data_if_meter_general_t general;
    
    dnx_data_if_meter_profile_t profile;
    
    dnx_data_if_meter_diag_t diag;
    
    dnx_data_if_meter_meter_db_t meter_db;
    
    dnx_data_if_meter_mem_mgmt_t mem_mgmt;
    
    dnx_data_if_meter_compensation_t compensation;
    
    dnx_data_if_meter_expansion_t expansion;
} dnx_data_if_meter_t;




extern dnx_data_if_meter_t dnx_data_meter;


#endif 

