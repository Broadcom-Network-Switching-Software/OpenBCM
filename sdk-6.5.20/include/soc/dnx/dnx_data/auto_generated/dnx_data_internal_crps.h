

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_CRPS_H_

#define _DNX_DATA_INTERNAL_CRPS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_crps_submodule_engine,
    dnx_data_crps_submodule_eviction,
    dnx_data_crps_submodule_expansion,
    dnx_data_crps_submodule_src_interface,
    dnx_data_crps_submodule_latency,

    
    _dnx_data_crps_submodule_nof
} dnx_data_crps_submodule_e;








int dnx_data_crps_engine_feature_get(
    int unit,
    dnx_data_crps_engine_feature_e feature);



typedef enum
{
    dnx_data_crps_engine_define_nof_engines,
    dnx_data_crps_engine_define_nof_mid_engines,
    dnx_data_crps_engine_define_nof_big_engines,
    dnx_data_crps_engine_define_nof_databases,
    dnx_data_crps_engine_define_data_mapping_table_size,
    dnx_data_crps_engine_define_max_counter_set_size,
    dnx_data_crps_engine_define_oam_counter_set_size,
    dnx_data_crps_engine_define_last_address_nof_bits,

    
    _dnx_data_crps_engine_define_nof
} dnx_data_crps_engine_define_e;



uint32 dnx_data_crps_engine_nof_engines_get(
    int unit);


uint32 dnx_data_crps_engine_nof_mid_engines_get(
    int unit);


uint32 dnx_data_crps_engine_nof_big_engines_get(
    int unit);


uint32 dnx_data_crps_engine_nof_databases_get(
    int unit);


uint32 dnx_data_crps_engine_data_mapping_table_size_get(
    int unit);


uint32 dnx_data_crps_engine_max_counter_set_size_get(
    int unit);


uint32 dnx_data_crps_engine_oam_counter_set_size_get(
    int unit);


uint32 dnx_data_crps_engine_last_address_nof_bits_get(
    int unit);



typedef enum
{
    dnx_data_crps_engine_table_engines_info,
    dnx_data_crps_engine_table_source_base,

    
    _dnx_data_crps_engine_table_nof
} dnx_data_crps_engine_table_e;



const dnx_data_crps_engine_engines_info_t * dnx_data_crps_engine_engines_info_get(
    int unit,
    int engine_id);


const dnx_data_crps_engine_source_base_t * dnx_data_crps_engine_source_base_get(
    int unit,
    int source);



shr_error_e dnx_data_crps_engine_engines_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_engine_source_base_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_crps_engine_engines_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_engine_source_base_info_get(
    int unit);






int dnx_data_crps_eviction_feature_get(
    int unit,
    dnx_data_crps_eviction_feature_e feature);



typedef enum
{
    dnx_data_crps_eviction_define_counters_fifo_depth,
    dnx_data_crps_eviction_define_nof_counters_fifo,
    dnx_data_crps_eviction_define_dma_record_entry_nof_bits,
    dnx_data_crps_eviction_define_packet_and_bytes_packet_size,
    dnx_data_crps_eviction_define_bubble_before_request_timeout,
    dnx_data_crps_eviction_define_bubble_after_request_timeout,
    dnx_data_crps_eviction_define_seq_address_nof_bits,
    dnx_data_crps_eviction_define_bg_thread_enable,

    
    _dnx_data_crps_eviction_define_nof
} dnx_data_crps_eviction_define_e;



uint32 dnx_data_crps_eviction_counters_fifo_depth_get(
    int unit);


uint32 dnx_data_crps_eviction_nof_counters_fifo_get(
    int unit);


uint32 dnx_data_crps_eviction_dma_record_entry_nof_bits_get(
    int unit);


uint32 dnx_data_crps_eviction_packet_and_bytes_packet_size_get(
    int unit);


uint32 dnx_data_crps_eviction_bubble_before_request_timeout_get(
    int unit);


uint32 dnx_data_crps_eviction_bubble_after_request_timeout_get(
    int unit);


uint32 dnx_data_crps_eviction_seq_address_nof_bits_get(
    int unit);


uint32 dnx_data_crps_eviction_bg_thread_enable_get(
    int unit);



