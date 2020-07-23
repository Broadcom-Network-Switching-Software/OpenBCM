

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_STIF_H_

#define _DNX_DATA_STIF_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_stif_init(
    int unit);






typedef struct
{
    
    uint32 sram_buffers;
    
    uint32 sram_pdbs;
    
    uint32 dram_bdb;
} dnx_data_stif_config_scrubber_buffers_t;


typedef struct
{
    
    uint32 min;
    
    uint32 max;
} dnx_data_stif_config_selective_report_range_t;



typedef enum
{
    
    dnx_data_stif_config_stif_enable,
    
    dnx_data_stif_config_stif_report_mc_single_copy,
    
    dnx_data_stif_config_mixing_ports_limitation,
    
    dnx_data_stif_config_map_source_to_two_stif_ports,
    
    dnx_data_stif_config_only_billing_mode_support,
    
    dnx_data_stif_config_is_stif_record_updated,

    
    _dnx_data_stif_config_feature_nof
} dnx_data_stif_config_feature_e;



typedef int(
    *dnx_data_stif_config_feature_get_f) (
    int unit,
    dnx_data_stif_config_feature_e feature);


typedef uint32(
    *dnx_data_stif_config_sram_buffers_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_sram_pdbs_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_dram_bdb_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_packet_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_scrubber_queue_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_scrubber_queue_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_scrubber_rate_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_scrubber_rate_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_report_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_report_size_ingress_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_report_size_egress_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_stif_idle_report_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_config_nof_valid_sources_get_f) (
    int unit);


typedef const dnx_data_stif_config_scrubber_buffers_t *(
    *dnx_data_stif_config_scrubber_buffers_get_f) (
    int unit,
    int thresh_id);


typedef const dnx_data_stif_config_selective_report_range_t *(
    *dnx_data_stif_config_selective_report_range_get_f) (
    int unit,
    int range_id);



typedef struct
{
    
    dnx_data_stif_config_feature_get_f feature_get;
    
    dnx_data_stif_config_sram_buffers_resolution_get_f sram_buffers_resolution_get;
    
    dnx_data_stif_config_sram_pdbs_resolution_get_f sram_pdbs_resolution_get;
    
    dnx_data_stif_config_dram_bdb_resolution_get_f dram_bdb_resolution_get;
    
    dnx_data_stif_config_stif_packet_size_get_f stif_packet_size_get;
    
    dnx_data_stif_config_stif_scrubber_queue_min_get_f stif_scrubber_queue_min_get;
    
    dnx_data_stif_config_stif_scrubber_queue_max_get_f stif_scrubber_queue_max_get;
    
    dnx_data_stif_config_stif_scrubber_rate_min_get_f stif_scrubber_rate_min_get;
    
    dnx_data_stif_config_stif_scrubber_rate_max_get_f stif_scrubber_rate_max_get;
    
    dnx_data_stif_config_stif_report_mode_get_f stif_report_mode_get;
    
    dnx_data_stif_config_stif_report_size_ingress_get_f stif_report_size_ingress_get;
    
    dnx_data_stif_config_stif_report_size_egress_get_f stif_report_size_egress_get;
    
    dnx_data_stif_config_stif_idle_report_get_f stif_idle_report_get;
    
    dnx_data_stif_config_nof_valid_sources_get_f nof_valid_sources_get;
    
    dnx_data_stif_config_scrubber_buffers_get_f scrubber_buffers_get;
    
    dnxc_data_table_info_get_f scrubber_buffers_info_get;
    
    dnx_data_stif_config_selective_report_range_get_f selective_report_range_get;
    
    dnxc_data_table_info_get_f selective_report_range_info_get;
} dnx_data_if_stif_config_t;






typedef struct
{
    
    uint32 valid;
    
    uint32 hw_select_value;
} dnx_data_stif_port_ethu_id_stif_valid_t;



typedef enum
{

    
    _dnx_data_stif_port_feature_nof
} dnx_data_stif_port_feature_e;



