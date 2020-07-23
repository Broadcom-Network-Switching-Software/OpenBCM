

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_LINKSCAN_H_

#define _DNX_DATA_LINKSCAN_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_linkscan.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_linkscan_init(
    int unit);







typedef enum
{

    
    _dnx_data_linkscan_general_feature_nof
} dnx_data_linkscan_general_feature_e;



typedef int(
    *dnx_data_linkscan_general_feature_get_f) (
    int unit,
    dnx_data_linkscan_general_feature_e feature);


typedef uint32(
    *dnx_data_linkscan_general_error_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_linkscan_general_max_error_get_f) (
    int unit);


typedef uint32(
    *dnx_data_linkscan_general_thread_priority_get_f) (
    int unit);


typedef uint32(
    *dnx_data_linkscan_general_m0_pause_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_linkscan_general_feature_get_f feature_get;
    
    dnx_data_linkscan_general_error_delay_get_f error_delay_get;
    
    dnx_data_linkscan_general_max_error_get_f max_error_get;
    
    dnx_data_linkscan_general_thread_priority_get_f thread_priority_get;
    
    dnx_data_linkscan_general_m0_pause_enable_get_f m0_pause_enable_get;
} dnx_data_if_linkscan_general_t;





typedef struct
{
    
    dnx_data_if_linkscan_general_t general;
} dnx_data_if_linkscan_t;




extern dnx_data_if_linkscan_t dnx_data_linkscan;


#endif 

