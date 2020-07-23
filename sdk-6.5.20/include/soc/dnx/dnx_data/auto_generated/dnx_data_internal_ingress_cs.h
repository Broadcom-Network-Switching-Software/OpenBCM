

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_INGRESS_CS_H_

#define _DNX_DATA_INTERNAL_INGRESS_CS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_ingress_cs_submodule_properties,
    dnx_data_ingress_cs_submodule_features,
    dnx_data_ingress_cs_submodule_parser,

    
    _dnx_data_ingress_cs_submodule_nof
} dnx_data_ingress_cs_submodule_e;








int dnx_data_ingress_cs_properties_feature_get(
    int unit,
    dnx_data_ingress_cs_properties_feature_e feature);



typedef enum
{
    dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max,
    dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix,

    
    _dnx_data_ingress_cs_properties_define_nof
} dnx_data_ingress_cs_properties_define_e;



uint32 dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get(
    int unit);


uint32 dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get(
    int unit);



typedef enum
{
    dnx_data_ingress_cs_properties_table_per_stage,

    
    _dnx_data_ingress_cs_properties_table_nof
} dnx_data_ingress_cs_properties_table_e;



const dnx_data_ingress_cs_properties_per_stage_t * dnx_data_ingress_cs_properties_per_stage_get(
    int unit,
    int stage);



shr_error_e dnx_data_ingress_cs_properties_per_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingress_cs_properties_per_stage_info_get(
    int unit);






int dnx_data_ingress_cs_features_feature_get(
    int unit,
    dnx_data_ingress_cs_features_feature_e feature);



typedef enum
{

    
    _dnx_data_ingress_cs_features_define_nof
} dnx_data_ingress_cs_features_define_e;




typedef enum
{

    
    _dnx_data_ingress_cs_features_table_nof
} dnx_data_ingress_cs_features_table_e;









int dnx_data_ingress_cs_parser_feature_get(
    int unit,
    dnx_data_ingress_cs_parser_feature_e feature);



typedef enum
{

    
    _dnx_data_ingress_cs_parser_define_nof
} dnx_data_ingress_cs_parser_define_e;




typedef enum
{

    
    _dnx_data_ingress_cs_parser_table_nof
} dnx_data_ingress_cs_parser_table_e;






shr_error_e dnx_data_ingress_cs_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

