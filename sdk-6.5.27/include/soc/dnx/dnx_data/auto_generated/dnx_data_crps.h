
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_CRPS_H_

#define _DNX_DATA_CRPS_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_crps.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_crps_init(
    int unit);






typedef struct
{
    int nof_entries;
    int meter_shared;
} dnx_data_crps_engine_engines_info_t;


typedef struct
{
    int base_val;
} dnx_data_crps_engine_source_base_t;


typedef struct
{
    int valid;
    int nof_bits_for_packets;
    int nof_bits_for_bytes;
    int nof_counters_per_entry;
    bcm_eviction_destination_type_t valid_eviction_destinations[DNX_DATA_MAX_CRPS_EVICTION_NOF_EVICTION_DESTINATION_TYPES];
} dnx_data_crps_engine_counter_format_types_t;



typedef enum
{
    dnx_data_crps_engine_valid_address_admission_is_supported,
    dnx_data_crps_engine_last_input_command_is_supported,
    dnx_data_crps_engine_multi_sets_is_supported,
    dnx_data_crps_engine_dynamic_counter_mode_is_supported,

    
    _dnx_data_crps_engine_feature_nof
} dnx_data_crps_engine_feature_e;



typedef int(
    *dnx_data_crps_engine_feature_get_f) (
    int unit,
    dnx_data_crps_engine_feature_e feature);


typedef uint32(
    *dnx_data_crps_engine_nof_engines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_nof_mid_engines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_nof_big_engines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_nof_databases_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_data_mapping_table_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_max_counter_set_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_oam_counter_set_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_max_nof_counter_sets_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_last_address_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_counter_format_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_counter_mode_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_nof_small_meter_shared_engines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_nof_mid_meter_shared_engines_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_engine_nof_big_meter_shared_engines_get_f) (
    int unit);


typedef const dnx_data_crps_engine_engines_info_t *(
    *dnx_data_crps_engine_engines_info_get_f) (
    int unit,
    int engine_id);


typedef const dnx_data_crps_engine_source_base_t *(
    *dnx_data_crps_engine_source_base_get_f) (
    int unit,
    int source);


typedef const dnx_data_crps_engine_counter_format_types_t *(
    *dnx_data_crps_engine_counter_format_types_get_f) (
    int unit,
    int format_type);



typedef struct
{
    
    dnx_data_crps_engine_feature_get_f feature_get;
    
    dnx_data_crps_engine_nof_engines_get_f nof_engines_get;
    
    dnx_data_crps_engine_nof_mid_engines_get_f nof_mid_engines_get;
    
    dnx_data_crps_engine_nof_big_engines_get_f nof_big_engines_get;
    
    dnx_data_crps_engine_nof_databases_get_f nof_databases_get;
    
    dnx_data_crps_engine_data_mapping_table_size_get_f data_mapping_table_size_get;
    
    dnx_data_crps_engine_max_counter_set_size_get_f max_counter_set_size_get;
    
    dnx_data_crps_engine_oam_counter_set_size_get_f oam_counter_set_size_get;
    
    dnx_data_crps_engine_max_nof_counter_sets_get_f max_nof_counter_sets_get;
    
    dnx_data_crps_engine_last_address_nof_bits_get_f last_address_nof_bits_get;
    
    dnx_data_crps_engine_counter_format_nof_bits_get_f counter_format_nof_bits_get;
    
    dnx_data_crps_engine_counter_mode_nof_bits_get_f counter_mode_nof_bits_get;
    
    dnx_data_crps_engine_nof_small_meter_shared_engines_get_f nof_small_meter_shared_engines_get;
    
    dnx_data_crps_engine_nof_mid_meter_shared_engines_get_f nof_mid_meter_shared_engines_get;
    
    dnx_data_crps_engine_nof_big_meter_shared_engines_get_f nof_big_meter_shared_engines_get;
    
    dnx_data_crps_engine_engines_info_get_f engines_info_get;
    
    dnxc_data_table_info_get_f engines_info_info_get;
    
    dnx_data_crps_engine_source_base_get_f source_base_get;
    
    dnxc_data_table_info_get_f source_base_info_get;
    
    dnx_data_crps_engine_counter_format_types_get_f counter_format_types_get;
    
    dnxc_data_table_info_get_f counter_format_types_info_get;
} dnx_data_if_crps_engine_t;






