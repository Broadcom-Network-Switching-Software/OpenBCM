

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SCH_H_

#define _DNX_DATA_SCH_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sch.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_sch_init(
    int unit);






typedef struct
{
    
    dbal_enum_value_field_low_rate_factor_e dbal_enum;
    
    int valid;
} dnx_data_sch_general_low_rate_factor_to_dbal_enum_t;


typedef struct
{
    
    int low_rate_factor;
} dnx_data_sch_general_dbal_enum_to_low_rate_factor_t;


typedef struct
{
    
    int rate;
    
    uint32 max_bucket;
} dnx_data_sch_general_slow_rate_max_bucket_t;



typedef enum
{
    
    dnx_data_sch_general_lag_scheduler_supported,
    
    dnx_data_sch_general_fsm_modes_config_supported,
    
    dnx_data_sch_general_calrx_rxi_enable_supported,
    
    dnx_data_sch_general_port_priority_propagation_supported,

    
    _dnx_data_sch_general_feature_nof
} dnx_data_sch_general_feature_e;



typedef int(
    *dnx_data_sch_general_feature_get_f) (
    int unit,
    dnx_data_sch_general_feature_e feature);


typedef uint32(
    *dnx_data_sch_general_nof_slow_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_general_nof_slow_levels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_general_cycles_per_credit_token_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_general_nof_fmq_class_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_general_port_priority_propagation_enable_get_f) (
    int unit);


typedef const dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *(
    *dnx_data_sch_general_low_rate_factor_to_dbal_enum_get_f) (
    int unit,
    int low_rate_factor);


typedef const dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *(
    *dnx_data_sch_general_dbal_enum_to_low_rate_factor_get_f) (
    int unit,
    int dbal_enum);


typedef const dnx_data_sch_general_slow_rate_max_bucket_t *(
    *dnx_data_sch_general_slow_rate_max_bucket_get_f) (
    int unit,
    int idx);



typedef struct
{
    
    dnx_data_sch_general_feature_get_f feature_get;
    
    dnx_data_sch_general_nof_slow_profiles_get_f nof_slow_profiles_get;
    
    dnx_data_sch_general_nof_slow_levels_get_f nof_slow_levels_get;
    
    dnx_data_sch_general_cycles_per_credit_token_get_f cycles_per_credit_token_get;
    
    dnx_data_sch_general_nof_fmq_class_get_f nof_fmq_class_get;
    
    dnx_data_sch_general_port_priority_propagation_enable_get_f port_priority_propagation_enable_get;
    
    dnx_data_sch_general_low_rate_factor_to_dbal_enum_get_f low_rate_factor_to_dbal_enum_get;
    
    dnxc_data_table_info_get_f low_rate_factor_to_dbal_enum_info_get;
    
    dnx_data_sch_general_dbal_enum_to_low_rate_factor_get_f dbal_enum_to_low_rate_factor_get;
    
    dnxc_data_table_info_get_f dbal_enum_to_low_rate_factor_info_get;
    
    dnx_data_sch_general_slow_rate_max_bucket_get_f slow_rate_max_bucket_get;
    
    dnxc_data_table_info_get_f slow_rate_max_bucket_info_get;
} dnx_data_if_sch_general_t;







typedef enum
{

    
    _dnx_data_sch_ps_feature_nof
} dnx_data_sch_ps_feature_e;



typedef int(
    *dnx_data_sch_ps_feature_get_f) (
    int unit,
    dnx_data_sch_ps_feature_e feature);


typedef uint32(
    *dnx_data_sch_ps_min_priority_for_tcg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_ps_max_nof_tcg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_ps_nof_hrs_in_ps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_ps_max_tcg_weight_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_ps_tc_tcg_calendar_access_period_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_ps_max_port_rate_mbps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_ps_max_burst_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sch_ps_feature_get_f feature_get;
    
    dnx_data_sch_ps_min_priority_for_tcg_get_f min_priority_for_tcg_get;
    
    dnx_data_sch_ps_max_nof_tcg_get_f max_nof_tcg_get;
    
    dnx_data_sch_ps_nof_hrs_in_ps_get_f nof_hrs_in_ps_get;
    
    dnx_data_sch_ps_max_tcg_weight_get_f max_tcg_weight_get;
    
    dnx_data_sch_ps_tc_tcg_calendar_access_period_get_f tc_tcg_calendar_access_period_get;
    
    dnx_data_sch_ps_max_port_rate_mbps_get_f max_port_rate_mbps_get;
    
    dnx_data_sch_ps_max_burst_get_f max_burst_get;
} dnx_data_if_sch_ps_t;






