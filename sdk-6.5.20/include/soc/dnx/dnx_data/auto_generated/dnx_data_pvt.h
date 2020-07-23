

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PVT_H_

#define _DNX_DATA_PVT_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pvt.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_pvt_init(
    int unit);







typedef enum
{
    
    dnx_data_pvt_general_pvt_temp_monitors_hw_support,

    
    _dnx_data_pvt_general_feature_nof
} dnx_data_pvt_general_feature_e;



typedef int(
    *dnx_data_pvt_general_feature_get_f) (
    int unit,
    dnx_data_pvt_general_feature_e feature);


typedef uint32(
    *dnx_data_pvt_general_nof_pvt_monitors_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_pvt_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_pvt_factor_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pvt_general_feature_get_f feature_get;
    
    dnx_data_pvt_general_nof_pvt_monitors_get_f nof_pvt_monitors_get;
    
    dnx_data_pvt_general_pvt_base_get_f pvt_base_get;
    
    dnx_data_pvt_general_pvt_factor_get_f pvt_factor_get;
} dnx_data_if_pvt_general_t;





typedef struct
{
    
    dnx_data_if_pvt_general_t general;
} dnx_data_if_pvt_t;




extern dnx_data_if_pvt_t dnx_data_pvt;


#endif 

