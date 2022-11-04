
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FIELD_MAP_H_

#define _DNX_DATA_INTERNAL_FIELD_MAP_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_field_map_submodule_action,
    dnx_data_field_map_submodule_qual,
    dnx_data_field_map_submodule_tcam,
    dnx_data_field_map_submodule_port_profile,
    dnx_data_field_map_submodule_bcm_to_dnx,

    
    _dnx_data_field_map_submodule_nof
} dnx_data_field_map_submodule_e;








int dnx_data_field_map_action_feature_get(
    int unit,
    dnx_data_field_map_action_feature_e feature);



typedef enum
{

    
    _dnx_data_field_map_action_define_nof
} dnx_data_field_map_action_define_e;




typedef enum
{
    dnx_data_field_map_action_table_params,

    
    _dnx_data_field_map_action_table_nof
} dnx_data_field_map_action_table_e;



const dnx_data_field_map_action_params_t * dnx_data_field_map_action_params_get(
    int unit,
    int stage,
    int action);



shr_error_e dnx_data_field_map_action_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_map_action_params_info_get(
    int unit);






int dnx_data_field_map_qual_feature_get(
    int unit,
    dnx_data_field_map_qual_feature_e feature);



typedef enum
{

    
    _dnx_data_field_map_qual_define_nof
} dnx_data_field_map_qual_define_e;




typedef enum
{
    dnx_data_field_map_qual_table_params,
    dnx_data_field_map_qual_table_cs,
    dnx_data_field_map_qual_table_special_metadata_qual,
    dnx_data_field_map_qual_table_cs_special_general_data_qual,
    dnx_data_field_map_qual_table_cs_pem_scratch_pad_mapping,
    dnx_data_field_map_qual_table_layer_record_info_ingress,
    dnx_data_field_map_qual_table_layer_record_info_egress,
    dnx_data_field_map_qual_table_header,

    
    _dnx_data_field_map_qual_table_nof
} dnx_data_field_map_qual_table_e;



const dnx_data_field_map_qual_params_t * dnx_data_field_map_qual_params_get(
    int unit,
    int stage,
    int qual);


const dnx_data_field_map_qual_cs_t * dnx_data_field_map_qual_cs_get(
    int unit,
    int stage,
    int qual);


const dnx_data_field_map_qual_special_metadata_qual_t * dnx_data_field_map_qual_special_metadata_qual_get(
    int unit,
    int stage,
    int qual);


const dnx_data_field_map_qual_cs_special_general_data_qual_t * dnx_data_field_map_qual_cs_special_general_data_qual_get(
    int unit,
    int stage,
    int qual);


const dnx_data_field_map_qual_cs_pem_scratch_pad_mapping_t * dnx_data_field_map_qual_cs_pem_scratch_pad_mapping_get(
    int unit,
    int stage,
    int container);


const dnx_data_field_map_qual_layer_record_info_ingress_t * dnx_data_field_map_qual_layer_record_info_ingress_get(
    int unit,
    int layer_record);


const dnx_data_field_map_qual_layer_record_info_egress_t * dnx_data_field_map_qual_layer_record_info_egress_get(
    int unit,
    int layer_record);


const dnx_data_field_map_qual_header_t * dnx_data_field_map_qual_header_get(
    int unit,
    int qual);



shr_error_e dnx_data_field_map_qual_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_cs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_special_metadata_qual_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_cs_special_general_data_qual_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_cs_pem_scratch_pad_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_layer_record_info_ingress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_layer_record_info_egress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_field_map_qual_header_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_map_qual_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_cs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_special_metadata_qual_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_cs_special_general_data_qual_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_cs_pem_scratch_pad_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_layer_record_info_ingress_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_layer_record_info_egress_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_field_map_qual_header_info_get(
    int unit);






int dnx_data_field_map_tcam_feature_get(
    int unit,
    dnx_data_field_map_tcam_feature_e feature);



typedef enum
{

    
    _dnx_data_field_map_tcam_define_nof
} dnx_data_field_map_tcam_define_e;




typedef enum
{
    dnx_data_field_map_tcam_table_tcam_pp_app,

    
    _dnx_data_field_map_tcam_table_nof
} dnx_data_field_map_tcam_table_e;



const dnx_data_field_map_tcam_tcam_pp_app_t * dnx_data_field_map_tcam_tcam_pp_app_get(
    int unit,
    int pp_app);



shr_error_e dnx_data_field_map_tcam_tcam_pp_app_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_map_tcam_tcam_pp_app_info_get(
    int unit);






int dnx_data_field_map_port_profile_feature_get(
    int unit,
    dnx_data_field_map_port_profile_feature_e feature);



typedef enum
{

    
    _dnx_data_field_map_port_profile_define_nof
} dnx_data_field_map_port_profile_define_e;




typedef enum
{
    dnx_data_field_map_port_profile_table_port_class_profile,

    
    _dnx_data_field_map_port_profile_table_nof
} dnx_data_field_map_port_profile_table_e;



const dnx_data_field_map_port_profile_port_class_profile_t * dnx_data_field_map_port_profile_port_class_profile_get(
    int unit,
    int port_class);



shr_error_e dnx_data_field_map_port_profile_port_class_profile_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_map_port_profile_port_class_profile_info_get(
    int unit);






int dnx_data_field_map_bcm_to_dnx_feature_get(
    int unit,
    dnx_data_field_map_bcm_to_dnx_feature_e feature);



typedef enum
{

    
    _dnx_data_field_map_bcm_to_dnx_define_nof
} dnx_data_field_map_bcm_to_dnx_define_e;




typedef enum
{
    dnx_data_field_map_bcm_to_dnx_table_forward_context,

    
    _dnx_data_field_map_bcm_to_dnx_table_nof
} dnx_data_field_map_bcm_to_dnx_table_e;



const dnx_data_field_map_bcm_to_dnx_forward_context_t * dnx_data_field_map_bcm_to_dnx_forward_context_get(
    int unit,
    int bcm_data);



shr_error_e dnx_data_field_map_bcm_to_dnx_forward_context_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_field_map_bcm_to_dnx_forward_context_info_get(
    int unit);



shr_error_e dnx_data_field_map_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

