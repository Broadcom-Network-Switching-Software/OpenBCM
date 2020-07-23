

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DEV_INIT_H_

#define _DNX_DATA_INTERNAL_DEV_INIT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_dev_init_submodule_time,
    dnx_data_dev_init_submodule_mem,
    dnx_data_dev_init_submodule_properties,
    dnx_data_dev_init_submodule_general,
    dnx_data_dev_init_submodule_context,
    dnx_data_dev_init_submodule_ha,
    dnx_data_dev_init_submodule_shadow,
    dnx_data_dev_init_submodule_cmc,

    
    _dnx_data_dev_init_submodule_nof
} dnx_data_dev_init_submodule_e;








int dnx_data_dev_init_time_feature_get(
    int unit,
    dnx_data_dev_init_time_feature_e feature);



typedef enum
{
    dnx_data_dev_init_time_define_analyze,
    dnx_data_dev_init_time_define_init_total_thresh,
    dnx_data_dev_init_time_define_appl_init_total_thresh,

    
    _dnx_data_dev_init_time_define_nof
} dnx_data_dev_init_time_define_e;



uint32 dnx_data_dev_init_time_analyze_get(
    int unit);


uint32 dnx_data_dev_init_time_init_total_thresh_get(
    int unit);


uint32 dnx_data_dev_init_time_appl_init_total_thresh_get(
    int unit);



typedef enum
{
    dnx_data_dev_init_time_table_step_thresh,
    dnx_data_dev_init_time_table_appl_step_thresh,

    
    _dnx_data_dev_init_time_table_nof
} dnx_data_dev_init_time_table_e;



const dnx_data_dev_init_time_step_thresh_t * dnx_data_dev_init_time_step_thresh_get(
    int unit,
    int step_id);


const dnx_data_dev_init_time_appl_step_thresh_t * dnx_data_dev_init_time_appl_step_thresh_get(
    int unit,
    int step_id);



shr_error_e dnx_data_dev_init_time_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dev_init_time_appl_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dev_init_time_step_thresh_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dev_init_time_appl_step_thresh_info_get(
    int unit);






int dnx_data_dev_init_mem_feature_get(
    int unit,
    dnx_data_dev_init_mem_feature_e feature);



typedef enum
{
    dnx_data_dev_init_mem_define_force_zeros,
    dnx_data_dev_init_mem_define_reset_mode,
    dnx_data_dev_init_mem_define_defaults_verify,
    dnx_data_dev_init_mem_define_emul_ext_init,

    
    _dnx_data_dev_init_mem_define_nof
} dnx_data_dev_init_mem_define_e;



uint32 dnx_data_dev_init_mem_force_zeros_get(
    int unit);


uint32 dnx_data_dev_init_mem_reset_mode_get(
    int unit);


uint32 dnx_data_dev_init_mem_defaults_verify_get(
    int unit);


uint32 dnx_data_dev_init_mem_emul_ext_init_get(
    int unit);



typedef enum
{
    dnx_data_dev_init_mem_table_default,
    dnx_data_dev_init_mem_table_emul_ext_init_path,

    
    _dnx_data_dev_init_mem_table_nof
} dnx_data_dev_init_mem_table_e;



const dnx_data_dev_init_mem_default_t * dnx_data_dev_init_mem_default_get(
    int unit,
    int index);


const dnx_data_dev_init_mem_emul_ext_init_path_t * dnx_data_dev_init_mem_emul_ext_init_path_get(
    int unit);



shr_error_e dnx_data_dev_init_mem_default_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dev_init_mem_emul_ext_init_path_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dev_init_mem_default_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dev_init_mem_emul_ext_init_path_info_get(
    int unit);






int dnx_data_dev_init_properties_feature_get(
    int unit,
    dnx_data_dev_init_properties_feature_e feature);



typedef enum
{
    dnx_data_dev_init_properties_define_name_max,

    
    _dnx_data_dev_init_properties_define_nof
} dnx_data_dev_init_properties_define_e;



