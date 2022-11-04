
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_L2_H_

#define _DNX_DATA_L2_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <bcm_int/dnx/l2/l2_addr.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_result_types.h>
#include <soc/dnx/dbal/dbal_external_defines.h>
#include <bcm_int/dnx/l2/l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l2.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_l2_init(
    int unit);






typedef struct
{
    uint32 dest_lif_stat;
    uint32 dest_stat;
    uint32 eei;
    uint32 no_outlif;
    uint32 single_outlif;
} dnx_data_l2_general_mact_result_type_map_t;


typedef struct
{
    dbal_tables_e age_scan;
    dbal_tables_e scan_cycles;
} dnx_data_l2_general_scan_data_bases_info_t;


typedef struct
{
    dbal_tables_e logical_data_base;
    dbal_physical_tables_e physical_data_base;
} dnx_data_l2_general_fwd_mact_info_t;


typedef struct
{
    dnx_l2_action_type_t l2_action_type;
} dnx_data_l2_general_lem_table_map_l2_action_t;



typedef enum
{
    dnx_data_l2_general_learning_use_insert_cmd,
    dnx_data_l2_general_flush_machine_support,
    dnx_data_l2_general_dynamic_entries_iteration_support,
    dnx_data_l2_general_counters_support,
    dnx_data_l2_general_aging_support,
    dnx_data_l2_general_transplant_over_stronger_support,
    dnx_data_l2_general_ivl_feature_support,
    dnx_data_l2_general_dma_support,
    dnx_data_l2_general_separate_fwd_learn_mact,
    dnx_data_l2_general_SLLB_v1_support,
    dnx_data_l2_general_enhanced_learn_key_generation,
    dnx_data_l2_general_full_learn_action_resolution_support,

    
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
    *dnx_data_l2_general_lem_nof_dbs_get_f) (
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


typedef uint32(
    *dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_mact_lela_fid_exceed_limit_int_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_api_supported_flags2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_traverse_api_supported_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_delete_api_supported_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_general_l2_add_get_apis_supported_flags_get_f) (
    int unit);


typedef const dnx_data_l2_general_mact_result_type_map_t *(
    *dnx_data_l2_general_mact_result_type_map_get_f) (
    int unit,
    int forward_learning_db,
    int vlan_learning_mode);


typedef const dnx_data_l2_general_scan_data_bases_info_t *(
    *dnx_data_l2_general_scan_data_bases_info_get_f) (
    int unit,
    int data_base);


typedef const dnx_data_l2_general_fwd_mact_info_t *(
    *dnx_data_l2_general_fwd_mact_info_get_f) (
    int unit,
    int l2_action_type,
    int is_ivl);


typedef const dnx_data_l2_general_lem_table_map_l2_action_t *(
    *dnx_data_l2_general_lem_table_map_l2_action_get_f) (
    int unit,
    int logical_data_base);



typedef struct
{
    
    dnx_data_l2_general_feature_get_f feature_get;
    
    dnx_data_l2_general_vsi_offset_shift_get_f vsi_offset_shift_get;
    
    dnx_data_l2_general_lif_offset_shift_get_f lif_offset_shift_get;
    
    dnx_data_l2_general_lem_nof_dbs_get_f lem_nof_dbs_get;
    
    dnx_data_l2_general_l2_learn_limit_mode_get_f l2_learn_limit_mode_get;
    
    dnx_data_l2_general_jr_mode_nof_fec_bits_get_f jr_mode_nof_fec_bits_get;
    
    dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get_f arad_plus_mode_nof_fec_bits_get;
    
    dnx_data_l2_general_l2_egress_max_extention_size_bytes_get_f l2_egress_max_extention_size_bytes_get;
    
    dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get_f l2_egress_max_additional_termination_size_bytes_get;
    
    dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_get_f mact_mngmnt_fid_exceed_limit_int_get;
    
    dnx_data_l2_general_mact_lela_fid_exceed_limit_int_get_f mact_lela_fid_exceed_limit_int_get;
    
    dnx_data_l2_general_l2_api_supported_flags2_get_f l2_api_supported_flags2_get;
    
    dnx_data_l2_general_l2_traverse_api_supported_flags_get_f l2_traverse_api_supported_flags_get;
    
    dnx_data_l2_general_l2_delete_api_supported_flags_get_f l2_delete_api_supported_flags_get;
    
    dnx_data_l2_general_l2_add_get_apis_supported_flags_get_f l2_add_get_apis_supported_flags_get;
    
    dnx_data_l2_general_mact_result_type_map_get_f mact_result_type_map_get;
    
    dnxc_data_table_info_get_f mact_result_type_map_info_get;
    
    dnx_data_l2_general_scan_data_bases_info_get_f scan_data_bases_info_get;
    
    dnxc_data_table_info_get_f scan_data_bases_info_info_get;
    
    dnx_data_l2_general_fwd_mact_info_get_f fwd_mact_info_get;
    
    dnxc_data_table_info_get_f fwd_mact_info_info_get;
    
    dnx_data_l2_general_lem_table_map_l2_action_get_f lem_table_map_l2_action_get;
    
    dnxc_data_table_info_get_f lem_table_map_l2_action_info_get;
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
    dnx_data_l2_feature_no_trap_for_unknown_destination,
    dnx_data_l2_feature_no_black_hole_for_unknown_destination,
    dnx_data_l2_feature_mact_access_by_opportunistic_learning,
    dnx_data_l2_feature_opportunistic_rejected,
    dnx_data_l2_feature_flush_drop_stuck,
    dnx_data_l2_feature_lif_flood_profile_always_update,
    dnx_data_l2_feature_olp_always_enters_source_cpu,
    dnx_data_l2_feature_learn_payload_compatible_jr1,
    dnx_data_l2_feature_unified_vsi_info_db,
    dnx_data_l2_feature_ingress_opportunistic_learning_support,
    dnx_data_l2_feature_age_refresh_mode_support,
    dnx_data_l2_feature_learn_payload_native_update_enable,
    dnx_data_l2_feature_accepted_data_with_vmv,

    
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


typedef uint32(
    *dnx_data_l2_vsi_vsi_table_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_vsi_feature_get_f feature_get;
    
    dnx_data_l2_vsi_nof_vsi_aging_profiles_get_f nof_vsi_aging_profiles_get;
    
    dnx_data_l2_vsi_nof_event_forwarding_profiles_get_f nof_event_forwarding_profiles_get;
    
    dnx_data_l2_vsi_nof_vsi_learning_profiles_get_f nof_vsi_learning_profiles_get;
    
    dnx_data_l2_vsi_vsi_table_get_f vsi_table_get;
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


typedef uint32(
    *dnx_data_l2_dma_l2_dma_cpu_learn_thread_priority_get_f) (
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
    
    dnx_data_l2_dma_l2_dma_cpu_learn_thread_priority_get_f l2_dma_cpu_learn_thread_priority_get;
} dnx_data_if_l2_dma_t;






typedef struct
{
    soc_reg_t flush_pulse_reg;
} dnx_data_l2_age_and_flush_machine_flush_pulse_t;


typedef struct
{
    soc_reg_t age_config;
    soc_field_t disable_aging;
    soc_reg_t scan_pulse;
} dnx_data_l2_age_and_flush_machine_age_t;



typedef enum
{
    dnx_data_l2_age_and_flush_machine_flush_init_enable,

    
    _dnx_data_l2_age_and_flush_machine_feature_nof
} dnx_data_l2_age_and_flush_machine_feature_e;



typedef int(
    *dnx_data_l2_age_and_flush_machine_feature_get_f) (
    int unit,
    dnx_data_l2_age_and_flush_machine_feature_e feature);


typedef uint32(
    *dnx_data_l2_age_and_flush_machine_max_age_states_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_age_and_flush_machine_flush_buffer_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_age_and_flush_machine_traverse_thread_priority_get_f) (
    int unit);


typedef const dnx_data_l2_age_and_flush_machine_flush_pulse_t *(
    *dnx_data_l2_age_and_flush_machine_flush_pulse_get_f) (
    int unit,
    int l2_action_type);


typedef const dnx_data_l2_age_and_flush_machine_age_t *(
    *dnx_data_l2_age_and_flush_machine_age_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_l2_age_and_flush_machine_feature_get_f feature_get;
    
    dnx_data_l2_age_and_flush_machine_max_age_states_get_f max_age_states_get;
    
    dnx_data_l2_age_and_flush_machine_flush_buffer_nof_entries_get_f flush_buffer_nof_entries_get;
    
    dnx_data_l2_age_and_flush_machine_traverse_thread_priority_get_f traverse_thread_priority_get;
    
    dnx_data_l2_age_and_flush_machine_flush_pulse_get_f flush_pulse_get;
    
    dnxc_data_table_info_get_f flush_pulse_info_get;
    
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


typedef uint32(
    *dnx_data_l2_olp_olp_learning_block_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_l2_olp_olp_fwd_block_id_get_f) (
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
    
    dnx_data_l2_olp_olp_learning_block_id_get_f olp_learning_block_id_get;
    
    dnx_data_l2_olp_olp_fwd_block_id_get_f olp_fwd_block_id_get;
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

