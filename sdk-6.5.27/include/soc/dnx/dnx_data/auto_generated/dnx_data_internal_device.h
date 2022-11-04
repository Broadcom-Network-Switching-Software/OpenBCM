
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DEVICE_H_

#define _DNX_DATA_INTERNAL_DEVICE_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_device_submodule_general,
    dnx_data_device_submodule_regression,
    dnx_data_device_submodule_emulation,
    dnx_data_device_submodule_otp,
    dnx_data_device_submodule_docs,

    
    _dnx_data_device_submodule_nof
} dnx_data_device_submodule_e;








int dnx_data_device_general_feature_get(
    int unit,
    dnx_data_device_general_feature_e feature);



typedef enum
{
    dnx_data_device_general_define_max_nof_system_ports,
    dnx_data_device_general_define_invalid_system_port,
    dnx_data_device_general_define_fmq_system_port,
    dnx_data_device_general_define_invalid_fap_id,
    dnx_data_device_general_define_max_nof_faps,
    dnx_data_device_general_define_fap_ids_per_core_bits,
    dnx_data_device_general_define_nof_cores,
    dnx_data_device_general_define_valid_cores_bitmap,
    dnx_data_device_general_define_nof_cores_for_traffic,
    dnx_data_device_general_define_max_nof_layer_protocols,
    dnx_data_device_general_define_nof_ipcs,
    dnx_data_device_general_define_nof_core_groups,
    dnx_data_device_general_define_nof_cores_per_core_group,
    dnx_data_device_general_define_nof_avs_status_bits,
    dnx_data_device_general_define_core_max_nof_bits,
    dnx_data_device_general_define_max_nof_fap_ids_per_core,
    dnx_data_device_general_define_max_nof_fap_ids_per_device,
    dnx_data_device_general_define_core_clock_khz,
    dnx_data_device_general_define_system_ref_core_clock_khz,
    dnx_data_device_general_define_ref_core_clock_mhz,
    dnx_data_device_general_define_core_clock_ndiv_size,
    dnx_data_device_general_define_core_clock_mdiv_size,
    dnx_data_device_general_define_core_clock_ndiv_offset,
    dnx_data_device_general_define_core_clock_mdiv_offset,
    dnx_data_device_general_define_bus_size_in_bits,
    dnx_data_device_general_define_maximal_core_bandwidth_mbps,
    dnx_data_device_general_define_enforce_maximal_core_bandwidth,
    dnx_data_device_general_define_max_mpps,
    dnx_data_device_general_define_packet_per_clock,
    dnx_data_device_general_define_nof_faps,
    dnx_data_device_general_define_device_id,
    dnx_data_device_general_define_nof_sku_bits,
    dnx_data_device_general_define_bist_enable,
    dnx_data_device_general_define_nof_bits_for_nof_cores,

    
    _dnx_data_device_general_define_nof
} dnx_data_device_general_define_e;



uint32 dnx_data_device_general_max_nof_system_ports_get(
    int unit);


uint32 dnx_data_device_general_invalid_system_port_get(
    int unit);


uint32 dnx_data_device_general_fmq_system_port_get(
    int unit);


uint32 dnx_data_device_general_invalid_fap_id_get(
    int unit);


uint32 dnx_data_device_general_max_nof_faps_get(
    int unit);


uint32 dnx_data_device_general_fap_ids_per_core_bits_get(
    int unit);


uint32 dnx_data_device_general_nof_cores_get(
    int unit);


uint32 dnx_data_device_general_valid_cores_bitmap_get(
    int unit);


uint32 dnx_data_device_general_nof_cores_for_traffic_get(
    int unit);


uint32 dnx_data_device_general_max_nof_layer_protocols_get(
    int unit);


uint32 dnx_data_device_general_nof_ipcs_get(
    int unit);


uint32 dnx_data_device_general_nof_core_groups_get(
    int unit);


uint32 dnx_data_device_general_nof_cores_per_core_group_get(
    int unit);


uint32 dnx_data_device_general_nof_avs_status_bits_get(
    int unit);


uint32 dnx_data_device_general_core_max_nof_bits_get(
    int unit);


uint32 dnx_data_device_general_max_nof_fap_ids_per_core_get(
    int unit);


uint32 dnx_data_device_general_max_nof_fap_ids_per_device_get(
    int unit);


uint32 dnx_data_device_general_core_clock_khz_get(
    int unit);


uint32 dnx_data_device_general_system_ref_core_clock_khz_get(
    int unit);


uint32 dnx_data_device_general_ref_core_clock_mhz_get(
    int unit);


uint32 dnx_data_device_general_core_clock_ndiv_size_get(
    int unit);


