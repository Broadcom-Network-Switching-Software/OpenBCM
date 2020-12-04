

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_LANE_MAP_H_

#define _DNX_DATA_INTERNAL_LANE_MAP_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_lane_map_submodule_nif,
    dnx_data_lane_map_submodule_fabric,
    dnx_data_lane_map_submodule_ilkn,

    
    _dnx_data_lane_map_submodule_nof
} dnx_data_lane_map_submodule_e;








int dnx_data_lane_map_nif_feature_get(
    int unit,
    dnx_data_lane_map_nif_feature_e feature);



typedef enum
{

    
    _dnx_data_lane_map_nif_define_nof
} dnx_data_lane_map_nif_define_e;




typedef enum
{
    dnx_data_lane_map_nif_table_mapping,

    
    _dnx_data_lane_map_nif_table_nof
} dnx_data_lane_map_nif_table_e;



const dnx_data_lane_map_nif_mapping_t * dnx_data_lane_map_nif_mapping_get(
    int unit,
    int lane_id);



shr_error_e dnx_data_lane_map_nif_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_lane_map_nif_mapping_info_get(
    int unit);






int dnx_data_lane_map_fabric_feature_get(
    int unit,
    dnx_data_lane_map_fabric_feature_e feature);



typedef enum
{
    dnx_data_lane_map_fabric_define_resolution,

    
    _dnx_data_lane_map_fabric_define_nof
} dnx_data_lane_map_fabric_define_e;



uint32 dnx_data_lane_map_fabric_resolution_get(
    int unit);



typedef enum
{
    dnx_data_lane_map_fabric_table_mapping,

    
    _dnx_data_lane_map_fabric_table_nof
} dnx_data_lane_map_fabric_table_e;



const dnx_data_lane_map_fabric_mapping_t * dnx_data_lane_map_fabric_mapping_get(
    int unit,
    int lane_id);



shr_error_e dnx_data_lane_map_fabric_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_lane_map_fabric_mapping_info_get(
    int unit);






int dnx_data_lane_map_ilkn_feature_get(
    int unit,
    dnx_data_lane_map_ilkn_feature_e feature);



typedef enum
{

    
    _dnx_data_lane_map_ilkn_define_nof
} dnx_data_lane_map_ilkn_define_e;




typedef enum
{
    dnx_data_lane_map_ilkn_table_remapping,

    
    _dnx_data_lane_map_ilkn_table_nof
} dnx_data_lane_map_ilkn_table_e;



const dnx_data_lane_map_ilkn_remapping_t * dnx_data_lane_map_ilkn_remapping_get(
    int unit,
    int lane_id,
    int ilkn_id);



shr_error_e dnx_data_lane_map_ilkn_remapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_lane_map_ilkn_remapping_info_get(
    int unit);



shr_error_e dnx_data_lane_map_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

