

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SCH_H_

#define _DNX_DATA_INTERNAL_SCH_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_sch_submodule_general,
    dnx_data_sch_submodule_ps,
    dnx_data_sch_submodule_flow,
    dnx_data_sch_submodule_se,
    dnx_data_sch_submodule_interface,
    dnx_data_sch_submodule_device,
    dnx_data_sch_submodule_sch_alloc,
    dnx_data_sch_submodule_dbal,
    dnx_data_sch_submodule_features,

    
    _dnx_data_sch_submodule_nof
} dnx_data_sch_submodule_e;








int dnx_data_sch_general_feature_get(
    int unit,
    dnx_data_sch_general_feature_e feature);



typedef enum
{
    dnx_data_sch_general_define_nof_slow_profiles,
    dnx_data_sch_general_define_nof_slow_levels,
    dnx_data_sch_general_define_cycles_per_credit_token,
    dnx_data_sch_general_define_nof_fmq_class,
    dnx_data_sch_general_define_port_priority_propagation_enable,

    
    _dnx_data_sch_general_define_nof
} dnx_data_sch_general_define_e;



uint32 dnx_data_sch_general_nof_slow_profiles_get(
    int unit);


uint32 dnx_data_sch_general_nof_slow_levels_get(
    int unit);


uint32 dnx_data_sch_general_cycles_per_credit_token_get(
    int unit);


uint32 dnx_data_sch_general_nof_fmq_class_get(
    int unit);


uint32 dnx_data_sch_general_port_priority_propagation_enable_get(
    int unit);



typedef enum
{
    dnx_data_sch_general_table_low_rate_factor_to_dbal_enum,
    dnx_data_sch_general_table_dbal_enum_to_low_rate_factor,
    dnx_data_sch_general_table_slow_rate_max_bucket,

    
    _dnx_data_sch_general_table_nof
} dnx_data_sch_general_table_e;



const dnx_data_sch_general_low_rate_factor_to_dbal_enum_t * dnx_data_sch_general_low_rate_factor_to_dbal_enum_get(
    int unit,
    int low_rate_factor);


const dnx_data_sch_general_dbal_enum_to_low_rate_factor_t * dnx_data_sch_general_dbal_enum_to_low_rate_factor_get(
    int unit,
    int dbal_enum);


const dnx_data_sch_general_slow_rate_max_bucket_t * dnx_data_sch_general_slow_rate_max_bucket_get(
    int unit,
    int idx);



shr_error_e dnx_data_sch_general_low_rate_factor_to_dbal_enum_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_general_dbal_enum_to_low_rate_factor_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_general_slow_rate_max_bucket_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_sch_general_low_rate_factor_to_dbal_enum_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_general_dbal_enum_to_low_rate_factor_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_general_slow_rate_max_bucket_info_get(
    int unit);






int dnx_data_sch_ps_feature_get(
    int unit,
    dnx_data_sch_ps_feature_e feature);



typedef enum
{
    dnx_data_sch_ps_define_min_priority_for_tcg,
    dnx_data_sch_ps_define_max_nof_tcg,
    dnx_data_sch_ps_define_nof_hrs_in_ps,
    dnx_data_sch_ps_define_max_tcg_weight,
    dnx_data_sch_ps_define_tc_tcg_calendar_access_period,
    dnx_data_sch_ps_define_max_port_rate_mbps,
    dnx_data_sch_ps_define_max_burst,

    
    _dnx_data_sch_ps_define_nof
} dnx_data_sch_ps_define_e;



uint32 dnx_data_sch_ps_min_priority_for_tcg_get(
    int unit);


uint32 dnx_data_sch_ps_max_nof_tcg_get(
    int unit);


uint32 dnx_data_sch_ps_nof_hrs_in_ps_get(
    int unit);


uint32 dnx_data_sch_ps_max_tcg_weight_get(
    int unit);


uint32 dnx_data_sch_ps_tc_tcg_calendar_access_period_get(
    int unit);


uint32 dnx_data_sch_ps_max_port_rate_mbps_get(
    int unit);


uint32 dnx_data_sch_ps_max_burst_get(
    int unit);



typedef enum
{

    
    _dnx_data_sch_ps_table_nof
} dnx_data_sch_ps_table_e;









int dnx_data_sch_flow_feature_get(
    int unit,
    dnx_data_sch_flow_feature_e feature);



