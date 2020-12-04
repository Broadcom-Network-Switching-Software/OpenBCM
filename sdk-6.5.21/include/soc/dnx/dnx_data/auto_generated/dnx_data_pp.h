

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PP_H_

#define _DNX_DATA_PP_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pp.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_pp_init(
    int unit);






typedef struct
{
    
    int valid;
    
    char *name;
    
    char *block;
    
    int is_fp;
    
    int is_kleap;
    
    int pema_id;
} dnx_data_pp_stages_params_t;



typedef enum
{

    
    _dnx_data_pp_stages_feature_nof
} dnx_data_pp_stages_feature_e;



typedef int(
    *dnx_data_pp_stages_feature_get_f) (
    int unit,
    dnx_data_pp_stages_feature_e feature);


typedef const dnx_data_pp_stages_params_t *(
    *dnx_data_pp_stages_params_get_f) (
    int unit,
    int stage);



typedef struct
{
    
    dnx_data_pp_stages_feature_get_f feature_get;
    
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



typedef struct
{
    
    dnx_data_pp_PEM_feature_get_f feature_get;
    
    dnx_data_pp_PEM_nof_pem_bits_get_f nof_pem_bits_get;
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


typedef const dnx_data_pp_application_device_image_t *(
    *dnx_data_pp_application_device_image_get_f) (
    int unit);


typedef const dnx_data_pp_application_ucode_t *(
    *dnx_data_pp_application_ucode_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pp_application_feature_get_f feature_get;
    
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



typedef struct
{
    
    dnx_data_pp_ETPP_feature_get_f feature_get;
    
    dnx_data_pp_ETPP_etps_type_size_bits_get_f etps_type_size_bits_get;
} dnx_data_if_pp_ETPP_t;







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



typedef struct
{
    
    dnx_data_pp_debug_feature_get_f feature_get;
    
    dnx_data_pp_debug_packet_header_data_get_f packet_header_data_get;
    
    dnx_data_pp_debug_ptc_size_get_f ptc_size_get;
    
    dnx_data_pp_debug_valid_bytes_get_f valid_bytes_get;
    
    dnx_data_pp_debug_time_stamp_get_f time_stamp_get;
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

