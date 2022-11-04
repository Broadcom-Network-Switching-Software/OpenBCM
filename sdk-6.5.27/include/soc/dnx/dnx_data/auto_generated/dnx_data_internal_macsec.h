
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MACSEC_H_

#define _DNX_DATA_INTERNAL_MACSEC_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_macsec_submodule_general,
    dnx_data_macsec_submodule_ingress,
    dnx_data_macsec_submodule_egress,
    dnx_data_macsec_submodule_wrapper,

    
    _dnx_data_macsec_submodule_nof
} dnx_data_macsec_submodule_e;








int dnx_data_macsec_general_feature_get(
    int unit,
    dnx_data_macsec_general_feature_e feature);



typedef enum
{
    dnx_data_macsec_general_define_macsec_nof,
    dnx_data_macsec_general_define_macsec_in_core_nof,
    dnx_data_macsec_general_define_ports_in_macsec_nof,
    dnx_data_macsec_general_define_etype_nof,
    dnx_data_macsec_general_define_etype_not_zero_verify,
    dnx_data_macsec_general_define_tx_threshold,
    dnx_data_macsec_general_define_macsec_tdm_cal_max_depth,
    dnx_data_macsec_general_define_macsec_tdm_cal_hw_depth,
    dnx_data_macsec_general_define_association_number_full_range_nof,
    dnx_data_macsec_general_define_macsec_gen,
    dnx_data_macsec_general_define_pfc_and_pause_timer_nof_bits,
    dnx_data_macsec_general_define_sectag_offset_min_value,
    dnx_data_macsec_general_define_supported_entries_nof,
    dnx_data_macsec_general_define_is_macsec_enabled,
    dnx_data_macsec_general_define_is_power_optimization_enabled,

    
    _dnx_data_macsec_general_define_nof
} dnx_data_macsec_general_define_e;



uint32 dnx_data_macsec_general_macsec_nof_get(
    int unit);


uint32 dnx_data_macsec_general_macsec_in_core_nof_get(
    int unit);


uint32 dnx_data_macsec_general_ports_in_macsec_nof_get(
    int unit);


uint32 dnx_data_macsec_general_etype_nof_get(
    int unit);


uint32 dnx_data_macsec_general_etype_not_zero_verify_get(
    int unit);


uint32 dnx_data_macsec_general_tx_threshold_get(
    int unit);


uint32 dnx_data_macsec_general_macsec_tdm_cal_max_depth_get(
    int unit);


uint32 dnx_data_macsec_general_macsec_tdm_cal_hw_depth_get(
    int unit);


uint32 dnx_data_macsec_general_association_number_full_range_nof_get(
    int unit);


uint32 dnx_data_macsec_general_macsec_gen_get(
    int unit);


uint32 dnx_data_macsec_general_pfc_and_pause_timer_nof_bits_get(
    int unit);


uint32 dnx_data_macsec_general_sectag_offset_min_value_get(
    int unit);


uint32 dnx_data_macsec_general_supported_entries_nof_get(
    int unit);


uint32 dnx_data_macsec_general_is_macsec_enabled_get(
    int unit);


uint32 dnx_data_macsec_general_is_power_optimization_enabled_get(
    int unit);



typedef enum
{
    dnx_data_macsec_general_table_pm_to_macsec,
    dnx_data_macsec_general_table_macsec_instances,
    dnx_data_macsec_general_table_macsec_block_mapping,
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec_general_table_macsec_control_options,

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec_general_table_stat_special_map,

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec_general_table_counter_map,

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 

    dnx_data_macsec_general_table_access_counter_map,

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 

    dnx_data_macsec_general_table_access_stat_map,

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec_general_table_stat_map,

#endif 

    
    _dnx_data_macsec_general_table_nof
} dnx_data_macsec_general_table_e;



const dnx_data_macsec_general_pm_to_macsec_t * dnx_data_macsec_general_pm_to_macsec_get(
    int unit,
    int ethu_index);


const dnx_data_macsec_general_macsec_instances_t * dnx_data_macsec_general_macsec_instances_get(
    int unit,
    int macsec_id);