typedef struct
{
    
    uint32 type;
    
    uint32 propagation_direction;
} dnx_data_sch_flow_region_type_t;


typedef struct
{
    
    uint32 val;
} dnx_data_sch_flow_nof_remote_cores_t;


typedef struct
{
    
    int quartet_offset;
} dnx_data_sch_flow_cl_num_to_quartet_offset_t;


typedef struct
{
    
    int cl_num;
} dnx_data_sch_flow_quartet_offset_to_cl_num_t;



typedef enum
{
    
    dnx_data_sch_flow_cl_fq_cl_fq_quartet,

    
    _dnx_data_sch_flow_feature_nof
} dnx_data_sch_flow_feature_e;



typedef int(
    *dnx_data_sch_flow_feature_get_f) (
    int unit,
    dnx_data_sch_flow_feature_e feature);


typedef uint32(
    *dnx_data_sch_flow_nof_hr_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_nof_cl_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_nof_se_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_nof_flows_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_region_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_nof_regions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_nof_hr_regions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_nof_connectors_only_regions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_flow_in_region_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_min_connector_bundle_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_conn_idx_interdig_flow_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_shaper_flipflops_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_erp_hr_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_reserved_hr_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_max_se_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_max_flow_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_first_hr_region_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_first_se_flow_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_hr_se_id_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_hr_se_id_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_default_credit_source_se_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_get_f) (
    int unit);


typedef const dnx_data_sch_flow_region_type_t *(
    *dnx_data_sch_flow_region_type_get_f) (
    int unit,
    int core,
    int region);


typedef const dnx_data_sch_flow_nof_remote_cores_t *(
    *dnx_data_sch_flow_nof_remote_cores_get_f) (
    int unit,
    int core,
    int region);


typedef const dnx_data_sch_flow_cl_num_to_quartet_offset_t *(
    *dnx_data_sch_flow_cl_num_to_quartet_offset_get_f) (
    int unit,
    int cl_num);


typedef const dnx_data_sch_flow_quartet_offset_to_cl_num_t *(
    *dnx_data_sch_flow_quartet_offset_to_cl_num_get_f) (
    int unit,
    int quartet_offset);



typedef struct
{
    
    dnx_data_sch_flow_feature_get_f feature_get;
    
    dnx_data_sch_flow_nof_hr_get_f nof_hr_get;
    
    dnx_data_sch_flow_nof_cl_get_f nof_cl_get;
    
    dnx_data_sch_flow_nof_se_get_f nof_se_get;
    
    dnx_data_sch_flow_nof_flows_get_f nof_flows_get;
    
    dnx_data_sch_flow_region_size_get_f region_size_get;
    
    dnx_data_sch_flow_nof_regions_get_f nof_regions_get;
    
    dnx_data_sch_flow_nof_hr_regions_get_f nof_hr_regions_get;
    
    dnx_data_sch_flow_nof_connectors_only_regions_get_f nof_connectors_only_regions_get;
    
    dnx_data_sch_flow_flow_in_region_mask_get_f flow_in_region_mask_get;
    
    dnx_data_sch_flow_min_connector_bundle_size_get_f min_connector_bundle_size_get;
    
    dnx_data_sch_flow_conn_idx_interdig_flow_get_f conn_idx_interdig_flow_get;
    
    dnx_data_sch_flow_shaper_flipflops_nof_get_f shaper_flipflops_nof_get;
    
    dnx_data_sch_flow_erp_hr_get_f erp_hr_get;
    
    dnx_data_sch_flow_reserved_hr_get_f reserved_hr_get;
    
    dnx_data_sch_flow_max_se_get_f max_se_get;
    
    dnx_data_sch_flow_max_flow_get_f max_flow_get;
    
    dnx_data_sch_flow_first_hr_region_get_f first_hr_region_get;
    
    dnx_data_sch_flow_first_se_flow_id_get_f first_se_flow_id_get;
    
    dnx_data_sch_flow_hr_se_id_min_get_f hr_se_id_min_get;
    
    dnx_data_sch_flow_hr_se_id_max_get_f hr_se_id_max_get;
    
    dnx_data_sch_flow_default_credit_source_se_id_get_f default_credit_source_se_id_get;
    
    dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_get_f runtime_performance_optimize_enable_sched_allocation_get;
    
    dnx_data_sch_flow_region_type_get_f region_type_get;
    
    dnxc_data_table_info_get_f region_type_info_get;
    
    dnx_data_sch_flow_nof_remote_cores_get_f nof_remote_cores_get;
    
    dnxc_data_table_info_get_f nof_remote_cores_info_get;
    
    dnx_data_sch_flow_cl_num_to_quartet_offset_get_f cl_num_to_quartet_offset_get;
    
    dnxc_data_table_info_get_f cl_num_to_quartet_offset_info_get;
    
    dnx_data_sch_flow_quartet_offset_to_cl_num_get_f quartet_offset_to_cl_num_get;
    
    dnxc_data_table_info_get_f quartet_offset_to_cl_num_info_get;
} dnx_data_if_sch_flow_t;







