
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_DEV_INIT_H_

#define _DNXF_DATA_DEV_INIT_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_dev_init.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e dnxf_data_if_dev_init_init(
    int unit);






typedef struct
{
    uint32 time;
    uint8 down_deviation;
    uint8 up_deviation;
    uint32 non_perc_up_deviation;
} dnxf_data_dev_init_time_step_time_t;


typedef struct
{
    uint32 time;
    uint8 down_deviation;
    uint8 up_deviation;
    uint32 non_perc_up_deviation;
} dnxf_data_dev_init_time_appl_step_time_t;



typedef enum
{

    
    _dnxf_data_dev_init_time_feature_nof
} dnxf_data_dev_init_time_feature_e;



typedef int(
    *dnxf_data_dev_init_time_feature_get_f) (
    int unit,
    dnxf_data_dev_init_time_feature_e feature);


typedef uint32(
    *dnxf_data_dev_init_time_init_total_time_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_time_appl_init_total_time_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_time_down_deviation_total_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_time_up_deviation_total_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_time_test_sensitivity_get_f) (
    int unit);


typedef const dnxf_data_dev_init_time_step_time_t *(
    *dnxf_data_dev_init_time_step_time_get_f) (
    int unit,
    int step_id);


typedef const dnxf_data_dev_init_time_appl_step_time_t *(
    *dnxf_data_dev_init_time_appl_step_time_get_f) (
    int unit,
    int step_id);



typedef struct
{
    
    dnxf_data_dev_init_time_feature_get_f feature_get;
    
    dnxf_data_dev_init_time_init_total_time_get_f init_total_time_get;
    
    dnxf_data_dev_init_time_appl_init_total_time_get_f appl_init_total_time_get;
    
    dnxf_data_dev_init_time_down_deviation_total_get_f down_deviation_total_get;
    
    dnxf_data_dev_init_time_up_deviation_total_get_f up_deviation_total_get;
    
    dnxf_data_dev_init_time_test_sensitivity_get_f test_sensitivity_get;
    
    dnxf_data_dev_init_time_step_time_get_f step_time_get;
    
    dnxc_data_table_info_get_f step_time_info_get;
    
    dnxf_data_dev_init_time_appl_step_time_get_f appl_step_time_get;
    
    dnxc_data_table_info_get_f appl_step_time_info_get;
} dnxf_data_if_dev_init_time_t;






typedef struct
{
    soc_mem_t mem;
    int is_valid;
} dnxf_data_dev_init_shadow_uncacheable_mem_t;

#ifdef BCM_ACCESS_SUPPORT


typedef struct
{
    uint32 regmem;
    int is_valid;
} dnxf_data_dev_init_shadow_uncacheable_regmem_t;


#endif 


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

#ifdef BCM_ACCESS_SUPPORT


typedef const dnxf_data_dev_init_shadow_uncacheable_regmem_t *(
    *dnxf_data_dev_init_shadow_uncacheable_regmem_get_f) (
    int unit,
    int index);

#endif 


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
#ifdef BCM_ACCESS_SUPPORT

    
    dnxf_data_dev_init_shadow_uncacheable_regmem_get_f uncacheable_regmem_get;

#endif 
#ifdef BCM_ACCESS_SUPPORT

    
    dnxc_data_table_info_get_f uncacheable_regmem_info_get;

#endif 
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
    *dnxf_data_dev_init_ha_warmboot_backoff_rate_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_ha_nof_guaranteed_reboots_get_f) (
    int unit);


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
    
    dnxf_data_dev_init_ha_warmboot_backoff_rate_get_f warmboot_backoff_rate_get;
    
    dnxf_data_dev_init_ha_nof_guaranteed_reboots_get_f nof_guaranteed_reboots_get;
    
    dnxf_data_dev_init_ha_warmboot_support_get_f warmboot_support_get;
    
    dnxf_data_dev_init_ha_sw_state_max_size_get_f sw_state_max_size_get;
    
    dnxf_data_dev_init_ha_stable_location_get_f stable_location_get;
    
    dnxf_data_dev_init_ha_stable_size_get_f stable_size_get;
    
    dnxf_data_dev_init_ha_stable_filename_get_f stable_filename_get;
    
    dnxc_data_table_info_get_f stable_filename_info_get;
} dnxf_data_if_dev_init_ha_t;







typedef enum
{

    
    _dnxf_data_dev_init_cmc_feature_nof
} dnxf_data_dev_init_cmc_feature_e;



typedef int(
    *dnxf_data_dev_init_cmc_feature_get_f) (
    int unit,
    dnxf_data_dev_init_cmc_feature_e feature);


typedef uint32(
    *dnxf_data_dev_init_cmc_cmc_num_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_num_cpu_cosq_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_cmc_pci_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_pci_cmc_num_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_cmc_uc0_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_cmc_uc1_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_num_queues_pci_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_num_queues_uc0_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_dev_init_cmc_num_queues_uc1_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_dev_init_cmc_feature_get_f feature_get;
    
    dnxf_data_dev_init_cmc_cmc_num_get_f cmc_num_get;
    
    dnxf_data_dev_init_cmc_num_cpu_cosq_get_f num_cpu_cosq_get;
    
    dnxf_data_dev_init_cmc_cmc_pci_get_f cmc_pci_get;
    
    dnxf_data_dev_init_cmc_pci_cmc_num_get_f pci_cmc_num_get;
    
    dnxf_data_dev_init_cmc_cmc_uc0_get_f cmc_uc0_get;
    
    dnxf_data_dev_init_cmc_cmc_uc1_get_f cmc_uc1_get;
    
    dnxf_data_dev_init_cmc_num_queues_pci_get_f num_queues_pci_get;
    
    dnxf_data_dev_init_cmc_num_queues_uc0_get_f num_queues_uc0_get;
    
    dnxf_data_dev_init_cmc_num_queues_uc1_get_f num_queues_uc1_get;
} dnxf_data_if_dev_init_cmc_t;





typedef struct
{
    
    dnxf_data_if_dev_init_time_t time;
    
    dnxf_data_if_dev_init_shadow_t shadow;
    
    dnxf_data_if_dev_init_ha_t ha;
    
    dnxf_data_if_dev_init_cmc_t cmc;
} dnxf_data_if_dev_init_t;




extern dnxf_data_if_dev_init_t dnxf_data_dev_init;


#endif 

