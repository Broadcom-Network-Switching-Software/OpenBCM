

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PORT_H_

#define _DNX_DATA_INTERNAL_PORT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_port_submodule_static_add,
    dnx_data_port_submodule_general,
    dnx_data_port_submodule_egress,
    dnx_data_port_submodule_imb,

    
    _dnx_data_port_submodule_nof
} dnx_data_port_submodule_e;








int dnx_data_port_static_add_feature_get(
    int unit,
    dnx_data_port_static_add_feature_e feature);



typedef enum
{
    dnx_data_port_static_add_define_default_speed_for_special_if,
    dnx_data_port_static_add_define_fabric_fw_load_method,
    dnx_data_port_static_add_define_fabric_fw_crc_check,
    dnx_data_port_static_add_define_fabric_fw_load_verify,
    dnx_data_port_static_add_define_nif_fw_load_method,
    dnx_data_port_static_add_define_nif_fw_crc_check,
    dnx_data_port_static_add_define_nif_fw_load_verify,

    
    _dnx_data_port_static_add_define_nof
} dnx_data_port_static_add_define_e;



uint32 dnx_data_port_static_add_default_speed_for_special_if_get(
    int unit);


uint32 dnx_data_port_static_add_fabric_fw_load_method_get(
    int unit);


uint32 dnx_data_port_static_add_fabric_fw_crc_check_get(
    int unit);


uint32 dnx_data_port_static_add_fabric_fw_load_verify_get(
    int unit);


uint32 dnx_data_port_static_add_nif_fw_load_method_get(
    int unit);


uint32 dnx_data_port_static_add_nif_fw_crc_check_get(
    int unit);


uint32 dnx_data_port_static_add_nif_fw_load_verify_get(
    int unit);



typedef enum
{
    dnx_data_port_static_add_table_ucode_port,
    dnx_data_port_static_add_table_speed,
    dnx_data_port_static_add_table_ext_stat_speed,
    dnx_data_port_static_add_table_eth_padding,
    dnx_data_port_static_add_table_link_training,
    dnx_data_port_static_add_table_ext_stat_link_training,
    dnx_data_port_static_add_table_fec_type,
    dnx_data_port_static_add_table_serdes_lane_config,
    dnx_data_port_static_add_table_serdes_tx_taps,
    dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps,
    dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps,
    dnx_data_port_static_add_table_tx_pam4_precoder,
    dnx_data_port_static_add_table_lp_tx_precoder,
    dnx_data_port_static_add_table_fabric_quad_info,
    dnx_data_port_static_add_table_header_type,
    dnx_data_port_static_add_table_erp_exist,
    dnx_data_port_static_add_table_base_flexe_instance,

    
    _dnx_data_port_static_add_table_nof
} dnx_data_port_static_add_table_e;



const dnx_data_port_static_add_ucode_port_t * dnx_data_port_static_add_ucode_port_get(
    int unit,
    int port);


const dnx_data_port_static_add_speed_t * dnx_data_port_static_add_speed_get(
    int unit,
    int port);


const dnx_data_port_static_add_ext_stat_speed_t * dnx_data_port_static_add_ext_stat_speed_get(
    int unit,
    int ext_stat_port);


const dnx_data_port_static_add_eth_padding_t * dnx_data_port_static_add_eth_padding_get(
    int unit,
    int port);


const dnx_data_port_static_add_link_training_t * dnx_data_port_static_add_link_training_get(
    int unit,
    int port);


const dnx_data_port_static_add_ext_stat_link_training_t * dnx_data_port_static_add_ext_stat_link_training_get(
    int unit,
    int ext_stat_port);


const dnx_data_port_static_add_fec_type_t * dnx_data_port_static_add_fec_type_get(
    int unit,
    int port);


const dnx_data_port_static_add_serdes_lane_config_t * dnx_data_port_static_add_serdes_lane_config_get(
    int unit,
    int port);


const dnx_data_port_static_add_serdes_tx_taps_t * dnx_data_port_static_add_serdes_tx_taps_get(
    int unit,
    int port);


const dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t * dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_get(
    int unit);


const dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t * dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_get(
    int unit,
    int lane);


const dnx_data_port_static_add_tx_pam4_precoder_t * dnx_data_port_static_add_tx_pam4_precoder_get(
    int unit,
    int port);


const dnx_data_port_static_add_lp_tx_precoder_t * dnx_data_port_static_add_lp_tx_precoder_get(
    int unit,
    int port);


const dnx_data_port_static_add_fabric_quad_info_t * dnx_data_port_static_add_fabric_quad_info_get(
    int unit,
    int quad);


const dnx_data_port_static_add_header_type_t * dnx_data_port_static_add_header_type_get(
    int unit,
    int port);


const dnx_data_port_static_add_erp_exist_t * dnx_data_port_static_add_erp_exist_get(
    int unit,
    int core);


const dnx_data_port_static_add_base_flexe_instance_t * dnx_data_port_static_add_base_flexe_instance_get(
    int unit,
    int port);



shr_error_e dnx_data_port_static_add_ucode_port_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_speed_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_ext_stat_speed_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_eth_padding_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_link_training_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_ext_stat_link_training_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_fec_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_serdes_lane_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_tx_pam4_precoder_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_lp_tx_precoder_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_fabric_quad_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_header_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_erp_exist_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_static_add_base_flexe_instance_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_port_static_add_ucode_port_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_speed_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_ext_stat_speed_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_eth_padding_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_link_training_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_ext_stat_link_training_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_fec_type_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_serdes_lane_config_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_serdes_tx_taps_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_tx_pam4_precoder_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_lp_tx_precoder_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_fabric_quad_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_header_type_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_erp_exist_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_static_add_base_flexe_instance_info_get(
    int unit);






