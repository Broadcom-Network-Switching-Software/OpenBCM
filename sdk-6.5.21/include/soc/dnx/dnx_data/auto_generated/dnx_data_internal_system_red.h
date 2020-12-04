

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SYSTEM_RED_H_

#define _DNX_DATA_INTERNAL_SYSTEM_RED_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_system_red_submodule_config,
    dnx_data_system_red_submodule_info,

    
    _dnx_data_system_red_submodule_nof
} dnx_data_system_red_submodule_e;








int dnx_data_system_red_config_feature_get(
    int unit,
    dnx_data_system_red_config_feature_e feature);



typedef enum
{
    dnx_data_system_red_config_define_enable,

    
    _dnx_data_system_red_config_define_nof
} dnx_data_system_red_config_define_e;



uint32 dnx_data_system_red_config_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_system_red_config_table_nof
} dnx_data_system_red_config_table_e;









int dnx_data_system_red_info_feature_get(
    int unit,
    dnx_data_system_red_info_feature_e feature);



typedef enum
{
    dnx_data_system_red_info_define_nof_red_q_size,
    dnx_data_system_red_info_define_max_red_q_size,
    dnx_data_system_red_info_define_max_ing_aging_period,
    dnx_data_system_red_info_define_max_sch_aging_period,
    dnx_data_system_red_info_define_sch_aging_period_factor,
    dnx_data_system_red_info_define_max_free_res_thr_range,

    
    _dnx_data_system_red_info_define_nof
} dnx_data_system_red_info_define_e;



uint32 dnx_data_system_red_info_nof_red_q_size_get(
    int unit);


uint32 dnx_data_system_red_info_max_red_q_size_get(
    int unit);


uint32 dnx_data_system_red_info_max_ing_aging_period_get(
    int unit);


uint32 dnx_data_system_red_info_max_sch_aging_period_get(
    int unit);


uint32 dnx_data_system_red_info_sch_aging_period_factor_get(
    int unit);


uint32 dnx_data_system_red_info_max_free_res_thr_range_get(
    int unit);



typedef enum
{
    dnx_data_system_red_info_table_resource,
    dnx_data_system_red_info_table_dbal,

    
    _dnx_data_system_red_info_table_nof
} dnx_data_system_red_info_table_e;



const dnx_data_system_red_info_resource_t * dnx_data_system_red_info_resource_get(
    int unit,
    int type);


const dnx_data_system_red_info_dbal_t * dnx_data_system_red_info_dbal_get(
    int unit,
    int type);



shr_error_e dnx_data_system_red_info_resource_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_system_red_info_dbal_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_system_red_info_resource_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_system_red_info_dbal_info_get(
    int unit);



shr_error_e dnx_data_system_red_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