typedef enum
{
    dnx_data_sch_flow_define_nof_hr,
    dnx_data_sch_flow_define_nof_cl,
    dnx_data_sch_flow_define_nof_se,
    dnx_data_sch_flow_define_nof_flows,
    dnx_data_sch_flow_define_region_size,
    dnx_data_sch_flow_define_nof_regions,
    dnx_data_sch_flow_define_nof_hr_regions,
    dnx_data_sch_flow_define_nof_connectors_only_regions,
    dnx_data_sch_flow_define_flow_in_region_mask,
    dnx_data_sch_flow_define_min_connector_bundle_size,
    dnx_data_sch_flow_define_conn_idx_interdig_flow,
    dnx_data_sch_flow_define_shaper_flipflops_nof,
    dnx_data_sch_flow_define_erp_hr,
    dnx_data_sch_flow_define_reserved_hr,
    dnx_data_sch_flow_define_max_se,
    dnx_data_sch_flow_define_max_flow,
    dnx_data_sch_flow_define_first_hr_region,
    dnx_data_sch_flow_define_first_se_flow_id,
    dnx_data_sch_flow_define_hr_se_id_min,
    dnx_data_sch_flow_define_hr_se_id_max,
    dnx_data_sch_flow_define_default_credit_source_se_id,
    dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation,

    
    _dnx_data_sch_flow_define_nof
} dnx_data_sch_flow_define_e;



uint32 dnx_data_sch_flow_nof_hr_get(
    int unit);


uint32 dnx_data_sch_flow_nof_cl_get(
    int unit);


uint32 dnx_data_sch_flow_nof_se_get(
    int unit);


uint32 dnx_data_sch_flow_nof_flows_get(
    int unit);


uint32 dnx_data_sch_flow_region_size_get(
    int unit);


uint32 dnx_data_sch_flow_nof_regions_get(
    int unit);


uint32 dnx_data_sch_flow_nof_hr_regions_get(
    int unit);


uint32 dnx_data_sch_flow_nof_connectors_only_regions_get(
    int unit);


uint32 dnx_data_sch_flow_flow_in_region_mask_get(
    int unit);


uint32 dnx_data_sch_flow_min_connector_bundle_size_get(
    int unit);


uint32 dnx_data_sch_flow_conn_idx_interdig_flow_get(
    int unit);


uint32 dnx_data_sch_flow_shaper_flipflops_nof_get(
    int unit);


uint32 dnx_data_sch_flow_erp_hr_get(
    int unit);


uint32 dnx_data_sch_flow_reserved_hr_get(
    int unit);


uint32 dnx_data_sch_flow_max_se_get(
    int unit);


uint32 dnx_data_sch_flow_max_flow_get(
    int unit);


uint32 dnx_data_sch_flow_first_hr_region_get(
    int unit);


uint32 dnx_data_sch_flow_first_se_flow_id_get(
    int unit);


uint32 dnx_data_sch_flow_hr_se_id_min_get(
    int unit);


uint32 dnx_data_sch_flow_hr_se_id_max_get(
    int unit);


uint32 dnx_data_sch_flow_default_credit_source_se_id_get(
    int unit);


uint32 dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_get(
    int unit);



typedef enum
{
    dnx_data_sch_flow_table_region_type,
    dnx_data_sch_flow_table_nof_remote_cores,
    dnx_data_sch_flow_table_cl_num_to_quartet_offset,
    dnx_data_sch_flow_table_quartet_offset_to_cl_num,

    
    _dnx_data_sch_flow_table_nof
} dnx_data_sch_flow_table_e;



const dnx_data_sch_flow_region_type_t * dnx_data_sch_flow_region_type_get(
    int unit,
    int core,
    int region);


const dnx_data_sch_flow_nof_remote_cores_t * dnx_data_sch_flow_nof_remote_cores_get(
    int unit,
    int core,
    int region);


const dnx_data_sch_flow_cl_num_to_quartet_offset_t * dnx_data_sch_flow_cl_num_to_quartet_offset_get(
    int unit,
    int cl_num);


const dnx_data_sch_flow_quartet_offset_to_cl_num_t * dnx_data_sch_flow_quartet_offset_to_cl_num_get(
    int unit,
    int quartet_offset);



shr_error_e dnx_data_sch_flow_region_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_flow_nof_remote_cores_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_flow_cl_num_to_quartet_offset_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_flow_quartet_offset_to_cl_num_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_sch_flow_region_type_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_flow_nof_remote_cores_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_flow_cl_num_to_quartet_offset_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_flow_quartet_offset_to_cl_num_info_get(
    int unit);






int dnx_data_sch_se_feature_get(
    int unit,
    dnx_data_sch_se_feature_e feature);