typedef int(
    *dnx_data_stif_port_feature_get_f) (
    int unit,
    dnx_data_stif_port_feature_e feature);


typedef uint32(
    *dnx_data_stif_port_max_nof_instances_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_port_stif_ethu_select_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_port_stif_lane_mapping_size_get_f) (
    int unit);


typedef const dnx_data_stif_port_ethu_id_stif_valid_t *(
    *dnx_data_stif_port_ethu_id_stif_valid_get_f) (
    int unit,
    int ethu_id);



typedef struct
{
    
    dnx_data_stif_port_feature_get_f feature_get;
    
    dnx_data_stif_port_max_nof_instances_get_f max_nof_instances_get;
    
    dnx_data_stif_port_stif_ethu_select_size_get_f stif_ethu_select_size_get;
    
    dnx_data_stif_port_stif_lane_mapping_size_get_f stif_lane_mapping_size_get;
    
    dnx_data_stif_port_ethu_id_stif_valid_get_f ethu_id_stif_valid_get;
    
    dnxc_data_table_info_get_f ethu_id_stif_valid_info_get;
} dnx_data_if_stif_port_t;






typedef struct
{
    
    uint32 size;
} dnx_data_stif_report_etpp_metadata_t;


typedef struct
{
    
    uint32 size;
} dnx_data_stif_report_report_size_t;



typedef enum
{
    dnx_data_stif_report_etpp_core_in_portmetadata,
    
    dnx_data_stif_report_stif_record_is_last_copy,

    
    _dnx_data_stif_report_feature_nof
} dnx_data_stif_report_feature_e;



typedef int(
    *dnx_data_stif_report_feature_get_f) (
    int unit,
    dnx_data_stif_report_feature_e feature);


typedef uint32(
    *dnx_data_stif_report_qsize_idle_report_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_qsize_idle_report_period_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_qsize_idle_report_period_invalid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_qsize_scrubber_report_cycles_unit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_max_thresholds_per_buffer_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_incoming_tc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_record_two_msb_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_nof_source_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_stat_id_max_count_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_stat_object_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_format_element_packet_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_format_msb_opcode_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_format_etpp_metadata_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_opsize_use_one_type_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_opsize_use_two_types_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_opsize_use_three_types_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_opsize_use_four_types_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_report_billing_enq_ing_size_shift_get_f) (
    int unit);


typedef const dnx_data_stif_report_etpp_metadata_t *(
    *dnx_data_stif_report_etpp_metadata_get_f) (
    int unit,
    int element);


typedef const dnx_data_stif_report_report_size_t *(
    *dnx_data_stif_report_report_size_get_f) (
    int unit,
    int dbal_enum);



typedef struct
{
    
    dnx_data_stif_report_feature_get_f feature_get;
    
    dnx_data_stif_report_qsize_idle_report_size_get_f qsize_idle_report_size_get;
    
    dnx_data_stif_report_qsize_idle_report_period_get_f qsize_idle_report_period_get;
    
    dnx_data_stif_report_qsize_idle_report_period_invalid_get_f qsize_idle_report_period_invalid_get;
    
    dnx_data_stif_report_qsize_scrubber_report_cycles_unit_get_f qsize_scrubber_report_cycles_unit_get;
    
    dnx_data_stif_report_max_thresholds_per_buffer_get_f max_thresholds_per_buffer_get;
    
    dnx_data_stif_report_incoming_tc_get_f incoming_tc_get;
    
    dnx_data_stif_report_record_two_msb_get_f record_two_msb_get;
    
    dnx_data_stif_report_nof_source_types_get_f nof_source_types_get;
    
    dnx_data_stif_report_stat_id_max_count_get_f stat_id_max_count_get;
    
    dnx_data_stif_report_stat_object_size_get_f stat_object_size_get;
    
    dnx_data_stif_report_billing_format_element_packet_size_get_f billing_format_element_packet_size_get;
    
    dnx_data_stif_report_billing_format_msb_opcode_size_get_f billing_format_msb_opcode_size_get;
    
    dnx_data_stif_report_billing_format_etpp_metadata_size_get_f billing_format_etpp_metadata_size_get;
    
    dnx_data_stif_report_billing_opsize_use_one_type_mask_get_f billing_opsize_use_one_type_mask_get;
    
    dnx_data_stif_report_billing_opsize_use_two_types_mask_get_f billing_opsize_use_two_types_mask_get;
    
    dnx_data_stif_report_billing_opsize_use_three_types_mask_get_f billing_opsize_use_three_types_mask_get;
    
    dnx_data_stif_report_billing_opsize_use_four_types_mask_get_f billing_opsize_use_four_types_mask_get;
    
    dnx_data_stif_report_billing_enq_ing_size_shift_get_f billing_enq_ing_size_shift_get;
    
    dnx_data_stif_report_etpp_metadata_get_f etpp_metadata_get;
    
    dnxc_data_table_info_get_f etpp_metadata_info_get;
    
    dnx_data_stif_report_report_size_get_f report_size_get;
    
    dnxc_data_table_info_get_f report_size_info_get;
} dnx_data_if_stif_report_t;







