

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DEV_INIT_H_

#define _DNX_DATA_DEV_INIT_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <soc/dnx/dnx_data/dnx_data_verify.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dev_init.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_dev_init_init(
    int unit);






typedef struct
{
    
    uint32 value;
} dnx_data_dev_init_time_step_thresh_t;


typedef struct
{
    
    uint32 value;
} dnx_data_dev_init_time_appl_step_thresh_t;



typedef enum
{

    
    _dnx_data_dev_init_time_feature_nof
} dnx_data_dev_init_time_feature_e;



typedef int(
    *dnx_data_dev_init_time_feature_get_f) (
    int unit,
    dnx_data_dev_init_time_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_time_analyze_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_time_init_total_thresh_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_time_appl_init_total_thresh_get_f) (
    int unit);


typedef const dnx_data_dev_init_time_step_thresh_t *(
    *dnx_data_dev_init_time_step_thresh_get_f) (
    int unit,
    int step_id);


typedef const dnx_data_dev_init_time_appl_step_thresh_t *(
    *dnx_data_dev_init_time_appl_step_thresh_get_f) (
    int unit,
    int step_id);



typedef struct
{
    
    dnx_data_dev_init_time_feature_get_f feature_get;
    
    dnx_data_dev_init_time_analyze_get_f analyze_get;
    
    dnx_data_dev_init_time_init_total_thresh_get_f init_total_thresh_get;
    
    dnx_data_dev_init_time_appl_init_total_thresh_get_f appl_init_total_thresh_get;
    
    dnx_data_dev_init_time_step_thresh_get_f step_thresh_get;
    
    dnxc_data_table_info_get_f step_thresh_info_get;
    
    dnx_data_dev_init_time_appl_step_thresh_get_f appl_step_thresh_get;
    
    dnxc_data_table_info_get_f appl_step_thresh_info_get;
} dnx_data_if_dev_init_time_t;






typedef struct
{
    
    soc_mem_t mem;
    
    dnx_init_mem_default_mode_e mode;
    
    dnx_init_mem_default_get_f default_get_cb;
    
    soc_field_t field;
} dnx_data_dev_init_mem_default_t;


typedef struct
{
    
    char *full_path;
} dnx_data_dev_init_mem_emul_ext_init_path_t;



typedef enum
{

    
    _dnx_data_dev_init_mem_feature_nof
} dnx_data_dev_init_mem_feature_e;



typedef int(
    *dnx_data_dev_init_mem_feature_get_f) (
    int unit,
    dnx_data_dev_init_mem_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_mem_force_zeros_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_mem_reset_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_mem_defaults_verify_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_mem_emul_ext_init_get_f) (
    int unit);


typedef const dnx_data_dev_init_mem_default_t *(
    *dnx_data_dev_init_mem_default_get_f) (
    int unit,
    int index);


typedef const dnx_data_dev_init_mem_emul_ext_init_path_t *(
    *dnx_data_dev_init_mem_emul_ext_init_path_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dev_init_mem_feature_get_f feature_get;
    
    dnx_data_dev_init_mem_force_zeros_get_f force_zeros_get;
    
    dnx_data_dev_init_mem_reset_mode_get_f reset_mode_get;
    
    dnx_data_dev_init_mem_defaults_verify_get_f defaults_verify_get;
    
    dnx_data_dev_init_mem_emul_ext_init_get_f emul_ext_init_get;
    
    dnx_data_dev_init_mem_default_get_f default_get;
    
    dnxc_data_table_info_get_f default_info_get;
    
    dnx_data_dev_init_mem_emul_ext_init_path_get_f emul_ext_init_path_get;
    
    dnxc_data_table_info_get_f emul_ext_init_path_info_get;
} dnx_data_if_dev_init_mem_t;






typedef struct
{
    
    char *property;
    
    char *suffix;
    
    int num_max;
    
    int per_port;
    
    char *err_msg;
} dnx_data_dev_init_properties_unsupported_t;



typedef enum
{

    
    _dnx_data_dev_init_properties_feature_nof
} dnx_data_dev_init_properties_feature_e;



typedef int(
    *dnx_data_dev_init_properties_feature_get_f) (
    int unit,
    dnx_data_dev_init_properties_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_properties_name_max_get_f) (
    int unit);


typedef const dnx_data_dev_init_properties_unsupported_t *(
    *dnx_data_dev_init_properties_unsupported_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_dev_init_properties_feature_get_f feature_get;
    
    dnx_data_dev_init_properties_name_max_get_f name_max_get;
    
    dnx_data_dev_init_properties_unsupported_get_f unsupported_get;
    
    dnxc_data_table_info_get_f unsupported_info_get;
} dnx_data_if_dev_init_properties_t;