typedef enum
{
    dnx_data_sch_se_define_default_cos,
    dnx_data_sch_se_define_nof_color_group,
    dnx_data_sch_se_define_max_hr_weight,
    dnx_data_sch_se_define_cl_class_profile_nof,

    
    _dnx_data_sch_se_define_nof
} dnx_data_sch_se_define_e;



uint32 dnx_data_sch_se_default_cos_get(
    int unit);


uint32 dnx_data_sch_se_nof_color_group_get(
    int unit);


uint32 dnx_data_sch_se_max_hr_weight_get(
    int unit);


uint32 dnx_data_sch_se_cl_class_profile_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_sch_se_table_nof
} dnx_data_sch_se_table_e;









int dnx_data_sch_interface_feature_get(
    int unit,
    dnx_data_sch_interface_feature_e feature);



typedef enum
{
    dnx_data_sch_interface_define_nof_big_calendars,
    dnx_data_sch_interface_define_nof_channelized_calendars,
    dnx_data_sch_interface_define_big_calendar_size,
    dnx_data_sch_interface_define_nof_sch_interfaces,
    dnx_data_sch_interface_define_cal_speed_resolution,
    dnx_data_sch_interface_define_reserved,
    dnx_data_sch_interface_define_max_if_rate_mbps,

    
    _dnx_data_sch_interface_define_nof
} dnx_data_sch_interface_define_e;



uint32 dnx_data_sch_interface_nof_big_calendars_get(
    int unit);


uint32 dnx_data_sch_interface_nof_channelized_calendars_get(
    int unit);


uint32 dnx_data_sch_interface_big_calendar_size_get(
    int unit);


uint32 dnx_data_sch_interface_nof_sch_interfaces_get(
    int unit);


uint32 dnx_data_sch_interface_cal_speed_resolution_get(
    int unit);


uint32 dnx_data_sch_interface_reserved_get(
    int unit);


uint32 dnx_data_sch_interface_max_if_rate_mbps_get(
    int unit);



typedef enum
{
    dnx_data_sch_interface_table_shaper_resolution,

    
    _dnx_data_sch_interface_table_nof
} dnx_data_sch_interface_table_e;



const dnx_data_sch_interface_shaper_resolution_t * dnx_data_sch_interface_shaper_resolution_get(
    int unit,
    int channelized);



shr_error_e dnx_data_sch_interface_shaper_resolution_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_sch_interface_shaper_resolution_info_get(
    int unit);






int dnx_data_sch_device_feature_get(
    int unit,
    dnx_data_sch_device_feature_e feature);



typedef enum
{
    dnx_data_sch_device_define_drm_resolution,

    
    _dnx_data_sch_device_define_nof
} dnx_data_sch_device_define_e;



uint32 dnx_data_sch_device_drm_resolution_get(
    int unit);



typedef enum
{

    
    _dnx_data_sch_device_table_nof
} dnx_data_sch_device_table_e;









int dnx_data_sch_sch_alloc_feature_get(
    int unit,
    dnx_data_sch_sch_alloc_feature_e feature);



typedef enum
{
    dnx_data_sch_sch_alloc_define_tag_aggregate_se_2,
    dnx_data_sch_sch_alloc_define_tag_aggregate_se_4,
    dnx_data_sch_sch_alloc_define_tag_aggregate_se_8,
    dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value,
    dnx_data_sch_sch_alloc_define_tag_size_aggregate_se,
    dnx_data_sch_sch_alloc_define_tag_size_con,
    dnx_data_sch_sch_alloc_define_type_con_reg_start,
    dnx_data_sch_sch_alloc_define_alloc_invalid_flow,
    dnx_data_sch_sch_alloc_define_dealloc_flow_ids,
    dnx_data_sch_sch_alloc_define_type_con_reg_end,
    dnx_data_sch_sch_alloc_define_type_hr_reg_start,

    
    _dnx_data_sch_sch_alloc_define_nof
} dnx_data_sch_sch_alloc_define_e;



uint32 dnx_data_sch_sch_alloc_tag_aggregate_se_2_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_tag_aggregate_se_4_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_tag_aggregate_se_8_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_tag_size_aggregate_se_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_tag_size_con_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_type_con_reg_start_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_alloc_invalid_flow_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_dealloc_flow_ids_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_type_con_reg_end_get(
    int unit);


uint32 dnx_data_sch_sch_alloc_type_hr_reg_start_get(
    int unit);



typedef enum
{
    dnx_data_sch_sch_alloc_table_region,
    dnx_data_sch_sch_alloc_table_connector,
    dnx_data_sch_sch_alloc_table_se_per_region_type,
    dnx_data_sch_sch_alloc_table_se,
    dnx_data_sch_sch_alloc_table_composite_se_per_region_type,
    dnx_data_sch_sch_alloc_table_composite_se,

    
    _dnx_data_sch_sch_alloc_table_nof
} dnx_data_sch_sch_alloc_table_e;



