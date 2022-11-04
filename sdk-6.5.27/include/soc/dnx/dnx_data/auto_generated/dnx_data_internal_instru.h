
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_INSTRU_H_

#define _DNX_DATA_INTERNAL_INSTRU_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_instru_submodule_ipt,
    dnx_data_instru_submodule_sflow,
    dnx_data_instru_submodule_ifa,
    dnx_data_instru_submodule_ipfix,
    dnx_data_instru_submodule_hbh_pt,
    dnx_data_instru_submodule_eventor,
    dnx_data_instru_submodule_synced_triggers,
    dnx_data_instru_submodule_synced_counters,
    dnx_data_instru_submodule_alternate_marking,

    
    _dnx_data_instru_submodule_nof
} dnx_data_instru_submodule_e;








int dnx_data_instru_ipt_feature_get(
    int unit,
    dnx_data_instru_ipt_feature_e feature);



typedef enum
{
    dnx_data_instru_ipt_define_profile_size,
    dnx_data_instru_ipt_define_metadata_bitmap_size,
    dnx_data_instru_ipt_define_metadata_edit_size,
    dnx_data_instru_ipt_define_node_id_padding_size,
    dnx_data_instru_ipt_define_telemetry_queue_occ_check,
    dnx_data_instru_ipt_define_telemetry_queue_occ_max,

    
    _dnx_data_instru_ipt_define_nof
} dnx_data_instru_ipt_define_e;



uint32 dnx_data_instru_ipt_profile_size_get(
    int unit);


uint32 dnx_data_instru_ipt_metadata_bitmap_size_get(
    int unit);


uint32 dnx_data_instru_ipt_metadata_edit_size_get(
    int unit);


uint32 dnx_data_instru_ipt_node_id_padding_size_get(
    int unit);


uint32 dnx_data_instru_ipt_telemetry_queue_occ_check_get(
    int unit);


uint32 dnx_data_instru_ipt_telemetry_queue_occ_max_get(
    int unit);



typedef enum
{
    dnx_data_instru_ipt_table_profile_info,
    dnx_data_instru_ipt_table_metadata,

    
    _dnx_data_instru_ipt_table_nof
} dnx_data_instru_ipt_table_e;



const dnx_data_instru_ipt_profile_info_t * dnx_data_instru_ipt_profile_info_get(
    int unit,
    int profile_id);


const dnx_data_instru_ipt_metadata_t * dnx_data_instru_ipt_metadata_get(
    int unit,
    int flag_id);



shr_error_e dnx_data_instru_ipt_profile_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_instru_ipt_metadata_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_instru_ipt_profile_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_instru_ipt_metadata_info_get(
    int unit);






int dnx_data_instru_sflow_feature_get(
    int unit,
    dnx_data_instru_sflow_feature_e feature);



typedef enum
{
    dnx_data_instru_sflow_define_max_nof_sflow_encaps,
    dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack,

    
    _dnx_data_instru_sflow_define_nof
} dnx_data_instru_sflow_define_e;



uint32 dnx_data_instru_sflow_max_nof_sflow_encaps_get(
    int unit);


uint32 dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_sflow_table_nof
} dnx_data_instru_sflow_table_e;









int dnx_data_instru_ifa_feature_get(
    int unit,
    dnx_data_instru_ifa_feature_e feature);



typedef enum
{
    dnx_data_instru_ifa_define_ingress_tod_feature,
    dnx_data_instru_ifa_define_max_lenght_check_for_ifa2,
    dnx_data_instru_ifa_define_ifa2_is_in_hl,
    dnx_data_instru_ifa_define_ifa2_is_supported,

    
    _dnx_data_instru_ifa_define_nof
} dnx_data_instru_ifa_define_e;



uint32 dnx_data_instru_ifa_ingress_tod_feature_get(
    int unit);


uint32 dnx_data_instru_ifa_max_lenght_check_for_ifa2_get(
    int unit);


uint32 dnx_data_instru_ifa_ifa2_is_in_hl_get(
    int unit);


uint32 dnx_data_instru_ifa_ifa2_is_supported_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_ifa_table_nof
} dnx_data_instru_ifa_table_e;