typedef struct
{
    uint32 obj_type_size;
    uint32 obj_type_offset;
    int reserved0_size;
    int reserved0_offset;
    uint32 opcode_size;
    uint32 opcode_offset;
    uint32 err_ind_size;
    uint32 err_ind_offset;
    int reserved1_size;
    int reserved1_offset;
    uint32 core_id_size;
    uint32 core_id_offset;
    uint32 engine_id_size;
    uint32 engine_id_offset;
    uint32 local_ctr_id_size;
    uint32 local_ctr_id_offset;
    uint32 ctr_value_size;
    uint32 ctr_value_offset;
    int is_probabilistic_size;
    int is_probabilistic_offset;
} dnx_data_crps_eviction_phy_record_format_t;


typedef struct
{
    uint32 valid;
} dnx_data_crps_eviction_condional_action_valid_t;


typedef struct
{
    int valid;
} dnx_data_crps_eviction_supported_eviction_destination_t;



typedef enum
{
    dnx_data_crps_eviction_algorithmic_is_supported,
    dnx_data_crps_eviction_probabilistic_is_supported,
    dnx_data_crps_eviction_low_rate_probabilistic_eviction,
    dnx_data_crps_eviction_conditional_action_is_supported,
    dnx_data_crps_eviction_eviction_from_memory_is_supported,
    dnx_data_crps_eviction_sequential_scan_speedup_is_supported,
    dnx_data_crps_eviction_probabilistic_eviction_thresholds_is_supported,
    dnx_data_crps_eviction_iteration_interval_time_config_is_supported,

    
    _dnx_data_crps_eviction_feature_nof
} dnx_data_crps_eviction_feature_e;



typedef int(
    *dnx_data_crps_eviction_feature_get_f) (
    int unit,
    dnx_data_crps_eviction_feature_e feature);


typedef uint32(
    *dnx_data_crps_eviction_counters_fifo_depth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_nof_counters_fifo_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_dma_record_entry_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_bubble_before_request_timeout_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_bubble_after_request_timeout_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_seq_address_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_nof_eviction_destination_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_nof_crps_network_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_eviction_bg_thread_enable_get_f) (
    int unit);


typedef const dnx_data_crps_eviction_phy_record_format_t *(
    *dnx_data_crps_eviction_phy_record_format_get_f) (
    int unit);


typedef const dnx_data_crps_eviction_condional_action_valid_t *(
    *dnx_data_crps_eviction_condional_action_valid_get_f) (
    int unit,
    int action_idx);


typedef const dnx_data_crps_eviction_supported_eviction_destination_t *(
    *dnx_data_crps_eviction_supported_eviction_destination_get_f) (
    int unit,
    int destination_type);



typedef struct
{
    
    dnx_data_crps_eviction_feature_get_f feature_get;
    
    dnx_data_crps_eviction_counters_fifo_depth_get_f counters_fifo_depth_get;
    
    dnx_data_crps_eviction_nof_counters_fifo_get_f nof_counters_fifo_get;
    
    dnx_data_crps_eviction_dma_record_entry_nof_bits_get_f dma_record_entry_nof_bits_get;
    
    dnx_data_crps_eviction_bubble_before_request_timeout_get_f bubble_before_request_timeout_get;
    
    dnx_data_crps_eviction_bubble_after_request_timeout_get_f bubble_after_request_timeout_get;
    
    dnx_data_crps_eviction_seq_address_nof_bits_get_f seq_address_nof_bits_get;
    
    dnx_data_crps_eviction_nof_eviction_destination_types_get_f nof_eviction_destination_types_get;
    
    dnx_data_crps_eviction_nof_crps_network_channels_get_f nof_crps_network_channels_get;
    
    dnx_data_crps_eviction_bg_thread_enable_get_f bg_thread_enable_get;
    
    dnx_data_crps_eviction_phy_record_format_get_f phy_record_format_get;
    
    dnxc_data_table_info_get_f phy_record_format_info_get;
    
    dnx_data_crps_eviction_condional_action_valid_get_f condional_action_valid_get;
    
    dnxc_data_table_info_get_f condional_action_valid_info_get;
    
    dnx_data_crps_eviction_supported_eviction_destination_get_f supported_eviction_destination_get;
    
    dnxc_data_table_info_get_f supported_eviction_destination_info_get;
} dnx_data_if_crps_eviction_t;






