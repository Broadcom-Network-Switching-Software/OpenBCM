

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_METER_H_

#define _DNX_DATA_METER_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/policer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_meter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_meter_init(
    int unit);







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
    
    dnx_data_meter_meter_db_tcsm_support,

    
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
    *dnx_data_meter_meter_db_meter_ids_in_group_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_meter_db_feature_get_f feature_get;
    
    dnx_data_meter_meter_db_nof_ingress_db_get_f nof_ingress_db_get;
    
    dnx_data_meter_meter_db_nof_egress_db_get_f nof_egress_db_get;
    
    dnx_data_meter_meter_db_meter_ids_in_group_get_f meter_ids_in_group_get;
} dnx_data_if_meter_meter_db_t;







typedef enum
{

    
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
    *dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_egress_single_bucket_engine_get_f) (
    int unit);


typedef uint32(
    *dnx_data_meter_mem_mgmt_counters_buckets_ratio_per_engine_get_f) (
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



typedef struct
{
    
    dnx_data_meter_mem_mgmt_feature_get_f feature_get;
    
    dnx_data_meter_mem_mgmt_adrress_range_resolution_get_f adrress_range_resolution_get;
    
    dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_get_f object_stat_pointer_base_resolution_get;
    
    dnx_data_meter_mem_mgmt_base_big_engine_for_meter_get_f base_big_engine_for_meter_get;
    
    dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_get_f ingress_single_bucket_engine_get;
    
    dnx_data_meter_mem_mgmt_egress_single_bucket_engine_get_f egress_single_bucket_engine_get;
    
    dnx_data_meter_mem_mgmt_counters_buckets_ratio_per_engine_get_f counters_buckets_ratio_per_engine_get;
    
    dnx_data_meter_mem_mgmt_meter_pointer_size_get_f meter_pointer_size_get;
    
    dnx_data_meter_mem_mgmt_meter_pointer_map_size_get_f meter_pointer_map_size_get;
    
    dnx_data_meter_mem_mgmt_invalid_bank_id_get_f invalid_bank_id_get;
    
    dnx_data_meter_mem_mgmt_ptr_map_table_resolution_get_f ptr_map_table_resolution_get;
    
    dnx_data_meter_mem_mgmt_bank_id_size_get_f bank_id_size_get;
    
    dnx_data_meter_mem_mgmt_nof_bank_ids_get_f nof_bank_ids_get;
    
    dnx_data_meter_mem_mgmt_sections_offset_get_f sections_offset_get;
} dnx_data_if_meter_mem_mgmt_t;






typedef struct
{
    
    int min;
    
    int max;
} dnx_data_meter_compensation_ingress_port_delta_value_t;



typedef enum
{

    
    _dnx_data_meter_compensation_feature_nof
} dnx_data_meter_compensation_feature_e;



typedef int(
    *dnx_data_meter_compensation_feature_get_f) (
    int unit,
    dnx_data_meter_compensation_feature_e feature);


typedef const dnx_data_meter_compensation_ingress_port_delta_value_t *(
    *dnx_data_meter_compensation_ingress_port_delta_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_compensation_feature_get_f feature_get;
    
    dnx_data_meter_compensation_ingress_port_delta_value_get_f ingress_port_delta_value_get;
    
    dnxc_data_table_info_get_f ingress_port_delta_value_info_get;
} dnx_data_if_meter_compensation_t;







typedef enum
{
    
    dnx_data_meter_expansion_expansion_based_on_tc_support,

    
    _dnx_data_meter_expansion_feature_nof
} dnx_data_meter_expansion_feature_e;



typedef int(
    *dnx_data_meter_expansion_feature_get_f) (
    int unit,
    dnx_data_meter_expansion_feature_e feature);


typedef uint32(
    *dnx_data_meter_expansion_max_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_meter_expansion_feature_get_f feature_get;
    
    dnx_data_meter_expansion_max_size_get_f max_size_get;
} dnx_data_if_meter_expansion_t;





typedef struct
{
    
    dnx_data_if_meter_profile_t profile;
    
    dnx_data_if_meter_diag_t diag;
    
    dnx_data_if_meter_meter_db_t meter_db;
    
    dnx_data_if_meter_mem_mgmt_t mem_mgmt;
    
    dnx_data_if_meter_compensation_t compensation;
    
    dnx_data_if_meter_expansion_t expansion;
} dnx_data_if_meter_t;




extern dnx_data_if_meter_t dnx_data_meter;


#endif 