const dnx_data_sch_sch_alloc_region_t * dnx_data_sch_sch_alloc_region_get(
    int unit,
    int region_type);


const dnx_data_sch_sch_alloc_connector_t * dnx_data_sch_sch_alloc_connector_get(
    int unit,
    int composite,
    int interdigitated);


const dnx_data_sch_sch_alloc_se_per_region_type_t * dnx_data_sch_sch_alloc_se_per_region_type_get(
    int unit,
    int flow_type,
    int region_type);


const dnx_data_sch_sch_alloc_se_t * dnx_data_sch_sch_alloc_se_get(
    int unit,
    int flow_type);


const dnx_data_sch_sch_alloc_composite_se_per_region_type_t * dnx_data_sch_sch_alloc_composite_se_per_region_type_get(
    int unit,
    int flow_type,
    int region_type);


const dnx_data_sch_sch_alloc_composite_se_t * dnx_data_sch_sch_alloc_composite_se_get(
    int unit,
    int flow_type,
    int odd_even_mode);



shr_error_e dnx_data_sch_sch_alloc_region_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_sch_alloc_connector_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_sch_alloc_se_per_region_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_sch_alloc_se_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_sch_alloc_composite_se_per_region_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_sch_sch_alloc_composite_se_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_sch_sch_alloc_region_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_sch_alloc_connector_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_sch_alloc_se_per_region_type_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_sch_alloc_se_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_sch_alloc_composite_se_per_region_type_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_sch_sch_alloc_composite_se_info_get(
    int unit);






int dnx_data_sch_dbal_feature_get(
    int unit,
    dnx_data_sch_dbal_feature_e feature);



typedef enum
{
    dnx_data_sch_dbal_define_scheduler_enable_grouping_factor,
    dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor,
    dnx_data_sch_dbal_define_flow_bits,
    dnx_data_sch_dbal_define_se_bits,
    dnx_data_sch_dbal_define_cl_bits,
    dnx_data_sch_dbal_define_hr_bits,
    dnx_data_sch_dbal_define_interface_bits,
    dnx_data_sch_dbal_define_calendar_bits,
    dnx_data_sch_dbal_define_flow_shaper_mant_bits,
    dnx_data_sch_dbal_define_ps_shaper_quanta_bits,
    dnx_data_sch_dbal_define_ps_shaper_max_burst_bits,
    dnx_data_sch_dbal_define_drm_nof_links_max,
    dnx_data_sch_dbal_define_scheduler_enable_key_size,
    dnx_data_sch_dbal_define_flow_id_pair_key_size,
    dnx_data_sch_dbal_define_flow_id_pair_max,
    dnx_data_sch_dbal_define_ps_bits,
    dnx_data_sch_dbal_define_flow_shaper_descr_bits,
    dnx_data_sch_dbal_define_ps_shaper_bits,
    dnx_data_sch_dbal_define_cl_profile_bits,

    
    _dnx_data_sch_dbal_define_nof
} dnx_data_sch_dbal_define_e;



uint32 dnx_data_sch_dbal_scheduler_enable_grouping_factor_get(
    int unit);


uint32 dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_get(
    int unit);


uint32 dnx_data_sch_dbal_flow_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_se_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_cl_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_hr_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_interface_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_calendar_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_flow_shaper_mant_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_ps_shaper_quanta_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_ps_shaper_max_burst_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_drm_nof_links_max_get(
    int unit);


uint32 dnx_data_sch_dbal_scheduler_enable_key_size_get(
    int unit);


uint32 dnx_data_sch_dbal_flow_id_pair_key_size_get(
    int unit);


uint32 dnx_data_sch_dbal_flow_id_pair_max_get(
    int unit);


uint32 dnx_data_sch_dbal_ps_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_flow_shaper_descr_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_ps_shaper_bits_get(
    int unit);


uint32 dnx_data_sch_dbal_cl_profile_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_sch_dbal_table_nof
} dnx_data_sch_dbal_table_e;









int dnx_data_sch_features_feature_get(
    int unit,
    dnx_data_sch_features_feature_e feature);



typedef enum
{

    
    _dnx_data_sch_features_define_nof
} dnx_data_sch_features_define_e;




typedef enum
{

    
    _dnx_data_sch_features_table_nof
} dnx_data_sch_features_table_e;






shr_error_e dnx_data_sch_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

