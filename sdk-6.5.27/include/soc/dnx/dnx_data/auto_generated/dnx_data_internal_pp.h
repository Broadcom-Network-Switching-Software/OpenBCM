
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PP_H_

#define _DNX_DATA_INTERNAL_PP_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_pp_submodule_stages,
    dnx_data_pp_submodule_PEM,
    dnx_data_pp_submodule_application,
    dnx_data_pp_submodule_ETPP,
    dnx_data_pp_submodule_debug,

    
    _dnx_data_pp_submodule_nof
} dnx_data_pp_submodule_e;








int dnx_data_pp_stages_feature_get(
    int unit,
    dnx_data_pp_stages_feature_e feature);



typedef enum
{
    dnx_data_pp_stages_define_is_vtt5_supported,
    dnx_data_pp_stages_define_is_erpp_supported,

    
    _dnx_data_pp_stages_define_nof
} dnx_data_pp_stages_define_e;



uint32 dnx_data_pp_stages_is_vtt5_supported_get(
    int unit);


uint32 dnx_data_pp_stages_is_erpp_supported_get(
    int unit);



typedef enum
{
    dnx_data_pp_stages_table_params,

    
    _dnx_data_pp_stages_table_nof
} dnx_data_pp_stages_table_e;



const dnx_data_pp_stages_params_t * dnx_data_pp_stages_params_get(
    int unit,
    int stage);



shr_error_e dnx_data_pp_stages_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pp_stages_params_info_get(
    int unit);






int dnx_data_pp_PEM_feature_get(
    int unit,
    dnx_data_pp_PEM_feature_e feature);



typedef enum
{
    dnx_data_pp_PEM_define_nof_pem_bits,
    dnx_data_pp_PEM_define_nof_pem_blocks_per_core,

    
    _dnx_data_pp_PEM_define_nof
} dnx_data_pp_PEM_define_e;



uint32 dnx_data_pp_PEM_nof_pem_bits_get(
    int unit);


uint32 dnx_data_pp_PEM_nof_pem_blocks_per_core_get(
    int unit);



typedef enum
{

    
    _dnx_data_pp_PEM_table_nof
} dnx_data_pp_PEM_table_e;









int dnx_data_pp_application_feature_get(
    int unit,
    dnx_data_pp_application_feature_e feature);



typedef enum
{
    dnx_data_pp_application_define_vtt1_lookup_by_exem1,

    
    _dnx_data_pp_application_define_nof
} dnx_data_pp_application_define_e;



uint32 dnx_data_pp_application_vtt1_lookup_by_exem1_get(
    int unit);



typedef enum
{
    dnx_data_pp_application_table_device_image,
    dnx_data_pp_application_table_ucode,

    
    _dnx_data_pp_application_table_nof
} dnx_data_pp_application_table_e;



const dnx_data_pp_application_device_image_t * dnx_data_pp_application_device_image_get(
    int unit);


const dnx_data_pp_application_ucode_t * dnx_data_pp_application_ucode_get(
    int unit);



shr_error_e dnx_data_pp_application_device_image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_pp_application_ucode_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pp_application_device_image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_pp_application_ucode_info_get(
    int unit);






int dnx_data_pp_ETPP_feature_get(
    int unit,
    dnx_data_pp_ETPP_feature_e feature);



typedef enum
{
    dnx_data_pp_ETPP_define_etps_type_size_bits,
    dnx_data_pp_ETPP_define_nof_additional_headers_profiles,
    dnx_data_pp_ETPP_define_nof_additional_headers_profiles_maps,
    dnx_data_pp_ETPP_define_dynamic_additional_headers_profile,
    dnx_data_pp_ETPP_define_ingress_packet_header_size,
    dnx_data_pp_ETPP_define_btc_max_header_to_add_in_bytes,

    
    _dnx_data_pp_ETPP_define_nof
} dnx_data_pp_ETPP_define_e;



uint32 dnx_data_pp_ETPP_etps_type_size_bits_get(
    int unit);


uint32 dnx_data_pp_ETPP_nof_additional_headers_profiles_get(
    int unit);


uint32 dnx_data_pp_ETPP_nof_additional_headers_profiles_maps_get(
    int unit);


uint32 dnx_data_pp_ETPP_dynamic_additional_headers_profile_get(
    int unit);


uint32 dnx_data_pp_ETPP_ingress_packet_header_size_get(
    int unit);


uint32 dnx_data_pp_ETPP_btc_max_header_to_add_in_bytes_get(
    int unit);



typedef enum
{

    
    _dnx_data_pp_ETPP_table_nof
} dnx_data_pp_ETPP_table_e;









int dnx_data_pp_debug_feature_get(
    int unit,
    dnx_data_pp_debug_feature_e feature);



typedef enum
{
    dnx_data_pp_debug_define_packet_header_data,
    dnx_data_pp_debug_define_ptc_size,
    dnx_data_pp_debug_define_valid_bytes,
    dnx_data_pp_debug_define_time_stamp,

    
    _dnx_data_pp_debug_define_nof
} dnx_data_pp_debug_define_e;



uint32 dnx_data_pp_debug_packet_header_data_get(
    int unit);


uint32 dnx_data_pp_debug_ptc_size_get(
    int unit);


uint32 dnx_data_pp_debug_valid_bytes_get(
    int unit);


uint32 dnx_data_pp_debug_time_stamp_get(
    int unit);



typedef enum
{
    dnx_data_pp_debug_table_from_signal_str,

    
    _dnx_data_pp_debug_table_nof
} dnx_data_pp_debug_table_e;



const dnx_data_pp_debug_from_signal_str_t * dnx_data_pp_debug_from_signal_str_get(
    int unit);



shr_error_e dnx_data_pp_debug_from_signal_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pp_debug_from_signal_str_info_get(
    int unit);



shr_error_e dnx_data_pp_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

