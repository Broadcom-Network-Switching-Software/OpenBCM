
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_L2_H_

#define _DNX_DATA_INTERNAL_L2_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_l2_submodule_general,
    dnx_data_l2_submodule_feature,
    dnx_data_l2_submodule_vsi,
    dnx_data_l2_submodule_vlan_domain,
    dnx_data_l2_submodule_dma,
    dnx_data_l2_submodule_age_and_flush_machine,
    dnx_data_l2_submodule_olp,

    
    _dnx_data_l2_submodule_nof
} dnx_data_l2_submodule_e;








int dnx_data_l2_general_feature_get(
    int unit,
    dnx_data_l2_general_feature_e feature);



typedef enum
{
    dnx_data_l2_general_define_vsi_offset_shift,
    dnx_data_l2_general_define_lif_offset_shift,
    dnx_data_l2_general_define_lem_nof_dbs,
    dnx_data_l2_general_define_l2_learn_limit_mode,
    dnx_data_l2_general_define_jr_mode_nof_fec_bits,
    dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits,
    dnx_data_l2_general_define_l2_egress_max_extention_size_bytes,
    dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes,
    dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int,
    dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int,
    dnx_data_l2_general_define_l2_api_supported_flags2,
    dnx_data_l2_general_define_l2_traverse_api_supported_flags,
    dnx_data_l2_general_define_l2_delete_api_supported_flags,
    dnx_data_l2_general_define_l2_add_get_apis_supported_flags,

    
    _dnx_data_l2_general_define_nof
} dnx_data_l2_general_define_e;



uint32 dnx_data_l2_general_vsi_offset_shift_get(
    int unit);


uint32 dnx_data_l2_general_lif_offset_shift_get(
    int unit);


uint32 dnx_data_l2_general_lem_nof_dbs_get(
    int unit);


uint32 dnx_data_l2_general_l2_learn_limit_mode_get(
    int unit);


uint32 dnx_data_l2_general_jr_mode_nof_fec_bits_get(
    int unit);


uint32 dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get(
    int unit);


uint32 dnx_data_l2_general_l2_egress_max_extention_size_bytes_get(
    int unit);


uint32 dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get(
    int unit);


uint32 dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_get(
    int unit);


uint32 dnx_data_l2_general_mact_lela_fid_exceed_limit_int_get(
    int unit);


uint32 dnx_data_l2_general_l2_api_supported_flags2_get(
    int unit);


uint32 dnx_data_l2_general_l2_traverse_api_supported_flags_get(
    int unit);


uint32 dnx_data_l2_general_l2_delete_api_supported_flags_get(
    int unit);


uint32 dnx_data_l2_general_l2_add_get_apis_supported_flags_get(
    int unit);



typedef enum
{
    dnx_data_l2_general_table_mact_result_type_map,
    dnx_data_l2_general_table_scan_data_bases_info,
    dnx_data_l2_general_table_fwd_mact_info,
    dnx_data_l2_general_table_lem_table_map_l2_action,

    
    _dnx_data_l2_general_table_nof
} dnx_data_l2_general_table_e;



const dnx_data_l2_general_mact_result_type_map_t * dnx_data_l2_general_mact_result_type_map_get(
    int unit,
    int forward_learning_db,
    int vlan_learning_mode);


const dnx_data_l2_general_scan_data_bases_info_t * dnx_data_l2_general_scan_data_bases_info_get(
    int unit,
    int data_base);


const dnx_data_l2_general_fwd_mact_info_t * dnx_data_l2_general_fwd_mact_info_get(
    int unit,
    int l2_action_type,
    int is_ivl);


const dnx_data_l2_general_lem_table_map_l2_action_t * dnx_data_l2_general_lem_table_map_l2_action_get(
    int unit,
    int logical_data_base);



