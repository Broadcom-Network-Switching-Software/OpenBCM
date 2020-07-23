

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SAT_H_

#define _DNX_DATA_SAT_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sat.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_sat_init(
    int unit);







typedef enum
{
    
    dnx_data_sat_general_is_sat_supported,

    
    _dnx_data_sat_general_feature_nof
} dnx_data_sat_general_feature_e;



typedef int(
    *dnx_data_sat_general_feature_get_f) (
    int unit,
    dnx_data_sat_general_feature_e feature);


typedef uint32(
    *dnx_data_sat_general_granularity_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sat_general_feature_get_f feature_get;
    
    dnx_data_sat_general_granularity_get_f granularity_get;
} dnx_data_if_sat_general_t;







typedef enum
{

    
    _dnx_data_sat_generator_feature_nof
} dnx_data_sat_generator_feature_e;



typedef int(
    *dnx_data_sat_generator_feature_get_f) (
    int unit,
    dnx_data_sat_generator_feature_e feature);


typedef uint32(
    *dnx_data_sat_generator_nof_evc_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_nof_gtf_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_pkt_header_max_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_pkt_length_max_patn_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_max_seq_period_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_stamp_max_inc_step_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_stamp_max_inc_period_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_bw_max_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_rate_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_pps_rate_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_pkt_header_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_pkt_header_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_gtf_pkt_len_patten_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_64B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_128B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_256B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_512B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_1024B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_1280B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_1518B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_bit_rate_in_9000B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_64B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_128B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_256B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_512B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_1024B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_1280B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_1518B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_packet_rate_in_9000B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sat_generator_feature_get_f feature_get;
    
    dnx_data_sat_generator_nof_evc_ids_get_f nof_evc_ids_get;
    
    dnx_data_sat_generator_nof_gtf_ids_get_f nof_gtf_ids_get;
    
    dnx_data_sat_generator_gtf_pkt_header_max_length_get_f gtf_pkt_header_max_length_get;
    
    dnx_data_sat_generator_gtf_pkt_length_max_patn_get_f gtf_pkt_length_max_patn_get;
    
    dnx_data_sat_generator_gtf_max_seq_period_get_f gtf_max_seq_period_get;
    
    dnx_data_sat_generator_gtf_stamp_max_inc_step_get_f gtf_stamp_max_inc_step_get;
    
    dnx_data_sat_generator_gtf_stamp_max_inc_period_get_f gtf_stamp_max_inc_period_get;
    
    dnx_data_sat_generator_gtf_bw_max_value_get_f gtf_bw_max_value_get;
    
    dnx_data_sat_generator_gtf_rate_max_get_f gtf_rate_max_get;
    
    dnx_data_sat_generator_gtf_pps_rate_max_get_f gtf_pps_rate_max_get;
    
    dnx_data_sat_generator_gtf_pkt_header_entries_get_f gtf_pkt_header_entries_get;
    
    dnx_data_sat_generator_gtf_pkt_header_length_get_f gtf_pkt_header_length_get;
    
    dnx_data_sat_generator_gtf_pkt_len_patten_size_get_f gtf_pkt_len_patten_size_get;
    
    dnx_data_sat_generator_max_bit_rate_in_64B_get_f max_bit_rate_in_64B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_128B_get_f max_bit_rate_in_128B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_256B_get_f max_bit_rate_in_256B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_512B_get_f max_bit_rate_in_512B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_1024B_get_f max_bit_rate_in_1024B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_1280B_get_f max_bit_rate_in_1280B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_1518B_get_f max_bit_rate_in_1518B_get;
    
    dnx_data_sat_generator_max_bit_rate_in_9000B_get_f max_bit_rate_in_9000B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_64B_get_f max_packet_rate_in_64B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_128B_get_f max_packet_rate_in_128B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_256B_get_f max_packet_rate_in_256B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_512B_get_f max_packet_rate_in_512B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_1024B_get_f max_packet_rate_in_1024B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_1280B_get_f max_packet_rate_in_1280B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_1518B_get_f max_packet_rate_in_1518B_get;
    
    dnx_data_sat_generator_max_packet_rate_in_9000B_get_f max_packet_rate_in_9000B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_get_f max_ctf_recv_packet_rate_in_64B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_get_f max_ctf_recv_packet_rate_in_128B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_get_f max_ctf_recv_packet_rate_in_256B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_get_f max_ctf_recv_packet_rate_in_512B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_get_f max_ctf_recv_packet_rate_in_1024B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_get_f max_ctf_recv_packet_rate_in_1280B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_get_f max_ctf_recv_packet_rate_in_1518B_get;
    
    dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_get_f max_ctf_recv_packet_rate_in_9000B_get;
} dnx_data_if_sat_generator_t;







