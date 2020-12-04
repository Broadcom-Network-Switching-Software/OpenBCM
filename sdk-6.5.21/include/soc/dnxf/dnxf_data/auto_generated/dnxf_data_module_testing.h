

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_MODULE_TESTING_H_

#define _DNXF_DATA_MODULE_TESTING_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_module_testing.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_if_module_testing_init(
    int unit);






typedef struct
{
    
    uint32 num;
} dnxf_data_module_testing_unsupported_supported_table_t;


typedef struct
{
    
    uint32 num;
} dnxf_data_module_testing_unsupported_unsupported_table_t;



typedef enum
{
    
    dnxf_data_module_testing_unsupported_supported_feature,
    
    dnxf_data_module_testing_unsupported_unsupported_feature,

    
    _dnxf_data_module_testing_unsupported_feature_nof
} dnxf_data_module_testing_unsupported_feature_e;



typedef int(
    *dnxf_data_module_testing_unsupported_feature_get_f) (
    int unit,
    dnxf_data_module_testing_unsupported_feature_e feature);


typedef uint32(
    *dnxf_data_module_testing_unsupported_supported_def_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_module_testing_unsupported_unsupported_def_get_f) (
    int unit);


typedef const dnxf_data_module_testing_unsupported_supported_table_t *(
    *dnxf_data_module_testing_unsupported_supported_table_get_f) (
    int unit);


typedef const dnxf_data_module_testing_unsupported_unsupported_table_t *(
    *dnxf_data_module_testing_unsupported_unsupported_table_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_module_testing_unsupported_feature_get_f feature_get;
    
    dnxf_data_module_testing_unsupported_supported_def_get_f supported_def_get;
    
    dnxf_data_module_testing_unsupported_unsupported_def_get_f unsupported_def_get;
    
    dnxf_data_module_testing_unsupported_supported_table_get_f supported_table_get;
    
    dnxc_data_table_info_get_f supported_table_info_get;
    
    dnxf_data_module_testing_unsupported_unsupported_table_get_f unsupported_table_get;
    
    dnxc_data_table_info_get_f unsupported_table_info_get;
} dnxf_data_if_module_testing_unsupported_t;






typedef struct
{
    
    int intval;
    
    uint8 uint8val;
    
    char charval;
    
    uint16 uint16val;
    
    bcm_fabric_device_type_t enumval;
    
    uint32 arrval[10];
    
    bcm_pbmp_t pbmpval;
    
    char *strval;
    
    char bufferval[100];
} dnxf_data_module_testing_types_all_t;



typedef enum
{

    
    _dnxf_data_module_testing_types_feature_nof
} dnxf_data_module_testing_types_feature_e;



typedef int(
    *dnxf_data_module_testing_types_feature_get_f) (
    int unit,
    dnxf_data_module_testing_types_feature_e feature);


typedef const dnxf_data_module_testing_types_all_t *(
    *dnxf_data_module_testing_types_all_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_module_testing_types_feature_get_f feature_get;
    
    dnxf_data_module_testing_types_all_get_f all_get;
    
    dnxc_data_table_info_get_f all_info_get;
} dnxf_data_if_module_testing_types_t;






typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_enable_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_port_enable_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_suffix_enable_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_range_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_port_range_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_suffix_range_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_direct_map_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_port_direct_map_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_suffix_direct_map_t;


typedef struct
{
    
    uint32 val;
} dnxf_data_module_testing_property_methods_custom_t;



typedef enum
{
    
    dnxf_data_module_testing_property_methods_feature_enable,

    
    _dnxf_data_module_testing_property_methods_feature_nof
} dnxf_data_module_testing_property_methods_feature_e;



typedef int(
    *dnxf_data_module_testing_property_methods_feature_get_f) (
    int unit,
    dnxf_data_module_testing_property_methods_feature_e feature);


typedef uint32(
    *dnxf_data_module_testing_property_methods_numeric_range_get_f) (
    int unit);


