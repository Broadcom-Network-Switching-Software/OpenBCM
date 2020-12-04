

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_LANE_MAP_H_

#define _DNX_DATA_LANE_MAP_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_lane_map.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_lane_map_init(
    int unit);






typedef struct
{
    
    int serdes_rx_id;
    
    int serdes_tx_id;
} dnx_data_lane_map_nif_mapping_t;



typedef enum
{

    
    _dnx_data_lane_map_nif_feature_nof
} dnx_data_lane_map_nif_feature_e;



typedef int(
    *dnx_data_lane_map_nif_feature_get_f) (
    int unit,
    dnx_data_lane_map_nif_feature_e feature);


typedef const dnx_data_lane_map_nif_mapping_t *(
    *dnx_data_lane_map_nif_mapping_get_f) (
    int unit,
    int lane_id);



typedef struct
{
    
    dnx_data_lane_map_nif_feature_get_f feature_get;
    
    dnx_data_lane_map_nif_mapping_get_f mapping_get;
    
    dnxc_data_table_info_get_f mapping_info_get;
} dnx_data_if_lane_map_nif_t;






typedef struct
{
    
    int serdes_rx_id;
    
    int serdes_tx_id;
} dnx_data_lane_map_fabric_mapping_t;



typedef enum
{

    
    _dnx_data_lane_map_fabric_feature_nof
} dnx_data_lane_map_fabric_feature_e;



typedef int(
    *dnx_data_lane_map_fabric_feature_get_f) (
    int unit,
    dnx_data_lane_map_fabric_feature_e feature);


typedef uint32(
    *dnx_data_lane_map_fabric_resolution_get_f) (
    int unit);


typedef const dnx_data_lane_map_fabric_mapping_t *(
    *dnx_data_lane_map_fabric_mapping_get_f) (
    int unit,
    int lane_id);



typedef struct
{
    
    dnx_data_lane_map_fabric_feature_get_f feature_get;
    
    dnx_data_lane_map_fabric_resolution_get_f resolution_get;
    
    dnx_data_lane_map_fabric_mapping_get_f mapping_get;
    
    dnxc_data_table_info_get_f mapping_info_get;
} dnx_data_if_lane_map_fabric_t;






typedef struct
{
    
    int ilkn_lane_id;
} dnx_data_lane_map_ilkn_remapping_t;



typedef enum
{

    
    _dnx_data_lane_map_ilkn_feature_nof
} dnx_data_lane_map_ilkn_feature_e;



typedef int(
    *dnx_data_lane_map_ilkn_feature_get_f) (
    int unit,
    dnx_data_lane_map_ilkn_feature_e feature);


typedef const dnx_data_lane_map_ilkn_remapping_t *(
    *dnx_data_lane_map_ilkn_remapping_get_f) (
    int unit,
    int lane_id,
    int ilkn_id);



typedef struct
{
    
    dnx_data_lane_map_ilkn_feature_get_f feature_get;
    
    dnx_data_lane_map_ilkn_remapping_get_f remapping_get;
    
    dnxc_data_table_info_get_f remapping_info_get;
} dnx_data_if_lane_map_ilkn_t;





typedef struct
{
    
    dnx_data_if_lane_map_nif_t nif;
    
    dnx_data_if_lane_map_fabric_t fabric;
    
    dnx_data_if_lane_map_ilkn_t ilkn;
} dnx_data_if_lane_map_t;




extern dnx_data_if_lane_map_t dnx_data_lane_map;


#endif 