typedef enum
{
    dnx_data_crps_eviction_table_phy_record_format,
    dnx_data_crps_eviction_table_condional_action_valid,

    
    _dnx_data_crps_eviction_table_nof
} dnx_data_crps_eviction_table_e;



const dnx_data_crps_eviction_phy_record_format_t * dnx_data_crps_eviction_phy_record_format_get(
    int unit);


const dnx_data_crps_eviction_condional_action_valid_t * dnx_data_crps_eviction_condional_action_valid_get(
    int unit,
    int action_idx);



shr_error_e dnx_data_crps_eviction_phy_record_format_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_eviction_condional_action_valid_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_crps_eviction_phy_record_format_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_eviction_condional_action_valid_info_get(
    int unit);






int dnx_data_crps_expansion_feature_get(
    int unit,
    dnx_data_crps_expansion_feature_e feature);



typedef enum
{
    dnx_data_crps_expansion_define_expansion_size,

    
    _dnx_data_crps_expansion_define_nof
} dnx_data_crps_expansion_define_e;



uint32 dnx_data_crps_expansion_expansion_size_get(
    int unit);



typedef enum
{
    dnx_data_crps_expansion_table_irpp_types,
    dnx_data_crps_expansion_table_itm_types,
    dnx_data_crps_expansion_table_itpp_types,
    dnx_data_crps_expansion_table_erpp_types,
    dnx_data_crps_expansion_table_etm_types,
    dnx_data_crps_expansion_table_etpp_types,

    
    _dnx_data_crps_expansion_table_nof
} dnx_data_crps_expansion_table_e;



const dnx_data_crps_expansion_irpp_types_t * dnx_data_crps_expansion_irpp_types_get(
    int unit);


const dnx_data_crps_expansion_itm_types_t * dnx_data_crps_expansion_itm_types_get(
    int unit);


const dnx_data_crps_expansion_itpp_types_t * dnx_data_crps_expansion_itpp_types_get(
    int unit);


const dnx_data_crps_expansion_erpp_types_t * dnx_data_crps_expansion_erpp_types_get(
    int unit);


const dnx_data_crps_expansion_etm_types_t * dnx_data_crps_expansion_etm_types_get(
    int unit);


const dnx_data_crps_expansion_etpp_types_t * dnx_data_crps_expansion_etpp_types_get(
    int unit);



shr_error_e dnx_data_crps_expansion_irpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_expansion_itm_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_expansion_itpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_expansion_erpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_expansion_etm_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_crps_expansion_etpp_types_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_crps_expansion_irpp_types_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_expansion_itm_types_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_expansion_itpp_types_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_expansion_erpp_types_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_expansion_etm_types_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_crps_expansion_etpp_types_info_get(
    int unit);






int dnx_data_crps_src_interface_feature_get(
    int unit,
    dnx_data_crps_src_interface_feature_e feature);



typedef enum
{

    
    _dnx_data_crps_src_interface_define_nof
} dnx_data_crps_src_interface_define_e;




typedef enum
{
    dnx_data_crps_src_interface_table_command_id,

    
    _dnx_data_crps_src_interface_table_nof
} dnx_data_crps_src_interface_table_e;



const dnx_data_crps_src_interface_command_id_t * dnx_data_crps_src_interface_command_id_get(
    int unit,
    int source);



shr_error_e dnx_data_crps_src_interface_command_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_crps_src_interface_command_id_info_get(
    int unit);






int dnx_data_crps_latency_feature_get(
    int unit,
    dnx_data_crps_latency_feature_e feature);



typedef enum
{
    dnx_data_crps_latency_define_etpp_command_id,
    dnx_data_crps_latency_define_stat_id_port_offset,
    dnx_data_crps_latency_define_stat_id_tc_offset,
    dnx_data_crps_latency_define_stat_id_multicast_offset,

    
    _dnx_data_crps_latency_define_nof
} dnx_data_crps_latency_define_e;



uint32 dnx_data_crps_latency_etpp_command_id_get(
    int unit);


uint32 dnx_data_crps_latency_stat_id_port_offset_get(
    int unit);


uint32 dnx_data_crps_latency_stat_id_tc_offset_get(
    int unit);


uint32 dnx_data_crps_latency_stat_id_multicast_offset_get(
    int unit);



typedef enum
{

    
    _dnx_data_crps_latency_table_nof
} dnx_data_crps_latency_table_e;






shr_error_e dnx_data_crps_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