typedef enum
{
    
    dnx_data_dev_init_general_data_path_hw,
    
    dnx_data_dev_init_general_remove_crc_bytes,
    
    dnx_data_dev_init_general_remove_crc_bytes_capability,
    
    dnx_data_dev_init_general_erpp_compensate_crc_size,
    
    dnx_data_dev_init_general_etpp_compensate_crc_size,
    
    dnx_data_dev_init_general_tail_editing_enable,
    
    dnx_data_dev_init_general_tail_editing_append_enable,
    
    dnx_data_dev_init_general_tail_editing_truncate_enable,
    
    dnx_data_dev_init_general_packet_size_compensation_term,
    
    dnx_data_dev_init_general_current_packet_size_compensation_term,
    
    dnx_data_dev_init_general_erpp_filters_non_separate_enablers,
    
    dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable,
    
    dnx_data_dev_init_general_l4_protocol_fields_config_enable,
    
    dnx_data_dev_init_general_da_type_map_enable,
    
    dnx_data_dev_init_general_ecologic_support,
    
    dnx_data_dev_init_general_context_selection_index_valid,
    
    dnx_data_dev_init_general_network_header_termination,
    
    dnx_data_dev_init_general_network_offset_for_system_headers_valid,

    
    _dnx_data_dev_init_general_feature_nof
} dnx_data_dev_init_general_feature_e;



typedef int(
    *dnx_data_dev_init_general_feature_get_f) (
    int unit,
    dnx_data_dev_init_general_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_general_access_only_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_general_heat_up_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_general_flexe_core_drv_select_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dev_init_general_feature_get_f feature_get;
    
    dnx_data_dev_init_general_access_only_get_f access_only_get;
    
    dnx_data_dev_init_general_heat_up_get_f heat_up_get;
    
    dnx_data_dev_init_general_flexe_core_drv_select_get_f flexe_core_drv_select_get;
} dnx_data_if_dev_init_general_t;







typedef enum
{
    
    dnx_data_dev_init_context_context_selection_profile,
    
    dnx_data_dev_init_context_oam_sub_type_in_context_selection,
    
    dnx_data_dev_init_context_int_profile_in_context_selection,

    
    _dnx_data_dev_init_context_feature_nof
} dnx_data_dev_init_context_feature_e;



typedef int(
    *dnx_data_dev_init_context_feature_get_f) (
    int unit,
    dnx_data_dev_init_context_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_context_forwarding_context_selection_mask_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_context_forwarding_context_selection_result_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_context_termination_context_selection_mask_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_context_termination_context_selection_result_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_context_trap_context_selection_mask_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_context_trap_context_selection_result_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_context_fwd_reycle_priority_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dev_init_context_feature_get_f feature_get;
    
    dnx_data_dev_init_context_forwarding_context_selection_mask_offset_get_f forwarding_context_selection_mask_offset_get;
    
    dnx_data_dev_init_context_forwarding_context_selection_result_offset_get_f forwarding_context_selection_result_offset_get;
    
    dnx_data_dev_init_context_termination_context_selection_mask_offset_get_f termination_context_selection_mask_offset_get;
    
    dnx_data_dev_init_context_termination_context_selection_result_offset_get_f termination_context_selection_result_offset_get;
    
    dnx_data_dev_init_context_trap_context_selection_mask_offset_get_f trap_context_selection_mask_offset_get;
    
    dnx_data_dev_init_context_trap_context_selection_result_offset_get_f trap_context_selection_result_offset_get;
    
    dnx_data_dev_init_context_fwd_reycle_priority_size_get_f fwd_reycle_priority_size_get;
} dnx_data_if_dev_init_context_t;






typedef struct
{
    
    char *val;
} dnx_data_dev_init_ha_stable_filename_t;



typedef enum
{

    
    _dnx_data_dev_init_ha_feature_nof
} dnx_data_dev_init_ha_feature_e;



typedef int(
    *dnx_data_dev_init_ha_feature_get_f) (
    int unit,
    dnx_data_dev_init_ha_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_ha_warmboot_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_ha_sw_state_max_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_ha_stable_location_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_ha_stable_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_ha_error_recovery_support_get_f) (
    int unit);