typedef enum
{
    dnx_data_stif_flow_control_instance_0_must_be_on,

    
    _dnx_data_stif_flow_control_feature_nof
} dnx_data_stif_flow_control_feature_e;



typedef int(
    *dnx_data_stif_flow_control_feature_get_f) (
    int unit,
    dnx_data_stif_flow_control_feature_e feature);


typedef uint32(
    *dnx_data_stif_flow_control_etpp_pipe_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_etpp_deq_size_of_fifo_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_cgm_pipe_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_cgm_size_of_fifo_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_billing_level_sampling_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_fc_disable_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_billing_etpp_flow_control_high_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_billing_etpp_flow_control_low_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_billing_cgm_flow_control_high_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_billing_cgm_flow_control_low_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_qsize_cgm_flow_control_high_get_f) (
    int unit);


typedef uint32(
    *dnx_data_stif_flow_control_qsize_cgm_flow_control_low_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_stif_flow_control_feature_get_f feature_get;
    
    dnx_data_stif_flow_control_etpp_pipe_length_get_f etpp_pipe_length_get;
    
    dnx_data_stif_flow_control_etpp_deq_size_of_fifo_get_f etpp_deq_size_of_fifo_get;
    
    dnx_data_stif_flow_control_cgm_pipe_length_get_f cgm_pipe_length_get;
    
    dnx_data_stif_flow_control_cgm_size_of_fifo_get_f cgm_size_of_fifo_get;
    
    dnx_data_stif_flow_control_billing_level_sampling_get_f billing_level_sampling_get;
    
    dnx_data_stif_flow_control_fc_disable_value_get_f fc_disable_value_get;
    
    dnx_data_stif_flow_control_billing_etpp_flow_control_high_get_f billing_etpp_flow_control_high_get;
    
    dnx_data_stif_flow_control_billing_etpp_flow_control_low_get_f billing_etpp_flow_control_low_get;
    
    dnx_data_stif_flow_control_billing_cgm_flow_control_high_get_f billing_cgm_flow_control_high_get;
    
    dnx_data_stif_flow_control_billing_cgm_flow_control_low_get_f billing_cgm_flow_control_low_get;
    
    dnx_data_stif_flow_control_qsize_cgm_flow_control_high_get_f qsize_cgm_flow_control_high_get;
    
    dnx_data_stif_flow_control_qsize_cgm_flow_control_low_get_f qsize_cgm_flow_control_low_get;
} dnx_data_if_stif_flow_control_t;





typedef struct
{
    
    dnx_data_if_stif_config_t config;
    
    dnx_data_if_stif_port_t port;
    
    dnx_data_if_stif_report_t report;
    
    dnx_data_if_stif_flow_control_t flow_control;
} dnx_data_if_stif_t;




extern dnx_data_if_stif_t dnx_data_stif;


#endif 

