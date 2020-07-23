

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SAT_H_

#define _DNX_DATA_INTERNAL_SAT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_sat_submodule_general,
    dnx_data_sat_submodule_generator,
    dnx_data_sat_submodule_collector,
    dnx_data_sat_submodule_rx_flow_param,
    dnx_data_sat_submodule_tx_flow_param,

    
    _dnx_data_sat_submodule_nof
} dnx_data_sat_submodule_e;








int dnx_data_sat_general_feature_get(
    int unit,
    dnx_data_sat_general_feature_e feature);



typedef enum
{
    dnx_data_sat_general_define_granularity,

    
    _dnx_data_sat_general_define_nof
} dnx_data_sat_general_define_e;



uint32 dnx_data_sat_general_granularity_get(
    int unit);



typedef enum
{

    
    _dnx_data_sat_general_table_nof
} dnx_data_sat_general_table_e;









int dnx_data_sat_generator_feature_get(
    int unit,
    dnx_data_sat_generator_feature_e feature);



typedef enum
{
    dnx_data_sat_generator_define_nof_evc_ids,
    dnx_data_sat_generator_define_nof_gtf_ids,
    dnx_data_sat_generator_define_gtf_pkt_header_max_length,
    dnx_data_sat_generator_define_gtf_pkt_length_max_patn,
    dnx_data_sat_generator_define_gtf_max_seq_period,
    dnx_data_sat_generator_define_gtf_stamp_max_inc_step,
    dnx_data_sat_generator_define_gtf_stamp_max_inc_period,
    dnx_data_sat_generator_define_gtf_bw_max_value,
    dnx_data_sat_generator_define_gtf_rate_max,
    dnx_data_sat_generator_define_gtf_pps_rate_max,
    dnx_data_sat_generator_define_gtf_pkt_header_entries,
    dnx_data_sat_generator_define_gtf_pkt_header_length,
    dnx_data_sat_generator_define_gtf_pkt_len_patten_size,
    dnx_data_sat_generator_define_max_bit_rate_in_64B,
    dnx_data_sat_generator_define_max_bit_rate_in_128B,
    dnx_data_sat_generator_define_max_bit_rate_in_256B,
    dnx_data_sat_generator_define_max_bit_rate_in_512B,
    dnx_data_sat_generator_define_max_bit_rate_in_1024B,
    dnx_data_sat_generator_define_max_bit_rate_in_1280B,
    dnx_data_sat_generator_define_max_bit_rate_in_1518B,
    dnx_data_sat_generator_define_max_bit_rate_in_9000B,
    dnx_data_sat_generator_define_max_packet_rate_in_64B,
    dnx_data_sat_generator_define_max_packet_rate_in_128B,
    dnx_data_sat_generator_define_max_packet_rate_in_256B,
    dnx_data_sat_generator_define_max_packet_rate_in_512B,
    dnx_data_sat_generator_define_max_packet_rate_in_1024B,
    dnx_data_sat_generator_define_max_packet_rate_in_1280B,
    dnx_data_sat_generator_define_max_packet_rate_in_1518B,
    dnx_data_sat_generator_define_max_packet_rate_in_9000B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B,
    dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B,

    
    _dnx_data_sat_generator_define_nof
} dnx_data_sat_generator_define_e;



uint32 dnx_data_sat_generator_nof_evc_ids_get(
    int unit);


uint32 dnx_data_sat_generator_nof_gtf_ids_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_pkt_header_max_length_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_pkt_length_max_patn_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_max_seq_period_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_stamp_max_inc_step_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_stamp_max_inc_period_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_bw_max_value_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_rate_max_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_pps_rate_max_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_pkt_header_entries_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_pkt_header_length_get(
    int unit);


uint32 dnx_data_sat_generator_gtf_pkt_len_patten_size_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_64B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_128B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_256B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_512B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_1024B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_1280B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_1518B_get(
    int unit);


uint32 dnx_data_sat_generator_max_bit_rate_in_9000B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_64B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_128B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_256B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_512B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_1024B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_1280B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_1518B_get(
    int unit);


uint32 dnx_data_sat_generator_max_packet_rate_in_9000B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_get(
    int unit);


uint32 dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_get(
    int unit);



typedef enum
{

    
    _dnx_data_sat_generator_table_nof
} dnx_data_sat_generator_table_e;









int dnx_data_sat_collector_feature_get(
    int unit,
    dnx_data_sat_collector_feature_e feature);



typedef enum
{
    dnx_data_sat_collector_define_nof_ctf_ids,
    dnx_data_sat_collector_define_nof_oam_ids,
    dnx_data_sat_collector_define_mef_payload_offset,
    dnx_data_sat_collector_define_mef_seq_num_offset,
    dnx_data_sat_collector_define_mef_time_stamp_offset,
    dnx_data_sat_collector_define_mef_crc_byte_offset,
    dnx_data_sat_collector_define_y1731_payload_offset,
    dnx_data_sat_collector_define_y1731_seq_num_offset,
    dnx_data_sat_collector_define_y1731_time_stamp_offset,
    dnx_data_sat_collector_define_y1731_crc_byte_offset,
    dnx_data_sat_collector_define_ctf_bins_limit_entries,
    dnx_data_sat_collector_define_trap_id_entries,
    dnx_data_sat_collector_define_trap_data_entries,

    
    _dnx_data_sat_collector_define_nof
} dnx_data_sat_collector_define_e;



uint32 dnx_data_sat_collector_nof_ctf_ids_get(
    int unit);


uint32 dnx_data_sat_collector_nof_oam_ids_get(
    int unit);


uint32 dnx_data_sat_collector_mef_payload_offset_get(
    int unit);


uint32 dnx_data_sat_collector_mef_seq_num_offset_get(
    int unit);


uint32 dnx_data_sat_collector_mef_time_stamp_offset_get(
    int unit);


uint32 dnx_data_sat_collector_mef_crc_byte_offset_get(
    int unit);


uint32 dnx_data_sat_collector_y1731_payload_offset_get(
    int unit);


uint32 dnx_data_sat_collector_y1731_seq_num_offset_get(
    int unit);


uint32 dnx_data_sat_collector_y1731_time_stamp_offset_get(
    int unit);


uint32 dnx_data_sat_collector_y1731_crc_byte_offset_get(
    int unit);


uint32 dnx_data_sat_collector_ctf_bins_limit_entries_get(
    int unit);


uint32 dnx_data_sat_collector_trap_id_entries_get(
    int unit);


uint32 dnx_data_sat_collector_trap_data_entries_get(
    int unit);



typedef enum
{

    
    _dnx_data_sat_collector_table_nof
} dnx_data_sat_collector_table_e;









int dnx_data_sat_rx_flow_param_feature_get(
    int unit,
    dnx_data_sat_rx_flow_param_feature_e feature);



typedef enum
{

    
    _dnx_data_sat_rx_flow_param_define_nof
} dnx_data_sat_rx_flow_param_define_e;




typedef enum
{

    
    _dnx_data_sat_rx_flow_param_table_nof
} dnx_data_sat_rx_flow_param_table_e;









int dnx_data_sat_tx_flow_param_feature_get(
    int unit,
    dnx_data_sat_tx_flow_param_feature_e feature);



typedef enum
{

    
    _dnx_data_sat_tx_flow_param_define_nof
} dnx_data_sat_tx_flow_param_define_e;




typedef enum
{

    
    _dnx_data_sat_tx_flow_param_table_nof
} dnx_data_sat_tx_flow_param_table_e;






shr_error_e dnx_data_sat_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

