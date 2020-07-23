

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_LINKSCAN_H_

#define _DNXF_DATA_LINKSCAN_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_linkscan.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_if_linkscan_init(
    int unit);






typedef struct
{
    
    bcm_pbmp_t sw;
    
    bcm_pbmp_t hw;
} dnxf_data_linkscan_general_pbmp_t;



typedef enum
{

    
    _dnxf_data_linkscan_general_feature_nof
} dnxf_data_linkscan_general_feature_e;



typedef int(
    *dnxf_data_linkscan_general_feature_get_f) (
    int unit,
    dnxf_data_linkscan_general_feature_e feature);


typedef uint32(
    *dnxf_data_linkscan_general_interval_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_linkscan_general_error_delay_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_linkscan_general_max_error_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_linkscan_general_thread_priority_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_linkscan_general_m0_pause_enable_get_f) (
    int unit);


typedef const dnxf_data_linkscan_general_pbmp_t *(
    *dnxf_data_linkscan_general_pbmp_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_linkscan_general_feature_get_f feature_get;
    
    dnxf_data_linkscan_general_interval_get_f interval_get;
    
    dnxf_data_linkscan_general_error_delay_get_f error_delay_get;
    
    dnxf_data_linkscan_general_max_error_get_f max_error_get;
    
    dnxf_data_linkscan_general_thread_priority_get_f thread_priority_get;
    
    dnxf_data_linkscan_general_m0_pause_enable_get_f m0_pause_enable_get;
    
    dnxf_data_linkscan_general_pbmp_get_f pbmp_get;
    
    dnxc_data_table_info_get_f pbmp_info_get;
} dnxf_data_if_linkscan_general_t;





typedef struct
{
    
    dnxf_data_if_linkscan_general_t general;
} dnxf_data_if_linkscan_t;




extern dnxf_data_if_linkscan_t dnxf_data_linkscan;


#endif 