typedef enum
{

    
    _dnx_data_sat_collector_feature_nof
} dnx_data_sat_collector_feature_e;



typedef int(
    *dnx_data_sat_collector_feature_get_f) (
    int unit,
    dnx_data_sat_collector_feature_e feature);


typedef uint32(
    *dnx_data_sat_collector_nof_ctf_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_nof_oam_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_mef_payload_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_mef_seq_num_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_mef_time_stamp_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_mef_crc_byte_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_y1731_payload_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_y1731_seq_num_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_y1731_time_stamp_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_y1731_crc_byte_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_ctf_bins_limit_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_trap_id_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_sat_collector_trap_data_entries_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_sat_collector_feature_get_f feature_get;
    
    dnx_data_sat_collector_nof_ctf_ids_get_f nof_ctf_ids_get;
    
    dnx_data_sat_collector_nof_oam_ids_get_f nof_oam_ids_get;
    
    dnx_data_sat_collector_mef_payload_offset_get_f mef_payload_offset_get;
    
    dnx_data_sat_collector_mef_seq_num_offset_get_f mef_seq_num_offset_get;
    
    dnx_data_sat_collector_mef_time_stamp_offset_get_f mef_time_stamp_offset_get;
    
    dnx_data_sat_collector_mef_crc_byte_offset_get_f mef_crc_byte_offset_get;
    
    dnx_data_sat_collector_y1731_payload_offset_get_f y1731_payload_offset_get;
    
    dnx_data_sat_collector_y1731_seq_num_offset_get_f y1731_seq_num_offset_get;
    
    dnx_data_sat_collector_y1731_time_stamp_offset_get_f y1731_time_stamp_offset_get;
    
    dnx_data_sat_collector_y1731_crc_byte_offset_get_f y1731_crc_byte_offset_get;
    
    dnx_data_sat_collector_ctf_bins_limit_entries_get_f ctf_bins_limit_entries_get;
    
    dnx_data_sat_collector_trap_id_entries_get_f trap_id_entries_get;
    
    dnx_data_sat_collector_trap_data_entries_get_f trap_data_entries_get;
} dnx_data_if_sat_collector_t;







typedef enum
{
    
    dnx_data_sat_rx_flow_param_natural_id,
    
    dnx_data_sat_rx_flow_param_odd_even_id,
    
    dnx_data_sat_rx_flow_param_twamp_mode,
    
    dnx_data_sat_rx_flow_param_twamp_rx_time_stamp,

    
    _dnx_data_sat_rx_flow_param_feature_nof
} dnx_data_sat_rx_flow_param_feature_e;



typedef int(
    *dnx_data_sat_rx_flow_param_feature_get_f) (
    int unit,
    dnx_data_sat_rx_flow_param_feature_e feature);



typedef struct
{
    
    dnx_data_sat_rx_flow_param_feature_get_f feature_get;
} dnx_data_if_sat_rx_flow_param_t;







typedef enum
{
    
    dnx_data_sat_tx_flow_param_seq_number_wrap_around,

    
    _dnx_data_sat_tx_flow_param_feature_nof
} dnx_data_sat_tx_flow_param_feature_e;



typedef int(
    *dnx_data_sat_tx_flow_param_feature_get_f) (
    int unit,
    dnx_data_sat_tx_flow_param_feature_e feature);



typedef struct
{
    
    dnx_data_sat_tx_flow_param_feature_get_f feature_get;
} dnx_data_if_sat_tx_flow_param_t;





typedef struct
{
    
    dnx_data_if_sat_general_t general;
    
    dnx_data_if_sat_generator_t generator;
    
    dnx_data_if_sat_collector_t collector;
    
    dnx_data_if_sat_rx_flow_param_t rx_flow_param;
    
    dnx_data_if_sat_tx_flow_param_t tx_flow_param;
} dnx_data_if_sat_t;




extern dnx_data_if_sat_t dnx_data_sat;


#endif 