typedef struct
{
    int offset;
    int nof_bits;
} dnx_data_crps_expansion_source_metadata_types_t;


typedef struct
{
    int expansion_size;
    int supports_expansion_per_type;
} dnx_data_crps_expansion_source_expansion_info_t;



typedef enum
{

    
    _dnx_data_crps_expansion_feature_nof
} dnx_data_crps_expansion_feature_e;



typedef int(
    *dnx_data_crps_expansion_feature_get_f) (
    int unit,
    dnx_data_crps_expansion_feature_e feature);


typedef uint32(
    *dnx_data_crps_expansion_expansion_size_get_f) (
    int unit);


typedef const dnx_data_crps_expansion_source_metadata_types_t *(
    *dnx_data_crps_expansion_source_metadata_types_get_f) (
    int unit,
    int source,
    int metadata_type);


typedef const dnx_data_crps_expansion_source_expansion_info_t *(
    *dnx_data_crps_expansion_source_expansion_info_get_f) (
    int unit,
    int source);



typedef struct
{
    
    dnx_data_crps_expansion_feature_get_f feature_get;
    
    dnx_data_crps_expansion_expansion_size_get_f expansion_size_get;
    
    dnx_data_crps_expansion_source_metadata_types_get_f source_metadata_types_get;
    
    dnxc_data_table_info_get_f source_metadata_types_info_get;
    
    dnx_data_crps_expansion_source_expansion_info_get_f source_expansion_info_get;
    
    dnxc_data_table_info_get_f source_expansion_info_info_get;
} dnx_data_if_crps_expansion_t;






typedef struct
{
    uint32 size;
    uint32 base;
} dnx_data_crps_src_interface_command_id_t;


typedef struct
{
    int valid_for_eviction;
} dnx_data_crps_src_interface_crps_eviction_t;


typedef struct
{
    int valid_for_counting;
} dnx_data_crps_src_interface_crps_counting_t;



typedef enum
{
    dnx_data_crps_src_interface_etm_count_pp_port_enable_table_is_supported,

    
    _dnx_data_crps_src_interface_feature_nof
} dnx_data_crps_src_interface_feature_e;



typedef int(
    *dnx_data_crps_src_interface_feature_get_f) (
    int unit,
    dnx_data_crps_src_interface_feature_e feature);


typedef const dnx_data_crps_src_interface_command_id_t *(
    *dnx_data_crps_src_interface_command_id_get_f) (
    int unit,
    int source);


typedef const dnx_data_crps_src_interface_crps_eviction_t *(
    *dnx_data_crps_src_interface_crps_eviction_get_f) (
    int unit,
    int source);


typedef const dnx_data_crps_src_interface_crps_counting_t *(
    *dnx_data_crps_src_interface_crps_counting_get_f) (
    int unit,
    int source);



typedef struct
{
    
    dnx_data_crps_src_interface_feature_get_f feature_get;
    
    dnx_data_crps_src_interface_command_id_get_f command_id_get;
    
    dnxc_data_table_info_get_f command_id_info_get;
    
    dnx_data_crps_src_interface_crps_eviction_get_f crps_eviction_get;
    
    dnxc_data_table_info_get_f crps_eviction_info_get;
    
    dnx_data_crps_src_interface_crps_counting_get_f crps_counting_get;
    
    dnxc_data_table_info_get_f crps_counting_info_get;
} dnx_data_if_crps_src_interface_t;







