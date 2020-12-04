

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_MODULE_TESTING_H_

#define _DNXF_DATA_INTERNAL_MODULE_TESTING_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_module_testing.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_module_testing_submodule_unsupported,
    dnxf_data_module_testing_submodule_types,
    dnxf_data_module_testing_submodule_property_methods,

    
    _dnxf_data_module_testing_submodule_nof
} dnxf_data_module_testing_submodule_e;








int dnxf_data_module_testing_unsupported_feature_get(
    int unit,
    dnxf_data_module_testing_unsupported_feature_e feature);



typedef enum
{
    dnxf_data_module_testing_unsupported_define_supported_def,
    dnxf_data_module_testing_unsupported_define_unsupported_def,

    
    _dnxf_data_module_testing_unsupported_define_nof
} dnxf_data_module_testing_unsupported_define_e;



uint32 dnxf_data_module_testing_unsupported_supported_def_get(
    int unit);


uint32 dnxf_data_module_testing_unsupported_unsupported_def_get(
    int unit);



typedef enum
{
    dnxf_data_module_testing_unsupported_table_supported_table,
    dnxf_data_module_testing_unsupported_table_unsupported_table,

    
    _dnxf_data_module_testing_unsupported_table_nof
} dnxf_data_module_testing_unsupported_table_e;



const dnxf_data_module_testing_unsupported_supported_table_t * dnxf_data_module_testing_unsupported_supported_table_get(
    int unit);


const dnxf_data_module_testing_unsupported_unsupported_table_t * dnxf_data_module_testing_unsupported_unsupported_table_get(
    int unit);



shr_error_e dnxf_data_module_testing_unsupported_supported_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_unsupported_unsupported_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_module_testing_unsupported_supported_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_unsupported_unsupported_table_info_get(
    int unit);






int dnxf_data_module_testing_types_feature_get(
    int unit,
    dnxf_data_module_testing_types_feature_e feature);



typedef enum
{

    
    _dnxf_data_module_testing_types_define_nof
} dnxf_data_module_testing_types_define_e;




typedef enum
{
    dnxf_data_module_testing_types_table_all,

    
    _dnxf_data_module_testing_types_table_nof
} dnxf_data_module_testing_types_table_e;



const dnxf_data_module_testing_types_all_t * dnxf_data_module_testing_types_all_get(
    int unit);



shr_error_e dnxf_data_module_testing_types_all_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_module_testing_types_all_info_get(
    int unit);






int dnxf_data_module_testing_property_methods_feature_get(
    int unit,
    dnxf_data_module_testing_property_methods_feature_e feature);



typedef enum
{
    dnxf_data_module_testing_property_methods_define_numeric_range,

    
    _dnxf_data_module_testing_property_methods_define_nof
} dnxf_data_module_testing_property_methods_define_e;



uint32 dnxf_data_module_testing_property_methods_numeric_range_get(
    int unit);



typedef enum
{
    dnxf_data_module_testing_property_methods_table_enable,
    dnxf_data_module_testing_property_methods_table_port_enable,
    dnxf_data_module_testing_property_methods_table_suffix_enable,
    dnxf_data_module_testing_property_methods_table_range,
    dnxf_data_module_testing_property_methods_table_port_range,
    dnxf_data_module_testing_property_methods_table_suffix_range,
    dnxf_data_module_testing_property_methods_table_direct_map,
    dnxf_data_module_testing_property_methods_table_port_direct_map,
    dnxf_data_module_testing_property_methods_table_suffix_direct_map,
    dnxf_data_module_testing_property_methods_table_custom,

    
    _dnxf_data_module_testing_property_methods_table_nof
} dnxf_data_module_testing_property_methods_table_e;



const dnxf_data_module_testing_property_methods_enable_t * dnxf_data_module_testing_property_methods_enable_get(
    int unit);


const dnxf_data_module_testing_property_methods_port_enable_t * dnxf_data_module_testing_property_methods_port_enable_get(
    int unit,
    int port);


const dnxf_data_module_testing_property_methods_suffix_enable_t * dnxf_data_module_testing_property_methods_suffix_enable_get(
    int unit,
    int link);


const dnxf_data_module_testing_property_methods_range_t * dnxf_data_module_testing_property_methods_range_get(
    int unit);


const dnxf_data_module_testing_property_methods_port_range_t * dnxf_data_module_testing_property_methods_port_range_get(
    int unit,
    int port);


const dnxf_data_module_testing_property_methods_suffix_range_t * dnxf_data_module_testing_property_methods_suffix_range_get(
    int unit,
    int link);


const dnxf_data_module_testing_property_methods_direct_map_t * dnxf_data_module_testing_property_methods_direct_map_get(
    int unit);


const dnxf_data_module_testing_property_methods_port_direct_map_t * dnxf_data_module_testing_property_methods_port_direct_map_get(
    int unit,
    int port);


const dnxf_data_module_testing_property_methods_suffix_direct_map_t * dnxf_data_module_testing_property_methods_suffix_direct_map_get(
    int unit,
    int link);


const dnxf_data_module_testing_property_methods_custom_t * dnxf_data_module_testing_property_methods_custom_get(
    int unit,
    int link,
    int pipe);



shr_error_e dnxf_data_module_testing_property_methods_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_port_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_suffix_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_port_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_suffix_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_port_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_suffix_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_module_testing_property_methods_custom_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_port_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_suffix_enable_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_port_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_suffix_range_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_direct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_port_direct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_suffix_direct_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_module_testing_property_methods_custom_info_get(
    int unit);



shr_error_e dnxf_data_module_testing_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

