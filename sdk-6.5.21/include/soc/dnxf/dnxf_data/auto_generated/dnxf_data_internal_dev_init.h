

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_DEV_INIT_H_

#define _DNXF_DATA_INTERNAL_DEV_INIT_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_dev_init_submodule_time,
    dnxf_data_dev_init_submodule_shadow,
    dnxf_data_dev_init_submodule_ha,

    
    _dnxf_data_dev_init_submodule_nof
} dnxf_data_dev_init_submodule_e;








int dnxf_data_dev_init_time_feature_get(
    int unit,
    dnxf_data_dev_init_time_feature_e feature);



typedef enum
{
    dnxf_data_dev_init_time_define_init_total_thresh,
    dnxf_data_dev_init_time_define_appl_init_total_thresh,

    
    _dnxf_data_dev_init_time_define_nof
} dnxf_data_dev_init_time_define_e;



uint32 dnxf_data_dev_init_time_init_total_thresh_get(
    int unit);


uint32 dnxf_data_dev_init_time_appl_init_total_thresh_get(
    int unit);



typedef enum
{
    dnxf_data_dev_init_time_table_step_thresh,
    dnxf_data_dev_init_time_table_appl_step_thresh,

    
    _dnxf_data_dev_init_time_table_nof
} dnxf_data_dev_init_time_table_e;



const dnxf_data_dev_init_time_step_thresh_t * dnxf_data_dev_init_time_step_thresh_get(
    int unit,
    int step_id);


const dnxf_data_dev_init_time_appl_step_thresh_t * dnxf_data_dev_init_time_appl_step_thresh_get(
    int unit,
    int step_id);



shr_error_e dnxf_data_dev_init_time_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_dev_init_time_appl_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_dev_init_time_step_thresh_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_dev_init_time_appl_step_thresh_info_get(
    int unit);






int dnxf_data_dev_init_shadow_feature_get(
    int unit,
    dnxf_data_dev_init_shadow_feature_e feature);



typedef enum
{
    dnxf_data_dev_init_shadow_define_cache_enable_all,
    dnxf_data_dev_init_shadow_define_cache_enable_ecc,
    dnxf_data_dev_init_shadow_define_cache_enable_parity,
    dnxf_data_dev_init_shadow_define_cache_enable_specific,
    dnxf_data_dev_init_shadow_define_cache_disable_specific,

    
    _dnxf_data_dev_init_shadow_define_nof
} dnxf_data_dev_init_shadow_define_e;



uint32 dnxf_data_dev_init_shadow_cache_enable_all_get(
    int unit);


uint32 dnxf_data_dev_init_shadow_cache_enable_ecc_get(
    int unit);


uint32 dnxf_data_dev_init_shadow_cache_enable_parity_get(
    int unit);


uint32 dnxf_data_dev_init_shadow_cache_enable_specific_get(
    int unit);


uint32 dnxf_data_dev_init_shadow_cache_disable_specific_get(
    int unit);



typedef enum
{
    dnxf_data_dev_init_shadow_table_uncacheable_mem,

    
    _dnxf_data_dev_init_shadow_table_nof
} dnxf_data_dev_init_shadow_table_e;



const dnxf_data_dev_init_shadow_uncacheable_mem_t * dnxf_data_dev_init_shadow_uncacheable_mem_get(
    int unit,
    int index);



shr_error_e dnxf_data_dev_init_shadow_uncacheable_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_dev_init_shadow_uncacheable_mem_info_get(
    int unit);






int dnxf_data_dev_init_ha_feature_get(
    int unit,
    dnxf_data_dev_init_ha_feature_e feature);



typedef enum
{
    dnxf_data_dev_init_ha_define_warmboot_support,
    dnxf_data_dev_init_ha_define_sw_state_max_size,
    dnxf_data_dev_init_ha_define_stable_location,
    dnxf_data_dev_init_ha_define_stable_size,

    
    _dnxf_data_dev_init_ha_define_nof
} dnxf_data_dev_init_ha_define_e;



uint32 dnxf_data_dev_init_ha_warmboot_support_get(
    int unit);


uint32 dnxf_data_dev_init_ha_sw_state_max_size_get(
    int unit);


uint32 dnxf_data_dev_init_ha_stable_location_get(
    int unit);


uint32 dnxf_data_dev_init_ha_stable_size_get(
    int unit);



typedef enum
{
    dnxf_data_dev_init_ha_table_stable_filename,

    
    _dnxf_data_dev_init_ha_table_nof
} dnxf_data_dev_init_ha_table_e;



const dnxf_data_dev_init_ha_stable_filename_t * dnxf_data_dev_init_ha_stable_filename_get(
    int unit);



shr_error_e dnxf_data_dev_init_ha_stable_filename_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_dev_init_ha_stable_filename_info_get(
    int unit);



shr_error_e dnxf_data_dev_init_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