uint32 dnx_data_dev_init_properties_name_max_get(
    int unit);



typedef enum
{
    dnx_data_dev_init_properties_table_unsupported,

    
    _dnx_data_dev_init_properties_table_nof
} dnx_data_dev_init_properties_table_e;



const dnx_data_dev_init_properties_unsupported_t * dnx_data_dev_init_properties_unsupported_get(
    int unit,
    int index);



shr_error_e dnx_data_dev_init_properties_unsupported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dev_init_properties_unsupported_info_get(
    int unit);






int dnx_data_dev_init_general_feature_get(
    int unit,
    dnx_data_dev_init_general_feature_e feature);



typedef enum
{
    dnx_data_dev_init_general_define_access_only,
    dnx_data_dev_init_general_define_heat_up,
    dnx_data_dev_init_general_define_flexe_core_drv_select,

    
    _dnx_data_dev_init_general_define_nof
} dnx_data_dev_init_general_define_e;



uint32 dnx_data_dev_init_general_access_only_get(
    int unit);


uint32 dnx_data_dev_init_general_heat_up_get(
    int unit);


uint32 dnx_data_dev_init_general_flexe_core_drv_select_get(
    int unit);



typedef enum
{

    
    _dnx_data_dev_init_general_table_nof
} dnx_data_dev_init_general_table_e;









int dnx_data_dev_init_context_feature_get(
    int unit,
    dnx_data_dev_init_context_feature_e feature);



typedef enum
{
    dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset,
    dnx_data_dev_init_context_define_forwarding_context_selection_result_offset,
    dnx_data_dev_init_context_define_termination_context_selection_mask_offset,
    dnx_data_dev_init_context_define_termination_context_selection_result_offset,
    dnx_data_dev_init_context_define_trap_context_selection_mask_offset,
    dnx_data_dev_init_context_define_trap_context_selection_result_offset,
    dnx_data_dev_init_context_define_fwd_reycle_priority_size,

    
    _dnx_data_dev_init_context_define_nof
} dnx_data_dev_init_context_define_e;



uint32 dnx_data_dev_init_context_forwarding_context_selection_mask_offset_get(
    int unit);


uint32 dnx_data_dev_init_context_forwarding_context_selection_result_offset_get(
    int unit);


uint32 dnx_data_dev_init_context_termination_context_selection_mask_offset_get(
    int unit);


uint32 dnx_data_dev_init_context_termination_context_selection_result_offset_get(
    int unit);


uint32 dnx_data_dev_init_context_trap_context_selection_mask_offset_get(
    int unit);


uint32 dnx_data_dev_init_context_trap_context_selection_result_offset_get(
    int unit);


uint32 dnx_data_dev_init_context_fwd_reycle_priority_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_dev_init_context_table_nof
} dnx_data_dev_init_context_table_e;









int dnx_data_dev_init_ha_feature_get(
    int unit,
    dnx_data_dev_init_ha_feature_e feature);



typedef enum
{
    dnx_data_dev_init_ha_define_warmboot_support,
    dnx_data_dev_init_ha_define_sw_state_max_size,
    dnx_data_dev_init_ha_define_stable_location,
    dnx_data_dev_init_ha_define_stable_size,
    dnx_data_dev_init_ha_define_error_recovery_support,

    
    _dnx_data_dev_init_ha_define_nof
} dnx_data_dev_init_ha_define_e;



uint32 dnx_data_dev_init_ha_warmboot_support_get(
    int unit);


uint32 dnx_data_dev_init_ha_sw_state_max_size_get(
    int unit);


uint32 dnx_data_dev_init_ha_stable_location_get(
    int unit);


uint32 dnx_data_dev_init_ha_stable_size_get(
    int unit);


uint32 dnx_data_dev_init_ha_error_recovery_support_get(
    int unit);