int dnx_data_port_general_feature_get(
    int unit,
    dnx_data_port_general_feature_e feature);



typedef enum
{
    dnx_data_port_general_define_erp_tm_port,
    dnx_data_port_general_define_max_nof_channels,
    dnx_data_port_general_define_fabric_phys_offset,
    dnx_data_port_general_define_pp_dsp_size,
    dnx_data_port_general_define_ingress_vlan_domain_size,
    dnx_data_port_general_define_vlan_domain_size,
    dnx_data_port_general_define_ffc_instruction_size,
    dnx_data_port_general_define_max_first_header_size_to_skip,
    dnx_data_port_general_define_prt_ffc_width_start_offset,
    dnx_data_port_general_define_prt_ffc_start_input_offset,
    dnx_data_port_general_define_nof_tm_ports,
    dnx_data_port_general_define_nof_tm_ports_per_fap_id,
    dnx_data_port_general_define_nof_pp_ports,
    dnx_data_port_general_define_reserved_tm_port,
    dnx_data_port_general_define_nof_vlan_membership_if,
    dnx_data_port_general_define_max_vlan_membership_if,
    dnx_data_port_general_define_fabric_port_base,
    dnx_data_port_general_define_pp_port_bits_size,
    dnx_data_port_general_define_nof_port_bits_in_pp_bus,
    dnx_data_port_general_define_prt_configuration_ptch2_default_index,
    dnx_data_port_general_define_prt_configuration_ptch1_default_index,
    dnx_data_port_general_define_prt_configuration_eth_default_index,

    
    _dnx_data_port_general_define_nof
} dnx_data_port_general_define_e;



uint32 dnx_data_port_general_erp_tm_port_get(
    int unit);


uint32 dnx_data_port_general_max_nof_channels_get(
    int unit);


uint32 dnx_data_port_general_fabric_phys_offset_get(
    int unit);


uint32 dnx_data_port_general_pp_dsp_size_get(
    int unit);


uint32 dnx_data_port_general_ingress_vlan_domain_size_get(
    int unit);


uint32 dnx_data_port_general_vlan_domain_size_get(
    int unit);


uint32 dnx_data_port_general_ffc_instruction_size_get(
    int unit);


uint32 dnx_data_port_general_max_first_header_size_to_skip_get(
    int unit);


uint32 dnx_data_port_general_prt_ffc_width_start_offset_get(
    int unit);


uint32 dnx_data_port_general_prt_ffc_start_input_offset_get(
    int unit);


uint32 dnx_data_port_general_nof_tm_ports_get(
    int unit);


uint32 dnx_data_port_general_nof_tm_ports_per_fap_id_get(
    int unit);


uint32 dnx_data_port_general_nof_pp_ports_get(
    int unit);


uint32 dnx_data_port_general_reserved_tm_port_get(
    int unit);


uint32 dnx_data_port_general_nof_vlan_membership_if_get(
    int unit);


uint32 dnx_data_port_general_max_vlan_membership_if_get(
    int unit);


uint32 dnx_data_port_general_fabric_port_base_get(
    int unit);


uint32 dnx_data_port_general_pp_port_bits_size_get(
    int unit);


uint32 dnx_data_port_general_nof_port_bits_in_pp_bus_get(
    int unit);


uint32 dnx_data_port_general_prt_configuration_ptch2_default_index_get(
    int unit);


uint32 dnx_data_port_general_prt_configuration_ptch1_default_index_get(
    int unit);


uint32 dnx_data_port_general_prt_configuration_eth_default_index_get(
    int unit);



typedef enum
{
    dnx_data_port_general_table_prt_configuration,
    dnx_data_port_general_table_first_header_size,

    
    _dnx_data_port_general_table_nof
} dnx_data_port_general_table_e;



const dnx_data_port_general_prt_configuration_t * dnx_data_port_general_prt_configuration_get(
    int unit,
    int index);


const dnx_data_port_general_first_header_size_t * dnx_data_port_general_first_header_size_get(
    int unit,
    int port);



shr_error_e dnx_data_port_general_prt_configuration_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_port_general_first_header_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_port_general_prt_configuration_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_port_general_first_header_size_info_get(
    int unit);






int dnx_data_port_egress_feature_get(
    int unit,
    dnx_data_port_egress_feature_e feature);



typedef enum
{
    dnx_data_port_egress_define_nof_ifs,

    
    _dnx_data_port_egress_define_nof
} dnx_data_port_egress_define_e;



uint32 dnx_data_port_egress_nof_ifs_get(
    int unit);



typedef enum
{

    
    _dnx_data_port_egress_table_nof
} dnx_data_port_egress_table_e;









int dnx_data_port_imb_feature_get(
    int unit,
    dnx_data_port_imb_feature_e feature);



typedef enum
{

    
    _dnx_data_port_imb_define_nof
} dnx_data_port_imb_define_e;




typedef enum
{
    dnx_data_port_imb_table_imb_type_info,

    
    _dnx_data_port_imb_table_nof
} dnx_data_port_imb_table_e;



const dnx_data_port_imb_imb_type_info_t * dnx_data_port_imb_imb_type_info_get(
    int unit,
    int type);



shr_error_e dnx_data_port_imb_imb_type_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_port_imb_imb_type_info_info_get(
    int unit);



shr_error_e dnx_data_port_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