typedef const dnxf_data_module_testing_property_methods_enable_t *(
    *dnxf_data_module_testing_property_methods_enable_get_f) (
    int unit);


typedef const dnxf_data_module_testing_property_methods_port_enable_t *(
    *dnxf_data_module_testing_property_methods_port_enable_get_f) (
    int unit,
    int port);


typedef const dnxf_data_module_testing_property_methods_suffix_enable_t *(
    *dnxf_data_module_testing_property_methods_suffix_enable_get_f) (
    int unit,
    int link);


typedef const dnxf_data_module_testing_property_methods_range_t *(
    *dnxf_data_module_testing_property_methods_range_get_f) (
    int unit);


typedef const dnxf_data_module_testing_property_methods_port_range_t *(
    *dnxf_data_module_testing_property_methods_port_range_get_f) (
    int unit,
    int port);


typedef const dnxf_data_module_testing_property_methods_suffix_range_t *(
    *dnxf_data_module_testing_property_methods_suffix_range_get_f) (
    int unit,
    int link);


typedef const dnxf_data_module_testing_property_methods_direct_map_t *(
    *dnxf_data_module_testing_property_methods_direct_map_get_f) (
    int unit);


typedef const dnxf_data_module_testing_property_methods_port_direct_map_t *(
    *dnxf_data_module_testing_property_methods_port_direct_map_get_f) (
    int unit,
    int port);


typedef const dnxf_data_module_testing_property_methods_suffix_direct_map_t *(
    *dnxf_data_module_testing_property_methods_suffix_direct_map_get_f) (
    int unit,
    int link);


typedef const dnxf_data_module_testing_property_methods_custom_t *(
    *dnxf_data_module_testing_property_methods_custom_get_f) (
    int unit,
    int link,
    int pipe);



typedef struct
{
    
    dnxf_data_module_testing_property_methods_feature_get_f feature_get;
    
    dnxf_data_module_testing_property_methods_numeric_range_get_f numeric_range_get;
    
    dnxf_data_module_testing_property_methods_enable_get_f enable_get;
    
    dnxc_data_table_info_get_f enable_info_get;
    
    dnxf_data_module_testing_property_methods_port_enable_get_f port_enable_get;
    
    dnxc_data_table_info_get_f port_enable_info_get;
    
    dnxf_data_module_testing_property_methods_suffix_enable_get_f suffix_enable_get;
    
    dnxc_data_table_info_get_f suffix_enable_info_get;
    
    dnxf_data_module_testing_property_methods_range_get_f range_get;
    
    dnxc_data_table_info_get_f range_info_get;
    
    dnxf_data_module_testing_property_methods_port_range_get_f port_range_get;
    
    dnxc_data_table_info_get_f port_range_info_get;
    
    dnxf_data_module_testing_property_methods_suffix_range_get_f suffix_range_get;
    
    dnxc_data_table_info_get_f suffix_range_info_get;
    
    dnxf_data_module_testing_property_methods_direct_map_get_f direct_map_get;
    
    dnxc_data_table_info_get_f direct_map_info_get;
    
    dnxf_data_module_testing_property_methods_port_direct_map_get_f port_direct_map_get;
    
    dnxc_data_table_info_get_f port_direct_map_info_get;
    
    dnxf_data_module_testing_property_methods_suffix_direct_map_get_f suffix_direct_map_get;
    
    dnxc_data_table_info_get_f suffix_direct_map_info_get;
    
    dnxf_data_module_testing_property_methods_custom_get_f custom_get;
    
    dnxc_data_table_info_get_f custom_info_get;
} dnxf_data_if_module_testing_property_methods_t;





typedef struct
{
    
    dnxf_data_if_module_testing_unsupported_t unsupported;
    
    dnxf_data_if_module_testing_types_t types;
    
    dnxf_data_if_module_testing_property_methods_t property_methods;
} dnxf_data_if_module_testing_t;




extern dnxf_data_if_module_testing_t dnxf_data_module_testing;


#endif 

