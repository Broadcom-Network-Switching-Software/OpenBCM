

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_AOD_SIZES_H_

#define _DNX_DATA_AOD_SIZES_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_aod_sizes.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_aod_sizes_init(
    int unit);






typedef struct
{
    
    uint32 value;
} dnx_data_aod_sizes_AOD_sizes_t;


typedef struct
{
    
    uint32 value;
} dnx_data_aod_sizes_AOD_dynamic_sizes_t;



typedef enum
{

    
    _dnx_data_aod_sizes_AOD_feature_nof
} dnx_data_aod_sizes_AOD_feature_e;



typedef int(
    *dnx_data_aod_sizes_AOD_feature_get_f) (
    int unit,
    dnx_data_aod_sizes_AOD_feature_e feature);


typedef uint32(
    *dnx_data_aod_sizes_AOD_count_get_f) (
    int unit);


typedef const dnx_data_aod_sizes_AOD_sizes_t *(
    *dnx_data_aod_sizes_AOD_sizes_get_f) (
    int unit,
    int index);


typedef const dnx_data_aod_sizes_AOD_dynamic_sizes_t *(
    *dnx_data_aod_sizes_AOD_dynamic_sizes_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_aod_sizes_AOD_feature_get_f feature_get;
    
    dnx_data_aod_sizes_AOD_count_get_f count_get;
    
    dnx_data_aod_sizes_AOD_sizes_get_f sizes_get;
    
    dnxc_data_table_info_get_f sizes_info_get;
    
    dnx_data_aod_sizes_AOD_dynamic_sizes_get_f dynamic_sizes_get;
    
    dnxc_data_table_info_get_f dynamic_sizes_info_get;
} dnx_data_if_aod_sizes_AOD_t;





typedef struct
{
    
    dnx_data_if_aod_sizes_AOD_t AOD;
} dnx_data_if_aod_sizes_t;




extern dnx_data_if_aod_sizes_t dnx_data_aod_sizes;


#endif 