int dnx_data_instru_ipfix_feature_get(
    int unit,
    dnx_data_instru_ipfix_feature_e feature);



typedef enum
{
    dnx_data_instru_ipfix_define_ipfix_v2,

    
    _dnx_data_instru_ipfix_define_nof
} dnx_data_instru_ipfix_define_e;



uint32 dnx_data_instru_ipfix_ipfix_v2_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_ipfix_table_nof
} dnx_data_instru_ipfix_table_e;









int dnx_data_instru_hbh_pt_feature_get(
    int unit,
    dnx_data_instru_hbh_pt_feature_e feature);



typedef enum
{
    dnx_data_instru_hbh_pt_define_Ingress_1588_ToD_LSB_size,

    
    _dnx_data_instru_hbh_pt_define_nof
} dnx_data_instru_hbh_pt_define_e;



uint32 dnx_data_instru_hbh_pt_Ingress_1588_ToD_LSB_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_hbh_pt_table_nof
} dnx_data_instru_hbh_pt_table_e;









int dnx_data_instru_eventor_feature_get(
    int unit,
    dnx_data_instru_eventor_feature_e feature);



typedef enum
{
    dnx_data_instru_eventor_define_rx_evpck_header_size,
    dnx_data_instru_eventor_define_tx_builder_max_header_size,
    dnx_data_instru_eventor_define_nof_contexts,
    dnx_data_instru_eventor_define_nof_contexts_bits,
    dnx_data_instru_eventor_define_nof_builders,
    dnx_data_instru_eventor_define_nof_builders_bits,
    dnx_data_instru_eventor_define_nof_sram_banks,
    dnx_data_instru_eventor_define_sram_bank_bits,
    dnx_data_instru_eventor_define_sram_banks_full_size,
    dnx_data_instru_eventor_define_sram_banks_net_size,
    dnx_data_instru_eventor_define_builder_max_buffer_size,
    dnx_data_instru_eventor_define_rx_max_buffer_size,
    dnx_data_instru_eventor_define_axi_sram_offset,
    dnx_data_instru_eventor_define_is_extra_words_copy,
    dnx_data_instru_eventor_define_is_rx_en,
    dnx_data_instru_eventor_define_tx_builder_limitation_1,
    dnx_data_instru_eventor_define_builder_max_timeout_ms,
    dnx_data_instru_eventor_define_sram_bank_mask,

    
    _dnx_data_instru_eventor_define_nof
} dnx_data_instru_eventor_define_e;



uint32 dnx_data_instru_eventor_rx_evpck_header_size_get(
    int unit);


uint32 dnx_data_instru_eventor_tx_builder_max_header_size_get(
    int unit);


uint32 dnx_data_instru_eventor_nof_contexts_get(
    int unit);


uint32 dnx_data_instru_eventor_nof_contexts_bits_get(
    int unit);


uint32 dnx_data_instru_eventor_nof_builders_get(
    int unit);


uint32 dnx_data_instru_eventor_nof_builders_bits_get(
    int unit);


uint32 dnx_data_instru_eventor_nof_sram_banks_get(
    int unit);


uint32 dnx_data_instru_eventor_sram_bank_bits_get(
    int unit);


uint32 dnx_data_instru_eventor_sram_banks_full_size_get(
    int unit);


uint32 dnx_data_instru_eventor_sram_banks_net_size_get(
    int unit);


uint32 dnx_data_instru_eventor_builder_max_buffer_size_get(
    int unit);


uint32 dnx_data_instru_eventor_rx_max_buffer_size_get(
    int unit);


uint32 dnx_data_instru_eventor_axi_sram_offset_get(
    int unit);


uint32 dnx_data_instru_eventor_is_extra_words_copy_get(
    int unit);


uint32 dnx_data_instru_eventor_is_rx_en_get(
    int unit);


uint32 dnx_data_instru_eventor_tx_builder_limitation_1_get(
    int unit);


uint32 dnx_data_instru_eventor_builder_max_timeout_ms_get(
    int unit);


uint32 dnx_data_instru_eventor_sram_bank_mask_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_eventor_table_nof
} dnx_data_instru_eventor_table_e;









