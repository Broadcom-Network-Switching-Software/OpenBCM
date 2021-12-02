
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FIELD_MAP_H_

#define _DNX_DATA_FIELD_MAP_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field_map.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_field_map_init(
    int unit);






typedef struct
{
    
    int id;
    
    int size;
} dnx_data_field_map_action_params_t;



typedef enum
{

    
    _dnx_data_field_map_action_feature_nof
} dnx_data_field_map_action_feature_e;



typedef int(
    *dnx_data_field_map_action_feature_get_f) (
    int unit,
    dnx_data_field_map_action_feature_e feature);


typedef const dnx_data_field_map_action_params_t *(
    *dnx_data_field_map_action_params_get_f) (
    int unit,
    int stage,
    int action);



typedef struct
{
    
    dnx_data_field_map_action_feature_get_f feature_get;
    
    dnx_data_field_map_action_params_get_f params_get;
    
    dnxc_data_table_info_get_f params_info_get;
} dnx_data_if_field_map_action_t;






typedef struct
{
    
    int offset;
    
    int size;
} dnx_data_field_map_qual_params_t;


typedef struct
{
    
    uint8 nof_parts;
    
    uint16 offsets[DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS];
    
    uint8 sizes[DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS];
} dnx_data_field_map_qual_special_metadata_qual_t;


typedef struct
{
    
    int valid;
    
    int size;
    
    int offset;
} dnx_data_field_map_qual_layer_record_info_ingress_t;


typedef struct
{
    
    int valid;
    
    int size;
    
    int offset;
} dnx_data_field_map_qual_layer_record_info_egress_t;



typedef enum
{

    
    _dnx_data_field_map_qual_feature_nof
} dnx_data_field_map_qual_feature_e;



typedef int(
    *dnx_data_field_map_qual_feature_get_f) (
    int unit,
    dnx_data_field_map_qual_feature_e feature);


typedef const dnx_data_field_map_qual_params_t *(
    *dnx_data_field_map_qual_params_get_f) (
    int unit,
    int stage,
    int qual);


typedef const dnx_data_field_map_qual_special_metadata_qual_t *(
    *dnx_data_field_map_qual_special_metadata_qual_get_f) (
    int unit,
    int stage,
    int qual);


typedef const dnx_data_field_map_qual_layer_record_info_ingress_t *(
    *dnx_data_field_map_qual_layer_record_info_ingress_get_f) (
    int unit,
    int layer_record);


typedef const dnx_data_field_map_qual_layer_record_info_egress_t *(
    *dnx_data_field_map_qual_layer_record_info_egress_get_f) (
    int unit,
    int layer_record);



typedef struct
{
    
    dnx_data_field_map_qual_feature_get_f feature_get;
    
    dnx_data_field_map_qual_params_get_f params_get;
    
    dnxc_data_table_info_get_f params_info_get;
    
    dnx_data_field_map_qual_special_metadata_qual_get_f special_metadata_qual_get;
    
    dnxc_data_table_info_get_f special_metadata_qual_info_get;
    
    dnx_data_field_map_qual_layer_record_info_ingress_get_f layer_record_info_ingress_get;
    
    dnxc_data_table_info_get_f layer_record_info_ingress_info_get;
    
    dnx_data_field_map_qual_layer_record_info_egress_get_f layer_record_info_egress_get;
    
    dnxc_data_table_info_get_f layer_record_info_egress_info_get;
} dnx_data_if_field_map_qual_t;





typedef struct
{
    
    dnx_data_if_field_map_action_t action;
    
    dnx_data_if_field_map_qual_t qual;
} dnx_data_if_field_map_t;




extern dnx_data_if_field_map_t dnx_data_field_map;


#endif 

