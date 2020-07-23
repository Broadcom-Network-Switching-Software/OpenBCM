

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_LINKSCAN_H_

#define _DNX_DATA_INTERNAL_LINKSCAN_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_linkscan.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_linkscan_submodule_general,

    
    _dnx_data_linkscan_submodule_nof
} dnx_data_linkscan_submodule_e;








int dnx_data_linkscan_general_feature_get(
    int unit,
    dnx_data_linkscan_general_feature_e feature);



typedef enum
{
    dnx_data_linkscan_general_define_error_delay,
    dnx_data_linkscan_general_define_max_error,
    dnx_data_linkscan_general_define_thread_priority,
    dnx_data_linkscan_general_define_m0_pause_enable,

    
    _dnx_data_linkscan_general_define_nof
} dnx_data_linkscan_general_define_e;



uint32 dnx_data_linkscan_general_error_delay_get(
    int unit);


uint32 dnx_data_linkscan_general_max_error_get(
    int unit);


uint32 dnx_data_linkscan_general_thread_priority_get(
    int unit);


uint32 dnx_data_linkscan_general_m0_pause_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_linkscan_general_table_nof
} dnx_data_linkscan_general_table_e;






shr_error_e dnx_data_linkscan_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

