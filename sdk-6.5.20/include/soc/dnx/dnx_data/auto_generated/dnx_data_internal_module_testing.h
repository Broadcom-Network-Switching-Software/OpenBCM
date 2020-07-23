

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MODULE_TESTING_H_

#define _DNX_DATA_INTERNAL_MODULE_TESTING_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_module_testing_submodule_unsupported,
    dnx_data_module_testing_submodule_types,
    dnx_data_module_testing_submodule_property_methods,
    dnx_data_module_testing_submodule_data_pointers,
    dnx_data_module_testing_submodule_dbal,

    
    _dnx_data_module_testing_submodule_nof
} dnx_data_module_testing_submodule_e;








int dnx_data_module_testing_unsupported_feature_get(
    int unit,
    dnx_data_module_testing_unsupported_feature_e feature);



typedef enum
{
    dnx_data_module_testing_unsupported_define_supported_def,
    dnx_data_module_testing_unsupported_define_unsupported_def,

    
    _dnx_data_module_testing_unsupported_define_nof
} dnx_data_module_testing_unsupported_define_e;



uint32 dnx_data_module_testing_unsupported_supported_def_get(
    int unit);


uint32 dnx_data_module_testing_unsupported_unsupported_def_get(
    int unit);



typedef enum
{
    dnx_data_module_testing_unsupported_table_supported_table,
    dnx_data_module_testing_unsupported_table_unsupported_table,

    
    _dnx_data_module_testing_unsupported_table_nof
} dnx_data_module_testing_unsupported_table_e;



const dnx_data_module_testing_unsupported_supported_table_t * dnx_data_module_testing_unsupported_supported_table_get(
    int unit);


const dnx_data_module_testing_unsupported_unsupported_table_t * dnx_data_module_testing_unsupported_unsupported_table_get(
    int unit);



shr_error_e dnx_data_module_testing_unsupported_supported_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_unsupported_unsupported_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_module_testing_unsupported_supported_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_unsupported_unsupported_table_info_get(
    int unit);






int dnx_data_module_testing_types_feature_get(
    int unit,
    dnx_data_module_testing_types_feature_e feature);



typedef enum
{
    dnx_data_module_testing_types_define_def_signed,
    dnx_data_module_testing_types_define_def_unsigned,
    dnx_data_module_testing_types_define_num_signed,
    dnx_data_module_testing_types_define_num_unsigned,

    
    _dnx_data_module_testing_types_define_nof
} dnx_data_module_testing_types_define_e;



int dnx_data_module_testing_types_def_signed_get(
    int unit);


uint32 dnx_data_module_testing_types_def_unsigned_get(
    int unit);


int dnx_data_module_testing_types_num_signed_get(
    int unit);


uint32 dnx_data_module_testing_types_num_unsigned_get(
    int unit);



typedef enum
{
    dnx_data_module_testing_types_table_all,

    
    _dnx_data_module_testing_types_table_nof
} dnx_data_module_testing_types_table_e;



const dnx_data_module_testing_types_all_t * dnx_data_module_testing_types_all_get(
    int unit);



shr_error_e dnx_data_module_testing_types_all_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_module_testing_types_all_info_get(
    int unit);






int dnx_data_module_testing_property_methods_feature_get(
    int unit,
    dnx_data_module_testing_property_methods_feature_e feature);



typedef enum
{
    dnx_data_module_testing_property_methods_define_numeric_range,

    
    _dnx_data_module_testing_property_methods_define_nof
} dnx_data_module_testing_property_methods_define_e;



uint32 dnx_data_module_testing_property_methods_numeric_range_get(
    int unit);



typedef enum
{
    dnx_data_module_testing_property_methods_table_enable,
    dnx_data_module_testing_property_methods_table_port_enable,
    dnx_data_module_testing_property_methods_table_suffix_enable,
    dnx_data_module_testing_property_methods_table_range,
    dnx_data_module_testing_property_methods_table_port_range,
    dnx_data_module_testing_property_methods_table_suffix_range,
    dnx_data_module_testing_property_methods_table_direct_map,
    dnx_data_module_testing_property_methods_table_port_direct_map,
    dnx_data_module_testing_property_methods_table_suffix_direct_map,
    dnx_data_module_testing_property_methods_table_custom,
    dnx_data_module_testing_property_methods_table_pbmp,
    dnx_data_module_testing_property_methods_table_port_pbmp,
    dnx_data_module_testing_property_methods_table_suffix_pbmp,
    dnx_data_module_testing_property_methods_table_str,
    dnx_data_module_testing_property_methods_table_port_str,
    dnx_data_module_testing_property_methods_table_suffix_str,

    
    _dnx_data_module_testing_property_methods_table_nof
} dnx_data_module_testing_property_methods_table_e;



const dnx_data_module_testing_property_methods_enable_t * dnx_data_module_testing_property_methods_enable_get(
    int unit);


const dnx_data_module_testing_property_methods_port_enable_t * dnx_data_module_testing_property_methods_port_enable_get(
    int unit,
    int port);


const dnx_data_module_testing_property_methods_suffix_enable_t * dnx_data_module_testing_property_methods_suffix_enable_get(
    int unit,
    int link);


const dnx_data_module_testing_property_methods_range_t * dnx_data_module_testing_property_methods_range_get(
    int unit);


const dnx_data_module_testing_property_methods_port_range_t * dnx_data_module_testing_property_methods_port_range_get(
    int unit,
    int port);


