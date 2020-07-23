

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_L2_H_

#define _DNX_DATA_L2_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l2.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_l2_init(
    int unit);







typedef enum
{
    
    dnx_data_l2_general_learning_use_insert_cmd,
    
    dnx_data_l2_general_flush_machine_support,
    
    dnx_data_l2_general_dynamic_entries_iteration_support,
    
    dnx_data_l2_general_counters_support,
    
    dnx_data_l2_general_aging_support,

    
    _dnx_data_l2_general_feature_nof
} dnx_data_l2_general_feature_e;



typedef int(
    *dnx_data_l2_general_feature_get_f) (
    int unit,
    dnx_data_l2_general_feature_e feature);


typedef uint32(
    *dnx_data_l2_general_vsi_offset_shift_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_lif_offset_shift_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_learn_limit_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_jr_mode_nof_fec_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_egress_max_extention_size_bytes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_general_feature_get_f feature_get;
    
    dnx_data_l2_general_vsi_offset_shift_get_f vsi_offset_shift_get;
    
    dnx_data_l2_general_lif_offset_shift_get_f lif_offset_shift_get;
    
    dnx_data_l2_general_l2_learn_limit_mode_get_f l2_learn_limit_mode_get;
    
    dnx_data_l2_general_jr_mode_nof_fec_bits_get_f jr_mode_nof_fec_bits_get;
    
    dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get_f arad_plus_mode_nof_fec_bits_get;
    
    dnx_data_l2_general_l2_egress_max_extention_size_bytes_get_f l2_egress_max_extention_size_bytes_get;
    
    dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get_f l2_egress_max_additional_termination_size_bytes_get;
} dnx_data_if_l2_general_t;







typedef enum
{
    
    dnx_data_l2_feature_age_out_and_refresh_profile_selection,
    
    dnx_data_l2_feature_age_machine_pause_after_flush,
    
    dnx_data_l2_feature_age_state_not_updated,
    
    dnx_data_l2_feature_appdb_id_for_olp,
    
    dnx_data_l2_feature_eth_qual_is_mc,
    dnx_data_l2_feature_bc_same_as_unknown_mc,
    dnx_data_l2_feature_fid_mgmt_ecc_error,
    
    dnx_data_l2_feature_wrong_limit_interrupt_handling,
    
    dnx_data_l2_feature_transplant_instead_of_refresh,
    
    dnx_data_l2_feature_static_mac_age_out,
    
    dnx_data_l2_feature_vmv_for_limit_in_wrong_location,
    
    dnx_data_l2_feature_learn_limit,
    dnx_data_l2_feature_limit_per_lif_counters,
    dnx_data_l2_feature_learn_events_wrong_command,
    dnx_data_l2_feature_opportunistic_learning_always_transplant,
    dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd,
    dnx_data_l2_feature_refresh_events_wrong_key_msbs,
    dnx_data_l2_feature_ignore_limit_check,

    
    _dnx_data_l2_feature_feature_nof
} dnx_data_l2_feature_feature_e;



typedef int(
    *dnx_data_l2_feature_feature_get_f) (
    int unit,
    dnx_data_l2_feature_feature_e feature);



typedef struct
{
    
    dnx_data_l2_feature_feature_get_f feature_get;
} dnx_data_if_l2_feature_t;







typedef enum
{

    
    _dnx_data_l2_vsi_feature_nof
} dnx_data_l2_vsi_feature_e;



typedef int(
    *dnx_data_l2_vsi_feature_get_f) (
    int unit,
    dnx_data_l2_vsi_feature_e feature);


typedef uint32(
    *dnx_data_l2_vsi_nof_vsi_aging_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_vsi_nof_event_forwarding_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_vsi_nof_vsi_learning_profiles_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_vsi_feature_get_f feature_get;
    
    dnx_data_l2_vsi_nof_vsi_aging_profiles_get_f nof_vsi_aging_profiles_get;
    
    dnx_data_l2_vsi_nof_event_forwarding_profiles_get_f nof_event_forwarding_profiles_get;
    
    dnx_data_l2_vsi_nof_vsi_learning_profiles_get_f nof_vsi_learning_profiles_get;
} dnx_data_if_l2_vsi_t;