typedef enum
{

    
    _dnx_data_sch_se_feature_nof
} dnx_data_sch_se_feature_e;



typedef int(
    *dnx_data_sch_se_feature_get_f) (
    int unit,
    dnx_data_sch_se_feature_e feature);


typedef uint32(
    *dnx_data_sch_se_default_cos_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_se_nof_color_group_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_se_max_hr_weight_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_se_cl_class_profile_nof_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sch_se_feature_get_f feature_get;
    
    dnx_data_sch_se_default_cos_get_f default_cos_get;
    
    dnx_data_sch_se_nof_color_group_get_f nof_color_group_get;
    
    dnx_data_sch_se_max_hr_weight_get_f max_hr_weight_get;
    
    dnx_data_sch_se_cl_class_profile_nof_get_f cl_class_profile_nof_get;
} dnx_data_if_sch_se_t;






typedef struct
{
    
    uint32 resolution;
} dnx_data_sch_interface_shaper_resolution_t;



typedef enum
{
    
    dnx_data_sch_interface_non_channelized_calendar,

    
    _dnx_data_sch_interface_feature_nof
} dnx_data_sch_interface_feature_e;



typedef int(
    *dnx_data_sch_interface_feature_get_f) (
    int unit,
    dnx_data_sch_interface_feature_e feature);


typedef uint32(
    *dnx_data_sch_interface_nof_big_calendars_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_interface_nof_channelized_calendars_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_interface_big_calendar_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_interface_nof_sch_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_interface_cal_speed_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_interface_reserved_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_interface_max_if_rate_mbps_get_f) (
    int unit);


typedef const dnx_data_sch_interface_shaper_resolution_t *(
    *dnx_data_sch_interface_shaper_resolution_get_f) (
    int unit,
    int channelized);



typedef struct
{
    
    dnx_data_sch_interface_feature_get_f feature_get;
    
    dnx_data_sch_interface_nof_big_calendars_get_f nof_big_calendars_get;
    
    dnx_data_sch_interface_nof_channelized_calendars_get_f nof_channelized_calendars_get;
    
    dnx_data_sch_interface_big_calendar_size_get_f big_calendar_size_get;
    
    dnx_data_sch_interface_nof_sch_interfaces_get_f nof_sch_interfaces_get;
    
    dnx_data_sch_interface_cal_speed_resolution_get_f cal_speed_resolution_get;
    
    dnx_data_sch_interface_reserved_get_f reserved_get;
    
    dnx_data_sch_interface_max_if_rate_mbps_get_f max_if_rate_mbps_get;
    
    dnx_data_sch_interface_shaper_resolution_get_f shaper_resolution_get;
    
    dnxc_data_table_info_get_f shaper_resolution_info_get;
} dnx_data_if_sch_interface_t;







typedef enum
{
    
    dnx_data_sch_device_shared_drm,

    
    _dnx_data_sch_device_feature_nof
} dnx_data_sch_device_feature_e;



typedef int(
    *dnx_data_sch_device_feature_get_f) (
    int unit,
    dnx_data_sch_device_feature_e feature);


typedef uint32(
    *dnx_data_sch_device_drm_resolution_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sch_device_feature_get_f feature_get;
    
    dnx_data_sch_device_drm_resolution_get_f drm_resolution_get;
} dnx_data_if_sch_device_t;






typedef struct
{
    
    dnx_sch_element_se_type_e flow_type[4];
    
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
} dnx_data_sch_sch_alloc_region_t;


typedef struct
{
    
    uint32 alignment;
    
    uint32 pattern;
    
    uint32 pattern_size;
    
    uint32 nof_in_pattern;
} dnx_data_sch_sch_alloc_connector_t;