int dnx_data_instru_synced_triggers_feature_get(
    int unit,
    dnx_data_instru_synced_triggers_feature_e feature);



typedef enum
{
    dnx_data_instru_synced_triggers_define_synced_trigger_version,

    
    _dnx_data_instru_synced_triggers_define_nof
} dnx_data_instru_synced_triggers_define_e;



uint32 dnx_data_instru_synced_triggers_synced_trigger_version_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_synced_triggers_table_nof
} dnx_data_instru_synced_triggers_table_e;









int dnx_data_instru_synced_counters_feature_get(
    int unit,
    dnx_data_instru_synced_counters_feature_e feature);



typedef enum
{
    dnx_data_instru_synced_counters_define_icgm_type,
    dnx_data_instru_synced_counters_define_nif_type,
    dnx_data_instru_synced_counters_define_nof_types,
    dnx_data_instru_synced_counters_define_period_adjusted_to_ns_through_time_ratio,
    dnx_data_instru_synced_counters_define_period_adjusting_clk_bits,
    dnx_data_instru_synced_counters_define_period_adjusting_ns_bits,
    dnx_data_instru_synced_counters_define_interval_period_size_hw,
    dnx_data_instru_synced_counters_define_max_interval_period_size,
    dnx_data_instru_synced_counters_define_cgm_max_nof_intervals,
    dnx_data_instru_synced_counters_define_nif_max_nof_intervals,
    dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around,
    dnx_data_instru_synced_counters_define_nif_write_counter_wrap_around,
    dnx_data_instru_synced_counters_define_bytes_in_word,
    dnx_data_instru_synced_counters_define_nof_pms_for_pfc,
    dnx_data_instru_synced_counters_define_nof_ports_for_pfc,
    dnx_data_instru_synced_counters_define_nof_tcs_per_port,

    
    _dnx_data_instru_synced_counters_define_nof
} dnx_data_instru_synced_counters_define_e;



uint32 dnx_data_instru_synced_counters_icgm_type_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nif_type_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nof_types_get(
    int unit);


uint32 dnx_data_instru_synced_counters_period_adjusted_to_ns_through_time_ratio_get(
    int unit);


uint32 dnx_data_instru_synced_counters_period_adjusting_clk_bits_get(
    int unit);


uint32 dnx_data_instru_synced_counters_period_adjusting_ns_bits_get(
    int unit);


uint32 dnx_data_instru_synced_counters_interval_period_size_hw_get(
    int unit);


uint32 dnx_data_instru_synced_counters_max_interval_period_size_get(
    int unit);


uint32 dnx_data_instru_synced_counters_cgm_max_nof_intervals_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nif_max_nof_intervals_get(
    int unit);


uint32 dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nif_write_counter_wrap_around_get(
    int unit);


uint32 dnx_data_instru_synced_counters_bytes_in_word_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nof_pms_for_pfc_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nof_ports_for_pfc_get(
    int unit);


uint32 dnx_data_instru_synced_counters_nof_tcs_per_port_get(
    int unit);



typedef enum
{
    dnx_data_instru_synced_counters_table_counters_info,

    
    _dnx_data_instru_synced_counters_table_nof
} dnx_data_instru_synced_counters_table_e;



const dnx_data_instru_synced_counters_counters_info_t * dnx_data_instru_synced_counters_counters_info_get(
    int unit,
    int entry_index,
    int source_type);



shr_error_e dnx_data_instru_synced_counters_counters_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_instru_synced_counters_counters_info_info_get(
    int unit);






int dnx_data_instru_alternate_marking_feature_get(
    int unit,
    dnx_data_instru_alternate_marking_feature_e feature);



typedef enum
{
    dnx_data_instru_alternate_marking_define_nof_L_bit_period_profile,

    
    _dnx_data_instru_alternate_marking_define_nof
} dnx_data_instru_alternate_marking_define_e;



uint32 dnx_data_instru_alternate_marking_nof_L_bit_period_profile_get(
    int unit);



typedef enum
{

    
    _dnx_data_instru_alternate_marking_table_nof
} dnx_data_instru_alternate_marking_table_e;






shr_error_e dnx_data_instru_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

