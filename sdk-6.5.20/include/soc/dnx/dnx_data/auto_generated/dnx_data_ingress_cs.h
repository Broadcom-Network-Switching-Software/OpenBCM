

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INGRESS_CS_H_

#define _DNX_DATA_INGRESS_CS_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingress_cs.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_ingress_cs_init(
    int unit);






typedef struct
{
    
    int valid;
    
    int supports_half_entries;
} dnx_data_ingress_cs_properties_per_stage_t;



typedef enum
{

    
    _dnx_data_ingress_cs_properties_feature_nof
} dnx_data_ingress_cs_properties_feature_e;



typedef int(
    *dnx_data_ingress_cs_properties_feature_get_f) (
    int unit,
    dnx_data_ingress_cs_properties_feature_e feature);


typedef uint32(
    *dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get_f) (
    int unit);


typedef const dnx_data_ingress_cs_properties_per_stage_t *(
    *dnx_data_ingress_cs_properties_per_stage_get_f) (
    int unit,
    int stage);



typedef struct
{
    
    dnx_data_ingress_cs_properties_feature_get_f feature_get;
    
    dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get_f cs_buffer_nof_bits_max_get;
    
    dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get_f nof_bits_entry_size_prefix_get;
    
    dnx_data_ingress_cs_properties_per_stage_get_f per_stage_get;
    
    dnxc_data_table_info_get_f per_stage_info_get;
} dnx_data_if_ingress_cs_properties_t;







typedef enum
{
    dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write,

    
    _dnx_data_ingress_cs_features_feature_nof
} dnx_data_ingress_cs_features_feature_e;



typedef int(
    *dnx_data_ingress_cs_features_feature_get_f) (
    int unit,
    dnx_data_ingress_cs_features_feature_e feature);



typedef struct
{
    
    dnx_data_ingress_cs_features_feature_get_f feature_get;
} dnx_data_if_ingress_cs_features_t;







typedef enum
{
    
    dnx_data_ingress_cs_parser_parsing_context_map_enable,

    
    _dnx_data_ingress_cs_parser_feature_nof
} dnx_data_ingress_cs_parser_feature_e;



typedef int(
    *dnx_data_ingress_cs_parser_feature_get_f) (
    int unit,
    dnx_data_ingress_cs_parser_feature_e feature);



typedef struct
{
    
    dnx_data_ingress_cs_parser_feature_get_f feature_get;
} dnx_data_if_ingress_cs_parser_t;





typedef struct
{
    
    dnx_data_if_ingress_cs_properties_t properties;
    
    dnx_data_if_ingress_cs_features_t features;
    
    dnx_data_if_ingress_cs_parser_t parser;
} dnx_data_if_ingress_cs_t;




extern dnx_data_if_ingress_cs_t dnx_data_ingress_cs;


#endif 