typedef enum
{

    
    _dnx_data_l2_vlan_domain_feature_nof
} dnx_data_l2_vlan_domain_feature_e;



typedef int(
    *dnx_data_l2_vlan_domain_feature_get_f) (
    int unit,
    dnx_data_l2_vlan_domain_feature_e feature);


typedef uint32(
    *dnx_data_l2_vlan_domain_nof_vlan_domains_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_vlan_domain_feature_get_f feature_get;
    
    dnx_data_l2_vlan_domain_nof_vlan_domains_get_f nof_vlan_domains_get;
    
    dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_get_f nof_bits_next_layer_network_domain_get;
} dnx_data_if_l2_vlan_domain_t;







typedef enum
{

    
    _dnx_data_l2_dma_feature_nof
} dnx_data_l2_dma_feature_e;



typedef int(
    *dnx_data_l2_dma_feature_get_f) (
    int unit,
    dnx_data_l2_dma_feature_e feature);


typedef uint32(
    *dnx_data_l2_dma_flush_nof_dma_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_flush_db_nof_dma_rules_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_flush_db_rule_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_flush_db_data_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_flush_group_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_learning_fifo_dma_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_learning_fifo_dma_timeout_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_dma_learning_fifo_dma_threshold_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_dma_feature_get_f feature_get;
    
    dnx_data_l2_dma_flush_nof_dma_entries_get_f flush_nof_dma_entries_get;
    
    dnx_data_l2_dma_flush_db_nof_dma_rules_get_f flush_db_nof_dma_rules_get;
    
    dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_get_f flush_db_nof_dma_rules_per_table_get;
    
    dnx_data_l2_dma_flush_db_rule_size_get_f flush_db_rule_size_get;
    
    dnx_data_l2_dma_flush_db_data_size_get_f flush_db_data_size_get;
    
    dnx_data_l2_dma_flush_group_size_get_f flush_group_size_get;
    
    dnx_data_l2_dma_learning_fifo_dma_buffer_size_get_f learning_fifo_dma_buffer_size_get;
    
    dnx_data_l2_dma_learning_fifo_dma_timeout_get_f learning_fifo_dma_timeout_get;
    
    dnx_data_l2_dma_learning_fifo_dma_threshold_get_f learning_fifo_dma_threshold_get;
} dnx_data_if_l2_dma_t;






typedef struct
{
    
    soc_mem_t table_name;
    
    soc_field_t valid;
    
    soc_field_t entry_mask;
    
    soc_field_t entry_filter;
    
    soc_field_t src_mask;
    
    soc_field_t src_filter;
    
    soc_field_t appdb_id_mask;
    
    soc_field_t appdb_id_filter;
    
    soc_field_t accessed_mask;
    
    soc_field_t accessed_filter;
} dnx_data_l2_age_and_flush_machine_filter_rules_t;


typedef struct
{
    
    soc_mem_t table_name;
    
    soc_field_t command;
    
    soc_field_t payload_mask;
    
    soc_field_t payload;
} dnx_data_l2_age_and_flush_machine_data_rules_t;


typedef struct
{
    
    soc_reg_t flush_pulse;
    
    uint32 traverse_thread_priority;
} dnx_data_l2_age_and_flush_machine_flush_t;


typedef struct
{
    
    soc_reg_t age_config;
    
    soc_field_t disable_aging;
    
    soc_reg_t scan_pulse;
} dnx_data_l2_age_and_flush_machine_age_t;



typedef enum
{

    
    _dnx_data_l2_age_and_flush_machine_feature_nof
} dnx_data_l2_age_and_flush_machine_feature_e;



typedef int(
    *dnx_data_l2_age_and_flush_machine_feature_get_f) (
    int unit,
    dnx_data_l2_age_and_flush_machine_feature_e feature);