uint32 dnx_data_device_general_core_clock_mdiv_size_get(
    int unit);


uint32 dnx_data_device_general_core_clock_ndiv_offset_get(
    int unit);


uint32 dnx_data_device_general_core_clock_mdiv_offset_get(
    int unit);


uint32 dnx_data_device_general_bus_size_in_bits_get(
    int unit);


uint32 dnx_data_device_general_maximal_core_bandwidth_mbps_get(
    int unit);


uint32 dnx_data_device_general_enforce_maximal_core_bandwidth_get(
    int unit);


uint32 dnx_data_device_general_max_mpps_get(
    int unit);


uint32 dnx_data_device_general_packet_per_clock_get(
    int unit);


uint32 dnx_data_device_general_nof_faps_get(
    int unit);


uint32 dnx_data_device_general_device_id_get(
    int unit);


uint32 dnx_data_device_general_nof_sku_bits_get(
    int unit);


uint32 dnx_data_device_general_bist_enable_get(
    int unit);


uint32 dnx_data_device_general_nof_bits_for_nof_cores_get(
    int unit);



typedef enum
{
    dnx_data_device_general_table_ecc_err_masking,
    dnx_data_device_general_table_ctest_full_file_indentifier,

    
    _dnx_data_device_general_table_nof
} dnx_data_device_general_table_e;



const dnx_data_device_general_ecc_err_masking_t * dnx_data_device_general_ecc_err_masking_get(
    int unit,
    int block_type);


const dnx_data_device_general_ctest_full_file_indentifier_t * dnx_data_device_general_ctest_full_file_indentifier_get(
    int unit);



shr_error_e dnx_data_device_general_ecc_err_masking_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_device_general_ctest_full_file_indentifier_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_device_general_ecc_err_masking_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_device_general_ctest_full_file_indentifier_info_get(
    int unit);






int dnx_data_device_regression_feature_get(
    int unit,
    dnx_data_device_regression_feature_e feature);



typedef enum
{

    
    _dnx_data_device_regression_define_nof
} dnx_data_device_regression_define_e;




typedef enum
{
    dnx_data_device_regression_table_regression_parms,

    
    _dnx_data_device_regression_table_nof
} dnx_data_device_regression_table_e;



const dnx_data_device_regression_regression_parms_t * dnx_data_device_regression_regression_parms_get(
    int unit);



shr_error_e dnx_data_device_regression_regression_parms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_device_regression_regression_parms_info_get(
    int unit);






int dnx_data_device_emulation_feature_get(
    int unit,
    dnx_data_device_emulation_feature_e feature);



typedef enum
{
    dnx_data_device_emulation_define_emulation_system,
    dnx_data_device_emulation_define_framer_init_mode,

    
    _dnx_data_device_emulation_define_nof
} dnx_data_device_emulation_define_e;



uint32 dnx_data_device_emulation_emulation_system_get(
    int unit);


uint32 dnx_data_device_emulation_framer_init_mode_get(
    int unit);



typedef enum
{

    
    _dnx_data_device_emulation_table_nof
} dnx_data_device_emulation_table_e;









int dnx_data_device_otp_feature_get(
    int unit,
    dnx_data_device_otp_feature_e feature);



typedef enum
{
    dnx_data_device_otp_define_nof_words_in_row,
    dnx_data_device_otp_define_read_timeout,

    
    _dnx_data_device_otp_define_nof
} dnx_data_device_otp_define_e;



uint32 dnx_data_device_otp_nof_words_in_row_get(
    int unit);


uint32 dnx_data_device_otp_read_timeout_get(
    int unit);



typedef enum
{
    dnx_data_device_otp_table_otp_instance_attributes,
    dnx_data_device_otp_table_reg_addresses,

    
    _dnx_data_device_otp_table_nof
} dnx_data_device_otp_table_e;



const dnx_data_device_otp_otp_instance_attributes_t * dnx_data_device_otp_otp_instance_attributes_get(
    int unit,
    int otp_instance_index);


const dnx_data_device_otp_reg_addresses_t * dnx_data_device_otp_reg_addresses_get(
    int unit);



shr_error_e dnx_data_device_otp_otp_instance_attributes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_device_otp_reg_addresses_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_device_otp_otp_instance_attributes_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_device_otp_reg_addresses_info_get(
    int unit);






int dnx_data_device_docs_feature_get(
    int unit,
    dnx_data_device_docs_feature_e feature);



typedef enum
{

    
    _dnx_data_device_docs_define_nof
} dnx_data_device_docs_define_e;




typedef enum
{

    
    _dnx_data_device_docs_table_nof
} dnx_data_device_docs_table_e;






shr_error_e dnx_data_device_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

