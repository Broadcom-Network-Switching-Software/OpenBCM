

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SYSTEM_RED_H_

#define _DNX_DATA_SYSTEM_RED_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_system_red.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_system_red_init(
    int unit);







typedef enum
{

    
    _dnx_data_system_red_config_feature_nof
} dnx_data_system_red_config_feature_e;



typedef int(
    *dnx_data_system_red_config_feature_get_f) (
    int unit,
    dnx_data_system_red_config_feature_e feature);


typedef uint32(
    *dnx_data_system_red_config_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_system_red_config_feature_get_f feature_get;
    
    dnx_data_system_red_config_enable_get_f enable_get;
} dnx_data_if_system_red_config_t;






typedef struct
{
    
    uint32 max;
    
    uint32 hw_resolution_max;
    
    uint32 hw_resolution_nof_bits;
} dnx_data_system_red_info_resource_t;


typedef struct
{
    
    uint32 offset;
} dnx_data_system_red_info_dbal_t;



typedef enum
{
    
    dnx_data_system_red_info_is_share_res_with_latency_based_admission,

    
    _dnx_data_system_red_info_feature_nof
} dnx_data_system_red_info_feature_e;



typedef int(
    *dnx_data_system_red_info_feature_get_f) (
    int unit,
    dnx_data_system_red_info_feature_e feature);


typedef uint32(
    *dnx_data_system_red_info_nof_red_q_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_system_red_info_max_red_q_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_system_red_info_max_ing_aging_period_get_f) (
    int unit);


typedef uint32(
    *dnx_data_system_red_info_max_sch_aging_period_get_f) (
    int unit);


typedef uint32(
    *dnx_data_system_red_info_sch_aging_period_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_system_red_info_max_free_res_thr_range_get_f) (
    int unit);


typedef const dnx_data_system_red_info_resource_t *(
    *dnx_data_system_red_info_resource_get_f) (
    int unit,
    int type);


typedef const dnx_data_system_red_info_dbal_t *(
    *dnx_data_system_red_info_dbal_get_f) (
    int unit,
    int type);



typedef struct
{
    
    dnx_data_system_red_info_feature_get_f feature_get;
    
    dnx_data_system_red_info_nof_red_q_size_get_f nof_red_q_size_get;
    
    dnx_data_system_red_info_max_red_q_size_get_f max_red_q_size_get;
    
    dnx_data_system_red_info_max_ing_aging_period_get_f max_ing_aging_period_get;
    
    dnx_data_system_red_info_max_sch_aging_period_get_f max_sch_aging_period_get;
    
    dnx_data_system_red_info_sch_aging_period_factor_get_f sch_aging_period_factor_get;
    
    dnx_data_system_red_info_max_free_res_thr_range_get_f max_free_res_thr_range_get;
    
    dnx_data_system_red_info_resource_get_f resource_get;
    
    dnxc_data_table_info_get_f resource_info_get;
    
    dnx_data_system_red_info_dbal_get_f dbal_get;
    
    dnxc_data_table_info_get_f dbal_info_get;
} dnx_data_if_system_red_info_t;





typedef struct
{
    
    dnx_data_if_system_red_config_t config;
    
    dnx_data_if_system_red_info_t info;
} dnx_data_if_system_red_t;




extern dnx_data_if_system_red_t dnx_data_system_red;


#endif 