typedef const dnx_data_dev_init_ha_stable_filename_t *(
    *dnx_data_dev_init_ha_stable_filename_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dev_init_ha_feature_get_f feature_get;
    
    dnx_data_dev_init_ha_warmboot_support_get_f warmboot_support_get;
    
    dnx_data_dev_init_ha_sw_state_max_size_get_f sw_state_max_size_get;
    
    dnx_data_dev_init_ha_stable_location_get_f stable_location_get;
    
    dnx_data_dev_init_ha_stable_size_get_f stable_size_get;
    
    dnx_data_dev_init_ha_error_recovery_support_get_f error_recovery_support_get;
    
    dnx_data_dev_init_ha_stable_filename_get_f stable_filename_get;
    
    dnxc_data_table_info_get_f stable_filename_info_get;
} dnx_data_if_dev_init_ha_t;






typedef struct
{
    
    soc_mem_t mem;
} dnx_data_dev_init_shadow_uncacheable_mem_t;



typedef enum
{

    
    _dnx_data_dev_init_shadow_feature_nof
} dnx_data_dev_init_shadow_feature_e;



typedef int(
    *dnx_data_dev_init_shadow_feature_get_f) (
    int unit,
    dnx_data_dev_init_shadow_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_shadow_cache_enable_all_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_shadow_cache_enable_ecc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_shadow_cache_enable_parity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_shadow_cache_enable_specific_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_shadow_cache_disable_specific_get_f) (
    int unit);


typedef const dnx_data_dev_init_shadow_uncacheable_mem_t *(
    *dnx_data_dev_init_shadow_uncacheable_mem_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_dev_init_shadow_feature_get_f feature_get;
    
    dnx_data_dev_init_shadow_cache_enable_all_get_f cache_enable_all_get;
    
    dnx_data_dev_init_shadow_cache_enable_ecc_get_f cache_enable_ecc_get;
    
    dnx_data_dev_init_shadow_cache_enable_parity_get_f cache_enable_parity_get;
    
    dnx_data_dev_init_shadow_cache_enable_specific_get_f cache_enable_specific_get;
    
    dnx_data_dev_init_shadow_cache_disable_specific_get_f cache_disable_specific_get;
    
    dnx_data_dev_init_shadow_uncacheable_mem_get_f uncacheable_mem_get;
    
    dnxc_data_table_info_get_f uncacheable_mem_info_get;
} dnx_data_if_dev_init_shadow_t;







typedef enum
{

    
    _dnx_data_dev_init_cmc_feature_nof
} dnx_data_dev_init_cmc_feature_e;



typedef int(
    *dnx_data_dev_init_cmc_feature_get_f) (
    int unit,
    dnx_data_dev_init_cmc_feature_e feature);


typedef uint32(
    *dnx_data_dev_init_cmc_cmc_num_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_num_cpu_cosq_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_cmc_pci_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_pci_cmc_num_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_cmc_uc0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_cmc_uc1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_num_queues_pci_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_num_queues_uc0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dev_init_cmc_num_queues_uc1_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dev_init_cmc_feature_get_f feature_get;
    
    dnx_data_dev_init_cmc_cmc_num_get_f cmc_num_get;
    
    dnx_data_dev_init_cmc_num_cpu_cosq_get_f num_cpu_cosq_get;
    
    dnx_data_dev_init_cmc_cmc_pci_get_f cmc_pci_get;
    
    dnx_data_dev_init_cmc_pci_cmc_num_get_f pci_cmc_num_get;
    
    dnx_data_dev_init_cmc_cmc_uc0_get_f cmc_uc0_get;
    
    dnx_data_dev_init_cmc_cmc_uc1_get_f cmc_uc1_get;
    
    dnx_data_dev_init_cmc_num_queues_pci_get_f num_queues_pci_get;
    
    dnx_data_dev_init_cmc_num_queues_uc0_get_f num_queues_uc0_get;
    
    dnx_data_dev_init_cmc_num_queues_uc1_get_f num_queues_uc1_get;
} dnx_data_if_dev_init_cmc_t;





typedef struct
{
    
    dnx_data_if_dev_init_time_t time;
    
    dnx_data_if_dev_init_mem_t mem;
    
    dnx_data_if_dev_init_properties_t properties;
    
    dnx_data_if_dev_init_general_t general;
    
    dnx_data_if_dev_init_context_t context;
    
    dnx_data_if_dev_init_ha_t ha;
    
    dnx_data_if_dev_init_shadow_t shadow;
    
    dnx_data_if_dev_init_cmc_t cmc;
} dnx_data_if_dev_init_t;




extern dnx_data_if_dev_init_t dnx_data_dev_init;


#endif 

