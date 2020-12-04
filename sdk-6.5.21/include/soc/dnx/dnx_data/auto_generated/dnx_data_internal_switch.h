

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SWITCH_H_

#define _DNX_DATA_INTERNAL_SWITCH_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_switch_submodule_load_balancing,
    dnx_data_switch_submodule_svtag,
    dnx_data_switch_submodule_feature,

    
    _dnx_data_switch_submodule_nof
} dnx_data_switch_submodule_e;








int dnx_data_switch_load_balancing_feature_get(
    int unit,
    dnx_data_switch_load_balancing_feature_e feature);



typedef enum
{
    dnx_data_switch_load_balancing_define_nof_lb_clients,
    dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries,
    dnx_data_switch_load_balancing_define_nof_crc_functions,
    dnx_data_switch_load_balancing_define_initial_reserved_label,
    dnx_data_switch_load_balancing_define_initial_reserved_label_force,
    dnx_data_switch_load_balancing_define_initial_reserved_next_label,
    dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid,
    dnx_data_switch_load_balancing_define_reserved_next_label_valid,
    dnx_data_switch_load_balancing_define_num_valid_mpls_protocols,
    dnx_data_switch_load_balancing_define_nof_layer_records_from_parser,
    dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function,
    dnx_data_switch_load_balancing_define_hashing_selection_per_layer,
    dnx_data_switch_load_balancing_define_mpls_split_stack_config,
    dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists,

    
    _dnx_data_switch_load_balancing_define_nof
} dnx_data_switch_load_balancing_define_e;



uint32 dnx_data_switch_load_balancing_nof_lb_clients_get(
    int unit);


uint32 dnx_data_switch_load_balancing_nof_lb_crc_sel_tcam_entries_get(
    int unit);


uint32 dnx_data_switch_load_balancing_nof_crc_functions_get(
    int unit);


uint32 dnx_data_switch_load_balancing_initial_reserved_label_get(
    int unit);


uint32 dnx_data_switch_load_balancing_initial_reserved_label_force_get(
    int unit);


uint32 dnx_data_switch_load_balancing_initial_reserved_next_label_get(
    int unit);


uint32 dnx_data_switch_load_balancing_initial_reserved_next_label_valid_get(
    int unit);


uint32 dnx_data_switch_load_balancing_reserved_next_label_valid_get(
    int unit);


uint32 dnx_data_switch_load_balancing_num_valid_mpls_protocols_get(
    int unit);


uint32 dnx_data_switch_load_balancing_nof_layer_records_from_parser_get(
    int unit);


uint32 dnx_data_switch_load_balancing_nof_seeds_per_crc_function_get(
    int unit);


uint32 dnx_data_switch_load_balancing_hashing_selection_per_layer_get(
    int unit);


uint32 dnx_data_switch_load_balancing_mpls_split_stack_config_get(
    int unit);


uint32 dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_get(
    int unit);



typedef enum
{
    dnx_data_switch_load_balancing_table_lb_clients,
    dnx_data_switch_load_balancing_table_lb_client_crc,
    dnx_data_switch_load_balancing_table_lb_field_enablers,

    
    _dnx_data_switch_load_balancing_table_nof
} dnx_data_switch_load_balancing_table_e;



const dnx_data_switch_load_balancing_lb_clients_t * dnx_data_switch_load_balancing_lb_clients_get(
    int unit,
    int client_hw_id);


const dnx_data_switch_load_balancing_lb_client_crc_t * dnx_data_switch_load_balancing_lb_client_crc_get(
    int unit,
    int client_hw_id);


const dnx_data_switch_load_balancing_lb_field_enablers_t * dnx_data_switch_load_balancing_lb_field_enablers_get(
    int unit,
    int header_id);



shr_error_e dnx_data_switch_load_balancing_lb_clients_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_switch_load_balancing_lb_client_crc_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_switch_load_balancing_lb_field_enablers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_switch_load_balancing_lb_clients_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_switch_load_balancing_lb_client_crc_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_switch_load_balancing_lb_field_enablers_info_get(
    int unit);






int dnx_data_switch_svtag_feature_get(
    int unit,
    dnx_data_switch_svtag_feature_e feature);



typedef enum
{
    dnx_data_switch_svtag_define_supported,
    dnx_data_switch_svtag_define_svtag_label_size_bits,
    dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits,
    dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits,
    dnx_data_switch_svtag_define_egress_svtag_signature_size_bits,
    dnx_data_switch_svtag_define_egress_svtag_signature_position_bits,
    dnx_data_switch_svtag_define_egress_svtag_sci_size_bits,
    dnx_data_switch_svtag_define_egress_svtag_sci_position_bits,
    dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits,
    dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits,
    dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits,
    dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits,
    dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits,
    dnx_data_switch_svtag_define_UDP_dedicated_port,
    dnx_data_switch_svtag_define_ingress_svtag_position_bytes,
    dnx_data_switch_svtag_define_enable_by_defualt,

    
    _dnx_data_switch_svtag_define_nof
} dnx_data_switch_svtag_define_e;



uint32 dnx_data_switch_svtag_supported_get(
    int unit);


uint32 dnx_data_switch_svtag_svtag_label_size_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_offset_addr_size_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_offset_addr_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_signature_size_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_signature_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_sci_size_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_sci_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_pkt_type_size_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_pkt_type_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_ipv6_indication_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_hw_field_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_egress_svtag_accumulation_indication_hw_field_position_bits_get(
    int unit);


uint32 dnx_data_switch_svtag_UDP_dedicated_port_get(
    int unit);


uint32 dnx_data_switch_svtag_ingress_svtag_position_bytes_get(
    int unit);


uint32 dnx_data_switch_svtag_enable_by_defualt_get(
    int unit);



typedef enum
{

    
    _dnx_data_switch_svtag_table_nof
} dnx_data_switch_svtag_table_e;









int dnx_data_switch_feature_feature_get(
    int unit,
    dnx_data_switch_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_switch_feature_define_nof
} dnx_data_switch_feature_define_e;




typedef enum
{

    
    _dnx_data_switch_feature_table_nof
} dnx_data_switch_feature_table_e;






shr_error_e dnx_data_switch_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

