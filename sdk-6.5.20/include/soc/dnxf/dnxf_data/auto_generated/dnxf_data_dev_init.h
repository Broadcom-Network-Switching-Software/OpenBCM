

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_DEV_INIT_H_

#define _DNXF_DATA_DEV_INIT_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_dev_init.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_if_dev_init_init(
    int unit);






typedef struct
{
    
    uint32 value;
} dnxf_data_dev_init_time_step_thresh_t;


typedef struct
{
    
    uint32 value;
} dnxf_data_dev_init_time_appl_step_thresh_t;



typedef enum
{

    
    _dnxf_data_dev_init_time_feature_nof
} dnxf_data_dev_init_time_feature_e;



typedef int(
    *dnxf_data_dev_init_time_feature_get_f) (
    int unit,
    dnxf_data_dev_init_time_feature_e feature);


typedef uint32(
    *dnxf_data_dev_init_time_init_total_thresh_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_time_appl_init_total_thresh_get_f) (
    int unit);


typedef const dnxf_data_dev_init_time_step_thresh_t *(
    *dnxf_data_dev_init_time_step_thresh_get_f) (
    int unit,
    int step_id);


typedef const dnxf_data_dev_init_time_appl_step_thresh_t *(
    *dnxf_data_dev_init_time_appl_step_thresh_get_f) (
    int unit,
    int step_id);



typedef struct
{
    
    dnxf_data_dev_init_time_feature_get_f feature_get;
    
    dnxf_data_dev_init_time_init_total_thresh_get_f init_total_thresh_get;
    
    dnxf_data_dev_init_time_appl_init_total_thresh_get_f appl_init_total_thresh_get;
    
    dnxf_data_dev_init_time_step_thresh_get_f step_thresh_get;
    
    dnxc_data_table_info_get_f step_thresh_info_get;
    
    dnxf_data_dev_init_time_appl_step_thresh_get_f appl_step_thresh_get;
    
    dnxc_data_table_info_get_f appl_step_thresh_info_get;
} dnxf_data_if_dev_init_time_t;






typedef struct
{
    
    soc_mem_t mem;
} dnxf_data_dev_init_shadow_uncacheable_mem_t;



typedef enum
{

    
    _dnxf_data_dev_init_shadow_feature_nof
} dnxf_data_dev_init_shadow_feature_e;



typedef int(
    *dnxf_data_dev_init_shadow_feature_get_f) (
    int unit,
    dnxf_data_dev_init_shadow_feature_e feature);


typedef uint32(
    *dnxf_data_dev_init_shadow_cache_enable_all_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_shadow_cache_enable_ecc_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_shadow_cache_enable_parity_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_shadow_cache_enable_specific_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_shadow_cache_disable_specific_get_f) (
    int unit);


typedef const dnxf_data_dev_init_shadow_uncacheable_mem_t *(
    *dnxf_data_dev_init_shadow_uncacheable_mem_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnxf_data_dev_init_shadow_feature_get_f feature_get;
    
    dnxf_data_dev_init_shadow_cache_enable_all_get_f cache_enable_all_get;
    
    dnxf_data_dev_init_shadow_cache_enable_ecc_get_f cache_enable_ecc_get;
    
    dnxf_data_dev_init_shadow_cache_enable_parity_get_f cache_enable_parity_get;
    
    dnxf_data_dev_init_shadow_cache_enable_specific_get_f cache_enable_specific_get;
    
    dnxf_data_dev_init_shadow_cache_disable_specific_get_f cache_disable_specific_get;
    
    dnxf_data_dev_init_shadow_uncacheable_mem_get_f uncacheable_mem_get;
    
    dnxc_data_table_info_get_f uncacheable_mem_info_get;
} dnxf_data_if_dev_init_shadow_t;






typedef struct
{
    
    char *val;
} dnxf_data_dev_init_ha_stable_filename_t;



typedef enum
{

    
    _dnxf_data_dev_init_ha_feature_nof
} dnxf_data_dev_init_ha_feature_e;



typedef int(
    *dnxf_data_dev_init_ha_feature_get_f) (
    int unit,
    dnxf_data_dev_init_ha_feature_e feature);


typedef uint32(
    *dnxf_data_dev_init_ha_warmboot_support_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_ha_sw_state_max_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_ha_stable_location_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_ha_stable_size_get_f) (
    int unit);


typedef const dnxf_data_dev_init_ha_stable_filename_t *(
    *dnxf_data_dev_init_ha_stable_filename_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_dev_init_ha_feature_get_f feature_get;
    
    dnxf_data_dev_init_ha_warmboot_support_get_f warmboot_support_get;
    
    dnxf_data_dev_init_ha_sw_state_max_size_get_f sw_state_max_size_get;
    
    dnxf_data_dev_init_ha_stable_location_get_f stable_location_get;
    
    dnxf_data_dev_init_ha_stable_size_get_f stable_size_get;
    
    dnxf_data_dev_init_ha_stable_filename_get_f stable_filename_get;
    
    dnxc_data_table_info_get_f stable_filename_info_get;
} dnxf_data_if_dev_init_ha_t;





typedef struct
{
    
    dnxf_data_if_dev_init_time_t time;
    
    dnxf_data_if_dev_init_shadow_t shadow;
    
    dnxf_data_if_dev_init_ha_t ha;
} dnxf_data_if_dev_init_t;




extern dnxf_data_if_dev_init_t dnxf_data_dev_init;


#endif 

