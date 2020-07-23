

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_CRPS_H_

#define _DNX_DATA_CRPS_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_crps.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_crps_init(
    int unit);






typedef struct
{
    
    int nof_counters;
    
    int meter_shared;
} dnx_data_crps_engine_engines_info_t;


typedef struct
{
    
    int base_val;
} dnx_data_crps_engine_source_base_t;



typedef enum
{
    
    dnx_data_crps_engine_valid_address_admission_is_supported,
    
    dnx_data_crps_engine_last_input_command_is_supported,

    
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
    *dnx_data_crps_engine_last_address_nof_bits_get_f) (
    int unit);


typedef const dnx_data_crps_engine_engines_info_t *(
    *dnx_data_crps_engine_engines_info_get_f) (
    int unit,
    int engine_id);


typedef const dnx_data_crps_engine_source_base_t *(
    *dnx_data_crps_engine_source_base_get_f) (
    int unit,
    int source);



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
    
    dnx_data_crps_engine_last_address_nof_bits_get_f last_address_nof_bits_get;
    
    dnx_data_crps_engine_engines_info_get_f engines_info_get;
    
    dnxc_data_table_info_get_f engines_info_info_get;
    
    dnx_data_crps_engine_source_base_get_f source_base_get;
    
    dnxc_data_table_info_get_f source_base_info_get;
} dnx_data_if_crps_engine_t;






typedef struct
{
    
    uint32 obj_type_size;
    
    uint32 obj_type_offset;
    
    uint32 reserved0_size;
    
    uint32 reserved0_offset;
    
    uint32 opcode_size;
    
    uint32 opcode_offset;
    
    uint32 err_ind_size;
    
    uint32 err_ind_offset;
    
    uint32 reserved1_size;
    
    uint32 reserved1_offset;
    
    uint32 core_id_size;
    
    uint32 core_id_offset;
    
    uint32 engine_id_size;
    
    uint32 engine_id_offset;
    
    uint32 local_ctr_id_size;
    
    uint32 local_ctr_id_offset;
    
    uint32 ctr_value_size;
    
    uint32 ctr_value_offset;
} dnx_data_crps_eviction_phy_record_format_t;


typedef struct
{
    
    uint32 valid;
} dnx_data_crps_eviction_condional_action_valid_t;



typedef enum
{
    
    dnx_data_crps_eviction_algorithmic_is_supported,
    
    dnx_data_crps_eviction_probabilistic_is_supported,
    
    dnx_data_crps_eviction_conditional_action_is_supported,
    
    dnx_data_crps_eviction_narrow_counters_is_supported,

    
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
    *dnx_data_crps_eviction_packet_and_bytes_packet_size_get_f) (
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
    *dnx_data_crps_eviction_bg_thread_enable_get_f) (
    int unit);


typedef const dnx_data_crps_eviction_phy_record_format_t *(
    *dnx_data_crps_eviction_phy_record_format_get_f) (
    int unit);


typedef const dnx_data_crps_eviction_condional_action_valid_t *(
    *dnx_data_crps_eviction_condional_action_valid_get_f) (
    int unit,
    int action_idx);



typedef struct
{
    
    dnx_data_crps_eviction_feature_get_f feature_get;
    
    dnx_data_crps_eviction_counters_fifo_depth_get_f counters_fifo_depth_get;
    
    dnx_data_crps_eviction_nof_counters_fifo_get_f nof_counters_fifo_get;
    
    dnx_data_crps_eviction_dma_record_entry_nof_bits_get_f dma_record_entry_nof_bits_get;
    
    dnx_data_crps_eviction_packet_and_bytes_packet_size_get_f packet_and_bytes_packet_size_get;
    
    dnx_data_crps_eviction_bubble_before_request_timeout_get_f bubble_before_request_timeout_get;
    
    dnx_data_crps_eviction_bubble_after_request_timeout_get_f bubble_after_request_timeout_get;
    
    dnx_data_crps_eviction_seq_address_nof_bits_get_f seq_address_nof_bits_get;
    
    dnx_data_crps_eviction_bg_thread_enable_get_f bg_thread_enable_get;
    
    dnx_data_crps_eviction_phy_record_format_get_f phy_record_format_get;
    
    dnxc_data_table_info_get_f phy_record_format_info_get;
    
    dnx_data_crps_eviction_condional_action_valid_get_f condional_action_valid_get;
    
    dnxc_data_table_info_get_f condional_action_valid_info_get;
} dnx_data_if_crps_eviction_t;






typedef struct
{
    
    bcm_stat_expansion_types_t valid_types[12];
    
    uint32 total_nof_bits;
    
    uint32 drop_reason_offset;
    
    uint32 disposition_offset;
    
    uint32 tc_offset;
    
    uint32 dp0_valid_offset;
    
    uint32 dp0_value_offset;
    
    uint32 dp1_valid_offset;
    
    uint32 dp1_value_offset;
    
    uint32 dp2_valid_offset;
    
    uint32 dp2_value_offset;
    
    uint32 final_dp_offset;
    
    uint32 reject_all_copies_offset;
    
    uint32 metadata_offset;
} dnx_data_crps_expansion_irpp_types_t;


