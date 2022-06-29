
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_KLEAP_STAGE_INFO_H_

#define _DNX_DATA_INTERNAL_KLEAP_STAGE_INFO_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_kleap_stage_info.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_kleap_stage_info_submodule_kleap_stage_info,

    
    _dnx_data_kleap_stage_info_submodule_nof
} dnx_data_kleap_stage_info_submodule_e;








int dnx_data_kleap_stage_info_kleap_stage_info_feature_get(
    int unit,
    dnx_data_kleap_stage_info_kleap_stage_info_feature_e feature);



typedef enum
{
    dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages,

    
    _dnx_data_kleap_stage_info_kleap_stage_info_define_nof
} dnx_data_kleap_stage_info_kleap_stage_info_define_e;



uint32 dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_get(
    int unit);



typedef enum
{
    dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage,
    dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr,
    dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd,
    dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage,

    
    _dnx_data_kleap_stage_info_kleap_stage_info_table_nof
} dnx_data_kleap_stage_info_kleap_stage_info_table_e;



const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_get(
    int unit,
    int stage_index);


const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_get(
    int unit,
    int stage_index);


const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_get(
    int unit,
    int stage_index,
    int pd_index);


const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_get(
    int unit,
    int stage_index,
    int sub_stage_index);



shr_error_e dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_info_get(
    int unit);



shr_error_e dnx_data_kleap_stage_info_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