shr_error_e dnx_data_l2_general_mact_result_type_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l2_general_scan_data_bases_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l2_general_fwd_mact_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l2_general_lem_table_map_l2_action_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l2_general_mact_result_type_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l2_general_scan_data_bases_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l2_general_fwd_mact_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l2_general_lem_table_map_l2_action_info_get(
    int unit);






int dnx_data_l2_feature_feature_get(
    int unit,
    dnx_data_l2_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_l2_feature_define_nof
} dnx_data_l2_feature_define_e;




typedef enum
{

    
    _dnx_data_l2_feature_table_nof
} dnx_data_l2_feature_table_e;









int dnx_data_l2_vsi_feature_get(
    int unit,
    dnx_data_l2_vsi_feature_e feature);



typedef enum
{
    dnx_data_l2_vsi_define_nof_vsi_aging_profiles,
    dnx_data_l2_vsi_define_nof_event_forwarding_profiles,
    dnx_data_l2_vsi_define_nof_vsi_learning_profiles,
    dnx_data_l2_vsi_define_vsi_table,

    
    _dnx_data_l2_vsi_define_nof
} dnx_data_l2_vsi_define_e;



uint32 dnx_data_l2_vsi_nof_vsi_aging_profiles_get(
    int unit);


uint32 dnx_data_l2_vsi_nof_event_forwarding_profiles_get(
    int unit);


uint32 dnx_data_l2_vsi_nof_vsi_learning_profiles_get(
    int unit);


uint32 dnx_data_l2_vsi_vsi_table_get(
    int unit);



typedef enum
{

    
    _dnx_data_l2_vsi_table_nof
} dnx_data_l2_vsi_table_e;









int dnx_data_l2_vlan_domain_feature_get(
    int unit,
    dnx_data_l2_vlan_domain_feature_e feature);



typedef enum
{
    dnx_data_l2_vlan_domain_define_nof_vlan_domains,
    dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain,

    
    _dnx_data_l2_vlan_domain_define_nof
} dnx_data_l2_vlan_domain_define_e;



uint32 dnx_data_l2_vlan_domain_nof_vlan_domains_get(
    int unit);


uint32 dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_get(
    int unit);



typedef enum
{

    
    _dnx_data_l2_vlan_domain_table_nof
} dnx_data_l2_vlan_domain_table_e;









int dnx_data_l2_dma_feature_get(
    int unit,
    dnx_data_l2_dma_feature_e feature);



typedef enum
{
    dnx_data_l2_dma_define_flush_nof_dma_entries,
    dnx_data_l2_dma_define_flush_db_nof_dma_rules,
    dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table,
    dnx_data_l2_dma_define_flush_db_rule_size,
    dnx_data_l2_dma_define_flush_db_data_size,
    dnx_data_l2_dma_define_flush_group_size,
    dnx_data_l2_dma_define_learning_fifo_dma_buffer_size,
    dnx_data_l2_dma_define_learning_fifo_dma_timeout,
    dnx_data_l2_dma_define_learning_fifo_dma_threshold,
    dnx_data_l2_dma_define_l2_dma_cpu_learn_thread_priority,

    
    _dnx_data_l2_dma_define_nof
} dnx_data_l2_dma_define_e;



uint32 dnx_data_l2_dma_flush_nof_dma_entries_get(
    int unit);


uint32 dnx_data_l2_dma_flush_db_nof_dma_rules_get(
    int unit);


uint32 dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_get(
    int unit);


uint32 dnx_data_l2_dma_flush_db_rule_size_get(
    int unit);


uint32 dnx_data_l2_dma_flush_db_data_size_get(
    int unit);


uint32 dnx_data_l2_dma_flush_group_size_get(
    int unit);


uint32 dnx_data_l2_dma_learning_fifo_dma_buffer_size_get(
    int unit);


uint32 dnx_data_l2_dma_learning_fifo_dma_timeout_get(
    int unit);


uint32 dnx_data_l2_dma_learning_fifo_dma_threshold_get(
    int unit);


