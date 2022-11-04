
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_METER_H_

#define _DNX_DATA_INTERNAL_METER_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_meter_submodule_general,
    dnx_data_meter_submodule_profile,
    dnx_data_meter_submodule_diag,
    dnx_data_meter_submodule_meter_db,
    dnx_data_meter_submodule_mem_mgmt,
    dnx_data_meter_submodule_compensation,
    dnx_data_meter_submodule_expansion,

    
    _dnx_data_meter_submodule_nof
} dnx_data_meter_submodule_e;








int dnx_data_meter_general_feature_get(
    int unit,
    dnx_data_meter_general_feature_e feature);



typedef enum
{

    
    _dnx_data_meter_general_define_nof
} dnx_data_meter_general_define_e;




typedef enum
{

    
    _dnx_data_meter_general_table_nof
} dnx_data_meter_general_table_e;









int dnx_data_meter_profile_feature_get(
    int unit,
    dnx_data_meter_profile_feature_e feature);



typedef enum
{
    dnx_data_meter_profile_define_large_bucket_mode_exp_add,
    dnx_data_meter_profile_define_max_regular_bucket_mode_burst,
    dnx_data_meter_profile_define_rev_exp_min,
    dnx_data_meter_profile_define_rev_exp_max,
    dnx_data_meter_profile_define_burst_size_min,
    dnx_data_meter_profile_define_nof_valid_global_meters,
    dnx_data_meter_profile_define_nof_dp_command_max,
    dnx_data_meter_profile_define_global_engine_hw_stat_database_id,
    dnx_data_meter_profile_define_ir_mant_nof_bits,

    
    _dnx_data_meter_profile_define_nof
} dnx_data_meter_profile_define_e;



uint32 dnx_data_meter_profile_large_bucket_mode_exp_add_get(
    int unit);


uint32 dnx_data_meter_profile_max_regular_bucket_mode_burst_get(
    int unit);


uint32 dnx_data_meter_profile_rev_exp_min_get(
    int unit);


uint32 dnx_data_meter_profile_rev_exp_max_get(
    int unit);


uint32 dnx_data_meter_profile_burst_size_min_get(
    int unit);


uint32 dnx_data_meter_profile_nof_valid_global_meters_get(
    int unit);


uint32 dnx_data_meter_profile_nof_dp_command_max_get(
    int unit);


uint32 dnx_data_meter_profile_global_engine_hw_stat_database_id_get(
    int unit);


uint32 dnx_data_meter_profile_ir_mant_nof_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_meter_profile_table_nof
} dnx_data_meter_profile_table_e;









int dnx_data_meter_diag_feature_get(
    int unit,
    dnx_data_meter_diag_feature_e feature);



typedef enum
{

    
    _dnx_data_meter_diag_define_nof
} dnx_data_meter_diag_define_e;




typedef enum
{

    
    _dnx_data_meter_diag_table_nof
} dnx_data_meter_diag_table_e;









int dnx_data_meter_meter_db_feature_get(
    int unit,
    dnx_data_meter_meter_db_feature_e feature);



typedef enum
{
    dnx_data_meter_meter_db_define_nof_ingress_db,
    dnx_data_meter_meter_db_define_nof_egress_db,
    dnx_data_meter_meter_db_define_nof_meters_id_in_group,

    
    _dnx_data_meter_meter_db_define_nof
} dnx_data_meter_meter_db_define_e;



uint32 dnx_data_meter_meter_db_nof_ingress_db_get(
    int unit);


uint32 dnx_data_meter_meter_db_nof_egress_db_get(
    int unit);


uint32 dnx_data_meter_meter_db_nof_meters_id_in_group_get(
    int unit);



typedef enum
{

    
    _dnx_data_meter_meter_db_table_nof
} dnx_data_meter_meter_db_table_e;









int dnx_data_meter_mem_mgmt_feature_get(
    int unit,
    dnx_data_meter_mem_mgmt_feature_e feature);



typedef enum
{
    dnx_data_meter_mem_mgmt_define_adrress_range_resolution,
    dnx_data_meter_mem_mgmt_define_object_stat_pointer_base_resolution,
    dnx_data_meter_mem_mgmt_define_base_big_engine_for_meter,
    dnx_data_meter_mem_mgmt_define_base_mid_engine_for_meter,
    dnx_data_meter_mem_mgmt_define_ingress_single_bucket_engine,
    dnx_data_meter_mem_mgmt_define_egress_single_bucket_engine,
    dnx_data_meter_mem_mgmt_define_meter_pointer_size,
    dnx_data_meter_mem_mgmt_define_meter_pointer_map_size,
    dnx_data_meter_mem_mgmt_define_invalid_bank_id,
    dnx_data_meter_mem_mgmt_define_default_bank_id,
    dnx_data_meter_mem_mgmt_define_ptr_map_table_resolution,
    dnx_data_meter_mem_mgmt_define_bank_id_size,
    dnx_data_meter_mem_mgmt_define_nof_bank_ids,
    dnx_data_meter_mem_mgmt_define_sections_offset,
    dnx_data_meter_mem_mgmt_define_nof_banks_in_big_engine,
    dnx_data_meter_mem_mgmt_define_nof_single_buckets_per_entry,
    dnx_data_meter_mem_mgmt_define_nof_dual_buckets_per_entry,
    dnx_data_meter_mem_mgmt_define_nof_profiles_per_entry,
    dnx_data_meter_mem_mgmt_define_nof_buckets_in_small_bank,
    dnx_data_meter_mem_mgmt_define_nof_buckets_in_big_bank,
    dnx_data_meter_mem_mgmt_define_bucket_size_nof_bits,

    
    _dnx_data_meter_mem_mgmt_define_nof
} dnx_data_meter_mem_mgmt_define_e;



