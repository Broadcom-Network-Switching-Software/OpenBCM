

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_STIF_H_

#define _DNX_DATA_INTERNAL_STIF_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_stif_submodule_config,
    dnx_data_stif_submodule_port,
    dnx_data_stif_submodule_report,
    dnx_data_stif_submodule_flow_control,

    
    _dnx_data_stif_submodule_nof
} dnx_data_stif_submodule_e;








int dnx_data_stif_config_feature_get(
    int unit,
    dnx_data_stif_config_feature_e feature);



typedef enum
{
    dnx_data_stif_config_define_sram_buffers_resolution,
    dnx_data_stif_config_define_sram_pdbs_resolution,
    dnx_data_stif_config_define_dram_bdb_resolution,
    dnx_data_stif_config_define_stif_packet_size,
    dnx_data_stif_config_define_stif_scrubber_queue_min,
    dnx_data_stif_config_define_stif_scrubber_queue_max,
    dnx_data_stif_config_define_stif_scrubber_rate_min,
    dnx_data_stif_config_define_stif_scrubber_rate_max,
    dnx_data_stif_config_define_stif_report_mode,
    dnx_data_stif_config_define_stif_report_size_ingress,
    dnx_data_stif_config_define_stif_report_size_egress,
    dnx_data_stif_config_define_stif_idle_report,
    dnx_data_stif_config_define_nof_valid_sources,

    
    _dnx_data_stif_config_define_nof
} dnx_data_stif_config_define_e;



uint32 dnx_data_stif_config_sram_buffers_resolution_get(
    int unit);


uint32 dnx_data_stif_config_sram_pdbs_resolution_get(
    int unit);


uint32 dnx_data_stif_config_dram_bdb_resolution_get(
    int unit);


uint32 dnx_data_stif_config_stif_packet_size_get(
    int unit);


uint32 dnx_data_stif_config_stif_scrubber_queue_min_get(
    int unit);


uint32 dnx_data_stif_config_stif_scrubber_queue_max_get(
    int unit);


uint32 dnx_data_stif_config_stif_scrubber_rate_min_get(
    int unit);


uint32 dnx_data_stif_config_stif_scrubber_rate_max_get(
    int unit);


uint32 dnx_data_stif_config_stif_report_mode_get(
    int unit);


uint32 dnx_data_stif_config_stif_report_size_ingress_get(
    int unit);


uint32 dnx_data_stif_config_stif_report_size_egress_get(
    int unit);


uint32 dnx_data_stif_config_stif_idle_report_get(
    int unit);


uint32 dnx_data_stif_config_nof_valid_sources_get(
    int unit);



typedef enum
{
    dnx_data_stif_config_table_scrubber_buffers,
    dnx_data_stif_config_table_selective_report_range,

    
    _dnx_data_stif_config_table_nof
} dnx_data_stif_config_table_e;



const dnx_data_stif_config_scrubber_buffers_t * dnx_data_stif_config_scrubber_buffers_get(
    int unit,
    int thresh_id);


const dnx_data_stif_config_selective_report_range_t * dnx_data_stif_config_selective_report_range_get(
    int unit,
    int range_id);



shr_error_e dnx_data_stif_config_scrubber_buffers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_stif_config_selective_report_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_stif_config_scrubber_buffers_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_stif_config_selective_report_range_info_get(
    int unit);






int dnx_data_stif_port_feature_get(
    int unit,
    dnx_data_stif_port_feature_e feature);



typedef enum
{
    dnx_data_stif_port_define_max_nof_instances,
    dnx_data_stif_port_define_stif_ethu_select_size,
    dnx_data_stif_port_define_stif_lane_mapping_size,

    
    _dnx_data_stif_port_define_nof
} dnx_data_stif_port_define_e;



uint32 dnx_data_stif_port_max_nof_instances_get(
    int unit);


uint32 dnx_data_stif_port_stif_ethu_select_size_get(
    int unit);


uint32 dnx_data_stif_port_stif_lane_mapping_size_get(
    int unit);



typedef enum
{
    dnx_data_stif_port_table_ethu_id_stif_valid,

    
    _dnx_data_stif_port_table_nof
} dnx_data_stif_port_table_e;



const dnx_data_stif_port_ethu_id_stif_valid_t * dnx_data_stif_port_ethu_id_stif_valid_get(
    int unit,
    int ethu_id);



shr_error_e dnx_data_stif_port_ethu_id_stif_valid_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_stif_port_ethu_id_stif_valid_info_get(
    int unit);






int dnx_data_stif_report_feature_get(
    int unit,
    dnx_data_stif_report_feature_e feature);



typedef enum
{
    dnx_data_stif_report_define_qsize_idle_report_size,
    dnx_data_stif_report_define_qsize_idle_report_period,
    dnx_data_stif_report_define_qsize_idle_report_period_invalid,
    dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit,
    dnx_data_stif_report_define_max_thresholds_per_buffer,
    dnx_data_stif_report_define_incoming_tc,
    dnx_data_stif_report_define_record_two_msb,
    dnx_data_stif_report_define_nof_source_types,
    dnx_data_stif_report_define_stat_id_max_count,
    dnx_data_stif_report_define_stat_object_size,
    dnx_data_stif_report_define_billing_format_element_packet_size,
    dnx_data_stif_report_define_billing_format_msb_opcode_size,
    dnx_data_stif_report_define_billing_format_etpp_metadata_size,
    dnx_data_stif_report_define_billing_opsize_use_one_type_mask,
    dnx_data_stif_report_define_billing_opsize_use_two_types_mask,
    dnx_data_stif_report_define_billing_opsize_use_three_types_mask,
    dnx_data_stif_report_define_billing_opsize_use_four_types_mask,
    dnx_data_stif_report_define_billing_enq_ing_size_shift,

    
    _dnx_data_stif_report_define_nof
} dnx_data_stif_report_define_e;