uint32 dnx_data_l2_dma_l2_dma_cpu_learn_thread_priority_get(
    int unit);



typedef enum
{

    
    _dnx_data_l2_dma_table_nof
} dnx_data_l2_dma_table_e;









int dnx_data_l2_age_and_flush_machine_feature_get(
    int unit,
    dnx_data_l2_age_and_flush_machine_feature_e feature);



typedef enum
{
    dnx_data_l2_age_and_flush_machine_define_max_age_states,
    dnx_data_l2_age_and_flush_machine_define_flush_buffer_nof_entries,
    dnx_data_l2_age_and_flush_machine_define_traverse_thread_priority,

    
    _dnx_data_l2_age_and_flush_machine_define_nof
} dnx_data_l2_age_and_flush_machine_define_e;



uint32 dnx_data_l2_age_and_flush_machine_max_age_states_get(
    int unit);


uint32 dnx_data_l2_age_and_flush_machine_flush_buffer_nof_entries_get(
    int unit);


uint32 dnx_data_l2_age_and_flush_machine_traverse_thread_priority_get(
    int unit);



typedef enum
{
    dnx_data_l2_age_and_flush_machine_table_flush_pulse,
    dnx_data_l2_age_and_flush_machine_table_age,

    
    _dnx_data_l2_age_and_flush_machine_table_nof
} dnx_data_l2_age_and_flush_machine_table_e;



const dnx_data_l2_age_and_flush_machine_flush_pulse_t * dnx_data_l2_age_and_flush_machine_flush_pulse_get(
    int unit,
    int l2_action_type);


const dnx_data_l2_age_and_flush_machine_age_t * dnx_data_l2_age_and_flush_machine_age_get(
    int unit);



shr_error_e dnx_data_l2_age_and_flush_machine_flush_pulse_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_l2_age_and_flush_machine_age_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_l2_age_and_flush_machine_flush_pulse_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_l2_age_and_flush_machine_age_info_get(
    int unit);






int dnx_data_l2_olp_feature_get(
    int unit,
    dnx_data_l2_olp_feature_e feature);



typedef enum
{
    dnx_data_l2_olp_define_lpkgv_shift,
    dnx_data_l2_olp_define_lpkgv_mask,
    dnx_data_l2_olp_define_lpkgv_with_outlif,
    dnx_data_l2_olp_define_lpkgv_wo_outlif,
    dnx_data_l2_olp_define_destination_offset,
    dnx_data_l2_olp_define_outlif_offset,
    dnx_data_l2_olp_define_eei_offset,
    dnx_data_l2_olp_define_fec_offset,
    dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable,
    dnx_data_l2_olp_define_olp_learning_block_id,
    dnx_data_l2_olp_define_olp_fwd_block_id,

    
    _dnx_data_l2_olp_define_nof
} dnx_data_l2_olp_define_e;



uint32 dnx_data_l2_olp_lpkgv_shift_get(
    int unit);


uint32 dnx_data_l2_olp_lpkgv_mask_get(
    int unit);


uint32 dnx_data_l2_olp_lpkgv_with_outlif_get(
    int unit);


uint32 dnx_data_l2_olp_lpkgv_wo_outlif_get(
    int unit);


uint32 dnx_data_l2_olp_destination_offset_get(
    int unit);


uint32 dnx_data_l2_olp_outlif_offset_get(
    int unit);


uint32 dnx_data_l2_olp_eei_offset_get(
    int unit);


uint32 dnx_data_l2_olp_fec_offset_get(
    int unit);


uint32 dnx_data_l2_olp_jr_mode_enhanced_performance_enable_get(
    int unit);


uint32 dnx_data_l2_olp_olp_learning_block_id_get(
    int unit);


uint32 dnx_data_l2_olp_olp_fwd_block_id_get(
    int unit);



typedef enum
{

    
    _dnx_data_l2_olp_table_nof
} dnx_data_l2_olp_table_e;






shr_error_e dnx_data_l2_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