typedef struct
{
    
    bcm_stat_expansion_types_t valid_types[6];
    
    uint32 total_nof_bits;
    
    uint32 drop_reason_offset;
    
    uint32 disposition_offset;
    
    uint32 tc_offset;
    
    uint32 final_dp_offset;
    
    uint32 last_copy_offset;
} dnx_data_crps_expansion_itm_types_t;


typedef struct
{
    
    bcm_stat_expansion_types_t valid_types[6];
    
    uint32 total_nof_bits;
    
    uint32 latency_bin_offset;
    
    uint32 tc_offset;
    
    uint32 latency_profile_offset;
    
    uint32 mc_offset;
} dnx_data_crps_expansion_itpp_types_t;


typedef struct
{
    
    bcm_stat_expansion_types_t valid_types[7];
    
    uint32 total_nof_bits;
    
    uint32 metadata_offset;
    
    uint32 dp_offset;
    
    uint32 tc_offset;
    
    uint32 mc_offset;
    
    uint32 tm_action_type_offset;
    
    uint32 pp_drop_reason_offset;
} dnx_data_crps_expansion_erpp_types_t;


typedef struct
{
    
    bcm_stat_expansion_types_t valid_types[7];
    
    uint32 total_nof_bits;
    
    uint32 drop_reason_offset;
    
    uint32 tm_discard_offset;
    
    uint32 pp_discard_offset;
    
    uint32 tc_offset;
    
    uint32 dp_offset;
} dnx_data_crps_expansion_etm_types_t;


typedef struct
{
    
    bcm_stat_expansion_types_t valid_types[8];
    
    uint32 total_nof_bits;
} dnx_data_crps_expansion_etpp_types_t;



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


typedef const dnx_data_crps_expansion_irpp_types_t *(
    *dnx_data_crps_expansion_irpp_types_get_f) (
    int unit);


typedef const dnx_data_crps_expansion_itm_types_t *(
    *dnx_data_crps_expansion_itm_types_get_f) (
    int unit);


typedef const dnx_data_crps_expansion_itpp_types_t *(
    *dnx_data_crps_expansion_itpp_types_get_f) (
    int unit);


typedef const dnx_data_crps_expansion_erpp_types_t *(
    *dnx_data_crps_expansion_erpp_types_get_f) (
    int unit);


typedef const dnx_data_crps_expansion_etm_types_t *(
    *dnx_data_crps_expansion_etm_types_get_f) (
    int unit);


typedef const dnx_data_crps_expansion_etpp_types_t *(
    *dnx_data_crps_expansion_etpp_types_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_crps_expansion_feature_get_f feature_get;
    
    dnx_data_crps_expansion_expansion_size_get_f expansion_size_get;
    
    dnx_data_crps_expansion_irpp_types_get_f irpp_types_get;
    
    dnxc_data_table_info_get_f irpp_types_info_get;
    
    dnx_data_crps_expansion_itm_types_get_f itm_types_get;
    
    dnxc_data_table_info_get_f itm_types_info_get;
    
    dnx_data_crps_expansion_itpp_types_get_f itpp_types_get;
    
    dnxc_data_table_info_get_f itpp_types_info_get;
    
    dnx_data_crps_expansion_erpp_types_get_f erpp_types_get;
    
    dnxc_data_table_info_get_f erpp_types_info_get;
    
    dnx_data_crps_expansion_etm_types_get_f etm_types_get;
    
    dnxc_data_table_info_get_f etm_types_info_get;
    
    dnx_data_crps_expansion_etpp_types_get_f etpp_types_get;
    
    dnxc_data_table_info_get_f etpp_types_info_get;
} dnx_data_if_crps_expansion_t;






typedef struct
{
    
    uint32 size;
    
    uint32 base;
} dnx_data_crps_src_interface_command_id_t;



typedef enum
{

    
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



typedef struct
{
    
    dnx_data_crps_src_interface_feature_get_f feature_get;
    
    dnx_data_crps_src_interface_command_id_get_f command_id_get;
    
    dnxc_data_table_info_get_f command_id_info_get;
} dnx_data_if_crps_src_interface_t;







typedef enum
{
    
    dnx_data_crps_latency_valid_flow_profile,
    
    dnx_data_crps_latency_valid_end_to_end_aqm_profile,
    
    dnx_data_crps_latency_probabilistic_drop_mechanism_support,

    
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
    
    dnx_data_crps_latency_stat_id_port_offset_get_f stat_id_port_offset_get;
    
    dnx_data_crps_latency_stat_id_tc_offset_get_f stat_id_tc_offset_get;
    
    dnx_data_crps_latency_stat_id_multicast_offset_get_f stat_id_multicast_offset_get;
} dnx_data_if_crps_latency_t;





typedef struct
{
    
    dnx_data_if_crps_engine_t engine;
    
    dnx_data_if_crps_eviction_t eviction;
    
    dnx_data_if_crps_expansion_t expansion;
    
    dnx_data_if_crps_src_interface_t src_interface;
    
    dnx_data_if_crps_latency_t latency;
} dnx_data_if_crps_t;




extern dnx_data_if_crps_t dnx_data_crps;


#endif 