typedef struct
{
    
    uint32 valid;
    
    uint32 alignment;
    
    uint32 pattern;
    
    uint32 pattern_size;
    
    uint32 nof_offsets;
    
    uint32 offset[4];
} dnx_data_sch_sch_alloc_se_per_region_type_t;


typedef struct
{
    
    uint32 alignment;
    
    uint32 pattern;
    
    uint32 pattern_size;
    
    uint32 nof_offsets;
    
    uint32 offset[4];
} dnx_data_sch_sch_alloc_se_t;


typedef struct
{
    
    uint32 valid;
    
    uint32 alignment;
    
    uint32 pattern;
    
    uint32 pattern_size;
} dnx_data_sch_sch_alloc_composite_se_per_region_type_t;


typedef struct
{
    
    uint32 alignment;
    
    uint32 pattern;
    
    uint32 pattern_size;
} dnx_data_sch_sch_alloc_composite_se_t;



typedef enum
{

    
    _dnx_data_sch_sch_alloc_feature_nof
} dnx_data_sch_sch_alloc_feature_e;



typedef int(
    *dnx_data_sch_sch_alloc_feature_get_f) (
    int unit,
    dnx_data_sch_sch_alloc_feature_e feature);


typedef uint32(
    *dnx_data_sch_sch_alloc_tag_aggregate_se_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_tag_aggregate_se_4_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_tag_aggregate_se_8_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_tag_size_aggregate_se_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_tag_size_con_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_type_con_reg_start_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_alloc_invalid_flow_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_dealloc_flow_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_type_con_reg_end_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_sch_alloc_type_hr_reg_start_get_f) (
    int unit);


typedef const dnx_data_sch_sch_alloc_region_t *(
    *dnx_data_sch_sch_alloc_region_get_f) (
    int unit,
    int region_type);


typedef const dnx_data_sch_sch_alloc_connector_t *(
    *dnx_data_sch_sch_alloc_connector_get_f) (
    int unit,
    int composite,
    int interdigitated);


typedef const dnx_data_sch_sch_alloc_se_per_region_type_t *(
    *dnx_data_sch_sch_alloc_se_per_region_type_get_f) (
    int unit,
    int flow_type,
    int region_type);


typedef const dnx_data_sch_sch_alloc_se_t *(
    *dnx_data_sch_sch_alloc_se_get_f) (
    int unit,
    int flow_type);


typedef const dnx_data_sch_sch_alloc_composite_se_per_region_type_t *(
    *dnx_data_sch_sch_alloc_composite_se_per_region_type_get_f) (
    int unit,
    int flow_type,
    int region_type);


typedef const dnx_data_sch_sch_alloc_composite_se_t *(
    *dnx_data_sch_sch_alloc_composite_se_get_f) (
    int unit,
    int flow_type,
    int odd_even_mode);



typedef struct
{
    
    dnx_data_sch_sch_alloc_feature_get_f feature_get;
    
    dnx_data_sch_sch_alloc_tag_aggregate_se_2_get_f tag_aggregate_se_2_get;
    
    dnx_data_sch_sch_alloc_tag_aggregate_se_4_get_f tag_aggregate_se_4_get;
    
    dnx_data_sch_sch_alloc_tag_aggregate_se_8_get_f tag_aggregate_se_8_get;
    
    dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_get_f shared_shaper_max_tag_value_get;
    
    dnx_data_sch_sch_alloc_tag_size_aggregate_se_get_f tag_size_aggregate_se_get;
    
    dnx_data_sch_sch_alloc_tag_size_con_get_f tag_size_con_get;
    
    dnx_data_sch_sch_alloc_type_con_reg_start_get_f type_con_reg_start_get;
    
    dnx_data_sch_sch_alloc_alloc_invalid_flow_get_f alloc_invalid_flow_get;
    
    dnx_data_sch_sch_alloc_dealloc_flow_ids_get_f dealloc_flow_ids_get;
    
    dnx_data_sch_sch_alloc_type_con_reg_end_get_f type_con_reg_end_get;
    
    dnx_data_sch_sch_alloc_type_hr_reg_start_get_f type_hr_reg_start_get;
    
    dnx_data_sch_sch_alloc_region_get_f region_get;
    
    dnxc_data_table_info_get_f region_info_get;
    
    dnx_data_sch_sch_alloc_connector_get_f connector_get;
    
    dnxc_data_table_info_get_f connector_info_get;
    
    dnx_data_sch_sch_alloc_se_per_region_type_get_f se_per_region_type_get;
    
    dnxc_data_table_info_get_f se_per_region_type_info_get;
    
    dnx_data_sch_sch_alloc_se_get_f se_get;
    
    dnxc_data_table_info_get_f se_info_get;
    
    dnx_data_sch_sch_alloc_composite_se_per_region_type_get_f composite_se_per_region_type_get;
    
    dnxc_data_table_info_get_f composite_se_per_region_type_info_get;
    
    dnx_data_sch_sch_alloc_composite_se_get_f composite_se_get;
    
    dnxc_data_table_info_get_f composite_se_info_get;
} dnx_data_if_sch_sch_alloc_t;