const dnx_data_macsec_general_macsec_block_mapping_t * dnx_data_macsec_general_macsec_block_mapping_get(
    int unit,
    int is_mss,
    int block_id);

#ifdef INCLUDE_XFLOW_MACSEC


const dnx_data_macsec_general_macsec_control_options_t * dnx_data_macsec_general_macsec_control_options_get(
    int unit,
    int control_option);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


const dnx_data_macsec_general_stat_special_map_t * dnx_data_macsec_general_stat_special_map_get(
    int unit,
    int index);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


const dnx_data_macsec_general_counter_map_t * dnx_data_macsec_general_counter_map_get(
    int unit,
    int counter_id);

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


const dnx_data_macsec_general_access_counter_map_t * dnx_data_macsec_general_access_counter_map_get(
    int unit,
    int counter_id);

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


const dnx_data_macsec_general_access_stat_map_t * dnx_data_macsec_general_access_stat_map_get(
    int unit,
    int stat_type);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


const dnx_data_macsec_general_stat_map_t * dnx_data_macsec_general_stat_map_get(
    int unit,
    int stat_type);

#endif 


shr_error_e dnx_data_macsec_general_pm_to_macsec_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_macsec_general_macsec_instances_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_macsec_general_macsec_block_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#ifdef INCLUDE_XFLOW_MACSEC


shr_error_e dnx_data_macsec_general_macsec_control_options_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


shr_error_e dnx_data_macsec_general_stat_special_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


shr_error_e dnx_data_macsec_general_counter_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


shr_error_e dnx_data_macsec_general_access_counter_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


shr_error_e dnx_data_macsec_general_access_stat_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


shr_error_e dnx_data_macsec_general_stat_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

#endif 


const dnxc_data_table_info_t * dnx_data_macsec_general_pm_to_macsec_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_macsec_general_macsec_instances_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_macsec_general_macsec_block_mapping_info_get(
    int unit);

#ifdef INCLUDE_XFLOW_MACSEC


const dnxc_data_table_info_t * dnx_data_macsec_general_macsec_control_options_info_get(
    int unit);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


const dnxc_data_table_info_t * dnx_data_macsec_general_stat_special_map_info_get(
    int unit);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


const dnxc_data_table_info_t * dnx_data_macsec_general_counter_map_info_get(
    int unit);

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


const dnxc_data_table_info_t * dnx_data_macsec_general_access_counter_map_info_get(
    int unit);

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 


const dnxc_data_table_info_t * dnx_data_macsec_general_access_stat_map_info_get(
    int unit);

#endif 
#ifdef INCLUDE_XFLOW_MACSEC


const dnxc_data_table_info_t * dnx_data_macsec_general_stat_map_info_get(
    int unit);

#endif 





int dnx_data_macsec_ingress_feature_get(
    int unit,
    dnx_data_macsec_ingress_feature_e feature);



typedef enum
{
    dnx_data_macsec_ingress_define_flow_nof,
    dnx_data_macsec_ingress_define_policy_nof,
    dnx_data_macsec_ingress_define_secure_assoc_nof,
    dnx_data_macsec_ingress_define_udf_nof_bits,
    dnx_data_macsec_ingress_define_mgmt_rule_exact_nof,
    dnx_data_macsec_ingress_define_tpid_nof,
    dnx_data_macsec_ingress_define_cpu_flex_map_nof,
    dnx_data_macsec_ingress_define_sc_tcam_nof,
    dnx_data_macsec_ingress_define_sa_per_sc_nof,
    dnx_data_macsec_ingress_define_invalidate_sa,
    dnx_data_macsec_ingress_define_secure_channel_nof,

    
    _dnx_data_macsec_ingress_define_nof
} dnx_data_macsec_ingress_define_e;



uint32 dnx_data_macsec_ingress_flow_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_policy_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_secure_assoc_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_udf_nof_bits_get(
    int unit);


uint32 dnx_data_macsec_ingress_mgmt_rule_exact_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_tpid_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_cpu_flex_map_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_sc_tcam_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_sa_per_sc_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_invalidate_sa_get(
    int unit);