typedef enum
{
    dnx_data_dev_init_ha_table_stable_filename,

    
    _dnx_data_dev_init_ha_table_nof
} dnx_data_dev_init_ha_table_e;



const dnx_data_dev_init_ha_stable_filename_t * dnx_data_dev_init_ha_stable_filename_get(
    int unit);



shr_error_e dnx_data_dev_init_ha_stable_filename_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dev_init_ha_stable_filename_info_get(
    int unit);






int dnx_data_dev_init_shadow_feature_get(
    int unit,
    dnx_data_dev_init_shadow_feature_e feature);



typedef enum
{
    dnx_data_dev_init_shadow_define_cache_enable_all,
    dnx_data_dev_init_shadow_define_cache_enable_ecc,
    dnx_data_dev_init_shadow_define_cache_enable_parity,
    dnx_data_dev_init_shadow_define_cache_enable_specific,
    dnx_data_dev_init_shadow_define_cache_disable_specific,

    
    _dnx_data_dev_init_shadow_define_nof
} dnx_data_dev_init_shadow_define_e;



uint32 dnx_data_dev_init_shadow_cache_enable_all_get(
    int unit);


uint32 dnx_data_dev_init_shadow_cache_enable_ecc_get(
    int unit);


uint32 dnx_data_dev_init_shadow_cache_enable_parity_get(
    int unit);


uint32 dnx_data_dev_init_shadow_cache_enable_specific_get(
    int unit);


uint32 dnx_data_dev_init_shadow_cache_disable_specific_get(
    int unit);



typedef enum
{
    dnx_data_dev_init_shadow_table_uncacheable_mem,

    
    _dnx_data_dev_init_shadow_table_nof
} dnx_data_dev_init_shadow_table_e;



const dnx_data_dev_init_shadow_uncacheable_mem_t * dnx_data_dev_init_shadow_uncacheable_mem_get(
    int unit,
    int index);



shr_error_e dnx_data_dev_init_shadow_uncacheable_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dev_init_shadow_uncacheable_mem_info_get(
    int unit);






int dnx_data_dev_init_cmc_feature_get(
    int unit,
    dnx_data_dev_init_cmc_feature_e feature);



typedef enum
{
    dnx_data_dev_init_cmc_define_cmc_num,
    dnx_data_dev_init_cmc_define_num_cpu_cosq,
    dnx_data_dev_init_cmc_define_cmc_pci,
    dnx_data_dev_init_cmc_define_pci_cmc_num,
    dnx_data_dev_init_cmc_define_cmc_uc0,
    dnx_data_dev_init_cmc_define_cmc_uc1,
    dnx_data_dev_init_cmc_define_num_queues_pci,
    dnx_data_dev_init_cmc_define_num_queues_uc0,
    dnx_data_dev_init_cmc_define_num_queues_uc1,

    
    _dnx_data_dev_init_cmc_define_nof
} dnx_data_dev_init_cmc_define_e;



uint32 dnx_data_dev_init_cmc_cmc_num_get(
    int unit);


uint32 dnx_data_dev_init_cmc_num_cpu_cosq_get(
    int unit);


uint32 dnx_data_dev_init_cmc_cmc_pci_get(
    int unit);


uint32 dnx_data_dev_init_cmc_pci_cmc_num_get(
    int unit);


uint32 dnx_data_dev_init_cmc_cmc_uc0_get(
    int unit);


uint32 dnx_data_dev_init_cmc_cmc_uc1_get(
    int unit);


uint32 dnx_data_dev_init_cmc_num_queues_pci_get(
    int unit);


uint32 dnx_data_dev_init_cmc_num_queues_uc0_get(
    int unit);


uint32 dnx_data_dev_init_cmc_num_queues_uc1_get(
    int unit);



typedef enum
{

    
    _dnx_data_dev_init_cmc_table_nof
} dnx_data_dev_init_cmc_table_e;






shr_error_e dnx_data_dev_init_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