uint32 dnx_data_meter_mem_mgmt_adrress_range_resolution_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_base_big_engine_for_meter_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_base_mid_engine_for_meter_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_egress_single_bucket_engine_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_meter_pointer_size_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_meter_pointer_map_size_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_invalid_bank_id_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_default_bank_id_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_ptr_map_table_resolution_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_bank_id_size_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_bank_ids_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_sections_offset_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_banks_in_big_engine_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_single_buckets_per_entry_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_dual_buckets_per_entry_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_profiles_per_entry_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_buckets_in_small_bank_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_nof_buckets_in_big_bank_get(
    int unit);


uint32 dnx_data_meter_mem_mgmt_bucket_size_nof_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_meter_mem_mgmt_table_nof
} dnx_data_meter_mem_mgmt_table_e;









int dnx_data_meter_compensation_feature_get(
    int unit,
    dnx_data_meter_compensation_feature_e feature);



typedef enum
{
    dnx_data_meter_compensation_define_nof_delta_compensation_profiles,

    
    _dnx_data_meter_compensation_define_nof
} dnx_data_meter_compensation_define_e;



uint32 dnx_data_meter_compensation_nof_delta_compensation_profiles_get(
    int unit);



typedef enum
{
    dnx_data_meter_compensation_table_per_ingress_pp_port_delta_range,
    dnx_data_meter_compensation_table_per_meter_delta_range,

    
    _dnx_data_meter_compensation_table_nof
} dnx_data_meter_compensation_table_e;



const dnx_data_meter_compensation_per_ingress_pp_port_delta_range_t * dnx_data_meter_compensation_per_ingress_pp_port_delta_range_get(
    int unit);


const dnx_data_meter_compensation_per_meter_delta_range_t * dnx_data_meter_compensation_per_meter_delta_range_get(
    int unit);



shr_error_e dnx_data_meter_compensation_per_ingress_pp_port_delta_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_meter_compensation_per_meter_delta_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_meter_compensation_per_ingress_pp_port_delta_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_meter_compensation_per_meter_delta_range_info_get(
    int unit);






int dnx_data_meter_expansion_feature_get(
    int unit,
    dnx_data_meter_expansion_feature_e feature);



typedef enum
{
    dnx_data_meter_expansion_define_max_size,
    dnx_data_meter_expansion_define_nof_expansion_sets,
    dnx_data_meter_expansion_define_max_meter_set_size,
    dnx_data_meter_expansion_define_max_meter_set_base,
    dnx_data_meter_expansion_define_ingress_metadata_size,
    dnx_data_meter_expansion_define_egress_metadata_size,
    dnx_data_meter_expansion_define_expansion_size,
    dnx_data_meter_expansion_define_expansion_offset_size,

    
    _dnx_data_meter_expansion_define_nof
} dnx_data_meter_expansion_define_e;



uint32 dnx_data_meter_expansion_max_size_get(
    int unit);


uint32 dnx_data_meter_expansion_nof_expansion_sets_get(
    int unit);


uint32 dnx_data_meter_expansion_max_meter_set_size_get(
    int unit);


uint32 dnx_data_meter_expansion_max_meter_set_base_get(
    int unit);


uint32 dnx_data_meter_expansion_ingress_metadata_size_get(
    int unit);


uint32 dnx_data_meter_expansion_egress_metadata_size_get(
    int unit);


uint32 dnx_data_meter_expansion_expansion_size_get(
    int unit);


uint32 dnx_data_meter_expansion_expansion_offset_size_get(
    int unit);



typedef enum
{
    dnx_data_meter_expansion_table_source_metadata_types,

    
    _dnx_data_meter_expansion_table_nof
} dnx_data_meter_expansion_table_e;



const dnx_data_meter_expansion_source_metadata_types_t * dnx_data_meter_expansion_source_metadata_types_get(
    int unit,
    int metadata_type);



shr_error_e dnx_data_meter_expansion_source_metadata_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_meter_expansion_source_metadata_types_info_get(
    int unit);



shr_error_e dnx_data_meter_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