const dnx_data_module_testing_property_methods_suffix_range_t * dnx_data_module_testing_property_methods_suffix_range_get(
    int unit,
    int link);


const dnx_data_module_testing_property_methods_direct_map_t * dnx_data_module_testing_property_methods_direct_map_get(
    int unit);


const dnx_data_module_testing_property_methods_port_direct_map_t * dnx_data_module_testing_property_methods_port_direct_map_get(
    int unit,
    int port);


const dnx_data_module_testing_property_methods_suffix_direct_map_t * dnx_data_module_testing_property_methods_suffix_direct_map_get(
    int unit,
    int link);


const dnx_data_module_testing_property_methods_custom_t * dnx_data_module_testing_property_methods_custom_get(
    int unit,
    int link,
    int pipe);


const dnx_data_module_testing_property_methods_pbmp_t * dnx_data_module_testing_property_methods_pbmp_get(
    int unit);


const dnx_data_module_testing_property_methods_port_pbmp_t * dnx_data_module_testing_property_methods_port_pbmp_get(
    int unit,
    int port);


const dnx_data_module_testing_property_methods_suffix_pbmp_t * dnx_data_module_testing_property_methods_suffix_pbmp_get(
    int unit,
    int index);


const dnx_data_module_testing_property_methods_str_t * dnx_data_module_testing_property_methods_str_get(
    int unit);


const dnx_data_module_testing_property_methods_port_str_t * dnx_data_module_testing_property_methods_port_str_get(
    int unit,
    int port);


const dnx_data_module_testing_property_methods_suffix_str_t * dnx_data_module_testing_property_methods_suffix_str_get(
    int unit,
    int index);



shr_error_e dnx_data_module_testing_property_methods_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_port_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_suffix_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_port_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_suffix_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_port_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_suffix_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_custom_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_port_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_suffix_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_port_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_module_testing_property_methods_suffix_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_port_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_suffix_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_port_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_suffix_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_direct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_port_direct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_suffix_direct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_custom_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_pbmp_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_port_pbmp_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_suffix_pbmp_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_str_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_port_str_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_module_testing_property_methods_suffix_str_info_get(
    int unit);






int dnx_data_module_testing_data_pointers_feature_get(
    int unit,
    dnx_data_module_testing_data_pointers_feature_e feature);



typedef enum
{
    dnx_data_module_testing_data_pointers_define_cores,
    dnx_data_module_testing_data_pointers_define_add_cores,
    dnx_data_module_testing_data_pointers_define_mul_cores,
    dnx_data_module_testing_data_pointers_define_div_cores,
    dnx_data_module_testing_data_pointers_define_op_cores,
    dnx_data_module_testing_data_pointers_define_num_cores,
    dnx_data_module_testing_data_pointers_define_num_add_cores,
    dnx_data_module_testing_data_pointers_define_num_mul_cores,
    dnx_data_module_testing_data_pointers_define_num_div_cores,
    dnx_data_module_testing_data_pointers_define_num_op_cores,
    dnx_data_module_testing_data_pointers_define_num_last_cores,
    dnx_data_module_testing_data_pointers_define_num_table_cores,

    
    _dnx_data_module_testing_data_pointers_define_nof
} dnx_data_module_testing_data_pointers_define_e;



uint32 dnx_data_module_testing_data_pointers_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_add_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_mul_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_div_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_op_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_add_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_mul_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_div_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_op_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_last_cores_get(
    int unit);


uint32 dnx_data_module_testing_data_pointers_num_table_cores_get(
    int unit);



typedef enum
{
    dnx_data_module_testing_data_pointers_table_table_pointers,

    
    _dnx_data_module_testing_data_pointers_table_nof
} dnx_data_module_testing_data_pointers_table_e;



const dnx_data_module_testing_data_pointers_table_pointers_t * dnx_data_module_testing_data_pointers_table_pointers_get(
    int unit,
    int index);



shr_error_e dnx_data_module_testing_data_pointers_table_pointers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_module_testing_data_pointers_table_pointers_info_get(
    int unit);






int dnx_data_module_testing_dbal_feature_get(
    int unit,
    dnx_data_module_testing_dbal_feature_e feature);



typedef enum
{
    dnx_data_module_testing_dbal_define_vrf_field_size,
    dnx_data_module_testing_dbal_define_field_define_example,
    dnx_data_module_testing_dbal_define_res_max_val,
    dnx_data_module_testing_dbal_define_res_field_size,

    
    _dnx_data_module_testing_dbal_define_nof
} dnx_data_module_testing_dbal_define_e;



uint32 dnx_data_module_testing_dbal_vrf_field_size_get(
    int unit);


uint32 dnx_data_module_testing_dbal_field_define_example_get(
    int unit);


uint32 dnx_data_module_testing_dbal_res_max_val_get(
    int unit);


uint32 dnx_data_module_testing_dbal_res_field_size_get(
    int unit);



typedef enum
{
    dnx_data_module_testing_dbal_table_all,

    
    _dnx_data_module_testing_dbal_table_nof
} dnx_data_module_testing_dbal_table_e;



const dnx_data_module_testing_dbal_all_t * dnx_data_module_testing_dbal_all_get(
    int unit);



shr_error_e dnx_data_module_testing_dbal_all_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_module_testing_dbal_all_info_get(
    int unit);



shr_error_e dnx_data_module_testing_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