uint32 dnx_data_stif_report_qsize_idle_report_size_get(
    int unit);


uint32 dnx_data_stif_report_qsize_idle_report_period_get(
    int unit);


uint32 dnx_data_stif_report_qsize_idle_report_period_invalid_get(
    int unit);


uint32 dnx_data_stif_report_qsize_scrubber_report_cycles_unit_get(
    int unit);


uint32 dnx_data_stif_report_max_thresholds_per_buffer_get(
    int unit);


uint32 dnx_data_stif_report_incoming_tc_get(
    int unit);


uint32 dnx_data_stif_report_record_two_msb_get(
    int unit);


uint32 dnx_data_stif_report_nof_source_types_get(
    int unit);


uint32 dnx_data_stif_report_stat_id_max_count_get(
    int unit);


uint32 dnx_data_stif_report_stat_object_size_get(
    int unit);


uint32 dnx_data_stif_report_billing_format_element_packet_size_get(
    int unit);


uint32 dnx_data_stif_report_billing_format_msb_opcode_size_get(
    int unit);


uint32 dnx_data_stif_report_billing_format_etpp_metadata_size_get(
    int unit);


uint32 dnx_data_stif_report_billing_opsize_use_one_type_mask_get(
    int unit);


uint32 dnx_data_stif_report_billing_opsize_use_two_types_mask_get(
    int unit);


uint32 dnx_data_stif_report_billing_opsize_use_three_types_mask_get(
    int unit);


uint32 dnx_data_stif_report_billing_opsize_use_four_types_mask_get(
    int unit);


uint32 dnx_data_stif_report_billing_enq_ing_size_shift_get(
    int unit);



typedef enum
{
    dnx_data_stif_report_table_etpp_metadata,
    dnx_data_stif_report_table_report_size,

    
    _dnx_data_stif_report_table_nof
} dnx_data_stif_report_table_e;



const dnx_data_stif_report_etpp_metadata_t * dnx_data_stif_report_etpp_metadata_get(
    int unit,
    int element);


const dnx_data_stif_report_report_size_t * dnx_data_stif_report_report_size_get(
    int unit,
    int dbal_enum);



shr_error_e dnx_data_stif_report_etpp_metadata_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_stif_report_report_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_stif_report_etpp_metadata_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_stif_report_report_size_info_get(
    int unit);






int dnx_data_stif_flow_control_feature_get(
    int unit,
    dnx_data_stif_flow_control_feature_e feature);



typedef enum
{
    dnx_data_stif_flow_control_define_etpp_pipe_length,
    dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo,
    dnx_data_stif_flow_control_define_cgm_pipe_length,
    dnx_data_stif_flow_control_define_cgm_size_of_fifo,
    dnx_data_stif_flow_control_define_billing_level_sampling,
    dnx_data_stif_flow_control_define_fc_disable_value,
    dnx_data_stif_flow_control_define_billing_etpp_flow_control_high,
    dnx_data_stif_flow_control_define_billing_etpp_flow_control_low,
    dnx_data_stif_flow_control_define_billing_cgm_flow_control_high,
    dnx_data_stif_flow_control_define_billing_cgm_flow_control_low,
    dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high,
    dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low,

    
    _dnx_data_stif_flow_control_define_nof
} dnx_data_stif_flow_control_define_e;



uint32 dnx_data_stif_flow_control_etpp_pipe_length_get(
    int unit);


uint32 dnx_data_stif_flow_control_etpp_deq_size_of_fifo_get(
    int unit);


uint32 dnx_data_stif_flow_control_cgm_pipe_length_get(
    int unit);


uint32 dnx_data_stif_flow_control_cgm_size_of_fifo_get(
    int unit);


uint32 dnx_data_stif_flow_control_billing_level_sampling_get(
    int unit);


uint32 dnx_data_stif_flow_control_fc_disable_value_get(
    int unit);


uint32 dnx_data_stif_flow_control_billing_etpp_flow_control_high_get(
    int unit);


uint32 dnx_data_stif_flow_control_billing_etpp_flow_control_low_get(
    int unit);


uint32 dnx_data_stif_flow_control_billing_cgm_flow_control_high_get(
    int unit);


uint32 dnx_data_stif_flow_control_billing_cgm_flow_control_low_get(
    int unit);


uint32 dnx_data_stif_flow_control_qsize_cgm_flow_control_high_get(
    int unit);


uint32 dnx_data_stif_flow_control_qsize_cgm_flow_control_low_get(
    int unit);



typedef enum
{

    
    _dnx_data_stif_flow_control_table_nof
} dnx_data_stif_flow_control_table_e;






shr_error_e dnx_data_stif_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

