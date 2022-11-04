
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_DEV_INIT_H_

#define _DNXF_DATA_INTERNAL_DEV_INIT_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



typedef enum
{
    dnxf_data_dev_init_submodule_time,
    dnxf_data_dev_init_submodule_shadow,
    dnxf_data_dev_init_submodule_ha,
    dnxf_data_dev_init_submodule_cmc,

    
    _dnxf_data_dev_init_submodule_nof
} dnxf_data_dev_init_submodule_e;








int dnxf_data_dev_init_time_feature_get(
    int unit,
    dnxf_data_dev_init_time_feature_e feature);



typedef enum
{
    dnxf_data_dev_init_time_define_init_total_time,
    dnxf_data_dev_init_time_define_appl_init_total_time,
    dnxf_data_dev_init_time_define_down_deviation_total,
    dnxf_data_dev_init_time_define_up_deviation_total,
    dnxf_data_dev_init_time_define_test_sensitivity,

    
    _dnxf_data_dev_init_time_define_nof
} dnxf_data_dev_init_time_define_e;



uint32 dnxf_data_dev_init_time_init_total_time_get(
    int unit);


uint32 dnxf_data_dev_init_time_appl_init_total_time_get(
    int unit);


uint32 dnxf_data_dev_init_time_down_deviation_total_get(
    int unit);


uint32 dnxf_data_dev_init_time_up_deviation_total_get(
    int unit);


uint32 dnxf_data_dev_init_time_test_sensitivity_get(
    int unit);



typedef enum
{
    dnxf_data_dev_init_time_table_step_time,
    dnxf_data_dev_init_time_table_appl_step_time,

    
    _dnxf_data_dev_init_time_table_nof
} dnxf_data_dev_init_time_table_e;



const dnxf_data_dev_init_time_step_time_t * dnxf_data_dev_init_time_step_time_get(
    int unit,
    int step_id);


const dnxf_data_dev_init_time_appl_step_time_t * dnxf_data_dev_init_time_appl_step_time_get(
    int unit,
    int step_id);



shr_error_e dnxf_data_dev_init_time_step_time_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_dev_init_time_appl_step_time_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_dev_init_time_step_time_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_dev_init_time_appl_step_time_info_get(
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
#ifdef BCM_ACCESS_SUPPORT

    dnxf_data_dev_init_shadow_table_uncacheable_regmem,

#endif 

    
    _dnxf_data_dev_init_shadow_table_nof
} dnxf_data_dev_init_shadow_table_e;



const dnxf_data_dev_init_shadow_uncacheable_mem_t * dnxf_data_dev_init_shadow_uncacheable_mem_get(
    int unit,
    int index);

#ifdef BCM_ACCESS_SUPPORT


const dnxf_data_dev_init_shadow_uncacheable_regmem_t * dnxf_data_dev_init_shadow_uncacheable_regmem_get(
    int unit,
    int index);

#endif 


shr_error_e dnxf_data_dev_init_shadow_uncacheable_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#ifdef BCM_ACCESS_SUPPORT


shr_error_e dnxf_data_dev_init_shadow_uncacheable_regmem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 


const dnxc_data_table_info_t * dnxf_data_dev_init_shadow_uncacheable_mem_info_get(
    int unit);

#ifdef BCM_ACCESS_SUPPORT


const dnxc_data_table_info_t * dnxf_data_dev_init_shadow_uncacheable_regmem_info_get(
    int unit);

#endif 





int dnxf_data_dev_init_ha_feature_get(
    int unit,
    dnxf_data_dev_init_ha_feature_e feature);



typedef enum
{
    dnxf_data_dev_init_ha_define_warmboot_backoff_rate,
    dnxf_data_dev_init_ha_define_nof_guaranteed_reboots,
    dnxf_data_dev_init_ha_define_warmboot_support,
    dnxf_data_dev_init_ha_define_sw_state_max_size,
    dnxf_data_dev_init_ha_define_stable_location,
    dnxf_data_dev_init_ha_define_stable_size,

    
    _dnxf_data_dev_init_ha_define_nof
} dnxf_data_dev_init_ha_define_e;



uint32 dnxf_data_dev_init_ha_warmboot_backoff_rate_get(
    int unit);


uint32 dnxf_data_dev_init_ha_nof_guaranteed_reboots_get(
    int unit);


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






int dnxf_data_dev_init_cmc_feature_get(
    int unit,
    dnxf_data_dev_init_cmc_feature_e feature);



typedef enum
{
    dnxf_data_dev_init_cmc_define_cmc_num,
    dnxf_data_dev_init_cmc_define_num_cpu_cosq,
    dnxf_data_dev_init_cmc_define_cmc_pci,
    dnxf_data_dev_init_cmc_define_pci_cmc_num,
    dnxf_data_dev_init_cmc_define_cmc_uc0,
    dnxf_data_dev_init_cmc_define_cmc_uc1,
    dnxf_data_dev_init_cmc_define_num_queues_pci,
    dnxf_data_dev_init_cmc_define_num_queues_uc0,
    dnxf_data_dev_init_cmc_define_num_queues_uc1,

    
    _dnxf_data_dev_init_cmc_define_nof
} dnxf_data_dev_init_cmc_define_e;



uint32 dnxf_data_dev_init_cmc_cmc_num_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_num_cpu_cosq_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_cmc_pci_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_pci_cmc_num_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_cmc_uc0_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_cmc_uc1_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_num_queues_pci_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_num_queues_uc0_get(
    int unit);


uint32 dnxf_data_dev_init_cmc_num_queues_uc1_get(
    int unit);



typedef enum
{

    
    _dnxf_data_dev_init_cmc_table_nof
} dnxf_data_dev_init_cmc_table_e;






shr_error_e dnxf_data_dev_init_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