typedef enum
{
    dnx_data_crps_latency_valid_flow_profile,
    dnx_data_crps_latency_valid_end_to_end_aqm_profile,
    dnx_data_crps_latency_drop_probabilistic_mechanism_support,

    
    _dnx_data_crps_latency_feature_nof
} dnx_data_crps_latency_feature_e;



typedef int(
    *dnx_data_crps_latency_feature_get_f) (
    int unit,
    dnx_data_crps_latency_feature_e feature);


typedef uint32(
    *dnx_data_crps_latency_etpp_command_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_latency_epp_command_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_latency_stat_id_port_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_latency_stat_id_tc_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_latency_stat_id_multicast_offset_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_crps_latency_feature_get_f feature_get;
    
    dnx_data_crps_latency_etpp_command_id_get_f etpp_command_id_get;
    
    dnx_data_crps_latency_epp_command_id_get_f epp_command_id_get;
    
    dnx_data_crps_latency_stat_id_port_offset_get_f stat_id_port_offset_get;
    
    dnx_data_crps_latency_stat_id_tc_offset_get_f stat_id_tc_offset_get;
    
    dnx_data_crps_latency_stat_id_multicast_offset_get_f stat_id_multicast_offset_get;
} dnx_data_if_crps_latency_t;






typedef struct
{
    int valid;
} dnx_data_crps_state_counters_counter_size_per_format_type_t;



typedef enum
{

    
    _dnx_data_crps_state_counters_feature_nof
} dnx_data_crps_state_counters_feature_e;



typedef int(
    *dnx_data_crps_state_counters_feature_get_f) (
    int unit,
    dnx_data_crps_state_counters_feature_e feature);


typedef uint32(
    *dnx_data_crps_state_counters_max_wide_state_counters_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_state_counters_max_narrow_state_counters_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_crps_state_counters_state_counter_data_size_per_full_counter_get_f) (
    int unit);


typedef const dnx_data_crps_state_counters_counter_size_per_format_type_t *(
    *dnx_data_crps_state_counters_counter_size_per_format_type_get_f) (
    int unit,
    int format_type,
    int counter_size);



typedef struct
{
    
    dnx_data_crps_state_counters_feature_get_f feature_get;
    
    dnx_data_crps_state_counters_max_wide_state_counters_size_get_f max_wide_state_counters_size_get;
    
    dnx_data_crps_state_counters_max_narrow_state_counters_size_get_f max_narrow_state_counters_size_get;
    
    dnx_data_crps_state_counters_state_counter_data_size_per_full_counter_get_f state_counter_data_size_per_full_counter_get;
    
    dnx_data_crps_state_counters_counter_size_per_format_type_get_f counter_size_per_format_type_get;
    
    dnxc_data_table_info_get_f counter_size_per_format_type_info_get;
} dnx_data_if_crps_state_counters_t;







typedef enum
{
    dnx_data_crps_general_crps_expansion_support,
    dnx_data_crps_general_count_mc_as_one_support,
    dnx_data_crps_general_crps_compensation_support,
    dnx_data_crps_general_crps_set_eviction_boundaries_support,
    dnx_data_crps_general_is_used,
    dnx_data_crps_general_power_down_supported,
    dnx_data_crps_general_crps_rate_calculation_support,

    
    _dnx_data_crps_general_feature_nof
} dnx_data_crps_general_feature_e;



typedef int(
    *dnx_data_crps_general_feature_get_f) (
    int unit,
    dnx_data_crps_general_feature_e feature);



typedef struct
{
    
    dnx_data_crps_general_feature_get_f feature_get;
} dnx_data_if_crps_general_t;





typedef struct
{
    
    dnx_data_if_crps_engine_t engine;
    
    dnx_data_if_crps_eviction_t eviction;
    
    dnx_data_if_crps_expansion_t expansion;
    
    dnx_data_if_crps_src_interface_t src_interface;
    
    dnx_data_if_crps_latency_t latency;
    
    dnx_data_if_crps_state_counters_t state_counters;
    
    dnx_data_if_crps_general_t general;
} dnx_data_if_crps_t;




extern dnx_data_if_crps_t dnx_data_crps;


#endif 