uint32 dnx_data_macsec_ingress_secure_channel_nof_get(
    int unit);



typedef enum
{
    dnx_data_macsec_ingress_table_udf,
    dnx_data_macsec_ingress_table_restricted_sc_tcam_id,
    dnx_data_macsec_ingress_table_restricted_sp_tcam_id,

    
    _dnx_data_macsec_ingress_table_nof
} dnx_data_macsec_ingress_table_e;



const dnx_data_macsec_ingress_udf_t * dnx_data_macsec_ingress_udf_get(
    int unit,
    int type);


const dnx_data_macsec_ingress_restricted_sc_tcam_id_t * dnx_data_macsec_ingress_restricted_sc_tcam_id_get(
    int unit,
    int key_index);


const dnx_data_macsec_ingress_restricted_sp_tcam_id_t * dnx_data_macsec_ingress_restricted_sp_tcam_id_get(
    int unit,
    int key_index);



shr_error_e dnx_data_macsec_ingress_udf_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_macsec_ingress_restricted_sc_tcam_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_macsec_ingress_restricted_sp_tcam_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_macsec_ingress_udf_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_macsec_ingress_restricted_sc_tcam_id_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_macsec_ingress_restricted_sp_tcam_id_info_get(
    int unit);






int dnx_data_macsec_egress_feature_get(
    int unit,
    dnx_data_macsec_egress_feature_e feature);



typedef enum
{
    dnx_data_macsec_egress_define_secure_assoc_nof,
    dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof,
    dnx_data_macsec_egress_define_an_bit_nof,
    dnx_data_macsec_egress_define_nh_profiles_nof,
    dnx_data_macsec_egress_define_association_number_actual_range_nof,
    dnx_data_macsec_egress_define_sa_per_sc_nof,
    dnx_data_macsec_egress_define_secure_channel_nof,

    
    _dnx_data_macsec_egress_define_nof
} dnx_data_macsec_egress_define_e;



uint32 dnx_data_macsec_egress_secure_assoc_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_an_bit_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_nh_profiles_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_association_number_actual_range_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_sa_per_sc_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_secure_channel_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_macsec_egress_table_nof
} dnx_data_macsec_egress_table_e;









int dnx_data_macsec_wrapper_feature_get(
    int unit,
    dnx_data_macsec_wrapper_feature_e feature);



typedef enum
{
    dnx_data_macsec_wrapper_define_macsec_arb_ports_nof,
    dnx_data_macsec_wrapper_define_per_port_speed_max_mbps,
    dnx_data_macsec_wrapper_define_wrapper_speed_max_mbps,
    dnx_data_macsec_wrapper_define_framer_ports_offset,
    dnx_data_macsec_wrapper_define_tdm_calendar_sbus_slot,
    dnx_data_macsec_wrapper_define_tdm_calendar_nof_slots_per_line,
    dnx_data_macsec_wrapper_define_tdm_calendar_speed_granularity,
    dnx_data_macsec_wrapper_define_nof_tags,

    
    _dnx_data_macsec_wrapper_define_nof
} dnx_data_macsec_wrapper_define_e;



uint32 dnx_data_macsec_wrapper_macsec_arb_ports_nof_get(
    int unit);


uint32 dnx_data_macsec_wrapper_per_port_speed_max_mbps_get(
    int unit);


uint32 dnx_data_macsec_wrapper_wrapper_speed_max_mbps_get(
    int unit);


uint32 dnx_data_macsec_wrapper_framer_ports_offset_get(
    int unit);


uint32 dnx_data_macsec_wrapper_tdm_calendar_sbus_slot_get(
    int unit);


uint32 dnx_data_macsec_wrapper_tdm_calendar_nof_slots_per_line_get(
    int unit);


uint32 dnx_data_macsec_wrapper_tdm_calendar_speed_granularity_get(
    int unit);


uint32 dnx_data_macsec_wrapper_nof_tags_get(
    int unit);



typedef enum
{

    
    _dnx_data_macsec_wrapper_table_nof
} dnx_data_macsec_wrapper_table_e;






shr_error_e dnx_data_macsec_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