typedef uint32(
    *dnx_data_l2_age_and_flush_machine_max_age_states_get_f) (
    int unit);


typedef const dnx_data_l2_age_and_flush_machine_filter_rules_t *(
    *dnx_data_l2_age_and_flush_machine_filter_rules_get_f) (
    int unit);


typedef const dnx_data_l2_age_and_flush_machine_data_rules_t *(
    *dnx_data_l2_age_and_flush_machine_data_rules_get_f) (
    int unit);


typedef const dnx_data_l2_age_and_flush_machine_flush_t *(
    *dnx_data_l2_age_and_flush_machine_flush_get_f) (
    int unit);


typedef const dnx_data_l2_age_and_flush_machine_age_t *(
    *dnx_data_l2_age_and_flush_machine_age_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_age_and_flush_machine_feature_get_f feature_get;
    
    dnx_data_l2_age_and_flush_machine_max_age_states_get_f max_age_states_get;
    
    dnx_data_l2_age_and_flush_machine_filter_rules_get_f filter_rules_get;
    
    dnxc_data_table_info_get_f filter_rules_info_get;
    
    dnx_data_l2_age_and_flush_machine_data_rules_get_f data_rules_get;
    
    dnxc_data_table_info_get_f data_rules_info_get;
    
    dnx_data_l2_age_and_flush_machine_flush_get_f flush_get;
    
    dnxc_data_table_info_get_f flush_info_get;
    
    dnx_data_l2_age_and_flush_machine_age_get_f age_get;
    
    dnxc_data_table_info_get_f age_info_get;
} dnx_data_if_l2_age_and_flush_machine_t;







typedef enum
{
    
    dnx_data_l2_olp_refresh_events_support,
    
    dnx_data_l2_olp_olp_learn_payload_initial_value_supported,
    
    dnx_data_l2_olp_dsp_messages_support,

    
    _dnx_data_l2_olp_feature_nof
} dnx_data_l2_olp_feature_e;



typedef int(
    *dnx_data_l2_olp_feature_get_f) (
    int unit,
    dnx_data_l2_olp_feature_e feature);


typedef uint32(
    *dnx_data_l2_olp_lpkgv_shift_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_lpkgv_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_lpkgv_with_outlif_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_lpkgv_wo_outlif_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_destination_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_outlif_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_eei_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_fec_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_jr_mode_enhanced_performance_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_olp_feature_get_f feature_get;
    
    dnx_data_l2_olp_lpkgv_shift_get_f lpkgv_shift_get;
    
    dnx_data_l2_olp_lpkgv_mask_get_f lpkgv_mask_get;
    
    dnx_data_l2_olp_lpkgv_with_outlif_get_f lpkgv_with_outlif_get;
    
    dnx_data_l2_olp_lpkgv_wo_outlif_get_f lpkgv_wo_outlif_get;
    
    dnx_data_l2_olp_destination_offset_get_f destination_offset_get;
    
    dnx_data_l2_olp_outlif_offset_get_f outlif_offset_get;
    
    dnx_data_l2_olp_eei_offset_get_f eei_offset_get;
    
    dnx_data_l2_olp_fec_offset_get_f fec_offset_get;
    
    dnx_data_l2_olp_jr_mode_enhanced_performance_enable_get_f jr_mode_enhanced_performance_enable_get;
} dnx_data_if_l2_olp_t;





typedef struct
{
    
    dnx_data_if_l2_general_t general;
    
    dnx_data_if_l2_feature_t feature;
    
    dnx_data_if_l2_vsi_t vsi;
    
    dnx_data_if_l2_vlan_domain_t vlan_domain;
    
    dnx_data_if_l2_dma_t dma;
    
    dnx_data_if_l2_age_and_flush_machine_t age_and_flush_machine;
    
    dnx_data_if_l2_olp_t olp;
} dnx_data_if_l2_t;




extern dnx_data_if_l2_t dnx_data_l2;


#endif 

