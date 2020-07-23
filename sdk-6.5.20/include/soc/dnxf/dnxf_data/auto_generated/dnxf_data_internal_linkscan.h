

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_LINKSCAN_H_

#define _DNXF_DATA_INTERNAL_LINKSCAN_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_linkscan_submodule_general,

    
    _dnxf_data_linkscan_submodule_nof
} dnxf_data_linkscan_submodule_e;








int dnxf_data_linkscan_general_feature_get(
    int unit,
    dnxf_data_linkscan_general_feature_e feature);



typedef enum
{
    dnxf_data_linkscan_general_define_interval,
    dnxf_data_linkscan_general_define_error_delay,
    dnxf_data_linkscan_general_define_max_error,
    dnxf_data_linkscan_general_define_thread_priority,
    dnxf_data_linkscan_general_define_m0_pause_enable,

    
    _dnxf_data_linkscan_general_define_nof
} dnxf_data_linkscan_general_define_e;



uint32 dnxf_data_linkscan_general_interval_get(
    int unit);


uint32 dnxf_data_linkscan_general_error_delay_get(
    int unit);


uint32 dnxf_data_linkscan_general_max_error_get(
    int unit);


uint32 dnxf_data_linkscan_general_thread_priority_get(
    int unit);


uint32 dnxf_data_linkscan_general_m0_pause_enable_get(
    int unit);



typedef enum
{
    dnxf_data_linkscan_general_table_pbmp,

    
    _dnxf_data_linkscan_general_table_nof
} dnxf_data_linkscan_general_table_e;



const dnxf_data_linkscan_general_pbmp_t * dnxf_data_linkscan_general_pbmp_get(
    int unit);



shr_error_e dnxf_data_linkscan_general_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_linkscan_general_pbmp_info_get(
    int unit);



shr_error_e dnxf_data_linkscan_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