typedef enum
{

    
    _dnx_data_sch_dbal_feature_nof
} dnx_data_sch_dbal_feature_e;



typedef int(
    *dnx_data_sch_dbal_feature_get_f) (
    int unit,
    dnx_data_sch_dbal_feature_e feature);


typedef uint32(
    *dnx_data_sch_dbal_scheduler_enable_grouping_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_flow_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_se_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_cl_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_hr_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_interface_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_calendar_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_flow_shaper_mant_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_ps_shaper_quanta_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_ps_shaper_max_burst_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_drm_nof_links_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_scheduler_enable_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_flow_id_pair_key_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_flow_id_pair_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_ps_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_flow_shaper_descr_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_ps_shaper_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sch_dbal_cl_profile_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sch_dbal_feature_get_f feature_get;
    
    dnx_data_sch_dbal_scheduler_enable_grouping_factor_get_f scheduler_enable_grouping_factor_get;
    
    dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_get_f scheduler_enable_dbal_mult_factor_get;
    
    dnx_data_sch_dbal_flow_bits_get_f flow_bits_get;
    
    dnx_data_sch_dbal_se_bits_get_f se_bits_get;
    
    dnx_data_sch_dbal_cl_bits_get_f cl_bits_get;
    
    dnx_data_sch_dbal_hr_bits_get_f hr_bits_get;
    
    dnx_data_sch_dbal_interface_bits_get_f interface_bits_get;
    
    dnx_data_sch_dbal_calendar_bits_get_f calendar_bits_get;
    
    dnx_data_sch_dbal_flow_shaper_mant_bits_get_f flow_shaper_mant_bits_get;
    
    dnx_data_sch_dbal_ps_shaper_quanta_bits_get_f ps_shaper_quanta_bits_get;
    
    dnx_data_sch_dbal_ps_shaper_max_burst_bits_get_f ps_shaper_max_burst_bits_get;
    
    dnx_data_sch_dbal_drm_nof_links_max_get_f drm_nof_links_max_get;
    
    dnx_data_sch_dbal_scheduler_enable_key_size_get_f scheduler_enable_key_size_get;
    
    dnx_data_sch_dbal_flow_id_pair_key_size_get_f flow_id_pair_key_size_get;
    
    dnx_data_sch_dbal_flow_id_pair_max_get_f flow_id_pair_max_get;
    
    dnx_data_sch_dbal_ps_bits_get_f ps_bits_get;
    
    dnx_data_sch_dbal_flow_shaper_descr_bits_get_f flow_shaper_descr_bits_get;
    
    dnx_data_sch_dbal_ps_shaper_bits_get_f ps_shaper_bits_get;
    
    dnx_data_sch_dbal_cl_profile_bits_get_f cl_profile_bits_get;
} dnx_data_if_sch_dbal_t;







typedef enum
{
    dnx_data_sch_features_dlm,

    
    _dnx_data_sch_features_feature_nof
} dnx_data_sch_features_feature_e;



typedef int(
    *dnx_data_sch_features_feature_get_f) (
    int unit,
    dnx_data_sch_features_feature_e feature);



typedef struct
{
    
    dnx_data_sch_features_feature_get_f feature_get;
} dnx_data_if_sch_features_t;





typedef struct
{
    
    dnx_data_if_sch_general_t general;
    
    dnx_data_if_sch_ps_t ps;
    
    dnx_data_if_sch_flow_t flow;
    
    dnx_data_if_sch_se_t se;
    
    dnx_data_if_sch_interface_t interface;
    
    dnx_data_if_sch_device_t device;
    
    dnx_data_if_sch_sch_alloc_t sch_alloc;
    
    dnx_data_if_sch_dbal_t dbal;
    
    dnx_data_if_sch_features_t features;
} dnx_data_if_sch_t;




extern dnx_data_if_sch_t dnx_data_sch;


#endif 

