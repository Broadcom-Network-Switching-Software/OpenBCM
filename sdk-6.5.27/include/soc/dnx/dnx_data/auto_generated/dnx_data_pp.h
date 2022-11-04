
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PP_H_

#define _DNX_DATA_PP_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pp.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_pp_init(
    int unit);






typedef struct
{
    int chuck_block_id;
    int valid;
    char *name;
    char *block;
    int is_fp;
    int is_kleap;
    int is_pem;
} dnx_data_pp_stages_params_t;



typedef enum
{

    
    _dnx_data_pp_stages_feature_nof
} dnx_data_pp_stages_feature_e;



typedef int(
    *dnx_data_pp_stages_feature_get_f) (
    int unit,
    dnx_data_pp_stages_feature_e feature);


typedef uint32(
    *dnx_data_pp_stages_is_vtt5_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_stages_is_erpp_supported_get_f) (
    int unit);


typedef const dnx_data_pp_stages_params_t *(
    *dnx_data_pp_stages_params_get_f) (
    int unit,
    int stage);



typedef struct
{
    
    dnx_data_pp_stages_feature_get_f feature_get;
    
    dnx_data_pp_stages_is_vtt5_supported_get_f is_vtt5_supported_get;
    
    dnx_data_pp_stages_is_erpp_supported_get_f is_erpp_supported_get;
    
    dnx_data_pp_stages_params_get_f params_get;
    
    dnxc_data_table_info_get_f params_info_get;
} dnx_data_if_pp_stages_t;







typedef enum
{
    dnx_data_pp_PEM_clock_power_down,

    
    _dnx_data_pp_PEM_feature_nof
} dnx_data_pp_PEM_feature_e;



typedef int(
    *dnx_data_pp_PEM_feature_get_f) (
    int unit,
    dnx_data_pp_PEM_feature_e feature);


typedef uint32(
    *dnx_data_pp_PEM_nof_pem_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_PEM_nof_pem_blocks_per_core_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pp_PEM_feature_get_f feature_get;
    
    dnx_data_pp_PEM_nof_pem_bits_get_f nof_pem_bits_get;
    
    dnx_data_pp_PEM_nof_pem_blocks_per_core_get_f nof_pem_blocks_per_core_get;
} dnx_data_if_pp_PEM_t;






typedef struct
{
    char *standard_image_name;
    char *name;
} dnx_data_pp_application_device_image_t;


typedef struct
{
    char *relative_path;
} dnx_data_pp_application_ucode_t;



typedef enum
{

    
    _dnx_data_pp_application_feature_nof
} dnx_data_pp_application_feature_e;



typedef int(
    *dnx_data_pp_application_feature_get_f) (
    int unit,
    dnx_data_pp_application_feature_e feature);


typedef uint32(
    *dnx_data_pp_application_vtt1_lookup_by_exem1_get_f) (
    int unit);


typedef const dnx_data_pp_application_device_image_t *(
    *dnx_data_pp_application_device_image_get_f) (
    int unit);


typedef const dnx_data_pp_application_ucode_t *(
    *dnx_data_pp_application_ucode_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pp_application_feature_get_f feature_get;
    
    dnx_data_pp_application_vtt1_lookup_by_exem1_get_f vtt1_lookup_by_exem1_get;
    
    dnx_data_pp_application_device_image_get_f device_image_get;
    
    dnxc_data_table_info_get_f device_image_info_get;
    
    dnx_data_pp_application_ucode_get_f ucode_get;
    
    dnxc_data_table_info_get_f ucode_info_get;
} dnx_data_if_pp_application_t;







typedef enum
{

    
    _dnx_data_pp_ETPP_feature_nof
} dnx_data_pp_ETPP_feature_e;



typedef int(
    *dnx_data_pp_ETPP_feature_get_f) (
    int unit,
    dnx_data_pp_ETPP_feature_e feature);


typedef uint32(
    *dnx_data_pp_ETPP_etps_type_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_ETPP_nof_additional_headers_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_ETPP_nof_additional_headers_profiles_maps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_ETPP_dynamic_additional_headers_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_ETPP_ingress_packet_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_ETPP_btc_max_header_to_add_in_bytes_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pp_ETPP_feature_get_f feature_get;
    
    dnx_data_pp_ETPP_etps_type_size_bits_get_f etps_type_size_bits_get;
    
    dnx_data_pp_ETPP_nof_additional_headers_profiles_get_f nof_additional_headers_profiles_get;
    
    dnx_data_pp_ETPP_nof_additional_headers_profiles_maps_get_f nof_additional_headers_profiles_maps_get;
    
    dnx_data_pp_ETPP_dynamic_additional_headers_profile_get_f dynamic_additional_headers_profile_get;
    
    dnx_data_pp_ETPP_ingress_packet_header_size_get_f ingress_packet_header_size_get;
    
    dnx_data_pp_ETPP_btc_max_header_to_add_in_bytes_get_f btc_max_header_to_add_in_bytes_get;
} dnx_data_if_pp_ETPP_t;






typedef struct
{
    char *in_port;
    char *ptc;
    char *in_lif;
    char *fwd_domain;
    char *ingress_layers;
    char *ingress_qos;
} dnx_data_pp_debug_from_signal_str_t;



typedef enum
{

    
    _dnx_data_pp_debug_feature_nof
} dnx_data_pp_debug_feature_e;



typedef int(
    *dnx_data_pp_debug_feature_get_f) (
    int unit,
    dnx_data_pp_debug_feature_e feature);


typedef uint32(
    *dnx_data_pp_debug_packet_header_data_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_debug_ptc_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_debug_valid_bytes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pp_debug_time_stamp_get_f) (
    int unit);


typedef const dnx_data_pp_debug_from_signal_str_t *(
    *dnx_data_pp_debug_from_signal_str_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pp_debug_feature_get_f feature_get;
    
    dnx_data_pp_debug_packet_header_data_get_f packet_header_data_get;
    
    dnx_data_pp_debug_ptc_size_get_f ptc_size_get;
    
    dnx_data_pp_debug_valid_bytes_get_f valid_bytes_get;
    
    dnx_data_pp_debug_time_stamp_get_f time_stamp_get;
    
    dnx_data_pp_debug_from_signal_str_get_f from_signal_str_get;
    
    dnxc_data_table_info_get_f from_signal_str_info_get;
} dnx_data_if_pp_debug_t;





typedef struct
{
    
    dnx_data_if_pp_stages_t stages;
    
    dnx_data_if_pp_PEM_t PEM;
    
    dnx_data_if_pp_application_t application;
    
    dnx_data_if_pp_ETPP_t ETPP;
    
    dnx_data_if_pp_debug_t debug;
} dnx_data_if_pp_t;




extern dnx_data_if_pp_t dnx_data_pp;


#endif 

